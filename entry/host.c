/// Copyright (C) strawberryhacker 

#include <citrus-boot/uart.h>
#include <citrus-boot/gpio.h>
#include <citrus-boot/clk.h>
#include <citrus-boot/apic.h>
#include <citrus-boot/print.h>
#include <stddef.h>

void uart_handler(void)
{
    u32 status = uart_get_status(UART4);
    if (status & BIT(0)) {
        u8 data = uart_read_unsafe(UART4);
        
    }
}

/// This will establish a serial connection to the host computer
void host_init(void)
{
    // Pin setup 
    struct gpio tx = { .hw = GPIOB, .pin = 3 };
    struct gpio rx = { .hw = GPIOB, .pin = 4 };

    gpio_set_func(&tx, GPIO_FUNC_A);
    gpio_set_func(&rx, GPIO_FUNC_A);

    // Clock setup 
    clk_pck_enable(28);

    struct uart uart_conf = {
        .clk = 83000000, // H32MX clock 
        .baud = 576000,
        .par = PAR_NO
    };
    uart_init(UART4, &uart_conf);
    uart_irq_en(UART4, UART_RX_IRQ);

    // Enable the APIC support 
    apic_irq_init(28, APIC_PRI_3, 0, uart_handler);
    apic_enable(28);
}
