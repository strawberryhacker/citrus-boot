/* Copyright (C) strawberryhacker */

#include <c-boot/types.h>
#include <c-boot/led.h>
#include <c-boot/hardware.h>
#include <c-boot/serial.h>
#include <c-boot/print.h>
#include <c-boot/bitops.h>
#include <c-boot/timer.h>
#include <c-boot/crc.h>

#include <drivers/irq/sama5d2x_apic.h>
#include <drivers/gpio/sama5d2x_gpio.h>
#include <drivers/clk/sama5d2x_clk.h>
#include <drivers/uart/sama5d2x_uart.h>
#include <drivers/timer/sama5d2x_pit.h>

#define MAX_PAYLOAD_SIZE 512

/* Response codes */
#define RESP_OK          (u8)(0 << 0)
#define RESP_ERROR       (u8)(1 << 0)
#define RESP_RETRANSMIT  (u8)(1 << 1)
#define RESP_HOST_EXIT   (u8)(1 << 2)
#define RESP_FLASH_ERROR (u8)(1 << 3)
#define RESP_FCS_ERROR   (u8)(1 << 4)
#define RESP_TIMEOUT     (u8)(1 << 5)
#define RESP_FRAME_ERROR (u8)(1 << 6)

#define START_BYTE 0xAA
#define END_BYTE   0x55
#define POLYNOMIAL 0xB2

enum bus_state {
    BUS_STATE_IDLE,
    BUS_STATE_CMD,
    BUS_STATE_SIZE,
    BUS_STATE_DATA,
    BUS_STATE_FCS,
    BUS_STATE_END
};

/*
 * This struct will contain useful information from the frame. The struct
 * has to be packed because the FCS is calulated from multiple fields so the 
 * compiler can not add padding
 */
struct frame {
    u8  cmd;
    u16 size;
    u8  data[MAX_PAYLOAD_SIZE];
    u8  fcs;
};

/* Frame interface variables */
volatile enum bus_state bus_state = BUS_STATE_IDLE;
volatile struct frame frame = {0};
volatile u32 frame_index = 0;
volatile u8 frame_received;

/*
 * Sends a response code back to the host and enable the next frame to be
 * processed
 */
void send_response(u8 error_code) {
    frame_received = 0;
    serial_write(error_code);
}

void serial_callback(u8 data)
{
    /* If the bus state is not IDLE the timeout interface will be reloaded */
    if (bus_state == BUS_STATE_IDLE) {
        if (data == 0) {
            print("c-boot\n");
            send_response(RESP_OK);
            bus_state = BUS_STATE_IDLE;
        }      
    } else {
        timer_restart();
    }

    switch(bus_state) {
        case BUS_STATE_IDLE : {
            if (data == START_BYTE) {
                bus_state = BUS_STATE_CMD;
                timer_restart();
            }
            break;
        }
        case BUS_STATE_CMD : {
            frame.cmd = data;
            bus_state = BUS_STATE_SIZE;
            frame_index = 0;
            frame.size  = 0;
            break;
        }
        case BUS_STATE_SIZE : {
            /* Receive 2 bytes in little endian */
            frame.size |= (data << (8 * frame_index++));

            if (frame_index >= 2) {
                bus_state = BUS_STATE_DATA;
                frame_index = 0;
            }
            break;
        }
        case BUS_STATE_DATA : {
            frame.data[frame_index++] = data;

            if (frame_index >= frame.size) {
                bus_state = BUS_STATE_FCS;
            }
            break;
        }
        case BUS_STATE_FCS : {
            frame.fcs = data;
            bus_state = BUS_STATE_END;
            break;
        }
        case BUS_STATE_END : {
            if (data == END_BYTE) {
                /* Check the FCS of the frame */
                u8 fcs = crc_calculate((u8 *)&frame.size, frame.size + 2, POLYNOMIAL);

                if (fcs == frame.fcs) {
                    frame_received = 1;
                } else {
                    print("FCS error => %#X\n", fcs);
                    send_response(RESP_ERROR | RESP_FCS_ERROR);
                }
            } else {
                print("Frame error\n");
                send_response(RESP_ERROR | RESP_FRAME_ERROR);
            }
            bus_state = BUS_STATE_IDLE;
            timer_stop();
            break;
        }
    }
}

void timeout_callback(void)
{
    print("Timeout\n");
    timer_stop();
    bus_state = BUS_STATE_IDLE;
}

/*
 * Initializes the components needed by s-boot
 */
static void c_boot_init(void)
{
    /* Custom hardware init spesific to the board */
    hardware_init();

    /* Initilaize hardware used by c-boot */
    led_init();
    serial_init();
    serial_add_handler(serial_callback);
    print_init();
    print("Starting bootloader\n");

    timer_init(1000);
    timer_add_handler(timeout_callback);

    frame_received = 0;
    bus_state = BUS_STATE_IDLE;
    asm volatile("cpsie ifa");
}

/*
 * Entry point after device spesific startup code
 */
int main(void)
{
    c_boot_init();

    while (1) {
        if (frame_received) {
            print("Downloading\n");
            frame_received = 0;
            send_response(RESP_OK);
        }
    }
    return 1;
}
