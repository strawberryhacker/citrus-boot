/* Copyright (C) strawberryhacker */

#include <c-boot/uart.h>
#include <c-boot/bitops.h>

void uart_init(struct uart_reg* hw, struct uart* conf)
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

void uart_write(struct uart_reg* hw, u8 data)
{
    /* Check is the buffer is empty */
    while (!(hw->SR & BIT(9)));

    hw->THR = data;
}

u8 uart_read_unsafe(struct uart_reg* hw)
{
    return hw->RHR;
}

void uart_irq_en(struct uart_reg* hw, u32 mask)
{
    hw->IER = mask;
}

u32 uart_get_status(struct uart_reg* hw)
{
    return hw->SR;
}
