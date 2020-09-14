/* Copyright (C) strawberryhacker */

#ifndef SAMA5D2X_MATRIX_X
#define SAMA5D2X_MATRIX_X

#include <c-boot/types.h>

enum matrix_bus {
    SAMA5D2X_H32MX,
    SAMA5D2X_H64MX
};

struct matrix_conn {
    u8 pid;
    enum matrix_bus bus;
};

enum matrix_bus sama5d2x_get_bus(u8 pid);

#endif
