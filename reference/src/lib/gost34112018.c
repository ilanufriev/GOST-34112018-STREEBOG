// Copyright 2025, Anufriev Ilia, anufriewwi@rambler.ru
// SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later

#include "gost34112018.h"
#include "gost34112018_private.h"

#define MAX_UINT64 0xFFFFFFFFFFFFFFFF

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

const union Vec512 VECTOR_512 = {
    .bytes = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
    },
};

/**
    @brief      Changes endianness of the uint64.
    @param      x - unsigned 64-bit value to be reversed.
    @return     reversed unsigned 64-bit value.
 */
static inline GostU64 Uint64_ReverseByteOrder(const GostU64 x)
{
    GostU64 result = 0;
    GostU8 *result_bytes = (GostU8 *) (&result);
    GostU8 *x_bytes = (GostU8 *) (&x);

    result_bytes[7] = x_bytes[0];
    result_bytes[6] = x_bytes[1];
    result_bytes[5] = x_bytes[2];
    result_bytes[4] = x_bytes[3];
    result_bytes[3] = x_bytes[4];
    result_bytes[2] = x_bytes[5];
    result_bytes[1] = x_bytes[6];
    result_bytes[0] = x_bytes[7];

    return result;
}

#ifdef DEBUG
static inline void PrintVec(const union Vec512 *vec)
{
    printf("    ");
    for (int i = 0; i < VEC512_QWORDS; i++)
    {
        GostU64 qword_reversed = Uint64_ReverseByteOrder(vec->qwords[i]);
        printf("%016llx ", qword_reversed);
    }

    printf("\n");
    fflush(stdout);
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

    for (int i = VEC512_QWORDS - 1; i >= 0; i--)
    {
        GostU64 in1_reverse = Uint64_ReverseByteOrder(in1->qwords[i]);
        GostU64 in2_reverse = Uint64_ReverseByteOrder(in2->qwords[i]);

        GostU64 out_reverse = in1_reverse + in2_reverse + carry;

        if (in2_reverse > (MAX_UINT64 - in1_reverse))
        {
            carry = 1;
        }
        else
        {
            carry = 0;
        }

        out->qwords[i] = Uint64_ReverseByteOrder(out_reverse);
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

    for (int i = VEC512_QWORDS - 1; i >= 0; i--)
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
static
void Uint64ToVec512(const GostU64 x, union Vec512 *out)
{
    out->qwords[7] = Uint64_ReverseByteOrder(x);
    out->qwords[6] = 0;
    out->qwords[5] = 0;
    out->qwords[4] = 0;
    out->qwords[3] = 0;
    out->qwords[2] = 0;
    out->qwords[1] = 0;
    out->qwords[0] = 0;
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
    @brief      S transformtaion of the algorithm as defined in the ch. 6 of the Standard.
    @param      a - argument 'a', according to The Standard.
    @param      out - output pointer.
 */
static
void STransform(const union Vec512 *a, union Vec512 *out)
{
    log_d("S transformation:");
    log_d("a: ");
    PrintVec(a);

    for (int i = 0; i < VEC512_BYTES; i++)
    {
        out->bytes[i] = PI[a->bytes[i]];
    }

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
    @brief      L transformation of the algorithm as defined in the ch. 6 of the Standard.
    @param      a - argument 'a', according to The Standard.
    @param      out - output pointer.
 */
static
void LTransform(const union Vec512 *a, union Vec512 *out)
{
    log_d("L transformation:");
    log_d("a: ");
    PrintVec(a);

    for (int i = 0; i < VEC512_QWORDS; i++)
    {
        GostU64 b = Uint64_ReverseByteOrder(a->qwords[i]);
        GostU64 c = 0;

        for (int j = 0; j < 64; j++)
        {
            if (b == ((b >> 1) << 1))
            {
                c ^= 0;
            }
            else
            {
                c ^= Uint64_ReverseByteOrder(A[A_SIZE - 1 - j]);
            }

            b = b >> 1;
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
    STransform(&r1, &r2);
    PTransform(&r2, &r1);
    LTransform(&r1, out);

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

    STransform(&r1, &r2);
    PTransform(&r2, &r1);
    LTransform(&r1, &r2);

    E(&r2, m, &r1);
    Vec512_Xor(&r1, h, &r2);
    Vec512_Xor(&r2, m, out);

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
                     union Vec512  *out)
{
    for (GostU64 i = 0; (i < VEC512_SIZE) && (i < size); i++)
    {
        out->bytes[VEC512_SIZE - 1 - i] = message[size - 1 - i];
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
            const  GostU8              *message,
            const  GostU64              size)
{
    union Vec512  r1;
    union Vec512  m       = ZERO_VECTOR_512;
    union Vec512 *h       = &ctx->h;
    union Vec512 *N       = &ctx->N;
    union Vec512 *sigma   = &ctx->sigma;
    union Vec512  size512;

    SplitMessage512(message, size, &m);
    Uint64ToVec512(size * BYTE_SIZE, &size512);

    m.bytes[BLOCK_SIZE - size - 1] = 0x01; // padding

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
                       unsigned char                *hash,
                       const GOST34112018_HashSize_t hash_size)
{
    struct GOST34112018_Context ctx;
    InitContext(&ctx, hash_size == GOST34112018_Hash512 ? &INIT_VECTOR_512
                                                        : &INIT_VECTOR_256);
    Start(&ctx, message, message_size);

    for (GostU32 i = 0; i < hash_size; i++)
    {
        hash[i] = ctx.h.bytes[i];
    }
}
