/* Copyright (C) strawberryhacker */

#include <c-boot/led.h>
#include "sama5d2x_gpio.h"

static inline void sama5d2x_gpio_mask(const struct sama5d2x_gpio* gpio)
{
    gpio->hw->MSKR = (1 << gpio->pin);
}

void sama5d2x_gpio_init(const struct sama5d2x_gpio* gpio,
    const struct sama5d2x_gpio_conf* conf)
{
    sama5d2x_gpio_mask(gpio);
    u32 cfg = (conf->dir << 8) | (conf->drive << 16) | (conf->event << 24) |
              (conf->func << 0);
    
    if (conf->pull == SAMA5D2X_GPIO_PULLDOWN) {
        cfg |= (1 << 10);
    } else if (conf->pull == SAMA5D2X_GPIO_PULLUP) {
        cfg |= (1 << 9);
    }
    gpio->hw->CFGR = cfg;
} 

void sama5d2x_gpio_set_func(const struct sama5d2x_gpio* gpio,
    enum sama5d2x_gpio_func func)
{
    sama5d2x_gpio_mask(gpio);
    u32 cfg = gpio->hw->CFGR;
    cfg &= ~0b111;
    cfg |= func;
    gpio->hw->CFGR = cfg;
}

void sama5d2x_gpio_set_dir(const struct sama5d2x_gpio* gpio,
    enum sama5d2x_gpio_dir dir)
{
    sama5d2x_gpio_mask(gpio);
    u32 cfg = gpio->hw->CFGR;
    cfg &= ~(1 << 8);
    cfg |= (dir << 8);
    gpio->hw->CFGR = cfg;
}

void sama5d2x_gpio_set_event(const struct sama5d2x_gpio* gpio,
    enum sama5d2x_gpio_event event)
{
    sama5d2x_gpio_mask(gpio);
    u32 cfg = gpio->hw->CFGR;
    cfg &= ~(0b111 << 24);
    cfg |= (event << 24);
    gpio->hw->CFGR = cfg;
}

void sama5d2x_gpio_set_pull(const struct sama5d2x_gpio* gpio,
    enum sama5d2x_gpio_pull pull)
{
    sama5d2x_gpio_mask(gpio);
    u32 cfg = gpio->hw->CFGR;
    cfg &= ~(0b11 << 9);

    if (pull == SAMA5D2X_GPIO_PULLDOWN) {
        cfg |= (1 << 10);
    } else if (pull == SAMA5D2X_GPIO_PULLUP) {
        cfg |= (1 << 9);
    }
    gpio->hw->CFGR = cfg;
}

void sama5d2x_gpio_set_drive(const struct sama5d2x_gpio* gpio,
    enum sama5d2x_gpio_drive drive)
{
    sama5d2x_gpio_mask(gpio);
    u32 cfg = gpio->hw->CFGR;
    cfg &= ~(0b11 << 16);
    cfg |= (drive << 16);
    gpio->hw->CFGR = cfg;
}

void sama5d2x_gpio_set(const struct sama5d2x_gpio* gpio)
{
    gpio->hw->SODR = (1 << gpio->pin);
}

void sama5d2x_gpio_clear(const struct sama5d2x_gpio* gpio)
{
    gpio->hw->CODR = (1 << gpio->pin);
}

void sama5d2x_gpio_toggle(const struct sama5d2x_gpio* gpio)
{
    if (gpio->hw->ODSR & (1 << gpio->pin)) {
        gpio->hw->CODR = (1 << gpio->pin);
    } else {
        gpio->hw->SODR = (1 << gpio->pin);
    }
}

/*
 * Returns the value driven on the GPIO
 */
u32 sama5d2x_gpio_get_out(const struct sama5d2x_gpio* gpio)
{
    return gpio->hw->ODSR;
}

/*
 * Returns the value present on the GPIO
 */
u32 sama5d2x_gpio_get_in(const struct sama5d2x_gpio* gpio)
{
    return gpio->hw->PDSR;
}

void sama5d2x_gpio_irq_enable(const struct sama5d2x_gpio* gpio)
{
    gpio->hw->IER = (1 << gpio->pin);
}
