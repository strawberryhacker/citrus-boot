/* Copyright (C) strawberryhacker */

#include <c-boot/types.h>
#include <c-boot/led.h>
#include <c-boot/print.h>
#include <c-boot/boot.h>
#include <c-boot/packet.h>
#include <c-boot/hardware.h>

#define TIMEOUT 1000 /* 1 second */

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
    packet_init(TIMEOUT);
    print("Starting bootloader\n");

    asm volatile("cpsie ifa");
    packet_respose(RESP_OK); /* Acknowledge startup */
}

void jump_to_image(u32 addr) 
{
    ((void (*)(void))addr)();
}

/*
 * Entry point after device spesific startup code
 */
int main(void)
{
    c_boot_init();

    load_kernel(0x20000000);
    jump_to_image(0x20000000);

    return 1;
}
