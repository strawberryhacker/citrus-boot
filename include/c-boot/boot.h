/* Copyright (C) strawberryhacker */

#ifndef BOOT_H
#define BOOT_H

#include <c-boot/types.h>

u8 load_page(u32 addr, u8* buffer, u32 size);

#endif
