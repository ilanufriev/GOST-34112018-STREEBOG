// Copyright 2025, Anufriev Ilia, anufriewwi@rambler.ru
// SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later

#ifndef __GOST34112018_COMMON_H__
#define __GOST34112018_COMMON_H__

#ifdef DEBUG
    #include "stdio.h"
    #define log_d(__fmt, ...) \
        printf("GOST34112018 [DEBUG] %s: " __fmt "\n", __func__,##__VA_ARGS__)
#else
    #define log_d(__fmd, ...) ;
#endif // DEBUG

#include "gost34112018_vec512.h"

/**
    @brief      Context of the algorithm, as described in ch. 8.1 of the Standard.
 */
struct GOST34112018_Context
{
    union Vec512 h;
    union Vec512 N;
    union Vec512 sigma;
};

/**
    @brief      S-box, as defined in the chapter 5.1 of The Standard. It is used for
                S-transformation.
 */
extern const GostU8 PI[256];

/**
    @brief      A byte shuffle table, as defined in the ch. 5.3 of The Standard. It is
                used for the P transformation.
 */
extern const GostU8 TAU[64];

enum
{
    A_SIZE = 64,
    C_SIZE = 12,
};

/**
    @brief      Matrix A, as defined in the ch. 5.4 of The Standard. It is used for the L
                transformation.
*/
extern const GostU64 A[64];

/**
    @brief      Iteration constants, that are used in the encryption function of the
                algorithm.
    @note       All constants are Little-Endian unsigned 512-bit numbers, which means that,
                for example, C1.bytes[0] is LSB and C1.bytes[63] is MSB.
 */
extern const union Vec512 *C[];

/**
    @brief      Initialization vector for the 256-bit long digest, as defined in ch. 5.1
                of The Standard.
*/
extern const union Vec512 INIT_VECTOR_256;

/**
    @brief      Initialization vector for the 512-bit long digest, as defined in ch. 5.1
                of The Standard.
*/
extern const union Vec512 INIT_VECTOR_512;

extern const union Vec512 ZERO_VECTOR_512;

#endif // __GOST34112018_COMMON_H__
