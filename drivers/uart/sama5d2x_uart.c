/* Copyright (C) strawberryhacker */

#include "sama5d2x_uart.h"
#include <c-boot/bitops.h>

void sama5d2x_uart_init(struct uart_reg* hw, struct sama5d2x_uart* conf)
{
    /* Reset the transmitter and the reciever */
    hw->CR = BIT(2) | BIT(3) | BIT(8);

    /* Set mode - all except parity and baud rate is default */
    hw->MR = (conf->par << 9);
    u16 cd = (u16)(conf->clk / 16 / conf->baud);
    hw->BRGR = cd;

    hw->RTOR = 0;
    hw->CMPR = 0;
    
    /* Enable the receiver and the transmitter */
    hw->CR = BIT(4) | BIT(6);
}

void sama5d2x_uart_write(struct uart_reg* hw, u8 data)
{
    /* Check is the buffer is empty */
    while (!(hw->SR & BIT(9)));

    hw->THR = data;
}

u8 sama5d2x_uart_read_unsafe(struct uart_reg* hw)
{
    return hw->RHR;
}

void sama5d2x_uart_irq_en(struct uart_reg* hw, u32 mask)
{
    hw->IER = mask;
}
