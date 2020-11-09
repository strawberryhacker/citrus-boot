/// Copyright (C) strawberryhacker 

#ifndef KPARAM_H
#define KPARAM_H

#include <citrus-boot/types.h>

#define KPARAM_MEM 0x00000001
#define KPARAM_END 0x00000000

// Gives information about one physical memory section 
struct kparam_mem {
    u32 type;
    u32 param_size;
    u32 size;
    u32 start;
};

struct kparam_end {
    u32 type;
    u32 param_size;
};

union kparam {
    struct kparam_mem mem;
    struct kparam_end end;
};

u32 get_kparam_paddr(void);

void relocate_kparam(u32 addr);

#endif
