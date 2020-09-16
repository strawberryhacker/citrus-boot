/* Copyright (C) strawberryhacker */

#include <drivers/uart/sama5d2x_uart.h>
#include <drivers/gpio/sama5d2x_gpio.h>
#include <drivers/clk/sama5d2x_clk.h>
#include <drivers/irq/sama5d2x_apic.h>
#include <drivers/matrix/sama5d2x_matrix.h>
#include <c-boot/print.h>
#include <c-boot/serial.h>

void uart_handler(void)
{
    /* Clear the interrupt flag */
    u8 data = sama5d2x_uart_read_unsafe(UART1);
    sama5d2x_uart_write(UART1, data);
}

/* Implementation of the API required by c-boot */
void serial_init(void)
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
        .baud = 115200,
        .par = SAMA5D2X_PAR_NO
    };
    sama5d2x_uart_init(UART1, &uart_conf);
    sama5d2x_uart_irq_en(UART1, SAMA5D2X_UART_RX_IRQ);

    /* Enable the APIC support */
    sama5d2x_apic_irq_init(25, SAMA5D2X_APIC_PRI_3, 0, uart_handler);
    sama5d2x_apic_enable(25);
}

void serial_write(u8 data)
{
    sama5d2x_uart_write(UART1, data);
}

void serial_add_handler(void (*func)(void))
{
    /* Implement this */
}
