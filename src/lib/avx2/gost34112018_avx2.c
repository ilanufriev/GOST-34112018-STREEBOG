// Copyright 2025, Anufriev Ilia, anufriewwi@rambler.ru
// SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later

#include "gost34112018.h"
#include "gost34112018_avx2_types.h"
#include "gost34112018_optimized_precomp.h"
#include "gost34112018_common.h"
#include "gost34112018_interface.h"

/**
    @brief       X transformation of the algorithm as defined in the ch. 6 of the Standard.
    @param       a - argument 'a', according to the standard.
    @param       k - argument 'k', according to the standard.
    @param       out - output pointer.
 */
static
void XTransform(const union Vec512 *a, const union Vec512 *k, union Vec512 *out)
{
    log_d("X transformation:");
    log_d("a: ");
    DebugPrintVec(a);
    log_d("K: ");
    DebugPrintVec(k);

    TimerStart(t);

    Vec512_Xor(a, k, out);

    TimerEnd(t);

    log_d("Out: ");
    DebugPrintVec(out);
}

/**
    @brief      P transformation of the algorithm as defined in the ch. 6 of the Standard.
    @param      a - argument 'a', according to The Standard.
    @param      out - output pointer.
 */
static
void PTransform(const union Vec512 *a, union Vec512 *out)
{
    union AVX2_Vec512 *avx2_out = (union AVX2_Vec512 *) out;

    log_d("P transformation:");
    log_d("a: ");
    DebugPrintVec(a);

    TimerStart(t);

    avx2_out->m256is[0] = _mm256_setr_epi8(
            a->bytes[TAU[ 0]], a->bytes[TAU[ 1]], a->bytes[TAU[ 2]], a->bytes[TAU[ 3]],
            a->bytes[TAU[ 4]], a->bytes[TAU[ 5]], a->bytes[TAU[ 6]], a->bytes[TAU[ 7]],
            a->bytes[TAU[ 8]], a->bytes[TAU[ 9]], a->bytes[TAU[10]], a->bytes[TAU[11]],
            a->bytes[TAU[12]], a->bytes[TAU[13]], a->bytes[TAU[14]], a->bytes[TAU[15]],
            a->bytes[TAU[16]], a->bytes[TAU[17]], a->bytes[TAU[18]], a->bytes[TAU[19]],
            a->bytes[TAU[20]], a->bytes[TAU[21]], a->bytes[TAU[22]], a->bytes[TAU[23]],
            a->bytes[TAU[24]], a->bytes[TAU[25]], a->bytes[TAU[26]], a->bytes[TAU[27]],
            a->bytes[TAU[28]], a->bytes[TAU[29]], a->bytes[TAU[30]], a->bytes[TAU[31]]
        );

    avx2_out->m256is[1] = _mm256_setr_epi8(
            a->bytes[TAU[32]], a->bytes[TAU[33]], a->bytes[TAU[34]], a->bytes[TAU[35]],
            a->bytes[TAU[36]], a->bytes[TAU[37]], a->bytes[TAU[38]], a->bytes[TAU[39]],
            a->bytes[TAU[40]], a->bytes[TAU[41]], a->bytes[TAU[42]], a->bytes[TAU[43]],
            a->bytes[TAU[44]], a->bytes[TAU[45]], a->bytes[TAU[46]], a->bytes[TAU[47]],
            a->bytes[TAU[48]], a->bytes[TAU[49]], a->bytes[TAU[50]], a->bytes[TAU[51]],
            a->bytes[TAU[52]], a->bytes[TAU[53]], a->bytes[TAU[54]], a->bytes[TAU[55]],
            a->bytes[TAU[56]], a->bytes[TAU[57]], a->bytes[TAU[58]], a->bytes[TAU[59]],
            a->bytes[TAU[60]], a->bytes[TAU[61]], a->bytes[TAU[62]], a->bytes[TAU[63]]
        );

    TimerEnd(t);

    log_d("Out: ");
    DebugPrintVec(out);
}

/**
    @brief      Accelerated combined transformations (S + L) with use of precomputed
                lookup-table.
    @param      a - argument 'a', according to The Standard.
    @param      out - output pointer.
 */
static
void SLCombinedTransform(const union Vec512 *a, union Vec512 *out)
{
    union AVX2_Vec512 *avx2_out = (union AVX2_Vec512 *) out;

    log_d("PL transformation:");
    log_d("a: ");
    DebugPrintVec(a);

    TimerStart(t);
    avx2_out->m256is[0] = _mm256_setr_epi64x(
            SL_transform_precomp[0][a->bytes[ 0]] ^
            SL_transform_precomp[1][a->bytes[ 1]] ^
            SL_transform_precomp[2][a->bytes[ 2]] ^
            SL_transform_precomp[3][a->bytes[ 3]] ^
            SL_transform_precomp[4][a->bytes[ 4]] ^
            SL_transform_precomp[5][a->bytes[ 5]] ^
            SL_transform_precomp[6][a->bytes[ 6]] ^
            SL_transform_precomp[7][a->bytes[ 7]],

            SL_transform_precomp[0][a->bytes[ 8]] ^
            SL_transform_precomp[1][a->bytes[ 9]] ^
            SL_transform_precomp[2][a->bytes[10]] ^
            SL_transform_precomp[3][a->bytes[11]] ^
            SL_transform_precomp[4][a->bytes[12]] ^
            SL_transform_precomp[5][a->bytes[13]] ^
            SL_transform_precomp[6][a->bytes[14]] ^
            SL_transform_precomp[7][a->bytes[15]],

            SL_transform_precomp[0][a->bytes[16]] ^
            SL_transform_precomp[1][a->bytes[17]] ^
            SL_transform_precomp[2][a->bytes[18]] ^
            SL_transform_precomp[3][a->bytes[19]] ^
            SL_transform_precomp[4][a->bytes[20]] ^
            SL_transform_precomp[5][a->bytes[21]] ^
            SL_transform_precomp[6][a->bytes[22]] ^
            SL_transform_precomp[7][a->bytes[23]],

            SL_transform_precomp[0][a->bytes[24]] ^
            SL_transform_precomp[1][a->bytes[25]] ^
            SL_transform_precomp[2][a->bytes[26]] ^
            SL_transform_precomp[3][a->bytes[27]] ^
            SL_transform_precomp[4][a->bytes[28]] ^
            SL_transform_precomp[5][a->bytes[29]] ^
            SL_transform_precomp[6][a->bytes[30]] ^
            SL_transform_precomp[7][a->bytes[31]]
        );

    avx2_out->m256is[1] = _mm256_setr_epi64x(
            SL_transform_precomp[0][a->bytes[32]] ^
            SL_transform_precomp[1][a->bytes[33]] ^
            SL_transform_precomp[2][a->bytes[34]] ^
            SL_transform_precomp[3][a->bytes[35]] ^
            SL_transform_precomp[4][a->bytes[36]] ^
            SL_transform_precomp[5][a->bytes[37]] ^
            SL_transform_precomp[6][a->bytes[38]] ^
            SL_transform_precomp[7][a->bytes[39]],

            SL_transform_precomp[0][a->bytes[40]] ^
            SL_transform_precomp[1][a->bytes[41]] ^
            SL_transform_precomp[2][a->bytes[42]] ^
            SL_transform_precomp[3][a->bytes[43]] ^
            SL_transform_precomp[4][a->bytes[44]] ^
            SL_transform_precomp[5][a->bytes[45]] ^
            SL_transform_precomp[6][a->bytes[46]] ^
            SL_transform_precomp[7][a->bytes[47]],

            SL_transform_precomp[0][a->bytes[48]] ^
            SL_transform_precomp[1][a->bytes[49]] ^
            SL_transform_precomp[2][a->bytes[50]] ^
            SL_transform_precomp[3][a->bytes[51]] ^
            SL_transform_precomp[4][a->bytes[52]] ^
            SL_transform_precomp[5][a->bytes[53]] ^
            SL_transform_precomp[6][a->bytes[54]] ^
            SL_transform_precomp[7][a->bytes[55]],

            SL_transform_precomp[0][a->bytes[56]] ^
            SL_transform_precomp[1][a->bytes[57]] ^
            SL_transform_precomp[2][a->bytes[58]] ^
            SL_transform_precomp[3][a->bytes[59]] ^
            SL_transform_precomp[4][a->bytes[60]] ^
            SL_transform_precomp[5][a->bytes[61]] ^
            SL_transform_precomp[6][a->bytes[62]] ^
            SL_transform_precomp[7][a->bytes[63]]
        );

    TimerEnd(t);
    log_d("Out: ");
    DebugPrintVec(out);
}

/**
    @brief      Computation of iteration values for encryption function, as defined in
                ch. 7 of The Standard.
    @param      i - index of the iteration value.
    @param      prev_K - [i - 1]th iteration value.
    @param      out - output pointer.
 */
static
void K_i(const GostU8 i, const union Vec512 *prev_K, union Vec512 *out)
{
    union Vec512 r1, r2;

    log_d("K_i ITERATION %d", i + 1);
    log_d("prev_K: ");
    DebugPrintVec(prev_K);

    TimerStart(t);
    if (i == 0)
    {
        *out = *prev_K;

        TimerEnd(t);
        return;
    }

    Vec512_Xor(prev_K, C[i - 1], &r1);
    PTransform(&r1, &r2);
    SLCombinedTransform(&r2, out);

    TimerEnd(t);
    log_d("Out: ");
    DebugPrintVec(out);
}

void E(const union Vec512 *K, const union Vec512 *m, union Vec512 *out)
{
    union Vec512 r1, r2, new_m, prev_K;

    log_d("E transformation:");
    log_d("K: ");
    DebugPrintVec(K);
    log_d("m: ");
    DebugPrintVec(m);

    log_d("K_i ITERATION %d", 1);
    log_d("K_1 = K:");
    DebugPrintVec(K);

    TimerStart(t);
    // K_1 = K
    XTransform(m, K, &r1);
    PTransform(&r1, &r2);
    SLCombinedTransform(&r2, &new_m);

    prev_K = *K;

    // from i = 1 to 10 (indecies go from 0 to 11)
    for (int i = 1; i < C_SIZE; i++)
    {
        K_i(i, &prev_K, &prev_K);
        XTransform(&new_m, &prev_K, &r1);
        PTransform(&r1, &r2);
        SLCombinedTransform(&r2, &new_m);
    }

    K_i(C_SIZE, &prev_K, &prev_K);
    XTransform(&new_m, &prev_K, out);

    TimerEnd(t);
    log_d("Out: ");
    DebugPrintVec(out);
}

void G_N(const union Vec512 *h,
         const union Vec512 *m,
         const union Vec512 *N,
               union Vec512 *out)
{
    union Vec512 r1, r2;

    log_d("G transformation:");
    log_d("h: ");
    DebugPrintVec(h);
    log_d("m: ");
    DebugPrintVec(m);
    log_d("N: ");
    DebugPrintVec(N);

    TimerStart(t);
    Vec512_Xor(h, N, &r1);

    PTransform(&r1, &r2);
    SLCombinedTransform(&r2, &r1);

    E(&r1, m, &r2);
    Vec512_Xor(&r2, h, &r1);
    Vec512_Xor(&r1, m, out);

    TimerEnd(t);
    log_d("Out: ");
    DebugPrintVec(out);
}
