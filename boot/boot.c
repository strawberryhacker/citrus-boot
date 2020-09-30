/* Copyright (C) strawberryhacker */

#include <c-boot/boot.h>
#include <c-boot/print.h>
#include <c-boot/packet.h>
#include <c-boot/led.h>
#include <stddef.h>

/* Fast memory copy */
static void _memcpy(const void* src, void* dest, u32 size)
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

/* Fast memory compare. Returns 1 is the memory regions are equal */
static u8 _memcmp(const void* src, void* dest, u32 size)
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

static u8 load_page(u32 addr, u8* buffer, u32 size)
{
    _memcpy(buffer, (void *)addr, size);
    return _memcmp(buffer, (void *)addr, size);
}

/* Defines the different commands that might occur in a frame from the host */
#define CMD_WRITE_PAGE       0x04
#define CMD_RESET            0x06

/*
 * This functions tries to load the image to the address specified. Every 
 * written page is verified. If the CPU cannot write a page to memory, it will
 * send the response to the host. If a message was received with faults, the 
 * physical layer will handle it.
 * 
 * If this function returns, the board has received a complete image from the
 * host. This is a number of full pages pluss a ZLP or a short packet. The
 * bootloader can start executing the image at this point
 */
void load_kernel(u32 addr)
{
    struct packet* packet;
    u32 write_addr = addr;

    while (1) {
        packet = get_packet();
        if (!packet) {
            continue;
        }
        /* Process the packets */
        if (packet->cmd == CMD_RESET) {
            /* The host will try loading again so just reset the load address */
            write_addr = addr;
            led_set(0);
        } else if (packet->cmd == CMD_WRITE_PAGE) {
            /*
             * The kernel load destination must be memory mapped. Therefore a
             * board spesific write page function is not needed. However, the 
             * board spesific hardware initialization should make sure the 
             * address is accessable and does not provide any memory faults 
             */
            if(!load_page(write_addr, packet->data, packet->size)) {
                packet_respose(RESP_BOOT_ERROR);
                led_set(1);
                continue;
            }
            /* Last packet is a short packet */      
            if (packet->size != 512) break;
            write_addr += packet->size;
        }
        packet_respose(RESP_OK);
    }
    packet_respose(RESP_OK);
}

u32 get_kernel_size(u32 addr)
{
    /* The first work in the executable should hold the kernel size */
    return *(volatile u32 *)addr;
}
