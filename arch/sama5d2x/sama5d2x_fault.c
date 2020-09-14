/* Copyright (C) strawberryhacker */

#include <c-boot/types.h>

void undef_handler(void)
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

void data_abort_handler(void)
{
    asm volatile ("bkpt #0");
}
