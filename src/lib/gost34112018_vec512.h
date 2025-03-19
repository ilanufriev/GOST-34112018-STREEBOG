// Copyright 2025, Anufriev Ilia, anufriewwi@rambler.ru
// SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later

#ifndef __GOST34112018_VEC512_H__
#define __GOST34112018_VEC512_H__

#include "gost34112018_types.h"

/**
    @brief       Little-endian unsigned 512-bit number.
*/
union Vec512 {
    GostU8  bytes  [VEC512_BYTES];
    GostU16 words  [VEC512_WORDS];
    GostU32 dwords [VEC512_DWORDS];
    GostU64 qwords [VEC512_QWORDS];
};

void Vec512_Add(const union Vec512 *in1, const union Vec512 *in2, union Vec512 *out);

void Vec512_Xor(const union Vec512 *in1, const union Vec512 *in2, union Vec512 *out);

void Uint64ToVec512(const GostU64 x, union Vec512 *out);

void DebugPrintVec(const union Vec512 *out);

#endif // __GOST34112018_VEC512_H__
