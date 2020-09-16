/* Copyright (C) strawberryhacker */

#ifndef SAMA5D2X_UART_H
#define SAMA5D2X_UART_H

#include <c-boot/types.h>
#include <regmap/sama5d2x.h>
#include <c-boot/bitops.h>

enum sam5d2x_parity {
    SAMA5D2X_PAR_EVEN,
    SAMA5D2X_PAR_ODD,
    SAMA5D2X_PAR_SPACE,
    SAMA5D2X_PAR_MARK,
    SAMA5D2X_PAR_NO
};

struct sama5d2x_uart {
    u32 clk;
    u32 baud;
    enum sam5d2x_parity par;
};

#define SAMA5D2X_UART_TX_IRQ   BIT(1)
#define SAMA5D2X_UART_RX_IRQ   BIT(0)

void sama5d2x_uart_init(struct uart_reg* hw, struct sama5d2x_uart* conf);

void sama5d2x_uart_write(struct uart_reg* hw, u8 data);

u8 sama5d2x_uart_read_unsafe(struct uart_reg* hw);

void sama5d2x_uart_irq_en(struct uart_reg* hw, u32 mask);

u32 sama5d2x_uart_get_status(struct uart_reg* hw);

#endif
