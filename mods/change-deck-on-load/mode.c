/* SPDX-License-Identifier: GPL-3.0-only */
#include "mode.h"
#include "mod-id.h"
static const az_core_v1 *api;
/* Low bits are independent feature gates, high bits a cancellation generation.
 * Row callbacks are single-writer on the SDK UI thread; workers only read. */
static uint64_t state;
static unsigned active;
static unsigned feature[2]={0,1};
static void changed(int on,void *user) {
    unsigned bit=*(const unsigned *)user;
    if((on!=0 && on!=1) || bit>1)return;
    uint64_t previous=__atomic_load_n(&state,__ATOMIC_RELAXED);
    uint64_t flags=(previous&3u)&~(UINT64_C(1)<<bit);
    uint64_t next=((previous+4)&~UINT64_C(3))|flags|((uint64_t)on<<bit);
    __atomic_store_n(&state,next,__ATOMIC_RELEASE);
    api->log(MOD_ID,"settings load-to-select=%u select-to-load=%u generation=%lu",
        (unsigned)(next&1u),(unsigned)((next>>1)&1u),(unsigned long)(next>>2));
}
int mode_init(const az_core_v1 *core) {
    __atomic_store_n(&active,0,__ATOMIC_RELEASE);
    if(!AZ_HAS(core,register_row)||!AZ_HAS(core,row_value)||!AZ_HAS(core,log))return 1;
    api=core;
    static const az_row_v1 group={.size=sizeof(az_row_v1),.key="controls",.label="Change Deck On Load",
        .kind=AZ_ROW_GROUP,.help="Enable either loading shortcut, both, or neither. Changes cancel queued shortcuts."};
    static const az_row_v1 rows[]={
        {.size=sizeof(az_row_v1),.key="select-deck-on-load",.label="Select Deck on Load",.kind=AZ_ROW_TOGGLE,
         .flags=AZ_ROW_DEFAULT_ON,.parent="controls",
         .help="Select the target deck after a successful native track load. Default: On.",.on_change=changed,.user=&feature[0]},
        {.size=sizeof(az_row_v1),.key="load-on-deck-select",.label="Load Track on Deck Select",.kind=AZ_ROW_TOGGLE,
         .parent="controls",.help="SELECT loads the highlighted track only in the track list. Elsewhere SELECT stays native. Default: Off.",
         .on_change=changed,.user=&feature[1]}
    };
    if(core->register_row(MOD_ID,&group)<0)return 2;
    unsigned flags=0;
    for(unsigned i=0;i<2;i++) {
        int handle=core->register_row(MOD_ID,&rows[i]);if(handle<0)return 3;
        int value=core->row_value(handle);if(value!=0 && value!=1)return 4;
        flags|=(unsigned)value<<i;
    }
    __atomic_store_n(&state,flags,__ATOMIC_RELEASE);
    return 0;
}
void mode_activate(void){__atomic_store_n(&active,1,__ATOMIC_RELEASE);}
uint64_t mode_snapshot(void){return __atomic_load_n(&state,__ATOMIC_ACQUIRE);}
int mode_current(uint64_t snapshot,unsigned desired) {
    return desired<2 && __atomic_load_n(&active,__ATOMIC_ACQUIRE) &&
        (snapshot&(UINT64_C(1)<<desired)) && mode_snapshot()==snapshot;
}
