/* Copyright (C) strawberryhacker */

#ifndef FRAME_H
#define FRAME_H

#include <c-boot/types.h>

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

/* Defines the different commands that might occur in a frame from the host */
#define CMD_WRITE_PAGE       0x04
#define CMD_WRITE_PAGE_LAST  0x05

/*
 * This struct will contain useful information from the frame. The struct
 * has to be packed because the FCS is calulated from multiple fields so the 
 * compiler can not add padding
 */
struct frame {
    /* Do not change the order */
    u8 _dummy;
    u8  cmd;
    u16 size;
    u8  data[MAX_PAYLOAD_SIZE];
    u8  fcs;
};

void frame_init(u32 timeout_ms);

struct frame* get_frame(void);

void send_response(u8 error_code);

#endif
