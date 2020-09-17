/* Copyright (C) strawberryhacker */

#include <c-boot/hardware.h>
#include <drivers/clk/sama5d2x_clk.h>
#include <drivers/wdt/sama5d2x_wdt.h>
#include <drivers/irq/sama5d2x_apic.h>
#include <drivers/mem/sama5d2x_ddr.h>
#include <drivers/matrix/sama5d2x_matrix.h>

#include <c-boot/bitops.h>
#include <c-boot/print.h>

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

    /* Enable the DDR clock */
    sama5d2x_clk_pck_enable(13);
    PMC->SCER = BIT(2);

    /* Internal SRAM - one region - 128 kB - no split */
    sama5d2x_matrix_set_sec(H64MX, SAMA5D2X_SRAM, 0xFF, 0xFF, 0xFF);
    sama5d2x_matrix_set_split(H64MX, SAMA5D2X_SRAM, SAMA5D2X_SPLIT_128K, 0);
    sama5d2x_matrix_set_top(H64MX, SAMA5D2X_SRAM, SAMA5D2X_SPLIT_128K, 0);

    /* External DDR2 all ports - one region - 128 MB - no split */
    for (u32 i = 0; i < 8; i++) {
        sama5d2x_matrix_set_sec(H64MX, SAMA5D2X_DDR_PORT0 + i, 0xFF, 0xFF, 0xFF);
        sama5d2x_matrix_set_split(H64MX, SAMA5D2X_DDR_PORT0 + i, SAMA5D2X_SPLIT_128M, 0);
        sama5d2x_matrix_set_top(H64MX, SAMA5D2X_DDR_PORT0 + i, SAMA5D2X_SPLIT_128M, 0);
    }

    /* Initialize DDR2 RAM the MCK frequency must be greater than 125 MHz */
    sama5d2x_ddr2_init();

    /* Initialize the APIC */
    asm volatile ("cpsid if");
    sama5d2x_apic_init(APIC);
    sama5d2x_apic_protect(APIC); /* Warning */
    sama5d2x_apic_init(SAPIC);
    secure_apic_remap();
}

void hardware_deinit(void)
{
    /* Release all resources except DDR and DDR clock */
    
}
