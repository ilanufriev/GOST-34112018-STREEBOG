// Copyright 2025, Anufriev Ilia, anufriewwi@rambler.ru
// SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later

#include "gost34112018.h"
#include "gost34112018_optimized_private.h"
#include "gost34112018_optimized_precomp.h"
#include <stdio.h>

#define MAX_UINT64 0xFFFFFFFFFFFFFFFF

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

    for (int i = 0; i < 8; i++)
    {
        printf("{\n");
        for (int j = 0; j < 256; j++)
        {
            if (j % 4 == 0 && j != 0)
            {
                printf("\n    ");
            }

            if (j == 0)
            {
                printf("    ");
            }
            printf("0x%016llx, ", table[i][j]);
        }
        printf("\n},");
        putc('\n', stdout);
    }
}

/**
    @brief      Initialization vector for the 256-bit long digest, as defined in ch. 5.1
                of The Standard.
*/
const union Vec512 INIT_VECTOR_256 = {
    .qwords = {
        [0] = 0x0101010101010101,
        [1] = 0x0101010101010101,
        [2] = 0x0101010101010101,
        [3] = 0x0101010101010101,
        [4] = 0x0101010101010101,
        [5] = 0x0101010101010101,
        [6] = 0x0101010101010101,
        [7] = 0x0101010101010101,
    }
};

/**
    @brief      Initialization vector for the 512-bit long digest, as defined in ch. 5.1
                of The Standard.
*/
const union Vec512 INIT_VECTOR_512 = {
    .qwords = {
        [0] = 0x0000000000000000,
        [1] = 0x0000000000000000,
        [2] = 0x0000000000000000,
        [3] = 0x0000000000000000,
        [4] = 0x0000000000000000,
        [5] = 0x0000000000000000,
        [6] = 0x0000000000000000,
        [7] = 0x0000000000000000,
    }
};

const union Vec512 ZERO_VECTOR_512 = {
   .qwords = {
        [0] = 0x0000000000000000,
        [1] = 0x0000000000000000,
        [2] = 0x0000000000000000,
        [3] = 0x0000000000000000,
        [4] = 0x0000000000000000,
        [5] = 0x0000000000000000,
        [6] = 0x0000000000000000,
        [7] = 0x0000000000000000,
    }
};

#ifdef DEBUG
static inline void PrintVec(const union Vec512 *vec)
{
    printf("    ");
    for (GostU32 i = 0; i < VEC512_BYTES; i++)
    {
        if (i != 0 && i % 8 == 0)
        {
            printf(" ");
        }
        printf("%02hhx", vec->bytes[i]);
    }
    printf("\n");
}
#else
static inline void PrintVec(const union Vec512 *out)
{

}
#endif // DEBUG

/**
    @brief      Vec512 addition operation.
    @param      in1 - first operand.
    @param      in2 - second operand.
    @param      out - output pointer.
 */
static
void Vec512_Add(const union Vec512 *in1,
                const union Vec512 *in2,
                      union Vec512 *out)
{
    GostU64 carry = 0;

    log_d("Added vectors:");
    log_d("In1: ");
    PrintVec(in1);
    log_d("In2: ");
    PrintVec(in2);

    for (int i = 0; i < VEC512_QWORDS; i++)
    {
        out->qwords[i] = in1->qwords[i] + in2->qwords[i] + carry;

        if (in2->qwords[i] > (MAX_UINT64 - in1->qwords[i]))
        {
            carry = 1;
        }
        else
        {
            carry = 0;
        }
    }

    log_d("Out: ");
    PrintVec(out);
}

/**
    @brief      Vec512 XOR operation.
    @param      in1 - first operand.
    @param      in2 - second operand.
    @param      out - output pointer.
 */
static
void Vec512_Xor(const union Vec512 *in1,
                const union Vec512 *in2,
                      union Vec512 *out)
{
    log_d("XOR'ed vectors:");
    log_d("In1: ");
    PrintVec(in1);
    log_d("In2: ");
    PrintVec(in2);

    for (int i = 0; i < VEC512_QWORDS; i++)
    {
        out->qwords[i] = in1->qwords[i] ^ in2->qwords[i];
    }

    log_d("Out: ");
    PrintVec(out);
}

/**
    @brief       Uint64 to Vec512 conversion function.
    @param       x - conversion operand.
    @param       out - output pointer.
 */
static inline void Uint64ToVec512(const GostU64 x, union Vec512 *out)
{
    out->qwords[0] = x;
    out->qwords[1] = 0;
    out->qwords[2] = 0;
    out->qwords[3] = 0;
    out->qwords[4] = 0;
    out->qwords[5] = 0;
    out->qwords[6] = 0;
    out->qwords[7] = 0;
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
    PrintVec(a);
    log_d("K: ");
    PrintVec(k);

    Vec512_Xor(a, k, out);

    log_d("Out: ");
    PrintVec(out);
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
    PrintVec(a);

    for (int i = 0; i < VEC512_BYTES; i++)
    {
        out->bytes[i] = a->bytes[TAU[i]];
    }

    log_d("Out: ");
    PrintVec(out);
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
    PrintVec(a);

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
    PrintVec(out);
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
    PrintVec(prev_K);

    if (i == 0)
    {
        *out = *prev_K;
        return;
    }

    Vec512_Xor(prev_K, C[i - 1], &r1);
    PTransform(&r1, &r2);
    SLCombinedTransform(&r2, out);

    log_d("Out: ");
    PrintVec(out);
}

/**
    @brief      Encryption function E(K, m), which is an integral part of the compression
                function, as defined in ch. 7 of The Standard.
    @param      K - iteration values initial vector, according to The Standard.
    @param      m - an argument 'm', according to The Standard.
    @param      out - output pointer.
 */
static
void E(const union Vec512 *K, const union Vec512 *m, union Vec512 *out)
{
    union Vec512 r1, r2, new_m, prev_K;

    log_d("E transformation:");
    log_d("K: ");
    PrintVec(K);
    log_d("m: ");
    PrintVec(m);

    log_d("K_i ITERATION %d", 1);
    log_d("K_1 = K:");
    PrintVec(K);

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
    PrintVec(out);
}

/**
    @brief      Compression function G_N(h, m), as defined in ch. 7 of The Standard.
    @param      m - parameter 'm', accroding to The Standard.
    @param      h - parameter 'h', according to The Standard.
    @param      N - parameter 'N', according to The Standard.
    @param      out - output pointer.
 */
static
void G_N(const union Vec512 *h,
         const union Vec512 *m,
         const union Vec512 *N,
               union Vec512 *out)
{
    union Vec512 r1, r2;

    log_d("G transformation:");
    log_d("h: ");
    PrintVec(h);
    log_d("m: ");
    PrintVec(m);
    log_d("N: ");
    PrintVec(N);

    Vec512_Xor(h, N, &r1);

    PTransform(&r1, &r2);
    SLCombinedTransform(&r2, &r1);

    E(&r1, m, &r2);
    Vec512_Xor(&r2, h, &r1);
    Vec512_Xor(&r1, m, out);

    log_d("Out: ");
    PrintVec(out);
}

/**
    @brief       This function takes message in natural ("as is") byte order,
                 and takes a 512-bit long part from it, starting from the end of
                 the message. While splitting, it also reverses the 512-bit block into
                 little endian-byte order.
    @param       message - message to sliced into 512-bit blocks.
    @param       size - size of the message.
    @param       out - a output pointer.
 */
static
void SplitMessage512(const GostU8 *message,
                     const GostU64 size,
                     union Vec512 *out)
{
    for (GostU64 i = 0; (i < VEC512_SIZE) && (i < size); i++)
    {
        out->bytes[i] = message[size - 1 - i];
    }
}

/**
    @brief       Initialize GOST34112018 context. This should be done
                 before any computations take place, according to ch. 8.1 of The Standard.
    @param       ctx - input pointer, context to be initialized.
    @param       init_vector - initialization vector for context.
 */
static
void InitContext(struct GOST34112018_Context *ctx,
                 const union  Vec512         *init_vector)
{
    ctx->h     = *init_vector;
    ctx->N     = ZERO_VECTOR_512;
    ctx->sigma = ZERO_VECTOR_512;
}

/**
    @brief       Stage 3 of the hashing algorithm, as defined in the ch. 8.3.
    @param       ctx - current context of the algorithm.
    @param       message - message of size less than 512 bits.
    @param       size - size of the message.
 */
static
void Stage3(struct GOST34112018_Context *ctx,
            const  GostU8               *message,
            const  GostU64               size)
{
    union Vec512  r1;
    union Vec512  m       = ZERO_VECTOR_512;
    union Vec512 *h       = &ctx->h;
    union Vec512 *N       = &ctx->N;
    union Vec512 *sigma   = &ctx->sigma;
    union Vec512  size512;

    SplitMessage512(message, size, &m);
    Uint64ToVec512(size * BYTE_SIZE, &size512);

    m.bytes[size] = 0x01; // padding

    G_N(h, &m, N, h);

    Vec512_Add(N, &size512, &r1);
    *N = r1;

    Vec512_Add(sigma, &m, &r1);
    *sigma = r1;

    G_N(h, N, &ZERO_VECTOR_512, h);
    G_N(h, sigma, &ZERO_VECTOR_512, h);
}

/**
    @brief      Stage 2 of the hashing algorithm, as defined in the ch. 8.2 of
                The Standard.
    @param      ctx - current context of the algorithm.
    @param      message - message of size more than 512 bits (or 64 bytes).
    @param      size - size of the message in bytes.
 */
static
void Stage2(struct GOST34112018_Context *ctx,
            const  GostU8               *message,
            const  GostU64               size)
{
    GostU64 current_size = size;
    union Vec512  r1;
    union Vec512  m;
    union Vec512 *h      = &ctx->h;
    union Vec512 *N      = &ctx->N;
    union Vec512 *sigma  = &ctx->sigma;
    union Vec512  vec512;

    Uint64ToVec512(512, &vec512);

    while (current_size >= BLOCK_SIZE)
    {
        SplitMessage512(message, current_size, &m);

        G_N(h, &m, N, h);

        Vec512_Add(N, &vec512, &r1);
        *N = r1;

        Vec512_Add(sigma, &m, &r1);
        *sigma = r1;

        current_size -= BLOCK_SIZE;
    }

    Stage3(ctx, message, current_size);
}

/**
    @brief       Start the hashing algorithm
    @param       ctx - current context of the algorithm.
    @param       message - message to be hashed.
    @param       size - size of the message.
 */
static
void Start(struct GOST34112018_Context   *ctx,
           const  GostU8                 *message,
           const  GostU64                 size)
{
    Stage2(ctx, message, size);
}

void GOST34112018_Hash(const unsigned char          *message,
                       const unsigned long long      message_size,
                       const GOST34112018_HashSize_t hash_size,
                       unsigned char                *hash_out)
{
    struct GOST34112018_Context ctx;
    InitContext(&ctx, hash_size == GOST34112018_Hash512 ? &INIT_VECTOR_512
                                                        : &INIT_VECTOR_256);
    Start(&ctx, message, message_size);

    for (GostU32 i = 0; i < hash_size; i++)
    {
        hash_out[i] = ctx.h.bytes[GOST34112018_Hash512 - 1 - i];
    }
}

