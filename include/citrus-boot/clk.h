/// Copyright (C) strawberryhacker 

#ifndef CLK_H
#define CLK_H

#include <citrus-boot/types.h>

/// This implementation does not cover nesserary function in order to work in
/// the kernel. This only initializes system clocks in order to be able to 
/// donwload the kernel image from serial or and MMC device and load it to DDR
void clk_rst(void);
void clk_rc_enable(void);

enum clk_src {
    CLK_SRC_RC,
    CLK_SRC_CRYSTAL
};

void clk_mainck_sel(enum clk_src src);

void clk_plla_init(u8 mult, u8 startup_time, u8 div_enable);

void clk_plla_disable(void);

enum mck_src {
    MCK_SRC_SLOW_CLK,
    MCK_SRC_MAIN_CLK,
    MCK_SRC_PLLA_CLK,
    MCK_SRC_UPLL_CLK
};

enum gck_src {
    GCK_SRC_SLOW_CLK,
    GCK_SRC_MAIN_CLK,
    GCK_SRC_PLLA_CLK,
    GCK_SRC_UPLL_CLK,
    GCK_SRC_MCK_CLK,
    GCK_SRC_AUDIO_CLK,
};

enum mck_presc {
    MCK_PRESC_DISABLED,
    MCK_PRESC_DIV2,
    MCK_PRESC_DIV4,
    MCK_PRESC_DIV8,
    MCK_PRESC_DIV16,
    MCK_PRESC_DIV32,
    MCK_PRESC_DIV64
};

enum mck_div {
    MCK_DIV_DISABLED, // DDR not available 
    MCK_DIV_2,
    MCK_DIV_4,
    MCK_DIV_3         // PLLA clock will be divided 
};

/// WARNING: the if the div factor is set to 3 the PLLADIV MUST be set to one
/// WARNING: if the H64MX is greater than 83 MHz the h32mx_div_enable must be set
void clk_mck_init(enum mck_src src, u8 h32mx_div_enable,
    enum mck_presc pres, enum mck_div div);

void clk_pck_enable(u32 per_id);

void clk_pck_disable(u32 per_id);

void clk_gck_enable(u8 pid, enum gck_src src, u8 div);

#endif
