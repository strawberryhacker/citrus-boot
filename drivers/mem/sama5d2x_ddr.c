/* Copyright (C) strawberryhacker */

#include "sama5d2x_ddr.h"
#include <regmap/sama5d2x.h>
#include <c-boot/bitops.h>

#define DDR2_BASE 0x20000000 

void sama5d2x_ddr2_wait(u32 us)
{
    /* The max operation is 500 MHz so 500 clocks per us is good enough */
    for (volatile u32 i = 0; i < 500; i++) {
        asm volatile ("nop");
    }
}

/*
 * Performs a normal command sequence; writes the command to the mode register,
 * reads the mode register and adds a data memory barrier after it. This 
 * function also performs a write operation to a specified address
 */
void sama5d2x_ddr2_cmd(u8 cmd, u32 addr)
{
    DDR->MR = cmd & 0b111;
    (volatile void)DDR->MR;
    asm volatile ("dmb" : : : "memory");
    if (addr) {
        *(volatile u32 *)addr = 0;
    }

    sama5d2x_ddr2_wait(1);
}

void sama5d2x_ddr2_init(void)
{
    /* 10 address bits and 16-bit access in interleaved mode */
    u32 ba = 11;

    /* Step 1 */
    u32 reg = DDR->MD;
    reg &= ~0b111;
    reg |= 0x6 | BIT(4);
    DDR->MD = reg;

    /* Step 2 - timing */
    DDR->CR = 1 | (2 << 2) | (3 << 4) | (1 << 8) | (7 << 12) |
              (1 << 20) | (1 << 22) | (1 << 23);
    DDR->TPR0 = (7 << 0) | (3 << 4) | (3 << 8) | (9 << 12) | 
                (3 << 16) | (2 << 20) | (2 << 24) | (2 << 28);
    DDR->TPR1 = (22 << 0) | (23 << 8) | (200 << 16) | (2 << 24);
    DDR->TPR2 = (2 << 0) | (8 << 4) | (4 << 8) | (2 << 12) | (8 << 16);

    /* Step 3 */
    sama5d2x_ddr2_cmd(1, DDR2_BASE);

    /* Step 4 */
    sama5d2x_ddr2_wait(200);

    /* Step 5 */
    sama5d2x_ddr2_cmd(1, DDR2_BASE);

    /* Step 6 */
    sama5d2x_ddr2_cmd(2, DDR2_BASE);

    /* Step 7 */
    sama5d2x_ddr2_cmd(5, DDR2_BASE + (2 << 12));

    /* Step 8 */
    sama5d2x_ddr2_cmd(5, DDR2_BASE + (3 << 12));

    /* Step 9 */
    sama5d2x_ddr2_cmd(5, DDR2_BASE + (1 << 12));

    /* Step 10 */
    sama5d2x_ddr2_wait(200);

    /* Step 11 */
    DDR->CR |= BIT(7);

    /* Step 12 */
    sama5d2x_ddr2_cmd(3, DDR2_BASE + (0 << 12));

    /* Step 13 */
    sama5d2x_ddr2_cmd(2, DDR2_BASE);

    /* Step 14 */
    sama5d2x_ddr2_cmd(4, DDR2_BASE);
    sama5d2x_ddr2_cmd(4, DDR2_BASE);

    /* Step 15 */
    DDR->CR &= ~BIT(7);

    /* Step 16 */
    sama5d2x_ddr2_cmd(3, DDR2_BASE + (0 << 12));

    /* Step 17 */
    reg = DDR->CR;
    reg &= ~(0b111 << 12);
    reg |= (7 << 12);
    DDR->CR = reg;

    /* Step 18 */
    sama5d2x_ddr2_cmd(5, DDR2_BASE + (1 << 12));

    /* Step 19 */
    reg = DDR->CR;
    reg &= ~(0b111 << 12);
    DDR->CR = reg;

    /* Step 20 */
    sama5d2x_ddr2_cmd(5, DDR2_BASE + (1 << 12));

    /* Step 21 */
    sama5d2x_ddr2_cmd(0, DDR2_BASE);

    DDR->RTR = 1328;
}
