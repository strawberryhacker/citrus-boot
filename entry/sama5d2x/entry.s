/* Copyright (C) StrawberryHacker */

.syntax unified
.cpu cortex-a5
.arm

.global vector_table
.section .vector_table, "ax", %progbits
vector_table:
    .word 0
