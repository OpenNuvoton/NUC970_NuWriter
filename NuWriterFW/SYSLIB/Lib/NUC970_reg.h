/******************************************************************************
 *                                                                          
 * Copyright (c) 2008 Nuvoton Technology. All rights reserved.             
 *                                                                         
 * FILENAME
 *     NUC900_reg.h
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *     This file contains the general register map of Nuvoton NUC900 processor. 
 *     Some registers are not defined for specific processor.
 *     
 *
 * HISTORY
 *     2008-06-19	Ver 1.0 Created by Min-Nan Cheng
 *
 * REMARK
 *     None
 *     
 ******************************************************************************/
#ifndef _NUC970_H
#define _NUC970_H

/*
   Register map
*/
#define    GCR_BA    0xB0000000 /* Global Control */
#define    CLK_BA    0xB0000200 /* Clock Control */
#define    EBI_BA    0xB0001000 /* EBI Control */
#define    SDIC_BA   0xB0001800 /* SDRAM (SDR/DDR/DDR2) Control */
#define    EMC0_BA   0xB0002000 /* Ethernet MAC 0 Control */
#define    EMC1_BA   0xB0003000 /* Ethernet MAC 1 Control */
#define    GDMA_BA   0xB0004000 /* GDMA control */
#define    USBH_BA   0xB0005000 /* USB Host EHCI Control */
#define    USBD_BA   0xB0006000 /* USB Device Control */
#define    USBO_BA   0xB0007000 /* OHCI USH OHCI Control */
#define    LCM_BA    0xB0008000 /* Display, LCM Interface */
#define    ACTL_BA   0xB0009000 /* Audio Control */
#define    JPEG_BA   0xB000A000 /* JPEG Engine Control */
#define    GE_BA     0xB000B000 /* 2-D Graphic Engine */
#define    SDH_BA    0xB000C000 /* SD/SDIO Host Controller */
#define    FMI_BA    0xB000D000 /* Flash Memory Card Interface */
#define    CAP_BA    0xB000E000 /* Sensor (Capture) Interface Control */
#define    CRPT_BA   0xB000F000 /* Crypto Engine Control */
#define    UART0_BA  0xB8000000 /* UART0 Control (High-Speed UART) */
#define    UART1_BA  0xB8000100 /* UART1 Control (High-Speed UART) */
#define    UART2_BA  0xB8000200 /* UART2 Control (console) */
#define    UART3_BA  0xB8000300 /* UART3 Control (micro-printer) */
#define    UART4_BA  0xB8000400 /* UART4 Control (High-Speed UART) */
#define    UART5_BA  0xB8000500 /* UART5 Control (console) */
#define    UART6_BA  0xB8000600 /* UART6 Control (High-Speed UART) */
#define    UART7_BA  0xB8000700 /* UART7 Control (console) */
#define    UART8_BA  0xB8000800 /* UART8 Control (High-Speed UART) */
#define    TMR_BA    0xB8001000 /* Timer */
#define    WDT_BA    0xB8001800 /* Watch Dog Timer */
#define    WWDT_BA   0xB8001900 /* Windows Watch Dog Timer */
#define    AIC_BA    0xB8002000 /* Interrupt Controller */
#define    GPIO_BA   0xB8003000 /* GPIO Control */
#define    RTC_BA    0xB8004000 /* Real Time Clock Control */
#define    SC0_BA    0xB8005000 /* Smart Card 0 Control */
#define    SC1_BA    0xB8005400 /* Smart Card 1 Control */
#define    I2C0_BA   0xB8006000 /* I2C 0 Control */
#define    I2C1_BA   0xB8006100 /* I2C 1 Control */
#define    USI_BA   0xB8006200 /* Universal Serial Interface 0 */
#define    USI0_BA   0xB8006200 /* Universal Serial Interface 0 */
#define    USI1_BA   0xB8006300 /* Universal Serial Interface 1 */
#define    PWM_BA    0xB8007000 /* Pulse Width Modulation (PWM) Control */
#define    KPI_BA    0xB8008000 /* Keypad Interface Control */
#define    PS2_BA    0xB8009000 /* PS2 Interface Control */
#define    ADC_BA    0xB800A000 /* ADC Control */
#define    CAN0_BA   0xB800B000 /* CAN 0 Control */
#define    CAN1_BA   0xB800B400 /* CAN 1 Control */
#define    OTP_BA    0xB800C000 /* OTP Control */
#define    IROM_BA   0xFFFF0000 /* Internal Boot ROM Space */
 
/*                                                             
  Global Control Registers  
*/
#define    REG_PDID     	(GCR_BA+0x000)  /* Product Identifier Register */
#define    REG_PWRON		(GCR_BA+0x004)  /* Power-On Setting Register */
#define    REG_ARBCON		(GCR_BA+0x008)  /* Arbitration Control Register */
#define    REG_LVRDCR 		(GCR_BA+0x020)  /* Low Voltage Reset & Detect Control Register */
#define    REG_MISCFCR  	(GCR_BA+0x030)  /* Miscellaneous Function Control Register */
#define    REG_MISCIER 		(GCR_BA+0x040)  /* Miscellaneous Interrupt Enable Register */
#define    REG_MISCISR 		(GCR_BA+0x044)  /* Miscellaneous Interrupt Status Register */
#define    REG_ROMSUM0 		(GCR_BA+0x048)  /* Internal ROM BIST Checksum Register 0 */
#define    REG_ROMSUM1 		(GCR_BA+0x04C)  /* Internal ROM BIST Checksum Register 1 */
#define    REG_WKUPSER		(GCR_BA+0x058)  /* System Wakeup Source Enable Register */
#define    REG_WKUPSSR		(GCR_BA+0x05C)  /* System Wakeup Source Status Register */
#define    REG_AHBIPRST		(GCR_BA+0x060)  /* AHB IP Reset Control Register */
#define    REG_APBIPRST0	(GCR_BA+0x064)  /* APB IP Reset Control Register 0 */
#define    REG_APBIPRST1	(GCR_BA+0x068)  /* APB IP Reset Control Register 1 */
#define    REG_RSTSTS		(GCR_BA+0x06C)  /* Reset Source Active Status Register */
#define    REG_MFP_GPA_L	(GCR_BA+0x070)  /* GPIOA Low Byte Multiple Function Control Register */
#define    REG_MFP_GPA_H	(GCR_BA+0x074)  /* GPIOA High Byte Multiple Function Control Register */
#define    REG_MFP_GPB_L	(GCR_BA+0x078)  /* GPIOB Low Byte Multiple Function Control Register */
#define    REG_MFP_GPB_H	(GCR_BA+0x07C)  /* GPIOB High Byte Multiple Function Control Register */
#define    REG_MFP_GPC_L	(GCR_BA+0x080)  /* GPIOC Low Byte Multiple Function Control Register */
#define    REG_MFP_GPC_H	(GCR_BA+0x084)  /* GPIOC High Byte Multiple Function Control Register */
#define    REG_MFP_GPD_L	(GCR_BA+0x088)  /* GPIOD Low Byte Multiple Function Control Register */
#define    REG_MFP_GPD_H	(GCR_BA+0x08C)  /* GPIOD High Byte Multiple Function Control Register */
#define    REG_MFP_GPE_L	(GCR_BA+0x090)  /* GPIOE Low Byte Multiple Function Control Register */
#define    REG_MFP_GPE_H	(GCR_BA+0x094)  /* GPIOE High Byte Multiple Function Control Register */
#define    REG_MFP_GPF_L	(GCR_BA+0x098)  /* GPIOF Low Byte Multiple Function Control Register */
#define    REG_MFP_GPF_H	(GCR_BA+0x09C)  /* GPIOF High Byte Multiple Function Control Register */
#define    REG_MFP_GPG_L	(GCR_BA+0x0A0)  /* GPIOG Low Byte Multiple Function Control Register */
#define    REG_MFP_GPG_H	(GCR_BA+0x0A4)  /* GPIOG High Byte Multiple Function Control Register */
#define    REG_MFP_GPH_L	(GCR_BA+0x0A8)  /* GPIOH Low Byte Multiple Function Control Register */
#define    REG_MFP_GPH_H	(GCR_BA+0x0AC)  /* GPIOH High Byte Multiple Function Control Register */
#define    REG_MFP_GPI_L	(GCR_BA+0x0B0)  /* GPIOI Low Byte Multiple Function Control Register */
#define    REG_MFP_GPI_H	(GCR_BA+0x0B4)  /* GPIOI High Byte Multiple Function Control Register */
#define    REG_MFP_GPJ_L	(GCR_BA+0x0B8)  /* GPIOJ Low Byte Multiple Function Control Register */
#define    REG_DDR_DS_CR	(GCR_BA+0x0E0)  /* DDR I/O Driving Strength Control Register */
#define    REG_PORDISCR	    (GCR_BA+0x100)  /* Power-On-Reset Disable Control Register */
#define    REG_ICEDBGCR	    (GCR_BA+0x104)  /* ICE Debug Interface Control Register */
#define    REG_ERRADDCR	    (GCR_BA+0x108)  /* Error Response Address Control Regsiter */
#define    REG_WRPRTR	    (GCR_BA+0x1FC)  /* Register Write-Protection Control Register */

#define    rREGWRPRTR			(*(unsigned int volatile *)REG_WRPRTR)

/*                                                            
  Clock Control Registers  
*/
#define    REG_PMCON			(CLK_BA+0x000)  /*  Power Management Control Register */
#define    REG_HCLKEN			(CLK_BA+0x010)  /*  AHB IP Clock Enable Control Register */
#define    REG_PCLKEN0		(CLK_BA+0x018)  /*  APB IP Clock Enable Control Register 0 */
#define    REG_PCLKEN1		(CLK_BA+0x01C)  /*  APB IP Clock Enable Control Register 1 */
#define    REG_CLKDIV0		(CLK_BA+0x020)  /*  Clock Divider Control Register 0 */
#define    REG_CLKDIV1		(CLK_BA+0x024)  /*  Clock Divider Control Register 1 */
#define    REG_CLKDIV2		(CLK_BA+0x028)  /*  Clock Divider Control Register 2 */
#define    REG_CLKDIV3		(CLK_BA+0x02C)  /*  Clock Divider Control Register 3 */
#define    REG_CLKDIV4		(CLK_BA+0x030)  /*  Clock Divider Control Register 4 */
#define    REG_CLKDIV5		(CLK_BA+0x034)  /*  Clock Divider Control Register 5 */
#define    REG_CLKDIV6		(CLK_BA+0x038)  /*  Clock Divider Control Register 6 */
#define    REG_CLKDIV7		(CLK_BA+0x03C)  /*  Clock Divider Control Register 7 */
#define    REG_CLKDIV8		(CLK_BA+0x040)  /*  Clock Divider Control Register 8 */
#define    REG_CLKDIV9		(CLK_BA+0x044)  /*  Clock Divider Control Register 9 */
#define    REG_APLLCON		(CLK_BA+0x060)  /*  APLL Control Register */
#define    REG_UPLLCON		(CLK_BA+0x064)  /*  UPLL Control Register */
#define    REG_CLKTREG		(CLK_BA+0x080)  /*  Test Clock Control Register */

#define    rCLKEN			(*(unsigned int volatile *)REG_CLKEN)
#define    rHCLKEN			(*(unsigned int volatile *)REG_HCLKEN)
#define    rPCLKEN0			(*(unsigned int volatile *)REG_PCLKEN0)
#define    rPCLKEN1			(*(unsigned int volatile *)REG_PCLKEN1)
#define    rCLKDIV			(*(unsigned int volatile *)REG_CLKDIV)
#define    rPLLCON0			(*(unsigned int volatile *)REG_PLLCON0)
#define    rPLLCON1			(*(unsigned int volatile *)REG_PLLCON1)
#define    rPMCON			(*(unsigned int volatile *)REG_PMCON)
#define    rIRQWAKECON		(*(unsigned int volatile *)REG_IRQWAKECON)
#define    rIRQWAKEFLAG		(*(unsigned int volatile *)REG_IRQWAKEFLAG)
#define    rIPSRST			(*(unsigned int volatile *)REG_IPSRST)
#define    rCLKEN1			(*(unsigned int volatile *)REG_CLKEN1)
#define    rCLKDIV1			(*(unsigned int volatile *)REG_CLKDIV1)
#define    rCLKDIV8			(*(unsigned int volatile *)REG_CLKDIV8)

/*
  EBI Control Registers  
*/
#define    REG_EBICON		(EBI_BA+0x000)  /* EBI control register */
#define    REG_ROMCON		(EBI_BA+0x004)  /* ROM/FLASH control register */
#define    REG_SDCONF0		(EBI_BA+0x008)  /* SDRAM bank 0 configuration register */
#define    REG_SDCONF1		(EBI_BA+0x00C)  /* SDRAM bank 1 configuration register */
#define    REG_SDTIME0		(EBI_BA+0x010)  /* SDRAM bank 0 timing control register */
#define    REG_SDTIME1		(EBI_BA+0x014)  /* SDRAM bank 1 timing control register */
#define    REG_EXT0CON		(EBI_BA+0x018)  /* External I/O 0 control register */
#define    REG_EXT1CON		(EBI_BA+0x01C)  /* External I/O 1 control register */
#define    REG_EXT2CON		(EBI_BA+0x020)  /* External I/O 2 control register */
#define    REG_EXT3CON		(EBI_BA+0x024)  /* External I/O 3 control register */
#define    REG_EXT4CON		(EBI_BA+0x028)  /* External I/O 4 control register */
#define    REG_CKSKEW		(EBI_BA+0x02C)  /* Clock skew control register */

#define    rEBICON			(*(unsigned int volatile *)REG_EBICON)
#define    rROMCON			(*(unsigned int volatile *)REG_ROMCON)
#define    rSDCONF0			(*(unsigned int volatile *)REG_SDCONF0)
#define    rSDCONF1			(*(unsigned int volatile *)REG_SDCONF1)
#define    rSDTIME0			(*(unsigned int volatile *)REG_SDTIME0)
#define    rSDTIME1			(*(unsigned int volatile *)REG_SDTIME1)
#define    rEXT0CON			(*(unsigned int volatile *)REG_EXT1CON)
#define    rEXT2CON			(*(unsigned int volatile *)REG_EXT2CON)
#define    rEXT3CON			(*(unsigned int volatile *)REG_EXT3CON)
#define    rEXT4CON			(*(unsigned int volatile *)REG_EXT4CON)
#define    rCKSKEW			(*(unsigned int volatile *)REG_CKSKEW)


/*
  EMAC Registers
*/
#define     REG_EMAC0_CAMCMR		(EMC0_BA+0x000)	/* CAM Command Register */
#define     REG_EMAC0_CAMEN			(EMC0_BA+0x004)	/* CAM Enable Register */
#define     REG_EMAC0_CAM0M_Base	(EMC0_BA+0x008)  /* CAM0 Most Significant Word Register */
#define     REG_EMAC0_CAM0L_Base	(EMC0_BA+0x00c)  /* CAM0 Least Significant Word Register */
#define     REG_EMAC0_CAMxM_Reg(x)	(REG_EMAC0_CAM0M_Base+x*0x8)	/* CAMx Most Significant Word Register */
#define     REG_EMAC0_CAMxL_Reg(x)	(REG_EMAC0_CAM0L_Base+x*0x8)	/* CAMx Least Significant Word Register */
#define     REG_EMAC0_TXDLSA		(EMC0_BA+0x088)	/* Transmit Descriptor Link List Start Address Register */
#define     REG_EMAC0_RXDLSA		(EMC0_BA+0x08C)	/* Receive Descriptor Link List Start Address Register */
#define     REG_EMAC0_MCMDR			(EMC0_BA+0x090)	/* MAC Command Register */
#define     REG_EMAC0_MIID			(EMC0_BA+0x094)	/* MII Management Data Register */
#define     REG_EMAC0_MIIDA			(EMC0_BA+0x098)	/* MII Management Control and Address Register */
#define     REG_EMAC0_FFTCR			(EMC0_BA+0x09C)	/* FIFO Threshold Control Register */
#define     REG_EMAC0_TSDR			(EMC0_BA+0x0a0)	/* Transmit Start Demand Register */
#define     REG_EMAC0_RSDR			(EMC0_BA+0x0a4)	/* Receive Start Demand Register */
#define     REG_EMAC0_DMARFC		(EMC0_BA+0x0a8)	/* Maximum Receive Frame Control Register */
#define     REG_EMAC0_MIEN			(EMC0_BA+0x0ac)	/* MAC Interrupt Enable Register */
#define     REG_EMAC0_MISTA			(EMC0_BA+0x0b0)	/* MAC Interrupt Status Register */
#define     REG_EMAC0_MGSTA			(EMC0_BA+0x0b4)	/* MAC General Status Register */
#define     REG_EMAC0_MPCNT			(EMC0_BA+0x0b8)	/* Missed Packet Count Register */
#define     REG_EMAC0_MRPC			(EMC0_BA+0x0bc)	/* MAC Receive Pause Count Register */
#define     REG_EMAC0_DMARFS		(EMC0_BA+0x0c8)	/* DMA Receive Frame Status Register */
#define     REG_EMAC0_CTXDSA		(EMC0_BA+0x0cc)	/* Current Transmit Descriptor Start Address Register */
#define     REG_EMAC0_CTXBSA		(EMC0_BA+0x0d0)	/* Current Transmit Buffer Start Address Register */
#define     REG_EMAC0_CRXDSA		(EMC0_BA+0x0d4)	/* Current Receive Descriptor Start Address Register */
#define     REG_EMAC0_CRXBSA		(EMC0_BA+0x0d8)	/* Current Receive Buffer Start Address Register */
#define     REG_EMAC0_TSCR			(EMC0_BA+0x100)	/* Time Stamp Control Register */
#define     REG_EMAC0_TSMSR			(EMC0_BA+0x110)	/* Time Stamp Most Significant Register  */
#define     REG_EMAC0_TSMLR			(EMC0_BA+0x114) /* Time Stamp Least Significant Register */
#define     REG_EMAC0_TSIR			(EMC0_BA+0x118)	/* Time Stamp Increment Register */
#define     REG_EMAC0_TSAR			(EMC0_BA+0x11c)	/* Time Stamp Addend Register */
#define     REG_EMAC0_TSMSUR		(EMC0_BA+0x120)	/* Time Stamp Most Significant Update Register */
#define     REG_EMAC0_TSLSUR		(EMC0_BA+0x124) /* Time Stamp Least Significant Update Register */
#define     REG_EMAC0_TSMSAR		(EMC0_BA+0x128) /* Time Stamp Most Significant Alarm Register */
#define     REG_EMAC0_TSLSAR		(EMC0_BA+0x12c)	/* Time Stamp Least Significant Alarm Register */
#define     REG_EMAC0_BIST			(EMC0_BA+0x300)	/* BIST Mode Register */
	
#define     rEMAC0_CAMCMR			(*(unsigned int volatile *)REG_EMAC0_CAMCMR)
#define     rEMAC0_CAMEN			(*(unsigned int volatile *)REG_EMAC0_CAMEN)
#define     rEMAC0_CAM0M_Base		(*(unsigned int volatile *)REG_EMAC0_CAM0M_Base)
#define     rEMAC0_CAM0L_Base		(*(unsigned int volatile *)REG_EMAC0_CAM0L_Base)
#define     rEMAC0_CAMxM_Reg(x)		(*(unsigned int volatile *)(REG_EMAC0_CAM0M_Base+x*0x8))	
#define     rEMAC0_CAMxL_Reg(x)		(*(unsigned int volatile *)(REG_EMAC0_CAM0L_Base+x*0x8))	
#define     rEMAC0_TXDLSA			(*(unsigned int volatile *)REG_EMAC0_TXDLSA)
#define     rEMAC0_RXDLSA			(*(unsigned int volatile *)REG_EMAC0_RXDLSA)
#define     rEMAC0_MCMDR			(*(unsigned int volatile *)REG_EMAC0_MCMDR)
#define     rEMAC0_MIID				(*(unsigned int volatile *)REG_EMAC0_MIID)
#define     rEMAC0_MIIDA			(*(unsigned int volatile *)REG_EMAC0_MIIDA)
#define     rEMAC0_FFTCR			(*(unsigned int volatile *)REG_EMAC0_FFTCR)
#define     rEMAC0_TSDR				(*(unsigned int volatile *)REG_EMAC0_TSDR)
#define     rEMAC0_RSDR				(*(unsigned int volatile *)REG_EMAC0_RSDR)
#define     rEMAC0_DMARFC			(*(unsigned int volatile *)REG_EMAC0_DMARFC)
#define     rEMAC0_MIEN				(*(unsigned int volatile *)REG_EMAC0_MIEN)
#define     rEMAC0_MISTA			(*(unsigned int volatile *)REG_EMAC0_MISTA)
#define     rEMAC0_MGSTA			(*(unsigned int volatile *)REG_EMAC0_MGSTA)
#define     rEMAC0_MPCNT			(*(unsigned int volatile *)REG_EMAC0_MPCNT)
#define     rEMAC0_MRPC				(*(unsigned int volatile *)REG_EMAC0_MRPC)
#define     rEMAC0_DMARFS			(*(unsigned int volatile *)REG_EMAC0_DMARFS)
#define     rEMAC0_CTXDSA			(*(unsigned int volatile *)REG_EMAC0_CTXDSA)
#define     rEMAC0_CTXBSA			(*(unsigned int volatile *)REG_EMAC0_CTXBSA)
#define     rEMAC0_CRXDSA			(*(unsigned int volatile *)REG_EMAC0_CRXDSA)
#define     rEMAC0_CRXBSA			(*(unsigned int volatile *)REG_EMAC0_CRXBSA)
#define 	rEMAC0_TSCR				(*(unsigned int volatile *)REG_EMAC0_TSCR)
#define 	rEMAC0_TSMSR			(*(unsigned int volatile *)REG_EMAC0_TSMSR)
#define 	rEMAC0_TSLSR			(*(unsigned int volatile *)REG_EMAC0_TSLSR)
#define 	rEMAC0_TSIR				(*(unsigned int volatile *)REG_EMAC0_TSIR)
#define 	rEMAC0_TSAR				(*(unsigned int volatile *)REG_EMAC0_TSAR)
#define 	rEMAC0_TSMSUR			(*(unsigned int volatile *)REG_EMAC0_TSMSUR)
#define 	rEMAC0_TSLSUR			(*(unsigned int volatile *)REG_EMAC0_TSLSUR)
#define 	rEMAC0_TSMSAR			(*(unsigned int volatile *)REG_EMAC0_TSMSAR)
#define 	rEMAC0_TSLSAR			(*(unsigned int volatile *)REG_EMAC0_TSLSAR)
#define 	rEMAC0_BIST				(*(unsigned int volatile *)REG_EMAC0_BIST)

#define     REG_EMAC1_CAMCMR		(EMC1_BA+0x000)	/* CAM Command Register */
#define     REG_EMAC1_CAMEN			(EMC1_BA+0x004)	/* CAM Enable Register */
#define     REG_EMAC1_CAM0M_Base	(EMC1_BA+0x008)  /* CAM0 Most Significant Word Register */
#define     REG_EMAC1_CAM0L_Base	(EMC1_BA+0x00c)  /* CAM0 Least Significant Word Register */
#define     REG_EMAC1_CAMxM_Reg(x)	(REG_EMAC1_CAM0M_Base+x*0x8)	/* CAMx Most Significant Word Register */
#define     REG_EMAC1_CAMxL_Reg(x)	(REG_EMAC1_CAM0L_Base+x*0x8)	/* CAMx Least Significant Word Register */
#define     REG_EMAC1_TXDLSA		(EMC1_BA+0x088)	/* Transmit Descriptor Link List Start Address Register */
#define     REG_EMAC1_RXDLSA		(EMC1_BA+0x08C)	/* Receive Descriptor Link List Start Address Register */
#define     REG_EMAC1_MCMDR			(EMC1_BA+0x090)	/* MAC Command Register */
#define     REG_EMAC1_MIID			(EMC1_BA+0x094)	/* MII Management Data Register */
#define     REG_EMAC1_MIIDA			(EMC1_BA+0x098)	/* MII Management Control and Address Register */
#define     REG_EMAC1_FFTCR			(EMC1_BA+0x09C)	/* FIFO Threshold Control Register */
#define     REG_EMAC1_TSDR			(EMC1_BA+0x0a0)	/* Transmit Start Demand Register */
#define     REG_EMAC1_RSDR			(EMC1_BA+0x0a4)	/* Receive Start Demand Register */
#define     REG_EMAC1_DMARFC		(EMC1_BA+0x0a8)	/* Maximum Receive Frame Control Register */
#define     REG_EMAC1_MIEN			(EMC1_BA+0x0ac)	/* MAC Interrupt Enable Register */
#define     REG_EMAC1_MISTA			(EMC1_BA+0x0b0)	/* MAC Interrupt Status Register */
#define     REG_EMAC1_MGSTA			(EMC1_BA+0x0b4)	/* MAC General Status Register */
#define     REG_EMAC1_MPCNT			(EMC1_BA+0x0b8)	/* Missed Packet Count Register */
#define     REG_EMAC1_MRPC			(EMC1_BA+0x0bc)	/* MAC Receive Pause Count Register */
#define     REG_EMAC1_DMARFS		(EMC1_BA+0x0c8)	/* DMA Receive Frame Status Register */
#define     REG_EMAC1_CTXDSA		(EMC1_BA+0x0cc)	/* Current Transmit Descriptor Start Address Register */
#define     REG_EMAC1_CTXBSA		(EMC1_BA+0x0d0)	/* Current Transmit Buffer Start Address Register */
#define     REG_EMAC1_CRXDSA		(EMC1_BA+0x0d4)	/* Current Receive Descriptor Start Address Register */
#define     REG_EMAC1_CRXBSA		(EMC1_BA+0x0d8)	/* Current Receive Buffer Start Address Register */
#define     REG_EMAC1_TSCR			(EMC1_BA+0x100)	/* Time Stamp Control Register */
#define     REG_EMAC1_TSMSR			(EMC1_BA+0x110)	/* Time Stamp Most Significant Register  */
#define     REG_EMAC1_TSMLR			(EMC1_BA+0x114) /* Time Stamp Least Significant Register */
#define     REG_EMAC1_TSIR			(EMC1_BA+0x118)	/* Time Stamp Increment Register */
#define     REG_EMAC1_TSAR			(EMC1_BA+0x11c)	/* Time Stamp Addend Register */
#define     REG_EMAC1_TSMSUR		(EMC1_BA+0x120)	/* Time Stamp Most Significant Update Register */
#define     REG_EMAC1_TSLSUR		(EMC1_BA+0x124) /* Time Stamp Least Significant Update Register */
#define     REG_EMAC1_TSMSAR		(EMC1_BA+0x128) /* Time Stamp Most Significant Alarm Register */
#define     REG_EMAC1_TSLSAR		(EMC1_BA+0x12c)	/* Time Stamp Least Significant Alarm Register */
#define     REG_EMAC1_BIST			(EMC1_BA+0x300)	/* BIST Mode Register */
	
#define     rEMAC1_CAMCMR			(*(unsigned int volatile *)REG_EMAC1_CAMCMR)
#define     rEMAC1_CAMEN			(*(unsigned int volatile *)REG_EMAC1_CAMEN)
#define     rEMAC1_CAM0M_Base		(*(unsigned int volatile *)REG_EMAC1_CAM0M_Base)
#define     rEMAC1_CAM0L_Base		(*(unsigned int volatile *)REG_EMAC1_CAM0L_Base)
#define     rEMAC1_CAMxM_Reg(x)		(*(unsigned int volatile *)(REG_EMAC1_CAM0M_Base+x*0x8))	
#define     rEMAC1_CAMxL_Reg(x)		(*(unsigned int volatile *)(REG_EMAC1_CAM0L_Base+x*0x8))	
#define     rEMAC1_TXDLSA			(*(unsigned int volatile *)REG_EMAC1_TXDLSA)
#define     rEMAC1_RXDLSA			(*(unsigned int volatile *)REG_EMAC1_RXDLSA)
#define     rEMAC1_MCMDR			(*(unsigned int volatile *)REG_EMAC1_MCMDR)
#define     rEMAC1_MIID				(*(unsigned int volatile *)REG_EMAC1_MIID)
#define     rEMAC1_MIIDA			(*(unsigned int volatile *)REG_EMAC1_MIIDA)
#define     rEMAC1_FFTCR			(*(unsigned int volatile *)REG_EMAC1_FFTCR)
#define     rEMAC1_TSDR				(*(unsigned int volatile *)REG_EMAC1_TSDR)
#define     rEMAC1_RSDR				(*(unsigned int volatile *)REG_EMAC1_RSDR)
#define     rEMAC1_DMARFC			(*(unsigned int volatile *)REG_EMAC1_DMARFC)
#define     rEMAC1_MIEN				(*(unsigned int volatile *)REG_EMAC1_MIEN)
#define     rEMAC1_MISTA			(*(unsigned int volatile *)REG_EMAC1_MISTA)
#define     rEMAC1_MGSTA			(*(unsigned int volatile *)REG_EMAC1_MGSTA)
#define     rEMAC1_MPCNT			(*(unsigned int volatile *)REG_EMAC1_MPCNT)
#define     rEMAC1_MRPC				(*(unsigned int volatile *)REG_EMAC1_MRPC)
#define     rEMAC1_DMARFS			(*(unsigned int volatile *)REG_EMAC1_DMARFS)
#define     rEMAC1_CTXDSA			(*(unsigned int volatile *)REG_EMAC1_CTXDSA)
#define     rEMAC1_CTXBSA			(*(unsigned int volatile *)REG_EMAC1_CTXBSA)
#define     rEMAC1_CRXDSA			(*(unsigned int volatile *)REG_EMAC1_CRXDSA)
#define     rEMAC1_CRXBSA			(*(unsigned int volatile *)REG_EMAC1_CRXBSA)
#define 	rEMAC1_TSCR				(*(unsigned int volatile *)REG_EMAC1_TSCR)
#define 	rEMAC1_TSMSR			(*(unsigned int volatile *)REG_EMAC1_TSMSR)
#define 	rEMAC1_TSLSR			(*(unsigned int volatile *)REG_EMAC1_TSLSR)
#define 	rEMAC1_TSIR				(*(unsigned int volatile *)REG_EMAC1_TSIR)
#define 	rEMAC1_TSAR				(*(unsigned int volatile *)REG_EMAC1_TSAR)
#define 	rEMAC1_TSMSUR			(*(unsigned int volatile *)REG_EMAC1_TSMSUR)
#define 	rEMAC1_TSLSUR			(*(unsigned int volatile *)REG_EMAC1_TSLSUR)
#define 	rEMAC1_TSMSAR			(*(unsigned int volatile *)REG_EMAC1_TSMSAR)
#define 	rEMAC1_TSLSAR			(*(unsigned int volatile *)REG_EMAC1_TSLSAR)
#define 	rEMAC1_BIST				(*(unsigned int volatile *)REG_EMAC1_BIST)


/*
  GDMA Registers
*/
#define     REG_GDMA_CTL0	(GDMA_BA+0x000)  /* Channel 0 Control Register */
#define     REG_GDMA_SRCB0	(GDMA_BA+0x004)  /* Channel 0 Source Base Address Register */
#define     REG_GDMA_DSTB0	(GDMA_BA+0x008)  /* Channel 0 Destination Base Address Register */
#define     REG_GDMA_TCNT0	(GDMA_BA+0x00C)  /* Channel 0 Transfer Count Register */
#define     REG_GDMA_CSRC0	(GDMA_BA+0x010)  /* Channel 0 Current Source Address Register */
#define     REG_GDMA_CDST0	(GDMA_BA+0x014)  /* Channel 0 Current Destination Address Register */
#define     REG_GDMA_CTCNT0	(GDMA_BA+0x018)  /* Channel 0 Current Transfer Count Register */
#define     REG_GDMA_DADR0	(GDMA_BA+0x01C)  /* Channel 0 Descriptor Address Register */
#define     REG_GDMA_CTL1	(GDMA_BA+0x020)  /* Channel 1 Control Register */
#define     REG_GDMA_SRCB1	(GDMA_BA+0x024)  /* Channel 1 Source Base Address Register */
#define     REG_GDMA_DSTB1	(GDMA_BA+0x028)  /* Channel 1 Destination Base Address Register */
#define     REG_GDMA_TCNT1	(GDMA_BA+0x02C)  /* Channel 1 Transfer Count Register */
#define     REG_GDMA_CSRC1	(GDMA_BA+0x030)  /* Channel 1 Current Source Address Register */
#define     REG_GDMA_CDST1	(GDMA_BA+0x034)  /* Channel 1 Current Destination Address Register */
#define     REG_GDMA_CTCNT1	(GDMA_BA+0x038)  /* Channel 1 Current Transfer Count Register */
#define     REG_GDMA_DADR1	(GDMA_BA+0x03C)  /* Channel 1 Descriptor Address Register */
#define     REG_GDMA_INTBUF0    (GDMA_BA+0x080)  /* GDMA Internal Buffer Word 0 */
#define     REG_GDMA_INTBUF1    (GDMA_BA+0x084)  /* GDMA Internal Buffer Word 1 */
#define     REG_GDMA_INTBUF2    (GDMA_BA+0x088)  /* GDMA Internal Buffer Word 2 */
#define     REG_GDMA_INTBUF3    (GDMA_BA+0x08C)  /* GDMA Internal Buffer Word 3 */
#define     REG_GDMA_INTBUF4    (GDMA_BA+0x090)  /* GDMA Internal Buffer Word 4 */
#define     REG_GDMA_INTBUF5    (GDMA_BA+0x094)  /* GDMA Internal Buffer Word 5 */
#define     REG_GDMA_INTBUF6    (GDMA_BA+0x098)  /* GDMA Internal Buffer Word 6 */
#define     REG_GDMA_INTBUF7    (GDMA_BA+0x09C)  /* GDMA Internal Buffer Word 7 */
#define     REG_GDMA_INTCS	(GDMA_BA+0x0A0)  /* Interrupt Control and Status Register */

#define     rCTL0			(*(unsigned int volatile *)REG_GDMA_CTL0)
#define     rSRCB0			(*(unsigned int volatile *)REG_GDMA_SRCB0)
#define     rDSTB0			(*(unsigned int volatile *)REG_GDMA_DSTB0)
#define     rTCNT0			(*(unsigned int volatile *)REG_GDMA_TCNT0)
#define     rCSRC0			(*(unsigned int volatile *)REG_GDMA_CSRC0)
#define     rCDST0			(*(unsigned int volatile *)REG_GDMA_CDST0)
#define     rCTCNT0			(*(unsigned int volatile *)REG_GDMA_CTCNT0)
#define     rDADR0			(*(unsigned int volatile *)REG_GDMA_DADR0)
#define     rCTL1			(*(unsigned int volatile *)REG_GDMA_CTL1)
#define     rSRCB1			(*(unsigned int volatile *)REG_GDMA_SRCB1)
#define     rDSTB1			(*(unsigned int volatile *)REG_GDMA_DSTB1)
#define     rTCNT1			(*(unsigned int volatile *)REG_GDMA_TCNT1)
#define     rCSRC1			(*(unsigned int volatile *)REG_GDMA_CSRC1)
#define     rCDST1			(*(unsigned int volatile *)REG_GDMA_CDST1)
#define     rCTCNT1			(*(unsigned int volatile *)REG_GDMA_CTCNT1)
#define     rDADR1			(*(unsigned int volatile *)REG_GDMA_DADR1)
#define     rINTBUF0    	(*(unsigned int volatile *)REG_GDMA_INTBUF0)   
#define     rINTBUF1    	(*(unsigned int volatile *)REG_GDMA_INTBUF1)
#define     rINTBUF2   		(*(unsigned int volatile *)REG_GDMA_INTBUF2)
#define     rINTBUF3    	(*(unsigned int volatile *)REG_GDMA_INTBUF3)
#define     rINTBUF4    	(*(unsigned int volatile *)REG_GDMA_INTBUF4)
#define     rINTBUF5    	(*(unsigned int volatile *)REG_GDMA_INTBUF5)
#define     rINTBUF6   		(*(unsigned int volatile *)REG_GDMA_INTBUF6)
#define     rINTBUF7    	(*(unsigned int volatile *)REG_GDMA_INTBUF7)
#define     rINTCS			(*(unsigned int volatile *)REG_GDMA_INTCS)


/*
  USBH Registers
*/
#define     REG_EHCVNR		(USBH_BA+0x000)  /* EHCI Version Number Register */
#define     REG_EHCSPR		(USBH_BA+0x004)  /* EHCI Structural Parameters Register */
#define     REG_EHCCPR		(USBH_BA+0x008)  /* EHCI Capability Parameters Register */
#define     REG_UCMDR		(USBH_BA+0x020)  /* USB Command Register */
#define     REG_USTSR		(USBH_BA+0x024)  /* USB Status Register */
#define     REG_UIENR		(USBH_BA+0x028)  /* USB Interrupt Enable Register */
#define     REG_UFINDR		(USBH_BA+0x02C)  /* USB Frame Index Register */
#define     REG_UPFLBAR		(USBH_BA+0x034)  /* USB Periodic Frame List Base Address Register */
#define     REG_UCALAR		(USBH_BA+0x038)  /* USB Current Asynchronous List Address Register */
#define     REG_UASSTR		(USBH_BA+0x03C)  /* USB Asynchronous Schedule Sleep Timer Register */
#define     REG_UCFGR		(USBH_BA+0x060)  /* USB Configure Flag Register */
#define     REG_UPSCR0		(USBH_BA+0x064)  /* USB Port 0 Status and Control Register */
#define     REG_UPSCR1		(USBH_BA+0x068)  /* USB Port 1 Status and Control Register */
#define     REG_USBPCR0		(USBH_BA+0x0C4)  /* USB PHY 0 Control Register */
#define     REG_USBPCR1		(USBH_BA+0x0C8)  /* USB PHY 1 Control Register */

#define     rEHCVNR			(*(unsigned int volatile *)REG_EHCVNR)
#define     rEHCSPR			(*(unsigned int volatile *)REG_EHCSPR)
#define     rEHCCPR			(*(unsigned int volatile *)REG_EHCCPR)
#define     rUCMDR			(*(unsigned int volatile *)REG_UCMDR)
#define     rUSTSR			(*(unsigned int volatile *)REG_USTSR)
#define     rUIENR			(*(unsigned int volatile *)REG_UIENR)
#define     rUFINDR			(*(unsigned int volatile *)REG_UFINDR)
#define     rUPFLBAR		(*(unsigned int volatile *)REG_UPFLBAR)
#define     rUCALAR			(*(unsigned int volatile *)REG_UCALAR)
#define     rUASSTR			(*(unsigned int volatile *)REG_UASSTR)
#define     rUCFGR			(*(unsigned int volatile *)REG_UCFGR)
#define     rUPSCR0			(*(unsigned int volatile *)REG_UPSCR0)
#define     rUPSCR1			(*(unsigned int volatile *)REG_UPSCR1)
#define     rUSBPCR0		(*(unsigned int volatile *)REG_USBPCR0)
#define     rUSBPCR1		(*(unsigned int volatile *)REG_USBPCR1)


/*
USBH OHCI Registers
*/
#define     REG_HcRev		(USBO_BA+0x000)  /* Host Controller Revision Register */
#define     REG_HcControl	(USBO_BA+0x004)  /* Host Controller Control Register */
#define     REG_HcComSts	(USBO_BA+0x008)  /* Host Controller Command Status Register */
#define     REG_HcIntSts	(USBO_BA+0x00C)  /* Host Controller Interrupt Status Register */
#define     REG_HcIntEn		(USBO_BA+0x010)  /* Host Controller Interrupt Enable Register */
#define     REG_HcIntDis	(USBO_BA+0x014)  /* Host Controller Interrupt Disable Register */
#define     REG_HcHCCA		(USBO_BA+0x018)  /* Host Controller Communication Area Register */
#define     REG_HcPerCED	(USBO_BA+0x01C)  /* Host Controller Period Current ED Register */
#define     REG_HcCtrHED	(USBO_BA+0x020)  /* Host Controller Control Head ED Registerr */
#define     REG_HcCtrCED	(USBO_BA+0x024)  /* Host Controller Control Current ED Register */
#define     REG_HcBlkHED	(USBO_BA+0x028)  /* Host Controller Bulk Head ED Register */
#define     REG_HcBlkCED	(USBO_BA+0x02C)	 /* Host Controller Bulk Current ED Register */
#define     REG_HcDoneH		(USBO_BA+0x030)	 /* Host Controller Done Head Register */
#define     REG_HcFmIntv	(USBO_BA+0x034)	 /* Host Controller Frame Interval Register */
#define     REG_HcFmRem		(USBO_BA+0x038)	 /* Host Controller Frame Remaining Register */
#define     REG_HcFNum		(USBO_BA+0x03C)	 /* Host Controller Frame Number Register */
#define     REG_HcPerSt		(USBO_BA+0x040)	 /* Host Controller Periodic Start Register */
#define     REG_HcLSTH		(USBO_BA+0x044)	 /* Host Controller Low Speed Threshold Register */
#define     REG_HcRhDeA		(USBO_BA+0x048)	 /* Host Controller Root Hub Descriptor A Register */
#define     REG_HcRhDeB		(USBO_BA+0x04C)	 /* Host Controller Root Hub Descriptor B Register */
#define     REG_HcRhSts		(USBO_BA+0x050)	 /* Host Controller Root Hub Status Register */
#define     REG_HcRhPrt1	(USBO_BA+0x054)	 /* Host Controller Root Hub Port Status [1] */
#define     REG_HcRhPrt2	(USBO_BA+0x058)	 /* Host Controller Root Hub Port Status [2] */
#define     REG_OpModEn		(USBO_BA+0x204)	 /* USB Operational Mode Enable Register */

#define     rHcRev			(*(unsigned int volatile *)REG_HcRev)
#define     rHcControl		(*(unsigned int volatile *)REG_HcControl)
#define     rHcComSts		(*(unsigned int volatile *)REG_HcComSts)
#define     rHcIntSts		(*(unsigned int volatile *)REG_HcIntSts)
#define     rHcIntEn		(*(unsigned int volatile *)REG_HcIntEn)
#define     rHcIntDis		(*(unsigned int volatile *)REG_HcIntDis)
#define     rHcHCCA			(*(unsigned int volatile *)REG_HcHCCA)
#define     rHcPerCED		(*(unsigned int volatile *)REG_HcPerCED)
#define     rHcCtrHED		(*(unsigned int volatile *)REG_HcCtrHED)
#define     rHcCtrCED		(*(unsigned int volatile *)REG_HcCtrCED)
#define     rHcBlkHED		(*(unsigned int volatile *)REG_HcBlkHED)
#define     rHcBlkCED		(*(unsigned int volatile *)REG_HcBlkCED)
#define     rHcDoneH		(*(unsigned int volatile *)REG_HcDoneH)
#define     rHcFmIntv		(*(unsigned int volatile *)REG_HcFmIntv)
#define     rHcFmRem		(*(unsigned int volatile *)REG_HcFmRem)
#define     rHcFNum			(*(unsigned int volatile *)REG_HcFNum)
#define     rHcPerSt		(*(unsigned int volatile *)REG_HcPerSt)
#define     rHcLSTH			(*(unsigned int volatile *)REG_HcLSTH)
#define     rHcRhDeA		(*(unsigned int volatile *)REG_HcRhDeA)
#define     rHcRhDeB		(*(unsigned int volatile *)REG_HcRhDeB)
#define     rHcRhSts		(*(unsigned int volatile *)REG_HcRhSts)
#define     rHcRhPrt1		(*(unsigned int volatile *)REG_HcRhPrt1)
#define     rHcRhPrt2		(*(unsigned int volatile *)REG_HcRhPrt2)
#define     rOpModEn		(*(unsigned int volatile *)REG_OpModEn)


/*
USB Device Control Registers  
*/
#define     REG_USBD_IRQ_STAT_L		(USBD_BA+0x00)  /* interrupt status register */
#define     REG_USBD_IRQ_ENB_L		(USBD_BA+0x08)  /* interrupt enable low register */
#define     REG_USBD_IRQ_STAT		(USBD_BA+0x10)  /* usb interrupt status register */
#define     REG_USBD_IRQ_ENB		(USBD_BA+0x14)  /* usb interrupt enable register */
#define     REG_USBD_OPER			(USBD_BA+0x18)  /* usb operation register */
#define     REG_USBD_FRAME_CNT		(USBD_BA+0x1C)	/* usb frame count register */
#define     REG_USBD_ADDR			(USBD_BA+0x20)  /* usb address register */
#define 		REG_USBD_TEST			(USBD_BA+0x024) /* usb Test Mode Register */
#define     REG_USBD_CEP_DATA_BUF	(USBD_BA+0x28)  /* control-ep data buffer register */
#define     REG_USBD_CEP_CTRL_STAT	(USBD_BA+0x2C)  /* control-ep control and status register */
#define     REG_USBD_CEP_IRQ_ENB	(USBD_BA+0x30)  /* control-ep interrupt enable register */
#define     REG_USBD_CEP_IRQ_STAT	(USBD_BA+0x34)  /* control-ep interrupt status register */
#define     REG_USBD_IN_TRNSFR_CNT	(USBD_BA+0x38)  /* in-transfer data count register */
#define     REG_USBD_OUT_TRNSFR_CNT	(USBD_BA+0x3C)  /* out-transfer data count register */
#define     REG_USBD_CEP_CNT		(USBD_BA+0x40)  /* control-ep data count register */
#define     REG_USBD_SETUP1_0		(USBD_BA+0x44)  /* setup byte1 & byte0 register */
#define     REG_USBD_SETUP3_2		(USBD_BA+0x48)  /* setup byte3 & byte2 register */
#define     REG_USBD_SETUP5_4		(USBD_BA+0x4C)  /* setup byte5 & byte4 register */
#define     REG_USBD_SETUP7_6		(USBD_BA+0x50)  /* setup byte7 & byte6 register */
#define     REG_USBD_CEP_START_ADDR	(USBD_BA+0x54)  /* control-ep ram start address register */
#define     REG_USBD_CEP_END_ADDR	(USBD_BA+0x58)  /* control-ep ram end address register */
#define     REG_USBD_DMA_CTRL_STS	(USBD_BA+0x5C)  /* dma control and status register */
#define     REG_USBD_DMA_CNT		(USBD_BA+0x60)  /* dma count register */
#define     REG_USBD_EPA_DATA_BUF	(USBD_BA+0x64)  /* endpoint A data buffer register */
#define     REG_USBD_EPA_IRQ_STAT	(USBD_BA+0x68)  /* endpoint A interrupt status register */
#define     REG_USBD_EPA_IRQ_ENB	(USBD_BA+0x6C)  /* endpoint A interrupt enable register */
#define     REG_USBD_EPA_DATA_CNT	(USBD_BA+0x70)  /* data count available in endpoint A buffer */
#define     REG_USBD_EPA_RSP_SC		(USBD_BA+0x74)  /* endpoint A response register set/clear */
#define     REG_USBD_EPA_MPS		(USBD_BA+0x78)  /* endpoint A max packet size register */
#define     REG_USBD_EPA_CNT		(USBD_BA+0x7C)  /* endpoint A transfer count register */
#define     REG_USBD_EPA_CFG		(USBD_BA+0x80)  /* endpoint A configuration register */
#define     REG_USBD_EPA_START_ADDR	(USBD_BA+0x84)  /* endpoint A ram start address register */
#define     REG_USBD_EPA_END_ADDR	(USBD_BA+0x88)  /* endpoint A ram end address register */
#define     REG_USBD_EPB_DATA_BUF	(USBD_BA+0x8C)  /* endpoint B data buffer register */
#define     REG_USBD_EPB_IRQ_STAT	(USBD_BA+0x90)  /* endpoint B interrupt status register */
#define     REG_USBD_EPB_IRQ_ENB	(USBD_BA+0x94)  /* endpoint B interrupt enable register */
#define     REG_USBD_EPB_DATA_CNT	(USBD_BA+0x98)  /* data count available in endpoint B buffer */
#define     REG_USBD_EPB_RSP_SC		(USBD_BA+0x9C)  /* endpoint B response register set/clear */
#define     REG_USBD_EPB_MPS		(USBD_BA+0xA0)  /* endpoint B max packet size register */
#define     REG_USBD_EPB_CNT		(USBD_BA+0xA4)  /* endpoint B transfer count register */
#define     REG_USBD_EPB_CFG		(USBD_BA+0xA8)  /* endpoint B configuration register */
#define     REG_USBD_EPB_START_ADDR	(USBD_BA+0xAC)  /* endpoint B ram start address register */
#define     REG_USBD_EPB_END_ADDR	(USBD_BA+0xB0)  /* endpoint B ram end address register */
#define     REG_USBD_EPC_DATA_BUF	(USBD_BA+0xB4)  /* endpoint C data buffer register */
#define     REG_USBD_EPC_IRQ_STAT	(USBD_BA+0xB8)  /* endpoint C interrupt status register */
#define     REG_USBD_EPC_IRQ_ENB	(USBD_BA+0xBC)  /* endpoint C interrupt enable register */
#define     REG_USBD_EPC_DATA_CNT	(USBD_BA+0xC0)  /* data count available in endpoint C buffer */
#define     REG_USBD_EPC_RSP_SC		(USBD_BA+0xC4)  /* endpoint C response register set/clear */
#define     REG_USBD_EPC_MPS		(USBD_BA+0xC8)  /* endpoint C max packet size register */
#define     REG_USBD_EPC_CNT		(USBD_BA+0xCC)  /* endpoint C transfer count register */
#define     REG_USBD_EPC_CFG		(USBD_BA+0xD0)  /* endpoint C configuration register */
#define     REG_USBD_EPC_START_ADDR	(USBD_BA+0xD4)  /* endpoint C ram start address register */
#define     REG_USBD_EPC_END_ADDR	(USBD_BA+0xD8)  /* endpoint C ram end address register */
#define     REG_USBD_EPD_DATA_BUF	(USBD_BA+0xDC)  /* endpoint D data buffer register */
#define     REG_USBD_EPD_IRQ_STAT	(USBD_BA+0xE0)  /* endpoint D interrupt status register */
#define     REG_USBD_EPD_IRQ_ENB	(USBD_BA+0xE4)  /* endpoint D interrupt enable register */
#define     REG_USBD_EPD_DATA_CNT	(USBD_BA+0xE8)  /* data count available in endpoint D buffer */
#define     REG_USBD_EPD_RSP_SC		(USBD_BA+0xEC)  /* endpoint D response register set/clear */
#define     REG_USBD_EPD_MPS		(USBD_BA+0xF0)  /* endpoint D max packet size register */
#define     REG_USBD_EPD_CNT		(USBD_BA+0xF4)  /* endpoint D transfer count register */
#define     REG_USBD_EPD_CFG		(USBD_BA+0xF8)  /* endpoint D configuration register */
#define     REG_USBD_EPD_START_ADDR	(USBD_BA+0xFC)  /* endpoint D ram start address register */
#define     REG_USBD_EPD_END_ADDR	(USBD_BA+0x100) /* endpoint D ram end address register */
#define     REG_USBD_EPE_DATA_BUF	(USBD_BA+0x104) /* endpoint E data buffer register */
#define     REG_USBD_EPE_IRQ_STAT	(USBD_BA+0x108) /* endpoint E interrupt status register */
#define     REG_USBD_EPE_IRQ_ENB	(USBD_BA+0x10C)	/* endpoint E interrupt enable register */
#define     REG_USBD_EPE_DATA_CNT	(USBD_BA+0x110) /* data count available in endpoint E buffer */
#define     REG_USBD_EPE_RSP_SC		(USBD_BA+0x114) /* endpoint E response register set/clear */
#define     REG_USBD_EPE_MPS		(USBD_BA+0x118) /* endpoint E max packet size register */
#define     REG_USBD_EPE_CNT		(USBD_BA+0x11C) /* endpoint E transfer count register */
#define     REG_USBD_EPE_CFG		(USBD_BA+0x120) /* endpoint E configuration register */
#define     REG_USBD_EPE_START_ADDR	(USBD_BA+0x124) /* endpoint E ram start address register */
#define     REG_USBD_EPE_END_ADDR	(USBD_BA+0x128) /* endpoint E ram end address register */
#define     REG_USBD_EPF_DATA_BUF	(USBD_BA+0x12C) /* endpoint F data buffer register */
#define     REG_USBD_EPF_IRQ_STAT	(USBD_BA+0x130) /* endpoint F interrupt status register */
#define     REG_USBD_EPF_IRQ_ENB	(USBD_BA+0x134) /* endpoint F interrupt enable register */
#define     REG_USBD_EPF_DATA_CNT	(USBD_BA+0x138) /* data count available in endpoint F buffer */
#define     REG_USBD_EPF_RSP_SC		(USBD_BA+0x13C)	/* endpoint F response register set/clear */
#define     REG_USBD_EPF_MPS		(USBD_BA+0x140) /* endpoint F max packet size register */
#define     REG_USBD_EPF_CNT		(USBD_BA+0x144) /* endpoint F transfer count register */
#define     REG_USBD_EPF_CFG		(USBD_BA+0x148) /* endpoint F configuration register */
#define     REG_USBD_EPF_START_ADDR	(USBD_BA+0x14C) /* endpoint F ram start address register */
#define     REG_USBD_EPF_END_ADDR	(USBD_BA+0x150) /* endpoint F ram end address register */
#define     REG_USBD_EPG_DATA_BUF	(USBD_BA+0x154) /* endpoint G data buffer register */
#define     REG_USBD_EPG_IRQ_STAT	(USBD_BA+0x158) /* endpoint G interrupt status register */
#define     REG_USBD_EPG_IRQ_ENB	(USBD_BA+0x15C)	/* endpoint G interrupt enable register */
#define     REG_USBD_EPG_DATA_CNT	(USBD_BA+0x160) /* data count available in endpoint G buffer */
#define     REG_USBD_EPG_RSP_SC		(USBD_BA+0x164) /* endpoint G response register set/clear */
#define     REG_USBD_EPG_MPS		(USBD_BA+0x168) /* endpoint G max packet size register */
#define     REG_USBD_EPG_CNT		(USBD_BA+0x16C) /* endpoint G transfer count register */
#define     REG_USBD_EPG_CFG		(USBD_BA+0x170) /* endpoint G configuration register */
#define     REG_USBD_EPG_START_ADDR	(USBD_BA+0x174) /* endpoint G ram start address register */
#define     REG_USBD_EPG_END_ADDR	(USBD_BA+0x178) /* endpoint G ram end address register */
#define     REG_USBD_EPH_DATA_BUF	(USBD_BA+0x17C) /* endpoint H data buffer register */
#define     REG_USBD_EPH_IRQ_STAT	(USBD_BA+0x180) /* endpoint H interrupt status register */
#define     REG_USBD_EPH_IRQ_ENB	(USBD_BA+0x184) /* endpoint H interrupt enable register */
#define     REG_USBD_EPH_DATA_CNT	(USBD_BA+0x188) /* data count available in endpoint H buffer */
#define     REG_USBD_EPH_RSP_SC		(USBD_BA+0x18C)	/* endpoint H response register set/clear */
#define     REG_USBD_EPH_MPS		(USBD_BA+0x190) /* endpoint H max packet size register */
#define     REG_USBD_EPH_CNT		(USBD_BA+0x194) /* endpoint H transfer count register */
#define     REG_USBD_EPH_CFG		(USBD_BA+0x198) /* endpoint H configuration register */
#define     REG_USBD_EPH_START_ADDR	(USBD_BA+0x19C) /* endpoint H ram start address register */
#define     REG_USBD_EPH_END_ADDR	(USBD_BA+0x1A0) /* endpoint H ram end address register */
#define     REG_USBD_EPI_DATA_BUF	(USBD_BA+0x1A4) /* endpoint I data buffer register */
#define     REG_USBD_EPI_IRQ_STAT	(USBD_BA+0x1A8) /* endpoint I interrupt status register */
#define     REG_USBD_EPI_IRQ_ENB	(USBD_BA+0x1AC)	/* endpoint I interrupt enable register */
#define     REG_USBD_EPI_DATA_CNT	(USBD_BA+0x1B0) /* data count available in endpoint I buffer */
#define     REG_USBD_EPI_RSP_SC		(USBD_BA+0x1B4) /* endpoint I response register set/clear */
#define     REG_USBD_EPI_MPS		(USBD_BA+0x1B8) /* endpoint I max packet size register */
#define     REG_USBD_EPI_CNT		(USBD_BA+0x1BC) /* endpoint I transfer count register */
#define     REG_USBD_EPI_CFG		(USBD_BA+0x1C0) /* endpoint I configuration register */
#define     REG_USBD_EPI_START_ADDR	(USBD_BA+0x1C4) /* endpoint I ram start address register */
#define     REG_USBD_EPI_END_ADDR	(USBD_BA+0x1C8) /* endpoint I ram end address register */
#define     REG_USBD_EPJ_DATA_BUF	(USBD_BA+0x1CC) /* endpoint J data buffer register */
#define     REG_USBD_EPJ_IRQ_STAT	(USBD_BA+0x1D0) /* endpoint J interrupt status register */
#define     REG_USBD_EPJ_IRQ_ENB	(USBD_BA+0x1D4) /* endpoint J interrupt enable register */
#define     REG_USBD_EPJ_DATA_CNT	(USBD_BA+0x1D8) /* data count available in endpoint J buffer */
#define     REG_USBD_EPJ_RSP_SC		(USBD_BA+0x1DC)	/* endpoint J response register set/clear */
#define     REG_USBD_EPJ_MPS		(USBD_BA+0x1E0) /* endpoint J max packet size register */
#define     REG_USBD_EPJ_CNT		(USBD_BA+0x1E4) /* endpoint J transfer count register */
#define     REG_USBD_EPJ_CFG		(USBD_BA+0x1E8) /* endpoint J configuration register */
#define     REG_USBD_EPJ_START_ADDR	(USBD_BA+0x1EC) /* endpoint J ram start address register */
#define     REG_USBD_EPJ_END_ADDR	(USBD_BA+0x1F0) /* endpoint J ram end address register */
#define     REG_USBD_EPK_DATA_BUF	(USBD_BA+0x1F4) /* endpoint K data buffer register */
#define     REG_USBD_EPK_IRQ_STAT	(USBD_BA+0x1F8) /* endpoint K interrupt status register */
#define     REG_USBD_EPK_IRQ_ENB	(USBD_BA+0x1FC)	/* endpoint K interrupt enable register */
#define     REG_USBD_EPK_DATA_CNT	(USBD_BA+0x200) /* data count available in endpoint K buffer */
#define     REG_USBD_EPK_RSP_SC		(USBD_BA+0x204) /* endpoint K response register set/clear */
#define     REG_USBD_EPK_MPS		(USBD_BA+0x208) /* endpoint K max packet size register */
#define     REG_USBD_EPK_CNT		(USBD_BA+0x20C) /* endpoint K transfer count register */
#define     REG_USBD_EPK_CFG		(USBD_BA+0x210) /* endpoint K configuration register */
#define     REG_USBD_EPK_START_ADDR	(USBD_BA+0x214) /* endpoint K ram start address register */
#define     REG_USBD_EPK_END_ADDR	(USBD_BA+0x218) /* endpoint K ram end address register */
#define     REG_USBD_EPL_DATA_BUF	(USBD_BA+0x21C) /* endpoint L data buffer register */
#define     REG_USBD_EPL_IRQ_STAT	(USBD_BA+0x220) /* endpoint L interrupt status register */
#define     REG_USBD_EPL_IRQ_ENB	(USBD_BA+0x224) /* endpoint L interrupt enable register */
#define     REG_USBD_EPL_DATA_CNT	(USBD_BA+0x228) /* data count available in endpoint L buffer */
#define     REG_USBD_EPL_RSP_SC		(USBD_BA+0x22C)	/* endpoint L response register set/clear */
#define     REG_USBD_EPL_MPS		(USBD_BA+0x230) /* endpoint L max packet size register */
#define     REG_USBD_EPL_CNT		(USBD_BA+0x234) /* endpoint L transfer count register */
#define     REG_USBD_EPL_CFG		(USBD_BA+0x238) /* endpoint L configuration register */
#define     REG_USBD_EPL_START_ADDR	(USBD_BA+0x23C) /* endpoint L ram start address register */
#define     REG_USBD_EPL_END_ADDR	(USBD_BA+0x240) /* endpoint L ram end address register */
#define     REG_USBD_AHB_DMA_ADDR	(USBD_BA+0x700) /* AHB_DMA address register */
#define     REG_USBD_PHY_CTL		(USBD_BA+0x704) /* USB PHY control register */


#define     rIRQ_STAT_L		(*(unsigned int volatile *)REG_USBD_IRQ_STAT_L)
#define     rIRQ_ENB_L		(*(unsigned int volatile *)REG_USBD_IRQ_ENB_L)
#define     rIRQ_STAT		(*(unsigned int volatile *)REG_USBD_IRQ_STAT)
#define     rIRQ_ENB		(*(unsigned int volatile *)REG_USBD_IRQ_ENB)
#define     rOPER			(*(unsigned int volatile *)REG_USBD_OPER)
#define     rFRAME_CNT		(*(unsigned int volatile *)REG_USBD_FRAME_CNT)
#define     rADDR			(*(unsigned int volatile *)REG_USBD_ADDR)
#define     rCEP_DATA_BUF	(*(unsigned int volatile *)REG_USBD_CEP_DATA_BUF)
#define     rCEP_CTRL_STAT	(*(unsigned int volatile *)REG_USBD_CEP_CTRL_STAT)
#define     rCEP_IRQ_ENB	(*(unsigned int volatile *)REG_USBD_CEP_IRQ_ENB)
#define     rCEP_IRQ_STAT	(*(unsigned int volatile *)REG_USBD_CEP_IRQ_STAT)
#define     rIN_TRNSFR_CNT	(*(unsigned int volatile *)REG_USBD_IN_TRNSFR_CNT)
#define     rOUT_TRNSFR_CNT	(*(unsigned int volatile *)REG_USBD_OUT_TRNSFR_CNT)
#define     rCEP_CNT		(*(unsigned int volatile *)REG_USBD_CEP_CNT)
#define     rSETUP1_0		(*(unsigned int volatile *)REG_USBD_SETUP1_0)
#define     rSETUP3_2		(*(unsigned int volatile *)REG_USBD_SETUP3_2)
#define     rSETUP5_4		(*(unsigned int volatile *)REG_USBD_SETUP5_4)
#define     rSETUP7_6		(*(unsigned int volatile *)REG_USBD_SETUP7_6)
#define     rCEP_START_ADDR	(*(unsigned int volatile *)REG_USBD_CEP_START_ADDR)
#define     rCEP_END_ADDR	(*(unsigned int volatile *)REG_USBD_CEP_END_ADDR)
#define     rDMA_CTRL_STS	(*(unsigned int volatile *)REG_USBD_DMA_CTRL_STS)
#define     rDMA_CNT		(*(unsigned int volatile *)REG_USBD_DMA_CNT)
#define     rEPA_DATA_BUF	(*(unsigned int volatile *)REG_USBD_EPA_DATA_BUF)
#define     rEPA_IRQ_STAT	(*(unsigned int volatile *)REG_USBD_EPA_IRQ_STAT)
#define     rEPA_IRQ_ENB	(*(unsigned int volatile *)REG_USBD_EPA_IRQ_ENB)
#define     rEPA_DATA_CNT	(*(unsigned int volatile *)REG_USBD_EPA_DATA_CNT)
#define     rEPA_RSP_SC		(*(unsigned int volatile *)REG_USBD_EPA_RSP_SC)
#define     rEPA_MPS		(*(unsigned int volatile *)REG_USBD_EPA_MPS)
#define     rEPA_CNT		(*(unsigned int volatile *)REG_USBD_EPA_CNT)
#define     rEPA_CFG		(*(unsigned int volatile *)REG_USBD_EPA_CFG)
#define     rEPA_START_ADDR	(*(unsigned int volatile *)REG_USBD_EPA_START_ADDR)
#define     rEPA_END_ADDR	(*(unsigned int volatile *)REG_USBD_EPA_END_ADDR)
#define     rEPB_DATA_BUF	(*(unsigned int volatile *)REG_USBD_EPB_DATA_BUF)
#define     rEPB_IRQ_STAT	(*(unsigned int volatile *)REG_USBD_EPB_IRQ_STAT)
#define     rEPB_IRQ_ENB	(*(unsigned int volatile *)REG_USBD_EPB_IRQ_ENB)
#define     rEPB_DATA_CNT	(*(unsigned int volatile *)REG_USBD_EPB_DATA_CNT)
#define     rEPB_RSP_SC		(*(unsigned int volatile *)REG_USBD_EPB_RSP_SC)
#define     rEPB_MPS		(*(unsigned int volatile *)REG_USBD_EPB_MPS)
#define     rEPB_CNT		(*(unsigned int volatile *)REG_USBD_EPB_CNT)
#define     rEPB_CFG		(*(unsigned int volatile *)REG_USBD_EPB_CFG)
#define     rEPB_START_ADDR	(*(unsigned int volatile *)REG_USBD_EPB_START_ADDR)
#define     rEPB_END_ADDR	(*(unsigned int volatile *)REG_USBD_EPB_END_ADDR)
#define     rEPC_DATA_BUF	(*(unsigned int volatile *)REG_USBD_EPC_DATA_BUF)
#define     rEPC_IRQ_STAT	(*(unsigned int volatile *)REG_USBD_EPC_IRQ_STAT)
#define     rEPC_IRQ_ENB	(*(unsigned int volatile *)REG_USBD_EPC_IRQ_ENB)
#define     rEPC_DATA_CNT	(*(unsigned int volatile *)REG_USBD_EPC_DATA_CNT)
#define     rEPC_RSP_SC		(*(unsigned int volatile *)REG_USBD_EPC_RSP_SC)
#define     rEPC_MPS		(*(unsigned int volatile *)REG_USBD_EPC_MPS)
#define     rEPC_CNT		(*(unsigned int volatile *)REG_USBD_EPC_CNT)
#define     rEPC_CFG		(*(unsigned int volatile *)REG_USBD_EPC_CFG)
#define     rEPC_START_ADDR	(*(unsigned int volatile *)REG_USBD_EPC_START_ADDR)
#define     rEPC_END_ADDR	(*(unsigned int volatile *)REG_USBD_EPC_END_ADDR)
#define     rEPD_DATA_BUF	(*(unsigned int volatile *)REG_USBD_EPD_DATA_BUF)
#define     rEPD_IRQ_STAT	(*(unsigned int volatile *)REG_USBD_EPD_IRQ_STAT)
#define     rEPD_IRQ_ENB	(*(unsigned int volatile *)REG_USBD_EPD_IRQ_ENB)
#define     rEPD_DATA_CNT	(*(unsigned int volatile *)REG_USBD_EPD_DATA_CNT)
#define     rEPD_RSP_SC		(*(unsigned int volatile *)REG_USBD_EPD_RSP_SC)
#define     rEPD_MPS		(*(unsigned int volatile *)REG_USBD_EPD_MPS)
#define     rEPD_CNT		(*(unsigned int volatile *)REG_USBD_EPD_CNT)
#define     rEPD_CFG		(*(unsigned int volatile *)REG_USBD_EPD_CFG)
#define     rEPD_START_ADDR	(*(unsigned int volatile *)REG_USBD_EPD_START_ADDR)
#define     rEPD_END_ADDR	(*(unsigned int volatile *)REG_USBD_EPD_END_ADDR)
#define     rEPE_DATA_BUF	(*(unsigned int volatile *)REG_USBD_EPE_DATA_BUF)
#define     rEPE_IRQ_STAT	(*(unsigned int volatile *)REG_USBD_EPE_IRQ_STAT)
#define     rEPE_IRQ_ENB	(*(unsigned int volatile *)REG_USBD_EPE_IRQ_ENB)
#define     rEPE_DATA_CNT	(*(unsigned int volatile *)REG_USBD_EPE_DATA_CNT)
#define     rEPE_RSP_SC		(*(unsigned int volatile *)REG_USBD_EPE_RSP_SC)
#define     rEPE_MPS		(*(unsigned int volatile *)REG_USBD_EPE_MPS)
#define     rEPE_CNT		(*(unsigned int volatile *)REG_USBD_EPE_CNT)
#define     rEPE_CFG		(*(unsigned int volatile *)REG_USBD_EPE_CFG)
#define     rEPE_START_ADDR	(*(unsigned int volatile *)REG_USBD_EPE_START_ADDR)
#define     rEPE_END_ADDR	(*(unsigned int volatile *)REG_USBD_EPE_END_ADDR)
#define     rEPF_DATA_BUF	(*(unsigned int volatile *)REG_USBD_EPF_DATA_BUF)
#define     rEPF_IRQ_STAT	(*(unsigned int volatile *)REG_USBD_EPF_IRQ_STAT)
#define     rEPF_IRQ_ENB	(*(unsigned int volatile *)REG_USBD_EPF_IRQ_ENB)
#define     rEPF_DATA_CNT	(*(unsigned int volatile *)REG_USBD_EPF_DATA_CNT)
#define     rEPF_RSP_SC		(*(unsigned int volatile *)REG_USBD_EPF_RSP_SC)
#define     rEPF_MPS		(*(unsigned int volatile *)REG_USBD_EPF_MPS)
#define     rEPF_CNT		(*(unsigned int volatile *)REG_USBD_EPF_CNT)
#define     rEPF_CFG		(*(unsigned int volatile *)REG_USBD_EPF_CFG)
#define     rEPF_START_ADDR	(*(unsigned int volatile *)REG_USBD_EPF_START_ADDR)
#define     rEPF_END_ADDR	(*(unsigned int volatile *)REG_USBD_EPF_END_ADDR)
#define     rEPG_DATA_BUF	(*(unsigned int volatile *)REG_USBD_EPG_DATA_BUF)
#define     rEPG_IRQ_STAT	(*(unsigned int volatile *)REG_USBD_EPG_IRQ_STAT)
#define     rEPG_IRQ_ENB	(*(unsigned int volatile *)REG_USBD_EPG_IRQ_ENB)
#define     rEPG_DATA_CNT	(*(unsigned int volatile *)REG_USBD_EPG_DATA_CNT)
#define     rEPG_RSP_SC		(*(unsigned int volatile *)REG_USBD_EPG_RSP_SC)
#define     rEPG_MPS		(*(unsigned int volatile *)REG_USBD_EPG_MPS)
#define     rEPG_CNT		(*(unsigned int volatile *)REG_USBD_EPG_CNT)
#define     rEPG_CFG		(*(unsigned int volatile *)REG_USBD_EPG_CFG)
#define     rEPG_START_ADDR	(*(unsigned int volatile *)REG_USBD_EPG_START_ADDR)
#define     rEPG_END_ADDR	(*(unsigned int volatile *)REG_USBD_EPG_END_ADDR)
#define     rEPH_DATA_BUF	(*(unsigned int volatile *)REG_USBD_EPH_DATA_BUF)
#define     rEPH_IRQ_STAT	(*(unsigned int volatile *)REG_USBD_EPH_IRQ_STAT)
#define     rEPH_IRQ_ENB	(*(unsigned int volatile *)REG_USBD_EPH_IRQ_ENB)
#define     rEPH_DATA_CNT	(*(unsigned int volatile *)REG_USBD_EPH_DATA_CNT)
#define     rEPH_RSP_SC		(*(unsigned int volatile *)REG_USBD_EPH_RSP_SC)
#define     rEPH_MPS		(*(unsigned int volatile *)REG_USBD_EPH_MPS)
#define     rEPH_CNT		(*(unsigned int volatile *)REG_USBD_EPH_CNT)
#define     rEPH_CFG		(*(unsigned int volatile *)REG_USBD_EPH_CFG)
#define     rEPH_START_ADDR	(*(unsigned int volatile *)REG_USBD_EPH_START_ADDR)
#define     rEPH_END_ADDR	(*(unsigned int volatile *)REG_USBD_EPH_END_ADDR)
#define     rEPI_DATA_BUF	(*(unsigned int volatile *)REG_USBD_EPI_DATA_BUF)
#define     rEPI_IRQ_STAT	(*(unsigned int volatile *)REG_USBD_EPI_IRQ_STAT)
#define     rEPI_IRQ_ENB	(*(unsigned int volatile *)REG_USBD_EPI_IRQ_ENB)
#define     rEPI_DATA_CNT	(*(unsigned int volatile *)REG_USBD_EPI_DATA_CNT)
#define     rEPI_RSP_SC		(*(unsigned int volatile *)REG_USBD_EPI_RSP_SC)
#define     rEPI_MPS		(*(unsigned int volatile *)REG_USBD_EPI_MPS)
#define     rEPI_CNT		(*(unsigned int volatile *)REG_USBD_EPI_CNT)
#define     rEPI_CFG		(*(unsigned int volatile *)REG_USBD_EPI_CFG)
#define     rEPI_START_ADDR	(*(unsigned int volatile *)REG_USBD_EPI_START_ADDR)
#define     rEPI_END_ADDR	(*(unsigned int volatile *)REG_USBD_EPI_END_ADDR)
#define     rEPJ_DATA_BUF	(*(unsigned int volatile *)REG_USBD_EPJ_DATA_BUF)
#define     rEPJ_IRQ_STAT	(*(unsigned int volatile *)REG_USBD_EPJ_IRQ_STAT)
#define     rEPJ_IRQ_ENB	(*(unsigned int volatile *)REG_USBD_EPJ_IRQ_ENB)
#define     rEPJ_DATA_CNT	(*(unsigned int volatile *)REG_USBD_EPJ_DATA_CNT)
#define     rEPJ_RSP_SC		(*(unsigned int volatile *)REG_USBD_EPJ_RSP_SC)
#define     rEPJ_MPS		(*(unsigned int volatile *)REG_USBD_EPJ_MPS)
#define     rEPJ_CNT		(*(unsigned int volatile *)REG_USBD_EPJ_CNT)
#define     rEPJ_CFG		(*(unsigned int volatile *)REG_USBD_EPJ_CFG)
#define     rEPJ_START_ADDR	(*(unsigned int volatile *)REG_USBD_EPJ_START_ADDR)
#define     rEPJ_END_ADDR	(*(unsigned int volatile *)REG_USBD_EPJ_END_ADDR)
#define     rEPK_DATA_BUF	(*(unsigned int volatile *)REG_USBD_EPK_DATA_BUF)
#define     rEPK_IRQ_STAT	(*(unsigned int volatile *)REG_USBD_EPK_IRQ_STAT)
#define     rEPK_IRQ_ENB	(*(unsigned int volatile *)REG_USBD_EPK_IRQ_ENB)
#define     rEPK_DATA_CNT	(*(unsigned int volatile *)REG_USBD_EPK_DATA_CNT)
#define     rEPK_RSP_SC		(*(unsigned int volatile *)REG_USBD_EPK_RSP_SC)
#define     rEPK_MPS		(*(unsigned int volatile *)REG_USBD_EPK_MPS)
#define     rEPK_CNT		(*(unsigned int volatile *)REG_USBD_EPK_CNT)
#define     rEPK_CFG		(*(unsigned int volatile *)REG_USBD_EPK_CFG)
#define     rEPK_START_ADDR	(*(unsigned int volatile *)REG_USBD_EPK_START_ADDR)
#define     rEPK_END_ADDR	(*(unsigned int volatile *)REG_USBD_EPK_END_ADDR)
#define     rEPL_DATA_BUF	(*(unsigned int volatile *)REG_USBD_EPL_DATA_BUF)
#define     rEPL_IRQ_STAT	(*(unsigned int volatile *)REG_USBD_EPL_IRQ_STAT)
#define     rEPL_IRQ_ENB	(*(unsigned int volatile *)REG_USBD_EPL_IRQ_ENB)
#define     rEPL_DATA_CNT	(*(unsigned int volatile *)REG_USBD_EPL_DATA_CNT)
#define     rEPL_RSP_SC		(*(unsigned int volatile *)REG_USBD_EPL_RSP_SC)
#define     rEPL_MPS		(*(unsigned int volatile *)REG_USBD_EPL_MPS)
#define     rEPL_CNT		(*(unsigned int volatile *)REG_USBD_EPL_CNT)
#define     rEPL_CFG		(*(unsigned int volatile *)REG_USBD_EPL_CFG)
#define     rEPL_START_ADDR	(*(unsigned int volatile *)REG_USBD_EPL_START_ADDR)
#define     rEPL_END_ADDR	(*(unsigned int volatile *)REG_USBD_EPL_END_ADDR)
#define     rAHB_DMA_ADDR	(*(unsigned int volatile *)REG_USBD_AHB_DMA_ADDR)
#define     rUSB_PHY_CTL	(*(unsigned int volatile *)REG_USB_PHY_CTL)


/*                                                              
  LCM Control Registers  
*/
#define     REG_LCM_DCCS	 	(LCM_BA+0x00)  /* Display Controller Control/Status Register */
#define     REG_LCM_DEV_CTRL 	(LCM_BA+0x04)  /* Display Output Device Control Register */
#define     REG_LCM_MPU_CMD	 	(LCM_BA+0x08)  /* MPU-Interface LCD Write Command */
#define     REG_LCM_INT_CS	 	(LCM_BA+0x0c)  /* Interrupt Control/Status Register */
#define     REG_LCM_CRTC_SIZE   (LCM_BA+0x10)  /* CRTC Display Size Control Register */
#define     REG_LCM_CRTC_DEND   (LCM_BA+0x14)  /* CRTC Display Enable End */
#define     REG_LCM_CRTC_HR	 	(LCM_BA+0x18)  /* CRTC Internal Horizontal Retrace Control Register */
#define     REG_LCM_CRTC_HSYNC  (LCM_BA+0x1C)  /* CRTC Horizontal Sync Control Register */
#define     REG_LCM_CRTC_VR	 	(LCM_BA+0x20)  /* CRTC Internal Vertical Retrace Control Register */
#define     REG_LCM_VA_BADDR0   (LCM_BA+0x24)  /* Video Stream Frame Buffer-0 Starting Address */
#define     REG_LCM_VA_BADDR1   (LCM_BA+0x28)  /* Video Stream Frame Buffer-1 Starting Address */
#define     REG_LCM_VA_FBCTRL   (LCM_BA+0x2C)  /* Video Stream Frame Buffer Control Register */
#define     REG_LCM_VA_SCALE    (LCM_BA+0x30)  /* Video Stream Scaling Control Register */
#define     REG_LCM_VA_WIN      (LCM_BA+0x38)  /* Image Stream Active Window Coordinates */
#define     REG_LCM_VA_STUFF    (LCM_BA+0x3C)  /* Image Stream Stuff Pixel */
#define     REG_LCM_OSD_WINS    (LCM_BA+0x40)  /* OSD Window Starting Coordinates */
#define     REG_LCM_OSD_WINE    (LCM_BA+0x44)  /* OSD Window Ending Coordinates */
#define     REG_LCM_OSD_BADDR   (LCM_BA+0x48)  /* OSD Stream Frame Buffer Starting Address */
#define     REG_LCM_OSD_FBCTRL  (LCM_BA+0x4c)  /* OSD Stream Frame Buffer Control Register */
#define     REG_LCM_OSD_OVERLAY (LCM_BA+0x50)  /* OSD Overlay Control Register */
#define     REG_LCM_OSD_CKEY    (LCM_BA+0x54)  /* OSD Overlay Color-Key Pattern Register */
#define     REG_LCM_OSD_CMASK   (LCM_BA+0x58)  /* OSD Overlay Color-Key Mask Register */
#define     REG_LCM_OSD_SKIP1   (LCM_BA+0x5C)  /* OSD Window Skip1 Register */
#define     REG_LCM_OSD_SKIP2   (LCM_BA+0x60)  /* OSD Window Skip2 Register */
#define     REG_LCM_OSD_SCALE   (LCM_BA+0x64)  /* OSD horizontal up scaling control register */
#define     REG_LCM_MPU_VSYNC   (LCM_BA+0x68)  /* MPU Vsync control register */
#define     REG_LCM_HC_CTRL	 	(LCM_BA+0x6C)  /* Hardware cursor control Register */
#define     REG_LCM_HC_POS    	(LCM_BA+0x70)  /* Hardware cursot tip point potison on va picture */
#define     REG_LCM_HC_WBCTRL   (LCM_BA+0x74)  /* Hardware Cursor Window Buffer Control Register */
#define     REG_LCM_HC_BADDR    (LCM_BA+0x78)  /* Hardware cursor memory base address register */
#define     REG_LCM_HC_COLOR0   (LCM_BA+0x7C)  /* Hardware cursor color ram register mapped to bpp = 0 */
#define     REG_LCM_HC_COLOR1   (LCM_BA+0x80)  /* Hardware cursor color ram register mapped to bpp = 1 */
#define     REG_LCM_HC_COLOR2   (LCM_BA+0x84)  /* Hardware cursor color ram register mapped to bpp = 2 */
#define     REG_LCM_HC_COLOR3   (LCM_BA+0x88)  /* Hardware cursor color ram register mapped to bpp = 3 */

#define     rDCCS			(*(unsigned int volatile *)REG_LCM_DCCS)
#define     rDEV_CTRL   	(*(unsigned int volatile *)REG_LCM_DEV_CTRL)
#define     rMPU_CMD	 	(*(unsigned int volatile *)REG_LCM_MPU_CMD)
#define     rINT_CS	 		(*(unsigned int volatile *)REG_LCM_INT_CS)
#define     rCRTC_SIZE   	(*(unsigned int volatile *)REG_LCM_CRTC_SIZE)
#define     rCRTC_DEND   	(*(unsigned int volatile *)REG_LCM_CRTC_DEND)
#define     rCRTC_HR	 	(*(unsigned int volatile *)REG_LCM_CRTC_HR)
#define     rCRTC_HSYNC  	(*(unsigned int volatile *)REG_LCM_CRTC_HSYNC)
#define     rCRTC_VR	 	(*(unsigned int volatile *)REG_LCM_CRTC_VR)
#define     rVA_BADDR0   	(*(unsigned int volatile *)REG_LCM_VA_BADDR0)
#define     rVA_BADDR1  	(*(unsigned int volatile *)REG_LCM_VA_BADDR1)
#define     rVA_FBCTRL   	(*(unsigned int volatile *)REG_LCM_VA_FBCTRL)
#define     rVA_SCALE    	(*(unsigned int volatile *)REG_LCM_VA_SCALE)
#define     rVA_WIN      	(*(unsigned int volatile *)REG_LCM_VA_WIN)
#define     rVA_STUFF    	(*(unsigned int volatile *)REG_LCM_VA_STUFF)
#define     rOSD_WINS    	(*(unsigned int volatile *)REG_LCM_OSD_WINS)
#define     rOSD_WINE    	(*(unsigned int volatile *)REG_LCM_OSD_WINE)
#define     rOSD_BADDR   	(*(unsigned int volatile *)REG_LCM_OSD_BADDR)
#define     rOSD_FBCTRL  	(*(unsigned int volatile *)REG_LCM_OSD_FBCTRL)
#define     rOSD_OVERLAY 	(*(unsigned int volatile *)REG_LCM_OSD_OVERLAY)
#define     rOSD_CKEY    	(*(unsigned int volatile *)REG_LCM_OSD_CKEY)
#define     rOSD_CMASK   	(*(unsigned int volatile *)REG_LCM_OSD_CMASK)
#define     rOSD_SKIP1   	(*(unsigned int volatile *)REG_LCM_OSD_SKIP1)
#define     rOSD_SKIP2   	(*(unsigned int volatile *)REG_LCM_OSD_SKIP2)
#define     rOSD_SCALE   	(*(unsigned int volatile *)REG_LCM_OSD_SCALE)
#define     rMPU_VSYNC   	(*(unsigned int volatile *)REG_LCM_MPU_VSYNC)
#define     rHC_CTRL	 	(*(unsigned int volatile *)REG_LCM_HC_CTRL)
#define     rHC_POS    		(*(unsigned int volatile *)REG_LCM_HC_POS)
#define     rHC_WBCTRL  	(*(unsigned int volatile *)REG_LCM_HC_WBCTRL)
#define     rHC_BADDR    	(*(unsigned int volatile *)REG_LCM_HC_BADDR)
#define     rHC_COLOR0   	(*(unsigned int volatile *)REG_LCM_HC_COLOR0)
#define     rHC_COLOR1   	(*(unsigned int volatile *)REG_LCM_HC_COLOR1)
#define     rHC_COLOR2   	(*(unsigned int volatile *)REG_LCM_HC_COLOR2)
#define     rHC_COLOR3   	(*(unsigned int volatile *)REG_LCM_HC_COLOR3)


/*
  Audio Interface Control Registers  
*/
#define     REG_ACTL_CON		(ACTL_BA+0x00)  /* Audio controller control register */
#define     REG_ACTL_RESET		(ACTL_BA+0x04)  /* Sub block reset control register */
#define     REG_ACTL_RDSTB		(ACTL_BA+0x08)  /* DMA destination base address register for record */
#define     REG_ACTL_RDST_LENGTH	(ACTL_BA+0x0C)  /* DMA destination length register for record */
#define     REG_ACTL_RDSTC          (ACTL_BA+0x10)  /* DMA destination current address */
#define     REG_ACTL_RSR		(ACTL_BA+0x14)  /* Record status register */
#define     REG_ACTL_PDSTB		(ACTL_BA+0x18)  /* DMA destination base address register for play */
#define     REG_ACTL_PDST_LENGTH	(ACTL_BA+0x1C)  /* DMA destination length register for play */
#define     REG_ACTL_PDSTC		(ACTL_BA+0x20)  /* DMA destination current address register for play */
#define     REG_ACTL_PSR		(ACTL_BA+0x24)  /* Play status register */
#define     REG_ACTL_IISCON		(ACTL_BA+0x28)  /* IIS control register */
#define     REG_ACTL_ACCON		(ACTL_BA+0x2C)  /* AC-link control register */
#define     REG_ACTL_ACOS0		(ACTL_BA+0x30)  /* AC-link out slot 0 */
#define     REG_ACTL_ACOS1		(ACTL_BA+0x34)  /* AC-link out slot 1 */
#define     REG_ACTL_ACOS2		(ACTL_BA+0x38)  /* AC-link out slot 2 */
#define     REG_ACTL_ACIS0		(ACTL_BA+0x3C)  /* AC-link in slot 0 */
#define     REG_ACTL_ACIS1		(ACTL_BA+0x40)  /* AC-link in slot 1 */
#define     REG_ACTL_ACIS2		(ACTL_BA+0x44)  /* AC-link in slot 2 */
#define     REG_ACTL_COUNTER	(ACTL_BA+0x48)  /* DMA counter down values */

#define     rACTL_CON			(*(unsigned int volatile *)REG_ACTL_CON)
#define     rACTL_RESET			(*(unsigned int volatile *)REG_ACTL_RESET)
#define     rACTL_RDSTB			(*(unsigned int volatile *)REG_ACTL_RDSTB)
#define     rACTL_RDST_LENGTH	(*(unsigned int volatile *)REG_ACTL_RDST_LENGTH)
#define     rACTL_RDSTC         (*(unsigned int volatile *)REG_ACTL_RDSTC)
#define     rACTL_RSR			(*(unsigned int volatile *)REG_ACTL_RSR)
#define     rACTL_PDSTB			(*(unsigned int volatile *)REG_ACTL_PDSTB)
#define     rACTL_PDST_LENGTH	(*(unsigned int volatile *)REG_ACTL_PDST_LENGTH)
#define     rACTL_PDSTC			(*(unsigned int volatile *)REG_ACTL_PDSTC)
#define     rACTL_PSR			(*(unsigned int volatile *)REG_ACTL_PSR)
#define     rACTL_IISCON		(*(unsigned int volatile *)REG_ACTL_IISCON)
#define     rACTL_ACCON			(*(unsigned int volatile *)REG_ACTL_ACCON)
#define     rACTL_ACOS0			(*(unsigned int volatile *)REG_ACTL_ACOS0)
#define     rACTL_ACOS1			(*(unsigned int volatile *)REG_ACTL_ACOS1)
#define     rACTL_ACOS2			(*(unsigned int volatile *)REG_ACTL_ACOS2)
#define     rACTL_ACIS0			(*(unsigned int volatile *)REG_ACTL_ACIS0)
#define     rACTL_ACIS1			(*(unsigned int volatile *)REG_ACTL_ACIS1)
#define     rACTL_ACIS2			(*(unsigned int volatile *)REG_ACTL_ACIS2)
#define     rACTL_COUNTER		(*(unsigned int volatile *)REG_ACTL_COUNTER)

/*
  JPEG Control Registers  
*/
//#define     REG_ACTL_CON		(JPEG_BA+0x00)  /* JPEG control register */




/*
  2-D Graphic Engine Control Registers  
*/
#define     REG_2D_GETG			(GE_BA+0x00)  /* Graphic Engine Trigger Control Register */
#define     REG_2D_GEXYSORG		(GE_BA+0x04)  /* Graphic Engine XY Mode Source Origin Starting Register */
#define     REG_2D_TileXY_VHSF	(GE_BA+0x08)  /* Graphic Engine Tile Width/Height or V/H Scale Factor N/M */
#define     REG_2D_GERRXY		(GE_BA+0x0C)  /* Graphic Engine Rotate Reference Point XY Address */
#define     REG_2D_GEINTS		(GE_BA+0x10)  /* Graphic Engine Interrupt Status Register */
#define     REG_2D_GEPLS		(GE_BA+0x14)  /* Graphic Engine Pattern Location Starting Address Register */
#define     REG_2D_GEBER		(GE_BA+0x18)  /* GE Bresenham Error Term Stepping Constant Register */
#define     REG_2D_GEBIR		(GE_BA+0x1C)  /* GE Bresenham Initial Error, Pixel Count Major M Register */
#define     REG_2D_GEC			(GE_BA+0x20)  /* Graphic Engine Control Register */
#define     REG_2D_GEBC			(GE_BA+0x24)  /* Graphic Engine Background Color Register */
#define     REG_2D_GEFC			(GE_BA+0x28)  /* Graphic Engine Foreground Color Register */
#define     REG_2D_GETC			(GE_BA+0x2C)  /* Graphic Engine Transparency Color Register */
#define     REG_2D_GETCM		(GE_BA+0x30)  /* Graphic Engine Transparency Color Mask Register */
#define     REG_2D_GEXYDORG		(GE_BA+0x34)  /* Graphic Engine XY Mode Display Origin Starting Register */
#define     REG_2D_GESDP		(GE_BA+0x38)  /* Graphic Engine Source/Destination Pitch Register */
#define     REG_2D_GESSXYL		(GE_BA+0x3C)  /* Graphic Engine Source Start XY/Linear Address Register */
#define     REG_2D_GEDSXYL		(GE_BA+0x40)  /* Graphic Engine Destination Start XY/Linear Register */
#define     REG_2D_GEDIXYL		(GE_BA+0x44)  /* Graphic Engine Dimension XY/Linear Register */
#define     REG_2D_GECBTL		(GE_BA+0x48)  /* Graphic Engine Clipping Boundary Top/Left Register */
#define     REG_2D_GECBBR		(GE_BA+0x4C)  /* Graphic Engine Clipping Boundary Bottom/Right Register */
#define     REG_2D_GEPTNA		(GE_BA+0x50)  /* Graphic Engine Pattern A Register */
#define     REG_2D_GEPTNB		(GE_BA+0x54)  /* Graphic Engine Pattern B Register */
#define     REG_2D_GEWPM		(GE_BA+0x58)  /* Graphic Engine Write Plane Mask Register */
#define     REG_2D_GEMC			(GE_BA+0x5C)  /* Graphic Engine Miscellaneous Control Register */

#define     rGETG			(*(unsigned int volatile *)REG_2D_GETG)
#define     rGEXYSORG		(*(unsigned int volatile *)REG_2D_GEXYSORG)
#define     rTileXY_VHSF	(*(unsigned int volatile *)REG_2D_TileXY_VHSF)
#define     rGERRXY			(*(unsigned int volatile *)REG_2D_GERRXY)
#define     rGEINTS			(*(unsigned int volatile *)REG_2D_GEINTS)
#define     rGEPLS			(*(unsigned int volatile *)REG_2D_GEPLS)
#define     rGEBER			(*(unsigned int volatile *)REG_2D_GEBER)
#define     rGEBIR			(*(unsigned int volatile *)REG_2D_GEBIR)
#define     rGEC			(*(unsigned int volatile *)REG_2D_GEC)
#define     rGEBC			(*(unsigned int volatile *)REG_2D_GEBC)
#define     rGEFC			(*(unsigned int volatile *)REG_2D_GEFC)
#define     rGETC			(*(unsigned int volatile *)REG_2D_GETC)
#define     rGETCM			(*(unsigned int volatile *)REG_2D_GETCM)
#define     rGEXYDORG		(*(unsigned int volatile *)REG_2D_GEXYDORG)
#define     rGESDP			(*(unsigned int volatile *)REG_2D_GESDP)
#define     rGESSXYL		(*(unsigned int volatile *)REG_2D_GESSXYL)
#define     rGEDSXYL		(*(unsigned int volatile *)REG_2D_GEDSXYL)
#define     rGEDIXYL		(*(unsigned int volatile *)REG_2D_GEDIXYL)
#define     rGECBTL			(*(unsigned int volatile *)REG_2D_GECBTL)
#define     rGECBBR			(*(unsigned int volatile *)REG_2D_GECBBR)
#define     rGEPTNA			(*(unsigned int volatile *)REG_2D_GEPTNA)
#define     rGEPTNB			(*(unsigned int volatile *)REG_2D_GEPTNB)
#define     rGEWPM			(*(unsigned int volatile *)REG_2D_GEWPM)
#define     rGEMC			(*(unsigned int volatile *)REG_2D_GEMC)


/*
  SD/SDIO Host Controller Registers  
*/
/* DMAC Control Registers*/
#define     REG_SD_FB0			(SDH_BA+0x000)  /* DMAC Control and Status Register */
#define     REG_SD_DMACCSR		(SDH_BA+0x400)  /* DMAC Control and Status Register */
#define     REG_SD_DMACSAR		(SDH_BA+0x408)  /* DMAC Transfer Starting Address Register */
#define     REG_SD_DMACBCR		(SDH_BA+0x40C)  /* DMAC Transfer Byte Count Register */
#define     REG_SD_DMACIER		(SDH_BA+0x410)  /* DMAC Interrupt Enable Register */
#define     REG_SD_DMACISR		(SDH_BA+0x414)  /* DMAC Interrupt Status Register */

#define     REG_SD_FMICSR		(SDH_BA+0x800)   /* Global Control and Status Register */
#define     REG_SD_FMIIER		(SDH_BA+0x804)   /* Global Interrupt Control Register */
#define     REG_SD_FMIISR		(SDH_BA+0x808)   /* Global Interrupt Status Register */

/* Secure Digit Registers */
#define     REG_SDCSR	    	(SDH_BA+0x820)   /* SD control and status register */
#define     REG_SDARG	    	(SDH_BA+0x824)   /* SD command argument register */
#define     REG_SDIER		    (SDH_BA+0x828)   /* SD interrupt enable register */
#define     REG_SDISR		    (SDH_BA+0x82C)   /* SD interrupt status register */
#define     REG_SDRSP0		    (SDH_BA+0x830)   /* SD receive response token register 0 */
#define     REG_SDRSP1		    (SDH_BA+0x834)   /* SD receive response token register 1 */
#define     REG_SDBLEN		    (SDH_BA+0x838)   /* SD block length register */
#define     REG_SDTMOUT	    	(SDH_BA+0x83C)   /* SD block length register */
 
/* DMAC Control Registers*/
#define     rSD_FB0		    	(*(unsigned int volatile *)REG_SD_FB0)
#define     rSD_DMACCSR	    	(*(unsigned int volatile *)REG_SD_DMACCSR)
#define     rSD_DMACSAR	    	(*(unsigned int volatile *)REG_SD_DMACSAR)
#define     rSD_DMACBCR	    	(*(unsigned int volatile *)REG_SD_DMACBCR)
#define     rSD_DMACIER	    	(*(unsigned int volatile *)REG_SD_DMACIER)
#define     rSD_DMACISR	    	(*(unsigned int volatile *)REG_SD_DMACISR)

#define     rSD_FMICSR	    	(*(unsigned int volatile *)REG_SD_FMICSR)
#define     rSD_FMIIER	    	(*(unsigned int volatile *)REG_SD_FMIIER)
#define     rSD_FMIISR	    	(*(unsigned int volatile *)REG_SD_FMIISR)

/* Secure Digit Registers */
#define     rSDCSR		    	(*(unsigned int volatile *)REG_SDCSR)
#define     rSDARG		    	(*(unsigned int volatile *)REG_SDARG)
#define     rSDIER		    	(*(unsigned int volatile *)REG_SDIER)
#define     rSDISR		    	(*(unsigned int volatile *)REG_SDISR)
#define     rSDRSP0		    	(*(unsigned int volatile *)REG_SDRSP0)
#define     rSDRSP1		    	(*(unsigned int volatile *)REG_SDRSP1)
#define     rSDBLEN		    	(*(unsigned int volatile *)REG_SDBLEN)
#define     rSDTMOUT	    	(*(unsigned int volatile *)REG_SDTMOUT)


/*
  Flash Memory Interface Control Registers  
*/
/* DMAC Control Registers*/
#define     REG_NAND_FB0		(FMI_BA+0x000)  /* DMAC Control and Status Register */
#define     REG_NAND_DMACCSR	(FMI_BA+0x400)  /* DMAC Control and Status Register */
#define     REG_NAND_DMACSAR	(FMI_BA+0x408)  /* DMAC Transfer Starting Address Register */
#define     REG_NAND_DMACBCR	(FMI_BA+0x40C)  /* DMAC Transfer Byte Count Register */
#define     REG_NAND_DMACIER	(FMI_BA+0x410)  /* DMAC Interrupt Enable Register */
#define     REG_NAND_DMACISR	(FMI_BA+0x414)  /* DMAC Interrupt Status Register */

#define     REG_NAND_FMICSR		(FMI_BA+0x800)   /* Global Control and Status Register */
#define     REG_NAND_FMIIER	    (FMI_BA+0x804)   /* Global Interrupt Control Register */
#define     REG_NAND_FMIISR	    (FMI_BA+0x808)   /* Global Interrupt Status Register */

/* eMMC Registers */
#define     REG_NAND_SDCSR	    (FMI_BA+0x820)   /* SD control and status register */
#define     REG_NAND_SDARG	    (FMI_BA+0x824)   /* SD command argument register */
#define     REG_NAND_SDIER		(FMI_BA+0x828)   /* SD interrupt enable register */
#define     REG_NAND_SDISR		(FMI_BA+0x82C)   /* SD interrupt status register */
#define     REG_NAND_SDRSP0		(FMI_BA+0x830)   /* SD receive response token register 0 */
#define     REG_NAND_SDRSP1		(FMI_BA+0x834)   /* SD receive response token register 1 */
#define     REG_NAND_SDBLEN		(FMI_BA+0x838)   /* SD block length register */
#define     REG_NAND_SDTMOUT	(FMI_BA+0x83C)   /* SD block length register */
 
/* NAND-type Flash Registers */
#define     REG_SMCSR	        (FMI_BA+0x8A0)   /* NAND Flash Control and Status Register */
#define     REG_SMTCR	        (FMI_BA+0x8A4)   /* NAND Flash Timing Control Register */
#define     REG_SMIER	        (FMI_BA+0x8A8)   /* NAND Flash Interrupt Control Register */
#define     REG_SMISR	        (FMI_BA+0x8AC)   /* NAND Flash Interrupt Status Register */
#define     REG_SMCMD	        (FMI_BA+0x8B0)   /* NAND Flash Command Port Register */
#define     REG_SMADDR	        (FMI_BA+0x8B4)   /* NAND Flash Address Port Register */
#define     REG_SMDATA		    (FMI_BA+0x8B8)   /* NAND Flash Data Port Register */
#define     REG_SMREACTL        (FMI_BA+0x8BC)   /* NAND Flash Smart-Media Redundant Area Control Register */
#define     REG_NFECR           (FMI_BA+0x8C0)   /* NAND Flash Extend Control Regsiter */
#define     REG_SMECC_ST0	    (FMI_BA+0x8D0)	 /* Smart-Media ECC Error Status 0 */
#define     REG_SMECC_ST1	    (FMI_BA+0x8D4)	 /* Smart-Media ECC Error Status 1 */
#define     REG_SMECC_ST2	    (FMI_BA+0x8D8)	 /* Smart-Media ECC Error Status 2 */
#define     REG_SMECC_ST3	    (FMI_BA+0x8DC)	 /* Smart-Media ECC Error Status 3 */
#define     REG_SMPROT_ADDR0    (FMI_BA+0x8E0)  /* Smart-Media Protect region end address 0 */
#define     REG_SMPROT_ADDR1    (FMI_BA+0x8E4)  /* Smart-Media Protect region end address 1 */

/* NAND-type Flash BCH Error Address Registers */
#define     REG_BCH_ECC_ADDR0	(FMI_BA+0x900)  /* BCH error byte address 0 */
#define     REG_BCH_ECC_ADDR1	(FMI_BA+0x904)  /* BCH error byte address 1 */
#define     REG_BCH_ECC_ADDR2	(FMI_BA+0x908)  /* BCH error byte address 2 */
#define     REG_BCH_ECC_ADDR3	(FMI_BA+0x90C)  /* BCH error byte address 3 */
#define     REG_BCH_ECC_ADDR4	(FMI_BA+0x910)  /* BCH error byte address 4 */
#define     REG_BCH_ECC_ADDR5	(FMI_BA+0x914)  /* BCH error byte address 5 */
#define     REG_BCH_ECC_ADDR6	(FMI_BA+0x918)  /* BCH error byte address 6 */
#define     REG_BCH_ECC_ADDR7	(FMI_BA+0x91C)  /* BCH error byte address 7 */
#define     REG_BCH_ECC_ADDR8	(FMI_BA+0x920)  /* BCH error byte address 8 */
#define     REG_BCH_ECC_ADDR9	(FMI_BA+0x924)  /* BCH error byte address 9 */
#define     REG_BCH_ECC_ADDR10	(FMI_BA+0x928)  /* BCH error byte address 10 */
#define     REG_BCH_ECC_ADDR11	(FMI_BA+0x92C)  /* BCH error byte address 11 */

/* NAND-type Flash BCH Error Data Registers */
#define     REG_BCH_ECC_DATA0	(FMI_BA+0x960)  /* BCH error byte data 0 */
#define     REG_BCH_ECC_DATA1	(FMI_BA+0x964)  /* BCH error byte data 1 */
#define     REG_BCH_ECC_DATA2	(FMI_BA+0x968)  /* BCH error byte data 2 */
#define     REG_BCH_ECC_DATA3	(FMI_BA+0x96C)  /* BCH error byte data 3 */
#define     REG_BCH_ECC_DATA4	(FMI_BA+0x970)  /* BCH error byte data 4 */
#define     REG_BCH_ECC_DATA5	(FMI_BA+0x974)  /* BCH error byte data 5 */

/* NAND-type Flash Redundant Area Registers */
#define     REG_SMRA0			(FMI_BA+0xA00)  /* Smart-Media Redundant Area Register */
#define     REG_SMRA1			(FMI_BA+0xA04)  /* Smart-Media Redundant Area Register */

/* DMAC Control Registers*/
#define     rNAND_FB0			(*(unsigned int volatile *)REG_NAND_FB0)
#define     rNAND_DMACCSR		(*(unsigned int volatile *)REG_NAND_DMACCSR)
#define     rNAND_DMACSAR		(*(unsigned int volatile *)REG_NAND_DMACSAR)
#define     rNAND_DMACBCR		(*(unsigned int volatile *)REG_NAND_DMACBCR)
#define     rNAND_DMACIER		(*(unsigned int volatile *)REG_NAND_DMACIER)
#define     rNAND_DMACISR		(*(unsigned int volatile *)REG_NAND_DMACISR)

#define     rNAND_FMICSR		(*(unsigned int volatile *)REG_NAND_FMICSR)
#define     rNAND_FMIIER		(*(unsigned int volatile *)REG_NAND_FMIIER)
#define     rNAND_FMIISR		(*(unsigned int volatile *)REG_NAND_FMIISR)

/* NAND-type Flash Registers */
#define     rSMCSR			    (*(unsigned int volatile *)REG_SMCSR)
#define     rSMTCR		    	(*(unsigned int volatile *)REG_SMTCR)
#define     rSMIER		    	(*(unsigned int volatile *)REG_SMIER)
#define     rSMISR		    	(*(unsigned int volatile *)REG_SMISR)
#define     rSMCMD		    	(*(unsigned int volatile *)REG_SMCMD)
#define     rSMADDR		    	(*(unsigned int volatile *)REG_SMADDR)
#define     rSMDATA		    	(*(unsigned int volatile *)REG_SMDATA)
#define     rSMREACTL	    	(*(unsigned int volatile *)REG_SMREACTL)
#define     rSMECC_ST0		    (*(unsigned int volatile *)REG_SMECC_ST0)
#define     rSMECC_ST1		    (*(unsigned int volatile *)REG_SMECC_ST1)
#define     rSMECC_ST2	    	(*(unsigned int volatile *)REG_SMECC_ST2)
#define     rSMECC_ST3	    	(*(unsigned int volatile *)REG_SMECC_ST3)
#define     rSMPROT_ADDR0	    (*(unsigned int volatile *)REG_SMPROT_ADDR0)
#define     rSMPROT_ADDR1	    (*(unsigned int volatile *)REG_SMPROT_ADDR1)

/* NAND-type Flash BCH Error Address Registers */
#define     rBCH_ECC_ADDR0	    (*(unsigned int volatile *)REG_BCH_ECC_ADDR0)
#define     rBCH_ECC_ADDR1	    (*(unsigned int volatile *)REG_BCH_ECC_ADDR1)
#define     rBCH_ECC_ADDR2  	(*(unsigned int volatile *)REG_BCH_ECC_ADDR2)
#define     rBCH_ECC_ADDR3	    (*(unsigned int volatile *)REG_BCH_ECC_ADDR3)
#define     rBCH_ECC_ADDR4	    (*(unsigned int volatile *)REG_BCH_ECC_ADDR4)
#define     rBCH_ECC_ADDR5  	(*(unsigned int volatile *)REG_BCH_ECC_ADDR5)
#define     rBCH_ECC_ADDR6	    (*(unsigned int volatile *)REG_BCH_ECC_ADDR6)
#define     rBCH_ECC_ADDR7	    (*(unsigned int volatile *)REG_BCH_ECC_ADDR7)
#define     rBCH_ECC_ADDR8	    (*(unsigned int volatile *)REG_BCH_ECC_ADDR8)
#define     rBCH_ECC_ADDR9	    (*(unsigned int volatile *)REG_BCH_ECC_ADDR9)
#define     rBCH_ECC_ADDR10	    (*(unsigned int volatile *)REG_BCH_ECC_ADDR10)
#define     rBCH_ECC_ADDR11	    (*(unsigned int volatile *)REG_BCH_ECC_ADDR11)

/* NAND-type Flash BCH Error Data Registers */
#define     rBCH_ECC_DATA0	    (*(unsigned int volatile *)REG_BCH_ECC_DATA0)
#define     rBCH_ECC_DATA1  	(*(unsigned int volatile *)REG_BCH_ECC_DATA1)
#define     rBCH_ECC_DATA2	    (*(unsigned int volatile *)REG_BCH_ECC_DATA2)
#define     rBCH_ECC_DATA3	    (*(unsigned int volatile *)REG_BCH_ECC_DATA3)
#define     rBCH_ECC_DATA4  	(*(unsigned int volatile *)REG_BCH_ECC_DATA4)
#define     rBCH_ECC_DATA5	    (*(unsigned int volatile *)REG_BCH_ECC_DATA5)

/* NAND-type Flash Redundant Area Registers */
#define     rSMRA0			    (*(unsigned int volatile *)REG_SMRA0)


/*
  UART0 Control Registers
*/
#define     REG_UART0_RBR		(UART0_BA+0x00)  /* Receive Buffer Register */
#define     REG_UART0_THR		(UART0_BA+0x00)  /* Transmit Holding Register */
#define     REG_UART0_IER		(UART0_BA+0x04)  /* Interrupt Enable Register */
#define     REG_UART0_FCR		(UART0_BA+0x08)  /* FIFO Control Register */
#define     REG_UART0_LCR		(UART0_BA+0x0C)  /* Line Control Register */
#define     REG_UART0_MCR		(UART0_BA+0x10)  /* Modem Control Register */
#define     REG_UART0_MSR		(UART0_BA+0x14)  /* MODEM Status Register */
#define     REG_UART0_FSR		(UART0_BA+0x18)  /* FIFO Status Register */
#define     REG_UART0_ISR		(UART0_BA+0x1C)  /* Interrupt Status Register */
#define     REG_UART0_TOR		(UART0_BA+0x20)  /* Time Out Register */
#define     REG_UART0_BAUD		(UART0_BA+0x24)  /* Baud Rate Divisor Register */
#define     REG_UART0_IRCR		(UART0_BA+0x28)  /* IrDA Control Register */
#define     REG_UART0_ALT_CSR	(UART0_BA+0x2C)  /* Alternate Control/Status Register */
#define     REG_UART0_FUN_SEL	(UART0_BA+0x30)  /* Function Select Register */
#define     REG_UART0_LIN_CTL	(UART0_BA+0x34)  /* LIN Control Register */
#define     REG_UART0_LIN_SR	(UART0_BA+0x38)  /* LIN Status Register */
#define     REG_UART0_DEBUG		(UART0_BA+0x3C)  /* Debug Register */
#define     REG_UART0_SC_CTL	(UART0_BA+0x40)  /* SC Control Register */
#define     REG_UART0_SC_FSR	(UART0_BA+0x44)  /* SC Flag Status Register */


#define     rUART0_RBR		(*(unsigned int volatile *)REG_UART0_RBR)
#define     rUART0_THR		(*(unsigned int volatile *)REG_UART0_THR)
#define     rUART0_IER		(*(unsigned int volatile *)REG_UART0_IER)
#define     rUART0_FCR		(*(unsigned int volatile *)REG_UART0_FCR)
#define     rUART0_LCR		(*(unsigned int volatile *)REG_UART0_LCR)
#define     rUART0_MCR		(*(unsigned int volatile *)REG_UART0_MCR)
#define     rUART0_MSR		(*(unsigned int volatile *)REG_UART0_MSR)
#define     rUART0_FSR		(*(unsigned int volatile *)REG_UART0_FSR)
#define     rUART0_ISR		(*(unsigned int volatile *)REG_UART0_ISR)
#define     rUART0_TOR		(*(unsigned int volatile *)REG_UART0_TOR)
#define     rUART0_BAUD		(*(unsigned int volatile *)REG_UART0_BAUD)
#define     rUART0_IRCR		(*(unsigned int volatile *)REG_UART0_IRCR)
#define     rUART0_ALT_CSR	(*(unsigned int volatile *)REG_UART0_ALT_CSR)
#define     rUART0_FUN_SEL	(*(unsigned int volatile *)REG_UART0_FUN_SEL)
#define     rUART0_LIN_CTL	(*(unsigned int volatile *)REG_UART0_LIN_CTL)
#define     rUART0_LIN_SR	(*(unsigned int volatile *)REG_UART0_LIN_SR)
#define     rUART0_DEBUG	(*(unsigned int volatile *)REG_UART0_DEBUG)
#define     rUART0_SC_CTL	(*(unsigned int volatile *)REG_UART0_SC_CTL)
#define     rUART0_SC_FSR	(*(unsigned int volatile *)REG_UART0_SC_FSR)

/*
  UART1 Control Registers
*/
#define     REG_UART1_RBR		(UART1_BA+0x00)  /* Receive Buffer Register */
#define     REG_UART1_THR		(UART1_BA+0x00)  /* Transmit Holding Register */
#define     REG_UART1_IER		(UART1_BA+0x04)  /* Interrupt Enable Register */
#define     REG_UART1_FCR		(UART1_BA+0x08)  /* FIFO Control Register */
#define     REG_UART1_LCR		(UART1_BA+0x0C)  /* Line Control Register */
#define     REG_UART1_MCR		(UART1_BA+0x10)  /* Modem Control Register */
#define     REG_UART1_MSR		(UART1_BA+0x14)  /* MODEM Status Register */
#define     REG_UART1_FSR		(UART1_BA+0x18)  /* FIFO Status Register */
#define     REG_UART1_ISR		(UART1_BA+0x1C)  /* Interrupt Status Register */
#define     REG_UART1_TOR		(UART1_BA+0x20)  /* Time Out Register */
#define     REG_UART1_BAUD		(UART1_BA+0x24)  /* Baud Rate Divisor Register */
#define     REG_UART1_IRCR		(UART1_BA+0x28)  /* IrDA Control Register */
#define     REG_UART1_ALT_CSR	(UART1_BA+0x2C)  /* Alternate Control/Status Register */
#define     REG_UART1_FUN_SEL	(UART1_BA+0x30)  /* Function Select Register */
#define     REG_UART1_LIN_CTL	(UART1_BA+0x34)  /* LIN Control Register */
#define     REG_UART1_LIN_SR	(UART1_BA+0x38)  /* LIN Status Register */
#define     REG_UART1_DEBUG		(UART1_BA+0x3C)  /* Debug Register */
#define     REG_UART1_SC_CTL	(UART1_BA+0x40)  /* SC Control Register */
#define     REG_UART1_SC_FSR	(UART1_BA+0x44)  /* SC Flag Status Register */


#define     rUART1_RBR		(*(unsigned int volatile *)REG_UART1_RBR)
#define     rUART1_THR		(*(unsigned int volatile *)REG_UART1_THR)
#define     rUART1_IER		(*(unsigned int volatile *)REG_UART1_IER)
#define     rUART1_FCR		(*(unsigned int volatile *)REG_UART1_FCR)
#define     rUART1_LCR		(*(unsigned int volatile *)REG_UART1_LCR)
#define     rUART1_MCR		(*(unsigned int volatile *)REG_UART1_MCR)
#define     rUART1_MSR		(*(unsigned int volatile *)REG_UART1_MSR)
#define     rUART1_FSR		(*(unsigned int volatile *)REG_UART1_FSR)
#define     rUART1_ISR		(*(unsigned int volatile *)REG_UART1_ISR)
#define     rUART1_TOR		(*(unsigned int volatile *)REG_UART1_TOR)
#define     rUART1_BAUD		(*(unsigned int volatile *)REG_UART1_BAUD)
#define     rUART1_IRCR		(*(unsigned int volatile *)REG_UART1_IRCR)
#define     rUART1_ALT_CSR	(*(unsigned int volatile *)REG_UART1_ALT_CSR)
#define     rUART1_FUN_SEL	(*(unsigned int volatile *)REG_UART1_FUN_SEL)
#define     rUART1_LIN_CTL	(*(unsigned int volatile *)REG_UART1_LIN_CTL)
#define     rUART1_LIN_SR	(*(unsigned int volatile *)REG_UART1_LIN_SR)
#define     rUART1_DEBUG	(*(unsigned int volatile *)REG_UART1_DEBUG)
#define     rUART1_SC_CTL	(*(unsigned int volatile *)REG_UART1_SC_CTL)
#define     rUART1_SC_FSR	(*(unsigned int volatile *)REG_UART1_SC_FSR)


/*
  UART2 Control Registers
*/
#define     REG_UART2_RBR		(UART2_BA+0x00)  /* Receive Buffer Register */
#define     REG_UART2_THR		(UART2_BA+0x00)  /* Transmit Holding Register */
#define     REG_UART2_IER		(UART2_BA+0x04)  /* Interrupt Enable Register */
#define     REG_UART2_FCR		(UART2_BA+0x08)  /* FIFO Control Register */
#define     REG_UART2_LCR		(UART2_BA+0x0C)  /* Line Control Register */
#define     REG_UART2_MCR		(UART2_BA+0x10)  /* Modem Control Register */
#define     REG_UART2_MSR		(UART2_BA+0x14)  /* MODEM Status Register */
#define     REG_UART2_FSR		(UART2_BA+0x18)  /* FIFO Status Register */
#define     REG_UART2_ISR		(UART2_BA+0x1C)  /* Interrupt Status Register */
#define     REG_UART2_TOR		(UART2_BA+0x20)  /* Time Out Register */
#define     REG_UART2_BAUD		(UART2_BA+0x24)  /* Baud Rate Divisor Register */
#define     REG_UART2_IRCR		(UART2_BA+0x28)  /* IrDA Control Register */
#define     REG_UART2_ALT_CSR	(UART2_BA+0x2C)  /* Alternate Control/Status Register */
#define     REG_UART2_FUN_SEL	(UART2_BA+0x30)  /* Function Select Register */
#define     REG_UART2_LIN_CTL	(UART2_BA+0x34)  /* LIN Control Register */
#define     REG_UART2_LIN_SR	(UART2_BA+0x38)  /* LIN Status Register */
#define     REG_UART2_DEBUG		(UART2_BA+0x3C)  /* Debug Register */
#define     REG_UART2_SC_CTL	(UART2_BA+0x40)  /* SC Control Register */
#define     REG_UART2_SC_FSR	(UART2_BA+0x44)  /* SC Flag Status Register */


#define     rUART2_RBR		(*(unsigned int volatile *)REG_UART2_RBR)
#define     rUART2_THR		(*(unsigned int volatile *)REG_UART2_THR)
#define     rUART2_IER		(*(unsigned int volatile *)REG_UART2_IER)
#define     rUART2_FCR		(*(unsigned int volatile *)REG_UART2_FCR)
#define     rUART2_LCR		(*(unsigned int volatile *)REG_UART2_LCR)
#define     rUART2_MCR		(*(unsigned int volatile *)REG_UART2_MCR)
#define     rUART2_MSR		(*(unsigned int volatile *)REG_UART2_MSR)
#define     rUART2_FSR		(*(unsigned int volatile *)REG_UART2_FSR)
#define     rUART2_ISR		(*(unsigned int volatile *)REG_UART2_ISR)
#define     rUART2_TOR		(*(unsigned int volatile *)REG_UART2_TOR)
#define     rUART2_BAUD		(*(unsigned int volatile *)REG_UART2_BAUD)
#define     rUART2_IRCR		(*(unsigned int volatile *)REG_UART2_IRCR)
#define     rUART2_ALT_CSR	(*(unsigned int volatile *)REG_UART2_ALT_CSR)
#define     rUART2_FUN_SEL	(*(unsigned int volatile *)REG_UART2_FUN_SEL)
#define     rUART2_LIN_CTL	(*(unsigned int volatile *)REG_UART2_LIN_CTL)
#define     rUART2_LIN_SR	(*(unsigned int volatile *)REG_UART2_LIN_SR)
#define     rUART2_DEBUG	(*(unsigned int volatile *)REG_UART2_DEBUG)
#define     rUART2_SC_CTL	(*(unsigned int volatile *)REG_UART2_SC_CTL)
#define     rUART2_SC_FSR	(*(unsigned int volatile *)REG_UART2_SC_FSR)

/*
  UART3 Control Registers
*/
#define     REG_UART3_RBR		(UART3_BA+0x00)  /* Receive Buffer Register */
#define     REG_UART3_THR		(UART3_BA+0x00)  /* Transmit Holding Register */
#define     REG_UART3_IER		(UART3_BA+0x04)  /* Interrupt Enable Register */
#define     REG_UART3_FCR		(UART3_BA+0x08)  /* FIFO Control Register */
#define     REG_UART3_LCR		(UART3_BA+0x0C)  /* Line Control Register */
#define     REG_UART3_MCR		(UART3_BA+0x10)  /* Modem Control Register */
#define     REG_UART3_MSR		(UART3_BA+0x14)  /* MODEM Status Register */
#define     REG_UART3_FSR		(UART3_BA+0x18)  /* FIFO Status Register */
#define     REG_UART3_ISR		(UART3_BA+0x1C)  /* Interrupt Status Register */
#define     REG_UART3_TOR		(UART3_BA+0x20)  /* Time Out Register */
#define     REG_UART3_BAUD		(UART3_BA+0x24)  /* Baud Rate Divisor Register */
#define     REG_UART3_IRCR		(UART3_BA+0x28)  /* IrDA Control Register */
#define     REG_UART3_ALT_CSR	(UART3_BA+0x2C)  /* Alternate Control/Status Register */
#define     REG_UART3_FUN_SEL	(UART3_BA+0x30)  /* Function Select Register */
#define     REG_UART3_LIN_CTL	(UART3_BA+0x34)  /* LIN Control Register */
#define     REG_UART3_LIN_SR	(UART3_BA+0x38)  /* LIN Status Register */
#define     REG_UART3_DEBUG		(UART3_BA+0x3C)  /* Debug Register */
#define     REG_UART3_SC_CTL	(UART3_BA+0x40)  /* SC Control Register */
#define     REG_UART3_SC_FSR	(UART3_BA+0x44)  /* SC Flag Status Register */


#define     rUART3_RBR		(*(unsigned int volatile *)REG_UART3_RBR)
#define     rUART3_THR		(*(unsigned int volatile *)REG_UART3_THR)
#define     rUART3_IER		(*(unsigned int volatile *)REG_UART3_IER)
#define     rUART3_FCR		(*(unsigned int volatile *)REG_UART3_FCR)
#define     rUART3_LCR		(*(unsigned int volatile *)REG_UART3_LCR)
#define     rUART3_MCR		(*(unsigned int volatile *)REG_UART3_MCR)
#define     rUART3_MSR		(*(unsigned int volatile *)REG_UART3_MSR)
#define     rUART3_FSR		(*(unsigned int volatile *)REG_UART3_FSR)
#define     rUART3_ISR		(*(unsigned int volatile *)REG_UART3_ISR)
#define     rUART3_TOR		(*(unsigned int volatile *)REG_UART3_TOR)
#define     rUART3_BAUD		(*(unsigned int volatile *)REG_UART3_BAUD)
#define     rUART3_IRCR		(*(unsigned int volatile *)REG_UART3_IRCR)
#define     rUART3_ALT_CSR	(*(unsigned int volatile *)REG_UART3_ALT_CSR)
#define     rUART3_FUN_SEL	(*(unsigned int volatile *)REG_UART3_FUN_SEL)
#define     rUART3_LIN_CTL	(*(unsigned int volatile *)REG_UART3_LIN_CTL)
#define     rUART3_LIN_SR	(*(unsigned int volatile *)REG_UART3_LIN_SR)
#define     rUART3_DEBUG	(*(unsigned int volatile *)REG_UART3_DEBUG)
#define     rUART3_SC_CTL	(*(unsigned int volatile *)REG_UART3_SC_CTL)
#define     rUART3_SC_FSR	(*(unsigned int volatile *)REG_UART3_SC_FSR)


/*
  UART4 Control Registers
*/
#define     REG_UART4_RBR		(UART4_BA+0x00)  /* Receive Buffer Register */
#define     REG_UART4_THR		(UART4_BA+0x00)  /* Transmit Holding Register */
#define     REG_UART4_IER		(UART4_BA+0x04)  /* Interrupt Enable Register */
#define     REG_UART4_FCR		(UART4_BA+0x08)  /* FIFO Control Register */
#define     REG_UART4_LCR		(UART4_BA+0x0C)  /* Line Control Register */
#define     REG_UART4_MCR		(UART4_BA+0x10)  /* Modem Control Register */
#define     REG_UART4_MSR		(UART4_BA+0x14)  /* MODEM Status Register */
#define     REG_UART4_FSR		(UART4_BA+0x18)  /* FIFO Status Register */
#define     REG_UART4_ISR		(UART4_BA+0x1C)  /* Interrupt Status Register */
#define     REG_UART4_TOR		(UART4_BA+0x20)  /* Time Out Register */
#define     REG_UART4_BAUD		(UART4_BA+0x24)  /* Baud Rate Divisor Register */
#define     REG_UART4_IRCR		(UART4_BA+0x28)  /* IrDA Control Register */
#define     REG_UART4_ALT_CSR	(UART4_BA+0x2C)  /* Alternate Control/Status Register */
#define     REG_UART4_FUN_SEL	(UART4_BA+0x30)  /* Function Select Register */
#define     REG_UART4_LIN_CTL	(UART4_BA+0x34)  /* LIN Control Register */
#define     REG_UART4_LIN_SR	(UART4_BA+0x38)  /* LIN Status Register */
#define     REG_UART4_DEBUG		(UART4_BA+0x3C)  /* Debug Register */
#define     REG_UART4_SC_CTL	(UART4_BA+0x40)  /* SC Control Register */
#define     REG_UART4_SC_FSR	(UART4_BA+0x44)  /* SC Flag Status Register */


#define     rUART4_RBR		(*(unsigned int volatile *)REG_UART4_RBR)
#define     rUART4_THR		(*(unsigned int volatile *)REG_UART4_THR)
#define     rUART4_IER		(*(unsigned int volatile *)REG_UART4_IER)
#define     rUART4_FCR		(*(unsigned int volatile *)REG_UART4_FCR)
#define     rUART4_LCR		(*(unsigned int volatile *)REG_UART4_LCR)
#define     rUART4_MCR		(*(unsigned int volatile *)REG_UART4_MCR)
#define     rUART4_MSR		(*(unsigned int volatile *)REG_UART4_MSR)
#define     rUART4_FSR		(*(unsigned int volatile *)REG_UART4_FSR)
#define     rUART4_ISR		(*(unsigned int volatile *)REG_UART4_ISR)
#define     rUART4_TOR		(*(unsigned int volatile *)REG_UART4_TOR)
#define     rUART4_BAUD		(*(unsigned int volatile *)REG_UART4_BAUD)
#define     rUART4_IRCR		(*(unsigned int volatile *)REG_UART4_IRCR)
#define     rUART4_ALT_CSR	(*(unsigned int volatile *)REG_UART4_ALT_CSR)
#define     rUART4_FUN_SEL	(*(unsigned int volatile *)REG_UART4_FUN_SEL)
#define     rUART4_LIN_CTL	(*(unsigned int volatile *)REG_UART4_LIN_CTL)
#define     rUART4_LIN_SR	(*(unsigned int volatile *)REG_UART4_LIN_SR)
#define     rUART4_DEBUG	(*(unsigned int volatile *)REG_UART4_DEBUG)
#define     rUART4_SC_CTL	(*(unsigned int volatile *)REG_UART4_SC_CTL)
#define     rUART4_SC_FSR	(*(unsigned int volatile *)REG_UART4_SC_FSR)

/*
  UART5 Control Registers
*/
#define     REG_UART5_RBR		(UART5_BA+0x00)  /* Receive Buffer Register */
#define     REG_UART5_THR		(UART5_BA+0x00)  /* Transmit Holding Register */
#define     REG_UART5_IER		(UART5_BA+0x04)  /* Interrupt Enable Register */
#define     REG_UART5_FCR		(UART5_BA+0x08)  /* FIFO Control Register */
#define     REG_UART5_LCR		(UART5_BA+0x0C)  /* Line Control Register */
#define     REG_UART5_MCR		(UART5_BA+0x10)  /* Modem Control Register */
#define     REG_UART5_MSR		(UART5_BA+0x14)  /* MODEM Status Register */
#define     REG_UART5_FSR		(UART5_BA+0x18)  /* FIFO Status Register */
#define     REG_UART5_ISR		(UART5_BA+0x1C)  /* Interrupt Status Register */
#define     REG_UART5_TOR		(UART5_BA+0x20)  /* Time Out Register */
#define     REG_UART5_BAUD		(UART5_BA+0x24)  /* Baud Rate Divisor Register */
#define     REG_UART5_IRCR		(UART5_BA+0x28)  /* IrDA Control Register */
#define     REG_UART5_ALT_CSR	(UART5_BA+0x2C)  /* Alternate Control/Status Register */
#define     REG_UART5_FUN_SEL	(UART5_BA+0x30)  /* Function Select Register */
#define     REG_UART5_LIN_CTL	(UART5_BA+0x34)  /* LIN Control Register */
#define     REG_UART5_LIN_SR	(UART5_BA+0x38)  /* LIN Status Register */
#define     REG_UART5_DEBUG		(UART5_BA+0x3C)  /* Debug Register */
#define     REG_UART5_SC_CTL	(UART5_BA+0x40)  /* SC Control Register */
#define     REG_UART5_SC_FSR	(UART5_BA+0x44)  /* SC Flag Status Register */


#define     rUART5_RBR		(*(unsigned int volatile *)REG_UART5_RBR)
#define     rUART5_THR		(*(unsigned int volatile *)REG_UART5_THR)
#define     rUART5_IER		(*(unsigned int volatile *)REG_UART5_IER)
#define     rUART5_FCR		(*(unsigned int volatile *)REG_UART5_FCR)
#define     rUART5_LCR		(*(unsigned int volatile *)REG_UART5_LCR)
#define     rUART5_MCR		(*(unsigned int volatile *)REG_UART5_MCR)
#define     rUART5_MSR		(*(unsigned int volatile *)REG_UART5_MSR)
#define     rUART5_FSR		(*(unsigned int volatile *)REG_UART5_FSR)
#define     rUART5_ISR		(*(unsigned int volatile *)REG_UART5_ISR)
#define     rUART5_TOR		(*(unsigned int volatile *)REG_UART5_TOR)
#define     rUART5_BAUD		(*(unsigned int volatile *)REG_UART5_BAUD)
#define     rUART5_IRCR		(*(unsigned int volatile *)REG_UART5_IRCR)
#define     rUART5_ALT_CSR	(*(unsigned int volatile *)REG_UART5_ALT_CSR)
#define     rUART5_FUN_SEL	(*(unsigned int volatile *)REG_UART5_FUN_SEL)
#define     rUART5_LIN_CTL	(*(unsigned int volatile *)REG_UART5_LIN_CTL)
#define     rUART5_LIN_SR	(*(unsigned int volatile *)REG_UART5_LIN_SR)
#define     rUART5_DEBUG	(*(unsigned int volatile *)REG_UART5_DEBUG)
#define     rUART5_SC_CTL	(*(unsigned int volatile *)REG_UART5_SC_CTL)
#define     rUART5_SC_FSR	(*(unsigned int volatile *)REG_UART5_SC_FSR)

/*
  UART6 Control Registers
*/
#define     REG_UART6_RBR		(UART6_BA+0x00)  /* Receive Buffer Register */
#define     REG_UART6_THR		(UART6_BA+0x00)  /* Transmit Holding Register */
#define     REG_UART6_IER		(UART6_BA+0x04)  /* Interrupt Enable Register */
#define     REG_UART6_FCR		(UART6_BA+0x08)  /* FIFO Control Register */
#define     REG_UART6_LCR		(UART6_BA+0x0C)  /* Line Control Register */
#define     REG_UART6_MCR		(UART6_BA+0x10)  /* Modem Control Register */
#define     REG_UART6_MSR		(UART6_BA+0x14)  /* MODEM Status Register */
#define     REG_UART6_FSR		(UART6_BA+0x18)  /* FIFO Status Register */
#define     REG_UART6_ISR		(UART6_BA+0x1C)  /* Interrupt Status Register */
#define     REG_UART6_TOR		(UART6_BA+0x20)  /* Time Out Register */
#define     REG_UART6_BAUD		(UART6_BA+0x24)  /* Baud Rate Divisor Register */
#define     REG_UART6_IRCR		(UART6_BA+0x28)  /* IrDA Control Register */
#define     REG_UART6_ALT_CSR	(UART6_BA+0x2C)  /* Alternate Control/Status Register */
#define     REG_UART6_FUN_SEL	(UART6_BA+0x30)  /* Function Select Register */
#define     REG_UART6_LIN_CTL	(UART6_BA+0x34)  /* LIN Control Register */
#define     REG_UART6_LIN_SR	(UART6_BA+0x38)  /* LIN Status Register */
#define     REG_UART6_DEBUG		(UART6_BA+0x3C)  /* Debug Register */
#define     REG_UART6_SC_CTL	(UART6_BA+0x40)  /* SC Control Register */
#define     REG_UART6_SC_FSR	(UART6_BA+0x44)  /* SC Flag Status Register */


#define     rUART6_RBR		(*(unsigned int volatile *)REG_UART6_RBR)
#define     rUART6_THR		(*(unsigned int volatile *)REG_UART6_THR)
#define     rUART6_IER		(*(unsigned int volatile *)REG_UART6_IER)
#define     rUART6_FCR		(*(unsigned int volatile *)REG_UART6_FCR)
#define     rUART6_LCR		(*(unsigned int volatile *)REG_UART6_LCR)
#define     rUART6_MCR		(*(unsigned int volatile *)REG_UART6_MCR)
#define     rUART6_MSR		(*(unsigned int volatile *)REG_UART6_MSR)
#define     rUART6_FSR		(*(unsigned int volatile *)REG_UART6_FSR)
#define     rUART6_ISR		(*(unsigned int volatile *)REG_UART6_ISR)
#define     rUART6_TOR		(*(unsigned int volatile *)REG_UART6_TOR)
#define     rUART6_BAUD		(*(unsigned int volatile *)REG_UART6_BAUD)
#define     rUART6_IRCR		(*(unsigned int volatile *)REG_UART6_IRCR)
#define     rUART6_ALT_CSR	(*(unsigned int volatile *)REG_UART6_ALT_CSR)
#define     rUART6_FUN_SEL	(*(unsigned int volatile *)REG_UART6_FUN_SEL)
#define     rUART6_LIN_CTL	(*(unsigned int volatile *)REG_UART6_LIN_CTL)
#define     rUART6_LIN_SR	(*(unsigned int volatile *)REG_UART6_LIN_SR)
#define     rUART6_DEBUG	(*(unsigned int volatile *)REG_UART6_DEBUG)
#define     rUART6_SC_CTL	(*(unsigned int volatile *)REG_UART6_SC_CTL)
#define     rUART6_SC_FSR	(*(unsigned int volatile *)REG_UART6_SC_FSR)

/*
  UART7 Control Registers
*/
#define     REG_UART7_RBR		(UART7_BA+0x00)  /* Receive Buffer Register */
#define     REG_UART7_THR		(UART7_BA+0x00)  /* Transmit Holding Register */
#define     REG_UART7_IER		(UART7_BA+0x04)  /* Interrupt Enable Register */
#define     REG_UART7_FCR		(UART7_BA+0x08)  /* FIFO Control Register */
#define     REG_UART7_LCR		(UART7_BA+0x0C)  /* Line Control Register */
#define     REG_UART7_MCR		(UART7_BA+0x10)  /* Modem Control Register */
#define     REG_UART7_MSR		(UART7_BA+0x14)  /* MODEM Status Register */
#define     REG_UART7_FSR		(UART7_BA+0x18)  /* FIFO Status Register */
#define     REG_UART7_ISR		(UART7_BA+0x1C)  /* Interrupt Status Register */
#define     REG_UART7_TOR		(UART7_BA+0x20)  /* Time Out Register */
#define     REG_UART7_BAUD		(UART7_BA+0x24)  /* Baud Rate Divisor Register */
#define     REG_UART7_IRCR		(UART7_BA+0x28)  /* IrDA Control Register */
#define     REG_UART7_ALT_CSR	(UART7_BA+0x2C)  /* Alternate Control/Status Register */
#define     REG_UART7_FUN_SEL	(UART7_BA+0x30)  /* Function Select Register */
#define     REG_UART7_LIN_CTL	(UART7_BA+0x34)  /* LIN Control Register */
#define     REG_UART7_LIN_SR	(UART7_BA+0x38)  /* LIN Status Register */
#define     REG_UART7_DEBUG		(UART7_BA+0x3C)  /* Debug Register */
#define     REG_UART7_SC_CTL	(UART7_BA+0x40)  /* SC Control Register */
#define     REG_UART7_SC_FSR	(UART7_BA+0x44)  /* SC Flag Status Register */


#define     rUART7_RBR		(*(unsigned int volatile *)REG_UART7_RBR)
#define     rUART7_THR		(*(unsigned int volatile *)REG_UART7_THR)
#define     rUART7_IER		(*(unsigned int volatile *)REG_UART7_IER)
#define     rUART7_FCR		(*(unsigned int volatile *)REG_UART7_FCR)
#define     rUART7_LCR		(*(unsigned int volatile *)REG_UART7_LCR)
#define     rUART7_MCR		(*(unsigned int volatile *)REG_UART7_MCR)
#define     rUART7_MSR		(*(unsigned int volatile *)REG_UART7_MSR)
#define     rUART7_FSR		(*(unsigned int volatile *)REG_UART7_FSR)
#define     rUART7_ISR		(*(unsigned int volatile *)REG_UART7_ISR)
#define     rUART7_TOR		(*(unsigned int volatile *)REG_UART7_TOR)
#define     rUART7_BAUD		(*(unsigned int volatile *)REG_UART7_BAUD)
#define     rUART7_IRCR		(*(unsigned int volatile *)REG_UART7_IRCR)
#define     rUART7_ALT_CSR	(*(unsigned int volatile *)REG_UART7_ALT_CSR)
#define     rUART7_FUN_SEL	(*(unsigned int volatile *)REG_UART7_FUN_SEL)
#define     rUART7_LIN_CTL	(*(unsigned int volatile *)REG_UART7_LIN_CTL)
#define     rUART7_LIN_SR	(*(unsigned int volatile *)REG_UART7_LIN_SR)
#define     rUART7_DEBUG	(*(unsigned int volatile *)REG_UART7_DEBUG)
#define     rUART7_SC_CTL	(*(unsigned int volatile *)REG_UART7_SC_CTL)
#define     rUART7_SC_FSR	(*(unsigned int volatile *)REG_UART7_SC_FSR)

/*
  UART8 Control Registers
*/
#define     REG_UART8_RBR		(UART8_BA+0x00)  /* Receive Buffer Register */
#define     REG_UART8_THR		(UART8_BA+0x00)  /* Transmit Holding Register */
#define     REG_UART8_IER		(UART8_BA+0x04)  /* Interrupt Enable Register */
#define     REG_UART8_FCR		(UART8_BA+0x08)  /* FIFO Control Register */
#define     REG_UART8_LCR		(UART8_BA+0x0C)  /* Line Control Register */
#define     REG_UART8_MCR		(UART8_BA+0x10)  /* Modem Control Register */
#define     REG_UART8_MSR		(UART8_BA+0x14)  /* MODEM Status Register */
#define     REG_UART8_FSR		(UART8_BA+0x18)  /* FIFO Status Register */
#define     REG_UART8_ISR		(UART8_BA+0x1C)  /* Interrupt Status Register */
#define     REG_UART8_TOR		(UART8_BA+0x20)  /* Time Out Register */
#define     REG_UART8_BAUD		(UART8_BA+0x24)  /* Baud Rate Divisor Register */
#define     REG_UART8_IRCR		(UART8_BA+0x28)  /* IrDA Control Register */
#define     REG_UART8_ALT_CSR	(UART8_BA+0x2C)  /* Alternate Control/Status Register */
#define     REG_UART8_FUN_SEL	(UART8_BA+0x30)  /* Function Select Register */
#define     REG_UART8_LIN_CTL	(UART8_BA+0x34)  /* LIN Control Register */
#define     REG_UART8_LIN_SR	(UART8_BA+0x38)  /* LIN Status Register */
#define     REG_UART8_DEBUG		(UART8_BA+0x3C)  /* Debug Register */
#define     REG_UART8_SC_CTL	(UART8_BA+0x40)  /* SC Control Register */
#define     REG_UART8_SC_FSR	(UART8_BA+0x44)  /* SC Flag Status Register */


#define     rUART8_RBR		(*(unsigned int volatile *)REG_UART8_RBR)
#define     rUART8_THR		(*(unsigned int volatile *)REG_UART8_THR)
#define     rUART8_IER		(*(unsigned int volatile *)REG_UART8_IER)
#define     rUART8_FCR		(*(unsigned int volatile *)REG_UART8_FCR)
#define     rUART8_LCR		(*(unsigned int volatile *)REG_UART8_LCR)
#define     rUART8_MCR		(*(unsigned int volatile *)REG_UART8_MCR)
#define     rUART8_MSR		(*(unsigned int volatile *)REG_UART8_MSR)
#define     rUART8_FSR		(*(unsigned int volatile *)REG_UART8_FSR)
#define     rUART8_ISR		(*(unsigned int volatile *)REG_UART8_ISR)
#define     rUART8_TOR		(*(unsigned int volatile *)REG_UART8_TOR)
#define     rUART8_BAUD		(*(unsigned int volatile *)REG_UART8_BAUD)
#define     rUART8_IRCR		(*(unsigned int volatile *)REG_UART8_IRCR)
#define     rUART8_ALT_CSR	(*(unsigned int volatile *)REG_UART8_ALT_CSR)
#define     rUART8_FUN_SEL	(*(unsigned int volatile *)REG_UART8_FUN_SEL)
#define     rUART8_LIN_CTL	(*(unsigned int volatile *)REG_UART8_LIN_CTL)
#define     rUART8_LIN_SR	(*(unsigned int volatile *)REG_UART8_LIN_SR)
#define     rUART8_DEBUG	(*(unsigned int volatile *)REG_UART8_DEBUG)
#define     rUART8_SC_CTL	(*(unsigned int volatile *)REG_UART8_SC_CTL)
#define     rUART8_SC_FSR	(*(unsigned int volatile *)REG_UART8_SC_FSR)

/* 
  Timer Control Registers 
*/
#define     REG_TMR_TCSR0		(TMR_BA+0x00)  /* Timer Control and Status Register 0 */
#define     REG_TMR_TICR0		(TMR_BA+0x04)  /* Timer Initial Control Register 0 */
#define     REG_TMR_TDR0		(TMR_BA+0x08)  /* Timer Data Register 0 */
#define     REG_TMR_TCSR1		(TMR_BA+0x10)  /* Timer Control and Status Register 1 */
#define     REG_TMR_TICR1		(TMR_BA+0x14)  /* Timer Initial Control Register 1 */
#define     REG_TMR_TDR1		(TMR_BA+0x18)  /* Timer Data Register 1 */
#define     REG_TMR_TCSR2		(TMR_BA+0x20)  /* Timer Control and Status Register 2 */
#define     REG_TMR_TICR2		(TMR_BA+0x24)  /* Timer Initial Control Register 2 */
#define     REG_TMR_TDR2		(TMR_BA+0x28)  /* Timer Data Register 2 */
#define     REG_TMR_TCSR3		(TMR_BA+0x30)  /* Timer Control and Status Register 3 */
#define     REG_TMR_TICR3		(TMR_BA+0x34)  /* Timer Initial Control Register 3 */
#define     REG_TMR_TDR3		(TMR_BA+0x38)  /* Timer Data Register 3 */
#define     REG_TMR_TCSR4		(TMR_BA+0x40)  /* Timer Control and Status Register 4 */
#define     REG_TMR_TICR4		(TMR_BA+0x44)  /* Timer Initial Control Register 4 */
#define     REG_TMR_TDR4		(TMR_BA+0x48)  /* Timer Data Register 4 */
#define     REG_TMR_TISR		(TMR_BA+0x60)  /* Timer Interrupt Status Register */

#define     rTMR_TCSR0			(*(unsigned int volatile *)REG_TMR_TCSR0)
#define     rTMR_TICR0			(*(unsigned int volatile *)REG_TMR_TICR0)
#define     rTMR_TDR0			(*(unsigned int volatile *)REG_TMR_TDR0)
#define     rTMR_TCSR1			(*(unsigned int volatile *)REG_TMR_TCSR1)
#define     rTMR_TICR1			(*(unsigned int volatile *)REG_TMR_TICR1)
#define     rTMR_TDR1			(*(unsigned int volatile *)REG_TMR_TDR1)
#define     rTMR_TCSR2			(*(unsigned int volatile *)REG_TMR_TCSR2)
#define     rTMR_TICR2			(*(unsigned int volatile *)REG_TMR_TICR2)
#define     rTMR_TDR2			(*(unsigned int volatile *)REG_TMR_TDR2)
#define     rTMR_TCSR3			(*(unsigned int volatile *)REG_TMR_TCSR3)
#define     rTMR_TICR3			(*(unsigned int volatile *)REG_TMR_TICR3)
#define     rTMR_TDR3			(*(unsigned int volatile *)REG_TMR_TDR3)
#define     rTMR_TCSR4			(*(unsigned int volatile *)REG_TMR_TCSR4)
#define     rTMR_TICR4			(*(unsigned int volatile *)REG_TMR_TICR4)
#define     rTMR_TDR4			(*(unsigned int volatile *)REG_TMR_TDR4)
#define     rTMR_TISR			(*(unsigned int volatile *)REG_TMR_TISR)


/*
  Watch Dog Timer Registers 
*/  
#define REG_WDT_CR				(WDT_BA + 0x00)	/* WDT control register */
#define REG_WDT_CRALT			(WDT_BA + 0x04)	/* WDT alternative control register */

#define rWDT_CR					(*(unsigned int volatile *)REG_WDT_CR)
#define rWDT_CRALT				(*(unsigned int volatile *)REG_WDT_CRALT)


/*
  Window Watch Dog Timer Registers 
*/  
#define REG_WWDT_RLD			(WWDT_BA + 0x00)	/* WWDT reload counter register */
#define REG_WWDT_CR				(WWDT_BA + 0x04)	/* WWDT control register */
#define REG_WWDT_SR				(WWDT_BA + 0x08)	/* WWDT status register */
#define REG_WWDT_CVR			(WWDT_BA + 0x0c)	/* WWDT counter value register */

#define rWWDT_RLD				(*(unsigned int volatile *)REG_WWDT_RLD)
#define rWWDT_CR				(*(unsigned int volatile *)REG_WWDT_CR)
#define rWWDT_SR				(*(unsigned int volatile *)REG_WWDT_SR)
#define rWWDT_CVR				(*(unsigned int volatile *)REG_WWDT_CVR)



/*
  Advance Interrupt Controller (AIC) Registers
*/
#define     REG_AIC_SCR1    (AIC_BA+0x00)    /* Source control register 1 */
#define     REG_AIC_SCR2    (AIC_BA+0x04)    /* Source control register 2 */
#define     REG_AIC_SCR3    (AIC_BA+0x08)    /* Source control register 3 */
#define     REG_AIC_SCR4    (AIC_BA+0x0C)    /* Source control register 4 */
#define     REG_AIC_SCR5    (AIC_BA+0x10)    /* Source control register 5 */
#define     REG_AIC_SCR6    (AIC_BA+0x14)    /* Source control register 6 */
#define     REG_AIC_SCR7    (AIC_BA+0x18)    /* Source control register 7 */
#define     REG_AIC_SCR8    (AIC_BA+0x1C)    /* Source control register 8 */
#define     REG_AIC_SCR9    (AIC_BA+0x20)    /* Source control register 9 */
#define     REG_AIC_SCR10   (AIC_BA+0x24)    /* Source control register 10 */
#define     REG_AIC_SCR11   (AIC_BA+0x28)    /* Source control register 11 */
#define     REG_AIC_SCR12   (AIC_BA+0x2C)    /* Source control register 12 */
#define     REG_AIC_SCR13   (AIC_BA+0x30)    /* Source control register 13 */
#define     REG_AIC_SCR14   (AIC_BA+0x34)    /* Source control register 14 */
#define     REG_AIC_SCR15   (AIC_BA+0x38)    /* Source control register 15 */
#define     REG_AIC_IRSR    (AIC_BA+0x100)   /* Interrupt raw status register */
#define     REG_AIC_IRSRH   (AIC_BA+0x104)   /* Interrupt raw status register (Hign) */
#define     REG_AIC_IASR    (AIC_BA+0x108)   /* Interrupt active status register */
#define     REG_AIC_IASRH   (AIC_BA+0x10C)   /* Interrupt active status register (Hign) */
#define     REG_AIC_ISR     (AIC_BA+0x110)   /* Interrupt status register */
#define     REG_AIC_ISRH    (AIC_BA+0x114)   /* Interrupt status register (High) */
#define     REG_AIC_IPER    (AIC_BA+0x118)   /* Interrupt priority encoding register */
#define     REG_AIC_ISNR    (AIC_BA+0x120)   /* Interrupt source number register */
#define     REG_AIC_OISR    (AIC_BA+0x124)   /* Output interrupt status register */
#define     REG_AIC_IMR     (AIC_BA+0x128)   /* Interrupt mask register */
#define     REG_AIC_IMRH    (AIC_BA+0x12C)   /* Interrupt mask register (High) */
#define     REG_AIC_MECR    (AIC_BA+0x130)   /* Mask enable command register */
#define     REG_AIC_MECRH   (AIC_BA+0x134)   /* Mask enable command register (High) */
#define     REG_AIC_MDCR    (AIC_BA+0x138)   /* Mask disable command register */
#define     REG_AIC_MDCRH   (AIC_BA+0x13C)   /* Mask disable command register (High) */
#define     REG_AIC_SSCR    (AIC_BA+0x140)   /* Source Set Command Register */
#define     REG_AIC_SSCRH   (AIC_BA+0x144)   /* Source Set Command Register (High) */
#define     REG_AIC_SCCR    (AIC_BA+0x148)   /* Source Clear Command Register */
#define     REG_AIC_SCCRH   (AIC_BA+0x14C)   /* Source Clear Command Register (High) */
#define     REG_AIC_EOSCR   (AIC_BA+0x150)   /* End of service command register */

#define     rAIC_SCR1    (*(unsigned int volatile *)REG_AIC_SCR1)
#define     rAIC_SCR2    (*(unsigned int volatile *)REG_AIC_SCR2)
#define     rAIC_SCR3    (*(unsigned int volatile *)REG_AIC_SCR3)
#define     rAIC_SCR4    (*(unsigned int volatile *)REG_AIC_SCR4)
#define     rAIC_SCR5    (*(unsigned int volatile *)REG_AIC_SCR5)
#define     rAIC_SCR6    (*(unsigned int volatile *)REG_AIC_SCR6)
#define     rAIC_SCR7    (*(unsigned int volatile *)REG_AIC_SCR7)
#define     rAIC_SCR8    (*(unsigned int volatile *)REG_AIC_SCR8)
#define     rAIC_SCR9    (*(unsigned int volatile *)REG_AIC_SCR9)
#define     rAIC_SCR10   (*(unsigned int volatile *)REG_AIC_SCR10)
#define     rAIC_SCR11   (*(unsigned int volatile *)REG_AIC_SCR11)
#define     rAIC_SCR12   (*(unsigned int volatile *)REG_AIC_SCR12)
#define     rAIC_SCR13   (*(unsigned int volatile *)REG_AIC_SCR13)
#define     rAIC_SCR14   (*(unsigned int volatile *)REG_AIC_SCR14)
#define     rAIC_SCR15   (*(unsigned int volatile *)REG_AIC_SCR15)
#define     rAIC_IRQSC   (*(unsigned int volatile *)REG_AIC_IRQSC)
#define     rAIC_GEN     (*(unsigned int volatile *)REG_AIC_GEN)
#define     rAIC_GASR    (*(unsigned int volatile *)REG_AIC_GASR)
#define     rAIC_GSCR    (*(unsigned int volatile *)REG_AIC_GSCR)
#define     rAIC_IRSR    (*(unsigned int volatile *)REG_AIC_IRSR)
#define     rAIC_IRSRH   (*(unsigned int volatile *)REG_AIC_IRSRH)
#define     rAIC_IASR    (*(unsigned int volatile *)REG_AIC_IASR)
#define     rAIC_IASRH   (*(unsigned int volatile *)REG_AIC_IASRH)
#define     rAIC_ISR     (*(unsigned int volatile *)REG_AIC_ISR)
#define     rAIC_ISRH    (*(unsigned int volatile *)REG_AIC_ISRH)
#define     rAIC_IPER    (*(unsigned int volatile *)REG_AIC_IPER)
#define     rAIC_ISNR    (*(unsigned int volatile *)REG_AIC_ISNR)
#define     rAIC_OISR    (*(unsigned int volatile *)REG_AIC_OISR)
#define     rAIC_IMR     (*(unsigned int volatile *)REG_AIC_IMR)
#define     rAIC_IMRH    (*(unsigned int volatile *)REG_AIC_IMRH)
#define     rAIC_MECR    (*(unsigned int volatile *)REG_AIC_MECR)
#define     rAIC_MECRH   (*(unsigned int volatile *)REG_AIC_MECRH)
#define     rAIC_MDCR    (*(unsigned int volatile *)REG_AIC_MDCR)
#define     rAIC_MDCRH   (*(unsigned int volatile *)REG_AIC_MDCRH)
#define     rAIC_SSCR    (*(unsigned int volatile *)REG_AIC_SSCR)
#define     rAIC_SSCRH   (*(unsigned int volatile *)REG_AIC_SSCRH)
#define     rAIC_SCCR    (*(unsigned int volatile *)REG_AIC_SCCR)
#define     rAIC_SCCRH   (*(unsigned int volatile *)REG_AIC_SCCRH)
#define     rAIC_EOSCR   (*(unsigned int volatile *)REG_AIC_EOSCR)


/*
  GPIO Control Registers  
*/
#if(0)
#define     REG_GPIOC_DIR		(GPIO_BA+0x04)  /* GPIO portC direction control register */
#define     REG_GPIOC_DATAOUT	(GPIO_BA+0x08)  /* GPIO portC data output register */
#define     REG_GPIOC_DATAIN	(GPIO_BA+0x0C)  /* GPIO portC data input register */
#define     REG_GPIOD_DIR		(GPIO_BA+0x14)  /* GPIO portD direction control register */
#define     REG_GPIOD_DATAOUT	(GPIO_BA+0x18)  /* GPIO portD data output register */
#define     REG_GPIOD_DATAIN	(GPIO_BA+0x1C)  /* GPIO portD data input register */
#define     REG_GPIOE_DIR		(GPIO_BA+0x24)  /* GPIO portE direction control register */
#define     REG_GPIOE_DATAOUT	(GPIO_BA+0x28)  /* GPIO portE data output register */
#define     REG_GPIOE_DATAIN	(GPIO_BA+0x2C)  /* GPIO portE data input register */
#define     REG_GPIOF_DIR		(GPIO_BA+0x34)  /* GPIO portF direction control register */
#define     REG_GPIOF_DATAOUT	(GPIO_BA+0x38)  /* GPIO portF data output register */
#define     REG_GPIOF_DATAIN	(GPIO_BA+0x3C)  /* GPIO portF data input register */
#define     REG_GPIOG_DIR		(GPIO_BA+0x44)  /* GPIO portG direction control register */
#define     REG_GPIOG_DATAOUT	(GPIO_BA+0x48)  /* GPIO portG data output register */
#define     REG_GPIOG_DATAIN	(GPIO_BA+0x4C)  /* GPIO portG data input register */
#define     REG_GPIOH_DBNCE		(GPIO_BA+0x50)  /* GPIO portH input debounce control reg. */
#define     REG_GPIOH_DIR		(GPIO_BA+0x54)  /* GPIO portH direction control register */
#define     REG_GPIOH_DATAOUT	(GPIO_BA+0x58)  /* GPIO portH data output register */
#define     REG_GPIOH_DATAIN	(GPIO_BA+0x5C)  /* GPIO portH data input register */
#define     REG_GPIOI_DIR		(GPIO_BA+0x64)  /* GPIO portI direction control register */
#define     REG_GPIOI_DATAOU	(GPIO_BA+0x68)  /* GPIO portI data output register */
#define     REG_GPIOI_DATAIN	(GPIO_BA+0x6C)  /* GPIO portI data input register */

#define     rGPIOC_DIR			(*(unsigned int volatile *)REG_GPIOC_DIR)
#define     rGPIOC_DATAOUT		(*(unsigned int volatile *)REG_GPIOC_DATAOUT)
#define     rGPIOC_DATAIN		(*(unsigned int volatile *)REG_GPIOC_DATAIN)
#define     rGPIOD_DIR			(*(unsigned int volatile *)REG_GPIOD_DIR)
#define     rGPIOD_DATAOUT		(*(unsigned int volatile *)REG_GPIOD_DATAOUT)
#define     rGPIOD_DATAIN		(*(unsigned int volatile *)REG_GPIOD_DATAIN)
#define     rGPIOE_DIR			(*(unsigned int volatile *)REG_GPIOE_DIR)
#define     rGPIOE_DATAOUT		(*(unsigned int volatile *)REG_GPIOE_DATAOUT)
#define     rGPIOE_DATAIN		(*(unsigned int volatile *)REG_GPIOE_DATAIN)
#define     rGPIOF_DIR			(*(unsigned int volatile *)REG_GPIOF_DIR)
#define     rGPIOF_DATAOUT		(*(unsigned int volatile *)REG_GPIOF_DATAOUT)
#define     rGPIOF_DATAIN		(*(unsigned int volatile *)REG_GPIOF_DATAIN)
#define     rGPIOG_DIR			(*(unsigned int volatile *)REG_GPIOG_DIR)
#define     rGPIOG_DATAOUT		(*(unsigned int volatile *)REG_GPIOG_DATAOUT)
#define     rGPIOG_DATAIN		(*(unsigned int volatile *)REG_GPIOG_DATAIN)
#define     rGPIOH_DBNCE		(*(unsigned int volatile *)REG_GPIOH_DBNCE)
#define     rGPIOH_DIR			(*(unsigned int volatile *)REG_GPIOH_DIR)
#define     rGPIOH_DATAOUT		(*(unsigned int volatile *)REG_GPIOH_DATAOUT)
#define     rGPIOH_DATAIN		(*(unsigned int volatile *)REG_GPIOH_DATAIN)
#define     rGPIOI_DIR			(*(unsigned int volatile *)REG_GPIOI_DIR)
#define     rGPIOI_DATAOU		(*(unsigned int volatile *)REG_GPIOI_DATAOU)
#define     rGPIOI_DATAIN		(*(unsigned int volatile *)REG_GPIOI_DATAIN)

#else
#define     REG_GPIOA_DIR       (GPIO_BA+0x000)  /*!< GPIO portA direction control register */
#define     REG_GPIOA_DATAOUT   (GPIO_BA+0x004)  /*!< GPIO portA data output register */
#define     REG_GPIOA_DATAIN    (GPIO_BA+0x008)  /*!< GPIO portA data input register */
#define     REG_GPIOA_IMD       (GPIO_BA+0x00C)  /*!< GPIO Port A Interrupt Mode Register */
#define     REG_GPIOA_IREN      (GPIO_BA+0x010)  /*!< GPIO Port A Interrupt Rising-Edge or Level-High Enable Register */
#define     REG_GPIOA_IFEN      (GPIO_BA+0x014)  /*!< GPIO Port A Interrupt Falling-Edge or Level-Low Enable Register */
#define     REG_GPIOA_ISR       (GPIO_BA+0x018)  /*!< GPIO Port A Interrupt Status Register */
#define     REG_GPIOA_DBEN      (GPIO_BA+0x01C)  /*!< GPIO Port A De-bounce Enable Register */
#define     REG_GPIOA_PUEN      (GPIO_BA+0x020)  /*!< GPIO Port A Pull-Up Enable Register */
#define     REG_GPIOA_PDEN      (GPIO_BA+0x024)  /*!< GPIO Port A Pull-Down Enable Register */
#define     REG_GPIOA_ICEN      (GPIO_BA+0x028)  /*!< GPIO Port A CMOS Input Enable Register */
#define     REG_GPIOA_ISEN      (GPIO_BA+0x02C)  /*!< GPIO Port A Schmitt-Trigger Input Enable Register */

#define     REG_GPIOB_DIR       (GPIO_BA+0x040)  /*!< GPIO port B direction control register */
#define     REG_GPIOB_DATAOUT   (GPIO_BA+0x044)  /*!< GPIO port B data output register */
#define     REG_GPIOB_DATAIN    (GPIO_BA+0x048)  /*!< GPIO port B data input register */
#define     REG_GPIOB_IMD       (GPIO_BA+0x04C)  /*!< GPIO Port B Interrupt Mode Register */
#define     REG_GPIOB_IREN      (GPIO_BA+0x050)  /*!< GPIO Port B Interrupt Rising-Edge or Level-High Enable Register */
#define     REG_GPIOB_IFEN      (GPIO_BA+0x054)  /*!< GPIO Port B Interrupt Falling-Edge or Level-Low Enable Register */
#define     REG_GPIOB_ISR       (GPIO_BA+0x058)  /*!< GPIO Port B Interrupt Status Register */
#define     REG_GPIOB_DBEN      (GPIO_BA+0x05C)  /*!< GPIO Port B De-bounce Enable Register */
#define     REG_GPIOB_PUEN      (GPIO_BA+0x060)  /*!< GPIO Port B Pull-Up Enable Register */
#define     REG_GPIOB_PDEN      (GPIO_BA+0x064)  /*!< GPIO Port B Pull-Down Enable Register */
#define     REG_GPIOB_ICEN      (GPIO_BA+0x068)  /*!< GPIO Port B CMOS Input Enable Register */
#define     REG_GPIOB_ISEN      (GPIO_BA+0x06C)  /*!< GPIO Port B Schmitt-Trigger Input Enable Register */

#define     REG_GPIOC_DIR       (GPIO_BA+0x080)  /*!< GPIO port C direction control register */
#define     REG_GPIOC_DATAOUT   (GPIO_BA+0x084)  /*!< GPIO port C data output register */
#define     REG_GPIOC_DATAIN    (GPIO_BA+0x088)  /*!< GPIO port C data input register */
#define     REG_GPIOC_IMD       (GPIO_BA+0x08C)  /*!< GPIO Port C Interrupt Mode Register */
#define     REG_GPIOC_IREN      (GPIO_BA+0x090)  /*!< GPIO Port C Interrupt Rising-Edge or Level-High Enable Register */
#define     REG_GPIOC_IFEN      (GPIO_BA+0x094)  /*!< GPIO Port C Interrupt Falling-Edge or Level-Low Enable Register */
#define     REG_GPIOC_ISR       (GPIO_BA+0x098)  /*!< GPIO Port C Interrupt Status Register */
#define     REG_GPIOC_DBEN      (GPIO_BA+0x09C)  /*!< GPIO Port C De-bounce Enable Register */
#define     REG_GPIOC_PUEN      (GPIO_BA+0x0A0)  /*!< GPIO Port C Pull-Up Enable Register */
#define     REG_GPIOC_PDEN      (GPIO_BA+0x0A4)  /*!< GPIO Port C Pull-Down Enable Register */
#define     REG_GPIOC_ICEN      (GPIO_BA+0x0A8)  /*!< GPIO Port C CMOS Input Enable Register */
#define     REG_GPIOC_ISEN      (GPIO_BA+0x0AC)  /*!< GPIO Port C Schmitt-Trigger Input Enable Register */

#define     REG_GPIOD_DIR       (GPIO_BA+0x0C0)  /*!< GPIO port D direction control register */
#define     REG_GPIOD_DATAOUT   (GPIO_BA+0x0C4)  /*!< GPIO port D data output register */
#define     REG_GPIOD_DATAIN    (GPIO_BA+0x0C8)  /*!< GPIO port D data input register */
#define     REG_GPIOD_IMD       (GPIO_BA+0x0CC)  /*!< GPIO Port D Interrupt Mode Register */
#define     REG_GPIOD_IREN      (GPIO_BA+0x0D0)  /*!< GPIO Port D Interrupt Rising-Edge or Level-High Enable Register */
#define     REG_GPIOD_IFEN      (GPIO_BA+0x0D4)  /*!< GPIO Port D Interrupt Falling-Edge or Level-Low Enable Register */
#define     REG_GPIOD_ISR       (GPIO_BA+0x0D8)  /*!< GPIO Port D Interrupt Status Register */
#define     REG_GPIOD_DBEN      (GPIO_BA+0x0DC)  /*!< GPIO Port D De-bounce Enable Register */
#define     REG_GPIOD_PUEN      (GPIO_BA+0x0E0)  /*!< GPIO Port D Pull-Up Enable Register */
#define     REG_GPIOD_PDEN      (GPIO_BA+0x0E4)  /*!< GPIO Port D Pull-Down Enable Register */
#define     REG_GPIOD_ICEN      (GPIO_BA+0x0E8)  /*!< GPIO Port D CMOS Input Enable Register */
#define     REG_GPIOD_ISEN      (GPIO_BA+0x0EC)  /*!< GPIO Port D Schmitt-Trigger Input Enable Register */

#define     REG_GPIOE_DIR       (GPIO_BA+0x100)  /*!< GPIO port E direction control register */
#define     REG_GPIOE_DATAOUT   (GPIO_BA+0x104)  /*!< GPIO port E data output register */
#define     REG_GPIOE_DATAIN    (GPIO_BA+0x108)  /*!< GPIO port E data input register */
#define     REG_GPIOE_IMD       (GPIO_BA+0x10C)  /*!< GPIO Port E Interrupt Mode Register */
#define     REG_GPIOE_IREN      (GPIO_BA+0x110)  /*!< GPIO Port E Interrupt Rising-Edge or Level-High Enable Register */
#define     REG_GPIOE_IFEN      (GPIO_BA+0x114)  /*!< GPIO Port E Interrupt Falling-Edge or Level-Low Enable Register */
#define     REG_GPIOE_ISR       (GPIO_BA+0x118)  /*!< GPIO Port E Interrupt Status Register */
#define     REG_GPIOE_DBEN      (GPIO_BA+0x11C)  /*!< GPIO Port E De-bounce Enable Register */
#define     REG_GPIOE_PUEN      (GPIO_BA+0x120)  /*!< GPIO Port E Pull-Up Enable Register */
#define     REG_GPIOE_PDEN      (GPIO_BA+0x124)  /*!< GPIO Port E Pull-Down Enable Register */
#define     REG_GPIOE_ICEN      (GPIO_BA+0x128)  /*!< GPIO Port E CMOS Input Enable Register */
#define     REG_GPIOE_ISEN      (GPIO_BA+0x12C)  /*!< GPIO Port E Schmitt-Trigger Input Enable Register */

#define     REG_GPIOF_DIR       (GPIO_BA+0x140)  /*!< GPIO port F direction control register */
#define     REG_GPIOF_DATAOUT   (GPIO_BA+0x144)  /*!< GPIO port F data output register */
#define     REG_GPIOF_DATAIN    (GPIO_BA+0x148)  /*!< GPIO port F data input register */
#define     REG_GPIOF_IMD       (GPIO_BA+0x14C)  /*!< GPIO Port F Interrupt Mode Register */
#define     REG_GPIOF_IREN      (GPIO_BA+0x150)  /*!< GPIO Port F Interrupt Rising-Edge or Level-High Enable Register */
#define     REG_GPIOF_IFEN      (GPIO_BA+0x154)  /*!< GPIO Port F Interrupt Falling-Edge or Level-Low Enable Register */
#define     REG_GPIOF_ISR       (GPIO_BA+0x158)  /*!< GPIO Port F Interrupt Status Register */
#define     REG_GPIOF_DBEN      (GPIO_BA+0x15C)  /*!< GPIO Port F De-bounce Enable Register */
#define     REG_GPIOF_PUEN      (GPIO_BA+0x160)  /*!< GPIO Port F Pull-Up Enable Register */
#define     REG_GPIOF_PDEN      (GPIO_BA+0x164)  /*!< GPIO Port F Pull-Down Enable Register */
#define     REG_GPIOF_ICEN      (GPIO_BA+0x168)  /*!< GPIO Port F CMOS Input Enable Register */
#define     REG_GPIOF_ISEN      (GPIO_BA+0x16C)  /*!< GPIO Port F Schmitt-Trigger Input Enable Register */

#define     REG_GPIOG_DIR       (GPIO_BA+0x180)  /*!< GPIO port G direction control register */
#define     REG_GPIOG_DATAOUT   (GPIO_BA+0x184)  /*!< GPIO port G data output register */
#define     REG_GPIOG_DATAIN    (GPIO_BA+0x188)  /*!< GPIO port G data input register */
#define     REG_GPIOG_IMD       (GPIO_BA+0x18C)  /*!< GPIO Port G Interrupt Mode Register */
#define     REG_GPIOG_IREN      (GPIO_BA+0x190)  /*!< GPIO Port G Interrupt Rising-Edge or Level-High Enable Register */
#define     REG_GPIOG_IFEN      (GPIO_BA+0x194)  /*!< GPIO Port G Interrupt Falling-Edge or Level-Low Enable Register */
#define     REG_GPIOG_ISR       (GPIO_BA+0x198)  /*!< GPIO Port G Interrupt Status Register */
#define     REG_GPIOG_DBEN      (GPIO_BA+0x19C)  /*!< GPIO Port G De-bounce Enable Register */
#define     REG_GPIOG_PUEN      (GPIO_BA+0x1A0)  /*!< GPIO Port G Pull-Up Enable Register */
#define     REG_GPIOG_PDEN      (GPIO_BA+0x1A4)  /*!< GPIO Port G Pull-Down Enable Register */
#define     REG_GPIOG_ICEN      (GPIO_BA+0x1A8)  /*!< GPIO Port G CMOS Input Enable Register */
#define     REG_GPIOG_ISEN      (GPIO_BA+0x1AC)  /*!< GPIO Port G Schmitt-Trigger Input Enable Register */

#define     REG_GPIOH_DIR       (GPIO_BA+0x1C0)  /*!< GPIO port H direction control register */
#define     REG_GPIOH_DATAOUT   (GPIO_BA+0x1C4)  /*!< GPIO port H data output register */
#define     REG_GPIOH_DATAIN    (GPIO_BA+0x1C8)  /*!< GPIO port H data input register */
#define     REG_GPIOH_IMD       (GPIO_BA+0x1CC)  /*!< GPIO Port H Interrupt Mode Register */
#define     REG_GPIOH_IREN      (GPIO_BA+0x1D0)  /*!< GPIO Port H Interrupt Rising-Edge or Level-High Enable Register */
#define     REG_GPIOH_IFEN      (GPIO_BA+0x1D4)  /*!< GPIO Port H Interrupt Falling-Edge or Level-Low Enable Register */
#define     REG_GPIOH_ISR       (GPIO_BA+0x1D8)  /*!< GPIO Port H Interrupt Status Register */
#define     REG_GPIOH_DBEN      (GPIO_BA+0x1DC)  /*!< GPIO Port H De-bounce Enable Register */
#define     REG_GPIOH_PUEN      (GPIO_BA+0x1E0)  /*!< GPIO Port H Pull-Up Enable Register */
#define     REG_GPIOH_PDEN      (GPIO_BA+0x1E4)  /*!< GPIO Port H Pull-Down Enable Register */
#define     REG_GPIOH_ICEN      (GPIO_BA+0x1E8)  /*!< GPIO Port H CMOS Input Enable Register */
#define     REG_GPIOH_ISEN      (GPIO_BA+0x1EC)  /*!< GPIO Port H Schmitt-Trigger Input Enable Register */

#define     REG_GPIOI_DIR       (GPIO_BA+0x200)  /*!< GPIO port I direction control register */
#define     REG_GPIOI_DATAOUT   (GPIO_BA+0x204)  /*!< GPIO port I data output register */
#define     REG_GPIOI_DATAIN    (GPIO_BA+0x208)  /*!< GPIO port I data input register */
#define     REG_GPIOI_IMD       (GPIO_BA+0x20C)  /*!< GPIO Port I Interrupt Mode Register */
#define     REG_GPIOI_IREN      (GPIO_BA+0x210)  /*!< GPIO Port I Interrupt Rising-Edge or Level-High Enable Register */
#define     REG_GPIOI_IFEN      (GPIO_BA+0x214)  /*!< GPIO Port I Interrupt Falling-Edge or Level-Low Enable Register */
#define     REG_GPIOI_ISR       (GPIO_BA+0x218)  /*!< GPIO Port I Interrupt Status Register */
#define     REG_GPIOI_DBEN      (GPIO_BA+0x21C)  /*!< GPIO Port I De-bounce Enable Register */
#define     REG_GPIOI_PUEN      (GPIO_BA+0x220)  /*!< GPIO Port I Pull-Up Enable Register */
#define     REG_GPIOI_PDEN      (GPIO_BA+0x224)  /*!< GPIO Port I Pull-Down Enable Register */
#define     REG_GPIOI_ICEN      (GPIO_BA+0x228)  /*!< GPIO Port I CMOS Input Enable Register */
#define     REG_GPIOI_ISEN      (GPIO_BA+0x22C)  /*!< GPIO Port I Schmitt-Trigger Input Enable Register */

#define     REG_GPIOJ_DIR       (GPIO_BA+0x240)  /*!< GPIO port J direction control register */
#define     REG_GPIOJ_DATAOUT   (GPIO_BA+0x244)  /*!< GPIO port J data output register */
#define     REG_GPIOJ_DATAIN    (GPIO_BA+0x248)  /*!< GPIO port J data input register */
#define     REG_GPIOJ_IMD       (GPIO_BA+0x24C)  /*!< GPIO Port J Interrupt Mode Register */
#define     REG_GPIOJ_IREN      (GPIO_BA+0x250)  /*!< GPIO Port J Interrupt Rising-Edge or Level-High Enable Register */
#define     REG_GPIOJ_IFEN      (GPIO_BA+0x254)  /*!< GPIO Port J Interrupt Falling-Edge or Level-Low Enable Register */
#define     REG_GPIOJ_ISR       (GPIO_BA+0x258)  /*!< GPIO Port J Interrupt Status Register */
#define     REG_GPIOJ_DBEN      (GPIO_BA+0x25C)  /*!< GPIO Port J De-bounce Enable Register */
#define     REG_GPIOJ_PUEN      (GPIO_BA+0x260)  /*!< GPIO Port J Pull-Up Enable Register */
#define     REG_GPIOJ_PDEN      (GPIO_BA+0x264)  /*!< GPIO Port J Pull-Down Enable Register */
#define     REG_GPIOJ_ICEN      (GPIO_BA+0x268)  /*!< GPIO Port J CMOS Input Enable Register */
#define     REG_GPIOJ_ISEN      (GPIO_BA+0x26C)  /*!< GPIO Port J Schmitt-Trigger Input Enable Register */

#define     REG_GPIO_DBNCECON   (GPIO_BA+0x3F0)  /*!< GPIO Debounce Control Register */
#define     REG_GPIO_ISR        (GPIO_BA+0x3FC)  /*!< GPIO Port Interrupt Status Register */

/**@}*/ /* end of GPIO register group */



#define     rGPIOC_DIR			(*(unsigned int volatile *)REG_GPIOC_DIR)
#define     rGPIOC_DATAOUT		(*(unsigned int volatile *)REG_GPIOC_DATAOUT)
#define     rGPIOC_DATAIN		(*(unsigned int volatile *)REG_GPIOC_DATAIN)
#define     rGPIOD_DIR			(*(unsigned int volatile *)REG_GPIOD_DIR)
#define     rGPIOD_DATAOUT		(*(unsigned int volatile *)REG_GPIOD_DATAOUT)
#define     rGPIOD_DATAIN		(*(unsigned int volatile *)REG_GPIOD_DATAIN)
#define     rGPIOE_DIR			(*(unsigned int volatile *)REG_GPIOE_DIR)
#define     rGPIOE_DATAOUT		(*(unsigned int volatile *)REG_GPIOE_DATAOUT)
#define     rGPIOE_DATAIN		(*(unsigned int volatile *)REG_GPIOE_DATAIN)
#define     rGPIOF_DIR			(*(unsigned int volatile *)REG_GPIOF_DIR)
#define     rGPIOF_DATAOUT		(*(unsigned int volatile *)REG_GPIOF_DATAOUT)
#define     rGPIOF_DATAIN		(*(unsigned int volatile *)REG_GPIOF_DATAIN)
#define     rGPIOG_DIR			(*(unsigned int volatile *)REG_GPIOG_DIR)
#define     rGPIOG_DATAOUT		(*(unsigned int volatile *)REG_GPIOG_DATAOUT)
#define     rGPIOG_DATAIN		(*(unsigned int volatile *)REG_GPIOG_DATAIN)
#define     rGPIOH_DBNCE		(*(unsigned int volatile *)REG_GPIOH_DBNCE)
#define     rGPIOH_DIR			(*(unsigned int volatile *)REG_GPIOH_DIR)
#define     rGPIOH_DATAOUT		(*(unsigned int volatile *)REG_GPIOH_DATAOUT)
#define     rGPIOH_DATAIN		(*(unsigned int volatile *)REG_GPIOH_DATAIN)
#define     rGPIOI_DIR			(*(unsigned int volatile *)REG_GPIOI_DIR)
#define     rGPIOI_DATAOU		(*(unsigned int volatile *)REG_GPIOI_DATAOU)
#define     rGPIOI_DATAIN		(*(unsigned int volatile *)REG_GPIOI_DATAIN)

#endif

/* 
  RTC Control Registers
*/
#define     REG_RTC_INIR	(RTC_BA+0x00)   /* RTC Initiation Register */
#define     REG_RTC_AER		(RTC_BA+0x04)   /* RTC Access Enable Register */
#define     REG_RTC_FCR		(RTC_BA+0x08)   /* RTC Frequency Compensation Register */
#define     REG_RTC_TLR		(RTC_BA+0x0C)   /* Time Loading Register */
#define     REG_RTC_CLR		(RTC_BA+0x10)   /* Calendar Loading Register */
#define     REG_RTC_TSSR	(RTC_BA+0x14)   /* Time Scale Selection Register */
#define     REG_RTC_DWR		(RTC_BA+0x18)   /* Day of the Week Register */
#define     REG_RTC_TAR		(RTC_BA+0x1C)   /* Time Alarm Register */
#define     REG_RTC_CAR		(RTC_BA+0x20)   /* Calendar Alarm Register */
#define     REG_RTC_LIR		(RTC_BA+0x24)   /* Leap year Indicator Register */
#define     REG_RTC_RIER	(RTC_BA+0x28)   /* RTC Interrupt Enable Register */
#define     REG_RTC_RIIR	(RTC_BA+0x2C)   /* RTC Interrupt Indicator Register */
#define     REG_RTC_TTR		(RTC_BA+0x30)   /* RTC Time Tick Register */

#define     rRTC_INIR		(*(unsigned int volatile *)REG_RTC_INIR)
#define     rRTC_AER		(*(unsigned int volatile *)REG_RTC_AER)
#define     rRTC_FCR		(*(unsigned int volatile *)REG_RTC_FCR)
#define     rRTC_TLR		(*(unsigned int volatile *)REG_RTC_TLR)
#define     rRTC_CLR		(*(unsigned int volatile *)REG_RTC_CLR)
#define     rRTC_TSSR		(*(unsigned int volatile *)REG_RTC_TSSR)
#define     rRTC_DWR		(*(unsigned int volatile *)REG_RTC_DWR)
#define     rRTC_TAR		(*(unsigned int volatile *)REG_RTC_TAR)
#define     rRTC_CAR		(*(unsigned int volatile *)REG_RTC_CAR)
#define     rRTC_LIR		(*(unsigned int volatile *)REG_RTC_LIR)
#define     rRTC_RIER		(*(unsigned int volatile *)REG_RTC_RIER)
#define     rRTC_RIIR		(*(unsigned int volatile *)REG_RTC_RIIR)
#define     rRTC_TTR		(*(unsigned int volatile *)REG_RTC_TTR)


/*
  SmartCard Controller Registers
*/
#define     REG_SC0_RBR		(SC0_BA+0x000)   /* Receive Buffer Register */
#define     REG_SC0_TBR		(SC0_BA+0x000)   /* Transmit Buffer Register */
#define     REG_SC0_CTL		(SC0_BA+0x004)   /* Control Register */
#define     REG_SC0_ALTCTL	(SC0_BA+0x008)   /* Alternate Control Register */
#define     REG_SC0_EGTR	(SC0_BA+0x00C)   /* Extend Gaurd Time Register */
#define     REG_SC0_FRTMR	(SC0_BA+0x010)   /* Receive Buffer Time Out Register */
#define     REG_SC0_ETUCR	(SC0_BA+0x014)   /* ETU Control Register */
#define     REG_SC0_IER		(SC0_BA+0x018)   /* Interrupt Enable Register */
#define     REG_SC0_ISR		(SC0_BA+0x01C)   /* Interrupt Status Register */
#define     REG_SC0_TRST	(SC0_BA+0x020)   /* Transfer Status Register */
#define     REG_SC0_PINCSR	(SC0_BA+0x024)   /* Pin Control State Register */
#define     REG_SC0_TMR0	(SC0_BA+0x028)   /* Internal Timer Control Register 0 */
#define     REG_SC0_TMR1	(SC0_BA+0x02C)   /* Internal Timer Control Register 1 */
#define     REG_SC0_TMR2	(SC0_BA+0x030)   /* Internal Timer Control Register 2 */
#define     REG_SC0_UACTL	(SC0_BA+0x034)   /* UART Mode Control Register */
#define     REG_SC0_TDRA	(SC0_BA+0x038)   /* Timer Current Data Register A */
#define     REG_SC0_TDRB	(SC0_BA+0x03C)   /* Timer Current Data Register B */

#define     rSC0_RBR		(*(unsigned int volatile *)REG_SC0_RBR)
#define     rSC0_TBR		(*(unsigned int volatile *)REG_SC0_TBR)
#define     rSC0_CTL		(*(unsigned int volatile *)REG_SC0_CTL)
#define     rSC0_ALTCTL		(*(unsigned int volatile *)REG_SC0_ALTCTL)
#define     rSC0_EGTR		(*(unsigned int volatile *)REG_SC0_EGTR)
#define     rSC0_FRTMR		(*(unsigned int volatile *)REG_SC0_FRTMR)
#define     rSC0_ETUCR		(*(unsigned int volatile *)REG_SC0_ETUCR)
#define     rSC0_IER		(*(unsigned int volatile *)REG_SC0_IER)
#define     rSC0_ISR		(*(unsigned int volatile *)REG_SC0_ISR)
#define     rSC0_TRST		(*(unsigned int volatile *)REG_SC0_TRST)
#define     rSC0_PINCSR		(*(unsigned int volatile *)REG_SC0_PINCSR)
#define     rSC0_TMR0		(*(unsigned int volatile *)REG_SC0_TMR0)
#define     rSC0_TMR1		(*(unsigned int volatile *)REG_SC0_TMR1)
#define     rSC0_TMR2		(*(unsigned int volatile *)REG_SC0_TMR2)
#define     rSC0_UACTL		(*(unsigned int volatile *)REG_SC0_UACTL)
#define     rSC0_TDRA		(*(unsigned int volatile *)REG_SC0_TDRA)
#define     rSC0_TDRB		(*(unsigned int volatile *)REG_SC0_TDRB)


#define     REG_SC1_RBR		(SC1_BA+0x000)   /* Receive Buffer Register */
#define     REG_SC1_TBR		(SC1_BA+0x000)   /* Transmit Buffer Register */
#define     REG_SC1_CTL		(SC1_BA+0x004)   /* Control Register */
#define     REG_SC1_ALTCTL	(SC1_BA+0x008)   /* Alternate Control Register */
#define     REG_SC1_EGTR	(SC1_BA+0x00C)   /* Extend Gaurd Time Register */
#define     REG_SC1_FRTMR	(SC1_BA+0x010)   /* Receive Buffer Time Out Register */
#define     REG_SC1_ETUCR	(SC1_BA+0x014)   /* ETU Control Register */
#define     REG_SC1_IER		(SC1_BA+0x018)   /* Interrupt Enable Register */
#define     REG_SC1_ISR		(SC1_BA+0x01C)   /* Interrupt Status Register */
#define     REG_SC1_TRST	(SC1_BA+0x020)   /* Transfer Status Register */
#define     REG_SC1_PINCSR	(SC1_BA+0x024)   /* Pin Control State Register */
#define     REG_SC1_TMR0	(SC1_BA+0x028)   /* Internal Timer Control Register 0 */
#define     REG_SC1_TMR1	(SC1_BA+0x02C)   /* Internal Timer Control Register 1 */
#define     REG_SC1_TMR2	(SC1_BA+0x030)   /* Internal Timer Control Register 2 */
#define     REG_SC1_UACTL	(SC1_BA+0x034)   /* UART Mode Control Register */
#define     REG_SC1_TDRA	(SC1_BA+0x038)   /* Timer Current Data Register A */
#define     REG_SC1_TDRB	(SC1_BA+0x03C)   /* Timer Current Data Register B */

#define     rSC1_RBR		(*(unsigned int volatile *)REG_SC1_RBR)
#define     rSC1_TBR		(*(unsigned int volatile *)REG_SC1_TBR)
#define     rSC1_CTL		(*(unsigned int volatile *)REG_SC1_CTL)
#define     rSC1_ALTCTL		(*(unsigned int volatile *)REG_SC1_ALTCTL)
#define     rSC1_EGTR		(*(unsigned int volatile *)REG_SC1_EGTR)
#define     rSC1_FRTMR		(*(unsigned int volatile *)REG_SC1_FRTMR)
#define     rSC1_ETUCR		(*(unsigned int volatile *)REG_SC1_ETUCR)
#define     rSC1_IER		(*(unsigned int volatile *)REG_SC1_IER)
#define     rSC1_ISR		(*(unsigned int volatile *)REG_SC1_ISR)
#define     rSC1_TRST		(*(unsigned int volatile *)REG_SC1_TRST)
#define     rSC1_PINCSR		(*(unsigned int volatile *)REG_SC1_PINCSR)
#define     rSC1_TMR0		(*(unsigned int volatile *)REG_SC1_TMR0)
#define     rSC1_TMR1		(*(unsigned int volatile *)REG_SC1_TMR1)
#define     rSC1_TMR2		(*(unsigned int volatile *)REG_SC1_TMR2)
#define     rSC1_UACTL		(*(unsigned int volatile *)REG_SC1_UACTL)
#define     rSC1_TDRA		(*(unsigned int volatile *)REG_SC1_TDRA)
#define     rSC1_TDRB		(*(unsigned int volatile *)REG_SC1_TDRB)


/*
  I2C 0 Control Registers
*/
#define     REG_IIC0_CS			(I2C0_BA+0x00)  /* Control and Status Register */
#define     REG_IIC0_DIVIDER	(I2C0_BA+0x04)  /* Clock Prescale Register */
#define     REG_IIC0_CMDR		(I2C0_BA+0x08)  /* Command Register */
#define     REG_IIC0_SWR		(I2C0_BA+0x0C)  /* Software Mode Control Register */
#define     REG_IIC0_RxR		(I2C0_BA+0x10)  /* Data Receive Register */
#define     REG_IIC0_TxR		(I2C0_BA+0x14)  /* Data Transmit Register */

#define     rIIC0_CS			(*(unsigned int volatile *)REG_IIC0_CS)
#define     rIIC0_DIVIDER		(*(unsigned int volatile *)REG_IIC0_DIVIDER)
#define     rIIC0_CMDR			(*(unsigned int volatile *)REG_IIC0_CMDR)
#define     rIIC0_SWR			(*(unsigned int volatile *)REG_IIC0_SWR)
#define     rIIC0_RxR			(*(unsigned int volatile *)REG_IIC0_RxR)
#define     rIIC0_TxR			(*(unsigned int volatile *)REG_IIC0_TxR)


/*
  I2C 1 Control Registers
*/
#define     REG_IIC1_CS			(I2C1_BA+0x00)  /* Control and Status Register */
#define     REG_IIC1_DIVIDER	(I2C1_BA+0x04)  /* Clock Prescale Register */
#define     REG_IIC1_CMDR		(I2C1_BA+0x08)  /* Command Register */
#define     REG_IIC1_SWR		(I2C1_BA+0x0C)  /* Software Mode Control Register */
#define     REG_IIC1_RxR		(I2C1_BA+0x10)  /* Data Receive Register */
#define     REG_IIC1_TxR		(I2C1_BA+0x14)  /* Data Transmit Register */

#define     rIIC1_CS			(*(unsigned int volatile *)REG_IIC1_CS)
#define     rIIC1_DIVIDER		(*(unsigned int volatile *)REG_IIC1_DIVIDER)
#define     rIIC1_CMDR			(*(unsigned int volatile *)REG_IIC1_CMDR)
#define     rIIC1_SWR			(*(unsigned int volatile *)REG_IIC1_SWR)
#define     rIIC1_RxR			(*(unsigned int volatile *)REG_IIC1_RxR)
#define     rIIC1_TxR			(*(unsigned int volatile *)REG_IIC1_TxR)


/* 
  USI Control Registers
*/
#define     REG_USI_CNTRL	(USI_BA+0x00)  /* Control and Status Register */
#define     REG_USI_DIVIDER	(USI_BA+0x04)  /* Clock Divider Register */
#define     REG_USI_SSR		(USI_BA+0x08)  /* Slave Select Register */
#define     REG_USI_Rx0		(USI_BA+0x10)  /* Data Receive Register 0 */
#define     REG_USI_Rx1		(USI_BA+0x14)  /* Data Receive Register 1 */
#define     REG_USI_Rx2		(USI_BA+0x18)  /* Data Receive Register 2 */
#define     REG_USI_Rx3		(USI_BA+0x1C)  /* Data Receive Register 3 */
#define     REG_USI_Tx0		(USI_BA+0x10)  /* Data Transmit Register 0 */
#define     REG_USI_Tx1		(USI_BA+0x14)  /* Data Transmit Register 1 */
#define     REG_USI_Tx2		(USI_BA+0x18)  /* Data Transmit Register 2 */
#define     REG_USI_Tx3		(USI_BA+0x1C)  /* Data Transmit Register 3 */

#define     rUSI_CNTRL		(*(unsigned int volatile *)REG_USI_CNTRL)
#define     rUSI_DIVIDER	(*(unsigned int volatile *)REG_USI_DIVIDER)
#define     rUSI_SSR		(*(unsigned int volatile *)REG_USI_SSR)
#define     rUSI_Rx0		(*(unsigned int volatile *)REG_USI_Rx0)
#define     rUSI_Rx1		(*(unsigned int volatile *)REG_USI_Rx1)
#define     rUSI_Rx2		(*(unsigned int volatile *)REG_USI_Rx2)
#define     rUSI_Rx3		(*(unsigned int volatile *)REG_USI_Rx3)
#define     rUSI_Tx0		(*(unsigned int volatile *)REG_USI_Tx0)
#define     rUSI_Tx1		(*(unsigned int volatile *)REG_USI_Tx1)
#define     rUSI_Tx2		(*(unsigned int volatile *)REG_USI_Tx2)
#define     rUSI_Tx3		(*(unsigned int volatile *)REG_USI_Tx3)


/*
  PWM Control Registers
*/
#define     REG_PWM_PPR		(PWM_BA+0x00)  /* PWM Pre-scale Register 0 */
#define     REG_PWM_CSR		(PWM_BA+0x04)  /* PWM Clock Select Register */
#define     REG_PWM_PCR		(PWM_BA+0x08)  /* PWM Control Register */
#define     REG_PWM_CNR0	(PWM_BA+0x0C)  /* PWM Counter Register 0 */
#define     REG_PWM_CMR0	(PWM_BA+0x10)  /* PWM Comparator Register 0 */
#define     REG_PWM_PDR0	(PWM_BA+0x14)  /* PWM Data Register 0 */
#define     REG_PWM_CNR1	(PWM_BA+0x18)  /* PWM Counter Register 1 */
#define     REG_PWM_CMR1	(PWM_BA+0x1C)  /* PWM Comparator Register 1 */
#define     REG_PWM_PDR1	(PWM_BA+0x20)  /* PWM Data Register 1 */
#define     REG_PWM_CNR2	(PWM_BA+0x24)  /* PWM Counter Register 2 */
#define     REG_PWM_CMR2	(PWM_BA+0x28)  /* PWM Comparator Register 2 */
#define     REG_PWM_PDR2	(PWM_BA+0x2C)  /* PWM Data Register 2 */
#define     REG_PWM_CNR3	(PWM_BA+0x30)  /* PWM Counter Register 3 */
#define     REG_PWM_CMR3	(PWM_BA+0x34)  /* PWM Comparator Register 3 */
#define     REG_PWM_PDR3	(PWM_BA+0x38)  /* PWM Data Register 3 */
#define     REG_PWM_PIER	(PWM_BA+0x3C)  /* PWM Timer Interrupt Enable Register */
#define     REG_PWM_PIIR	(PWM_BA+0x40)  /* PWM Timer Interrupt Identification Register */

#define     rPWM_PPR		(*(unsigned int volatile *)REG_PWM_PPR)
#define     rPWM_CSR		(*(unsigned int volatile *)REG_PWM_CSR)
#define     rPWM_PCR		(*(unsigned int volatile *)REG_PWM_PCR)
#define     rPWM_CNR0		(*(unsigned int volatile *)REG_PWM_CNR0)
#define     rPWM_CMR0		(*(unsigned int volatile *)REG_PWM_CMR0)
#define     rPWM_PDR0		(*(unsigned int volatile *)REG_PWM_PDR0)
#define     rPWM_CNR1		(*(unsigned int volatile *)REG_PWM_CNR1)
#define     rPWM_CMR1		(*(unsigned int volatile *)REG_PWM_CMR1)
#define     rPWM_PDR1		(*(unsigned int volatile *)REG_PWM_PDR1)
#define     rPWM_CNR2		(*(unsigned int volatile *)REG_PWM_CNR2)
#define     rPWM_CMR2		(*(unsigned int volatile *)REG_PWM_CMR2)
#define     rPWM_PDR2		(*(unsigned int volatile *)REG_PWM_PDR2)
#define     rPWM_CNR3		(*(unsigned int volatile *)REG_PWM_CNR3)
#define     rPWM_CMR3		(*(unsigned int volatile *)REG_PWM_CMR3)
#define     rPWM_PDR3		(*(unsigned int volatile *)REG_PWM_PDR3)
#define     rPWM_PIER		(*(unsigned int volatile *)REG_PWM_PIER)
#define     rPWM_PIIR		(*(unsigned int volatile *)REG_PWM_PIIR)


/*
  KPI Control Registers
*/
#define     REG_KPI_CONF	(KPI_BA+0x00)  /* Keypad controller configuration Register */
#define     REG_KPI_3KCONF	(KPI_BA+0x04)  /* Keypad controller 3-keys configuration register */
#define     REG_KPI_LPCONF	(KPI_BA+0x08)  /* Keypad controller low power configuration register */
#define     REG_KPI_STATUS	(KPI_BA+0x0C)  /* Keypad controller status register */

#define     rKPI_CONF	(*(unsigned int volatile *)REG_KPI_CONF)
#define     rKPI_3KCONF	(*(unsigned int volatile *)REG_KPI_3KCONF)
#define     rKPI_LPCONF	(*(unsigned int volatile *)REG_KPI_LPCONF)
#define     rKPI_STATUS	(*(unsigned int volatile *)REG_KPI_STATUS)


/*
  PS2 Control Registers
*/
#define     REG_PS2_CMD0		(PS2_BA+0x000)  /* PS2 port 0 Host Controller Command Register */
#define     REG_PS2_STS0		(PS2_BA+0x004)  /* PS2 port 0 Host Controller Status Register */
#define     REG_PS2_SCANCODE0	(PS2_BA+0x008)  /* PS2 port 0 Host Controller Rx Scan Code Register */
#define     REG_PS2_ASCII0		(PS2_BA+0x00C)  /* PS2 port 0 Host Controller Rx ASCII Code */
#define     REG_PS2_CMD1		(PS2_BA+0x100)  /* PS2 port 1 Host Controller Command Register */
#define     REG_PS2_STS1		(PS2_BA+0x104)  /* PS2 port 1 Host Controller Status Register */
#define     REG_PS2_SCANCODE1	(PS2_BA+0x108)  /* PS2 port 1 Host Controller Rx Scan Code Register */
#define     REG_PS2_ASCII1		(PS2_BA+0x10C)  /* PS2 port 1 Host Controller Rx ASCII Code */
 
#define     rPS2_CMD0			(*(unsigned int volatile *)REG_PS2_CMD0)
#define     rPS2_STS0			(*(unsigned int volatile *)REG_PS2_STS0)
#define     rPS2_SCANCODE0		(*(unsigned int volatile *)REG_PS2_SCANCODE0)
#define     rPS2_ASCII0			(*(unsigned int volatile *)REG_PS2_ASCII0)
#define     rPS2_CMD1			(*(unsigned int volatile *)REG_PS2_CMD1)
#define     rPS2_STS1			(*(unsigned int volatile *)REG_PS2_STS1)
#define     rPS2_SCANCODE1		(*(unsigned int volatile *)REG_PS2_SCANCODE1)
#define     rPS2_ASCII1			(*(unsigned int volatile *)REG_PS2_ASCII1)
 
 
/*
  ADC Control Registers
*/
#define     REG_ADC_CON		(ADC_BA+0x00)  /* ADC control register */
#define     REG_ADC_TSC		(ADC_BA+0x04)  /* Touch screen control register */
#define     REG_ADC_DLY		(ADC_BA+0x08)  /* ADC delay register */
#define     REG_ADC_XDATA	(ADC_BA+0x0C)  /* ADC XDATA register */
#define     REG_ADC_YDATA	(ADC_BA+0x10)  /* ADC YDATA register */
#define     REG_ADC_LV_CON	(ADC_BA+0x14)  /* Low Voltage Detector Control register */
#define     REG_ADC_LV_STS	(ADC_BA+0x18)  /* Low Voltage Detector Status register */

#define     rADC_CON		(*(unsigned int volatile *)REG_ADC_CON)
#define     rADC_TSC		(*(unsigned int volatile *)REG_ADC_TSC)
#define     rADC_DLY		(*(unsigned int volatile *)REG_ADC_DLY)
#define     rADC_XDATA		(*(unsigned int volatile *)REG_ADC_XDATA)
#define     rADC_YDATA		(*(unsigned int volatile *)REG_ADC_YDATA)
#define     rADC_LV_CON		(*(unsigned int volatile *)REG_ADC_LV_CON)
#define     rADC_LV_STS		(*(unsigned int volatile *)REG_ADC_LV_STS)


/*
  CAN0 Control Register
*/
#define     REG_CAN0_CON		(CAN0_BA+0x00)  /* Control register */
#define     REG_CAN0_STATUS		(CAN0_BA+0x04)  /* Status register */
#define     REG_CAN0_ERR		(CAN0_BA+0x08)  /* Error Counter */
#define     REG_CAN0_BTIME		(CAN0_BA+0x0C)  /* Bit Timing register */
#define     REG_CAN0_IIDR		(CAN0_BA+0x10)  /* Interrupt Identifier register */
#define     REG_CAN0_TEST		(CAN0_BA+0x14)  /* Test register */
#define     REG_CAN0_BRPE		(CAN0_BA+0x18)  /* BRP Extension register */
#define     REG_CAN0_IF1_CREQ	(CAN0_BA+0x20)  /* IF1 Command Request register */
#define     REG_CAN0_IF2_CREQ	(CAN0_BA+0x80)  /* IF2 Command Request register */
#define     REG_CAN0_IF1_CMASK	(CAN0_BA+0x24)  /* IF1 Command Mask register */
#define     REG_CAN0_IF2_CMASK	(CAN0_BA+0x84)  /* IF2 Command Mask register */
#define     REG_CAN0_IF1_MASK1	(CAN0_BA+0x28)  /* IF1 Mask 1 register */
#define     REG_CAN0_IF2_MASK1	(CAN0_BA+0x88)  /* IF2 Mask 1 register */
#define     REG_CAN0_IF1_MASK2	(CAN0_BA+0x2C)  /* IF1 Mask 2 register */
#define     REG_CAN0_IF2_MASK2	(CAN0_BA+0x8C)  /* IF2 Mask 2 register */
#define     REG_CAN0_IF1_ARB1	(CAN0_BA+0x30)  /* IF1 Arbitration 1 register */
#define     REG_CAN0_IF2_ARB1	(CAN0_BA+0x90)  /* IF2 Arbitration 1 register */
#define     REG_CAN0_IF1_ARB2	(CAN0_BA+0x34)  /* IF1 Arbitration 2 register */
#define     REG_CAN0_IF2_ARB2	(CAN0_BA+0x94)  /* IF2 Arbitration 2 register */
#define     REG_CAN0_IF1_MCON	(CAN0_BA+0x38)  /* IF1 Message Control register */
#define     REG_CAN0_IF2_MCON	(CAN0_BA+0x98)  /* IF2 Message Control register */
#define     REG_CAN0_IF1_DAT_A1	(CAN0_BA+0x3C)  /* IF1 Data A1 register */
#define     REG_CAN0_IF1_DAT_A2	(CAN0_BA+0x40)  /* IF1 Data A2 register */
#define     REG_CAN0_IF1_DAT_B1	(CAN0_BA+0x44)  /* IF1 Data B1 register */
#define     REG_CAN0_IF1_DAT_B2	(CAN0_BA+0x48)  /* IF1 Data B2 register */
#define     REG_CAN0_IF2_DAT_A1	(CAN0_BA+0x9C)  /* IF2 Data A1 register */
#define     REG_CAN0_IF2_DAT_A2	(CAN0_BA+0xA0)  /* IF2 Data A2 register */
#define     REG_CAN0_IF2_DAT_B1	(CAN0_BA+0xA4)  /* IF2 Data B1 register */
#define     REG_CAN0_IF2_DAT_B2	(CAN0_BA+0xA8)  /* IF2 Data B2 register */
#define     REG_CAN0_TXREQ1		(CAN0_BA+0x100) /* Transmission Request 1 */
#define     REG_CAN0_TXREQ2		(CAN0_BA+0x104) /* Transmission Request 2 */
#define     REG_CAN0_NDAT1		(CAN0_BA+0x120) /* New Data Registers 1 */
#define     REG_CAN0_NDAT2		(CAN0_BA+0x124) /* New Data Registers 2 */
#define     REG_CAN0_IPND1		(CAN0_BA+0x140) /* Interrupt Pending Registers 1 */
#define     REG_CAN0_IPND2		(CAN0_BA+0x144) /* Interrupt Pending Registers 2 */
#define     REG_CAN0_MVLD1		(CAN0_BA+0x160) /* Message Valid Registers 1 */
#define     REG_CAN0_MVLD2		(CAN0_BA+0x164) /* Message Valid Registers 2 */
#define     REG_CAN0_WU_EN		(CAN0_BA+0x168) /* Wake Up Function Enable */
#define     REG_CAN0_WU_STATUS	(CAN0_BA+0x16C) /* Wake Up Function Status */

#define     rCAN0_CON			(*(unsigned int volatile *)REG_CAN0_CON)
#define     rCAN0_STATUS		(*(unsigned int volatile *)REG_CAN0_STATUS)
#define     rCAN0_ERR			(*(unsigned int volatile *)REG_CAN0_ERR)
#define     rCAN0_BTIME			(*(unsigned int volatile *)REG_CAN0_BTIME)
#define     rCAN0_IIDR			(*(unsigned int volatile *)REG_CAN0_IIDR)
#define     rCAN0_TEST			(*(unsigned int volatile *)REG_CAN0_TEST)
#define     rCAN0_BRPE			(*(unsigned int volatile *)REG_CAN0_BRPE)
#define     rCAN0_IF1_CREQ		(*(unsigned int volatile *)REG_CAN0_IF1_CREQ)
#define     rCAN0_IF2_CREQ		(*(unsigned int volatile *)REG_CAN0_IF2_CREQ)
#define     rCAN0_IF1_CMASK		(*(unsigned int volatile *)REG_CAN0_IF1_CMASK)
#define     rCAN0_IF2_CMASK		(*(unsigned int volatile *)REG_CAN0_IF2_CMASK)
#define     rCAN0_IF1_MASK1		(*(unsigned int volatile *)REG_CAN0_IF1_MASK1)
#define     rCAN0_IF2_MASK1		(*(unsigned int volatile *)REG_CAN0_IF2_MASK1)
#define     rCAN0_IF1_MASK2		(*(unsigned int volatile *)REG_CAN0_IF1_MASK2)
#define     rCAN0_IF2_MASK2		(*(unsigned int volatile *)REG_CAN0_IF2_MASK2)
#define     rCAN0_IF1_ARB1		(*(unsigned int volatile *)REG_CAN0_IF1_ARB1)
#define     rCAN0_IF2_ARB1		(*(unsigned int volatile *)REG_CAN0_IF2_ARB1)
#define     rCAN0_IF1_ARB2		(*(unsigned int volatile *)REG_CAN0_IF1_ARB2)
#define     rCAN0_IF2_ARB2		(*(unsigned int volatile *)REG_CAN0_IF2_ARB2)
#define     rCAN0_IF1_MCON		(*(unsigned int volatile *)REG_CAN0_IF1_MCON)
#define     rCAN0_IF2_MCON		(*(unsigned int volatile *)REG_CAN0_IF2_MCON)
#define     rCAN0_IF1_DAT_A1	(*(unsigned int volatile *)REG_CAN0_IF1_DAT_A1)
#define     rCAN0_IF1_DAT_A2	(*(unsigned int volatile *)REG_CAN0_IF1_DAT_A2)
#define     rCAN0_IF1_DAT_B1	(*(unsigned int volatile *)REG_CAN0_IF1_DAT_B1)
#define     rCAN0_IF1_DAT_B2	(*(unsigned int volatile *)REG_CAN0_IF1_DAT_B2)
#define     rCAN0_IF2_DAT_A1	(*(unsigned int volatile *)REG_CAN0_IF2_DAT_A1)
#define     rCAN0_IF2_DAT_A2	(*(unsigned int volatile *)REG_CAN0_IF2_DAT_A2)
#define     rCAN0_IF2_DAT_B1	(*(unsigned int volatile *)REG_CAN0_IF2_DAT_B1)
#define     rCAN0_IF2_DAT_B2	(*(unsigned int volatile *)REG_CAN0_IF2_DAT_B2)
#define     rCAN0_TXREQ1		(*(unsigned int volatile *)REG_CAN0_TXREQ1)
#define     rCAN0_TXREQ2		(*(unsigned int volatile *)REG_CAN0_TXREQ2)
#define     rCAN0_NDAT1			(*(unsigned int volatile *)REG_CAN0_NDAT1)
#define     rCAN0_NDAT2			(*(unsigned int volatile *)REG_CAN0_NDAT2)
#define     rCAN0_IPND1			(*(unsigned int volatile *)REG_CAN0_IPND1)
#define     rCAN0_IPND2			(*(unsigned int volatile *)REG_CAN0_IPND2)
#define     rCAN0_MVLD1			(*(unsigned int volatile *)REG_CAN0_MVLD1)
#define     rCAN0_MVLD2			(*(unsigned int volatile *)REG_CAN0_MVLD2)
#define     rCAN0_WU_EN			(*(unsigned int volatile *)REG_CAN0_WU_EN)
#define     rCAN0_WU_STATUS		(*(unsigned int volatile *)REG_CAN0_WU_STATUS)

/*
  CAN1 Control Register
*/
#define     REG_CAN1_CON		(CAN1_BA+0x00)  /* Control register */
#define     REG_CAN1_STATUS		(CAN1_BA+0x04)  /* Status register */
#define     REG_CAN1_ERR		(CAN1_BA+0x08)  /* Error Counter */
#define     REG_CAN1_BTIME		(CAN1_BA+0x0C)  /* Bit Timing register */
#define     REG_CAN1_IIDR		(CAN1_BA+0x10)  /* Interrupt Identifier register */
#define     REG_CAN1_TEST		(CAN1_BA+0x14)  /* Test register */
#define     REG_CAN1_BRPE		(CAN1_BA+0x18)  /* BRP Extension register */
#define     REG_CAN1_IF1_CREQ	(CAN1_BA+0x20)  /* IF1 Command Request register */
#define     REG_CAN1_IF2_CREQ	(CAN1_BA+0x80)  /* IF2 Command Request register */
#define     REG_CAN1_IF1_CMASK	(CAN1_BA+0x24)  /* IF1 Command Mask register */
#define     REG_CAN1_IF2_CMASK	(CAN1_BA+0x84)  /* IF2 Command Mask register */
#define     REG_CAN1_IF1_MASK1	(CAN1_BA+0x28)  /* IF1 Mask 1 register */
#define     REG_CAN1_IF2_MASK1	(CAN1_BA+0x88)  /* IF2 Mask 1 register */
#define     REG_CAN1_IF1_MASK2	(CAN1_BA+0x2C)  /* IF1 Mask 2 register */
#define     REG_CAN1_IF2_MASK2	(CAN1_BA+0x8C)  /* IF2 Mask 2 register */
#define     REG_CAN1_IF1_ARB1	(CAN1_BA+0x30)  /* IF1 Arbitration 1 register */
#define     REG_CAN1_IF2_ARB1	(CAN1_BA+0x90)  /* IF2 Arbitration 1 register */
#define     REG_CAN1_IF1_ARB2	(CAN1_BA+0x34)  /* IF1 Arbitration 2 register */
#define     REG_CAN1_IF2_ARB2	(CAN1_BA+0x94)  /* IF2 Arbitration 2 register */
#define     REG_CAN1_IF1_MCON	(CAN1_BA+0x38)  /* IF1 Message Control register */
#define     REG_CAN1_IF2_MCON	(CAN1_BA+0x98)  /* IF2 Message Control register */
#define     REG_CAN1_IF1_DAT_A1	(CAN1_BA+0x3C)  /* IF1 Data A1 register */
#define     REG_CAN1_IF1_DAT_A2	(CAN1_BA+0x40)  /* IF1 Data A2 register */
#define     REG_CAN1_IF1_DAT_B1	(CAN1_BA+0x44)  /* IF1 Data B1 register */
#define     REG_CAN1_IF1_DAT_B2	(CAN1_BA+0x48)  /* IF1 Data B2 register */
#define     REG_CAN1_IF2_DAT_A1	(CAN1_BA+0x9C)  /* IF2 Data A1 register */
#define     REG_CAN1_IF2_DAT_A2	(CAN1_BA+0xA0)  /* IF2 Data A2 register */
#define     REG_CAN1_IF2_DAT_B1	(CAN1_BA+0xA4)  /* IF2 Data B1 register */
#define     REG_CAN1_IF2_DAT_B2	(CAN1_BA+0xA8)  /* IF2 Data B2 register */
#define     REG_CAN1_TXREQ1		(CAN1_BA+0x100) /* Transmission Request 1 */
#define     REG_CAN1_TXREQ2		(CAN1_BA+0x104) /* Transmission Request 2 */
#define     REG_CAN1_NDAT1		(CAN1_BA+0x120) /* New Data Registers 1 */
#define     REG_CAN1_NDAT2		(CAN1_BA+0x124) /* New Data Registers 2 */
#define     REG_CAN1_IPND1		(CAN1_BA+0x140) /* Interrupt Pending Registers 1 */
#define     REG_CAN1_IPND2		(CAN1_BA+0x144) /* Interrupt Pending Registers 2 */
#define     REG_CAN1_MVLD1		(CAN1_BA+0x160) /* Message Valid Registers 1 */
#define     REG_CAN1_MVLD2		(CAN1_BA+0x164) /* Message Valid Registers 2 */
#define     REG_CAN1_WU_EN		(CAN1_BA+0x168) /* Wake Up Function Enable */
#define     REG_CAN1_WU_STATUS	(CAN1_BA+0x16C) /* Wake Up Function Status */

#define     rCAN1_CON			(*(unsigned int volatile *)REG_CAN1_CON)
#define     rCAN1_STATUS		(*(unsigned int volatile *)REG_CAN1_STATUS)
#define     rCAN1_ERR			(*(unsigned int volatile *)REG_CAN1_ERR)
#define     rCAN1_BTIME			(*(unsigned int volatile *)REG_CAN1_BTIME)
#define     rCAN1_IIDR			(*(unsigned int volatile *)REG_CAN1_IIDR)
#define     rCAN1_TEST			(*(unsigned int volatile *)REG_CAN1_TEST)
#define     rCAN1_BRPE			(*(unsigned int volatile *)REG_CAN1_BRPE)
#define     rCAN1_IF1_CREQ		(*(unsigned int volatile *)REG_CAN1_IF1_CREQ)
#define     rCAN1_IF2_CREQ		(*(unsigned int volatile *)REG_CAN1_IF2_CREQ)
#define     rCAN1_IF1_CMASK		(*(unsigned int volatile *)REG_CAN1_IF1_CMASK)
#define     rCAN1_IF2_CMASK		(*(unsigned int volatile *)REG_CAN1_IF2_CMASK)
#define     rCAN1_IF1_MASK1		(*(unsigned int volatile *)REG_CAN1_IF1_MASK1)
#define     rCAN1_IF2_MASK1		(*(unsigned int volatile *)REG_CAN1_IF2_MASK1)
#define     rCAN1_IF1_MASK2		(*(unsigned int volatile *)REG_CAN1_IF1_MASK2)
#define     rCAN1_IF2_MASK2		(*(unsigned int volatile *)REG_CAN1_IF2_MASK2)
#define     rCAN1_IF1_ARB1		(*(unsigned int volatile *)REG_CAN1_IF1_ARB1)
#define     rCAN1_IF2_ARB1		(*(unsigned int volatile *)REG_CAN1_IF2_ARB1)
#define     rCAN1_IF1_ARB2		(*(unsigned int volatile *)REG_CAN1_IF1_ARB2)
#define     rCAN1_IF2_ARB2		(*(unsigned int volatile *)REG_CAN1_IF2_ARB2)
#define     rCAN1_IF1_MCON		(*(unsigned int volatile *)REG_CAN1_IF1_MCON)
#define     rCAN1_IF2_MCON		(*(unsigned int volatile *)REG_CAN1_IF2_MCON)
#define     rCAN1_IF1_DAT_A1	(*(unsigned int volatile *)REG_CAN1_IF1_DAT_A1)
#define     rCAN1_IF1_DAT_A2	(*(unsigned int volatile *)REG_CAN1_IF1_DAT_A2)
#define     rCAN1_IF1_DAT_B1	(*(unsigned int volatile *)REG_CAN1_IF1_DAT_B1)
#define     rCAN1_IF1_DAT_B2	(*(unsigned int volatile *)REG_CAN1_IF1_DAT_B2)
#define     rCAN1_IF2_DAT_A1	(*(unsigned int volatile *)REG_CAN1_IF2_DAT_A1)
#define     rCAN1_IF2_DAT_A2	(*(unsigned int volatile *)REG_CAN1_IF2_DAT_A2)
#define     rCAN1_IF2_DAT_B1	(*(unsigned int volatile *)REG_CAN1_IF2_DAT_B1)
#define     rCAN1_IF2_DAT_B2	(*(unsigned int volatile *)REG_CAN1_IF2_DAT_B2)
#define     rCAN1_TXREQ1		(*(unsigned int volatile *)REG_CAN1_TXREQ1)
#define     rCAN1_TXREQ2		(*(unsigned int volatile *)REG_CAN1_TXREQ2)
#define     rCAN1_NDAT1			(*(unsigned int volatile *)REG_CAN1_NDAT1)
#define     rCAN1_NDAT2			(*(unsigned int volatile *)REG_CAN1_NDAT2)
#define     rCAN1_IPND1			(*(unsigned int volatile *)REG_CAN1_IPND1)
#define     rCAN1_IPND2			(*(unsigned int volatile *)REG_CAN1_IPND2)
#define     rCAN1_MVLD1			(*(unsigned int volatile *)REG_CAN1_MVLD1)
#define     rCAN1_MVLD2			(*(unsigned int volatile *)REG_CAN1_MVLD2)
#define     rCAN1_WU_EN			(*(unsigned int volatile *)REG_CAN1_WU_EN)
#define     rCAN1_WU_STATUS		(*(unsigned int volatile *)REG_CAN1_WU_STATUS)

/**********************************************************************************************************
 *                                                               
 * 24. OTP Control Registers
 *
 **********************************************************************************************************/
#define     REG_OTP_KEYEN	  (OTP_BA+0x00)  /* Key Enable Register */
#define     REG_OTP_OPTION	(OTP_BA+0x0C)  /* OTP Option Register */
#define     REG_OTP_VALUE0	(OTP_BA+0x10)  /* OTP Value 0 Register */
#define     REG_OTP_VALUE1	(OTP_BA+0x14)  /* OTP Value 1 Register */
#define     REG_OTP_VALUE2	(OTP_BA+0x18)  /* OTP Value 2 Register */
#define     REG_OTP_VALUE3	(OTP_BA+0x1C)  /* OTP Value 3 Register */
#define     REG_OTP_VALUE4	(OTP_BA+0x20)  /* OTP Value 4 Register */
#define     REG_OTP_VALUE5	(OTP_BA+0x24)  /* OTP Value 5 Register */
#define     REG_OTP_VALUE6	(OTP_BA+0x28)  /* OTP Value 6 Register */
#define     REG_OTP_VALUE7	(OTP_BA+0x2C)  /* OTP Value 7 Register */
#define     REG_OTP_CYCLE	  (OTP_BA+0x30)  /* Cycle Count Register */
#define     REG_OTP_MODE	  (OTP_BA+0x34)  /* OTP Mode Register */
#define     REG_OTP_START	  (OTP_BA+0x38)  /* OTP Program Start Register */
#define     REG_OTP_STATUS	(OTP_BA+0x40)  /* OTP Status Register */
#define     REG_OTP_PROTECT	(OTP_BA+0x50)  /* OTP Protect Register */


/**********************************************************************************************************
 *                                                               
 * 25. Crypto Engine Control Registers
 *
 **********************************************************************************************************/
#define     REG_CRPT_IPSEC_INT_EN	(CRPT_BA+0x0000)  /* IPsec interrupt enable */
#define     REG_CRPT_IPSEC_INT_FLAG	(CRPT_BA+0x0004)  /* IPsec interrupt flag */
#define     REG_CRPT_AES_CTL		(CRPT_BA+0x0100)  /* AES engine setting */
#define     REG_CRPT_AES_KEY0		(CRPT_BA+0x0110)  /* AES Key Word 0 Register */
#define     REG_CRPT_AES_KEY1		(CRPT_BA+0x0114)  /* AES Key Word 1 Register */
#define     REG_CRPT_AES_KEY2		(CRPT_BA+0x0118)  /* AES Key Word 2 Register */
#define     REG_CRPT_AES_KEY3		(CRPT_BA+0x011C)  /* AES Key Word 3 Register */
#define     REG_CRPT_AES_KEY4		(CRPT_BA+0x0120)  /* AES Key Word 4 Register */
#define     REG_CRPT_AES_KEY5		(CRPT_BA+0x0124)  /* AES Key Word 5 Register */
#define     REG_CRPT_AES_KEY6		(CRPT_BA+0x0128)  /* AES Key Word 6 Register */
#define     REG_CRPT_AES_KEY7		(CRPT_BA+0x012C)  /* AES Key Word 7 Register */
#define     REG_CRPT_AES_IV0		(CRPT_BA+0x0130)  /* AES Initial Vector Word 0 Register */
#define     REG_CRPT_AES_IV1		(CRPT_BA+0x0134)  /* AES Initial Vector Word 1 Register */
#define     REG_CRPT_AES_IV2		(CRPT_BA+0x0138)  /* AES Initial Vector Word 2 Register */
#define     REG_CRPT_AES_IV3		(CRPT_BA+0x013C)  /* AES Initial Vector Word 3 Register */
#define     REG_CRPT_AES_SADR		(CRPT_BA+0x0140)  /* AES Source Address Register */
#define     REG_CRPT_AES_DADR		(CRPT_BA+0x0144)  /* AES Destination Address Register */
#define     REG_CRPT_AES_CNT		(CRPT_BA+0x0148)  /* AES Byte Count Register */


/*! Unlock protected register */
#define UNLOCKREG(x)		do{rREGWRPRTR = 0x59; rREGWRPRTR = 0x16; rREGWRPRTR = 0x88;}while(rREGWRPRTR == 0x00)
/*! Lock protected register */
#define LOCKREG(x)          do{rREGWRPRTR = 0x00;}while(0)

#endif /* _NUC970_H */
