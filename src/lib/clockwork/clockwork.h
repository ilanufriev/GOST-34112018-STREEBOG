// Copyright 2025, Anufriev Ilia, anufriewwi@rambler.ru
// SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later

#ifndef __CLOCKWORK_H__
#define __CLOCKWORK_H__

#include "time.h"

#define CLKW_TimerStart(__timer, __md_list)                             \
    static struct CLKW_TimingMetaData __timer##_mdata;                  \
    if (!__timer##_mdata.is_registered)                                 \
    {                                                                   \
        __timer##_mdata.clk       = 0;                                  \
        __timer##_mdata.func_name = __func__;                           \
        __timer##_mdata.next      = NULL;                               \
        CLKW_TimingMetaData_Register((__md_list), (&__timer##_mdata));  \
    }                                                                   \
    clock_t __timer##_start = clock();
    

#define CLKW_TimerEnd(__timer) \
    __timer##_mdata.clk += (clock() - __timer##_start);

struct CLKW_TimingMetaData
{
    const char                 *func_name;
    clock_t                     clk;
    char                        is_registered;
    struct CLKW_TimingMetaData *next;
};

struct CLKW_TimingMetaDataListHead
{
    struct CLKW_TimingMetaData *first;
    struct CLKW_TimingMetaData *last;
};

void CLKW_TimingMetaData_Register(struct CLKW_TimingMetaDataListHead *llist, 
                                  struct CLKW_TimingMetaData         *mdata);

#endif // __CLOCKWORK_H__