/* SPDX-License-Identifier: GPL-3.0-only
 * Genuine native toggles, without synthetic radio-button synchronization.
 * The native layout is latched at init. Saving never changes running decks.
 */
#include "profile.h"
#include "mod-id.h"
static const az_core_v1 *api;
static unsigned active;
static int enable_handle,slot_handle,pending;
unsigned profile_current(void){return active;}
static int available(void *unused){(void)unused;return api->row_value(enable_handle)==1;}
static void changed(int value,void *unused){
 (void)unused;if(value==0||value==1)pending=1;
}
void profile_tick(void){
 if(!pending)return;
 pending=0;
 int enabled=api->row_value(enable_handle),four=api->row_value(slot_handle);
 /* Keep the older logical keys for rollback/migration, not UI control.
  * The actual controls save their own values through Core's normal path.
  */
 api->cfg_set(MOD_ID,"three_plus_one",enabled);
 api->cfg_set(MOD_ID,"external_deck",four?4:3);
 api->log(MOD_ID,"Saved 3+1=%d; external deck=%d; active profile=%u; Reload required",enabled,four?4:3,active);
}
int profile_init(const az_core_v1 *core){
 if(!AZ_HAS(core,register_row)||!AZ_HAS(core,row_value)||!AZ_HAS(core,log)||
    !AZ_HAS(core,cfg_get)||!AZ_HAS(core,cfg_set))return 1;
 api=core;active=0;pending=0;
 static const az_row_v1 group={.size=sizeof(az_row_v1),.key="link-layout",.label="4 Deck Pro DJ Link Plus",.kind=AZ_ROW_GROUP,
  .help="Choose the external slot, then Reload. LINK Off always keeps four internal decks."};
 static const az_row_v1 enabled={.size=sizeof(az_row_v1),.key="three-plus-one",.label="3 Internal + 1 External",.kind=AZ_ROW_TOGGLE,.parent="link-layout",
  .help="Off: original 2+2. On: three internal decks and the selected external player. Apply with Reload.",.on_change=changed};
 az_row_v1 slot={.size=sizeof(az_row_v1),.key="external-is-four",.label="External Deck 4 (off: Deck 3)",.kind=AZ_ROW_TOGGLE,.parent="link-layout",
  .help="Off: external 3, internal 1/2/4. On: external 4, internal 1/2/3. Apply with Reload.",.available=available,.on_change=changed};
 /* The old selected number seeds only a previously unset native toggle. */
 if(core->cfg_get(MOD_ID,"external_deck",3)==4)slot.flags=AZ_ROW_DEFAULT_ON;
 if(core->register_row(MOD_ID,&group)<0)return 2;
 enable_handle=core->register_row(MOD_ID,&enabled);if(enable_handle<0)return 3;
 slot_handle=core->register_row(MOD_ID,&slot);if(slot_handle<0)return 4;
 int enabled_value=core->row_value(enable_handle),four=core->row_value(slot_handle);
 if(enabled_value<0||enabled_value>1||four<0||four>1)return 5;
 active=enabled_value?(four?2u:1u):0u;
 pending=1;
 core->log(MOD_ID,"Active external profile=%u; saved 3+1=%d, external deck=%d; applied until Reload",active,enabled_value,four?4:3);
 return 0;
}
