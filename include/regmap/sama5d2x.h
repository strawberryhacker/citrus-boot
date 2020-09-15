/*  Copyright (C) strawberryhacker */

/*
 * This file contains some of the registers and system definitions for the Atmel
 * SAMA5D27 microprocessor
 * 
 * To add more register definitions do the following. Add a struct with the 
 * mudule name followed by underscore + reg. Add the relevant register entries
 * in the right order (remember to add padding). Use the attributes _r, _w and
 * _rw according to the device datasheet. Define the address of the modules by
 * using the module name in capital letters. It is not necessary to cast to a
 * volatile structure
 */

#ifndef SAMA5D2X_H
#define SAMA5D2X_H

#include <c-boot/types.h>

/* Types used for registers */
#define  _r volatile const
#define  _w volatile 
#define _rw volatile

/* Watchdog registers */
struct wdt_reg {
    _w  u32 CR;
    _rw u32 MR;
    _r  u32 SR;
};

#define WDT ((struct wdt_reg *)0xF8048040)

/* Parallel input output controller registers */
struct gpio_reg {
    _rw u32 MSKR;
    _rw u32 CFGR;
    _r  u32 PDSR;
    _r  u32 LOCKSR;
    _w  u32 SODR;
    _w  u32 CODR;
    _rw u32 ODSR;
    _r  u32 RESERVED1;
    _w  u32 IER;
    _w  u32 IDR;
    _r  u32 IMR;
    _r  u32 ISR;
    _r  u32 RESERVED2[3];
    _w  u32 IOFR;
};

#define GPIOA ((struct gpio_reg *)0xFC038000)
#define GPIOB ((struct gpio_reg *)0xFC038040)
#define GPIOC ((struct gpio_reg *)0xFC038080)
#define GPIOD ((struct gpio_reg *)0xFC0380C0)

/* UART registers */
struct uart_reg {
    _w  u32 CR;
    _rw u32 MR;  
    _w  u32 IER;
    _w  u32 IDR;
    _r  u32 IMR;
    _r  u32 SR;
    _r  u32 RHR;
    _w  u32 THR;
    _rw u32 BRGR;
    _rw u32 CMPR;
    _rw u32 RTOR;
    _r  u32 RESERVED[46];
    _rw u32 WPMR;
};

#define UART0 ((struct uart_reg *)0xF801C000)
#define UART1 ((struct uart_reg *)0xF8020000)
#define UART2 ((struct uart_reg *)0xF8024000)
#define UART3 ((struct uart_reg *)0xFC008000)
#define UART4 ((struct uart_reg *)0xFC00C000)

/* Power management controller registers */
struct pmc_reg {
    _w  u32 SCER;
    _w  u32 SCDR;
    _r  u32 SCSR;
    _r  u32 RESERVED1;
    _w  u32 PCER0;
    _w  u32 PCDR0;
    _r  u32 PCSR0;
    _rw u32 UCKR;
    _rw u32 MOR;
    _rw u32 MCFR;
    _rw u32 PLLAR;
    _r  u32 RESERVED2;
    _rw u32 MCKR;
    _r  u32 RESERVED3;
    _rw u32 USB;
    _r  u32 RESERVED4;
    _rw u32 PCK0;
    _rw u32 PCK1;
    _rw u32 PCK2;
    _r  u32 RESERVED5[5];
    _w  u32 IER;
    _w  u32 IDR;
    _r  u32 SR;
    _r  u32 IMR;
    _rw u32 FSMR;
    _rw u32 FSPR;
    _rw u32 FOCR;
    _r  u32 RESERVED6;
    _rw u32 PLLICPR;
    _r  u32 RESERVED7[24];
    _rw u32 WPMR;
    _r  u32 WPSR;
    _r  u32 RESERVED8[5];
    _w  u32 PCER1;
    _w  u32 PCDR1;
    _r  u32 PCSR1;
    _rw u32 PCR;
    _rw u32 OCR;
    _w  u32 SLPWK_ER0;
    _w  u32 SLPWK_DR0;
    _r  u32 SLPWK_SR0;
    _r  u32 SLPWK_ASR0;
    _r  u32 RESERVED9[4];
    _w  u32 SLPWK_ER1;
    _w  u32 SLPWK_DR1;
    _r  u32 SLPWK_SR1;
    _r  u32 SLPWK_ASR1;
    _r  u32 SLPWK_AIPR;
    _rw u32 SLPWKCR;
    _rw u32 AUDIO_PLL0;
    _rw u32 AUDIO_PLL1;
};

#define PMC ((struct pmc_reg *)0xF0014000)

/* Peridic interval timer registers */
struct pit_reg {
    _rw u32 MR;
    _r  u32 SR;
    _r  u32 PIVR;
    _r  u32 PIIR;
};

#define PIT ((struct pit_reg *)0xF8048030)

/* Advanced interrupt controller registers */
struct apic_reg {
    _rw u32 SSR;
    _rw u32 SMR;
    _rw u32 SVR;
    _r  u32 RESERVED0;
    _rw u32 IVR;        /* WARNING: dont write unless you know what ur doing */
    _r  u32 FVR;
    _r  u32 ISR;
    _r  u32 RESERVED1;
    _r  u32 IPR0;
    _r  u32 IPR1;
    _r  u32 IPR2;
    _r  u32 IPR3;
    _r  u32 IMR;
    _r  u32 CISR;
    _w  u32 EOICR;
    _rw u32 SPU;
    _w  u32 IECR;
    _w  u32 IDCR;
    _w  u32 ICCR;
    _w  u32 ISCR;
    _r  u32 RESERVED2[7];
    _rw u32 DCR;
    _r  u32 RESERVED3[29];
    _rw u32 WPMR;
    _r  u32 WPSR;
};

#define APIC ((struct apic_reg *)0xFC020000)
#define SAPIC ((struct apic_reg *)0xF803C000)

/* SDMMC / MCI registers */
struct mci_reg {
    _rw u32 SSAR;
    _rw u16 BSR;
    _rw u16 BCR;
    _rw u32 ARG1R;
    _rw u16 TMR;
    _rw u16 CR;
    _r  u32 RR0;
    _r  u32 RR1;
    _r  u32 RR2;
    _r  u32 RR3;
    _rw u32 BDPR;
    _r  u32 PSR;
    _rw u8  HC1R;
    _rw u8  PCR;
    _rw u8  BGCR;
    _rw u8  WCR;
    _rw u16 CCR;
    _rw u8  TCR;
    _rw u8  SRR;
    _rw u16 NISTR;
    _rw u16 EISTR;
    _rw u16 NISTER;
    _rw u16 EISTER;
    _rw u16 NISIER;
    _rw u16 EISIER;
    _r  u16 ACESR;
    _rw u16 HC2R;
    _r  u32 CA0R;
    _rw u32 CA1R;
    _r  u32 MCCAR;
    _r  u16 RESERVED0;
    _w  u16 FERACES;
    _w  u32 FEREIS;
    _r  u8  AESR;
    _r  u8  RESERVED1[3];
    _rw u32 ASAR;
    _r  u32 RESERVED2;
    _rw u16 PVR[8];
    _r  u32 RESERVED3[35];
    _r  u16 SISR;
    _r  u16 HCVR;
    _r  u32 RESERVED4[64];
    _r  u32 APSR;
    _rw u8  MC1R;
    _w  u8  MC2R;
    _r  u16 RESERVED5;
    _rw u32 ACR;
    _rw u32 CC2R;
    _rw u8  RTC1R;
    _w  u8  RTC2R;
    _r  u16 RESERVED6;
    _rw u32 RTCVR;
    _rw u8  RTISTER;
    _rw u8  RTISIER;
    _r  u16 RESERVED7;
    _rw u8  RTISTR;
    _r  u8  RTSSR;
    _r  u16 RESERVED8;
    _rw u32 TUNCR;
    _r  u32 RESERVED9[3];
    _rw u32 CACR;
    _r  u32 RESERVED10[3];
    _rw u32 CALCR;
};

#define MCI0 ((struct mci_reg *)0xA0000000)
#define MCI1 ((struct mci_reg *)0xB0000000)

/* DDR registers */
struct ddr_reg {
    _rw u32 MR;
    _rw u32 RTR;
    _rw u32 CR;
    _rw u32 TPR0;
    _rw u32 TPR1;
    _rw u32 TPR2;
    _r  u32 RESERVED0;
    _rw u32 LPR;
    _rw u32 MD;
    _r  u32 RESERVED1;
    _rw u32 LPDDR2_LPDDR3_LPR;
    _rw u32 LPDDR2_LPDDR3_DDR3_MR4;
    _rw u32 LPDDR2_LPDDR3_DDR3_CAL;
    _rw u32 IO_CALIBR;
    _rw u32 OCMS;
    _w  u32 OCMS_KEY1;
    _w  u32 OCMS_KEY2;
    _rw u32 CONF_ARBITER;
    _rw u32 TIMEOUT;
    _rw u32 REQ_PORT_0123;
    _rw u32 REQ_PORT_4567;
    _r  u32 BDW_PORT_0123;
    _r  u32 BDW_PORT_4567;
    _rw u32 RD_DATA_PATH;
    _rw u32 MCFGR;
    _rw u32 MADDR[8];
    _r  u32 MINFO[8];
    _r  u32 RESERVED2[16];
    _rw u32 WPMR;
    _r  u32 WPSR;
};

#define DDR ((struct ddr_reg *)0xF000C000)

struct matrix_pri {
    _rw u32 A;
    _rw u32 B;
};

/* Matrix AHB-64 and AHB-32 register */
struct matrix_reg {
    _rw u32 MCFG[12];
    _r  u32 RESERVED0[4];
    _rw u32 SCFG[15];
    _r  u32 RESERVED1;
    struct matrix_pri PRI[15];
    _r  u32 RESERVED2[21];
    _w  u32 MEIER;
    _w  u32 MEIDR;
    _r  u32 MEIMR;
    _r  u32 MESR;
    _r  u32 MEAR[12];
    _r  u32 RESERVED3[21];
    _rw u32 WPMR;
    _r  u32 WPSR;
    _r  u32 RESERVED4[5];
    _rw u32 SSR[15];
    _r  u32 RESERVED5;
    _rw u32 SASSR[15];
    _r  u32 RESERVED6[2];
    _rw u32 SRTSR[15];
    _r  u32 RESERVED7;
    _rw u32 SPSELR1;
    _rw u32 SPSELR2;
    _rw u32 SPSELR3;
};

#define H32MX ((struct matrix_reg *)0xFC03C000)
#define H64MX ((struct matrix_reg *)0xF0018000)

/* Special function register */
struct sfr_reg {
    _r  u32 RESERVED0;
    _rw u32 DDRCFG;
    _r  u32 RESERVED1;
    _rw u32 OHCIICR;
    _r  u32 OHCIISR;
    _r  u32 RESERVED2[4];
    _rw u32 SECURE;
    _r  u32 RESERVED3;
    _rw u32 UTMICKTRIM;
    _rw u32 UTMIHSTRIM;
    _rw u32 UTMIFSTRIM;
    _rw u32 UTMISWAP;
    _r  u32 RESERVED4[2];
    _rw u32 CAN;
    _r  u32 SN0;
    _r  u32 SN1;
    _rw u32 AICREDIR;
    _rw u32 L2CC_HRAMC;
    _r  u32 RESERVED5[13];
    _rw u32 I2SCLKSEL;
    _rw u32 QSPICLK;
};

#define SFR ((struct sfr_reg *)0xF8030000)

#endif
