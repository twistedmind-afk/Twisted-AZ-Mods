/* SPDX-License-Identifier: GPL-3.0-only
 * Per-LOAD-controller update, on its native thread and with native locking.
 * No serializer/getColor hook, frame filter, retained firmware object or raw
 * writes into native indicator state. Exact firmware only. QEMU validation
 * does not establish physical hardware stability.
 */
#include "azmod-mod.h"
#include "settings.h"
#include "mod-id.h"
#include "firmware-check.h"
#include <cstdint>
#include <cstring>
#include <memory>

extern "C" unsigned led_onair(unsigned deck);
static const az_core_v1 *api;
static void (*original_update)(void *);
static unsigned calls[4], overrides[4], unknown;

/* Layout established from 0x21b8bb8 / 0x21b8c10 / 0x21b8d28. The native
 * animation is non-polymorphic; its constructor owns pointers to its fields.
 * Construct in final storage, never copy or move this object afterward.
 * Native shared_ptr consumers use the standard libstdc++ control block ABI.
 */
struct alignas(8) Animation {
 unsigned char bytes[0x40];
 Animation() {
  using ctor=void (*)(void *,int,int,unsigned,unsigned,unsigned);
  reinterpret_cast<ctor>(0x21b8bb8)(this,500,0,0,0,2);
 }
 Animation(const Animation &)=delete;
 Animation &operator=(const Animation &)=delete;
};
static std::shared_ptr<Animation> *blink;
static_assert(sizeof(std::shared_ptr<Animation>)==16,"Native shared_ptr layout required");

extern "C" unsigned load_led_bright(void) {
 /* Called only after led_install publishes ready. Constructor completed first;
  * the shared animation is then immutable for the lifetime of the process.
  * Native phase getter 0x21b8c10 only reads the object and system_clock::now.
  * It returns the same 2/0 levels that the LOAD indicator consumes. */
 using phase_fn=unsigned (*)(const void *);
 return reinterpret_cast<phase_fn>(0x21b8c10)(blink->get())==2;
}

static void update_load(void *self) {
 original_update(self);
 if(!self || (reinterpret_cast<uintptr_t>(self)&7) ||
    *reinterpret_cast<const uintptr_t *>(self)!=0x31bdb10) {
  __atomic_fetch_add(&unknown,1,__ATOMIC_RELAXED);return;
 }
 /* Native factory gives each LOAD controller its browser indicator 1..4;
  * the constructor stores that identifier at +0x68, immutable for its life.
  */
 auto state=*reinterpret_cast<void *const *>(static_cast<unsigned char *>(self)+8);
 if(!state || (reinterpret_cast<uintptr_t>(state)&7) ||
    *reinterpret_cast<const uintptr_t *>(state)!=0x31c0288) {
  __atomic_fetch_add(&unknown,1,__ATOMIC_RELAXED);return;
 }
 unsigned id;std::memcpy(&id,static_cast<unsigned char *>(state)+0x68,4);
 if(id<1 || id>4) {__atomic_fetch_add(&unknown,1,__ATOMIC_RELAXED);return;}
 unsigned deck=id-1;
 __atomic_fetch_add(&calls[deck],1,__ATOMIC_RELAXED);
 if(!blink_load_enabled() || led_onair(deck)!=2)return; /* Unknown and Off Air retain native behavior. */
 using get_color=uint64_t (*)(void *);
 using publish=void (*)(void *,const unsigned *,const unsigned char *,const std::shared_ptr<Animation> *);
 uint64_t current=reinterpret_cast<get_color>(0x21e71a8)(state);
 unsigned char rgb[3]={static_cast<unsigned char>(current>>32),
  static_cast<unsigned char>(current>>40),static_cast<unsigned char>(current>>48)};
 const unsigned bright=2;
 reinterpret_cast<publish>(0x21e7a68)(state,&bright,rgb,blink);
 __atomic_fetch_add(&overrides[deck],1,__ATOMIC_RELAXED);
}

extern "C" int load_led_prepare(const az_core_v1 *core) {
 api=core;

 

 
 /* Init-time bounded fingerprints precede SDK hook claims. ARM64 call and
  * ownership checks are recorded by tools/verify_sources.py. */
 if(*reinterpret_cast<const uintptr_t *>(0x31bdb10)!=0x21be130 ||
    *reinterpret_cast<const uintptr_t *>(0x31c0298)!=0x21e7488)return 1;
 if(!firmware_matches(0x21be130, 16, UINT64_C(0xf977ffd5ef4ca8cf)))return 2;
 if(!firmware_matches(0x21e71a8, 16, UINT64_C(0x0cea7ce4718faa7b)))return 3;
 if(!firmware_matches(0x21e7a68, 16, UINT64_C(0xd2381d95aba9da35)) ||
    !firmware_matches(0x21b8bb8, 16, UINT64_C(0x995a461be2f65df6)) ||
    !firmware_matches(0x21b8c10, 16, UINT64_C(0xa2d9247af6643382)) ||
    !firmware_matches(0x21b8d28, 16, UINT64_C(0x492656bb96bd9e3f)))return 4;
 try {blink=new std::shared_ptr<Animation>(std::make_shared<Animation>());}
 catch(...) {return 5;}
 return 0;
}
extern "C" int load_led_install(const az_core_v1 *core) {
 /* Process-lifetime ownership, initialized only after firmware validation.
  * There is no pre-init firmware call or global C++ constructor. */
 api=core;
 if(core->claim_hook(MOD_ID,0x21be130,reinterpret_cast<void *>(update_load),reinterpret_cast<void **>(&original_update)))return 1;
 core->log(MOD_ID,"LOAD LED: native 500ms full/off animation on On Air; one controller hook; no frame filters");
 return 0;
}
extern "C" void load_led_stats(void) {
 api->log(MOD_ID,"LOADLED calls=%u,%u,%u,%u overrides=%u,%u,%u,%u unknown=%u",
  __atomic_load_n(&calls[0],__ATOMIC_RELAXED),__atomic_load_n(&calls[1],__ATOMIC_RELAXED),
  __atomic_load_n(&calls[2],__ATOMIC_RELAXED),__atomic_load_n(&calls[3],__ATOMIC_RELAXED),
  __atomic_load_n(&overrides[0],__ATOMIC_RELAXED),__atomic_load_n(&overrides[1],__ATOMIC_RELAXED),
  __atomic_load_n(&overrides[2],__ATOMIC_RELAXED),__atomic_load_n(&overrides[3],__ATOMIC_RELAXED),
  __atomic_load_n(&unknown,__ATOMIC_RELAXED));
}
