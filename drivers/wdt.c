/* Copyright (C) strawberryhacker */

#include <c-boot/regmap.h>
#include <c-boot/wdt.h>

void wdt_disable(void)
{
    WDT->MR = (1 << 15);
}
