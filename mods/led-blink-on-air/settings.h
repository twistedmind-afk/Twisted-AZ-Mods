/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef TWISTED_BLINK_SETTINGS_H
#define TWISTED_BLINK_SETTINGS_H
#include "azmod-mod.h"
#ifdef __cplusplus
extern "C" {
#endif
int blink_settings_init(const az_core_v1 *core);
int blink_load_enabled(void);
int blink_select_enabled(void);
#ifdef __cplusplus
}
#endif
#endif
