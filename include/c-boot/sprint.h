/* Copyright (C) strawberryhacker */

#ifndef SPRINT_H
#define SPRINT_H

#include <stdarg.h>
#include <c-boot/types.h>

u32 sprint(char* buf, const char* fmt, ...);

u32 vsprint(char* buf, const char* fmt, va_list args);

#endif
