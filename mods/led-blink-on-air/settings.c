/* SPDX-License-Identifier: GPL-3.0-only */
#include "settings.h"
#include "mod-id.h"
/* Native LOAD and SELECT updates may run on different threads. */
static unsigned enabled[2];
static void changed(int value,void *user) {
    if((user==&enabled[0] || user==&enabled[1]) && (value==0 || value==1))
        __atomic_store_n((unsigned *)user,(unsigned)value,__ATOMIC_RELEASE);
}
int blink_load_enabled(void){return (int)__atomic_load_n(&enabled[0],__ATOMIC_ACQUIRE);}
int blink_select_enabled(void){return (int)__atomic_load_n(&enabled[1],__ATOMIC_ACQUIRE);}
int blink_settings_init(const az_core_v1 *core) {
    for(unsigned i=0;i<2;i++)__atomic_store_n(&enabled[i],0,__ATOMIC_RELEASE);
    if(!AZ_HAS(core,register_row)||!AZ_HAS(core,row_value))return 1;
    static const az_row_v1 group={.size=sizeof(az_row_v1),.key="led-controls",.label="LED Blink On Air",
        .kind=AZ_ROW_GROUP,.help="Independently enable LOAD and SELECT LED blinking. Changes apply without Reload."};
    static const az_row_v1 rows[]={
        {.size=sizeof(az_row_v1),.key="blink-load-leds",.label="Blink LOAD LEDs",
         .kind=AZ_ROW_TOGGLE,.flags=AZ_ROW_DEFAULT_ON,.parent="led-controls",
         .help="Blink LOAD LEDs bright/off at 2Hz while ON AIR. Off restores native LOAD indication. Default: On.",
         .on_change=changed,.user=&enabled[0]},
        {.size=sizeof(az_row_v1),.key="blink-select-leds",.label="Blink SELECT LEDs",
         .kind=AZ_ROW_TOGGLE,.flags=AZ_ROW_DEFAULT_ON,.parent="led-controls",
         .help="Blink SELECT LEDs at 2Hz while ON AIR: selected bright/dim, unselected dim/off. Default: On.",
         .on_change=changed,.user=&enabled[1]}
    };
    if(core->register_row(MOD_ID,&group)<0)return 2;
    int values[2];
    for(unsigned i=0;i<2;i++) {
        int h=core->register_row(MOD_ID,&rows[i]);if(h<0)return 2;
        values[i]=core->row_value(h);if(values[i]!=0 && values[i]!=1)return 3;
    }
    for(unsigned i=0;i<2;i++)changed(values[i],&enabled[i]);
    return 0;
}
