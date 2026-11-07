/* SPDX-License-Identifier: GPL-3.0-only
 * LINK Plus p01 experimental prototype. Exact firmware only.
 * Native LINK setting and network SystemManager remain On. Regular function hooks: selected local-role
 * listeners receive the ordinary internal-deck mode; factory count becomes four.
 * A validated four-role task enables the native allocator event-3 early return.
 * No direct object/code/setting writes, no fake packets, no network in callbacks,
 * no Core/watchdog changes. Initial state must be Off before loading this variant.
 */
#include "azmod-mod.h"
#include "firmware-check.h"
#include <stdint.h>
#include <string.h>
#include "mod-id.h"
#define ID MOD_ID
typedef void (*listener_fn)(void *,void *,const int *);
typedef void (*count_fn)(void *,unsigned);
typedef void (*mode_fn)(void *,unsigned char);
typedef void (*run_fn)(void *);
typedef unsigned (*packet_fn)(void *,unsigned,const void *,const void *);
static const az_core_v1 *api;
static unsigned ready;
static uintptr_t validated_owner;
static unsigned native_on;
static unsigned public_on(void){return __atomic_load_n(&native_on,__ATOMIC_ACQUIRE);}
struct observation {unsigned state,kind,input,output,entries;uintptr_t owner,caller;};
static struct observation observations[64];
static unsigned cursor,dropped,events[4];
static int identity(const void *p,uintptr_t vt) {return p && *(const uintptr_t *)p==vt;}
static void note(unsigned kind,void *owner,uintptr_t caller,unsigned input,unsigned output,unsigned entries) {
 unsigned n=__atomic_fetch_add(&cursor,1,__ATOMIC_RELAXED)%64;
 struct observation *r=&observations[n];unsigned empty=0;
 if(!__atomic_compare_exchange_n(&r->state,&empty,2,0,__ATOMIC_ACQUIRE,__ATOMIC_RELAXED)) {
  __atomic_fetch_add(&dropped,1,__ATOMIC_RELAXED);return;
 }
 r->kind=kind;r->owner=(uintptr_t)owner;r->caller=caller;r->input=input;r->output=output;r->entries=entries;
 __atomic_store_n(&r->state,1,__ATOMIC_RELEASE);
}
/* usecase::sync::BeatInfo; this pointer is its exact setting-listener subobject. */
static listener_fn original_0;
static void local_0(void *self,void *item,const int *value) {
 const int *use=value;
 note(0,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_0(self,item,use);
}
/* usecase::load::LoadController; this pointer is its exact setting-listener subobject. */
static listener_fn original_1;
static void local_1(void *self,void *item,const int *value) {
 const int *use=value;
 note(1,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_1(self,item,use);
}
/* usecase::load::InstantDoublesLoader; this pointer is its exact setting-listener subobject. */
static listener_fn original_2;
static void local_2(void *self,void *item,const int *value) {
 const int *use=value;
 note(2,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_2(self,item,use);
}
/* usecase::sync::SyncMemberSystem; this pointer is its exact setting-listener subobject. */
static listener_fn original_3;
static void local_3(void *self,void *item,const int *value) {
 const int *use=value;
 note(3,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_3(self,item,use);
}
/* gui::LoadMenuWidgetController; this pointer is its exact setting-listener subobject. */
static listener_fn original_4;
static void local_4(void *self,void *item,const int *value) {
 const int *use=value;
 note(4,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_4(self,item,use);
}
/* gui::PlayerSelectWidgetController; this pointer is its exact setting-listener subobject. */
static listener_fn original_5;
static void local_5(void *self,void *item,const int *value) {
 const int *use=value;
 note(5,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_5(self,item,use);
}
/* gui::browser_logic::ApplicationSettingUpdateFacade; this pointer is its exact setting-listener subobject. */
static listener_fn original_6;
static void local_6(void *self,void *item,const int *value) {
 const int *use=value;
 note(6,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_6(self,item,use);
}
/* gui::DeckStateInfoManager; this pointer is its exact setting-listener subobject. */
static listener_fn original_7;
static void local_7(void *self,void *item,const int *value) {
 const int *use=value;
 note(7,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_7(self,item,use);
}
/* gui::UtilitySetting; this pointer is its exact setting-listener subobject. */
static listener_fn original_8;
static void local_8(void *self,void *item,const int *value) {
 const int *use=value;
 note(8,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_8(self,item,use);
}
/* hui::deck::DeckSelector; this pointer is its exact setting-listener subobject. */
static listener_fn original_9;
static void local_9(void *self,void *item,const int *value) {
 const int *use=value;
 note(9,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_9(self,item,use);
 /* The independent LOAD mod captures the queued native setting task. */
}
/* PlayerLinkInfoCrawler: retain its native four-player collection policy.
 * This is its ordinary setting method, not the network mode setter. */
static listener_fn crawler_original;
static void crawler_mode(void *self,void *item,const int *value) {
 const int *use=value;
 note(10,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 crawler_original(self,item,use);
}
/* Only the native LINK PLAYER caution listener receives internal mode. */
static listener_fn caution_original;
static void caution_mode(void *self,void *item,const int *value) {
 const int *use=value;
 note(11,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 caution_original(self,item,use);
}
static count_fn count_original;
static mode_fn mixer_original,network_original;
static run_fn task_original;
static packet_fn packet_original;
static void player_count(void *owner,unsigned count) {
 uintptr_t caller=(uintptr_t)__builtin_return_address(0);unsigned use=count;
 if(__atomic_load_n(&ready,__ATOMIC_ACQUIRE) && (count==2 || count==4) && caller==0x13e4a28 &&
    identity(owner,0x2bbf270) && ((const unsigned char *)owner)[0x400]==0x64) use=public_on()?2:4;
 note(100,owner,caller,count,use,0);
 count_original(owner,use);
}
static void mixer_mode(void *owner,unsigned char mode) {
 uintptr_t caller=(uintptr_t)__builtin_return_address(0);unsigned char use=mode;
 note(101,owner,caller,mode,use,0);
 mixer_original(owner,use);
}
static void network_mode(void *owner,unsigned char mode) {
 uintptr_t caller=(uintptr_t)__builtin_return_address(0);
 if(!__atomic_load_n(&ready,__ATOMIC_ACQUIRE) || mode>1 || caller!=0xb61608 || !identity(owner,0x2bc4c38)) {
  network_original(owner,mode);return;
 }
 __atomic_store_n(&native_on,(unsigned)mode,__ATOMIC_RELEASE);
 __atomic_store_n(&validated_owner,0,__ATOMIC_RELEASE);
 note(102,owner,caller,mode,1,0);
 network_original(owner,1); /* Network stays enabled; public setting is unchanged. */
 /* Native queued role update is required even when network mode stays On.
  * Use the current validated owner, never a retained object from tick. */
 void *roles=*(void **)((unsigned char *)owner+0x4b0);
 if(identity(roles,0x2bbf270)) {
  mode_fn change_roles=(mode_fn)(uintptr_t)0x13de6e8;
  change_roles(roles,mode);
 }
}
static void player_task(void *task) {
 void *owner=0;unsigned mode=99,count=0xffffffffu;
 if(identity(task,0x2bbf550)) {owner=*(void **)((unsigned char *)task+0x18);mode=((const unsigned char *)task)[0x20];}
 __atomic_store_n(&validated_owner,0,__ATOMIC_RELEASE);
 task_original(task);
 if(identity(owner,0x2bbf270)) {
  const unsigned char *p=owner;
  uintptr_t begin=*(const uintptr_t *)(p+0x398),end=*(const uintptr_t *)(p+0x3a0);
  if(end>=begin && (end-begin)%0x50==0 && (end-begin)/0x50<=8) count=(unsigned)((end-begin)/0x50);
  unsigned players=public_on()?2:4;
  if(__atomic_load_n(&ready,__ATOMIC_ACQUIRE) && p[0x400]==0x64 && begin && count==players+1) {
   const unsigned char *roles=(const unsigned char *)begin;
   unsigned valid=1;
   for(unsigned i=0;i<players;++i) if(roles[i*0x50]!=1 || roles[i*0x50+1]!=i+1) valid=0;
   if(roles[players*0x50]!=2 || roles[players*0x50+1]!=33) valid=0;
   if(valid) __atomic_store_n(&validated_owner,(uintptr_t)owner,__ATOMIC_RELEASE);
  }
 }
 note(103,owner,(uintptr_t)__builtin_return_address(0),mode,mode,count);
}
/* No role-vector access here: player_task publishes its validation on the native
 * thread after the factory completes. Event 3 already has a native return-5 path. */
static unsigned operate_packet(void *self,unsigned event,const void *ip,const void *wlan) {
 if(__atomic_load_n(&ready,__ATOMIC_ACQUIRE) && (event&0xffu)==3 &&
    !public_on() && identity(self,0x2bc1d78)) {
  void *owner=*(void *const *)((const unsigned char *)self+0x10);
  if((uintptr_t)owner==__atomic_load_n(&validated_owner,__ATOMIC_ACQUIRE) &&
     identity(owner,0x2bbf270) && ((const unsigned char *)owner)[0x400]==0x64) {
   note(104,owner,(uintptr_t)__builtin_return_address(0),event,5,5);
   return 5;
  }
 }
 return packet_original(self,event,ip,wlan);
}
/* Observers call their original exactly once. Native buffers are read only
 * before returning to their owner, never retained or used from tick. */
typedef void (*collect_fn)(void *,unsigned char);
typedef void (*track_fn)(void *,void *,unsigned,unsigned char);
typedef void (*link_up_fn)(void *,const void *,const void *);
static collect_fn collect_original;
static track_fn track_original;
static link_up_fn sender_original;
static unsigned collect_calls[4],track_seen[4];
static uint64_t track_last[4];
static unsigned read32(const unsigned char *p) {unsigned v;memcpy(&v,p,4);return v;}
static unsigned read16(const unsigned char *p) {uint16_t v;memcpy(&v,p,2);return v;}
static void collect_observer(void *self,unsigned char native_ready) {
 collect_original(self,native_ready);
 if(__atomic_load_n(&ready,__ATOMIC_ACQUIRE) && identity(self,0x2924a10)) {
  unsigned deck=read32((const unsigned char *)self+0x1c);
  if(deck<4) {
   unsigned n=__atomic_fetch_add(&collect_calls[deck],1,__ATOMIC_RELAXED);
   if(!n) note(120+deck,self,(uintptr_t)__builtin_return_address(0),native_ready,0,n+1);
  }
 }
}
static void track_observer(void *self,void *buffer,unsigned flags,unsigned char option) {
 track_original(self,buffer,flags,option);
 if(__atomic_load_n(&ready,__ATOMIC_ACQUIRE) && identity(self,0x2924a10) && buffer) {
  unsigned deck=read32((const unsigned char *)self+0x1c);
  if(deck<4) {
   const unsigned char *p=buffer;
   unsigned id=read32(p+8),index=read16(p+0x10),source=read32(p+0x30);
   unsigned mask=(p[0]!=0)|((!!(p[4]&31))<<1)|((p[6]!=0)<<2)|((id!=0)<<3)|((index!=0xffff)<<4)|((source!=0xffffffffu)<<5);
   uint64_t key=((uint64_t)mask<<32)|id;
   uint64_t previous=__atomic_exchange_n(&track_last[deck],key,__ATOMIC_RELAXED);
   unsigned seen=__atomic_exchange_n(&track_seen[deck],1,__ATOMIC_RELAXED);
   if(!seen || previous!=key) {
    note(130+deck,self,(uintptr_t)__builtin_return_address(0),mask,id,((unsigned)p[0]<<16)|((unsigned)p[4]<<8)|p[6]);
    note(140+deck,self,(uintptr_t)__builtin_return_address(0),index,source,flags);
   }
  }
 }
}
static void sender_observer(void *self,const void *member,const void *ip) {
 if(__atomic_load_n(&ready,__ATOMIC_ACQUIRE) && identity(self,0x2b71280) && member) {
  const unsigned char *p=self;unsigned deck=p[0x139];
  if(deck<4) {
   note(150+deck,self,(uintptr_t)__builtin_return_address(0),read32(member),0,deck);
  }
 }
 sender_original(self,member,ip);
}
/* The original transmitter already constructs four deck-numbered headers.
 * Its private +0x20 policy only gates indices 2/3 in these two send methods.
 * Use its ordinary native setter on the current live object/native send thread.
 * No captured object is replayed from tick; global LINK remains On. */
typedef void (*transmit_fn)(void *,const void *,const void *,unsigned char);
static transmit_fn full_original,condensed_original;
static void retain_four_transmitters(void *self,unsigned char index,uintptr_t caller) {
 if(__atomic_load_n(&ready,__ATOMIC_ACQUIRE) && index<4 &&
    (caller==0x10e2538 || caller==0x10e23cc) &&
    __atomic_load_n(&validated_owner,__ATOMIC_ACQUIRE) && identity(self,0x2bbdd18)) {
  unsigned mode=public_on()?1:0;
  unsigned before=((const unsigned char *)self)[0x20];
  if(before<=1 && before!=mode) {
   mode_fn set_native_policy=(mode_fn)(uintptr_t)0x13d1290;
   set_native_policy(self,(unsigned char)mode);
   note(105,self,caller,before,mode,index);
  }
 }
}
static void transmit_full(void *self,const void *data,const void *destination,unsigned char index) {
 retain_four_transmitters(self,index,(uintptr_t)__builtin_return_address(0));
 full_original(self,data,destination,index);
}
static void transmit_condensed(void *self,const void *data,const void *destination,unsigned char index) {
 retain_four_transmitters(self,index,(uintptr_t)__builtin_return_address(0));
 condensed_original(self,data,destination,index);
}
void link_deck_event(int deck,int event,void *user) {
 (void)user;if(deck>=0 && deck<4 && event==AZ_DECK_LOADED) __atomic_fetch_add(&events[deck],1,__ATOMIC_RELAXED);
}
void link_tick(void) {
 static unsigned ticks;
 if(++ticks%100==0) api->log(ID,"collect_calls=%u,%u,%u,%u",__atomic_load_n(&collect_calls[0],__ATOMIC_RELAXED),__atomic_load_n(&collect_calls[1],__ATOMIC_RELAXED),__atomic_load_n(&collect_calls[2],__ATOMIC_RELAXED),__atomic_load_n(&collect_calls[3],__ATOMIC_RELAXED));
 for(unsigned i=0;i<64;++i) {
  struct observation *r=&observations[i];unsigned full=1;
  if(!__atomic_compare_exchange_n(&r->state,&full,2,0,__ATOMIC_ACQUIRE,__ATOMIC_RELAXED)) continue;
  api->log(ID,"kind=%u owner=%lx caller=%lx value=%u->%u native_entries=%u",r->kind,(unsigned long)r->owner,(unsigned long)r->caller,r->input,r->output,r->entries);
  __atomic_store_n(&r->state,0,__ATOMIC_RELEASE);
 }
 static unsigned seen[4];
 for(unsigned d=0;d<4;++d) {unsigned n=__atomic_load_n(&events[d],__ATOMIC_RELAXED);if(n!=seen[d]) {api->log(ID,"LOADED deck=%u count=%u",d+1,n);seen[d]=n;}}
 unsigned lost=__atomic_exchange_n(&dropped,0,__ATOMIC_RELAXED);if(lost) api->log(ID,"telemetry_dropped=%u",lost);
}
int link_prepare(const az_core_v1 *core) {
 api=core;
 if(!AZ_HAS(core,claim_hook)||!AZ_HAS(core,on_deck_event)||!AZ_HAS(core,fw_tag)||!AZ_HAS(core,log))return 1;
 const char *tag=core->fw_tag();if(!tag||strcmp(tag,"ep147-9e9454e357389ece"))return 2;
 if(*(const uintptr_t *)0x29ab6f8!=0x93d2f8)return 10;
 if(*(const uintptr_t *)0x29f2908!=0xa79300)return 11;
 if(*(const uintptr_t *)0x2a03f68!=0xaac810)return 12;
 if(*(const uintptr_t *)0x2a9abb0!=0xc9c350)return 13;
 if(*(const uintptr_t *)0x2ed7c98!=0x1cd97b0)return 14;
 if(*(const uintptr_t *)0x3083540!=0x1eb7d50)return 15;
 if(*(const uintptr_t *)0x3167f78!=0x2037210)return 16;
 if(*(const uintptr_t *)0x3188f78!=0x20f1710)return 17;
 if(*(const uintptr_t *)0x31a0668!=0x21321e8)return 18;
 if(*(const uintptr_t *)0x31b9df0!=0x21a7a18)return 19;
 
 if(!firmware_matches(0x13e4498, 16, UINT64_C(0x30a4370fe069acc0)))return 30;
 
 if(!firmware_matches(0x1409530, 16, UINT64_C(0x2295da7ca67ac59e)))return 31;
 if(!firmware_matches(0x13de6e8,16,UINT64_C(0xe5027cd16455c13d)))return 125;
 if(*(const uintptr_t *)0x31f5758!=0x22a19d0 || *(const uintptr_t *)0x2bbf560!=0x13e49b0)return 32;
 
 if(!firmware_matches(0x93d2f8, 16, UINT64_C(0x0c215ebbcbd00403)))return 100;
 
 if(!firmware_matches(0xa78fc0, 16, UINT64_C(0x4bb7e13c33962bcc)))return 101;
 
 if(!firmware_matches(0xaac4d0, 16, UINT64_C(0x4bb7e13c33962bcc)))return 102;
 
 if(!firmware_matches(0xc9c048, 16, UINT64_C(0x58d7a3720a2df7bb)))return 103;
 
 if(!firmware_matches(0x1cd93f0, 16, UINT64_C(0x7cbd263e2da90a63)))return 104;
 
 if(!firmware_matches(0x1eb78f8, 16, UINT64_C(0x207d3960ddbb3818)))return 105;
 
 if(!firmware_matches(0x2036f10, 16, UINT64_C(0x22e65c72894c3761)))return 106;
 
 if(!firmware_matches(0x20f1410, 16, UINT64_C(0x22e65c72894c3761)))return 107;
 
 if(!firmware_matches(0x2131ee0, 16, UINT64_C(0x22e65c72894c3761)))return 108;
 
 if(!firmware_matches(0x21a7720, 16, UINT64_C(0x7cbd263e2da90a63)))return 109;
 
 if(!firmware_matches(0x22a19d0, 16, UINT64_C(0x4f7be7e4746d3068)))return 110;
 
 if(!firmware_matches(0x13e49b0, 16, UINT64_C(0xf770407abe90ba91)))return 111;
 
 if(!firmware_matches(0x13e4498, 16, UINT64_C(0x30a4370fe069acc0)))return 112;
 
 if(!firmware_matches(0x1409530, 16, UINT64_C(0x2295da7ca67ac59e)))return 113;
 
 if(!firmware_matches(0x79fc40, 16, UINT64_C(0x58d7a3720a2df7bb)))return 114;
 
 if(!firmware_matches(0x1a81828, 16, UINT64_C(0x22e65c72894c3761)))return 115;
 
 if(!firmware_matches(0x13f22b8, 16, UINT64_C(0x35c79183ff03c1b4)))return 116;
 if(*(const uintptr_t *)0x2d165d8!=0x1a81828)return 117;
 if(*(const uintptr_t *)0x2bc1db0!=0x13f22b8)return 118;
 
 if(!firmware_matches(0x798fc8, 16, UINT64_C(0xf425f04113c2fe28)))return 119;
 
 if(!firmware_matches(0x796b48, 16, UINT64_C(0xb23b313f975c060d)))return 120;
 
 if(!firmware_matches(0x10e0c18, 16, UINT64_C(0x4cfdedb7c5d8a1bd)))return 121;
 
 if(!firmware_matches(0x13d2360, 16, UINT64_C(0x71111f3178bab096)))return 122;
 
 if(!firmware_matches(0x13d37b8, 16, UINT64_C(0xb2dfaa03db57fb65)))return 123;
 
 if(!firmware_matches(0x13d1290, 16, UINT64_C(0x3cd320c06a51a42f)))return 124;
 return 0;
}
int link_install(const az_core_v1 *core) {
 core->log(ID,"LINK Plus p02: public Off=four; public On=original 2+2; restore native transmitter policy in both directions");
 if(core->claim_hook(ID,0x93d2f8,(void *)local_0,(void **)&original_0))return 40;
 if(core->claim_hook(ID,0xa78fc0,(void *)local_1,(void **)&original_1))return 41;
 if(core->claim_hook(ID,0xaac4d0,(void *)local_2,(void **)&original_2))return 42;
 if(core->claim_hook(ID,0xc9c048,(void *)local_3,(void **)&original_3))return 43;
 if(core->claim_hook(ID,0x1cd93f0,(void *)local_4,(void **)&original_4))return 44;
 if(core->claim_hook(ID,0x1eb78f8,(void *)local_5,(void **)&original_5))return 45;
 if(core->claim_hook(ID,0x2036f10,(void *)local_6,(void **)&original_6))return 46;
 if(core->claim_hook(ID,0x20f1410,(void *)local_7,(void **)&original_7))return 47;
 if(core->claim_hook(ID,0x2131ee0,(void *)local_8,(void **)&original_8))return 48;
 if(core->claim_hook(ID,0x21a7720,(void *)local_9,(void **)&original_9))return 49;
 if(core->claim_hook(ID,0x22a19d0,(void *)mixer_mode,(void **)&mixer_original))return 60;
 if(core->claim_hook(ID,0x13e49b0,(void *)player_task,(void **)&task_original))return 61;
 if(core->claim_hook(ID,0x13e4498,(void *)player_count,(void **)&count_original))return 62;
 if(core->claim_hook(ID,0x1409530,(void *)network_mode,(void **)&network_original))return 63;
 if(core->claim_hook(ID,0x79fc40,(void *)crawler_mode,(void **)&crawler_original))return 64;
 if(core->claim_hook(ID,0x1a81828,(void *)caution_mode,(void **)&caution_original))return 65;
 if(core->claim_hook(ID,0x13f22b8,(void *)operate_packet,(void **)&packet_original))return 66;
 if(core->claim_hook(ID,0x798fc8,(void *)collect_observer,(void **)&collect_original))return 67;
 if(core->claim_hook(ID,0x796b48,(void *)track_observer,(void **)&track_original))return 68;
 if(core->claim_hook(ID,0x10e0c18,(void *)sender_observer,(void **)&sender_original))return 69;
 if(core->claim_hook(ID,0x13d2360,(void *)transmit_full,(void **)&full_original))return 70;
 if(core->claim_hook(ID,0x13d37b8,(void *)transmit_condensed,(void **)&condensed_original))return 71;
 __atomic_store_n(&ready,1,__ATOMIC_RELEASE);return 0;
}
