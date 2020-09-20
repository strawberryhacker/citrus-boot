/* Copyright (C) strawberryhacker */

#include <drivers/uart/sama5d2x_uart.h>
#include <drivers/gpio/sama5d2x_gpio.h>
#include <drivers/clk/sama5d2x_clk.h>
#include <drivers/irq/sama5d2x_apic.h>
#include <drivers/matrix/sama5d2x_matrix.h>
#include <c-boot/print.h>
#include <c-boot/serial.h>
#include <c-boot/sprint.h>
#include <stddef.h>
#include <stdarg.h>

void print_init(void)
{
    /* Pin setup */
    struct sama5d2x_gpio tx = { .hw = GPIOD, .pin = 3 };
    struct sama5d2x_gpio rx = { .hw = GPIOD, .pin = 2 };

    sama5d2x_gpio_set_func(&tx, SAMA5D2X_GPIO_FUNC_A);
    sama5d2x_gpio_set_func(&rx, SAMA5D2X_GPIO_FUNC_A);

    /* Clock setup */
    sama5d2x_clk_pck_enable(25);

    struct sama5d2x_uart uart_conf = {
        .clk = 83000000, /* H32MX clock */
        .baud = 230400,
        .par = SAMA5D2X_PAR_NO
    };
    sama5d2x_uart_init(UART1, &uart_conf);
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
        sama5d2x_uart_write(UART1, *start);
        start++;
    }
}
