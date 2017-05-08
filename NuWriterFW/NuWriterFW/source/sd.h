/******************************************************************************
 *
 * Copyright (c) 2013 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: sd.h $
 *
 * $Author: schung $
 ******************************************************************************/
 #ifndef __SD_H__
 #define __SD_H__

#include "wbio.h"
#include "stdio.h"
#include <stdio.h>
#include "nuc970_reg.h"
#include "fmi.h"

#define SD_CARD             0
#define SD_ERR_ID           0xFFFF0100

#define SD_TIMEOUT          (SD_ERR_ID|0x01)
#define SD_NO_MEMORY        (SD_ERR_ID|0x02)

//-- function return value
#define    Successful       0
#define    Fail             1

//--- define type of SD card or MMC
#define SD_TYPE_UNKNOWN     0
#define SD_TYPE_SD_HIGH     1
#define SD_TYPE_SD_LOW      2
#define SD_TYPE_MMC         3
#define SD_TYPE_EMMC        4

/* SD error */
#define SD_NO_SD_CARD       (SD_ERR_ID|0x10)
#define SD_ERR_DEVICE       (SD_ERR_ID|0x11)
#define SD_INIT_TIMEOUT     (SD_ERR_ID|0x12)
#define SD_SELECT_ERROR     (SD_ERR_ID|0x13)
#define SD_WRITE_PROTECT    (SD_ERR_ID|0x14)
#define SD_INIT_ERROR       (SD_ERR_ID|0x15)
#define SD_CRC7_ERROR       (SD_ERR_ID|0x16)
#define SD_CRC16_ERROR      (SD_ERR_ID|0x17)
#define SD_CRC_ERROR        (SD_ERR_ID|0x18)
#define SD_CMD8_ERROR       (SD_ERR_ID|0x19)


/*******************************************/
/* Bit definition of DMACCSR register */
#define DMAC_CSR_EN         ((unsigned int)0x00000001)
#define DMAC_CSR_SWRST      ((unsigned int)0x00000002)
#define DMAC_CSR_SG_EN2     ((unsigned int)0x00000008)

/* Bit definition of DMACSAR register */
#define DMAC_SAR_PAD_ORDER  ((unsigned int)0x00000001)

/* Bit definition of FMICSR register */
#define FMI_CSR_SWRST       ((unsigned int)0x00000001)
#define FMI_CSR_SD_EN       ((unsigned int)0x00000002)
#define FMI_CSR_SM_EN       ((unsigned int)0x00000008)

/* Bit definition of FMIIER/FMIISR register */
#define FMI_IER_DTA_IE      ((unsigned int)0x00000001)
#define FMI_ISR_DTA_IF      ((unsigned int)0x00000001)

/* Bit definition of SDCSR register */
#define SD_CSR_CO_EN        ((unsigned int)0x00000001)
#define SD_CSR_RI_EN        ((unsigned int)0x00000002)
#define SD_CSR_DI_EN        ((unsigned int)0x00000004)
#define SD_CSR_DO_EN        ((unsigned int)0x00000008)
#define SD_CSR_R2_EN        ((unsigned int)0x00000010)
#define SD_CSR_CLK74_OE     ((unsigned int)0x00000020)
#define SD_CSR_CLK8_OE      ((unsigned int)0x00000040)
#define SD_CSR_CLK_KEEP0    ((unsigned int)0x00000080)
#define SD_CSR_CMD_MASK     ((unsigned int)0x00003F00)
#define SD_CSR_SWRST        ((unsigned int)0x00004000)
#define SD_CSR_DBW_4BIT     ((unsigned int)0x00008000)
#define SD_CSR_BLK_CNT_MASK ((unsigned int)0x00FF0000)
#define SD_CSR_NWR_MASK     ((unsigned int)0x0F000000)
#define SD_CSR_PORT0        ((unsigned int)0x00000000)
#define SD_CSR_PORT1        ((unsigned int)0x20000000)
#define SD_CSR_PORT_MASK    ((unsigned int)0x60000000)
#define SD_CSR_CLK_KEEP1    ((unsigned int)0x80000000)

/* Bit definition of SDIER register */
#define SD_IER_BLKD_IE      ((unsigned int)0x00000001)
#define SD_IER_CRC_IE       ((unsigned int)0x00000002)
#define SD_IER_CD0_IE       ((unsigned int)0x00000100)
#define SD_IER_SDIO0_IE     ((unsigned int)0x00000400)
#define SD_IER_RITO_IE      ((unsigned int)0x00001000)
#define SD_IER_DITO_IE      ((unsigned int)0x00002000)
#define SD_IER_WKUP_EN      ((unsigned int)0x00004000)
#define SD_IER_CD0SRC_GPIO  ((unsigned int)0x40000000)

/* Bit definition of SDISR register */
#define SD_ISR_BLKD_IF      ((unsigned int)0x00000001)
#define SD_ISR_CRC_IF       ((unsigned int)0x00000002)
#define SD_ISR_CRC7_OK      ((unsigned int)0x00000004)
#define SD_ISR_CRC16_OK     ((unsigned int)0x00000008)
#define SD_ISR_DATA0        ((unsigned int)0x00000080)
#define SD_ISR_CD0_IF       ((unsigned int)0x00000100)
#define SD_ISR_SDIO0_IF     ((unsigned int)0x00000400)
#define SD_ISR_RITO_IF      ((unsigned int)0x00001000)
#define SD_ISR_DITO_IF      ((unsigned int)0x00002000)
#define SD_ISR_CDPS0        ((unsigned int)0x00010000)
#define SD_ISR_DATA1        ((unsigned int)0x00040000)


/********************************************************/
#if 0
typedef struct SD_info_t
{
    UINT32  CardType;       // sd2.0, sd1.1, or mmc
    UINT32  RCA;            // relative card address
    UINT8   bIsCardInsert;
}SD_INFO_T;

extern SD_INFO_T *pSD0;
extern SD_INFO_T *pSD1;
#endif
/* extern function */


//#define SD_FREQ       24000
#define SD_FREQ     12000

//#define SDHC_FREQ 50000
#define SDHC_FREQ   12000


//*******************************************************
//  sd.h
//*******************************************************

// extern global variables
extern UINT32 _sd_ReferenceClock;
extern UINT8 volatile _sd_SDDataReady;

//#define STOR_STRING_LEN   32

#if 0
/* we allocate one of these for every device that we remember */
typedef struct disk_data_t
{
    struct disk_data_t  *next;           /* next device */

    /* information about the device -- always good */
    unsigned int  totalSectorN;
    unsigned int  diskSize;         /* disk size in Kbytes */
    int           sectorSize;
    char          vendor[STOR_STRING_LEN];
    char          product[STOR_STRING_LEN];
    char          serial[STOR_STRING_LEN];
} DISK_DATA_T;
#endif

// function declaration

// SD functions
int  SD_Command(FMI_SD_INFO_T *pSD, UINT8 ucCmd, UINT32 uArg);
int  SD_CmdAndRsp(FMI_SD_INFO_T *pSD, UINT8 ucCmd, UINT32 uArg, int nCount);
int  SD_CmdAndRsp2(FMI_SD_INFO_T *pSD, UINT8 ucCmd, UINT32 uArg, UINT32 *puR2ptr);
int  SD_CmdAndRspDataIn(FMI_SD_INFO_T *pSD, UINT8 ucCmd, UINT32 uArg);
int  SD_SelectCardType(FMI_SD_INFO_T *pSD);
void SD_Get_SD_info(FMI_SD_INFO_T *pSD, DISK_DATA_T *_info);
int  SD_Read_in(FMI_SD_INFO_T *pSD, UINT32 uSector, UINT32 uBufcnt, UINT32 uDAddr);
int  SD_Write_in(FMI_SD_INFO_T *pSD, UINT32 uSector, UINT32 uBufcnt, UINT32 uSAddr);
void SD_CheckRB(void);
void SD_SetReferenceClock(UINT32 uClock);
void SD_Set_clock(UINT32 sd_clock_khz);
void SD_CardSelect(int cardSel);
int SD_Init(FMI_SD_INFO_T *pSD);
void SD_Show_info(int sdport);
int  SD_Read_in_blksize(FMI_SD_INFO_T *pSD, UINT32 uSector, UINT32 uBufcnt, UINT32 uDAddr, UINT32 blksize);

//*******************************************************

 #endif /* END __SD_H__ */
