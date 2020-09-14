
# Connect to the J-Link server
target remote localhost:2331

mon reg cpsr = 0xd3

# Reset peripherals (using RSTC_CR)
set *0xF8048000 = 0xA5000004

# Reset L2 Cache controller
set *0x00A00100 = 0x0

# Disable Watchdog (using WDT_MR)
set *0xF8048044 = 0x00008000

# Disable D-Cache, I-Cache and MMU
mon cp15 1 0 0 0 = 0x00C50078

set *0xF0014004 = 0x4
set *0xF0014014 = (1 << 13)

mon reg r8_fiq = 0
mon reg r9_fiq = 0
mon reg r10_fiq = 0
mon reg r11_fiq = 0
mon reg r12_fiq = 0
mon reg sp_fiq = 0
mon reg lr_fiq = 0
mon reg spsr_fiq = 0
mon reg sp_irq = 0
mon reg lr_irq = 0
mon reg spsr_irq = 0
mon reg sp_abt = 0
mon reg lr_abt = 0
mon reg spsr_abt = 0
mon reg sp_und = 0
mon reg lr_und = 0
mon reg spsr_und = 0
mon reg sp_svc = 0
mon reg lr_svc = 0
mon reg spsr_svc = 0
mon reg r0 = 0
mon reg r1 = 0
mon reg r2 = 0
mon reg r3 = 0
mon reg r4 = 0
mon reg r5 = 0
mon reg r6 = 0
mon reg r7 = 0
mon reg r8_usr = 0
mon reg r9_usr = 0
mon reg r10_usr = 0
mon reg r11_usr = 0
mon reg r12_usr = 0
mon reg sp_usr = 0
mon reg lr_usr = 0

load
mon reg pc = 0x00200000
continue
