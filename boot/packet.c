/* Copyright (C) strawberryhacker */

#include <c-boot/packet.h>
#include <c-boot/serial.h>
#include <c-boot/timer.h>
#include <c-boot/crc.h>
#include <c-boot/print.h>
#include <stddef.h>

#define START_BYTE 0xAA
#define END_BYTE   0x55
#define POLYNOMIAL 0xB2

/*
 * Bus states used in the communication protocol, see link for more info
 * https://github.com/strawberryhacker/vanilla/blob/master/doc/bootloader.md
 */
enum bus_state {
    BUS_STATE_IDLE,
    BUS_STATE_CMD,
    BUS_STATE_SIZE,
    BUS_STATE_DATA,
    BUS_STATE_FCS,
    BUS_STATE_END
};

/* Frame interface variables */
volatile enum bus_state bus_state = BUS_STATE_IDLE;
volatile struct packet packet = {0};
volatile u32 data_index = 0;
volatile u8 packet_received;

/* Private handlers */
static void serial_callback(u8 data);
static void timeout_callback(void);

/* 1 second timeout */
#define TIMEOUT 1000

/*
 * Initializes the c-boot serial connection and the timer callbacks
 */
void packet_init(void)
{
    serial_init();
    serial_add_handler(serial_callback);
    timer_init(TIMEOUT);
    timer_add_handler(timeout_callback);

    /* This is due to a bug */
    packet_received = 0;
    bus_state = BUS_STATE_IDLE;
}

/*
 * Returns the frame pointer if a new frame has been received and NULL if not
 */
struct packet* get_packet(void)
{
    if (packet_received) {
        return (struct packet *)&packet;
    }
    return NULL;
}

/*
 * Sends a response code back to the host and enable the next frame to be
 * processed. This should be called when the host is done processing a packet
 */
void packet_respose(u8 error_code)
{
    packet_received = 0;
    serial_write(error_code);
}

/*
 * Processes packets from the host. This does not set any error flags. If the
 * frame has any errors the host will retry. The host can also try to reset the 
 * bootloader by issuing a reset command, and try sending the firmare again
 */
void serial_callback(u8 data)
{
    /*
     * The boot code should ack with a OK response when receving 0 in IDLE
     * state. This is used to instruct the kernel to enter the bootloader by
     * asserting the internal reset line
     */
    if (bus_state == BUS_STATE_IDLE) {
        if (data == 6) {
            packet_respose(RESP_OK);
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
            packet.cmd = data;
            bus_state = BUS_STATE_SIZE;
            data_index = 0;
            packet.size  = 0;
            break;
        }
        case BUS_STATE_SIZE : {
            packet.size |= (data << (8 * data_index++));
            if (data_index >= 2) {
                if (packet.size) {
                    bus_state = BUS_STATE_DATA;
                } else {
                    bus_state = BUS_STATE_FCS;
                }
                data_index = 0;
            }
            break;
        }
        case BUS_STATE_DATA : {
            if (packet.size) {
                packet.data[data_index++] = data;
            }
            if (data_index >= packet.size) {
                bus_state = BUS_STATE_FCS;
            }
            break;
        }
        case BUS_STATE_FCS : {
            packet.fcs = data;
            bus_state = BUS_STATE_END;
            break;
        }
        case BUS_STATE_END : {
            if (data == END_BYTE) {
                u8 fcs = crc_calculate((u8 *)&packet.cmd, packet.size + 3, POLYNOMIAL);
                if (fcs == packet.fcs) {
                    packet_received = 1;
                } else {
                    packet_respose(RESP_FCS_ERROR);
                }
            } else {
                packet_respose(RESP_FRAME_ERROR);
            }
            bus_state = BUS_STATE_IDLE;
            timer_stop();
            break;
        }
    }
}

/*
 * Called by /drivers/board/xxx/timer.c when a timeout has occured. The timeout
 * is specified from the timeout_init . In case of a timeout the frame state
 * machine logic is reset and the timer is stopped. The interface will then be
 * ready for the next frame
 */
void timeout_callback(void)
{
    timer_stop();
    bus_state = BUS_STATE_IDLE;
}
