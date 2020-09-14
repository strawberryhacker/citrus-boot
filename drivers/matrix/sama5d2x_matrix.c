/* Copyright (C) strawberryhacker */

#include "sama5d2x_matrix.h"

static struct matrix_conn per_matrix[] = {
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  1, .bus = SAMA5D2X_H32MX },
    {.pid =  2, .bus = SAMA5D2X_H32MX },
    {.pid =  3, .bus = SAMA5D2X_H32MX },
    {.pid =  4, .bus = SAMA5D2X_H32MX },
    {.pid =  5, .bus = SAMA5D2X_H32MX },
    {.pid =  6, .bus = SAMA5D2X_H32MX },
    {.pid =  7, .bus = SAMA5D2X_H32MX },
    {.pid =  8, .bus = SAMA5D2X_H32MX },
    {.pid =  9, .bus = SAMA5D2X_H32MX },
    {.pid = 10, .bus = SAMA5D2X_H32MX },
    {.pid = 11, .bus = SAMA5D2X_H32MX },
    {.pid = 12, .bus = SAMA5D2X_H32MX },
    {.pid = 13, .bus = SAMA5D2X_H32MX },
    {.pid = 14, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX },
    {.pid =  0, .bus = SAMA5D2X_H32MX }
};

enum matrix_bus sama5d2x_get_bus(u8 pid);
