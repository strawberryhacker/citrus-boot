/* Copyright (C) strawberryhacker */

#ifndef PRINT_H
#define PRINT_H

#include <c-boot/types.h>
#include <stdarg.h>

void print_init(void);

/* Optional print function */
void print(const char* data, ...);

#endif
