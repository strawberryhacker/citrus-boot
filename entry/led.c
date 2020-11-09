/// Copyright (C) strawberryhacker 

#include <citrus-boot/led.h>
#include <citrus-boot/regmap.h>
#include <citrus-boot/gpio.h>

static struct gpio c_boot_led = {
    .hw = GPIOA,
    .pin = 31
 };

void led_init(void)
{
    struct gpio_conf cfg = {
        .dir = GPIO_OUTPUT,
        .drive = GPIO_DRIVE_ME,
        .func = GPIO_FUNC_OFF,
        .pull = GPIO_NO_PULL
    };
    gpio_init(&c_boot_led, &cfg);
}

void led_set(u8 val)
{
    if (val) {
        gpio_set(&c_boot_led);
    } else {
        gpio_clear(&c_boot_led);
    }
}
