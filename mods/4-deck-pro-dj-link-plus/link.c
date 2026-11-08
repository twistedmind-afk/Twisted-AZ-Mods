/* SPDX-License-Identifier: GPL-3.0-only
 * LINK Plus fixed-slot 3+1. Exact firmware only.
 * Native LINK setting and network SystemManager remain On. Regular function hooks: selected local-role
 * listeners receive the ordinary internal-deck mode; factory count becomes four.
 * A validated four-role task enables the native allocator event-3 early return.
 * No direct object/code/setting writes, no fake packets, no network in callbacks,
 * no Core/watchdog changes. Native LINK may already be On at startup.
 */
#include "azmod-mod.h"
#include "firmware-check.h"
#include "profile.h"
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
extern int role_vector_keep(void *,unsigned);
static unsigned mask(void){return profile_mask(public_on(),profile_current());}
static unsigned hybrid(void){return public_on() && profile_current()!=0;}
static const int INTERNAL=0;
static unsigned word_at(const void *p,unsigned offset){unsigned v;memcpy(&v,(const unsigned char *)p+offset,4);return v;}
static unsigned deck_internal(unsigned deck){return deck<4 && (mask()&(1u<<deck));}
/* Each listener's event is authoritative; the network listener can run later. */
static unsigned listener_internal(unsigned deck){return deck<4 && (profile_mask(1,profile_current())&(1u<<deck));}

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
 if(profile_current()!=0 && value && *value==1 && identity(self,0x29ab6e8) && word_at(self,0x40)<4 && listener_internal(word_at(self,0x40)))use=&INTERNAL;
 note(0,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_0(self,item,use);
}
/* usecase::load::LoadController; this pointer is its exact setting-listener subobject. */
static listener_fn original_1;
static void local_1(void *self,void *item,const int *value) {
 const int *use=value;
 if(profile_current()!=0 && value && *value==1 && identity(self,0x29f2458) && word_at(self,0x1e8)<4 && listener_internal(word_at(self,0x1e8)))use=&INTERNAL;
 note(1,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_1(self,item,use);
}
/* usecase::load::InstantDoublesLoader; this pointer is its exact setting-listener subobject. */
static listener_fn original_2;
static void local_2(void *self,void *item,const int *value) {
 const int *use=value;
 if(profile_current()!=0 && value && *value==1 && identity(self,0x2a03db0) && word_at(self,0x104)==1 && word_at(self,0x108)<4 && listener_internal(word_at(self,0x108)))use=&INTERNAL;
 note(2,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_2(self,item,use);
}
/* usecase::sync::SyncMemberSystem; this pointer is its exact setting-listener subobject. */
static listener_fn original_3;
static void local_3(void *self,void *item,const int *value) {
 const int *use=value;
 if(profile_current()!=0 && value && *value==1 && identity(self,0x2a9aa20))use=&INTERNAL;
 note(3,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_3(self,item,use);
}
/* gui::LoadMenuWidgetController; this pointer is its exact setting-listener subobject. */
static listener_fn original_4;
static void local_4(void *self,void *item,const int *value) {
 const int *use=value;
 if(profile_current()!=0 && value && *value==1 && identity(self,0x2ed7b68))use=&INTERNAL;
 note(4,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_4(self,item,use);
}
/* gui::PlayerSelectWidgetController; this pointer is its exact setting-listener subobject. */
static listener_fn original_5;
static void local_5(void *self,void *item,const int *value) {
 const int *use=value;
 if(profile_current()!=0 && value && *value==1 && identity(self,0x3083430))use=&INTERNAL;
 note(5,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_5(self,item,use);
}
/* gui::browser_logic::ApplicationSettingUpdateFacade; this pointer is its exact setting-listener subobject. */
static listener_fn original_6;
static void local_6(void *self,void *item,const int *value) {
 const int *use=value;
 if(profile_current()!=0 && value && *value==1 && identity(self,0x3167f08))use=&INTERNAL;
 note(6,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_6(self,item,use);
}
/* gui::DeckStateInfoManager; this pointer is its exact setting-listener subobject. */
static listener_fn original_7;
static void local_7(void *self,void *item,const int *value) {
 const int *use=value;
 if(profile_current()!=0 && value && *value==1 && identity(self,0x3188de8))use=&INTERNAL;
 note(7,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_7(self,item,use);
}
/* gui::UtilitySetting; this pointer is its exact setting-listener subobject. */
static listener_fn original_8;
static void local_8(void *self,void *item,const int *value) {
 const int *use=value;
 if(profile_current()!=0 && value && *value==1 && identity(self,0x31a0418) && word_at(self,0x1e8)<4 && listener_internal(word_at(self,0x1e8)))use=&INTERNAL;
 note(8,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_8(self,item,use);
}
/* hui::deck::DeckSelector; this pointer is its exact setting-listener subobject. */
static listener_fn original_9;
static void local_9(void *self,void *item,const int *value) {
 const int *use=value;
 if(profile_current()!=0 && value && *value==1 && identity(self,0x31b9db0)) {
  const uintptr_t *p=self;
  const unsigned char *a=(const unsigned char *)p[6],*b=(const unsigned char *)p[7];
  if(identity(a,0x31ba738) && identity(b,0x31ba738) && a[12]>=1 && a[12]<=2 && b[12]==a[12]+2 && listener_internal(b[12]-1))use=&INTERNAL;
 }
 note(9,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 original_9(self,item,use);
 /* The independent LOAD mod captures the queued native setting task. */
}
/* PlayerLinkInfoCrawler: retain its native four-player collection policy.
 * This is its ordinary setting method, not the network mode setter. */
static listener_fn crawler_original;
static void crawler_mode(void *self,void *item,const int *value) {
 const int *use=value;
 if(profile_current()!=0 && value && *value==1 && identity(self,0x2926a10))use=&INTERNAL;
 note(10,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 crawler_original(self,item,use);
}
/* Native overview constructor owns a four-element vector of CautionManager
 * pointers. Copy identity metadata only; never dereference a saved pointer. */
struct caution_identity {unsigned state,deck;uintptr_t target;};
static struct caution_identity caution_ids[64];
static unsigned caution_deck(const void *target) {
 for(unsigned i=0;i<64;i++)if(__atomic_load_n(&caution_ids[i].state,__ATOMIC_ACQUIRE)==1 && caution_ids[i].target==(uintptr_t)target)return __atomic_load_n(&caution_ids[i].deck,__ATOMIC_ACQUIRE);
 return 99;
}
static void remember_caution(const void *target,unsigned deck) {
 for(unsigned i=0;i<64;i++) {
  unsigned state=__atomic_load_n(&caution_ids[i].state,__ATOMIC_ACQUIRE);
  if(state==1 && caution_ids[i].target==(uintptr_t)target){__atomic_store_n(&caution_ids[i].deck,deck,__ATOMIC_RELEASE);return;}
  unsigned zero=0;
  if(__atomic_compare_exchange_n(&caution_ids[i].state,&zero,2,0,__ATOMIC_ACQUIRE,__ATOMIC_RELAXED)) {
   caution_ids[i].target=(uintptr_t)target;caution_ids[i].deck=deck;
   __atomic_store_n(&caution_ids[i].state,1,__ATOMIC_RELEASE);return;
  }
 }
}
static void (*overview_ctor_original)(void *,void *);
static void overview_ctor(void *self,void *context) {
 overview_ctor_original(self,context);
 const uintptr_t *v=(const uintptr_t *)((const unsigned char *)self+0x38);
 uintptr_t begin=v[0],end=v[1];
 if(!begin || (begin&7) || end<begin || end-begin!=4*sizeof(void *))return;
 for(unsigned d=0;d<4;d++) {
  void *target=*(void **)(begin+d*sizeof(void *));
  if(!identity(target,0x31af208))continue;
  remember_caution(target,d);note(192,target,(uintptr_t)__builtin_return_address(0),d,mask(),0);
  /* Its setting registration can precede completion of the parent ctor.
   * Apply only the ordinary native clear request for this internal deck. */
  if(d>=2 && hybrid() && deck_internal(d)) {
   void (*clear_native)(void *,unsigned)=(void (*)(void *,unsigned))(uintptr_t)0x1ac5580;
   clear_native(target,0x23);
  }
 }
}
/* Only the native LINK PLAYER caution listener receives internal mode. */
static listener_fn caution_original;
static void caution_mode(void *self,void *item,const int *value) {
 const int *use=value;
 if(identity(self,0x2d165c8)) {
  const void *target=*(const void *const *)((const unsigned char *)self+0x50);
  if(target && !((uintptr_t)target&7)) {
   uintptr_t vt=*(const uintptr_t *)target;
   unsigned d=caution_deck(target);
   if(profile_current()!=0 && value && *value==1 && d<4 && listener_internal(d))use=&INTERNAL;
   if(vt==0x31af208)note(191,(void *)target,(uintptr_t)__builtin_return_address(0),d,use?(unsigned)*use:99,(unsigned)vt);
  }
 }
 note(11,self,(uintptr_t)__builtin_return_address(0),value?(unsigned)*value:99,use?(unsigned)*use:99,0);
 caution_original(self,item,use);
}
static count_fn count_original;
/* The initial setting can queue LINK PLAYER before the owning overview has
 * completed construction. Clearing in the constructor alone is too early.
 * Filter only that exact caution, on a validated internal secondary deck, at
 * its ordinary native request entry. No periodic clearing or task mutation.
 */
static void (*caution_request_original)(void *,unsigned);
static void caution_request(void *target,unsigned kind) {
 if(kind==0x23 && profile_current()!=0 && identity(target,0x31af208)) {
  unsigned deck=caution_deck(target);
  if(deck>=2 && deck<4 && listener_internal(deck)) {
   note(195,target,(uintptr_t)__builtin_return_address(0),deck,0,kind);
   return;
  }
 }
 caution_request_original(target,kind);
}
static mode_fn mixer_original,network_original;
static run_fn task_original;
static packet_fn packet_original;
static void player_count(void *owner,unsigned count) {
 uintptr_t caller=(uintptr_t)__builtin_return_address(0);unsigned use=count;
 if(__atomic_load_n(&ready,__ATOMIC_ACQUIRE) && (count==2 || count==4) && caller==0x13e4a28 &&
    identity(owner,0x2bbf270) && ((const unsigned char *)owner)[0x400]==0x64) use=public_on()?(profile_current()?3:2):4;
 note(100,owner,caller,count,use,0);
 if(__atomic_load_n(&ready,__ATOMIC_ACQUIRE) && hybrid() && caller==0x13e4a28 && identity(owner,0x2bbf270) && ((const unsigned char *)owner)[0x400]==0x64) {
  count_original(owner,4);
  int rc=role_vector_keep(owner,mask());
  note(181,owner,caller,mask(),(unsigned)rc,0);
  if(rc)count_original(owner,2); /* Native two-player fallback; no conflicting local 3/4 identities. */
 } else count_original(owner,use);
}
static void mixer_mode(void *owner,unsigned char mode) {
 uintptr_t caller=(uintptr_t)__builtin_return_address(0);unsigned char use=mode;
 if(hybrid() && mode==1 && caller==0xb61850 && identity(owner,0x31f5728))use=0;
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
  unsigned players=public_on()?(profile_current()?3:2):4;
  if(__atomic_load_n(&ready,__ATOMIC_ACQUIRE) && p[0x400]==0x64 && begin && count==players+1) {
   const unsigned char *roles=(const unsigned char *)begin;
   unsigned valid=1;
   unsigned slot=0;
   for(unsigned i=0;i<4;++i)if(mask()&(1u<<i)) {
    if(slot>=players || roles[slot*0x50]!=1 || roles[slot*0x50+1]!=i+1)valid=0;
    ++slot;
   }
   if(slot!=players)valid=0;
   if(roles[players*0x50]!=2 || roles[players*0x50+1]!=33) valid=0;
   if(valid) __atomic_store_n(&validated_owner,(uintptr_t)owner,__ATOMIC_RELEASE);
  }
 }
 note(103,owner,(uintptr_t)__builtin_return_address(0),mode,mode,count);
}
/* Hybrid mode must use native assignment/admission; the four-internal
 * early-return applies only when every deck number is local.
 * No role-vector access here: player_task publishes its validation on the native
 * thread after the factory completes. Event 3 already has a native return-5 path. */
static unsigned operate_packet(void *self,unsigned event,const void *ip,const void *wlan) {
 if(__atomic_load_n(&ready,__ATOMIC_ACQUIRE) && (event&0xffu)==3 &&
    mask()==15 && identity(self,0x2bc1d78)) {
  void *owner=*(void *const *)((const unsigned char *)self+0x10);
  if((uintptr_t)owner==__atomic_load_n(&validated_owner,__ATOMIC_ACQUIRE) &&
     identity(owner,0x2bbf270) && ((const unsigned char *)owner)[0x400]==0x64) {
   note(104,owner,(uintptr_t)__builtin_return_address(0),event,5,5);
   return 5;
  }
 }
 return packet_original(self,event,ip,wlan);
}

extern void *native_lookup_original;
extern void allocation_lookup(void);
unsigned link_reserves_number(void *owner,unsigned number,uintptr_t caller) {
 if(caller!=0x13f2b90 || !hybrid() || number<3 || number>4 ||
    !__atomic_load_n(&ready,__ATOMIC_ACQUIRE) || !identity(owner,0x2bbf270) ||
    (uintptr_t)owner!=__atomic_load_n(&validated_owner,__ATOMIC_ACQUIRE))return 0;
 unsigned occupied=!!(mask()&(1u<<(number-1)));
 if(occupied)note(194,owner,caller,number,0,mask());
 return occupied;
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
  unsigned mode=mask()==3?1:0;
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
 if(!hybrid() || deck_internal(index))full_original(self,data,destination,index);
}
static void transmit_condensed(void *self,const void *data,const void *destination,unsigned char index) {
 retain_four_transmitters(self,index,(uintptr_t)__builtin_return_address(0));
 if(!hybrid() || deck_internal(index))condensed_original(self,data,destination,index);
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
 if(!firmware_matches(0x13e5670,16,UINT64_C(0x80d5739cf6b4aaf8)))return 126;
 if(!firmware_matches(0x13e5810,16,UINT64_C(0x9c691015efe761a0)))return 126;
 if(!firmware_matches(0x13e5f50,16,UINT64_C(0x3f63091a2735059a)))return 126;
 if(!firmware_matches(0x217e968,16,UINT64_C(0x061b9152eb3660ac)))return 127;
 if(!firmware_matches(0x1ac5580,16,UINT64_C(0xdef91ab135c6a4be)))return 127;
 if(!firmware_matches(0x13dc650,16,UINT64_C(0x97a656c2bac97f98)))return 129;
 if(!firmware_matches(0x1ac4f40,16,UINT64_C(0xbdbfa0169010db62)))return 130;
 return 0;
}
int link_install(const az_core_v1 *core) {
 core->log(ID,"LINK Plus p11: explicit 3+1 slot selection; native admission and occupied-slot reservation");
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
 if(core->claim_hook(ID,0x217e968,(void *)overview_ctor,(void **)&overview_ctor_original))return 73;
 if(core->claim_hook(ID,0x13dc650,(void *)allocation_lookup,&native_lookup_original))return 75;
 if(core->claim_hook(ID,0x1ac4f40,(void *)caution_request,(void **)&caution_request_original))return 76;
 __atomic_store_n(&ready,1,__ATOMIC_RELEASE);return 0;
}
