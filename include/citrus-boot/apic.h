/// Copyright (C) strawberryhacker 

#ifndef APIC_H
#define APIC_H

#include <citrus-boot/regmap.h>
#include <citrus-boot/types.h>

// Priority table for the APIC controller. PRI_7 is highest. PRI_0 is lowest 
enum apic_pri {
    APIC_PRI_7,
    APIC_PRI_6,
    APIC_PRI_5,
    APIC_PRI_4,
    APIC_PRI_3,
    APIC_PRI_2,
    APIC_PRI_1,
    APIC_PRI_0
};

enum apic_event {
    APIC_EVENT_INT_LEVEL,
    APIC_EVENT_EXT_NEG_EDGE,
    APIC_EVENT_EXT_HIGH_LEVEL,
    APIC_EVENT_INT_EXT_POS_EDGE
};

void apic_init(struct apic_reg* apic);

void apic_irq_init(u8 irq, enum apic_pri pri, enum apic_event event,
    void (*func)(void));

void apic_protect(struct apic_reg* apic);

void apic_enable(u8 irq);

void apic_disable(u8 irq);

#endif
