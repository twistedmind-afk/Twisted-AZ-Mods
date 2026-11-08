/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef TWISTED_FIRMWARE_CHECK_H
#define TWISTED_FIRMWARE_CHECK_H
#include <stddef.h>
#include <stdint.h>
/* A bounded init-time fingerprint; firmware bytes are never distributed. */
static inline int firmware_matches(uintptr_t address,size_t length,uint64_t expected) {
 const unsigned char *p=(const unsigned char *)address;
 uint64_t hash=UINT64_C(0xcbf29ce484222325);
 for(size_t i=0;i<length;i++)hash=(hash^p[i])*UINT64_C(0x100000001b3);
 return hash==expected;
}
#endif
