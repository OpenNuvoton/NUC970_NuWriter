/*-----------------------------------------------------------------------------------*/
/* Nuvoton Electronics Corporation confidential                                      */
/*                                                                                   */
/* Copyright (c) 2008 by Nuvoton Electronics Corporation                             */
/* All rights reserved                                                               */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/
/* File Contents:                                                                    */
/*   usbd.c                                                                          */
/*                                                                                   */
/* This file contains:                                                               */
/*                                                                                   */
/* Project:                                                                          */
/*   1. Support non-OS version.                                                      */
/*   2. Take care the cache on issue.                                                */
/*   3. It's recommended that don't print any debug message.                         */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/
#include "usbd.h"

extern UCHAR NandBlockSize;

/* Define the vendor id and product id */
#define USB_VID		0x0416
#define USB_PID		0x5963

#define printf	sysprintf

//extern UINT32 volatile Mass_Base_Addr, Storage_Base_Addr;

#define USE_TOKEN	0

/* The interface number of mass storage interface */
#define MASS_INF_NUM		1

/* The endpoint number of mass storage interface's endpoint */
#define EP_NUM_ISO			1
#define EP_NUM_BKO			2
#define EP_NUM_INT			3

/* length of descriptors */
#define DEVICE_DSCPT_LEN	0x12
#define CONFIG_DSCPT_LEN	0x20
#define STR0_DSCPT_LEN		4
#define STR1_DSCPT_LEN		16
#define STR2_DSCPT_LEN		24

#define QUALIFIER_DSCPT_LEN		0x0a
#define OSCONFIG_DSCPT_LEN		0x20

#define USBD_DMA_LEN	0x1000

#define WM_LED_ON     0x51
#define WM_LED_OFF    0x50
#define WM_RESET      0x46

/* for USB */
USB_CMD_T	_usb_cmd_pkt;
UINT32 volatile _usbd_remlen=0;
UINT8 volatile _usbd_remlen_flag=0;
UINT8 *_usbd_ptr;

UINT8 volatile GET_DEV_Flag=0;
UINT8 volatile GET_CFG_Flag=0;
UINT8 volatile GET_QUL_Flag=0;
UINT8 volatile GET_OSCFG_Flag=0;
UINT8 volatile GET_STR_Flag=0;
UINT8 volatile CLASS_CMD_Flag=0;
UINT8 volatile usbdGetConfig=0;
UINT8 volatile usbdGetInterface=0;
UINT8 volatile usbdGetStatus=0;
UINT8 volatile GET_VEN_Flag=0;

// for DMA state flag
UINT8 volatile _usbd_DMA_Flag=0;
UINT8 volatile _usbd_Less_MPS=0;
UINT8 volatile _usbd_DMA_Dir;

UINT8 volatile bulkonlycmd=0;
UINT8 volatile USBPlugIn=0;

UINT32 volatile usbdMaxPacketSize=200;
// _usbd_devstate:	1.default state 2.addressed state 3.configured state
UINT32	_usbd_devstate;
UINT32	_usbd_address;
UINT32	_usbd_speedset;
UINT16	_usbd_confsel;
UINT16	_usbd_altsel;
UINT32	_usbd_haltep0=0;
UINT8	_usbd_haltep1=0;
UINT8	_usbd_haltep2=0;
UINT8 _usbd_unhaltep=0xff;

INT32		remotewakeup=0;
INT32		enableremotewakeup;
INT32		enabletestmode;
INT32		disableremotewakeup;
INT32		testselector;
UINT32	usbdGetStatusData;

//------------------------------------------
UINT32 ChipID;
unsigned int volatile FLASH_BASE;
UINT32 volatile Bulk_Out_Transfer_Size=0;

int volatile _usbd_IntraROM;
int volatile _usbd_flash_type = -1;
int volatile _usbd_xusb_type = -1;
UINT8 imgInfo_raw[400], *pImgInfo_raw;
//------------------------------------------

#define LEDON() (outpw(REG_GPIOH_DATAOUT, inpw(REG_GPIOH_DATAOUT) & (~0x00000002)))
#define LEDOFF() (outpw(REG_GPIOH_DATAOUT, inpw(REG_GPIOH_DATAOUT) | 0x00000002))

void leddisplay(UINT8 u8parm)
{
    outpw(REG_MFP_GPH_L, inpw(REG_MFP_GPH_L) & (~0x000000f0));	// GPH1 select gpio function
    outpw(REG_GPIOH_DIR, inpw(REG_GPIOH_DIR) | 0x00000002);	// set GPH1 as out

    if(u8parm) {
        outpw(REG_GPIOH_DATAOUT, inpw(REG_GPIOH_DATAOUT) | 0x00000002);	// set GPH1 to high
        //sysprintf("REG_GPIOH_DATAOUT=0x%x\n", inpw(REG_GPIOH_DATAOUT));
    } else {
        outpw(REG_GPIOH_DATAOUT, inpw(REG_GPIOH_DATAOUT) & (~0x00000002));	// set GPH1 to low
        //sysprintf("REG_GPIOH_DATAOUT=0x%x\n", inpw(REG_GPIOH_DATAOUT));
    }
}

extern UINT8 udcFlashInit(void);

/*!<USB Device Descriptor */
__align(4) static UINT16 _DeviceDescriptor[10] = {
    0x0112, 0x0200, 0x0000, 0x4000, 0x0416, 0x5963, 0x0100, 0x0201, 0x0100, 0x0000
};

__align(4) static UINT16 _QualifierDescriptor[6] = {
    0x060a, 0x0200, 0x0000, 0x4000, 0x0001, 0x0000
};

__align(4) static UINT16 _ConfigurationBlock[16] = {
    0x0209, 0x0020, 0x0101, 0xC000, 0x0932, 0x0004, 0x0200, 0x0000, 0x0000, 0x0507,
    0x0281, 0x0200, 0x0701, 0x0205, 0x0002, 0x0102
};

__align(4) static UINT16 _ConfigurationBlockFull[16] = {
    0x0209, 0x0020, 0x0101, 0xc000, 0x0932, 0x0004, 0x0200, 0x0000, 0x0000, 0x0507,
    0x0281, 0x0040, 0x0701, 0x0205, 0x4002, 0x0100
};

__align(4) static UINT16 _OSConfigurationBlock[16] = {
    0x0709, 0x0020, 0x0101, 0xc000, 0x0932, 0x0004, 0x0200, 0x0000, 0x0000, 0x0507,
    0x0281, 0x0040, 0x0701, 0x0205, 0x4002, 0x0100
};

__align(4) static UINT16 _StringDescriptor0[2] = {
    0x0304, 0x0409
};

__align(4) static UINT16 _StringDescriptor1[11] = {
    0x0316, 0x0055, 0x0053, 0x0042, 0x0020, 0x0044, 0x0065, 0x0076, 0x69, 0x0063, 0x0065
};

__align(4) static UINT16 _StringDescriptor2[26] = {
    0x0334, 0x004E, 0x0075, 0x0076, 0x006F, 0x0074, 0x006F, 0x006E, 0x0020, 0x0041,
    0x0052, 0x004D, 0x0020, 0x0039, 0x0032, 0x0036, 0x002D, 0x0042, 0x0061, 0x0073,
    0x0065, 0x0064, 0x0020, 0x004D, 0x0043, 0x0055
};

void SendAck(UINT32 status)
{
    char buf[4];
    unsigned int *state;
    state = ((unsigned int *)(((unsigned int)buf) | NON_CACHE));
    /* send status */
    *state = status;
    usb_send((unsigned char *)state, 4);//send ack to PC
}

int usb_recv(UINT8* buf,UINT32 len)
{
    _usbd_DMA_Dir = Ep_Out;

    outpw(REG_USBD_DMA_CTRL_STS, (inpw(REG_USBD_DMA_CTRL_STS)&0xe0) | 0x02);	// bulk out, dma write, ep2
    SDRAM_USB_Transfer((UINT32)buf,len);
    Bulk_Out_Transfer_Size = 0;

    outpw(REG_USBD_CEP_IRQ_ENB, (CEP_SUPPKT | CEP_STS_END));//lsshi

    return len;
}

int usb_send(UINT8* buf,UINT32 len)
{
    int volatile count;
    _usbd_DMA_Dir = Ep_In;
    outpw(REG_USBD_DMA_CTRL_STS, (inpw(REG_USBD_DMA_CTRL_STS)&0xe0) | 0x11);	// bulk in, dma read, ep1

    count = len / usbdMaxPacketSize;
    if (count != 0) {
        outpw(REG_USBD_EPA_IRQ_ENB, 0x08);
        while(!(inpw(REG_USBD_EPA_IRQ_STAT) & 0x02)) {
            __nop();
        }
        _usbd_Less_MPS=0;
        SDRAM_USB_Transfer((UINT32)buf,count*usbdMaxPacketSize);
        buf = (UINT8 *)((UINT32)buf + count*usbdMaxPacketSize);
    }

    count = len % usbdMaxPacketSize;
    if (count != 0) {
        outpw(REG_USBD_EPA_IRQ_ENB, 0x08);
        while(!(inpw(REG_USBD_EPA_IRQ_STAT) & 0x02)) {
            __nop();
        }

        _usbd_Less_MPS=1;
        SDRAM_USB_Transfer((UINT32)buf,count);
    }

    return len;
}


void usbdClearAllFlags()
{
    GET_VEN_Flag=0;
    CLASS_CMD_Flag=0;
    GET_DEV_Flag=0;
    GET_CFG_Flag=0;
    GET_QUL_Flag=0;
    GET_OSCFG_Flag=0;
    GET_STR_Flag=0;
    usbdGetConfig=0;
    usbdGetInterface=0;
    usbdGetStatus=0;
}

void UsbResetDma(void) // reset DMA and endpoints
{
    outpw(REG_USBD_DMA_CNT, 0);
    outpw(REG_USBD_DMA_CTRL_STS, 0x80);
    outpw(REG_USBD_DMA_CTRL_STS, 0x00);

    outpw(REG_USBD_EPA_RSP_SC, 0);
    outpw(REG_USBD_EPB_RSP_SC, 0);
    outpw(REG_USBD_EPA_RSP_SC, EP_TOGGLE);
    outpw(REG_USBD_EPB_RSP_SC, EP_TOGGLE);
    outpw(REG_USBD_EPA_RSP_SC, 0x01);		// flush fifo
    outpw(REG_USBD_EPB_RSP_SC, 0x01);		// flush fifo
}

void usbd_update_device()
{
    //update this device for set requests
    switch(_usb_cmd_pkt.bRequest) {
    case USBR_SET_ADDRESS:
        outpw(REG_USBD_ADDR, _usbd_address);
        break;

    case USBR_SET_CONFIGURATION:
//MSG_DEBUG("update\n");
        outpw(REG_USBD_EPA_RSP_SC, EP_TOGGLE);
        outpw(REG_USBD_EPB_RSP_SC, EP_TOGGLE);
        break;

    case USBR_SET_INTERFACE:
        break;

    case USBR_SET_FEATURE:
        if(_usbd_haltep0 == HALT)
            outpw(REG_USBD_CEP_CTRL_STAT, CEP_SEND_STALL);
        else if(_usbd_haltep1 == HALT) {
            outpw(REG_USBD_EPA_RSP_SC, EP_HALT);
        } else if(_usbd_haltep2 == HALT) {
            outpw(REG_USBD_EPB_RSP_SC, EP_HALT);
        } else if(enableremotewakeup == 1) {
            enableremotewakeup = 0;
            remotewakeup = 1;
        }
        break;

    case USBR_CLEAR_FEATURE:
        if(_usbd_unhaltep == 1 && _usbd_haltep1 == HALT) {
            outpw(REG_USBD_EPA_RSP_SC, 0x0);
            outpw(REG_USBD_EPA_RSP_SC, EP_TOGGLE);
            _usbd_haltep1 = UNHALT;
            _usbd_unhaltep = 0xff;
        }
        if(_usbd_unhaltep == 2 && _usbd_haltep2 == HALT) {
            outpw(REG_USBD_EPB_RSP_SC, 0x0);
            outpw(REG_USBD_EPB_RSP_SC, EP_TOGGLE);
            _usbd_haltep2 = UNHALT; // just for changing the haltep value
            _usbd_unhaltep = 0xff;
        } else if(disableremotewakeup == 1) {
            disableremotewakeup=0;
            remotewakeup=0;
        }
        break;

    default:
        break;
    }
}

void usbd_send_descriptor()
{
    UINT32 temp_cnt;
    //UINT16 volatile data;
    int volatile i;
    UINT8 *ptr=NULL;

    if ((GET_DEV_Flag == 1) || (GET_QUL_Flag == 1) || (GET_CFG_Flag == 1) ||
            (GET_OSCFG_Flag == 1) || (GET_STR_Flag == 1) ||  (CLASS_CMD_Flag == 1) ||
            (usbdGetConfig == 1) || (usbdGetInterface == 1) || (usbdGetStatus == 1) ||
            (GET_VEN_Flag == 1)) {
        if (_usbd_remlen_flag == 0) {
            if (GET_DEV_Flag == 1)
                ptr = (UINT8 *)_DeviceDescriptor;
            else if (GET_QUL_Flag == 1)
                ptr = (UINT8 *)_QualifierDescriptor;
            else if (GET_CFG_Flag == 1) {
                if (_usbd_speedset == 2)
                    ptr = (UINT8 *)_ConfigurationBlock;
                else if (_usbd_speedset == 1)
                    ptr = (UINT8 *)_ConfigurationBlockFull;
            } else if (GET_OSCFG_Flag == 1)
                ptr = (UINT8 *)_OSConfigurationBlock;
            else if (GET_STR_Flag == 1) {
                if ((_usb_cmd_pkt.wValue & 0xff) == 0)
                    ptr = (UINT8 *)_StringDescriptor0;
                else if ((_usb_cmd_pkt.wValue & 0xff) == 1)
                    ptr = (UINT8 *)_StringDescriptor1;
                else if ((_usb_cmd_pkt.wValue & 0xff) == 2)
                    ptr = (UINT8 *)_StringDescriptor2;
            } else if (CLASS_CMD_Flag == 1) {
                outpb(REG_USBD_CEP_DATA_BUF, 0);
                outpw(REG_USBD_IN_TRNSFR_CNT, 1);
                return;
            } else if (usbdGetConfig == 1) {
                outpb(REG_USBD_CEP_DATA_BUF, _usbd_confsel);
                outpw(REG_USBD_IN_TRNSFR_CNT, 1);
                return;
            } else if (usbdGetInterface == 1) {
                outpb(REG_USBD_CEP_DATA_BUF, _usbd_altsel);
                outpw(REG_USBD_IN_TRNSFR_CNT, 1);
                return;
            } else if (usbdGetStatus == 1) {
                outpb(REG_USBD_CEP_DATA_BUF, usbdGetStatusData & 0xff);
                outpb(REG_USBD_CEP_DATA_BUF, (usbdGetStatusData >> 8) & 0xff);
                outpw(REG_USBD_IN_TRNSFR_CNT, 2);
                return;
            } else if (GET_VEN_Flag == 1) {
                temp_cnt = _usb_cmd_pkt.wValue & 0x00ff;
                if (_usb_cmd_pkt.wValue == 0x84)
                    temp_cnt = (_usb_cmd_pkt.wValue & 0x00ff) | (NandBlockSize << 8);
                outpw(REG_USBD_CEP_DATA_BUF, temp_cnt);
                outpw(REG_USBD_IN_TRNSFR_CNT, 2);
                GET_VEN_Flag = 0;
                return;
            }
        } else
            ptr = _usbd_ptr;

        if (_usb_cmd_pkt.wLength > 0x40) {
            _usbd_remlen_flag = 1;
            _usbd_remlen = _usb_cmd_pkt.wLength - 0x40;
            _usb_cmd_pkt.wLength = 0x40;
        } else if (_usbd_remlen != 0) {
            _usbd_remlen_flag = 0;
            _usb_cmd_pkt.wLength = _usbd_remlen;
            _usbd_remlen = 0;
        } else {
            _usbd_remlen_flag = 0;
            _usbd_remlen = 0;
        }

        temp_cnt = _usb_cmd_pkt.wLength / 4;

        for (i=0; i<temp_cnt; i++, ptr+=4)
            outpw(REG_USBD_CEP_DATA_BUF, *(UINT32 *)ptr);

        temp_cnt = _usb_cmd_pkt.wLength % 4;

        for (i=0; i<temp_cnt; i++)
            outpb(REG_USBD_CEP_DATA_BUF, *ptr++);

        if (_usbd_remlen_flag)
            _usbd_ptr = ptr;

        outpw(REG_USBD_IN_TRNSFR_CNT, _usb_cmd_pkt.wLength);
    }
}

void usbd_control_packet()
{
    UINT32	temp;
    UINT8	ReqErr=0;
    temp = inpw(REG_USBD_SETUP1_0);
    _usb_cmd_pkt.bmRequestType = (UINT8)temp & 0xff;
    _usb_cmd_pkt.bRequest = (UINT8)(temp >> 8) & 0xff;
    _usb_cmd_pkt.wValue = (UINT16)inpw(REG_USBD_SETUP3_2);
    _usb_cmd_pkt.wIndex = (UINT16)inpw(REG_USBD_SETUP5_4);
    _usb_cmd_pkt.wLength = (UINT16)inpw(REG_USBD_SETUP7_6);
    // vendor command
    if (_usb_cmd_pkt.bmRequestType == 0x40) {
        // clear flags
        usbdClearAllFlags();

        Bulk_Out_Transfer_Size = 0;
        if (_usb_cmd_pkt.bRequest == 0xa0) {
            if (_usb_cmd_pkt.wValue == 0x12) {
                Bulk_Out_Transfer_Size = _usb_cmd_pkt.wIndex;
                outpw(REG_USBD_CEP_CTRL_STAT, CEP_NAK_CLEAR);	// clear nak so that sts stage is complete//lsshi
            } else if (_usb_cmd_pkt.wValue == 0x13) {
                // reset DMA
                outpw(REG_USBD_DMA_CTRL_STS, 0x80);
                outpw(REG_USBD_DMA_CTRL_STS, 0x00);
                outpw(REG_USBD_EPA_RSP_SC, 0x01);		// flush fifo
                outpw(REG_USBD_EPB_RSP_SC, 0x01);		// flush fifo

                outpw(REG_USBD_CEP_CTRL_STAT, CEP_NAK_CLEAR);	// clear nak so that sts stage is complete
            }
        }
        if (_usb_cmd_pkt.bRequest == 0xb0) {
            if (_usb_cmd_pkt.wValue == (USBD_BURN_TYPE+USBD_FLASH_SDRAM)) {
                _usbd_flash_type = USBD_FLASH_SDRAM;
                outpw(REG_USBD_CEP_CTRL_STAT, CEP_NAK_CLEAR);	// clear nak so that sts stage is complete//lsshi
            } else if (_usb_cmd_pkt.wValue == (USBD_BURN_TYPE+USBD_FLASH_NAND)) {
                _usbd_flash_type = USBD_FLASH_NAND;
                outpw(REG_USBD_CEP_CTRL_STAT, CEP_NAK_CLEAR);	// clear nak so that sts stage is complete//lsshi
            } else if (_usb_cmd_pkt.wValue == (USBD_BURN_TYPE+USBD_FLASH_NAND_RAW)) {
                _usbd_flash_type = USBD_FLASH_NAND_RAW;
                outpw(REG_USBD_CEP_CTRL_STAT, CEP_NAK_CLEAR);	// clear nak so that sts stage is complete//lsshi
            } else if (_usb_cmd_pkt.wValue == (USBD_BURN_TYPE+USBD_FLASH_MMC)) {
                _usbd_flash_type = USBD_FLASH_MMC;
                outpw(REG_USBD_CEP_CTRL_STAT, CEP_NAK_CLEAR);	// clear nak so that sts stage is complete//lsshi
            } else if (_usb_cmd_pkt.wValue == (USBD_BURN_TYPE+USBD_FLASH_MMC_RAW)) {
                _usbd_flash_type = USBD_FLASH_MMC_RAW;
                outpw(REG_USBD_CEP_CTRL_STAT, CEP_NAK_CLEAR);	// clear nak so that sts stage is complete//lsshi
            } else if (_usb_cmd_pkt.wValue == (USBD_BURN_TYPE+USBD_FLASH_SPI)) {
                _usbd_flash_type = USBD_FLASH_SPI;
                outpw(REG_USBD_CEP_CTRL_STAT, CEP_NAK_CLEAR);	// clear nak so that sts stage is complete//lsshi
            } else if (_usb_cmd_pkt.wValue == (USBD_BURN_TYPE+USBD_FLASH_SPI_RAW)) {
                _usbd_flash_type = USBD_FLASH_SPI_RAW;
                outpw(REG_USBD_CEP_CTRL_STAT, CEP_NAK_CLEAR);	// clear nak so that sts stage is complete//lsshi
            } else if (_usb_cmd_pkt.wValue == (USBD_BURN_TYPE+USBD_MTP)) {
                _usbd_flash_type = USBD_MTP;
                outpw(REG_USBD_CEP_CTRL_STAT, CEP_NAK_CLEAR);	// clear nak so that sts stage is complete//lsshi
            } else if (_usb_cmd_pkt.wValue == (USBD_BURN_TYPE+USBD_INFO))
                _usbd_flash_type = USBD_INFO;
            outpw(REG_USBD_CEP_CTRL_STAT, CEP_NAK_CLEAR);	// clear nak so that sts stage is complete//lsshi
        }

        outpw(REG_USBD_CEP_IRQ_STAT, 0x400);
        return;
    }

    if( _usb_cmd_pkt.bmRequestType == WM_LED_ON) {
        //sysprintf("LED_ON LED_ON LED_ON LED_ON LED_ON LED_ON LED_ON LED_ON\n");
        leddisplay(1);
    }

    if( _usb_cmd_pkt.bmRequestType == WM_LED_OFF) {
        //sysprintf("LED_OFF LED_OFF LED_OFF LED_OFF LED_OFF LED_OFF LED_OFF LED_OFF\n");
        leddisplay(0);
    }

    if( _usb_cmd_pkt.bmRequestType == WM_RESET) {
        //sysprintf("RESET NUC970\n");
        outpw(REG_AHBIPRST, 0x00000001);
    }

    /* vendor IN for ack */
    if (_usb_cmd_pkt.bmRequestType == 0xc0) {
        // clear flags
        usbdClearAllFlags();

        GET_VEN_Flag=1;
        outpw(REG_USBD_CEP_IRQ_STAT, 0x408);
        outpw(REG_USBD_CEP_IRQ_ENB, 0x408);		//suppkt int ,status and in token
        return;
    }

    switch (_usb_cmd_pkt.bRequest) {
    case USBR_GET_DESCRIPTOR:

        switch	((_usb_cmd_pkt.wValue & 0xff00) >> 8) {
            //high byte contains desc type so need to shift???
        case USB_DT_DEVICE:
            // clear flags
            usbdClearAllFlags();

            GET_DEV_Flag = 1;
            if (_usb_cmd_pkt.wLength > DEVICE_DSCPT_LEN)
                _usb_cmd_pkt.wLength = DEVICE_DSCPT_LEN;
            break;

        case USB_DT_CONFIG:
            // clear flags
            usbdClearAllFlags();

            GET_CFG_Flag = 1;
            if (_usb_cmd_pkt.wLength > CONFIG_DSCPT_LEN)
                _usb_cmd_pkt.wLength = CONFIG_DSCPT_LEN;
            break;

        case USB_DT_QUALIFIER:	// high-speed operation
            // clear flags
            usbdClearAllFlags();

            GET_QUL_Flag = 1;
            if (_usb_cmd_pkt.wLength > QUALIFIER_DSCPT_LEN)
                _usb_cmd_pkt.wLength = QUALIFIER_DSCPT_LEN;
            break;

        case USB_DT_OSCONFIG:	// other speed configuration
            // clear flags
            usbdClearAllFlags();

            GET_OSCFG_Flag = 1;
            if (_usb_cmd_pkt.wLength > OSCONFIG_DSCPT_LEN)
                _usb_cmd_pkt.wLength = OSCONFIG_DSCPT_LEN;
            break;

        case USB_DT_STRING:
            // clear flags
            usbdClearAllFlags();

            GET_STR_Flag = 1;
            if ((_usb_cmd_pkt.wValue & 0xff) == 0) {
                if (_usb_cmd_pkt.wLength > STR0_DSCPT_LEN)
                    _usb_cmd_pkt.wLength = STR0_DSCPT_LEN;
//printf("string\n");
            } else if ((_usb_cmd_pkt.wValue & 0xff) == 1) {
                if (_usb_cmd_pkt.wLength > STR1_DSCPT_LEN)
                    _usb_cmd_pkt.wLength = STR1_DSCPT_LEN;
            } else if ((_usb_cmd_pkt.wValue & 0xff) == 2) {
                if (_usb_cmd_pkt.wLength > STR2_DSCPT_LEN)
                    _usb_cmd_pkt.wLength = STR2_DSCPT_LEN;
            }
            break;

        default:
            ReqErr=1;
            break;
        }	//end of switch
        if (ReqErr == 0) {
            outpw(REG_USBD_CEP_IRQ_STAT, 0x008);
            outpw(REG_USBD_CEP_IRQ_ENB, 0x008);		//suppkt int ,status and in token
        }
        break;

    case USBR_SET_ADDRESS:
        ReqErr = ((_usb_cmd_pkt.bmRequestType == 0) && ((_usb_cmd_pkt.wValue & 0xff00) == 0)
                  && (_usb_cmd_pkt.wIndex == 0) && (_usb_cmd_pkt.wLength == 0)) ? 0 : 1;

        if ((_usb_cmd_pkt.wValue & 0xffff) > 0x7f) {	//within 7f
            ReqErr=1;	//Devaddr > 127
            MSG_DEBUG("ERROR -  Request Error - Device address greater than 127\n");
        }

        if (_usbd_devstate == 3) {
            ReqErr=1;	//Dev is configured
            MSG_DEBUG("ERROR - Request Error - Device is already in configure state\n");
        }

        if (ReqErr==1) {
            MSG_DEBUG("ERROR - CepEvHndlr:USBR_SET_ADDRESS <= Request Error\n");
            break;		//break this switch loop
        }

        if(_usbd_devstate == 2) {
            if(_usb_cmd_pkt.wValue == 0)
                _usbd_devstate = 1;		//enter default state
            _usbd_address = _usb_cmd_pkt.wValue;	//if wval !=0,use new address
        }

        if(_usbd_devstate == 1) {
            if(_usb_cmd_pkt.wValue != 0) {
                //enter addr state whether to write or not NOW!!!
                _usbd_address = _usb_cmd_pkt.wValue;
                _usbd_devstate = 2;
            }
        }
        outpw(REG_USBD_CEP_IRQ_STAT, 0x400);
        outpw(REG_USBD_CEP_CTRL_STAT, CEP_NAK_CLEAR);	//clear nak so that sts stage is complete
        outpw(REG_USBD_CEP_IRQ_ENB, 0x400);		// enable status complete interrupt
        break;

    case USBR_GET_CONFIGURATION:
        ReqErr = ((_usb_cmd_pkt.bmRequestType == 0x80) && (_usb_cmd_pkt.wValue == 0) &&
                  (_usb_cmd_pkt.wIndex == 0) && (_usb_cmd_pkt.wLength == 0x1) ) ? 0 : 1;

        if (ReqErr==1) {
            MSG_DEBUG("ERROR - CepEvHndlr:USBR_GET_CONFIGURATION <= Request Error\n");
            break;	//break this switch loop
        }

        usbdClearAllFlags();
        usbdGetConfig=1;

        outpw(REG_USBD_CEP_IRQ_STAT, 0x008);
        outpw(REG_USBD_CEP_IRQ_ENB, 0x008);		//status and in token
        break;

    case USBR_SET_CONFIGURATION:
        ReqErr = ((_usb_cmd_pkt.bmRequestType == 0) && ((_usb_cmd_pkt.wValue & 0xff00) == 0) &&
                  ((_usb_cmd_pkt.wValue & 0x80) == 0) && (_usb_cmd_pkt.wIndex == 0) &&
                  (_usb_cmd_pkt.wLength == 0)) ? 0 : 1;
//printf("set config\n");

        if (_usbd_devstate == 1) {
            MSG_DEBUG("ERROR - Device is in Default state\n");
            ReqErr=1;
        }

        if ((_usb_cmd_pkt.wValue != 1) && (_usb_cmd_pkt.wValue != 0) ) { //Only configuration one is supported
            MSG_DEBUG("ERROR - Configuration choosed is invalid\n");
            ReqErr=1;
        }

        if(ReqErr==1) {
            MSG_DEBUG("ERROR - CepEvHndlr:USBR_SET_CONFIGURATION <= Request Error\n");
            break;	//break this switch loop
        }

        if (_usb_cmd_pkt.wValue == 0) {
            _usbd_confsel = 0;
            _usbd_devstate = 2;
        } else {
            _usbd_confsel = _usb_cmd_pkt.wValue;
            _usbd_devstate = 3;
        }
        outpw(REG_USBD_CEP_IRQ_STAT, 0x400);
        outpw(REG_USBD_CEP_CTRL_STAT, CEP_NAK_CLEAR);	//clear nak so that sts stage is complete
        outpw(REG_USBD_CEP_IRQ_ENB, 0x400);		//status and in token
        break;

    case USBR_GET_INTERFACE:
        ReqErr = ((_usb_cmd_pkt.bmRequestType == 0x81) && (_usb_cmd_pkt.wValue == 0) &&
                  (_usb_cmd_pkt.wIndex == 0) && (_usb_cmd_pkt.wLength == 0x1)) ? 0 : 1;

        if ((_usbd_devstate == 1) || (_usbd_devstate == 2)) {
            MSG_DEBUG("ERROR - Device state is not valid\n");
            ReqErr=1;
        }

        if(ReqErr == 1) {
            MSG_DEBUG("ERROR - CepEvHndlr:USBR_GET_INTERFACE <= Request Error\n");
            break;	//break this switch loop
        }

        usbdClearAllFlags();
        usbdGetInterface=1;

        outpw(REG_USBD_CEP_IRQ_STAT, 0x008);
        outpw(REG_USBD_CEP_IRQ_ENB, 0x008);		//suppkt int ,status and in token
        break;

    case USBR_SET_INTERFACE:
        ReqErr = ((_usb_cmd_pkt.bmRequestType == 0x1) && ((_usb_cmd_pkt.wValue & 0xff80) == 0)
                  && ((_usb_cmd_pkt.wIndex & 0xfff0) == 0) && (_usb_cmd_pkt.wLength == 0)) ? 0 : 1;

        if ((_usbd_devstate == 0x3) && (_usb_cmd_pkt.wIndex == 0x0) && (_usb_cmd_pkt.wValue == 0x0)) {
            ReqErr=0;
            MSG_DEBUG("interface choosen is already is use, so stall was not sent\n");
        } else {
            MSG_DEBUG("ERROR - Choosen interface was not supported\n");
            ReqErr=1;
        }

        if (ReqErr == 1) {
            MSG_DEBUG("CepEvHndlr:USBR_SET_INTERFACE <= Request Error\n");
            break;	//break this switch loop
        }
        _usbd_altsel = _usb_cmd_pkt.wValue;
        outpw(REG_USBD_CEP_IRQ_STAT, 0x400);
        outpw(REG_USBD_CEP_CTRL_STAT, CEP_NAK_CLEAR);	//clear nak so that sts stage is complete
        outpw(REG_USBD_CEP_IRQ_ENB, 0x400);		//suppkt int ,status and in token
        break;

    case USBR_SET_FEATURE:
        MSG_DEBUG("\nUSBR_SET_FEATURE\n\n");
        //ReqErr = (((_usb_cmd_pkt.bmRequestType & 0xfc) == 0) && ((_usb_cmd_pkt.wValue & 0xfffc) == 0)
        //&& ((_usb_cmd_pkt.wIndex & 0xff00) == 0) && (_usb_cmd_pkt.wLength == 0)) ? 0 : 1;

        if (_usbd_devstate == 1) {
            if((_usb_cmd_pkt.bmRequestType & 0x3) == 0x0) { // Receipent is Device
                if((_usb_cmd_pkt.wValue & 0x3) == TEST_MODE) {
                    enabletestmode = 1;
                    testselector = (_usb_cmd_pkt.wIndex >> 8);
                    outpw(REG_USBD_CEP_IRQ_STAT, 0x400);
                    outpw(REG_USBD_CEP_IRQ_ENB, 0x400);
                }
            } else {
                MSG_DEBUG("ERROR - Device is in Default State\n");
                ReqErr=1;
            }
        }

        if (_usbd_devstate == 2) {
            if ((_usb_cmd_pkt.bmRequestType & 0x3 == 2) && ((_usb_cmd_pkt.wIndex & 0xff) != 0)) {	//ep but not cep
                MSG_DEBUG("ERROR - Device is in Addressed State, but for noncep\n");
                ReqErr =1;
            } else if ((_usb_cmd_pkt.bmRequestType & 0x3) == 0x1) {
                MSG_DEBUG("ERROR - Device is in Addressed State, but for interface\n");
                ReqErr=1;
            }
        }

        if (ReqErr == 1) {
            MSG_DEBUG("CepEvHndlr:USBR_SET_FEATURE <= Request Error\n");
            break;	//break this switch loop
        }

        //check if recipient and wvalue are appropriate
        switch(_usb_cmd_pkt.bmRequestType & 0x3) {
        case 0:		//device
            if ((_usb_cmd_pkt.wValue & 0x3) == DEVICE_REMOTE_WAKEUP)
                enableremotewakeup = 1;
            else if((_usb_cmd_pkt.wValue & 0x3) == TEST_MODE) {
                enabletestmode = 1;
                testselector = (_usb_cmd_pkt.wIndex >> 8);
            } else {
                MSG_DEBUG("ERROR - No such feature for device\n");
                ReqErr=1;
            }
            break;

        case 1:		//interface
            break;

        case 2:		//endpoint
            if((_usb_cmd_pkt.wValue & 0x3) == ENDPOINT_HALT) {
                if((_usb_cmd_pkt.wIndex & 0x3) == 0) { //endPoint zero
                    _usbd_haltep0 = HALT;
                } else if((_usb_cmd_pkt.wIndex & 0x3) == 1) { //endPoint one
                    _usbd_haltep1 = HALT;
                } else if((_usb_cmd_pkt.wIndex & 0x3) == 2) { //endPoint two
                    _usbd_haltep2 = HALT;
                } else {
                    MSG_DEBUG("ERROR - Selected endpoint was not present\n");
                    ReqErr=1;
                }
            } else {
                MSG_DEBUG("ERROR - Neither device,endpoint nor interface was choosen\n");
                ReqErr=1;
            }
            break;

        default:
            break;
        }//device
        outpw(REG_USBD_CEP_IRQ_STAT, 0x400);
        outpw(REG_USBD_CEP_CTRL_STAT, CEP_NAK_CLEAR);	//clear nak so that sts stage is complete
        outpw(REG_USBD_CEP_IRQ_ENB, 0x400);		//suppkt int ,status and in token
        break;

    case USBR_CLEAR_FEATURE:
        MSG_DEBUG("\nUSBR_CLEAR_FEATURE\n\n");
        ReqErr = (((_usb_cmd_pkt.bmRequestType & 0xfc) == 0) && ((_usb_cmd_pkt.wValue & 0xfffc) == 0)
                  && ((_usb_cmd_pkt.wIndex & 0xff00) == 0) && (_usb_cmd_pkt.wLength == 0)) ? 0 : 1;

        if (_usbd_devstate == 1) {
            MSG_DEBUG("ERROR - Device is in default state\n");
            ReqErr =1;
        }

        if (_usbd_devstate == 2) {
            if((_usb_cmd_pkt.bmRequestType == 2) && (_usb_cmd_pkt.wIndex != 0)) { //ep but not cep
                MSG_DEBUG("ERROR - Device is in Addressed State, but for noncep\n");
                ReqErr =1;
            } else if(_usb_cmd_pkt.bmRequestType == 0x1) {	//recip is interface
                MSG_DEBUG("ERROR - Device is in Addressed State, but for interface\n");
                ReqErr=1;
            }
        }

        if(ReqErr == 1) {
            MSG_DEBUG("ERROR - CepEvHndlr:USBR_CLEAR_FEATURE <= Request Error\n");
            break;	//break this switch loop
        }

        switch((_usb_cmd_pkt.bmRequestType & 0x3)) {
        case 0:		//device
            if((_usb_cmd_pkt.wValue & 0x3) == DEVICE_REMOTE_WAKEUP)
                disableremotewakeup = 1;
            else {
                MSG_DEBUG("ERROR - No such feature for device\n");
                ReqErr=1;
            }
            break;

        case 1:		//interface
            break;

        case 2:		//endpoint
            if((_usb_cmd_pkt.wValue & 0x3) == ENDPOINT_HALT) {
                if((_usb_cmd_pkt.wIndex & 0x3) == 0) //endPoint zero
                    _usbd_unhaltep = 0;
                else if((_usb_cmd_pkt.wIndex & 0x3) == 1) { //endPoint one
                    //if (!errorFlagCBW)
                    _usbd_unhaltep = 1;

                } else if((_usb_cmd_pkt.wIndex & 0x3) == 2) { //endPoint two
                    //if (!errorFlagCBW)
                    _usbd_unhaltep = 2;
                } else {
                    MSG_DEBUG("ERROR - endpoint choosen was not supported\n");
                    ReqErr=1;
                }
            } else {
                MSG_DEBUG("ERROR - Neither device,interface nor endpoint was choosen\n");
                ReqErr=1;
            }
            break;

        default:
            break;
        }	//device

        outpw(REG_USBD_CEP_IRQ_STAT, 0x400);
        outpw(REG_USBD_CEP_CTRL_STAT, CEP_NAK_CLEAR);	//clear nak so that sts stage is complete
        outpw(REG_USBD_CEP_IRQ_ENB, 0x400);		//suppkt int ,status and in token
        break;

    case USBR_GET_STATUS:
        MSG_DEBUG("\nUSBR_GET_STATUS\n\n");
        //check if this is valid
        ReqErr = (((_usb_cmd_pkt.bmRequestType & 0xfc) == 0x80) && (_usb_cmd_pkt.wValue == 0)
                  && ((_usb_cmd_pkt.wIndex & 0xff00) == 0) && (_usb_cmd_pkt.wLength == 0x2)) ? 0 : 1;

        if (_usbd_devstate == 1) {
            MSG_DEBUG("ERROR - Device is in default State\n");
            ReqErr =1;
        }
        if (_usbd_devstate == 2) {
            if ((_usb_cmd_pkt.bmRequestType & 0x3 == 0x2) && (_usb_cmd_pkt.wIndex != 0)) {
                MSG_DEBUG("ERROR - Device is in Addressed State, but for noncep\n");
                ReqErr =1;
            } else if (_usb_cmd_pkt.bmRequestType & 0x3 == 0x1) {
                MSG_DEBUG("ERROR - Device is in Addressed State, but for interface\n");
                ReqErr =1;
            }
        }

        if (ReqErr == 1) {
            MSG_DEBUG("ERROR - CepEvHndlr:USBR_GET_STATUS <= Request Error\n");
            break;	//break this switch loop
        }

        usbdClearAllFlags();
        usbdGetStatus=1;

        switch (_usb_cmd_pkt.bmRequestType & 0x3) {
        case 0:	// device
            MSG_DEBUG("value of dx->remotewakeup is %d\n", remotewakeup);
            if (remotewakeup == 1)
                usbdGetStatusData = 0x3;
            else
                usbdGetStatusData = 0x1;
            break;

        case 1:	//interface
            if (_usb_cmd_pkt.wIndex == 0) {
                MSG_DEBUG("Status of interface zero\n");
                usbdGetStatusData = 0;
            } else {
                MSG_DEBUG("Error - Status of interface non zero\n");
                ReqErr=1;
            }
            break;

        case 2:	//endpoint
            if (_usb_cmd_pkt.wIndex == 0x0) {
                MSG_DEBUG("Status of Endpoint zero\n");
                usbdGetStatusData = 0;
            } else if (_usb_cmd_pkt.wIndex == 0x81) {
                MSG_DEBUG("Status of Endpoint one\n");
                if (_usbd_haltep1 == HALT)
                    usbdGetStatusData = 0x1;
                else
                    usbdGetStatusData = 0;
            } else if (_usb_cmd_pkt.wIndex == 0x2) {
                MSG_DEBUG("Status of Endpoint two\n");
                if (_usbd_haltep2 == HALT)
                    usbdGetStatusData = 0x1;
                else
                    usbdGetStatusData = 0;
            } else {
                MSG_DEBUG("Error - Status of non-existing Endpoint\n");
                ReqErr=1;
            }
            break;

        default:
            break;
        }
        outpw(REG_USBD_CEP_IRQ_STAT, 0x008);
        outpw(REG_USBD_CEP_IRQ_ENB, 0x008);		//suppkt int ,status and in token
        break;

    default:	// other request
        break;
    }
    if (ReqErr == 1) {
        // not supported request, send stall
        outpw(REG_USBD_CEP_CTRL_STAT, CEP_SEND_STALL);
    }
}

void USBD_IRQHandler(void)
{
    UINT32 volatile IrqStL, IrqEnL, IrqSt, IrqEn;

    IrqStL = inpw(REG_USBD_IRQ_STAT_L);		/* get interrupt status */
    IrqEnL = inpw(REG_USBD_IRQ_ENB_L);
    if (!(IrqStL & IrqEnL)) {
        MSG_DEBUG("Not our interrupt!\n");
        goto isr_end;
    }

    /* USB interrupt */
    if (IrqStL & IRQ_USB_STAT) {
        IrqSt = inpw(REG_USBD_IRQ_STAT);
        IrqEn = inpw(REG_USBD_IRQ_ENB);

//MSG_DEBUG("INT status[%x], oper[%x]\n", IrqSt, inpw(REG_USBD_OPER));

        if (IrqSt & USB_SOF & IrqEn) {
            MSG_DEBUG("SOF Interrupt\n");
            outpw(REG_USBD_IRQ_STAT, 0x01);
        }

        if (IrqSt & USB_RST_STS & IrqEn) {
            //MSG_DEBUG("Reset Status Interrupt\n");
            _usbd_devstate = 0;
            _usbd_speedset = 0;
            _usbd_address = 0;

            // clear flags
            usbdClearAllFlags();
            _usbd_DMA_Flag=0;
            _usbd_Less_MPS=0;

//printf("!!! reset DMA !!!\n");
            UsbResetDma();

            _usbd_devstate = 1;		//default state
            _usbd_address = 0;		//zero

            if (inpw(REG_USBD_OPER) & 0x04) {
                MSG_DEBUG("High speed after reset\n");
                _usbd_speedset = 2;		//for high speed
                usbdHighSpeedInit();
            } else {
                MSG_DEBUG("Full speed after reset\n");
                _usbd_speedset = 1;		//for full speed
                usbdFullSpeedInit();
            }
            outpw(REG_USBD_CEP_IRQ_ENB, 0x002);		//suppkt int

            outpw(REG_USBD_ADDR, 0);
            outpw(REG_USBD_IRQ_ENB, (USB_RST_STS|USB_SUS_REQ|USB_RESUME));
            outpw(REG_USBD_IRQ_STAT, 0x02);
            outpw(REG_USBD_CEP_IRQ_STAT, 0xfffc);
        }

        if (IrqSt & USB_RESUME & IrqEn) {
            MSG_DEBUG("Resume Interrupt\n");
            outpw(REG_USBD_IRQ_ENB, (USB_RST_STS|USB_SUS_REQ));
            outpw(REG_USBD_IRQ_STAT, 0x04);
        }

        if (IrqSt & USB_SUS_REQ & IrqEn) {
            MSG_DEBUG("Suspend Request Interrupt\n");
            outpw(REG_USBD_IRQ_ENB, (USB_RST_STS | USB_RESUME ));
            outpw(REG_USBD_IRQ_STAT, 0x08);
        }

        if (IrqSt & USB_HS_SETTLE & IrqEn) {
            MSG_DEBUG("Device settled in High speed\n");
            _usbd_devstate = 1;		//default state
            _usbd_speedset = 2;		//for high speed
            _usbd_address = 0;		//zero
            outpw(REG_USBD_CEP_IRQ_ENB, 0x002);
            outpw(REG_USBD_IRQ_STAT, 0x10);
        }

        if (IrqSt & USB_DMA_REQ & IrqEn) {
            _usbd_DMA_Flag = 1;
            outpw(REG_USBD_IRQ_STAT, USB_DMA_REQ);

            if (_usbd_DMA_Dir == Ep_Out) {
                //	MSG_DEBUG("OUT DMA [%d]\n", _usbd_Less_MPS);
                outpw(REG_USBD_EPB_IRQ_ENB, 0x10);
            }
            if (_usbd_DMA_Dir == Ep_In) {
                //MSG_DEBUG("IN DMA [%d]\n", _usbd_Less_MPS);
                if (_usbd_Less_MPS == 1) {
                    outpw(REG_USBD_EPA_RSP_SC, 0x00000040);	// packet end
                    _usbd_Less_MPS = 0;
                }
            }
        }

        if (IrqSt & USABLE_CLK & IrqEn)
            outpw(REG_USBD_IRQ_STAT, 0x40);
        //MSG_DEBUG("Usable Clock Interrupt\n");

        outpw(REG_USBD_IRQ_STAT_L, IRQ_USB_STAT);
    }

    if (IrqStL & IRQ_CEP) {
        IrqSt = inpw(REG_USBD_CEP_IRQ_STAT);
        IrqEn = inpw(REG_USBD_CEP_IRQ_ENB);

        MSG_DEBUG("control INT status[%x]\n", IrqSt);

        if (IrqSt & CEP_SUPTOK & IrqEn) {
            //MSG_DEBUG("Setup Token Interrupt\n");
            outpw(REG_USBD_CEP_IRQ_STAT, 0x001);
            goto isr_end;
        }

        if (IrqSt & CEP_SUPPKT & IrqEn) {
            //MSG_DEBUG("Setup\n");
            outpw(REG_USBD_CEP_IRQ_STAT, 0x002);
            usbd_control_packet();
            goto isr_end;
        }

        if (IrqSt & CEP_OUT_TOK & IrqEn) {
            //MSG_DEBUG("Out\n");
            outpw(REG_USBD_CEP_IRQ_STAT, 0x004);
            outpw(REG_USBD_CEP_IRQ_ENB, 0x400);		// suppkt int//enb sts completion int
            goto isr_end;
        }

        if (IrqSt & CEP_IN_TOK & IrqEn) {
            //MSG_DEBUG("In[%x]\n", IrqSt);
            outpw(REG_USBD_CEP_IRQ_STAT, 0x008);
            if (!(IrqSt & CEP_STS_END)) {
                outpw(REG_USBD_CEP_IRQ_STAT, 0x020);
                outpw(REG_USBD_CEP_IRQ_ENB, 0x020);
                usbd_send_descriptor();
            } else {
                outpw(REG_USBD_CEP_IRQ_STAT, 0x020);
                outpw(REG_USBD_CEP_IRQ_ENB, 0x420);
            }
            //outpw(REG_USBD_CEP_IRQ_STAT, 0x008);
            goto isr_end;
        }

        if (IrqSt & CEP_PING_TOK & IrqEn) {
            MSG_DEBUG("Ping\n");
            //outpw(REG_USBD_CEP_IRQ_ENB, 0x402);		// suppkt int//enb sts completion int
            outpw(REG_USBD_CEP_IRQ_STAT, 0x010);
            goto isr_end;
        }

        if (IrqSt & CEP_DATA_TXD & IrqEn) {
            //MSG_DEBUG("Txd\n");
            outpw(REG_USBD_CEP_IRQ_STAT, 0x400);
            outpw(REG_USBD_CEP_CTRL_STAT, CEP_NAK_CLEAR);	// clear nak so that sts stage is complete
            if (_usbd_remlen_flag) {
                outpw(REG_USBD_CEP_IRQ_STAT, 0x08);
                outpw(REG_USBD_CEP_IRQ_ENB, 0x08);
            } else {
                outpw(REG_USBD_CEP_IRQ_STAT, 0x400);
                outpw(REG_USBD_CEP_IRQ_ENB, 0x402);
            }
            outpw(REG_USBD_CEP_IRQ_STAT, 0x020);
            goto isr_end;
        }

        if (IrqSt & CEP_DATA_RXD & IrqEn) {
            //MSG_DEBUG("Rxd\n");
            outpw(REG_USBD_CEP_IRQ_STAT, 0x040);
            outpw(REG_USBD_CEP_CTRL_STAT, CEP_NAK_CLEAR);
            outpw(REG_USBD_CEP_IRQ_ENB, 0x402);
            goto isr_end;
        }

        if (IrqSt & CEP_NAK_SENT & IrqEn) {
            MSG_DEBUG("NAK Sent Interrupt\n");
            outpw(REG_USBD_CEP_IRQ_STAT, 0x080);
            goto isr_end;
        }

        if (IrqSt & CEP_STALL_SENT & IrqEn) {
            MSG_DEBUG("STALL Sent Interrupt\n");
            outpw(REG_USBD_CEP_IRQ_STAT, 0x100);
            goto isr_end;
        }

        if (IrqSt & CEP_USB_ERR & IrqEn) {
            MSG_DEBUG("USB Error Interrupt\n");
            outpw(REG_USBD_CEP_IRQ_STAT, 0x200);
            goto isr_end;
        }

        if (IrqSt & CEP_STS_END & IrqEn) {
            //MSG_DEBUG("Status\n");

            usbd_update_device();
            outpw(REG_USBD_CEP_IRQ_STAT, 0x400);
            //MSG_DEBUG("Status [%x]\n", inpw(REG_USBD_CEP_IRQ_STAT));
            outpw(REG_USBD_CEP_IRQ_ENB, 0x002);
            goto isr_end;
        }

        if (IrqSt & CEP_BUFF_FULL & IrqEn) {
            MSG_DEBUG("Buffer Full Interrupt\n");
            outpw(REG_USBD_CEP_IRQ_STAT, 0x800);
            goto isr_end;
        }

        if (IrqSt & CEP_BUFF_EMPTY & IrqEn) {
            MSG_DEBUG("Buffer Empty Interrupt\n");
            outpw(REG_USBD_CEP_IRQ_STAT, 0x1000);
            goto isr_end;
        }
        outpw(REG_USBD_IRQ_STAT_L, IRQ_CEP);
    }

    if (IrqStL & IRQ_NCEP) {
        if (IrqStL & 0x04) {
            IrqSt = inpw(REG_USBD_EPA_IRQ_STAT);
            IrqEn = inpw(REG_USBD_EPA_IRQ_ENB);
            //MSG_DEBUG(" Interrupt from Endpoint A m[%x], s[%x]\n", IrqEn, IrqSt);

            outpw(REG_USBD_EPA_IRQ_STAT, 0x40);
            outpw(REG_USBD_EPA_IRQ_STAT, IrqSt);
        }

        if (IrqStL & 0x08) {
            IrqSt = inpw(REG_USBD_EPB_IRQ_STAT);
            IrqEn = inpw(REG_USBD_EPB_IRQ_ENB);
            //MSG_DEBUG(" Interrupt from Endpoint B m[%x], s[%x]\n", IrqEn, IrqSt);

            outpw(REG_USBD_EPB_IRQ_ENB, 0x0);
            outpw(REG_USBD_EPB_IRQ_STAT, IrqSt);
        }

        if (IrqStL & 0x10) {
            MSG_DEBUG(" Interrupt from Endpoint C \n");
            IrqSt = inpw(REG_USBD_EPC_IRQ_STAT);
            IrqEn = inpw(REG_USBD_EPC_IRQ_ENB);

            outpw(REG_USBD_EPC_IRQ_STAT, 0x40);	//data pkt transmited
            outpw(REG_USBD_EPC_IRQ_STAT, IrqSt);
        }

        if (IrqStL & 0x20) {
            MSG_DEBUG(" Interrupt from Endpoint D \n");
            IrqSt = inpw(REG_USBD_EPD_IRQ_STAT);
            IrqEn = inpw(REG_USBD_EPD_IRQ_ENB);

            outpw(REG_USBD_EPD_IRQ_ENB, 0x0);
            outpw(REG_USBD_EPD_IRQ_STAT, IrqSt);
        }

        if (IrqStL & 0x40) {
            MSG_DEBUG(" Interrupt from Endpoint E \n");
            IrqSt = inpw(REG_USBD_EPE_IRQ_STAT);
            IrqEn = inpw(REG_USBD_EPE_IRQ_ENB);

            outpw(REG_USBD_EPE_IRQ_STAT, 0x40);	//data pkt transmited
            outpw(REG_USBD_EPE_IRQ_STAT, IrqSt);
        }

        if (IrqStL & 0x80) {
            MSG_DEBUG(" Interrupt from Endpoint F \n");
            IrqSt = inpw(REG_USBD_EPF_IRQ_STAT);
            IrqEn = inpw(REG_USBD_EPF_IRQ_ENB);

            outpw(REG_USBD_EPF_IRQ_ENB, 0x0);
            outpw(REG_USBD_EPF_IRQ_STAT, IrqSt);
        }
        outpw(REG_USBD_IRQ_STAT_L, IRQ_NCEP);
    }

isr_end:

    return;  /* return is compile issue of single task library */
}

void SDRAM_USB_Transfer(UINT32 DRAM_Addr ,UINT32 Tran_Size)
{
    if(Tran_Size != 0) {
        outpw(REG_USBD_IRQ_ENB, (USB_DMA_REQ | USB_RST_STS | USB_SUS_REQ ));
        outpw(REG_USBD_AHB_DMA_ADDR, DRAM_Addr);
        outpw(REG_USBD_DMA_CNT, Tran_Size);
        _usbd_DMA_Flag=0;
        outpw(REG_USBD_DMA_CTRL_STS, inpw(REG_USBD_DMA_CTRL_STS)|0x00000020);
        while(_usbd_DMA_Flag==0) {
            __nop();   //waiting for DMA complete
        }
    }
}

/* USB means usb host, sdram->host=> bulk in */
void SDRAM2USB_Bulk(UINT32 DRAM_Addr ,UINT32 Tran_Size)
{
    unsigned int volatile count=0;
    int volatile i=0, loop;
    UINT32 volatile addr, len;

    //printf("SDRAM2USB_Bulk: addr[%x], size[%d]\n", DRAM_Addr, Tran_Size);

    _usbd_DMA_Dir = Ep_In;
    outpw(REG_USBD_DMA_CTRL_STS, (inpw(REG_USBD_DMA_CTRL_STS)&0xe0) | 0x11);	// bulk in, dma read, ep1

    loop = Tran_Size / USBD_DMA_LEN;
    for (i=0; i<loop; i++) {
        outpw(REG_USBD_EPA_IRQ_ENB, 0x08);
        _usbd_Less_MPS = 0;
        while(1) {
            if (inpw(REG_USBD_EPA_IRQ_STAT) & 0x02) {
                SDRAM_USB_Transfer(DRAM_Addr+i*USBD_DMA_LEN, USBD_DMA_LEN);
                break;
            }
        }
    }

    addr = DRAM_Addr + i * USBD_DMA_LEN;
    loop = Tran_Size % USBD_DMA_LEN;
    if (loop != 0) {
        count = loop / usbdMaxPacketSize;
        if (count != 0) {
            outpw(REG_USBD_EPA_IRQ_ENB, 0x08);
            _usbd_Less_MPS = 0;
            while(1) {
                if (inpw(REG_USBD_EPA_IRQ_STAT) & 0x02) {
                    SDRAM_USB_Transfer(addr, usbdMaxPacketSize*count);
                    break;
                }
            }
            addr = addr + count*usbdMaxPacketSize;
        }

        len = loop % usbdMaxPacketSize;
        if (len != 0) {
            outpw(REG_USBD_EPA_IRQ_ENB, 0x08);
            _usbd_Less_MPS = 1;
            while(1) {
                if (inpw(REG_USBD_EPA_IRQ_STAT) & 0x02) {
                    SDRAM_USB_Transfer(addr, len);
                    break;
                }
            }
        }
    }
}

void USB2SDRAM_Bulk(UINT32 DRAM_Addr ,UINT32 Tran_Size)
{
    int volatile i=0, loop;

    //printf("USB2SDRAM_Bulk: addr[%x], size[%d]\n", DRAM_Addr, Tran_Size);

    _usbd_DMA_Dir = Ep_Out;
    _usbd_Less_MPS = 0;
    outpw(REG_USBD_DMA_CTRL_STS, (inpw(REG_USBD_DMA_CTRL_STS) & 0xe0)|0x02);	// bulk out, dma write, ep2

    loop = Tran_Size / USBD_DMA_LEN;
    for (i=0; i<loop; i++) {
        SDRAM_USB_Transfer((DRAM_Addr+i*USBD_DMA_LEN), USBD_DMA_LEN);
    }

    loop = Tran_Size % USBD_DMA_LEN;
    if (loop != 0) {
        SDRAM_USB_Transfer((DRAM_Addr+i*USBD_DMA_LEN), loop);
    }
}

void usbdHighSpeedInit()
{
    MSG_ERROR("Device is configured for High Speed\n");
    usbdMaxPacketSize = 0x200;
    /* bulk in */
    outpw(REG_USBD_EPA_IRQ_ENB, 0x00000008);	// data pkt transmited and in token intr enb
    outpw(REG_USBD_EPA_RSP_SC, 0x00000000);		// auto validation
    outpw(REG_USBD_EPA_MPS, 0x00000200);		// mps 512
    outpw(REG_USBD_EPA_CFG, 0x0000001b);		// bulk in ep no 1
    outpw(REG_USBD_EPA_START_ADDR, 0x00000100);
    outpw(REG_USBD_EPA_END_ADDR, 0x000002ff);

    /* bulk out */
    outpw(REG_USBD_EPB_IRQ_ENB, 0x00000010);	// data pkt received  and outtokenb
    outpw(REG_USBD_EPB_RSP_SC, 0x00000000);		// auto validation
    outpw(REG_USBD_EPB_MPS, 0x00000200);		// mps 512
    outpw(REG_USBD_EPB_CFG, 0x00000023);		// bulk out ep no 2
    outpw(REG_USBD_EPB_START_ADDR, 0x00000300);
    outpw(REG_USBD_EPB_END_ADDR, 0x000006ff);
}

void usbdFullSpeedInit()
{
    MSG_ERROR("Device is configured for Full Speed\n");
    usbdMaxPacketSize = 0x40;
    /* bulk in */
    outpw(REG_USBD_EPA_IRQ_ENB, 0x00000008);	// data pkt transmited and in token intr enb
    outpw(REG_USBD_EPA_RSP_SC, 0x00000000);		// auto validation
    outpw(REG_USBD_EPA_MPS, 0x00000040);		// mps 64
    outpw(REG_USBD_EPA_CFG, 0x0000001b);		// bulk in ep no 1
    outpw(REG_USBD_EPA_START_ADDR, 0x00000100);
    outpw(REG_USBD_EPA_END_ADDR, 0x000002ff);

    /* bulk out */
    outpw(REG_USBD_EPB_IRQ_ENB, 0x00000010);	// data pkt received  and outtokenb
    outpw(REG_USBD_EPB_RSP_SC, 0x00000000);		// auto validation
    outpw(REG_USBD_EPB_MPS, 0x00000040);		// mps 64
    outpw(REG_USBD_EPB_CFG, 0x00000023);		// bulk out ep no 2
    outpw(REG_USBD_EPB_START_ADDR, 0x00000300);
    outpw(REG_USBD_EPB_END_ADDR, 0x000006ff);
}


void udcInit(void)
{
    UINT32 volatile gDramSize;
    //UINT32 i;
    outpw(REG_MFP_GPH_L, 0x00000007);

    /* Enable USB Clock */
    outpw(REG_HCLKEN, inpw(REG_HCLKEN) | 0x80000);
    /* Enable PHY */
    outpw(REG_USBD_PHY_CTL, inpw(REG_USBD_PHY_CTL) | 0x200);
    /* wait PHY clock ready */
    while (1) {
        outpw(REG_USBD_EPA_MPS, 0x20);
        if (inpw(REG_USBD_EPA_MPS) == 0x20)
            break;
    }

    /* initial state */
    _usbd_devstate = 0;		//initial state
    _usbd_address  = 0;		//not set
    _usbd_confsel  = 0;		//not selected
    _usbd_altsel   = 0;
    _usbd_speedset = 2;		// default at high speed mode

    /*
     * configure mass storage interface endpoint
     */

    usbdHighSpeedInit();

    /*
     * configure USB controller
     */
    outpw(REG_USBD_IRQ_ENB_L, 0x0F);	/* enable usb, cep, epa, epb interrupt */
    outpw(REG_USBD_IRQ_ENB, (USB_DMA_REQ | USB_RESUME | USB_RST_STS ));
    outpw(REG_USBD_OPER, USB_HS);
    //outpw(REG_USBD_OPER, 0);

    outpw(REG_USBD_ADDR, 0);

    /* allocate 0xff bytes mem for cep */
    outpw(REG_USBD_CEP_START_ADDR, 0);
    outpw(REG_USBD_CEP_END_ADDR, 0x0ff);
    outpw(REG_USBD_CEP_IRQ_ENB, (CEP_SUPPKT | CEP_STS_END));

    /* Enable Interrupt Handler */
    sysInstallISR(IRQ_LEVEL_1, IRQ_UDC, (PVOID)USBD_IRQHandler);
    sysEnableInterrupt(IRQ_UDC);

    /* enable CPSR I bit */
    sysSetLocalInterrupt(ENABLE_IRQ);

    /* check floating detect */
    MSG_DEBUG("0x%x\n", inpw(REG_USBD_PHY_CTL));
    if (inpw(REG_USBD_PHY_CTL) & 0x80000000) {
        outpw(REG_USBD_PHY_CTL, inpw(REG_USBD_PHY_CTL) | 0x100);
        USBPlugIn = 1;
    }
}
