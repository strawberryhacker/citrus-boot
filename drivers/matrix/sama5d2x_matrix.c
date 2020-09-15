/* Copyright (C) strawberryhacker */

#include "sama5d2x_matrix.h"
#include <regmap/sama5d2x.h>
#include <c-boot/bitops.h>

#define PID_CNT 31

/* Defines the PID numbers of the peripherals connected to the H64MX matrix */
static const u8 h64mx_table[] =
    { 2, 6, 7, 9, 10, 12, 13, 15, 31, 32, 45, 46, 52, 53, 63 };

/*
 * Returns the connected bus matrix based on the PID number
 */
struct matrix_reg* sama5d2x_get_matrix(u8 pid)
{
    for (u8 i = 0; i < sizeof(h64mx_table); i++) {
        if (pid == h64mx_table[i]) {
            return H64MX;
        }
    }
    return H32MX;
}

/*
 * Get the security configuration of a peripheral
 */
u8 sama5d2x_is_secure(struct matrix_reg* matrix, u8 pid)
{
     const u32* reg = (const u32 *)&matrix->SPSELR1;
     if (reg[pid / 32] & BIT(pid % 32)) {
         return 0;
     } else {
         return 1;
     }
}
