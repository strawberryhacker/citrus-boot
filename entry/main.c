/* Copyright (C) strawberryhacker */

#include <c-boot/types.h>
#include <c-boot/led.h>
#include <c-boot/hardware.h>
#include <c-boot/serial.h>
#include <c-boot/print.h>
#include <c-boot/bitops.h>

#include <drivers/irq/sama5d2x_apic.h>
#include <drivers/gpio/sama5d2x_gpio.h>
#include <drivers/clk/sama5d2x_clk.h>
#include <drivers/uart/sama5d2x_uart.h>

static struct sama5d2x_gpio btn = { .hw = GPIOA, .pin = 29 };

void btn_handler(void)
{
    (void)GPIOA->ISR;
    print("Button pressed\n");
}

/*
 * Initializes the components needed by s-boot
 */
static void c_boot_init(void)
{
    /* Custom hardware init spesific to the board */
    hardware_init();


    /* Initilaize hardware used by c-boot */
    led_init();
    serial_init();

    sama5d2x_clk_pck_enable(18);

    /* Just for testing */
    sama5d2x_gpio_set_func(&btn, SAMA5D2X_GPIO_FUNC_OFF);
    sama5d2x_gpio_set_dir(&btn, SAMA5D2X_GPIO_INPUT);
    sama5d2x_gpio_set_pull(&btn, SAMA5D2X_GPIO_PULLUP);
    sama5d2x_gpio_set_event(&btn, SAMA5D2X_GPIO_EVENT_FALLING);
    sama5d2x_gpio_irq_enable(&btn);
    
    sama5d2x_apic_irq_init(18, SAMA5D2X_APIC_PRI_3, 0, btn_handler);
    sama5d2x_apic_enable(18);

    asm volatile("cpsie ifa");

    while (1) {
        APIC->SSR = 25;
        if (UART1->SR & BIT(0)) {

            u8 rec = UART1->RHR;
        }
    }
}

/*
 * Entry point after device spesific startup code
 */
int main(void)
{
    c_boot_init();

    u32 led_state = 0;
    while (1) {
        for (u32 i = 0; i < 500000; i++) {
            asm volatile ("nop");
        }
        led_state = (led_state) ? 0 : 1;
        led_set(led_state);
        ///print("We are awqesome!\n");
    }
    return 1;
}
