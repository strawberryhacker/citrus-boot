/* Copyright (C) strawberryhacker */

#ifndef SAMA5D2X_PIT_H
#define SAMA5D2X_PIT_H

#include <c-boot/types.h>

/*
 * The timer runs on master clock divided by 16
 */
struct sama5d2x_pit_conf {
    /* 20-bit period */
    u32 per;
    u8 irq_en : 1;
};

void sama5d2x_pit_init(struct sama5d2x_pit_conf* conf);

void sama5d2x_pit_enable(void);

void sama5d2x_pit_disable(void);

u32 sama5d2x_pit_get_value(void);

u32 sama5d2x_pit_get_image(void);

u32 sama5d2x_pit_get_status(void);


#endif
