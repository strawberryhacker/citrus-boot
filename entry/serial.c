/* Copyright (C) strawberryhacker */

#include <c-boot/uart.h>
#include <c-boot/gpio.h>
#include <c-boot/clk.h>
#include <c-boot/apic.h>
#include <c-boot/print.h>
#include <stddef.h>

/* Private function pointer to callback */
static void (*rx_callback)(u8) = NULL;
static void (*timeout_callback)(void) = NULL;

void uart_handler(void)
{
    u32 status = uart_get_status(UART4);
    if (status & BIT(0)) {
        u8 data = uart_read_unsafe(UART4);
        if (rx_callback) {
            rx_callback(data);
        }
    }
}

/* Implementation of the API required by c-boot */
void serial_init(void)
{
    /* Pin setup */
    struct gpio tx = { .hw = GPIOB, .pin = 3 };
    struct gpio rx = { .hw = GPIOB, .pin = 4 };

    gpio_set_func(&tx, GPIO_FUNC_A);
    gpio_set_func(&rx, GPIO_FUNC_A);

    /* Clock setup */
    clk_pck_enable(28);

    struct uart uart_conf = {
        .clk = 83000000, /* H32MX clock */
        .baud = 576000,
        .par = PAR_NO
    };
    uart_init(UART4, &uart_conf);
    uart_irq_en(UART4, UART_RX_IRQ);

    /* Enable the APIC support */
    apic_irq_init(28, APIC_PRI_3, 0, uart_handler);
    apic_enable(28);
}

