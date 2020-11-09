/* Copyright (C) strawberryhacker */

#include <c-boot/uart.h>
#include <c-boot/gpio.h>
#include <c-boot/clk.h>
#include <c-boot/apic.h>
#include <c-boot/matrix.h>
#include <c-boot/print.h>
#include <c-boot/sprint.h>
#include <stddef.h>
#include <stdarg.h>

void print_init(void)
{
    /* Pin setup */
    struct gpio tx = { .hw = GPIOD, .pin = 3 };
    struct gpio rx = { .hw = GPIOD, .pin = 2 };

    gpio_set_func(&tx, GPIO_FUNC_A);
    gpio_set_func(&rx, GPIO_FUNC_A);

    /* Clock setup */
    clk_pck_enable(25);

    struct uart uart_conf = {
        .clk = 83000000,
        .baud = 230400,
        .par = PAR_NO
    };
    uart_init(UART1, &uart_conf);
}

static char buffer[128];

void print(const char* data, ...)
{
    va_list args;
    va_start(args, data);
    u32 cnt = vsprint(buffer, data, args);
    va_end(args);

    const u8* start = (const u8 *)buffer;
    while (cnt--) {
        uart_write(UART1, *start);
        start++;
    }
}
