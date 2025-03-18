/// @copyright Anufriev Ilia, anufriewwi@rambler.ru. All rights reserved.

#ifndef __GOST34112018_H__
#define __GOST34112018_H__

typedef enum {
    GOST34112018_Hash256 = 32,
    GOST34112018_Hash512 = 64,
} GOST34112018_HashSize_t;


void GOST34112018_Hash(const unsigned char          *message,
                       const unsigned long long      message_size,
                       unsigned char                *hash,
                       const GOST34112018_HashSize_t hash_size);

void PRECOMPUTE_TRANSFORM_TABLE();

#endif // __GOST34112018_H__
