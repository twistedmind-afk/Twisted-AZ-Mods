/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef TWISTED_LINK_PROFILE_H
#define TWISTED_LINK_PROFILE_H
#include "azmod-mod.h"
int profile_init(const az_core_v1 *);
unsigned profile_current(void);
void profile_tick(void);
static inline unsigned profile_mask(unsigned on,unsigned profile) {
 return !on?15u:profile==1?11u:profile==2?7u:3u;
}
#endif
