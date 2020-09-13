# Copyright (C) strawberryhacker

TOP         = $(shell pwd)
BUILDDIR    = $(TOP)/build

# Modify this according to what the ROM bootloader (FSBL) expect to find on
# the MMC drive. In our case this will be called boot.bin and will be placed
# in the root directory
TARGET_NAME = boot

# Specifies what target to build for
SAMA5D2X = y

# Compilers
CC      = arm-none-eabi-gcc
OBJDUMP = arm-none-eabi-objdump
OBJCOPY = arm-none-eabi-objcopy
ARM_ASM = arm-none-eabi-as
GDB     = arm-none-eabi-gdb

# Set the compiler flags
CFLAGS   += -x c -O1 -g3 -ffunction-sections -fdata-sections -mlong-calls -Wall
CFLAGS   += -std=gnu99 -mcpu=cortex-a5 -c

# Supress warnings
CFLAGS   += -Wno-unused-function -Wno-unused-variable

LDFLAGS  += -Wl,--start-group -Wl,--end-group
LDFLAGS  += -Wl,--gc-sections -mcpu=cortex-a5

ASMFLAGS += -mcpu=cortex-a5 -g3

# Include all the project makefiles. This will add all object files and all 
# include paths to the OBJ and CPPFLAGS respectively
include-flags-y :=
linker-script-y :=
obj-y :=

include $(TOP)/entry/Makefile
include $(TOP)/include/Makefile
include $(TOP)/drivers/Makefile
include $(TOP)/arch/Makefile

# Check that everything is in order
ifndef linker-script-y
$(error linker script is not provided)
endif

# All object files are addes so we place them in the build directory
BUILDOBJ = $(addprefix $(BUILDDIR), $(obj-y))
CPFLAGS += $(include-flags-y)
LDFLAGS += -T$(linker-script-y)

.SECONDARY: $(BUILDOBJ)
.PHONY: all elf upload debug clean
all: elf lss bin

# Subtargets
elf: $(BUILDDIR)/$(TARGET_NAME).elf
lss: $(BUILDDIR)/$(TARGET_NAME).lss
bin: $(BUILDDIR)/$(TARGET_NAME).bin

$(BUILDDIR)/%.elf: $(BUILDOBJ)
	@echo
	@$(CC) $(LDFLAGS) -Wl,--print-memory-usage $^ -o $@
	@echo

$(BUILDDIR)/%.lss: $(BUILDDIR)/%.elf
	@$(OBJDUMP) -h -S $< > $@

$(BUILDDIR)/%.bin: $(BUILDDIR)/%.elf
	@$(OBJCOPY) -O binary $< $@

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo " >" $<
	@$(CC) $(CFLAGS) $(CPFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: %.s
	@mkdir -p $(dir $@)
	@echo " >" $<
	@$(ARM_ASM) $(ASMFLAGS) -c $< -o $@

upload: all
	$(GDB) -f $(BUILDDIR)/$(TARGET_NAME).elf -x tools/debug.gdb

# Currently not supported
debug:
	@echo "Starting debugger"

clean:
	@rm -r -f $(BUILDDIR)
