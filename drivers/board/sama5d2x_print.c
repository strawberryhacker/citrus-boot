/* Copyright (C) strawberryhacker */

#include <drivers/uart/sama5d2x_uart.h>
#include <c-boot/sprint.h>
#include <stdarg.h>

static char buffer[128];

void print(const char* data, ...)
{
    va_list args;
    va_start(args, data);
    u32 cnt = vsprint(buffer, data, args);
    va_end(args);

    const u8* start = (const u8 *)buffer;
    while (cnt--) {
        sama5d2x_uart_write(UART1, *start);
        start++;
    }
}
