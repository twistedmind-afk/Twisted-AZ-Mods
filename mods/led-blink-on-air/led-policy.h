/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef TWISTED_LED_POLICY_H
#define TWISTED_LED_POLICY_H
static inline unsigned select_led_level(unsigned enabled,unsigned air,unsigned selected,
                                        unsigned available,unsigned phase,unsigned native) {
    if(!enabled || !available || (air!=1 && air!=2) || selected>1 || phase>1)return native;
    return air==2 ? selected+phase : 1+selected;
}
#endif
