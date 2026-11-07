/* SPDX-License-Identifier: GPL-3.0-only
 * Native LOAD-to-SELECT experiment, exact firmware and SDK hooks only.
 * No frame filters, ERP editing or changes to Core/watchdog protections.
 */
#include "azmod-mod.h"
#include "mode.h"
#include "firmware-check.h"
#include <stdint.h>
#include <string.h>
#include "mod-id.h"
#define ID MOD_ID
#define TRANSITION ((uintptr_t)0x21a9cb0)
#define DUAL_VT ((uintptr_t)0x31bab80)
#define STATE_VT ((uintptr_t)0x31ba738)
#define UNKNOWN_STATE 0x10000u
#define SLOTS 32u
typedef void (*transition_fn)(void *, unsigned);
static transition_fn original;
static const az_core_v1 *api;
struct event {
    unsigned state; /* 0 free, 3 writer owns, 1 published, 2 reader owns */
    uintptr_t self, thread, caller;
    unsigned choice, before, after, primary_id, secondary_id, enabled;
};
static struct event events[SLOTS];
static unsigned cursor, dropped, unknown;
extern void queue_arm(void);
extern void queue_stats(uint64_t *);
extern void *queue_worker(void *);
extern void queue_capture(void *);
extern void queue_manual(void *,uintptr_t);
extern void queue_loaded(unsigned);
extern int queue_gui_deck(void *,unsigned);
extern void bridge_init(const az_core_v1 *);
extern void bridge_construct(void *);
extern void bridge_request(unsigned);
extern void bridge_stats(uint64_t *);
extern void *bridge_worker(void *);
typedef void (*ctor_fn)(void *,void *,void *,void *,void *,void *);
typedef void (*gui_fn)(void *,unsigned);
typedef void (*task_fn)(void *);
static task_fn original_capture_task;
static void capture_task(void *task) {
    void *selector=0;
    if(task && *(const uintptr_t *)task==0x31b9e80)
        selector=*(void **)((unsigned char *)task+0x18);
    original_capture_task(task);
    if(selector)queue_capture(selector);
}
static ctor_fn original_ctor;
static gui_fn original_gui;
static void construct(void *self,void *a,void *b,void *c,void *d,void *e) {
    original_ctor(self,a,b,c,d,e);
    queue_capture(self);
}
static void gui_selection(void *self,unsigned choice) {
    uintptr_t caller=(uintptr_t)__builtin_return_address(0);
    queue_manual(self,caller);
    if(caller==0x21a76c0 || caller==0x21a7710) {
        int deck=queue_gui_deck(self,choice);
        if(deck>=0)bridge_request((unsigned)deck);
    }
    original_gui(self,choice);
}
static void loaded(int deck,int what,void *user) {
    (void)user;
    if(what==AZ_DECK_LOADED && deck>=0 && deck<4) {
        api->log(ID,"LOADED deck=%d",deck+1);
        queue_loaded((unsigned)deck);
    }
}
static uintptr_t token(void) { return (uintptr_t)__builtin_thread_pointer(); }
static unsigned snapshot(const void *self, unsigned *p, unsigned *q, unsigned *en) {
    /* EP147 itself uses these owned fields during this call. Saved identities
     * are never dereferenced after the call, even if objects are rebuilt. */
    const uintptr_t *obj = self;
    if (obj[0] != DUAL_VT) return UNKNOWN_STATE;
    const unsigned char *a = (const void *)obj[8];
    const unsigned char *b = (const void *)obj[20];
    if (!a || !b || *(const uintptr_t *)a != STATE_VT || *(const uintptr_t *)b != STATE_VT)
        return UNKNOWN_STATE;
    *p = a[0xc]; *q = b[0xc];
    *en = a[0x1d] | ((unsigned)b[0x1d] << 8);
    return a[0x1c] | ((unsigned)b[0x1c] << 8);
}
static void observe(void *self, unsigned choice) {
    unsigned index = __atomic_fetch_add(&cursor, 1, __ATOMIC_RELAXED) % SLOTS;
    struct event *e = &events[index];
    unsigned expected = 0;
    int owned = __atomic_compare_exchange_n(&e->state, &expected, 3, 0,
                                            __ATOMIC_ACQUIRE, __ATOMIC_RELAXED);
    if (owned) {
        e->self = (uintptr_t)self;
        e->thread = token();
        e->caller = (uintptr_t)__builtin_return_address(0);
        e->choice = choice;
        e->primary_id = e->secondary_id = e->enabled = 0;
        e->before = snapshot(self, &e->primary_id, &e->secondary_id, &e->enabled);
    } else __atomic_fetch_add(&dropped, 1, __ATOMIC_RELAXED);
    original(self, choice); /* Same arguments, exactly once. */
    queue_arm();
    if (owned) {
        unsigned p = 0, q = 0, en = 0;
        e->after = snapshot(self, &p, &q, &en);
        if (e->before == UNKNOWN_STATE || e->after == UNKNOWN_STATE)
            __atomic_fetch_add(&unknown, 1, __ATOMIC_RELAXED);
        __atomic_store_n(&e->state, 1, __ATOMIC_RELEASE);
    }
}
static void load_tick(void);
static void tick(void) {
    load_tick();
    uint64_t b[19];static uint64_t oldb[19];bridge_stats(b);
    if(memcmp(b,oldb,sizeof b)) {
        api->log(ID,"bridge capture=%lu request=%lu posted=%lu ran=%lu invoked=%lu skipped=%lu destroyed=%lu error=%lu ui_thread=%lx select_thread=%lx rejected=%lu overflow=%lu reason=%lu browse_head=%lx handler_vt=%lx flags=%lx row=%lu type=%lu menu_type=%lu",
            b[0],b[1],b[2],b[3],b[4],b[5],b[6],b[7],b[8],b[9],b[10],b[11],b[12],b[13],b[14],b[15],b[16],b[17],b[18]);
        memcpy(oldb,b,sizeof b);
    }
    uint64_t values[14];
    static uint64_t previous[14];
    queue_stats(values);
    if(memcmp(values,previous,sizeof values)) {
        api->log(ID,"queue posted=%lu ran=%lu destroyed=%lu error=%lu producer=%lx consumer=%lx selected=%lu skipped=%lu captures=%lu manual=%lu,%lu overflow=%lu owners=%lu no_ticket=%lu",
            values[0],values[1],values[2],values[3],values[4],values[5],values[6],values[7],values[8],values[9],values[10],values[11],values[12],values[13]);
        memcpy(previous,values,sizeof values);
    }
    for (unsigned i = 0; i < SLOTS; ++i) {
        struct event *e = &events[i];
        unsigned expected = 1;
        if (!__atomic_compare_exchange_n(&e->state, &expected, 2, 0,
                                         __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) continue;
        api->log(ID, "native owner=%lx caller=%lx thread=%lx same_overlay=%u choice=%u ids=%u,%u enabled=%x state=%x->%x",
                 (unsigned long)e->self, (unsigned long)e->caller, (unsigned long)e->thread,
                 e->thread == token(), e->choice, e->primary_id, e->secondary_id,
                 e->enabled, e->before, e->after);
        __atomic_store_n(&e->state, 0, __ATOMIC_RELEASE);
    }
    unsigned d = __atomic_exchange_n(&dropped, 0, __ATOMIC_RELAXED);
    unsigned u = __atomic_exchange_n(&unknown, 0, __ATOMIC_RELAXED);
    if (d || u) api->log(ID, "telemetry dropped=%u unknown=%u", d, u);
}

typedef void (*load_fn)(void*,unsigned);
typedef void (*load_ctor_fn)(void*,const void*);
static load_fn original_load;
static load_ctor_fn original_load_ctor;
struct load_record {unsigned state,kind,deck,view; uintptr_t self,thread,weak,browse,handler;};
static struct load_record load_records[32];
static unsigned load_cursor,load_dropped;
static void load_record(void *self,unsigned kind,unsigned deck) {
    unsigned index=__atomic_fetch_add(&load_cursor,1,__ATOMIC_RELAXED)%32;
    struct load_record *r=&load_records[index];unsigned zero=0;
    if(!__atomic_compare_exchange_n(&r->state,&zero,2,0,__ATOMIC_ACQUIRE,__ATOMIC_RELAXED)) {
        __atomic_fetch_add(&load_dropped,1,__ATOMIC_RELAXED);return;
    }
    const uintptr_t *s=self;
    r->self=(uintptr_t)self;r->kind=kind;r->deck=deck;
    r->thread=(uintptr_t)__builtin_thread_pointer();r->view=(unsigned)api->view();
    r->weak=s[7];r->browse=s[9];r->handler=s[11];
    __atomic_store_n(&r->state,1,__ATOMIC_RELEASE);
}
static void load_construct(void *self,const void *args) {
    original_load_ctor(self,args);load_record(self,1,99);bridge_construct(self);
}
static void load_command(void *self,unsigned deck) {
    load_record(self,2,deck);original_load(self,deck);load_record(self,3,deck);
}
static void load_tick(void) {
    for(unsigned i=0;i<32;i++) {
        struct load_record *r=&load_records[i];unsigned one=1;
        if(!__atomic_compare_exchange_n(&r->state,&one,2,0,__ATOMIC_ACQUIRE,__ATOMIC_RELAXED))continue;
        api->log(ID,"load-probe kind=%u deck=%u view=%u self=%lx thread=%lx weak=%lx browse=%lx handler=%lx",
            r->kind,r->deck,r->view,r->self,r->thread,r->weak,r->browse,r->handler);
        __atomic_store_n(&r->state,0,__ATOMIC_RELEASE);
    }
    unsigned n=__atomic_exchange_n(&load_dropped,0,__ATOMIC_RELAXED);
    if(n)api->log(ID,"load-probe dropped=%u",n);
}

static int init(const az_core_v1 *core) {
    api = core;
    if(!AZ_HAS(core,view))return 11;
    if (!AZ_HAS(core, log) || !AZ_HAS(core, fw_tag) || !AZ_HAS(core, claim_hook) || !AZ_HAS(core,thread_start) || !AZ_HAS(core,on_deck_event)) return 1;
    const char *tag = core->fw_tag();
    if (!tag || strcmp(tag, "ep147-9e9454e357389ece")) return 2;
    
    if (!firmware_matches(0x21a9cb0, 16, UINT64_C(0x45cb8c4196bbd221))) return 3;

    
    /* Do not fingerprint LINK's owned entry: it may already be SDK-hooked. */
    if(!firmware_matches(0x21a7ac8, 16, UINT64_C(0xb78da017066c359e)) || !firmware_matches(0x21ab938, 16, UINT64_C(0xe2dfc7d14cd85a0d))) return 3;
    
    if(!firmware_matches(0x2093c18, 16, UINT64_C(0x2295da7ca67ac59e)))return 12;
    
    if(!firmware_matches(0x2092ee8, 16, UINT64_C(0x5374dfb8884ae25d)))return 12;

    if(!firmware_matches(0x21a75c0, 16, UINT64_C(0x7cbdb2a8e833d91a)) || *(const uintptr_t *)0x31b9e90!=0x21a75c0)return 17;
    if(mode_init(core))return 16;
    core->log(ID,"0.1.0 independent LOAD/SELECT switches; native setting-task capture; no frame filters");
    bridge_init(core);
    if(core->on_deck_event(ID,loaded,0)) return 7;
    if(core->thread_start(ID,"select-load",bridge_worker,0))return 15;
    if(core->thread_start(ID,"native-select",queue_worker,0)) return 6;
    int rc = core->claim_hook(ID, TRANSITION, (void *)observe, (void **)&original);
    if(rc) return 4;
    /* The LINK mod exclusively owns 0x21a7720; this mod captures its native task instead. */
    if(core->claim_hook(ID,0x21a7ac8,(void *)construct,(void **)&original_ctor)) return 9;
    rc=core->claim_hook(ID,0x21ab938,(void *)gui_selection,(void **)&original_gui);
    if(rc)return 10;
    if(core->claim_hook(ID,0x2093c18,(void*)load_command,(void**)&original_load))return 13;
    if(core->claim_hook(ID,0x2092ee8,(void*)load_construct,(void**)&original_load_ctor))return 14;
    if(core->claim_hook(ID,0x21a75c0,(void *)capture_task,(void **)&original_capture_task))return 17;
    mode_activate();
    return 0;
}
static const char *const tags[] = {"ep147-9e9454e357389ece", 0};
static const az_mod_v1 mod = {.abi=AZ_MOD_ABI, .size=sizeof(az_mod_v1), .id=ID,
                            .fw_tags=tags, .init=init, .tick=tick};
AZ_MOD_EXPORT const az_mod_v1 *az_mod_v1_entry(void) { return &mod; }
