/* Copyright (C) strawberryhacker */

#ifndef SAMA5D2X_GPIO_H
#define SAMA5D2X_GPIO_H

#include <s-boot/types.h>
#include <regmap/sama5d2x.h>

struct sama5d2x_gpio {
    struct gpio_reg* hw;
    u32 pin;
};

/*
 * Private functions for the other device spesific drivers
 */
void gpio_set_func(struct sama5d2x_gpio* gpio);

#endif
