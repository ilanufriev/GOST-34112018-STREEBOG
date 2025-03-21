// Copyright 2025, Anufriev Ilia, anufriewwi@rambler.ru
// SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later

#ifndef __GOST34112018_AVX2_TYPES_H__
#define __GOST34112018_AVX2_TYPES_H__

#include "immintrin.h"
#include "gost34112018.h"
#include "gost34112018_types.h"

/**
    @brief      Internal AVX2-capable Vec512 type.
 */
union GOST34112018_AlignAttribute(32) AVX2_Vec512
{
    __m256i m256is[2];
    GostU8  bytes [VEC512_BYTES];
    GostU16 words [VEC512_WORDS];
    GostU32 dwords[VEC512_DWORDS];
    GostU64 qwords[VEC512_QWORDS];
};

#endif // __GOST34112018_AVX2_TYPES_H__