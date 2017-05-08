/****************************************************************************
 *                                                                          *
 * Copyright (c) 2008 Nuvoton Technology. All rights reserved.              *
 *                                                                          *
 ***************************************************************************/

/****************************************************************************
 *
 * FILENAME
 *     wberrcode.h
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *	   The Error Codes returned by Nuvoton S/W library are defined as the following
 *     format:
 *              0xFFFFXX## (XX : Module ID, ##:Error Number)
 *
 *     The Module IDs for each S/W library are defined in this file. The error
 *     numbers are defined by S/W library according to its requirement.
 *
 *
 * DATA STRUCTURES
 *     None
 *
 * FUNCTIONS
 *     None
 *
 * HISTORY
 *     2008-06-26  Ver 1.0 draft by Min-Nan Cheng
 * REMARK
 *     None
 **************************************************************************/
#ifndef _WBERRCODE_H_
#define _WBERRCODE_H_

/* Error Code's Module ID */
#define FMI_ERR_ID			0xFFFF0100	/* FMI library ID */
#define ADO_ERR_ID			0xFFFF0200	/* Audio library ID */
#define USB_ERR_ID			0xFFFF0300	/* USB Device library ID */
#define GDMA_ERR_ID			0xFFFF0400	/* GDMA library ID */
#define ATA_ERR_ID			0xFFFF0500	/* ATA library ID */
#define DMAC_ERR_ID			0xFFFF0600	/* DMA library ID */
#define TMR_ERR_ID			0xFFFF0700	/* Timer library ID */
#define GE_ERR_ID			0xFFFF0800	/* 2D graphics library ID */
#define AIC_ERR_ID			0xFFFF0900	/* AIC library ID */
#define SYSLIB_ERR_ID		0xFFFF0A00	/* System library ID */
#define USBO_ERR_ID			0xFFFF0C00	/* OHCI library ID */
#define USBH_ERR_ID			0xFFFF0D00	/* USB Host library ID */
#define RTC_ERR_ID			0xFFFF0E00	/* RTC library ID */
#define GPIO_ERR_ID			0xFFFF0F00	/* GPIO library ID */
#define SMARTCARD_ERR_ID	0xFFFF1000	/* Smartcard library ID */
#define I2C_ERR_ID			0xFFFF1100	/* I2C library ID */
#define SPI_ERR_ID			0xFFFF1200	/* SPI library ID */
#define PWM_ERR_ID			0xFFFF1300	/* PWM library ID */
#define KEYPAD_ERR_ID		0xFFFF1400	/* KEYPAD library ID*/
#define PS2_ERR_ID			0xFFFF1500	/* PS2 library ID */
#define MAC_ERR_ID			0xFFFF1600	/* MAC library ID */
#define UART_ERR_ID			0xFFFF1700	/* UART library ID */
#define LCD_ERR_ID			0xFFFF1800	/* LCD library ID */
#define USI_ERR_ID			0xFFFF1900	/* USI library ID */
#define ADC_ERR_ID			0xFFFF1A00	/* ADC library ID */


#define FAT_ERR_ID			0xFFFF8200	/* FAT file system library ID */
#define AACE_ERR_ID			0xFFFF8400	/* AAC encoder ID */
#define AACD_ERR_ID			0xFFFF8600	/* AAC/AAC+ decoder ID */
#define AMRNE_ERR_ID		0xFFFF8800	/* AMR NB encoder ID */
#define AMRND_ERR_ID		0xFFFF8A00	/* AMR NB decoder ID */
#define AMRWD_ERR_ID		0xFFFF8C00	/* AMR WB decoder ID */
#define MP3_ERR_ID			0xFFFF9000	/* MP3 decoder ID */
#define WMA_ERR_ID			0xFFFF9200	/* WMA decoder ID */
#define ADPCM_ERR_ID		0xFFFF9400	/* ADPCM encoder/decoder ID */

#define FONT_ERR_ID     	0xFFFFA000  /* FONT library ID */
#define FILTER_ERR_ID   	0xFFFFA200  /* Image Filter library ID */
#define PBRIDGE_ERR_ID		0xFFFFA400	/* PictBridge ID */

/* Macros for handing error code */
#define WBAPI_RESULT_IS_ERROR(value)	((value) < 0) ? 1 : 0		/* 1:error, 0:non-err */
#define WBAPI_GET_ERROR_ID(value)		((value) & 0x0000FF00) >> 8 /* get Module ID of error code */
#define WBAPI_GET_ERROR_NUMBER(value)	((value) & 0x000000FF) 		/* get Error Number of error code */

/* Error Number defined by XXX library */

#endif /* _WBERRCODE_H */
