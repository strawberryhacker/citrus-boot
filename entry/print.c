/// Copyright (C) strawberryhacker 

#include <citrus-boot/uart.h>
#include <citrus-boot/gpio.h>
#include <citrus-boot/clk.h>
#include <citrus-boot/apic.h>
#include <citrus-boot/matrix.h>
#include <citrus-boot/print.h>
#include <citrus-boot/sprint.h>
#include <stddef.h>
#include <stdarg.h>

void print_init(void)
{
    // Pin setup 
    struct gpio tx = { .hw = GPIOD, .pin = 3 };
    struct gpio rx = { .hw = GPIOD, .pin = 2 };

    gpio_set_func(&tx, GPIO_FUNC_A);
    gpio_set_func(&rx, GPIO_FUNC_A);

    // Clock setup 
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
