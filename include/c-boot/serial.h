/* Copyright (C) strawberryhacker */

#include <c-boot/types.h>

void serial_init(void);

void serial_write(u8 data);

void serial_add_handler(void (*func)(u8));
