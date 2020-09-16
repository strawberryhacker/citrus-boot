/* Copyright (C) strawberryhacker */

#include <c-boot/timer.h>
#include <drivers/clk/sama5d2x_clk.h>
#include <drivers/irq/sama5d2x_apic.h>
#include <drivers/timer/sama5d2x_pit.h>
#include <stddef.h>
#include <c-boot/print.h>

static void (*timeout_callback)(void) = NULL;

static volatile u32 tick = 0;
static volatile u32 top = 0;
static volatile u32 timer_active = 0;

static void tick_handler(void)
{
    sama5d2x_pit_get_value();
    tick++;
    if (timer_active) {
        if (tick == top) {
            if (timeout_callback) {
                timeout_callback();
            }
        }
    }
}

void timer_init(u32 ms)
{
    sama5d2x_apic_irq_init(3, SAMA5D2X_APIC_PRI_1, 0, tick_handler);
    sama5d2x_apic_enable(3);

    struct sama5d2x_pit_conf conf = { .irq_en = 1, .per = 5375 };
    sama5d2x_pit_init(&conf);
    sama5d2x_pit_enable();

    top = ms;
    timer_active = 0;
}

void timer_restart(void)
{
    timer_active = 1;
    tick = 0;
}

void timer_stop(void)
{
    timer_active = 0;
}

void timer_add_handler(void (*func)(void))
{
    timeout_callback = func;
}
