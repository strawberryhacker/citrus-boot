/* Copyright (C) strawberryhacker */

#include <c-boot/types.h>
#include <c-boot/led.h>
#include <c-boot/hardware.h>
#include <c-boot/serial.h>
#include <c-boot/print.h>
#include <c-boot/bitops.h>
#include <c-boot/timer.h>
#include <c-boot/crc.h>
#include <c-boot/boot.h>
#include <c-boot/frame.h>

#include <drivers/irq/sama5d2x_apic.h>
#include <drivers/gpio/sama5d2x_gpio.h>
#include <drivers/clk/sama5d2x_clk.h>
#include <drivers/uart/sama5d2x_uart.h>
#include <drivers/timer/sama5d2x_pit.h>



/*
 * Initializes the components needed by s-boot
 */
static void c_boot_init(void)
{
    /* Custom hardware init spesific to the board */
    hardware_init();

    /* Initilaize hardware used by c-boot */
    led_init();
    print_init();
    frame_init(1000);
    print("Starting bootloader\n");
    asm volatile("cpsie ifa");
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

    struct frame* frame;
    u32 status;
    u32 addr = 0x20000000;
    while (1) {
        frame = get_frame();
        if (!frame) {
            continue;
        }

        status = 1;
        if (frame->cmd == CMD_WRITE_PAGE) {
             status = load_page(addr, frame->data, frame->size);
        } else if (frame->cmd == CMD_WRITE_PAGE_LAST) {
            status = load_page(addr, frame->data, frame->size);

            jump_to_image(0x20000000);
        }

        if (status) {
            addr += frame->size;
            send_response(RESP_OK);
        } else {
            send_response(RESP_ERROR);
        }
    }
    return 1;
}
