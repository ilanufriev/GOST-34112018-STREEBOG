// Copyright 2025, Anufriev Ilia, anufriewwi@rambler.ru
// SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later

#ifndef __GOST34112018_TYPES_H__
#define __GOST34112018_TYPES_H__

#define MAX_GOSTU64 0xFFFFFFFFFFFFFFFF

#define GostNull ((void *)0)

enum GOST_34112018_CONSTANTS
{
    BYTE_SIZE     = (1 * 8), // bits in byte
    WORD_SIZE     = (2 * 8), // bits in word (16-bit integer)
    DWORD_SIZE    = (4 * 8), // bits in double-word (32-bit integer)
    QWORD_SIZE    = (8 * 8), // bits in quad-word (64-bit integer)

    VEC512_BYTES  = 512 / BYTE_SIZE,
    VEC512_WORDS  = 512 / WORD_SIZE,
    VEC512_DWORDS = 512 / DWORD_SIZE,
    VEC512_QWORDS = 512 / QWORD_SIZE,

    VEC512_SIZE   = VEC512_BYTES,

    BLOCK_SIZE    = 512 / BYTE_SIZE,
};

typedef unsigned long long   GostU64;
typedef unsigned int         GostU32;
typedef unsigned short       GostU16;
typedef unsigned char        GostU8;

typedef long long            GostI64;
typedef int                  GostI32;
typedef short                GostI16;
typedef char                 GostI8;

typedef enum { false, true } GostBool;

#endif // __GOST34112018_TYPES_H__
