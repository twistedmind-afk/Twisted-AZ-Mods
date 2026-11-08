/* SPDX-License-Identifier: GPL-3.0-only
 * Compatibility for one identified Core build's Compact Track Info layout.
 * Claim only the native EP147 setBounds function through the SDK. Never patch
 * Core code, modify its cache/configuration, or force a Left/Center preference.
 */
#define _GNU_SOURCE
#include "azmod-mod.h"
#include "mod-id.h"
#include <dlfcn.h>
#include <elf.h>
#include <stdint.h>
#include <string.h>

typedef void (*bounds_fn)(void *,int,int,int,int);
static bounds_fn bounds_original;
static const az_core_v1 *api;
static uintptr_t affected_core;
static unsigned corrected;
static const unsigned char affected_build_id[20]={
 0x0a,0x4e,0x88,0x38,0x9d,0x15,0x26,0x64,0x20,0x76,
 0x6f,0x0c,0xf1,0x06,0x4b,0x27,0xef,0x45,0x65,0x9e
};

static int readable_note(const Elf64_Phdr *ph,unsigned count,
                         uintptr_t start,uintptr_t length) {
 if(!length || length>4096 || start+length<start)return 0;
 for(unsigned i=0;i<count;i++)
  if(ph[i].p_type==PT_LOAD && (ph[i].p_flags&PF_R) &&
     start>=ph[i].p_vaddr && start+length<=ph[i].p_vaddr+ph[i].p_memsz)return 1;
 return 0;
}

static int known_core(const void *image) {
 const unsigned char *base=image;
 const Elf64_Ehdr *eh=image;
 if(memcmp(eh->e_ident,ELFMAG,SELFMAG) || eh->e_ident[EI_CLASS]!=ELFCLASS64 ||
    eh->e_ident[EI_DATA]!=ELFDATA2LSB || eh->e_machine!=EM_AARCH64 ||
    eh->e_phentsize!=sizeof(Elf64_Phdr) || !eh->e_phnum || eh->e_phnum>32 ||
    eh->e_phoff>4096 || eh->e_phoff+eh->e_phnum*sizeof(Elf64_Phdr)>4096)return 0;
 const Elf64_Phdr *ph=(const Elf64_Phdr *)(base+eh->e_phoff);
 for(unsigned i=0;i<eh->e_phnum;i++) {
  if(ph[i].p_type!=PT_NOTE || !readable_note(ph,eh->e_phnum,ph[i].p_vaddr,ph[i].p_filesz))continue;
  const unsigned char *p=base+ph[i].p_vaddr,*end=p+ph[i].p_filesz;
  while((size_t)(end-p)>=sizeof(Elf64_Nhdr)) {
   Elf64_Nhdr n;memcpy(&n,p,sizeof n);p+=sizeof n;
   if(n.n_namesz>64 || n.n_descsz>64)break;
   size_t names=(n.n_namesz+3u)&~3u,desc=(n.n_descsz+3u)&~3u;
   if(names+desc>(size_t)(end-p))break;
   if(n.n_type==NT_GNU_BUILD_ID && n.n_namesz==4 && !memcmp(p,"GNU",4) &&
      n.n_descsz==sizeof affected_build_id &&
      !memcmp(p+names,affected_build_id,sizeof affected_build_id))return 1;
   p+=names+desc;
  }
 }
 return 0;
}

static int layout_caller(uintptr_t caller) {
 if(caller<affected_core)return 0;
 uintptr_t offset=caller-affected_core;
 /* Ranges from the byte-identical official rebuild. pl_bounds tail-branches
  * to EP147, so LR identifies its caller, pl_lane or pl_restore. */
 return (offset>=0x27e88 && offset<0x283ac) ||
        (offset>=0x27d38 && offset<0x27dbc);
}

static void marker_bounds(void *component,int x,int y,int width,int height) {
 if(component && *(const uintptr_t *)component==0x2ee0728 &&
    layout_caller((uintptr_t)__builtin_return_address(0))) {
  int live_x,live_y,live_width;
  memcpy(&live_x,(const unsigned char *)component+0x20,sizeof live_x);
  memcpy(&live_y,(const unsigned char *)component+0x24,sizeof live_y);
  memcpy(&live_width,(const unsigned char *)component+0x28,sizeof live_width);
  if((x!=live_x || y!=live_y || width!=live_width) &&
     !__atomic_load_n(&corrected,__ATOMIC_RELAXED))
   __atomic_store_n(&corrected,1,__ATOMIC_RELEASE);
  x=live_x;y=live_y;width=live_width;
 }
 bounds_original(component,x,y,width,height);
}

void layout_fix_init(const az_core_v1 *core) {
 api=core;
 Dl_info info;
 if(!dladdr((const void *)core->log,&info) || !info.dli_fbase || !known_core(info.dli_fbase)) {
  core->log(MOD_ID,"Playhead compatibility: Core is not the identified affected build; native geometry unchanged");
  return;
 }
 uint64_t hash=UINT64_C(0xcbf29ce484222325);
 const unsigned char *code=(const unsigned char *)(uintptr_t)0x260c480;
 for(unsigned i=0;i<32;i++)hash=(hash^code[i])*UINT64_C(0x100000001b3);
 if(hash!=UINT64_C(0xe1c02ec67287e803)) {
  core->log(MOD_ID,"Playhead compatibility: unsupported native setBounds; use documented workaround");
  return;
 }
 affected_core=(uintptr_t)info.dli_fbase;
 int rc=core->claim_hook(MOD_ID,0x260c480,(void *)marker_bounds,(void **)&bounds_original);
 if(rc) {
  affected_core=0;
  core->log(MOD_ID,"Playhead compatibility: native hook unavailable (%d); use documented workaround",rc);
 } else core->log(MOD_ID,"Playhead compatibility active: preserve native Left/Center geometry during Compact Track Info resizing");
}

void layout_fix_tick(void) {
 unsigned pending=1;
 if(__atomic_compare_exchange_n(&corrected,&pending,2,0,__ATOMIC_ACQUIRE,__ATOMIC_RELAXED))
  api->log(MOD_ID,"Playhead compatibility prevented a stale layout position/width");
}
