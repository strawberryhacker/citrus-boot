/* Copyright (C) strawberryhacker */

#include <c-boot/types.h>
#include <c-boot/led.h>
#include <c-boot/print.h>
#include <c-boot/boot.h>
#include <c-boot/packet.h>
#include <c-boot/hardware.h>
#include <c-boot/serial.h>

/*
 * Initializes the components needed by s-boot
 */
static void c_boot_init(void)
{
    /* Custom hardware initialization spesific to the board */
    hardware_init();

    /* Initilaize hardware used by c-boot */
    led_init();
    print_init();
    packet_init();

    asm volatile("cpsie ifa");

    /* 
     * Acknowledge startup. After this command the host will start sending the
     * kernel in 512 byte bursts. 
     */
    packet_respose(RESP_OK);
}

static void c_boot_deinit(void)
{
    asm volatile ("cpsid aif" : : : "memory");
}

/*
 * Entry point after device spesific startup code
 */
int main(void)
{
    c_boot_init();

    /* The load address is defined in the build config */
    load_kernel(LOAD_ADDR);

    /* Make a function pointer to jump to the kernel */
    void (*kernel)(u32 reserved, u32 info_addr, u32 mach);
    kernel = (void(*)(u32, u32, u32))LOAD_ADDR;

    /* Release all the resources except the kernel memory and clocks */
    c_boot_deinit();

    /* Start the kernel */
    u32 reserved = 0;
    u32 kernel_info = 0xC0DEBABE;
    u32 machine_type = 0xCAFECAFE;

    /* No need for barriers since the kernel starts with the same stack pointer */
    kernel(reserved, kernel_info, machine_type);
}
