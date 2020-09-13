/* Copyright (C) strawberryhacker */

#include <s-boot/types.h>
#include <s-boot/led.h>

/*
 * Initializes the components needed by s-boot
 */
static void hardware_init(void)
{
    led_init();
}

/*
 * Entry point after device spesific startup code
 */
int main(void)
{
    hardware_init();

    while (1) {

    }
    return 1;
}
