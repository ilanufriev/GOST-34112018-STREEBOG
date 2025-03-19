// Copyright 2025, Anufriev Ilia, anufriewwi@rambler.ru
// SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later

#include "gost34112018.h"
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

    Vec512_Xor(a, k, out);

    log_d("Out: ");
    DebugPrintVec(out);
}

/**
    @brief      S transformtaion of the algorithm as defined in the ch. 6 of the Standard.
    @param      a - argument 'a', according to The Standard.
    @param      out - output pointer.
 */
static
void STransform(const union Vec512 *a, union Vec512 *out)
{
    log_d("S transformation:");
    log_d("a: ");
    DebugPrintVec(a);

    for (int i = 0; i < VEC512_BYTES; i++)
    {
        out->bytes[i] = PI[a->bytes[i]];
    }

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
    @brief      L transformation of the algorithm as defined in the ch. 6 of the Standard.
    @param      a - argument 'a', according to The Standard.
    @param      out - output pointer.
 */
static
void LTransform(const union Vec512 *a, union Vec512 *out)
{
    log_d("L transformation:");
    log_d("a: ");
    DebugPrintVec(a);

    for (int i = 0; i < VEC512_QWORDS; i++)
    {
        GostU64 b = a->qwords[i];
        GostU64 c = 0;

        for (int j = 0; j < 64; j++)
        {
            if (b == ((b >> 1) << 1))
            {
                c ^= 0;
            }
            else
            {
                c ^= A[A_SIZE - 1 - j];
            }

            b = b >> 1;
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
    STransform(&r1, &r2);
    PTransform(&r2, &r1);
    LTransform(&r1, out);

    log_d("Out: ");
    DebugPrintVec(out);
}

/**
    @brief      Encryption function E(K, m), which is an integral part of the compression
                function, as defined in ch. 7 of The Standard.
    @param      K - iteration values initial vector, according to The Standard.
    @param      m - an argument 'm', according to The Standard.
    @param      out - output pointer.
 */
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
    STransform(&r1, &r2);
    PTransform(&r2, &r1);
    LTransform(&r1, &new_m);

    prev_K = *K;

    // from i = 1 to 10 (indecies go from 0 to 11)
    for (int i = 1; i < C_SIZE; i++)
    {
        K_i(i, &prev_K, &prev_K);
        XTransform(&new_m, &prev_K, &r1);
        STransform(&r1, &r2);
        PTransform(&r2, &r1);
        LTransform(&r1, &new_m);
    }

    K_i(C_SIZE, &prev_K, &prev_K);
    XTransform(&new_m, &prev_K, out);

    log_d("Out: ");
    DebugPrintVec(out);
}

/**
    @brief      Compression function G_N(h, m), as defined in ch. 7 of The Standard.
    @param      m - parameter 'm', accroding to The Standard.
    @param      h - parameter 'h', according to The Standard.
    @param      N - parameter 'N', according to The Standard.
    @param      out - output pointer.
 */
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

    STransform(&r1, &r2);
    PTransform(&r2, &r1);
    LTransform(&r1, &r2);

    E(&r2, m, &r1);
    Vec512_Xor(&r1, h, &r2);
    Vec512_Xor(&r2, m, out);

    log_d("Out: ");
    DebugPrintVec(out);
}
