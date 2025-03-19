// Copyright 2025, Anufriev Ilia, anufriewwi@rambler.ru
// SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later

#include "gost34112018.h"
#include "gost34112018_optimized_precomp.h"
#include "gost34112018_common.h"
#include "gost34112018_interface.h"

/**
    @brief     This function computes a lookup table for L (ch. 5.3) and S (ch. 5.2)
               transformations combined. It is not used in the computation itself, rather
               it can be used to produce an array of numbers that can later be pasted into
               source code (as it is in this project, see file
               'gost34112018_optimized_precomp.h')
    @note      Where 'i' is a sequence number of the byte in 64-bit number NUM
               (i = 0 is LSB) 'j' is a value of the byte NUM[i]. To compute LP transform,
               one would for each i-th number in NUM find TABLE[i][NUM[i]] and XOR it
               with an accumulator value
 */
void LINEAR_TRANSFORM_TABLE(const GostU64 *A, const GostU64 A_size,
                            const GostU8  *P, const GostU64 P_size,
                                  GostU64 **out)
{
    for (GostU64 i = 0; i < 8; i++)
    {
        for (GostU64 j = 0; j < P_size; j++)
        {
            GostU64 accum = 0;
            for (GostU64 k = 0; k < 8; k++)
            {
                if ((P[j] & (((GostU8) 1) << k)) != 0)
                {
                    accum ^= A[A_size - 1 - (8 * i + k)];
                }
            }

            out[i][j] = accum;
        }
    }
}

void PRECOMPUTE_TRANSFORM_TABLE(unsigned long long **out)
{
    GostU64 **table = out;
    LINEAR_TRANSFORM_TABLE(A, 64, PI, 256, table);
}

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

    Vec512_Xor(a, k, out);

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
    log_d("P transformation:");
    log_d("a: ");
    DebugPrintVec(a);

    for (int i = 0; i < VEC512_BYTES; i++)
    {
        out->bytes[i] = a->bytes[TAU[i]];
    }

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
    log_d("PL transformation:");
    log_d("a: ");
    DebugPrintVec(a);

    for (GostU32 i = 0; i < VEC512_QWORDS; i++)
    {
        GostU64 c = 0;
        for (GostU64 j = 0; j < sizeof(GostU64); j++)
        {
            GostU64 byte = ((a->qwords[i]) >> (j * 8)) & 0xFF;
            c ^= SL_transform_precomp[j][byte];
        }

        out->qwords[i] = c;
    }

    log_d("Out: ");
    DebugPrintVec(out);
}

/**
    @brief      Computation of iteration values for encryption function, as defined in
                ch. 7 of The Standard.
    @param      i - index of the iteration value.
    @param      K[i - 1]th iteration value.
    @param      out - output pointer.
 */
static
void K_i(const GostU8 i, const union Vec512 *prev_K, union Vec512 *out)
{
    union Vec512 r1, r2;

    log_d("K_i ITERATION %d", i + 1);
    log_d("prev_K: ");
    DebugPrintVec(prev_K);

    if (i == 0)
    {
        *out = *prev_K;
        return;
    }

    Vec512_Xor(prev_K, C[i - 1], &r1);
    PTransform(&r1, &r2);
    SLCombinedTransform(&r2, out);

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

    Vec512_Xor(h, N, &r1);

    PTransform(&r1, &r2);
    SLCombinedTransform(&r2, &r1);

    E(&r1, m, &r2);
    Vec512_Xor(&r2, h, &r1);
    Vec512_Xor(&r1, m, out);

    log_d("Out: ");
    DebugPrintVec(out);
}
