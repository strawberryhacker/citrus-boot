/* Copyright (C) strawberryhacker */

#include "sama5d2x_pit.h"
#include <regmap/sama5d2x.h>
#include <c-boot/bitops.h>

void sama5d2x_pit_init(struct sama5d2x_pit_conf* conf)
{
    u32 reg = PIT->MR;
    reg &= BIT(24);
    reg = (conf->per & 0xFFFFF) | (conf->irq_en << 25);
    PIT->MR = reg;
}

void sama5d2x_pit_enable(void)
{
    PIT->MR |= BIT(24);
}

void sama5d2x_pit_disable(void)
{
    PIT->MR &= ~BIT(24);
}

u32 sama5d2x_pit_get_value(void)
{
    return PIT->PIVR;
}

u32 sama5d2x_pit_get_image(void)
{
    return PIT->PIIR;
}

u32 sama5d2x_pit_get_status(void)
{
    return PIT->SR;
}
