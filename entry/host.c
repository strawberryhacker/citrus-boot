/// Copyright (C) strawberryhacker 

#include <citrus-boot/uart.h>
#include <citrus-boot/gpio.h>
#include <citrus-boot/clk.h>
#include <citrus-boot/apic.h>
#include <citrus-boot/print.h>
#include <citrus-boot/dma.h>
#include <citrus-boot/cache.h>
#include <citrus-boot/mem.h>
#include <citrus-boot/crc.h>
#include <stddef.h>
#include <stdalign.h>

#define DMA_BUFFER_SIZE (4096 + 32 * 10)
#define CRC_POLY 0x45
#define PACKET_TAG 0xCA

#define CMD_DATA 0x00
#define CMD_SIZE 0x01
#define CMD_RESET 0x02

#define PACKET_ERROR 0x00 
#define PACKET_OK    0x01

static void uart4_interrupt(void);
static u8 process_packet(const u8* data, u32 size);
static u8 handle_packet(const u8* data, u32 size, u8 cmd);

/// Packet header
struct packet_header {
    u8 tag;
    u8 crc;
    u8 cmd;
    u8 header_size;
    u32 data_size;
};

/// Intermediate buffer for the DMA
static volatile u8 alignas(32) dma_buffer[DMA_BUFFER_SIZE];

/// Allocate a DMA channel
static struct dma_channel* channel;

/// Pre-allocate the DMA request
static struct dma_req req;

/// Sends a response back to the computer
static void send_response(u8 resp)
{
    while (!(FLEX4->U_SR & (1 << 1)));
    FLEX4->U_THR = resp;
}

void flex_print_init(void)
{
    // Pin setup 
    struct gpio tx = { .hw = GPIOC, .pin = 28 };
    struct gpio rx = { .hw = GPIOC, .pin = 29 };

    gpio_set_func(&tx, GPIO_FUNC_B);
    gpio_set_func(&rx, GPIO_FUNC_B);

    // Clock setup 
    clk_pck_enable(23);

    struct flexcom_reg* const hw = FLEX4;
    hw->U_CR = (1 << 3) | (1 << 2) | (1 << 8);
    hw->FLEX_MR = 1;
    hw->U_MR = (3 << 6) | (4 << 9) | (0b11 << 20);

    // Interrupt
    hw->U_BRGR = 5 | (5 << 16);

    hw->U_RTOR = 0;
    hw->U_TTGR = 0;

    hw->U_CR = (1 << 4) | (1 << 6);
}

void dma_receive_init(void)
{
    /// Configure the DMA channel
    req = (struct dma_req) {
        .burst          = DMA_BURST_16,
        .chunk          = DMA_CHUNK_1,
        .data           = DMA_DATA_U8,
        .dest_addr      = (void *)dma_buffer,
        .dest_am        = DMA_AM_INC,
        .dest_interface = 0,
        .src_addr       = (void *)&FLEX4->U_RHR,
        .src_am         = DMA_AM_FIXED,
        .src_interface  = 1,
        .memset_enable  = 0,
        .non_secure     = 0,
        .transfer_done  = NULL,
        .trigger        = DMA_TRIGGER_HW,
        .type           = DMA_TYPE_PER_MEM,
        .id             = 20,
        .ublock_cnt     = 1,
        .ublock_len     = DMA_BUFFER_SIZE
    };

    // Allocate a DMA channel. This will never be freed
    channel = alloc_dma_channel();
    if (channel == NULL) {
        print("Not enough channels");
    }

    dma_submit_request(&req, channel);
}

void dma_deinit(void)
{
    free_dma_channel(channel);
    dma_flush_channel(channel);
    dma_stop(channel);

    DMA0->GD = 0xFFFFFFFF;
    DMA1->GD = 0xFFFFFFFF;
}

static volatile u8* start_addr;
static volatile u8* curr_addr;
static volatile u8 host_done = 0;

static u8 handle_packet(const u8* data, u32 size, u8 cmd)
{
    if (cmd == CMD_SIZE) {
        u32 acc_size = mem_read_le32(data);
    } else if (cmd == CMD_DATA) {
        for (u32 i = 0; i < size; i++) {
            curr_addr[i] = data[i];
        }

        for (u32 i = 0; i < size; i++) {
            if (curr_addr[i] != data[i]) {
                print("Error\n");
                while(1);
            }
        }
        curr_addr += size;

        if (size != 4096) {
            // Last or zero-length packet
            host_done = 1;
            asm volatile ("cpsid if");
        }
    } else if (cmd == CMD_RESET) {
        curr_addr = start_addr;
        host_done = 0;
    } else {
        print("Command not supported\n");
    }

    return 1;
}

static u8 process_packet(const u8* data, u32 size)
{
    // The data should have a packet header
    struct packet_header* header = (struct packet_header *)data;

    // Check if the tag matches
    if (header->tag != PACKET_TAG) {
        return 0;
    }

    u32 header_data_size = mem_read_le32(&header->data_size);
    if (header_data_size + header->header_size != size) {
        print("%d + %d = %d\n", header_data_size, header->header_size, size);
        print("Packet size error\n");
        return 0;
    }

    // Verify the CRC
    u8 crc = crc_calculate(data + header->header_size, 
        header_data_size, CRC_POLY);
    
    if (crc != header->crc) {
        print("CRC mismatch\n");
        return 0;
    }
    
    return handle_packet(data + header->header_size,
        header_data_size, header->cmd);
    
    return 1;
}

void uart4_interrupt(void)
{
    if (FLEX4->U_SR & (1 << 8)) {
        FLEX4->U_CR = (1 << 11);

        dma_flush_channel(channel);
        dma_stop(channel);
        u32 size = DMA_BUFFER_SIZE - dma_get_microblock_size(channel);

        // Do somethong with the buffer
        u32 status = process_packet((u8 *)dma_buffer, size);
        dma_submit_request(&req, channel);

        // Send the status. This has to be done after the DMA is re-armed
        if (status) {
            send_response(PACKET_OK);
        } else {
            send_response(PACKET_ERROR);
        }
    }
}

/// This will establish a serial connection to the host computer
void host_init(u8* load_addr)
{
    start_addr = load_addr;
    curr_addr = load_addr;
    host_done = 0;

    // Flexcom
    flex_print_init();
    FLEX4->U_RTOR = 80;
    FLEX4->U_CR = (1 << 11);
    FLEX4->U_IER = (1 << 8);
    // Enable the APIC support 
    apic_irq_init(23, APIC_PRI_3, 0, uart4_interrupt);
    apic_enable(23);

    dma_init();

    // Init the DMA interface
    dma_receive_init();

    asm volatile ("cpsie ifa");

    // This will indicate that the the board is ready for the kernel
    send_response(PACKET_OK);
}

void host_deinit(void)
{
    dma_deinit();
}

u8 kernel_download_complete_host(void)
{
    return (host_done) ? 1 : 0;
}
