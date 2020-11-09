/// Copyright (C) strawberryhacker

#ifndef HOST_H
#define HOST_H

#include <citrus-boot/types.h>

void host_init(u8* load_addr);

void host_deinit(void);

u8 kernel_download_complete_host(void);

#endif
