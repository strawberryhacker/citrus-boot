/* Copyright (C) strawberryhacker */

#ifndef SAMA5D2X_CLK_H
#define SAMA5D2X_CLK_H

#include <c-boot/types.h>

/*
 * This implementation does not cover nesserary function in order to work in
 * the kernel. This only initializes system clocks in order to be able to 
 * donwload the kernel image from serial or and MMC device and load it to DDR
 */

void sama5d2x_clk_rc_enable(void);
void sama5d2x_clk_rc_disable(void);

enum sama5d2x_clk_src {
    SAMA5D2X_CLK_SRC_RC,
    SAMA5D2X_CLK_SRC_CRYSTAL
};

void sama5d2x_clk_mainck_sel(enum sama5d2x_clk_src src);

void sama5d2x_clk_plla_init(u8 mult, u8 startup_time, u8 div_enable);

enum sama5d2x_mck_src {
    SAMA5D2X_MCK_SRC_SLOW_CLK,
    SAMA5D2X_MCK_SRC_MAIN_CLK,
    SAMA5D2X_MCK_SRC_PLLA_CLK,
    SAMA5D2X_MCK_SRC_UPLL_CLK
};

enum sama5d2x_gck_src {
    SAMA5D2X_GCK_SRC_SLOW_CLK,
    SAMA5D2X_GCK_SRC_MAIN_CLK,
    SAMA5D2X_GCK_SRC_PLLA_CLK,
    SAMA5D2X_GCK_SRC_UPLL_CLK,
    SAMA5D2X_GCK_SRC_MCK_CLK,
    SAMA5D2X_GCK_SRC_AUDIO_CLK,
};

enum sama5d2x_mck_presc {
    SAMA5D2X_MCK_PRESC_DISABLED,
    SAMA5D2X_MCK_PRESC_DIV2,
    SAMA5D2X_MCK_PRESC_DIV4,
    SAMA5D2X_MCK_PRESC_DIV8,
    SAMA5D2X_MCK_PRESC_DIV16,
    SAMA5D2X_MCK_PRESC_DIV32,
    SAMA5D2X_MCK_PRESC_DIV64
};

enum sama5d2x_mck_div {
    SAMA5D2X_MCK_DIV_DISABLED, /* DDR not available */
    SAMA5D2X_MCK_DIV_2,
    SAMA5D2X_MCK_DIV_4,
    SAMA5D2X_MCK_DIV_3         /* PLLA clock will be divided */
};

/*
 * WARNING: the if the div factor is set to 3 the PLLADIV MUST be set to one
 * WARNING: if the H64MX is greater than 83 MHz the h32mx_div_enable must be set
 */
void sama5d2x_clk_mck_init(enum sama5d2x_mck_src src, u8 h32mx_div_enable,
    enum sama5d2x_mck_presc pres, enum sama5d2x_mck_div div);

void sama5d2x_clk_pck_enable(u32 per_id);

void sama5d2x_clk_pck_disable(u32 per_id);

void sama5d2x_clk_gck_enable(u8 pid, enum sama5d2x_gck_src src, u8 div);

#endif
