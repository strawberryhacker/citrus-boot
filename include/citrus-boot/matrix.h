/// Copyright (C) strawberryhacker 

#ifndef MATRIX_X
#define MATRIX_X

#include <citrus-boot/types.h>

enum matrix_split {
    SPLIT_4K,
    SPLIT_8K,
    SPLIT_16K,
    SPLIT_32K,
    SPLIT_64K,
    SPLIT_128K,
    SPLIT_256K,
    SPLIT_512K,
    SPLIT_1M,
    SPLIT_2M,
    SPLIT_4M,
    SPLIT_8M,
    SPLIT_16M,
    SPLIT_32M,
    SPLIT_64M,
    SPLIT_128M
};

enum slave {
    AHB_BRIDGE,
    SDMMC,
    DDR_PORT0,
    DDR_PORT1,
    DDR_PORT2,
    DDR_PORT3,
    DDR_PORT4,
    DDR_PORT5,
    DDR_PORT6,
    DDR_PORT7,
    SRAM,
    SRAM_L2,
    QSPI0,
    QSPI1,
    AESB
};

struct matrix_reg* get_matrix(u8 pid);

u8 is_secure(struct matrix_reg* matrix, u8 pid);

void set_non_secure(u8 pid);

void set_secure(u8 pid);

// Matrix configuration 
void matrix_set_split(struct matrix_reg* matrix, enum slave s,
    enum matrix_split split, u8 reg);

void matrix_set_top(struct matrix_reg* matrix, enum slave s,
    enum matrix_split top, u8 reg);

void matrix_set_sec(struct matrix_reg* matrix, enum slave s,
    u8 low_non_sec, u8 w_non_sec, u8 r_non_sec);

#endif
