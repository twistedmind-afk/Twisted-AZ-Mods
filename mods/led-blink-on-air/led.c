/* SPDX-License-Identifier: GPL-3.0-only
 * Exact-firmware QEMU test. Native deck-specific indication policy, no frame callbacks.
 * Source getIndication is distinct from HuiLed::getColor / bus serialization.
 * Hardware safety of this new source hook has NOT been established.
 */
#include "azmod-mod.h"
#include "settings.h"
#include "firmware-check.h"
#include "led-policy.h"
#include <stdint.h>
#include <string.h>
#include "mod-id.h"
#define ID MOD_ID
typedef struct { uint64_t head, rest; } indication;
typedef indication (*indicate_fn)(void *);
typedef unsigned (*air_get_fn)(void *);
typedef void (*air_update_fn)(void *);
static indicate_fn original_indicate;
static air_get_fn original_air_get;
static air_update_fn original_air_update;
static const az_core_v1 *api;
static unsigned ready;
/* 0 unknown, 1 native Off, 2 native On. No retained native pointers. */
static unsigned onair[4],samples[4],native_calls[4],last[4],unknown_identity;
extern int load_led_prepare(const az_core_v1 *);
extern int load_led_install(const az_core_v1 *);
extern void load_led_stats(void);
extern unsigned load_led_bright(void);
unsigned led_onair(unsigned deck) {
 return deck<4 ? __atomic_load_n(&onair[deck],__ATOMIC_ACQUIRE) : 0;
}
static unsigned read32(const void *p) { unsigned n;memcpy(&n,p,sizeof n);return n; }
static int is_type(const void *p,uintptr_t vt) {
 return p && !((uintptr_t)p&7) && *(const uintptr_t *)p==vt;
}
static void observe_air(void *self,unsigned value) {
 if(!__atomic_load_n(&ready,__ATOMIC_ACQUIRE) || value>1 || !is_type(self,0x2a15750))return;
 const unsigned char *p=self;
 /* Constructor 0xad4f58 stores local PlayerID variant tag=1 and deck index.
  * Factory 0xad7150 creates these for its bounded four-deck loop. */
 if(read32(p+0x20)!=1)return;
 unsigned deck=read32(p+0x24);if(deck>=4)return;
 __atomic_store_n(&onair[deck],value+1,__ATOMIC_RELEASE);
 __atomic_fetch_add(&samples[deck],1,__ATOMIC_RELAXED);
}
static unsigned air_get(void *self) {
 unsigned value=original_air_get(self);observe_air(self,value);return value;
}
static void air_update(void *self) {
 original_air_update(self);
 if(is_type(self,0x2a15750)) {
  unsigned value=__atomic_load_n((const unsigned char *)self+0x4a,__ATOMIC_ACQUIRE);
  observe_air(self,value);
 }
}
static indication indicate(void *self) {
 indication result=original_indicate(self);
 if(!__atomic_load_n(&ready,__ATOMIC_ACQUIRE))return result;
 if(!is_type(self,0x31ba738)) {
  __atomic_fetch_add(&unknown_identity,1,__ATOMIC_RELAXED);return result;
 }
 const unsigned char *p=self;unsigned native_id=p[0x0c];
 if(native_id<1 || native_id>4) {__atomic_fetch_add(&unknown_identity,1,__ATOMIC_RELAXED);return result;}
 unsigned deck=native_id-1;
 if(deck>=4 || p[0x1c]>1 || p[0x1d]>1 || (result.head&255)>2) {
  __atomic_fetch_add(&unknown_identity,1,__ATOMIC_RELAXED);return result;
 }
 unsigned air=__atomic_load_n(&onair[deck],__ATOMIC_ACQUIRE);
 unsigned before=result.head&255;
 /* Keep disabled decks and unknown OnAir states native. Selection determines
  * the blink range: selected bright/dim; unselected dim/off. */
 if(blink_select_enabled() && air && p[0x1d]) {
  unsigned selected=p[0x1c];
  /* Sample the very same native animation as LOAD. No independently phased
   * clock or Mod-tick cache: both indicators use its 500 ms epoch/phase. */
  unsigned brightness=select_led_level(1,air,selected,1,air==2 ? load_led_bright() : 0,before);
  result.head=(result.head&~UINT64_C(255))|brightness;
 }
 unsigned state=before|((unsigned)(result.head&255)<<4)|(air<<8)|((unsigned)p[0x1c]<<12)|((unsigned)p[0x1d]<<13);
 __atomic_store_n(&last[deck],state+1,__ATOMIC_RELEASE);
 __atomic_fetch_add(&native_calls[deck],1,__ATOMIC_RELAXED);
 return result;
}
void led_tick(void) {
 static unsigned seen[4],ticks;
 for(unsigned d=0;d<4;++d) {
  unsigned state=__atomic_load_n(&last[d],__ATOMIC_ACQUIRE);
  /* Do not log each blink edge; only native/OnAir/selection changes. */
  if(state && (state&~0xf0u)!=(seen[d]&~0xf0u)) {
   seen[d]=state;unsigned s=state-1;
   api->log(ID,"LED deck=%u native=%u output=%u air=%u selected=%u enabled=%u",d+1,s&15,(s>>4)&15,(s>>8)&3,(s>>12)&1,(s>>13)&1);
  }
 }
 if(++ticks%150==0) {load_led_stats();api->log(ID,"COUNTS air=%u,%u,%u,%u led=%u,%u,%u,%u unknown=%u",
  __atomic_load_n(&samples[0],__ATOMIC_RELAXED),__atomic_load_n(&samples[1],__ATOMIC_RELAXED),
  __atomic_load_n(&samples[2],__ATOMIC_RELAXED),__atomic_load_n(&samples[3],__ATOMIC_RELAXED),
  __atomic_load_n(&native_calls[0],__ATOMIC_RELAXED),__atomic_load_n(&native_calls[1],__ATOMIC_RELAXED),
  __atomic_load_n(&native_calls[2],__ATOMIC_RELAXED),__atomic_load_n(&native_calls[3],__ATOMIC_RELAXED),
  __atomic_load_n(&unknown_identity,__ATOMIC_RELAXED));}
}
/* Checked against the original ELF before any SDK claim; supplied below. */

int led_prepare(const az_core_v1 *core) {
 api=core;
 if(!AZ_HAS(core,claim_hook)||!AZ_HAS(core,fw_tag)||!AZ_HAS(core,log))return 1;
 const char *tag=core->fw_tag();if(!tag||strcmp(tag,"ep147-9e9454e357389ece"))return 2;
 if(*(const uintptr_t *)0x31ba748!=0x21a86e8 || *(const uintptr_t *)0x2a15760!=0xad5cb8)return 3;
 if(!firmware_matches(0x21a86e8, 16, UINT64_C(0xdc74d1d352ffe0cb)) ||
    !firmware_matches(0xad5cb8, 16, UINT64_C(0x779d4da8c2a4b665)) ||
    !firmware_matches(0xad5020, 16, UINT64_C(0xcfc32e26a05cceda)))return 4;
 return load_led_prepare(core) ? 5 : 0;
}
int led_install(const az_core_v1 *core) {
 if(core->claim_hook(ID,0xad5020,(void *)air_update,(void **)&original_air_update))return 10;
 if(core->claim_hook(ID,0xad5cb8,(void *)air_get,(void **)&original_air_get))return 11;
 if(core->claim_hook(ID,0x21a86e8,(void *)indicate,(void **)&original_indicate))return 12;
 if(load_led_install(core))return 13;
 __atomic_store_n(&ready,1,__ATOMIC_RELEASE);
 core->log(ID,"LED policy 0.2.2: SELECT shares LOAD native 500ms animation phase; selected bright/dim, unselected dim/off; no frame callbacks");
 return 0;
}
