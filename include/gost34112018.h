// Copyright 2025, Anufriev Ilia, anufriewwi@rambler.ru
// SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later

#ifndef __GOST34112018_H__
#define __GOST34112018_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GOST34112018_Hash256 = 32,
    GOST34112018_Hash512 = 64,
} GOST34112018_HashSize_t;

/**
    @brief      Context of the algorithm, as described in ch. 8.1 of the Standard.
 */
struct GOST34112018_Context
{
    unsigned char           h    [64];
    unsigned char           N    [64];
    unsigned char           sigma[64];
    GOST34112018_HashSize_t hash_size;
    unsigned char           prev_block_size;
};

/**
    @brief      Computes a cryptographic digest of the given bytes array, using the hashing
                algorithm defined in Russian National Standard GOST 34.11-2018
                "Information Technology - Cryptographic Information Security - Hash
                Function".
    @param      message - message bytes in the big-endian (MSB-first) order.
    @param      message_size - size of the message in bytes.
    @param      hash_size - size of the message digest (32 bytes (256 bits) or
                64 bytes (512 bits)).
    @param      hash_out - output pointer, message digest.
*/
void GOST34112018_HashBytes(const unsigned char      *message,
                       const unsigned long long       message_size,
                       const GOST34112018_HashSize_t  hash_size,
                       unsigned char                 *hash_out);

/**
    @brief      Initialize algorithm context with initial values defined in The Standard.
                Context should be allocated by user and initialized before use in the
                functions that explicitly require it.
    @param      ctx - context to be initialized.
    @param      hash_size - size in bytes of the future digest. It is recommended to use
                enum GOST34112018_HashSize_t instead of plain 32 or 64.
 */
void GOST34112018_InitContext(struct GOST34112018_Context   *ctx,
                                     GOST34112018_HashSize_t hash_size);

/**
    @brief      Hash a block of bytes of given size. The current state of the algorithm
                is stored in the ctx parameter. This function can be used to hash large
                streams of data, which can not be made available all at once.
    @param      block - block of data to be hashed in big-endian order.
    @param      block_size - size of the block.
    @param      ctx - current context of the algorithm.
 */
void GOST34112018_HashBlock(const unsigned char          *block,
                            const unsigned long long      block_size,
                            struct GOST34112018_Context  *ctx);

/**
    @brief      Performs necessary routines to correctly finish hashing.
    @param      cyx - current context of the algorithm.
 */
void GOST34112018_HashBlockEnd(struct GOST34112018_Context *ctx);

/**
    @brief      Extract hash from the context.
    @param      ctx - context with the hash in it.
    @param      out - output pointer.
 */
void GOST34112018_GetHashFromContext(const struct GOST34112018_Context *ctx,
                                     unsigned char                     *out);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __GOST34112018_H__
