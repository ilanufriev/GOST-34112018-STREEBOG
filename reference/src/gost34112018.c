/// @file gost34112018.c
/// @copyright Anufriev Ilia, anufriewwi@rambler.ru. All rights reserved.

#include "gost34112018.h"
#include "gost34112018_private.h"

#define MAX_UINT64 0xFFFFFFFFFFFFFFFF

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

static inline uint64_t Uint64_ReverseByteOrder(const uint64_t x)
{
    uint64_t result = 0;
    uint8_t *result_bytes = (uint8_t *) (&result);
    uint8_t *x_bytes = (uint8_t *) (&x);

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
        uint64_t qword_reversed = Uint64_ReverseByteOrder(vec->qwords[i]);
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

void Vec512_Add(const union Vec512 *in1,
                const union Vec512 *in2,
                      union Vec512 *out)
{
    uint64_t carry = 0;

    log_d("Added vectors:");
    log_d("In1: ");
    PrintVec(in1);
    log_d("In2: ");
    PrintVec(in2);

    for (int i = VEC512_QWORDS - 1; i >= 0; i--)
    {
        uint64_t in1_reverse = Uint64_ReverseByteOrder(in1->qwords[i]);
        uint64_t in2_reverse = Uint64_ReverseByteOrder(in2->qwords[i]);

        uint64_t out_reverse = in1_reverse + in2_reverse + carry;

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

void Uint64ToVec512(const uint64_t x, union Vec512 *out)
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

void STransform(const union Vec512 *a, union Vec512 *out)
{
    log_d("S transformation:");
    log_d("a: ");
    PrintVec(a);

    for (int i = 0; i < VEC512_BYTES; i++)
    {
        out->bytes[i] = P[a->bytes[i]];
    }

    log_d("Out: ");
    PrintVec(out);
}

void PTransform(const union Vec512 *a, union Vec512 *out)
{
    log_d("P transformation:");
    log_d("a: ");
    PrintVec(a);

    for (int i = 0; i < VEC512_BYTES; i++)
    {
        out->bytes[i] = a->bytes[t[i]];
    }

    log_d("Out: ");
    PrintVec(out);
}

void LTransform(const union Vec512 *a, union Vec512 *out)
{
    log_d("L transformation:");
    log_d("a: ");
    PrintVec(a);

    for (int i = 0; i < VEC512_QWORDS; i++)
    {
        uint64_t b = Uint64_ReverseByteOrder(a->qwords[i]);
        uint64_t c = 0;

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

void K_i(const uint8_t i, const union Vec512 *prev_K, union Vec512 *out)
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

void SplitMessage512(const uint8_t *message,
                     const uint64_t size,
                     union Vec512  *out)
{
    for (uint64_t i = 0; (i < VEC512_SIZE) && (i < size); i++)
    {
        out->bytes[VEC512_SIZE - 1 - i] = message[size - 1 - i];
    }
}

void InitContext(struct GOST34112018_Context *ctx,
                 const union  Vec512         *init_vector)
{
    ctx->h     = *init_vector;
    ctx->N     = ZERO_VECTOR_512;
    ctx->sigma = ZERO_VECTOR_512;
}

void Stage3(struct GOST34112018_Context *ctx,
            const  uint8_t              *message,
            const  uint64_t              size)
{
    union Vec512  r1, r2;
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

void Stage2(struct GOST34112018_Context *ctx,
            const  uint8_t              *message,
            const  uint64_t              size)
{
    uint64_t current_size = size;
    union Vec512  r1, r2;
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

void Start(struct GOST34112018_Context   *ctx,
           const  uint8_t                *message,
           const  uint64_t                size)
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

    for (int i = 0; i < hash_size; i++)
    {
        hash[i] = ctx.h.bytes[i];
    }
}
