/* Copyright (C) strawberryhacker */

#ifndef SAMA5D2X_MATRIX_X
#define SAMA5D2X_MATRIX_X

#include <c-boot/types.h>

enum sama5d2x_matrix_split {
    SAMA5D2X_SPLIT_4K,
    SAMA5D2X_SPLIT_8K,
    SAMA5D2X_SPLIT_16K,
    SAMA5D2X_SPLIT_32K,
    SAMA5D2X_SPLIT_64K,
    SAMA5D2X_SPLIT_128K,
    SAMA5D2X_SPLIT_256K,
    SAMA5D2X_SPLIT_512K,
    SAMA5D2X_SPLIT_1M,
    SAMA5D2X_SPLIT_2M,
    SAMA5D2X_SPLIT_4M,
    SAMA5D2X_SPLIT_8M,
    SAMA5D2X_SPLIT_16M,
    SAMA5D2X_SPLIT_32M,
    SAMA5D2X_SPLIT_64M,
    SAMA5D2X_SPLIT_128M
};

enum sama5d2x_slave {
    SAMA5D2X_AHB_BRIDGE,
    SAMA5D2X_SDMMC,
    SAMA5D2X_DDR_PORT0,
    SAMA5D2X_DDR_PORT1,
    SAMA5D2X_DDR_PORT2,
    SAMA5D2X_DDR_PORT3,
    SAMA5D2X_DDR_PORT4,
    SAMA5D2X_DDR_PORT5,
    SAMA5D2X_DDR_PORT6,
    SAMA5D2X_DDR_PORT7,
    SAMA5D2X_SRAM,
    SAMA5D2X_SRAM_L2,
    SAMA5D2X_QSPI0,
    SAMA5D2X_QSPI1,
    SAMA5D2X_AESB
};

struct matrix_reg* sama5d2x_get_matrix(u8 pid);

u8 sama5d2x_is_secure(struct matrix_reg* matrix, u8 pid);

void sama5d2x_set_non_secure(u8 pid);

void sama5d2x_set_secure(u8 pid);

/* Matrix configuration */
void sama5d2x_matrix_set_split(struct matrix_reg* matrix, enum sama5d2x_slave s,
    enum sama5d2x_matrix_split split, u8 reg);

void sama5d2x_matrix_set_top(struct matrix_reg* matrix, enum sama5d2x_slave s,
    enum sama5d2x_matrix_split top, u8 reg);

void sama5d2x_matrix_set_sec(struct matrix_reg* matrix, enum sama5d2x_slave s,
    u8 low_non_sec, u8 w_non_sec, u8 r_non_sec);

#endif
