// Copyright 2025, Anufriev Ilia, anufriewwi@rambler.ru
// SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later

#include "gost34112018.h"
#include "gost34112018_interface.h"
#include "gost34112018_common.h"

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
