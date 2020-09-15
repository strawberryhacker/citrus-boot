/* Copyright (C) strawberryhacker */

#ifndef SAMA5D2X_MATRIX_X
#define SAMA5D2X_MATRIX_X

#include <c-boot/types.h>

struct matrix_reg* sama5d2x_get_matrix(u8 pid);

u8 sama5d2x_is_secure(struct matrix_reg* matrix, u8 pid);

#endif
