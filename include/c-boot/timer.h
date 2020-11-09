/* Copyright (C) strawberryhacker */

#ifndef TIMER_H
#define TIMER_H

#include <c-boot/types.h>

void timer_init(u32 ms);

void timer_restart(void);

void timer_stop(void);

void timer_add_handler(void (*func)(void));

#endif
