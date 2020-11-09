///  Copyright (C) strawberryhacker 

/// This module implements basic sprintf functionality. The following commands 
/// are implemented
/// 
/// c - chars
/// x - hexadecimal lowercase
/// X - hexadecimal uppercase
/// u - unsigned integer
/// i - signed integer
/// b - binary
/// s - string
/// r - register
/// p - pointer lowercase
/// P - pointer uppercase
/// 
/// It upports # after % to add a prefix, and a number before the format option
/// to specify the minumum ouput characters. This module only supports 32-bit 
/// data types
///  
/// Thanks to ataradov for inspiration

#include <citrus-boot/sprint.h>

#define FMT_FLAG_TAG    0x01
#define FMT_FLAG_SIGN   0x02
#define FMT_FLAG_SMALL  0x20

// Table for converting decimal to ASCII number 
static const char hex_table[16] = "0123456789ABCDEF";

/// Prints a number with the given base and with optional flags to the buffer,
/// and returns the pointer to the next element in the buffer
static char* put_int(char* buf, i32 num, u8 flags, u8 base, u8 width)
{
	// Toggeling bit 5 switches between uppercase and lowercase 
    u8 lower = (flags & FMT_FLAG_SMALL) ? FMT_FLAG_SMALL : 0;
    u8 cnt = 0;

    char tmp[32];
    char* tmp_ptr = tmp;

    // Nagative number 
    if (flags & FMT_FLAG_SIGN) {
        *buf++ = '-';
        if (num < 0) {
            num = -num;
        }
    }

    // Print the number in the tmp buffer 
    u32 u_num = (u32)num;
    if (u_num) {
         while (u_num) {
            *tmp_ptr++ = lower | hex_table[u_num % base];
            u_num = u_num / base;
            cnt++;
        }
    } else {
        *buf++ = '0';
    }

    // Check for tag option 
    if (flags & FMT_FLAG_TAG) {
        *buf++ = '0';
        if (base == 2) {
            *buf++ = 'b';
        } else if (base == 16) {
            *buf++ = 'x';
        }
    }

    // Zero pad given the width is longer than the number 
    while (width-- > cnt) {
        *buf++ = '0';
    }

    // Reverse the buffer 
    while (--tmp_ptr >= tmp) {
        *buf++ = *tmp_ptr;
    }
	return buf;
}

static inline u8 is_digit(char c)
{
    return ((c >= '0') && (c <= '9')) ? 1 : 0;
}

static inline u8 char_to_digit(char c) {
    return c - '0';
}

u32 vsprint(char* buf, const char* fmt, va_list args)
{
    char* str;
    for (str = buf; *fmt; ++fmt) {

        // Skip if no formatting 
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }

        // Check for the # flag 
        u8 flags = 0;
        fmt++;
        if (*fmt == '#') {
            flags |= FMT_FLAG_TAG;
            fmt++;
        }

        // Get the width 
        i32 width = 0;
        if (*fmt == '*') {
            width = (i32)va_arg(args, int);
            fmt++;
        } else {
            // No number present gives a width of 0 
            while (is_digit(*fmt)) {
                width = width * 10 + char_to_digit(*fmt++);
            }
        }

        // Check the formatting option 
        u8 base = 0;
        switch(*fmt) {
            case 'r' : {
                width = 32;
                flags |= FMT_FLAG_TAG;
            }
            case 'b' : {
                base = 2;
                break;
            }
            case 'p' : {
                flags |= FMT_FLAG_SMALL;
            }
            case 'P' : {
                width = 8;
                base = 16;
                flags |= FMT_FLAG_TAG;
                break;
            }
            case 'x' : {
                flags |= FMT_FLAG_SMALL;
            }
            case 'X' : {
                base = 16;
                break;
            }
            case 'i' : {
                flags |= FMT_FLAG_SIGN;
            }
            case 'u' : {
                base = 10;
                break;
            }
            case 's' : {
                const char* s = va_arg(args, char*);
                while (width--) {
                    *str++ = *s++;
                }
                continue;
            }
            case 'c' : {
                *str++ = va_arg(args, int);
                continue;
            }
            case '%' : {
                *str++ = '%';
                continue;
            }
            default : {
                continue;
            }
        }

        // A number should be printed 
        i32 num = (i32)va_arg(args, long);
        str = put_int(str, num, flags, base, width);
    }
    return str - buf;
}

/// Formatted print to buffer
u32 sprint(char* buf, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    u32 cnt = vsprint(buf, fmt, args);
    va_end(args);

    return cnt;
}
