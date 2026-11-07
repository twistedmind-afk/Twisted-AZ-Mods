/* SPDX-License-Identifier: GPL-3.0-only
 * Exact EP147 firmware ABI. Uses the ordinary JUCE Message queue and native
 * listener weak ownership. Does not patch queues, vtables, guards or Core.
 * The worker only retains control blocks; browser objects are accessed solely
 * in callbacks on the native JUCE message thread.
 */
#include "azmod-mod.h"
#include "mode.h"
#include <stdint.h>
#include <new>
#include <time.h>

template<class F> static F ep(uintptr_t p){return reinterpret_cast<F>(p);}
static const az_core_v1 *api;
static uint64_t frequency;
static uint64_t clock_hz(){return __atomic_load_n(&frequency,__ATOMIC_ACQUIRE);}
static uint64_t counters[19];
enum {CAPTURE,REQUEST,POSTED,RAN,INVOKED,SKIPPED,DESTROYED,ERROR,UI_THREAD,SELECT_THREAD,REJECTED,OVERFLOW};
static void inc(unsigned i){__atomic_fetch_add(counters+i,1,__ATOMIC_RELAXED);}
static uint64_t now(){uint64_t n;asm volatile("mrs %0,cntvct_el0":"=r"(n));return n;}
static void fail(unsigned e){__atomic_store_n(counters+ERROR,e,__ATOMIC_RELEASE);}
static bool healthy(){return !__atomic_load_n(counters+ERROR,__ATOMIC_ACQUIRE);}
struct Owner {unsigned ready;void *control;};
static Owner owners[8]; /* immutable retained weak references, process lifetime */
static unsigned active;
struct Job {unsigned state,deck;uint64_t stamp,mode;};
static Job mailbox[16];
static unsigned cursor;
static void retain(void *ctrl){__atomic_fetch_add((uint32_t*)((char*)ctrl+8),1,__ATOMIC_ACQ_REL);}

static bool track_row(void *self) {
    const uintptr_t *s=(const uintptr_t*)self;
    __atomic_store_n(counters+12,3,__ATOMIC_RELAXED);
    void *browse=(void*)s[9],*handler=(void*)s[11];
    __atomic_store_n(counters+13,browse?*(uintptr_t*)browse:0,__ATOMIC_RELAXED);__atomic_store_n(counters+14,handler?*(uintptr_t*)handler:0,__ATOMIC_RELAXED);
    /* Facade +0x48 is plain browser configuration/state, not the polymorphic
     * BrowseDispData class. Verify the live LoadHandler owns this same state. */
    if(!browse||!handler||*(uintptr_t*)handler!=0x3166358||*(void**)((char*)handler+8)!=browse)return false;
    /* The native menu producer 0x20ac360 copies state+0x1c0 into its
     * signed menu type; 0x20ad318 publishes visible iff type != -1.
     * The native toggle 0x2031130 uses exactly the same field/test. Read it
     * directly through the facade's owned state, on the browser GUI thread. */
    unsigned menu_type=*((const unsigned char*)browse+0x1c0);
    __atomic_store_n(counters+18,menu_type,__ATOMIC_RELAXED);
    __atomic_store_n(counters+12,8,__ATOMIC_RELAXED);
    if(menu_type!=255)return false;
    __atomic_store_n(counters+12,4,__ATOMIC_RELAXED);
    const uint64_t flags=*(uint64_t*)((char*)browse+0x1e0);
    __atomic_store_n(counters+15,flags,__ATOMIC_RELAXED);
    if(!(flags&255)||!(flags&0x1f00000000ULL))return false;
    /* Same native getter and row qualification as LoadHandler::loadTrack.
     * It returns a borrowed pointer to the page holder, NOT a shared_ptr value. */
    __atomic_store_n(counters+12,5,__ATOMIC_RELAXED);
    void **holder=ep<void**(*)(void*)>(0x1fd8258)(browse);
    if(!holder||!*holder)return false;
    __atomic_store_n(counters+12,6,__ATOMIC_RELAXED);
    const char *page=(const char*)*holder;
    uintptr_t begin=*(const uintptr_t*)(page+0x68),end=*(const uintptr_t*)(page+0x70);
    unsigned row=*(const uint32_t*)(page+0x64);
    if(!begin||(begin&7)||end<begin||end-begin>0x2800000||(end-begin)%0x28||row>=(end-begin)/0x28)return false;
    __atomic_store_n(counters+12,7,__ATOMIC_RELAXED);__atomic_store_n(counters+16,row,__ATOMIC_RELAXED);
    const unsigned char *item=*(const unsigned char**)(begin+0x28*row+0x18);
    __atomic_store_n(counters+17,item?item[0x10]:0,__ATOMIC_RELAXED);
    bool ok=item&&(item[0x10]=='e'||item[0x10]==4);if(ok)__atomic_store_n(counters+12,0,__ATOMIC_RELAXED);return ok;
}

class Message {
public:
    uint32_t refs,padding;
    void *weak;
    unsigned kind,deck;
    uint64_t stamp,mode;
    Message(unsigned k,unsigned d,uint64_t t,uint64_t md=0):refs(0),padding(0),weak(nullptr),kind(k),deck(d),stamp(t),mode(md){
        /* Match the reviewed JUCE intrusive Message header at offsets 8,16. */
        if((char*)&refs-(char*)this!=8 || (char*)&weak-(char*)this!=16)fail(1);
    }
    virtual ~Message() noexcept {
        ep<void(*)(void*)>(0x25021e0)(this); /* normal native base destructor releases weak */
        inc(DESTROYED);
    }
    virtual void messageCallback() noexcept {
        inc(RAN);
        __atomic_store_n(counters+UI_THREAD,(uintptr_t)__builtin_thread_pointer(),__ATOMIC_RELAXED);
        try {
            if(!healthy()||!weak)return;
            if(*(uintptr_t*)weak!=0x34f5380){fail(2);return;}
            char *listener=*(char**)((char*)weak+0x10);
            if(!listener){inc(SKIPPED);return;}
            if(*(uintptr_t*)listener!=0x321b098){fail(3);return;}
            void *self=listener-0x30;
            if(*(uintptr_t*)self!=0x317ab80){fail(4);return;}
            if(!kind) {
                for(unsigned i=0;i<8;i++) {
                    Owner &o=owners[i];
                    if(__atomic_load_n(&o.ready,__ATOMIC_ACQUIRE)==1 && o.control==weak){__atomic_store_n(&active,i+1,__ATOMIC_RELEASE);return;}
                    if(!o.ready) {
                        retain(weak);o.control=weak;
                        __atomic_store_n(&o.ready,1,__ATOMIC_RELEASE);
                        __atomic_store_n(&active,i+1,__ATOMIC_RELEASE);inc(CAPTURE);return;
                    }
                }
                inc(OVERFLOW);fail(5);return;
            }
            __atomic_store_n(counters+12,1,__ATOMIC_RELAXED);
            if(!mode_current(mode,1)||deck>=4||now()-stamp>=clock_hz()*2) {inc(SKIPPED);return;}
            __atomic_store_n(counters+12,2,__ATOMIC_RELAXED);
            if(api->view()!=AZ_VIEW_BROWSE||!track_row(self)||!mode_current(mode,1)) {inc(SKIPPED);return;}
            ep<void(*)(void*,unsigned)>(0x2093c18)(self,deck); /* full native command, ordinary validation */
            inc(INVOKED); /* command call evidence only; AZ_DECK_LOADED is separate */
        }catch(...){fail(6);}
    }
};
static_assert(sizeof(Message)==48,"native JUCE message header plus prototype payload");

extern "C" void bridge_init(const az_core_v1 *core){api=core;}
extern "C" void bridge_construct(void *self) noexcept {
    try {
        if(!healthy())return;
        /* Constructor hook runs while the listener is owned. Its own normal
         * postMessage helper creates and retains its canonical weak reference.
         * The queued capture runs after construction, before any SELECT load. */
        Message *m=new(std::nothrow) Message(0,0,0);
        if(!m){fail(7);return;}
        bool ok=ep<bool(*)(void*,void*)>(0x2503280)((char*)self+0x30,m);
        inc(POSTED);if(!ok)inc(REJECTED); /* post owns/destroys m even on rejection */
    }catch(...){fail(8);}
}
extern "C" void bridge_request(unsigned deck) {
    uint64_t mode=mode_snapshot();
    if(!mode_current(mode,1)||deck>=4||!healthy()||api->view()!=AZ_VIEW_BROWSE)return;
    __atomic_store_n(counters+SELECT_THREAD,(uintptr_t)__builtin_thread_pointer(),__ATOMIC_RELAXED);
    inc(REQUEST);
    unsigned i=__atomic_fetch_add(&cursor,1,__ATOMIC_RELAXED)%16,zero=0;
    Job &j=mailbox[i];
    if(!__atomic_compare_exchange_n(&j.state,&zero,2,0,__ATOMIC_ACQUIRE,__ATOMIC_RELAXED)){inc(OVERFLOW);return;}
    j.deck=deck;j.stamp=now();j.mode=mode;__atomic_store_n(&j.state,1,__ATOMIC_RELEASE);
}
extern "C" void bridge_stats(uint64_t *v){for(unsigned i=0;i<19;i++)v[i]=__atomic_load_n(counters+i,__ATOMIC_ACQUIRE);}
extern "C" void *bridge_worker(void*) noexcept {
    uint64_t hz;asm volatile("mrs %0,cntfrq_el0":"=r"(hz));
        __atomic_store_n(&frequency,hz,__ATOMIC_RELEASE);
    const timespec pause={0,20000000};
    try {
        while(healthy()) {
            for(unsigned i=0;i<16;i++) {
                Job &j=mailbox[i];unsigned one=1;
                if(!__atomic_compare_exchange_n(&j.state,&one,2,0,__ATOMIC_ACQUIRE,__ATOMIC_RELAXED))continue;
                unsigned d=j.deck;uint64_t t=j.stamp,mode=j.mode;__atomic_store_n(&j.state,0,__ATOMIC_RELEASE);
                unsigned a=__atomic_load_n(&active,__ATOMIC_ACQUIRE);
                if(!mode_current(mode,1)||!a||a>8||now()-t>=clock_hz()*2){inc(SKIPPED);continue;}
                Owner &o=owners[a-1];
                Message *m=new(std::nothrow) Message(1,d,t,mode);
                if(!m){fail(9);break;}
                retain(o.control);m->weak=o.control;
                bool ok=ep<bool(*)(void*)>(0x25030e0)(m);
                inc(POSTED);if(!ok)inc(REJECTED);
            }
            nanosleep(&pause,nullptr);
        }
    }catch(...){fail(10);}
    return nullptr;
}
