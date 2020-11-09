/// Copyright (C) strawberryhacker 

#include <citrus-boot/regmap.h>
#include <citrus-boot/bitops.h>
#include <citrus-boot/clk.h>

static inline void clk_write_mor(u32 reg)
{
    reg &= ~((0b111 << 4) | (0xFF << 16));
    reg |= BIT(5);
    reg |= 0x370000;

    PMC->MOR = reg;
}

void clk_rst(void)
{
    // Set master clock to mainclk (12 - 24 MHz) 
    clk_mck_init(MCK_SRC_MAIN_CLK, 0, 
        MCK_PRESC_DISABLED, MCK_DIV_DISABLED);
    
    clk_plla_disable();

    clk_mainck_sel(CLK_SRC_RC);
}

void clk_rc_enable(void)
{
    u32 reg = PMC->MOR;
    if ((reg & BIT(3)) == 0) {
        reg |= BIT(3);
        clk_write_mor(reg);
    }

    // Check stability 
    while (!(PMC->SR & BIT(17)));
}

void clk_mainck_sel(enum clk_src src)
{
    u32 reg = PMC->MOR;

    if (src == CLK_SRC_CRYSTAL) {
        reg |= BIT(24);
    } else {
        reg &= ~BIT(24);
    }
    clk_write_mor(reg);

    // Wait for MCLK switch to complete 
    while (!(PMC->SR & BIT(16)));
}

static inline void clk_wait_mck_ready(void)
{
    while (!(PMC->SR & BIT(3)));
}

void clk_gck_enable(u8 pid, enum gck_src src, u8 div)
{
    PMC->PCR = pid & 0x7F;  // Write mode 
    u32 en = (PMC->PCR & BIT(28)) ? 1 : 0;
    PMC->PCR = (en << 28) | (src << 8) | BIT(12) | (div << 20) |
               BIT(29) | (pid & 0x7F);
}

/// WARNING: the if the div factor is set to 3 the PLLADIV MUST be set to one
/// WARNING: if the H64MX is greater than 83 MHz the h32mx_div_enable must be set
void clk_mck_init(enum mck_src src, u8 h32mx_div_enable,
    enum mck_presc pres, enum mck_div div)
{
    u32 reg;
    if (h32mx_div_enable) {
        PMC->MCKR |= BIT(24);
    } else {
        PMC->MCKR &= ~BIT(24);
    }
    
    if ((src == MCK_SRC_PLLA_CLK) || (src == MCK_SRC_UPLL_CLK)) {
        reg = PMC->MCKR;
        reg &= ~(0b111 << 4);
        reg |= (pres << 4);
        PMC->MCKR = reg;
        clk_wait_mck_ready();

        reg = PMC->MCKR;
        reg &= ~(0b11 << 8);
        reg |= (div << 8);
        PMC->MCKR = reg;
        clk_wait_mck_ready();

        reg = PMC->MCKR;
        reg &= ~0b11;
        reg |= src;
        PMC->MCKR = reg;
        clk_wait_mck_ready();
    } else {
        // MAINCK can only be 24 MHz 
        reg = PMC->MCKR;
        reg &= ~0b11;
        reg |= src;
        PMC->MCKR = reg;
        clk_wait_mck_ready();

        reg = PMC->MCKR;
        reg &= ~(0b111 << 4);
        reg |= (pres << 4);
        PMC->MCKR = reg;
        clk_wait_mck_ready();
    }
}

/// Enables the PLLA. The PLLA output before the divider has to be in the range
/// 600 MHz - 1200 MHz. The multiplication factor should not be 1 or lower. 
void clk_plla_init(u8 mult, u8 startup_time, u8 div_enable)
{
    if (div_enable) {
        PMC->MCKR |= BIT(12);
    } else {
        PMC->MCKR &= ~BIT(12);
    }
    u32 reg = (((mult - 1) & 0x7F) << 18) | ((startup_time & 0x3F) << 8) |
              (1 << 29) | (1 << 0);

    PMC->PLLAR = reg;

    // Wait for LOCKA 
    while (!(PMC->SR & (1 << 1)));
}

void clk_plla_disable(void)
{
    PMC->PLLAR = 0;
}

/// Enables the clock for å peripheral. It takes in the periphal ID number
/// defined in the datasheet at page 57. This only turns on the clock and does 
/// not perform any configuration
void clk_pck_enable(u32 pid)
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

/// Disables the clock for å peripheral. It takes in the periphal ID number
/// defined in the datasheet at page 57
void clk_pck_disable(u32 pid)
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

/// Enables the generic clock for a peripheral
void clk_gclk_enable(u32 pid)
{
    
}
