/* Copyright (C) strawberryhacker */

#ifndef PACKET_H
#define PACKET_H

#include <c-boot/types.h>

#define MAX_PAYLOAD_SIZE 512

/* Response codes */
#define RESP_OK 6
#define RESP_FRAME_ERROR 7
#define RESP_FCS_ERROR 8
#define RESP_BOOT_ERROR 9

/*
 * This struct will contain useful information from the frame. The struct
 * has to be packed because the FCS is calulated from multiple fields so the 
 * compiler can not add padding
 */
struct packet {
    /* Do not change the order */
    u8 _dummy;
    u8  cmd;
    u16 size;
    u8  data[MAX_PAYLOAD_SIZE];
    u8  fcs;
};

void packet_init(void);

struct packet* get_packet(void);

void packet_respose(u8 error_code);

#endif
