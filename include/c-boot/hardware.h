/* Copyright (C) strawberryhacker */

#ifndef HARDWARE_H
#define HARDWARE_H

#include <c-boot/types.h>

void hardware_init(void);

/*
 * This function should release all resources used by c-boot except the 
 * resources needed by the kernel. This functions should NOT deinitialize any
 * of the following peripherals; DDR memory, DDR clock
 */
void hardware_deinit(void);

#endif