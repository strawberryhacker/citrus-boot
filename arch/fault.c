/* Copyright (C) strawberryhacker */

#include <c-boot/types.h>
#include <c-boot/print.h>

void undef_handler(u32 pc)
{
    asm volatile ("bkpt #0");
}

void svc_handler(void)
{
    asm volatile ("bkpt #0");
}

void prefetch_abort_handler(void)
{
    asm volatile ("bkpt #0");
}

void data_abort_handler(u32 pc)
{
    asm volatile ("bkpt #0");
}
