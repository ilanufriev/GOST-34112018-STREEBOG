// Copyright 2025, Anufriev Ilia, anufriewwi@rambler.ru
// SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later

#ifndef __GOST34112018_INTERFACE_H__
#define __GOST34112018_INTERFACE_H__

#include "gost34112018_vec512.h"

/**
    @brief      Compression function G_N(h, m), as defined in ch. 7 of The Standard.
    @param      m - parameter 'm', accroding to The Standard.
    @param      h - parameter 'h', according to The Standard.
    @param      N - parameter 'N', according to The Standard.
    @param      out - output pointer.
 */
void G_N(const union Vec512 *h, const union Vec512 *m, const union Vec512 *N,
               union Vec512 *out);

/**
    @brief      Encryption function E(K, m), which is an integral part of the compression
                function, as defined in ch. 7 of The Standard.
    @param      K - iteration values initial vector, according to The Standard.
    @param      m - an argument 'm', according to The Standard.
    @param      out - output pointer.
 */
void E(const union Vec512 *K, const union Vec512 *m,
             union Vec512 *out);

#endif // __GOST34112018_INTERFACE_H__