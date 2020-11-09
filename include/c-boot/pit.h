/* Copyright (C) strawberryhacker */

#ifndef PIT_H
#define PIT_H

#include <c-boot/types.h>

/*
 * The timer runs on master clock divided by 16
 */
struct pit_conf {
    /* 20-bit period */
    u32 per;
    u8 irq_en : 1;
};

void pit_init(struct pit_conf* conf);

void pit_enable(void);

void pit_disable(void);

u32 pit_get_value(void);

u32 pit_get_image(void);

u32 pit_get_status(void);


#endif
