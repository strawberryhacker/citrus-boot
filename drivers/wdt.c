/// Copyright (C) strawberryhacker 

#include <citrus-boot/regmap.h>
#include <citrus-boot/wdt.h>

void wdt_disable(void)
{
    WDT->MR = (1 << 15);
}
