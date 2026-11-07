/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef TWISTED_LOAD_MODE_H
#define TWISTED_LOAD_MODE_H
#include "azmod-mod.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
int mode_init(const az_core_v1 *core);
void mode_activate(void);
uint64_t mode_snapshot(void);
/* Independent gates: 0 LOAD->SELECT, 1 SELECT->LOAD. Any setting change
 * invalidates previously queued work, including an OFF/ON round trip. */
int mode_current(uint64_t snapshot,unsigned desired);
#ifdef __cplusplus
}
#endif
#endif
