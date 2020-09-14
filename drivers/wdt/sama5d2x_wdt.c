/* Copyright (C) strawberryhacker */

#include <regmap/sama5d2x.h>

#include "sama5d2x_wdt.h"

void sama5d2x_wdt_disable(void)
{
    WDT->MR = (1 << 15);
}
