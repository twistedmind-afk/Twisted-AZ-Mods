/* SPDX-License-Identifier: GPL-3.0-only */
#include "azmod-mod.h"
#include "mod-id.h"
int link_prepare(const az_core_v1 *core);
int link_install(const az_core_v1 *core);
void link_tick(void);
static int init(const az_core_v1 *core) {
    int rc=link_prepare(core);if(rc)return 200+rc;
    return link_install(core);
}
static const char *const tags[]={"ep147-9e9454e357389ece",0};
static const az_mod_v1 mod={.abi=AZ_MOD_ABI,.size=sizeof(az_mod_v1),.id=MOD_ID,
    .fw_tags=tags,.init=init,.tick=link_tick};
AZ_MOD_EXPORT const az_mod_v1 *az_mod_v1_entry(void){return &mod;}
