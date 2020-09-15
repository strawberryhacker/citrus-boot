/* Copyright (C) strawberryhacker */

#include <c-boot/hardware.h>
#include <drivers/clk/sama5d2x_clk.h>
#include <drivers/wdt/sama5d2x_wdt.h>
#include <drivers/irq/sama5d2x_apic.h>
#include <c-boot/bitops.h>

/*
 * Remaps all secure interrupts to the non-secure APIC controller
 */
void secure_apic_remap(void)
{
    u32 key = SFR->SN1 ^ 0xB6D81C4D;
    SFR->AICREDIR = key | 1;
}

void hardware_init(void)
{
    /* Disable watchdog */
    sama5d2x_wdt_disable();

    /* Reset the clock tree */
    sama5d2x_clk_rst();

    /* Set up the clock to running full speed */
    sama5d2x_clk_rc_enable();
    sama5d2x_clk_mainck_sel(SAMA5D2X_CLK_SRC_RC);    /* RC at 24 MHz */
    sama5d2x_clk_plla_init(83, 0x3F, 1);             /* PLLA at 498 MHz */

    /*
     * Configure CPU clock to 498 MHz, master clock and H64MX clock to 166 MHz
     * and the H32MX to 83 MHz
     */
    sama5d2x_clk_mck_init(SAMA5D2X_MCK_SRC_PLLA_CLK, 1,
                          SAMA5D2X_MCK_PRESC_DISABLED, SAMA5D2X_MCK_DIV_3);

    /* Initialize the APIC */
    asm volatile ("cpsid if");
    sama5d2x_apic_init(APIC);
    sama5d2x_apic_init(SAPIC);
    secure_apic_remap();
    sama5d2x_apic_protect(APIC); /* Warning */
}

void hardware_deinit(void)
{
    /* Release all resources except DDR and DDR clock */
    
}
