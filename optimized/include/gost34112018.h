/// @copyright Anufriev Ilia, anufriewwi@rambler.ru. All rights reserved.

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

/**
    @brief      Computes a lookup table for acceleration of L and S transformations (
                consult chapters 5.2 and 5.3 of The Standard). Generally, there is no use
                in this function for anyone other than a developer.
    @param      out - output pointer, a lookup table((unsgined long long)[8][256]) with 
                precomputed values.
    @note       out should be at least of size [8][256].
*/
void PRECOMPUTE_TRANSFORM_TABLE(unsigned long long **out);

#endif // __GOST34112018_H__
