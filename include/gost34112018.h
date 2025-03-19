// Copyright 2025, Anufriev Ilia, anufriewwi@rambler.ru
// SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later

#ifndef __GOST34112018_H__
#define __GOST34112018_H__

typedef enum {
    GOST34112018_Hash256 = 32,
    GOST34112018_Hash512 = 64,
} GOST34112018_HashSize_t;

/**
    @brief      Computes a cryptographic digest of the given message, using the hashing
                algorithm defined in Russian National Standard GOST 34.11-2018
                "Information Technology - Cryptographic Information Security - Hash
                Function".
    @param      message - message bytes in the natural order.
    @param      message_size - size of the message in bytes.
    @param      hash_size - size of the message digest (32 bytes (256 bits) or
                64 bytes (512 bits)).
    @param      hash_out - output pointer, message digest.
*/
void GOST34112018_Hash(const unsigned char          *message,
                       const unsigned long long      message_size,
                       const GOST34112018_HashSize_t hash_size,
                       unsigned char                *hash_out);

#endif // __GOST34112018_H__
