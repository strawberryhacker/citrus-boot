/* Copyright (C) strawberryhacker */

#ifndef SAMA5D2X_APIC_H
#define SAMA5D2X_APIC_H

#include <regmap/sama5d2x.h>
#include <c-boot/types.h>

/* Priority table for the APIC controller. PRI_7 is highest. PRI_0 is lowest */
enum sama5d2x_apic_pri {
    SAMA5D2X_APIC_PRI_7,
    SAMA5D2X_APIC_PRI_6,
    SAMA5D2X_APIC_PRI_5,
    SAMA5D2X_APIC_PRI_4,
    SAMA5D2X_APIC_PRI_3,
    SAMA5D2X_APIC_PRI_2,
    SAMA5D2X_APIC_PRI_1,
    SAMA5D2X_APIC_PRI_0
};

enum sama5d2x_apic_event {
    SAMA5D2X_APIC_EVENT_INT_LEVEL,
    SAMA5D2X_APIC_EVENT_EXT_NEG_EDGE,
    SAMA5D2X_APIC_EVENT_EXT_HIGH_LEVEL,
    SAMA5D2X_APIC_EVENT_INT_EXT_POS_EDGE
};

void sama5d2x_apic_init(struct apic_reg* apic);

void sama5d2x_apic_irq_init(u8 irq, enum sama5d2x_apic_pri pri,
    enum sama5d2x_apic_event event, void (*func)(void));

void sama5d2x_apic_protect(struct apic_reg* apic);

void sama5d2x_apic_enable(u8 irq);

void sama5d2x_apic_disable(u8 irq);

#endif
