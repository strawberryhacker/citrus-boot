# Copyright (C) strawberryhacker

TOP         = $(shell pwd)
BUILDDIR    = $(TOP)/build

# Include the configuration file to know what files to build
ifneq ($(MAKECMDGOALS),clean)
ifeq ($(CONFIG),)

# Just print out all the available config files
space := $(subst ,, )
comma := ,
config_list = $(notdir $(wildcard $(TOP)/arch/configs/*))
$(info Usage: make CONFIG=[$(subst $(space),$(comma) ,$(config_list))])
$(error Missing configuration file)
else 
include $(TOP)/arch/configs/$(CONFIG)
endif

# A load address is required
ifeq ($(LOAD_ADDRESS),)
$(error LOAD_ADDRESS must be specified in the config)
endif
endif

# Compilers
CC      = arm-none-eabi-gcc
OBJDUMP = arm-none-eabi-objdump
OBJCOPY = arm-none-eabi-objcopy
ARM_ASM = arm-none-eabi-as
GDB     = arm-none-eabi-gdb

# Set the compiler flags
CFLAGS   += -x c -O3 -g3 -ffunction-sections -fdata-sections -mlong-calls -Wall
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
include $(TOP)/lib/Makefile
include $(TOP)/boot/Makefile

# Check that the linker script is provided
ifneq ($(MAKECMDGOALS),clean)
ifndef linker-script-y
$(error linker script is not provided)
endif
endif

# All object files are addes so we place them in the build directory
BUILDOBJ = $(addprefix $(BUILDDIR), $(obj-y))
CPFLAGS += $(include-flags-y)
CPFLAGS += -I. -DLOAD_ADDR=$(LOAD_ADDRESS)
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
	@echo "Starting debugger"

# Currently not supported
debug:
	@echo "Starting debugger"
	@$(GDB) -f $(BUILDDIR)/$(TARGET_NAME).elf -x tools/debug.gdb

clean:
	@rm -r -f $(BUILDDIR)
