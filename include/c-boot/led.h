/* Copyright (C) strawberryhacker */

#ifndef LED_H
#define LED_H

#include <c-boot/types.h>

/*
 * Initialization routine for the onboard LED
 */
void led_init(void);

/*
 * Called by s-boot in order to show status information. If not applicable 
 * just return from the function
 */
void led_set(u8 val);

#endif
