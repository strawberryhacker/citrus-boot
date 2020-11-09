/// Copyright (C) strawberryhacker 

#include <citrus-boot/matrix.h>
#include <citrus-boot/bitops.h>
#include <citrus-boot/regmap.h>

#define PID_CNT 31

// Defines the PID numbers of the peripherals connected to the H64MX matrix 
static const u8 h64mx_table[] =
    { 2, 6, 7, 9, 10, 12, 13, 15, 31, 32, 45, 46, 52, 53, 63 };

/// Returns the connected bus matrix based on the PID number
struct matrix_reg* get_matrix(u8 pid)
{
    for (u8 i = 0; i < sizeof(h64mx_table); i++) {
        if (pid == h64mx_table[i]) {
            return H64MX;
        }
    }
    return H32MX;
}

/// Get the security configuration of a peripheral
u8 is_secure(struct matrix_reg* matrix, u8 pid)
{
     const u32* reg = (const u32 *)&matrix->SPSELR1;
     if (reg[pid / 32] & BIT(pid % 32)) {
         return 0;
     } else {
         return 1;
     }
}

void set_non_secure(u8 pid)
{
    struct matrix_reg* matrix = get_matrix(pid);
    u32* reg = (u32 *)&matrix->SPSELR1;
    reg[pid / 32] &= ~BIT(pid % 32);
}

void set_secure(u8 pid)
{
    struct matrix_reg* matrix = get_matrix(pid);
    u32* reg = (u32 *)&matrix->SPSELR1;
    reg[pid / 32] |= BIT(pid % 32);
}

void matrix_set_split(struct matrix_reg* matrix, enum slave s,
    enum matrix_split split, u8 reg)
{
    u32 tmp = matrix->SASSR[s];
    tmp &= ~(0xF << (reg * 4));
    tmp |= (split << (reg * 4));
    matrix->SASSR[s] = tmp;
}

void matrix_set_top(struct matrix_reg* matrix, enum slave s,
    enum matrix_split top, u8 reg)
{
    u32 tmp = matrix->SRTSR[s];
    tmp &= ~(0xF << (reg * 4));
    tmp |= (top << (reg * 4));
    matrix->SRTSR[s] = tmp;
}

void matrix_set_sec(struct matrix_reg* matrix, enum slave s,
    u8 low_non_sec, u8 w_non_sec, u8 r_non_sec)
{
    matrix->SSR[s] = low_non_sec | (r_non_sec << 8) | (w_non_sec << 16);
}
