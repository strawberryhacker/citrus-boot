/* Copyright (C) strawberryhacker */

#include <c-boot/frame.h>
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
volatile struct frame frame = {0};
volatile u32 frame_index = 0;
volatile u8 frame_received;

/* Handlers */
static void serial_callback(u8 data);
static void timeout_callback(void);

void frame_init(u32 timeout_ms)
{
    serial_init();
    serial_add_handler(serial_callback);
    timer_init(timeout_ms);
    timer_add_handler(timeout_callback);

    /* This is due to a bug */
    frame_received = 0;
    bus_state = BUS_STATE_IDLE;
}

/*
 * Returns the frame pointer if a new frame has been received and NULL if not
 */
struct frame* get_frame(void)
{
    if (frame_received) {
        return (struct frame *)&frame;
    }
    return NULL;
}

/*
 * Sends a response code back to the host and enable the next frame to be
 * processed
 */
void send_response(u8 error_code)
{
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
                u8 fcs = crc_calculate((u8 *)&frame.cmd, frame.size + 3, POLYNOMIAL);

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
