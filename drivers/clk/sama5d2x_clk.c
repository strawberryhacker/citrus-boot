/* Copyright (C) strawberryhacker */

#include <regmap/sama5d2x.h>
#include <c-boot/bitops.h>

#include "sama5d2x_clk.h"

static inline void sama5d2x_clk_write_mor(u32 reg)
{
    reg &= ~((0b111 << 4) | (0xFF << 16));
    reg |= BIT(5);
    reg |= 0x370000;

    PMC->MOR = reg;
}

void sama5d2x_clk_rc_enable(void)
{
    u32 reg = PMC->MOR;
    if ((reg & BIT(3)) == 0) {
        reg |= BIT(3);
        sama5d2x_clk_write_mor(reg);
    }

    /* Check stability */
    while (!(PMC->SR & BIT(17)));
}

void sama5d2x_clk_rc_disable(void)
{
    /* Not implemented */
}

void sama5d2x_clk_mainck_sel(enum sama5d2x_clk_src src)
{
    sama5d2x_clk_mck_init(SAMA5D2X_MCK_SRC_MAIN_CLK, 1,
        SAMA5D2X_MCK_PRESC_DISABLED, SAMA5D2X_MCK_DIV_DISABLED);
    u32 reg = PMC->MOR;

    if (src == SAMA5D2X_CLK_SRC_CRYSTAL) {
        reg |= BIT(24);
    } else {
        reg &= ~BIT(24);
    }
    sama5d2x_clk_write_mor(reg);

    /* Wait for MCLK switch to complete */
    while (!(PMC->SR & BIT(16)));
}

static inline void sama5d2x_clk_wait_mck_ready(void)
{
    while (!(PMC->SR & BIT(3)));
}

void sama5d2x_clk_gck_enable(u8 pid, enum sama5d2x_gck_src src, u8 div)
{
    PMC->PCR = pid & 0x7F;  /* Write mode */
    u32 en = (PMC->PCR & BIT(28)) ? 1 : 0;
    PMC->PCR = (en << 28) | (src << 8) | BIT(12) | (div << 20) |
               BIT(29) | (pid & 0x7F);
}

/*
 * WARNING: the if the div factor is set to 3 the PLLADIV MUST be set to one
 * WARNING: if the H64MX is greater than 83 MHz the h32mx_div_enable must be set
 */
void sama5d2x_clk_mck_init(enum sama5d2x_mck_src src, u8 h32mx_div_enable,
    enum sama5d2x_mck_presc pres, enum sama5d2x_mck_div div)
{
    u32 reg;
    PMC->WPMR = 0x504D4300;

    /* Check the source */
    if (PMC->MOR & BIT(24)) {
        asm volatile ("bkpt");
    }

    if (h32mx_div_enable) {
        PMC->MCKR |= BIT(24);
    } else {
        PMC->MCKR &= ~BIT(24);
    }
    
    if ((src == SAMA5D2X_MCK_SRC_PLLA_CLK) || (src == SAMA5D2X_MCK_SRC_UPLL_CLK)) {
        reg = PMC->MCKR;
        reg &= ~(0b111 << 4);
        reg |= (pres << 4);
        PMC->MCKR = reg;
        sama5d2x_clk_wait_mck_ready();

        reg = PMC->MCKR;
        reg &= ~(0b11 << 8);
        reg |= (div << 8);
        PMC->MCKR = reg;
        sama5d2x_clk_wait_mck_ready();

        reg = PMC->MCKR;
        reg &= ~0b11;
        reg |= src;
        PMC->MCKR = reg;
        sama5d2x_clk_wait_mck_ready();
    } else {
        /* MAINCK can only be 24 MHz */
        reg = PMC->MCKR;
        reg &= ~0b11;
        reg |= src;
        PMC->MCKR = reg;
        sama5d2x_clk_wait_mck_ready();

        reg = PMC->MCKR;
        reg &= ~(0b111 << 4);
        reg |= (pres << 4);
        PMC->MCKR = reg;
        sama5d2x_clk_wait_mck_ready();
    }
}

/*
 * Enables the PLLA. The PLLA output before the divider has to be in the range
 * 600 MHz - 1200 MHz. The multiplication factor should not be 1 or lower. 
 */
void sama5d2x_clk_plla_init(u8 mult, u8 startup_time, u8 div_enable)
{
    /*
     * The ROM code does NOT relase the resources so we have to manually do it
     */
    sama5d2x_clk_mck_init(SAMA5D2X_MCK_SRC_MAIN_CLK, 1,
        SAMA5D2X_MCK_PRESC_DISABLED, SAMA5D2X_MCK_DIV_DISABLED);

    if (div_enable) {
        PMC->MCKR |= BIT(12);
    } else {
        PMC->MCKR &= ~BIT(12);
    }
    u32 reg = (((mult - 1) & 0x7F) << 18) | ((startup_time & 0x3F) << 8) |
              (1 << 29) | (1 << 0);

    PMC->PLLAR = reg;

    /* Wait for LOCKA */
    while (!(PMC->SR & (1 << 1)));
}

/*
 * Enables the clock for å peripheral. It takes in the periphal ID number
 * defined in the datasheet at page 57. This only turns on the clock and does 
 * not perform any configuration
 */
void sama5d2x_clk_pck_enable(u32 pid)
{
    if ((pid >= 2) && (pid < 32)) {
        PMC->PCER0 = (1 << pid);
    } else if ((pid >= 32) && (pid < 64)) {
        pid -= 32;
        PMC->PCER1 = (1 << pid);
    } else {
        PMC->PCR = pid & 0x7F;
        u32 reg = PMC->PCR;
        PMC->PCR = reg | BIT(12) | BIT(28);
    }
}

/*
 * Disables the clock for å peripheral. It takes in the periphal ID number
 * defined in the datasheet at page 57
 */
void sama5d2x_clk_pck_disable(u32 pid)
{
    if ((pid >= 2) && (pid < 32)) {
        PMC->PCDR0 = (1 << pid);
    } else if ((pid >= 32) && (pid < 64)) {
        pid -= 32;
        PMC->PCDR1 = (1 << pid);
    } else {
        PMC->PCR = pid & 0x7F;
        u32 reg = PMC->PCR;
        reg &= ~BIT(28);
        PMC->PCR = reg | BIT(12);
    }
}

/*
 * Enables the generic clock for a peripheral
 */
void sama5d2x_clk_gclk_enable(u32 pid)
{
    
}