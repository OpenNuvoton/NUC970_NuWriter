/*-----------------------------------------------------------------------------------*/
/* Winbond Electronics Corporation confidential                                      */
/*                                                                                   */
/* Copyright (c) 2007 by Winbond Electronics Corporation                             */
/* All rights reserved                                                               */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/
/* File Contents:                                                                    */
/*   usbd.h                                                                          */
/*                                                                                   */
/* This file contains:                                                               */
/*                                                                                   */
/* Project:                                                                          */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/
#ifndef _USBD_H
#define _USBD_H
#include <stdio.h>
#include <string.h>
#include "wbio.h"
#include "wblib.h"
#include "usbd.h"
#include "nuc970_reg.h"

#define EPSTADDR				0x400
/*
 * Standard requests
 */
#define USBR_GET_STATUS					0x00
#define USBR_CLEAR_FEATURE			0x01
#define USBR_SET_FEATURE				0x03
#define USBR_SET_ADDRESS		    0x05
#define USBR_GET_DESCRIPTOR		  0x06
#define USBR_SET_DESCRIPTOR		  0x07
#define USBR_GET_CONFIGURATION	0x08
#define USBR_SET_CONFIGURATION	0x09
#define USBR_GET_INTERFACE		  0x0A
#define USBR_SET_INTERFACE		  0x0B
#define USBR_SYNCH_FRAME		    0x0C

/*
 * Descriptor types
 */
#define USB_DT_DEVICE			  0x01
#define USB_DT_CONFIG			  0x02
#define USB_DT_STRING			  0x03
#define USB_DT_INTERFACE		0x04
#define USB_DT_ENDPOINT			0x05
#define USB_DT_QUALIFIER		0x06
#define USB_DT_OSCONFIG			0x07
#define USB_DT_IFPOWER			0x08

//USB FEATURE SELECTOR			value
#define DEVICE_REMOTE_WAKEUP	1
#define ENDPOINT_HALT			    0
#define TEST_MODE				      2

//USB TEST MODES
#define TEST_J					    0x01
#define TEST_K					    0x02
#define TEST_SE0_NAK			  0x03
#define TEST_PACKET				  0x04
#define TEST_FORCE_ENABLE		0x05

//Driver definition of tokens
#define OUT_TOK		0x00
#define IN_TOK		0x01
#define SUP_TOK 	0x02
#define PING_TOK	0x03
#define NO_TOK		0x04

//Bit Definitions of IRQ_ENB/STAT register
#define	IRQ_USB_STAT		0x01
#define IRQ_CEP				  0x02
#define IRQ_NCEP			  0x3ffc   

//Definition of Bits in USB_IRQ_STS register

#define USB_SOF			  0x01	
#define USB_RST_STS		0x02
#define	USB_RESUME		0x04
#define	USB_SUS_REQ		0x08
#define	USB_HS_SETTLE	0x10
#define	USB_DMA_REQ		0x20
#define USABLE_CLK		0x40
#define	USB_FLOAT_DET	0x100

//Definition of Bits in USB_OPER register
#define USB_GEN_RES     0x1
#define USB_HS		    0x2
#define USB_CUR_SPD_HS  0x4

//Definition of Bits in CEP_IRQ_STS register

#define CEP_SUPTOK	 	  0x0001
#define CEP_SUPPKT		  0x0002
#define CEP_OUT_TOK		  0x0004
#define CEP_IN_TOK		  0x0008
#define CEP_PING_TOK	  0x0010
#define CEP_DATA_TXD	  0x0020
#define CEP_DATA_RXD	  0x0040
#define CEP_NAK_SENT	  0x0080
#define CEP_STALL_SENT	0x0100
#define CEP_USB_ERR		  0x0200
#define CEP_STS_END		  0x0400
#define CEP_BUFF_FULL	  0x0800
#define CEP_BUFF_EMPTY	0x1000


//Definition of Bits in CEP_CTRL_STS register
#define CEP_NAK_CLEAR		0x00  //writing zero clears the nak bit
#define CEP_SEND_STALL		0x02


//Definition of Bits in EP_IRQ_STS register

#define EP_BUFF_FULL	    0x001
#define EP_BUFF_EMPTY	    0x002
#define EP_SHORT_PKT	    0x004
#define EP_DATA_TXD		    0x008
#define EP_DATA_RXD		    0x010
#define EP_OUT_TOK		    0x020
#define EP_IN_TOK		      0x040
#define EP_PING_TOK		    0x080
#define EP_NAK_SENT		    0x100
#define EP_STALL_SENT	    0x200
#define EP_NYET_SENT		  0x400
#define EP_USB_ERR			  0x800
#define EP_OUT_SHORT_PKT	0x1000


//Bit Definitons of EP_RSP_SC Register
#define EP_BUFF_FLUSH   0x01
#define EP_MODE         0x06
#define EP_MODE_AUTO	  0x01
#define EP_MODE_MAN 	  0x02
#define EP_MODE_FLY		  0x03
#define EP_TOGGLE		    0x8
#define EP_HALT			    0x10
#define EP_ZERO_IN      0x20
#define EP_PKT_END      0x40

//Bit Definitons of EP_CFG Register
#define EP_VALID		  0x01
#define EP_TYPE			  0x06 //2-bit size	
#define EP_TYPE_BLK		0x01
#define EP_TYPE_INT		0x02
#define EP_TYPE_ISO		0x03
#define EP_DIR		  	0x08
#define EP_NO			    0xf0 //4-bit size


/* Define Endpoint feature */
#define Ep_In        0x01
#define Ep_Out       0x00
#define Ep_Bulk      0x01
#define Ep_Int       0x02
#define Ep_Iso       0x03
#define EP_A         0x00
#define EP_B         0x01
#define EP_C         0x02
#define EP_D         0x03
#define EP_E         0x04
#define EP_F         0x05

typedef struct usb_cmd
{
	UINT8	bmRequestType;
	UINT8	bRequest;
	UINT16	wValue;
	UINT16	wIndex;
	UINT16	wLength;
}	USB_CMD_T;

typedef struct
{
	UINT8 EP_Num;
	UINT8 EP_Dir;
	UINT8 EP_Type;
}	USB_EP_Inf_T;

/* for mass storage */
typedef struct Disk_Par_Inf {
	UINT32	partition_size,
			data_location,
			bpb_location,
			fat_location,
			rootdir_location,
			free_size;
	UINT16	rootdirentryno,
			totalcluster,
			NumCyl;
	UINT8	NumHead,
			NumSector,
			capacity,
			fatcopyno,
			fatsectorno,
			fat16flg;
	} Disk_Par_Info;

/* extern flags */
extern UINT8 volatile bulkonlycmd;
extern UINT8 _usbd_haltep1;
extern UINT8 _usbd_haltep2;

#define HALT 	1
#define UNHALT 	0

/* extern functions */
void ReceiveCBW(UINT32 );
void SDRAM2USB_Int(UINT32  ,UINT32 );
void SDRAM2USB_Bulk(UINT32  ,UINT32 );
void USB2SDRAM_Bulk(UINT32  ,UINT32 );
void FshBuf2CBW(void);
void CSW2FshBuf(void);
void Inquiry_Command(void);
void ModeSense10_Command(void);
void ModeSense6_Command(void);
void ModeSense_Command(void);
void ReqSen_Command(void);
void ModeSel_Command(void);

void RdFmtCap_Command(void);
void RdCurCap_Command(void);
void Rd10_Command(void);
void Wt10_Command(void);

void udcMassBulk(void);
UINT8 udcFlashInit(void);

void usbdHighSpeedInit(void);
void usbdFullSpeedInit(void);

void udcInit(void);  

/* RAM Disk Function Prototype */
void DRAM_Identify(UINT8 cap);


void UsbResetDma(void);
//---------------------------------------

/* It's recommended that don't printf any message. */
//#define MSG_DEBUG_EN
#ifndef MSG_DEBUG_EN
#define MSG_DEBUG(...)
#define MSG_WARNING(...)
#define MSG_ERROR(...)
#else
#define MSG_DEBUG			sysprintf
#define MSG_WARNING			sysprintf
#define MSG_ERROR			sysprintf
#endif

#define DOWNLOAD_BASE	0x10000
#define NON_CACHE		0x80000000

// for upgrade
#define USBD_FLASH_SDRAM		  0x0
//#define USBD_FLASH_NOR			  0x1
//#define USBD_FLASH_NOR_RAW		0x2
#define USBD_FLASH_NAND			  0x3
#define USBD_FLASH_NAND_RAW		0x4
#define USBD_FLASH_MMC			  0x5 
#define USBD_FLASH_MMC_RAW		0x6
#define USBD_FLASH_SPI			  0x7
#define USBD_FLASH_SPI_RAW		0x8
#define USBD_MTP              0x9
#define USBD_INFO							0xA
//#define USBD_PACK							0xA

// #define USBD_NOR_IMAGE_LIST		0x9
// #define USBD_NAND_IMAGE_LIST		0xA
// #define USBD_MMC_IMAGE_LIST		0xB
// #define USBD_SPI_IMAGE_LIST		0xC
//#define USBD_NOR_TYPE					0xD


#define USBD_BURN_TYPE 			0x80

extern UINT32 ChipID;
extern unsigned int volatile FLASH_BASE;
extern UINT32 volatile Bulk_Out_Transfer_Size;
extern int volatile _usbd_IntraROM;
extern int volatile _usbd_flash_type;
extern int volatile _usbd_xusb_type;

void udcInit(void);
void SendAck(UINT32 status);
int usb_send(UINT8* buf,UINT32 len);
int usb_recv(UINT8* buf,UINT32 len);
void SDRAM_USB_Transfer(UINT32 DRAM_Addr ,UINT32 Tran_Size);
//---------------------------------------

#endif

