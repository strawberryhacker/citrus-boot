/* Copyright (C) strawberryhacker */

#include <c-boot/boot.h>
#include <c-boot/print.h>
#include <stddef.h>

void _memcpy(const void* src, void* dest, u32 size)
{
    const volatile u32* src_ptr = (const volatile u32 *)src;
    volatile u32* dest_ptr = (volatile u32 *)dest;

    while (size) {
        *dest_ptr++ = *src_ptr++;
        size -= 4;
    }
    if (size != 0) {
        size += 4;
        const volatile u8* src_ptr_b = (const volatile u8 *)src_ptr;
        volatile u8* dest_ptr_b = (volatile u8 *)dest_ptr;
        
        while (size--) {
            *dest_ptr_b++ = *src_ptr_b++;
        }
    }
}

u8 _memcmp(const void* src, void* dest, u32 size)
{
    const volatile u32* src_ptr = (const volatile u32 *)src;
    volatile u32* dest_ptr = (volatile u32 *)dest;

    while (size) {
        if (*dest_ptr++ != *src_ptr++) {
            return 0;
        }
        size -= 4;
    }
    if (size != 0) {
        size += 4;
        const volatile u8* src_ptr_b = (const volatile u8 *)src_ptr;
        volatile u8* dest_ptr_b = (volatile u8 *)dest_ptr;
        
        while (size--) {
            if (*dest_ptr_b++ != *src_ptr_b++) {
                return 0;
            }
        }
    }
    return 1;
}

u8 load_page(u32 addr, u8* buffer, u32 size)
{
    _memcpy(buffer, (void *)addr, size);
    return _memcmp(buffer, (void *)addr, size);
}
