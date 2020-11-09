/* Copyright (C) strawberryhacker */

#ifndef UART_H
#define UART_H

#include <c-boot/types.h>
#include <c-boot/regmap.h>
#include <c-boot/bitops.h>

enum sam5d2x_parity {
    PAR_EVEN,
    PAR_ODD,
    PAR_SPACE,
    PAR_MARK,
    PAR_NO
};

struct uart {
    u32 clk;
    u32 baud;
    enum sam5d2x_parity par;
};

#define UART_TX_IRQ   BIT(1)
#define UART_RX_IRQ   BIT(0)

void uart_init(struct uart_reg* hw, struct uart* conf);

void uart_write(struct uart_reg* hw, u8 data);

u8 uart_read_unsafe(struct uart_reg* hw);

void uart_irq_en(struct uart_reg* hw, u32 mask);

u32 uart_get_status(struct uart_reg* hw);

#endif
