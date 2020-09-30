/* Copyright (C) strawberryhacker */

#ifndef BOOT_H
#define BOOT_H

#include <c-boot/types.h>

/* Set to zero if not present */
struct boot_header {
    u32 ram_present;
    u32 ram_start;
    u32 ram_end;
    u32 ddr_start;
    u32 ddr_end;
    u32 c_boot_version;
    u32 machine;
};

u32 get_kernel_load_addr(void);

u32 get_header_load_addr(void);

void load_kernel(u32 addr);

u32 get_kernel_size(u32 addr);

#endif
