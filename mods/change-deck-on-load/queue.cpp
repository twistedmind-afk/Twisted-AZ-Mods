/* SPDX-License-Identifier: GPL-3.0-only
 * EP147-9e9454e357389ece only. Native semantic selection, never ERP editing.
 * Tasks use EP147's allocator, IAsyncTask identity and ordinary queue admission.
 * A retained native weak-owner reference gates the selector's own task box.
 */
#include "azmod-mod.h"
#include "mode.h"
#include <stdint.h>
#include <stddef.h>
#include <new>
#include <time.h>
#include <string.h>
template<class F> static F ep(uintptr_t p) {return reinterpret_cast<F>(p);}
static uintptr_t token(){return (uintptr_t)__builtin_thread_pointer();}
static uint64_t now(){uint64_t n;asm volatile("mrs %0,cntvct_el0":"=r"(n));return n;}
static uint64_t frequency;
static uint64_t clock_hz(){return __atomic_load_n(&frequency,__ATOMIC_ACQUIRE);}
static uint64_t armed, submitted, ran, destroyed, error, consumer, producer;
static uint64_t selected, skipped, manual[2], latest[2], serial, captures, overflow;
static uint64_t no_ticket;
static uint64_t active_weak[2]; /* slot+1; zero is unknown */
alignas(16) static unsigned char box[0x38], name_storage[8];
struct Weak {
    unsigned ready, side;
    void *control; /* native retained reference, immutable after publication */
    uintptr_t selector, gui; /* identities; dereferenced only in native-owned task */
};
static Weak weak[8];
struct Job {uint64_t serial, stamp, manual, mode; unsigned deck, slot;};
struct Mail {unsigned state; Job job;};
static Mail mailbox[16];
static unsigned mail_cursor;
struct NativeString {
    uintptr_t data;
    explicit NativeString(const char *s){ep<void(*)(void*,const char*)>(0x24a1180)(this,s);}
    NativeString(const NativeString &s) noexcept {ep<void(*)(void*,const void*)>(0x247d310)(this,&s);}
    ~NativeString() noexcept {ep<void(*)(void*)>(0x2469aa0)(this);}
};
static_assert(sizeof(NativeString)==8,"native String ABI");
static bool current(const Job &j) {
    unsigned side=j.deck&1u;
    return mode_current(j.mode,0) && j.deck<4 && !__atomic_load_n(&error,__ATOMIC_ACQUIRE) &&
        __atomic_load_n(&latest[side],__ATOMIC_ACQUIRE)==j.serial &&
        __atomic_load_n(&manual[side],__ATOMIC_ACQUIRE)==j.manual &&
        now()-j.stamp<clock_hz()*2;
}
static void execute(unsigned kind,const Job &j);
class Task {
public:
    uint32_t identity, padding;
    uint64_t sequence;
    unsigned kind;
    Job job;
    Task(unsigned k,const Job &j):identity(ep<uint32_t(*)(void)>(0x7305f8)()),padding(0),sequence(0),kind(k),job(j){}
    virtual ~Task() noexcept {identity=0;__atomic_fetch_add(&destroyed,1,__ATOMIC_RELAXED);}
    virtual void run() noexcept {
        __atomic_store_n(&consumer,token(),__ATOMIC_RELAXED);
        __atomic_fetch_add(&ran,1,__ATOMIC_RELAXED);
        try {execute(kind,job);} catch(...) {__atomic_store_n(&error,20,__ATOMIC_RELEASE);}
    }
    virtual NativeString name() const noexcept {return NativeString(*reinterpret_cast<const NativeString*>(name_storage));}
};
static bool post(void *owner,unsigned kind,const Job &job) {
    void *allocator=*reinterpret_cast<void**>((char*)owner+8);
    void **vt=*reinterpret_cast<void***>(allocator);
    void *memory=reinterpret_cast<void*(*)(void*,size_t,size_t)>(vt[2])(allocator,sizeof(Task),8);
    if(!memory){__atomic_store_n(&error,4,__ATOMIC_RELEASE);return false;}
    Task *t=new(memory) Task(kind,job);
    t->sequence=__atomic_add_fetch(reinterpret_cast<uint64_t*>((char*)owner+0x20),1,__ATOMIC_RELAXED);
    uintptr_t ticket=ep<uintptr_t(*)(void*,Task*)>(0x23636d0)(owner,t);
    /* Zero is NOT an admission result. The native dispatcher can execute and
     * destroy the task synchronously, then return no outstanding ticket. Never
     * access t after submission; the ordinary native guards remain unchanged.
     * Delivery is evidenced separately by ran/selected/native state telemetry. */
    if(!ticket)__atomic_fetch_add(&no_ticket,1,__ATOMIC_RELAXED);
    __atomic_fetch_add(&submitted,1,__ATOMIC_RELAXED);
    return true;
}
static void execute(unsigned kind,const Job &job) {
    if(!current(job)){__atomic_fetch_add(&skipped,1,__ATOMIC_RELAXED);return;}
    unsigned side=job.deck&1u;
    if(!kind) {
        uint64_t slot=__atomic_load_n(&active_weak[side],__ATOMIC_ACQUIRE);
        if(!slot || slot>8){__atomic_fetch_add(&skipped,1,__ATOMIC_RELAXED);return;}
        Weak &w=weak[slot-1];
        /* Retaining this native reference owns the control object. Its target is
         * read on EP147's task thread, exactly as the native envelope dispatcher
         * at 0x23606b0 does; a destroyed box clears that target first. */
        uintptr_t master=*((uintptr_t*)w.control+3);
        if(!master || master!=w.selector+0x80 || *(uintptr_t*)master!=0x32174c0) {
            __atomic_fetch_add(&skipped,1,__ATOMIC_RELAXED);return;
        }
        Job inner=job;inner.slot=(unsigned)(slot-1);
        /* Native submission creates the ordinary envelope bound to the live
         * selector's box. Native destruction suppresses delivery to a dead owner. */
        post((void*)(master-0x28),1,inner);
        return;
    }
    if(job.slot>=8 || __atomic_load_n(&active_weak[side],__ATOMIC_ACQUIRE)!=job.slot+1) {
        __atomic_fetch_add(&skipped,1,__ATOMIC_RELAXED);return;
    }
    Weak &w=weak[job.slot];
    /* This task is executing through this owner's native weak-gated box. */
    const uintptr_t *s=(const uintptr_t*)w.selector;
    if(s[0]!=0x31b9db0){__atomic_store_n(&error,21,__ATOMIC_RELEASE);return;}
    const unsigned char *a=(const unsigned char*)s[6], *b=(const unsigned char*)s[7];
    if(!a||!b||*(const uintptr_t*)a!=0x31ba738||*(const uintptr_t*)b!=0x31ba738||
       a[12]!=side+1||b[12]!=side+3) {__atomic_store_n(&error,22,__ATOMIC_RELEASE);return;}
    unsigned choice=job.deck>=2;
    if(!(choice?b:a)[0x1d] || !current(job)) {__atomic_fetch_add(&skipped,1,__ATOMIC_RELAXED);return;}
    ep<void(*)(void*,unsigned)>(0x21a7a20)((void*)s,choice);
    if(!(choice?b:a)[0x1c] || (choice?a:b)[0x1c]) {
        __atomic_store_n(&error,24,__ATOMIC_RELEASE);return;
    }
    __atomic_fetch_add(&selected,1,__ATOMIC_RELAXED);
}
extern "C" void queue_arm(void){__atomic_store_n(&armed,1,__ATOMIC_RELEASE);}
extern "C" void queue_capture(void *self) noexcept {
    try {
        const uintptr_t *s=(const uintptr_t*)self;
        if(s[0]!=0x31b9db0)return;
        const unsigned char *a=(const unsigned char*)s[6], *b=(const unsigned char*)s[7];
        if(!a||!b||*(const uintptr_t*)a!=0x31ba738||*(const uintptr_t*)b!=0x31ba738)return;
        unsigned side=a[12]==1&&b[12]==3?0:a[12]==2&&b[12]==4?1:2;
        if(side>1)return;
        void *master=(char*)self+0x80;
        /* Do not create a native weak control block from a busy hook. The
         * original setting callback already submitted its normal task. Copying
         * an existing reference only retains it; slots are never replaced/freed. */
        void *control=*((void**)master+1);
        if(!control)return;
        uintptr_t gui=0;
        uintptr_t begin=s[8],end=s[9];
        if(!begin || (begin&7) || end<begin || end-begin>64)return;
        for(uintptr_t p=begin;p<end;p+=8) {
            uintptr_t listener=*(const uintptr_t*)p;
            if(listener && *(const uintptr_t*)listener==0x31bb9b8)gui=listener;
        }
        if(!gui)return;
        for(unsigned i=0;i<8;i++) {
            Weak &w=weak[i];
            unsigned state=__atomic_load_n(&w.ready,__ATOMIC_ACQUIRE);
            if(state==1 && w.selector==(uintptr_t)self && w.control==control)return;
            unsigned zero=0;
            if(state==0 && __atomic_compare_exchange_n(&w.ready,&zero,2,0,__ATOMIC_ACQUIRE,__ATOMIC_RELAXED)) {
                ep<void(*)(void*,void*)>(0x2367b40)(&w.control,master);
                w.selector=(uintptr_t)self;w.gui=gui;w.side=side;
                __atomic_store_n(&w.ready,1,__ATOMIC_RELEASE);
                __atomic_store_n(&active_weak[side],i+1,__ATOMIC_RELEASE);
                __atomic_fetch_add(&captures,1,__ATOMIC_RELAXED);
                queue_arm();return;
            }
        }
        __atomic_fetch_add(&overflow,1,__ATOMIC_RELAXED);
    }catch(...){__atomic_store_n(&error,23,__ATOMIC_RELEASE);}
}
extern "C" void queue_manual(void *gui,uintptr_t caller) {
    if(caller!=0x21a76c0 && caller!=0x21a7710)return;
    for(unsigned i=0;i<8;i++) {
        Weak &w=weak[i];
        if(__atomic_load_n(&w.ready,__ATOMIC_ACQUIRE)==1 && w.gui==(uintptr_t)gui) {
            __atomic_fetch_add(&manual[w.side],1,__ATOMIC_RELEASE);return;
        }
    }
}
extern "C" void queue_loaded(unsigned deck) {
    uint64_t mode=mode_snapshot();
    if(deck>=4 || !mode_current(mode,0))return;
    unsigned side=deck&1u;
    Job j={__atomic_add_fetch(&serial,1,__ATOMIC_RELAXED),now(),
        __atomic_load_n(&manual[side],__ATOMIC_ACQUIRE),mode,deck,0};
    __atomic_store_n(&latest[side],j.serial,__ATOMIC_RELEASE);
    unsigned index=__atomic_fetch_add(&mail_cursor,1,__ATOMIC_RELAXED)%16;
    Mail &m=mailbox[index];unsigned zero=0;
    if(!__atomic_compare_exchange_n(&m.state,&zero,2,0,__ATOMIC_ACQUIRE,__ATOMIC_RELAXED)) {
        __atomic_fetch_add(&overflow,1,__ATOMIC_RELAXED);return;
    }
    m.job=j;__atomic_store_n(&m.state,1,__ATOMIC_RELEASE);
}
extern "C" void queue_stats(uint64_t *v) {
    uint64_t *fields[]={&submitted,&ran,&destroyed,&error,&producer,&consumer,&selected,&skipped,&captures,&manual[0],&manual[1],&overflow};
    for(unsigned i=0;i<12;i++)v[i]=__atomic_load_n(fields[i],__ATOMIC_ACQUIRE);
    v[12]=(__atomic_load_n(&active_weak[0],__ATOMIC_ACQUIRE)?1u:0u)|
          (__atomic_load_n(&active_weak[1],__ATOMIC_ACQUIRE)?2u:0u);
    v[13]=__atomic_load_n(&no_ticket,__ATOMIC_ACQUIRE);
}
extern "C" void *queue_worker(void*) noexcept {
    try {
        uint64_t hz;asm volatile("mrs %0,cntfrq_el0":"=r"(hz));
        __atomic_store_n(&frequency,hz,__ATOMIC_RELEASE);
        const timespec pause={0,20000000};
        while(!__atomic_load_n(&armed,__ATOMIC_ACQUIRE))nanosleep(&pause,nullptr);
        __atomic_store_n(&producer,token(),__ATOMIC_RELEASE);
        ep<void(*)(void*)>(0x235f630)(box);
        new(name_storage) NativeString("Local LOAD to native SELECT");
        while(!__atomic_load_n(&error,__ATOMIC_ACQUIRE)) {
            for(unsigned i=0;i<16;i++) {
                Mail &m=mailbox[i];unsigned one=1;
                if(!__atomic_compare_exchange_n(&m.state,&one,2,0,__ATOMIC_ACQUIRE,__ATOMIC_RELAXED))continue;
                Job j=m.job;__atomic_store_n(&m.state,0,__ATOMIC_RELEASE);
                if(current(j))post(box,0,j);else __atomic_fetch_add(&skipped,1,__ATOMIC_RELAXED);
            }
            nanosleep(&pause,nullptr);
        }
    }catch(...){__atomic_store_n(&error,3,__ATOMIC_RELEASE);}
    return nullptr;
}

/* Map a known native GUI listener without dereferencing a cached owner. */
extern "C" int queue_gui_deck(void *gui,unsigned choice) {
    for(unsigned i=0;i<8;i++) {
        Weak &w=weak[i];
        if(__atomic_load_n(&w.ready,__ATOMIC_ACQUIRE)==1 && w.gui==(uintptr_t)gui)
            return (int)(w.side+(choice?2u:0u));
    }
    return -1;
}
