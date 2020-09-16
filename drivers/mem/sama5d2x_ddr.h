/* Copyright (C) strawberryhacker */

#ifndef SAMA5D2X_DDR_H
#define SAMA5D2X_DDR_H

#include <c-boot/types.h>

/*
 * The clock must be initialized before calling this function
 */
void sama5d2x_ddr2_init(void);

#endif
