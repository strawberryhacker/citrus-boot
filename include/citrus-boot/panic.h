/// Copyright (C) strawberryhacker

#ifndef PANIC_H
#define PANIC_H

#include <citrus-boot/types.h>

#define panic(reason) panic_handler(__FILE__, __LINE__, (reason))

#define assert(statement) assert_handler(__FILE__, __LINE__, ((statement) != 0));

void panic_handler(const char* file, u32 line, const char* reason);

void assert_handler(const char* file, u32 line, u32 statement);

#endif
