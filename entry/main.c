/* Copyright (C) strawberryhacker */

#include <c-boot/types.h>
#include <c-boot/led.h>
#include <c-boot/hardware.h>

/*
 * Initializes the components needed by s-boot
 */
static void c_boot_init(void)
{
    /* Custom hardware init spesific to the board */
    hardware_init();


    /* Initilaize hardware used by c-boot */
    led_init();
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
    }
    return 1;
}
