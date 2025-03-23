// Copyright 2025, Anufriev Ilia, anufriewwi@rambler.ru
// SPDX-License-Identifier: BSD-3-Clause-No-Military-License OR GPL-3.0-or-later

#include "clockwork.h"

void CLKW_TimingMetaData_Register(struct CLKW_TimingMetaDataListHead *llist, 
                                  struct CLKW_TimingMetaData         *mdata)
{
    // it's a first element in the list
    if (!llist->first && !llist->last)
    {
        llist->first = mdata;
        llist->last  = mdata;
    }
    else
    {
        llist->last->next = mdata;
        llist->last = llist->last->next;
    }

    mdata->is_registered = 1;
}