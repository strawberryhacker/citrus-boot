/// Copyright (C) strawberryhacker

#include <citrus-boot/disk.h>
#include <citrus-boot/print.h>
#include <citrus-boot/string.h>
#include <citrus-boot/mem.h>
#include <citrus-boot/panic.h>
#include <citrus-boot/fat.h>
#include <stdalign.h>

static u8 alignas(32) mbr_buffer[512];

/// Read the disk MBR (sector 0) and find the disk partitions and add them to 
/// the partitions. This will give all valid partitins a name
void disk_find_partitions(struct disk* disk)
{
    mem_set(disk->partitions, 0, sizeof(disk->partitions));

    // Read the MBR at sector zero
    u8* buf = mbr_buffer;
    
    if (!disk->read(disk, 0, 1, buf)) {
        panic("Cant read MBR");
    }

    // Check is this is a valid MBR
    if (buf[510] != 0x55 || buf[511] != 0xAA) {
        return;
    }


    // Partition table entry
    struct part_table_entry* pte = (struct part_table_entry *)(buf + 446);

    u8 part_index = 0;
    for (u32 i = 0; i < 4; i++, pte++) {
        // Check if the partition is active
        if (pte->status >= 0x01 && pte->status <= 0x7F) {
            // Invalid
            continue;
        }

        // Check if this is a valid partition
        if (mem_read_le32(&pte->sectors)) {

            struct partition* part = &disk->partitions[i];
            part->sect_count = mem_read_le32(&pte->sectors);
            part->start_lba = mem_read_le32(&pte->lba);

            part->parent_disk = disk;
            part->part_number = i;

            // Set the partition name
            part_index++;
        }
    }
}

struct file new_file;
struct file new_dir;
struct file_info new_info;

static u8 sd_done = 0;
static u8 buffer[4096];

static volatile u8* kernel = (volatile u8 *)0x20000000;

u8 load_kernel(struct partition* part, struct file* file)
{
    u32 rec = 0;
    do {
        u8 status = fat_file_read(part, file, buffer, 4096, &rec);
        if (status & FAT_ERROR_MASK) {
            return 0;
        }
        for (u32 i = 0; i < rec; i++) {
            *kernel++ = buffer[i];
        }
    } while (rec == 4096);
    sd_done = 1;
    return 1;
}

/// Adds a new disk to the system
void disk_add(struct disk* disk, enum disk_type type)
{
    // Find all the partitions on the MBR
    sd_done = 0;
    disk_find_partitions(disk);

    struct file* file = &new_file;
    struct file* dir = &new_dir;
    fat_file_init(file);
    fat_file_init(dir);
    struct file_info* info = &new_info;

    // Try to mount the partitions
    struct partition* part = &disk->partitions[0];
    for (u32 i = 0; i < 4; i++) {
        if (part->sect_count) {

            // Try to mount the partition
            if(!fat_mount_partition(part)) {
                print("Cannot mound partition\n");
                continue;
            }

            u8 status = fat_file_open(part, file, "/citrus-kernel.bin", 18);            
            if (status == FAT_OK) {
                if (load_kernel(part, file)) {
                    asm volatile ("cpsid ifa");
                    return;
                }
            }
        }
        part++;
    }
    print("Cannot find kernel\n");
    while (1);
}

u8 kernel_download_complete_sd(void)
{
    return (sd_done) ? 1 : 0;
}
