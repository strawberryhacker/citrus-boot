/* Copyright (C) strawberryhacker */

#include <drivers/uart/sama5d2x_uart.h>
#include <drivers/gpio/sama5d2x_gpio.h>
#include <drivers/clk/sama5d2x_clk.h>
#include <drivers/irq/sama5d2x_apic.h>
#include <drivers/matrix/sama5d2x_matrix.h>
#include <c-boot/print.h>
#include <c-boot/serial.h>
#include <stddef.h>

/* Private function pointer to callback */
static void (*rx_callback)(u8) = NULL;
static void (*timeout_callback)(void) = NULL;

void uart_handler(void)
{
    u32 status = sama5d2x_uart_get_status(UART4);
    if (status & BIT(0)) {
        u8 data = sama5d2x_uart_read_unsafe(UART4);
        if (rx_callback) {
            rx_callback(data);
        }
    }
}

/* Implementation of the API required by c-boot */
void serial_init(void)
{
    /* Pin setup */
    struct sama5d2x_gpio tx = { .hw = GPIOB, .pin = 3 };
    struct sama5d2x_gpio rx = { .hw = GPIOB, .pin = 4 };

    sama5d2x_gpio_set_func(&tx, SAMA5D2X_GPIO_FUNC_A);
    sama5d2x_gpio_set_func(&rx, SAMA5D2X_GPIO_FUNC_A);

    /* Clock setup */
    sama5d2x_clk_pck_enable(28);

    struct sama5d2x_uart uart_conf = {
        .clk = 83000000, /* H32MX clock */
        .baud = 230400,
        .par = SAMA5D2X_PAR_NO
    };
    sama5d2x_uart_init(UART4, &uart_conf);
    sama5d2x_uart_irq_en(UART4, SAMA5D2X_UART_RX_IRQ | BIT(8));

    /* Enable the APIC support */
    sama5d2x_apic_irq_init(28, SAMA5D2X_APIC_PRI_3, 0, uart_handler);
    sama5d2x_apic_enable(28);
}

void serial_write(u8 data)
{
    sama5d2x_uart_write(UART4, data);
}

void serial_add_handler(void (*func)(u8))
{
    rx_callback = func;
}
