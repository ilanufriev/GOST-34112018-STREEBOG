// Copyright 2025, Anufriev Ilia, anufriewwi@rambler.ru
// SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later

#include "gost34112018_common.h"
#include "gost34112018_types.h"
#include "gost34112018_vec512.h"
#include "gost34112018_avx2_types.h"

/**
    @brief      Vec512 addition operation. There is no way to make it better for now.
    @param      in1 - first operand.
    @param      in2 - second operand.
    @param      out - output pointer.
 */
void Vec512_Add(const union Vec512 *in1,
                const union Vec512 *in2,
                      union Vec512 *out)
{
    GostU64 carry = 0;

    log_d("Added vectors:");
    log_d("In1: ");
    DebugPrintVec(in1);
    log_d("In2: ");
    DebugPrintVec(in2);

    TimerStart(t);

    for (int i = 0; i < VEC512_QWORDS; i++)
    {
        out->qwords[i] = in1->qwords[i] + in2->qwords[i] + carry;

        if (in2->qwords[i] > (MAX_GOSTU64 - in1->qwords[i]))
        {
            carry = 1;
        }
        else
        {
            carry = 0;
        }
    }

    TimerEnd(t);

    log_d("Out: ");
    DebugPrintVec(out);
}

/**
    @brief      AVX2 Vec512 XOR operation.
    @param      in1 - first operand.
    @param      in2 - second operand.
    @param      out - output pointer.
 */
void Vec512_Xor(const union Vec512 *in1,
                const union Vec512 *in2,
                      union Vec512 *out)
{
    const union AVX2_Vec512 *avx2_in1 = (const union AVX2_Vec512 *) in1;
    const union AVX2_Vec512 *avx2_in2 = (const union AVX2_Vec512 *) in2;
    union AVX2_Vec512       *avx2_out = (union AVX2_Vec512 *)       out;

    log_d("XOR'ed vectors:");
    log_d("In1: ");
    DebugPrintVec(in1);
    log_d("In2: ");
    DebugPrintVec(in2);

    TimerStart(t);

    avx2_out->m256is[0] = _mm256_xor_si256(avx2_in1->m256is[0], avx2_in2->m256is[0]);
    avx2_out->m256is[1] = _mm256_xor_si256(avx2_in1->m256is[1], avx2_in2->m256is[1]);

    TimerEnd(t);

    log_d("Out: ");
    DebugPrintVec(out);
}

/**
    @brief       Uint64 to Vec512 conversion function.
    @param       x - conversion operand.
    @param       out - output pointer.
 */
void Uint64ToVec512(const GostU64 x, union Vec512 *out)
{
    union AVX2_Vec512 *avx2_out = (union AVX2_Vec512 *) out;
    const GostU32 le = (x >> 0 ) & 0xffffffff;
    const GostU32 be = (x >> 32) & 0xffffffff;

    TimerStart(t);

    avx2_out->m256is[0] = _mm256_setr_epi32(le, be, 0, 0, 0, 0, 0, 0);
    avx2_out->m256is[1] = _mm256_set1_epi32(0);

    TimerEnd(t);
}

#ifdef __ENABLE_DEBUG_OUTPUT__
void DebugPrintVec(const union Vec512 *vec)
{
    printf("    ");
    for (int i = 0; i < VEC512_BYTES; i++)
    {
        if (i != 0 && i % 8 == 0)
            printf(" ");
        printf("%02hhx", vec->bytes[i]);
    }

    printf("\n");
    fflush(stdout);
}
#else
void DebugPrintVec(const union Vec512 *out)
{

}
#endif // DEBUG
