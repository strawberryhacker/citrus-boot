/* Copyright (C) strawberryhacker */

#include <c-boot/pit.h>
#include <c-boot/regmap.h>
#include <c-boot/bitops.h>

void pit_init(struct pit_conf* conf)
{
    u32 reg = PIT->MR;
    reg &= BIT(24);
    reg = (conf->per & 0xFFFFF) | (conf->irq_en << 25);
    PIT->MR = reg;
}

void pit_enable(void)
{
    PIT->MR |= BIT(24);
}

void pit_disable(void)
{
    PIT->MR &= ~BIT(24);
}

u32 pit_get_value(void)
{
    return PIT->PIVR;
}

u32 pit_get_image(void)
{
    return PIT->PIIR;
}

u32 pit_get_status(void)
{
    return PIT->SR;
}
