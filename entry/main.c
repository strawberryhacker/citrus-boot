/* Copyright (C) strawberryhacker */

#include <c-boot/types.h>
#include <c-boot/led.h>
#include <c-boot/hardware.h>
#include <c-boot/serial.h>
#include <c-boot/print.h>
#include <c-boot/bitops.h>
#include <c-boot/timer.h>

#include <drivers/irq/sama5d2x_apic.h>
#include <drivers/gpio/sama5d2x_gpio.h>
#include <drivers/clk/sama5d2x_clk.h>
#include <drivers/uart/sama5d2x_uart.h>
#include <drivers/timer/sama5d2x_pit.h>

static struct sama5d2x_gpio btn = { .hw = GPIOA, .pin = 29 };
static volatile u32 tick = 0;

void btn_handler(void)
{
    (void)GPIOA->ISR;
    print("Button pressed\n");
}

void pit_handler(void)
{
    sama5d2x_pit_get_value();
}

void rec(u8 data)
{
    sama5d2x_uart_write(UART1, data);
}

void timer(void)
{
    print("Timeout\n");
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
    serial_add_handler(rec);
    timer_init(255);
    timer_add_handler(timer);

    sama5d2x_clk_pck_enable(18);

    /* Just for testing */
    sama5d2x_gpio_set_func(&btn, SAMA5D2X_GPIO_FUNC_OFF);
    sama5d2x_gpio_set_dir(&btn, SAMA5D2X_GPIO_INPUT);
    sama5d2x_gpio_set_pull(&btn, SAMA5D2X_GPIO_PULLUP);
    sama5d2x_gpio_set_event(&btn, SAMA5D2X_GPIO_EVENT_FALLING);
    sama5d2x_gpio_irq_enable(&btn);
    
    sama5d2x_apic_irq_init(18, SAMA5D2X_APIC_PRI_3, 0, btn_handler);
    sama5d2x_apic_enable(18);

    /* Enable the PIT timer */
    struct sama5d2x_pit_conf conf = {
        .irq_en = 1, .per = 5375
    };
    sama5d2x_apic_irq_init(3, SAMA5D2X_APIC_PRI_3, 0, pit_handler);
    sama5d2x_apic_enable(3);
    sama5d2x_pit_init(&conf);
    sama5d2x_pit_enable();

    asm volatile("cpsie ifa");
}

/*
 * Entry point after device spesific startup code
 */
int main(void)
{
    c_boot_init();



    while (1) {
        
    }
    return 1;
}
