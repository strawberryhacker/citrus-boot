/* Copyright (C) strawberryhacker */

#include <c-boot/led.h>
#include <regmap/sama5d2x.h>
#include <drivers/gpio/sama5d2x_gpio.h>

static struct sama5d2x_gpio c_boot_led = {
    .hw = GPIOA,
    .pin = 31
 };

void led_init(void)
{
    struct sama5d2x_gpio_conf cfg = {
        .dir = SAMA5D2X_GPIO_OUTPUT,
        .drive = SAMA5D2X_GPIO_DRIVE_ME,
        .func = SAMA5D2X_GPIO_FUNC_OFF,
        .pull = SAMA5D2X_GPIO_NO_PULL
    };
    sama5d2x_gpio_init(&c_boot_led, &cfg);
}

void led_set(u8 val)
{
    if (val) {
        sama5d2x_gpio_set(&c_boot_led);
    } else {
        sama5d2x_gpio_clear(&c_boot_led);
    }
}
