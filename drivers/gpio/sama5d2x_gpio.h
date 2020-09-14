/* Copyright (C) strawberryhacker */

#ifndef SAMA5D2X_GPIO_H
#define SAMA5D2X_GPIO_H

#include <c-boot/types.h>
#include <regmap/sama5d2x.h>

enum sama5d2x_gpio_func {
    SAMA5D2X_GPIO_FUNC_OFF,
    SAMA5D2X_GPIO_FUNC_A,
    SAMA5D2X_GPIO_FUNC_B,
    SAMA5D2X_GPIO_FUNC_C,
    SAMA5D2X_GPIO_FUNC_D,
    SAMA5D2X_GPIO_FUNC_E,
    SAMA5D2X_GPIO_FUNC_F,
    SAMA5D2X_GPIO_FUNC_G
};

enum sama5d2x_gpio_dir {
    SAMA5D2X_GPIO_INPUT,
    SAMA5D2X_GPIO_OUTPUT
};

enum sama5d2x_gpio_pull {
    SAMA5D2X_GPIO_PULLUP,
    SAMA5D2X_GPIO_PULLDOWN,
    SAMA5D2X_GPIO_NO_PULL
};

enum sama5d2x_gpio_drive {
    SAMA5D2X_GPIO_DIRVE_LO,
    SAMA5D2X_GPIO_DRIVE_ME,
    SAMA5D2X_GPIO_DRIVE_HI
};

enum sama5d2x_gpio_event {
    SAMA5D2X_GPIO_EVENT_FALLING,
    SAMA5D2X_GPIO_EVENT_RISING,
    SAMA5D2X_GPIO_EVENT_BOTH,
    SAMA5D2X_GPIO_EVENT_LOW,
    SAMA5D2X_GPIO_EVENT_HIGH
};

struct sama5d2x_gpio {
    struct gpio_reg* hw;
    u32 pin;
};

struct sama5d2x_gpio_conf {
    enum sama5d2x_gpio_func func;
    enum sama5d2x_gpio_dir dir;
    enum sama5d2x_gpio_event event;
    enum sama5d2x_gpio_pull pull;
    enum sama5d2x_gpio_drive drive;
};

void sama5d2x_gpio_init(const struct sama5d2x_gpio* gpio,
    const struct sama5d2x_gpio_conf* conf); 

void sama5d2x_gpio_set_func(const struct sama5d2x_gpio* gpio,
    enum sama5d2x_gpio_func func);

void sama5d2x_gpio_set_dir(const struct sama5d2x_gpio* gpio,
    enum sama5d2x_gpio_dir dir);

void sama5d2x_gpio_set_event(const struct sama5d2x_gpio* gpio,
    enum sama5d2x_gpio_event event);

void sama5d2x_gpio_set_pull(const struct sama5d2x_gpio* gpio,
    enum sama5d2x_gpio_pull pull);

void sama5d2x_gpio_set_drive(const struct sama5d2x_gpio* gpio,
    enum sama5d2x_gpio_drive drive);

void sama5d2x_gpio_set(const struct sama5d2x_gpio* gpio);

void sama5d2x_gpio_clear(const struct sama5d2x_gpio* gpio);

void sama5d2x_gpio_toggle(const struct sama5d2x_gpio* gpio);

u32 sama5d2x_gpio_get_out(const struct sama5d2x_gpio* gpio);

u32 sama5d2x_gpio_get_in(const struct sama5d2x_gpio* gpio);

void sama5d2x_gpio_irq_enable(const struct sama5d2x_gpio* gpio);

#endif
