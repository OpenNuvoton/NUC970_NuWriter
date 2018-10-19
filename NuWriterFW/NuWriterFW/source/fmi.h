/******************************************************************************
 *
 * Copyright (c) 2013 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: fmi.h $
 *
 * $Author: schung $
 ******************************************************************************/
 #ifndef __FMI_H__
 #define __FMI_H__
#include <stdio.h>

#include "wbio.h"

#define SPI_HEAD_ADDR 4 //Blocks

//static volatile unsigned char Enable4ByteFlag=0;

//#define DMAC_SCATTER_GETTER
//#define _USE_DAT3_DETECT_

//-- function return value
#define    Successful  0
#define    Failed     -1

/* IntraROM version define */
#define INTRAROM_CA     0x20070502
//#define INTRAROM_CB       0x20080117

// extern global variables
extern UINT32 _fmi_uFMIReferenceClock;
extern UINT32 _fmi_uFirst_L2P;
extern BOOL volatile _fmi_bIsSDDataReady, _fmi_bIsSMDataReady;
extern BOOL volatile _fmi_bIsMSDataReady, _fmi_bIsMSTimeOut;

#define STOR_STRING_LEN 32

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

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


typedef struct dmac_desc_t {
    UINT32 uPhyAddress;
    UINT32 uSectorCount;
} DMAC_DESC_T;
extern DMAC_DESC_T DMAC_DESC[32];


typedef struct fmi_sd_info_t
{
    UINT32  uCardType;      // sd2.0, sd1.1, or mmc
    UINT32  uRCA;           // relative card address
    BOOL    bIsCardInsert;
} FMI_SD_INFO_T;

extern FMI_SD_INFO_T *pSD0, *pSD1;

#define BCH_T15     0x00400000
#define BCH_T12     0x00200000
#define BCH_T8      0x00100000
#define BCH_T4      0x00080000
#define BCH_T24     0x00040000

typedef struct fmi_sm_info_t
{
	UINT32	uBlockPerFlash;
	UINT32	uPagePerBlock;
	UINT32	uSectorPerBlock;
	UINT32	uPageSize;
	UINT32	uBadBlockCount;
	UINT32  uRegionProtect;     // the page number for Region Protect End Address
	UINT32	uNandECC;
	UINT32	uSpareSize;
	BOOL	bIsMulticycle;
	BOOL	bIsMLCNand;
  BOOL    bIsInResetState;
  BOOL    bIsRA224;

} FMI_SM_INFO_T;

extern FMI_SM_INFO_T *pSM;

/* F/W update information */
typedef struct fw_update_info_t
{
    UINT16  imageNo;
    UINT16  imageFlag;
    UINT16  startBlock;
    UINT16  endBlock;
    UINT32  executeAddr;
    UINT32  blockCount;
    CHAR    imageName[16];
} FW_UPDATE_INFO_T;

typedef struct fw_nand_image_t
{
    UINT32  actionFlag;
    UINT32  fileLength;
    UINT32  imageNo;
    CHAR    imageName[16];
    UINT32  imageType;
    UINT32  executeAddr;    // endblock
    UINT32  blockNo;        // startblock
    UINT32  dummy;
		UCHAR macaddr[8];
		UINT32 initSize;
} FW_NAND_IMAGE_T;

typedef struct fw_nor_image_t
{
    UINT32  actionFlag;		
    UINT32  fileLength;
	union{
    UINT32  imageNo;
		UINT32  num;
	};
    CHAR    imageName[16];
    UINT32  imageType;
    UINT32  executeAddr;
    UINT32  flashOffset;
    UINT32  endAddr;
		UCHAR   macaddr[8];	
		UINT32  initSize;
} FW_NOR_IMAGE_T;

//MMC---------------------------------
typedef struct fw_mmc_image_t
{
    UINT32  actionFlag;
    UINT32  fileLength;
    UINT32  imageNo;
    CHAR    imageName[16];
    UINT32  imageType;
    UINT32  executeAddr;
    UINT32  flashOffset;
    UINT32  endAddr;
    UINT32  ReserveSize;  //unit of sector
    UCHAR   macaddr[8];
    UINT32  initSize;
} FW_MMC_IMAGE_T;
//------------------------------------

//MTP---------------------------------
typedef struct fw_otp_image_t
{
    UCHAR   Mode;
    UCHAR   Option;
    UCHAR   Lock;
    UCHAR   Reserved[2];
    UINT32  KeyLen;
    UINT32  KeyOTP[8];
} FW_OTP_IMAGE_T;
//------------------------------------

//PACK--------------------------------
typedef struct _PACK_CHILD_HEAD
{
	UINT32 filelen;
	UINT32 startaddr;
	UINT32 imagetype;
	UINT32 reserve[1];
}PACK_CHILD_HEAD,*PPACK_CHILD_HEAD;

typedef struct _PACK_HEAD
{
	UINT32 actionFlag;
	UINT32 fileLength;		 
	UINT32 num;
	UINT32 reserve[1];
}PACK_HEAD,*PPACK_HEAD;
//------------------------------------

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

// function declaration
#ifdef DMAC_SCATTER_GETTER
INT  dmacSetDescriptor(UINT32 uaddr, UINT32 ucount);
#endif

// SD functions
INT  fmiSDCommand(FMI_SD_INFO_T *pSD, UINT8 ucCmd, UINT32 uArg);
INT  fmiSDCmdAndRsp(FMI_SD_INFO_T *pSD, UINT8 ucCmd, UINT32 uArg, INT nCount);
INT  fmiSDCmdAndRsp2(FMI_SD_INFO_T *pSD, UINT8 ucCmd, UINT32 uArg, UINT32 *puR2ptr);
INT  fmiSD_Init(FMI_SD_INFO_T *pSD);
INT  fmiSelectCard(FMI_SD_INFO_T *pSD);
VOID fmiGet_SD_info(FMI_SD_INFO_T *pSD, DISK_DATA_T *_info);
INT  fmiSD_Read_in(FMI_SD_INFO_T *pSD, UINT32 uSector, UINT32 uBufcnt, UINT32 uDAddr);
INT  fmiSD_Write_in(FMI_SD_INFO_T *pSD, UINT32 uSector, UINT32 uBufcnt, UINT32 uSAddr);

// SM functions
INT fmiSMCheckRB(void);
INT fmiSM_Reset(void);
VOID fmiSM_Initial(FMI_SM_INFO_T *pSM);
INT fmiSM_ReadID(FMI_SM_INFO_T *pSM);
INT fmiSM2BufferM_large_page(UINT32 uPage, UINT32 ucColAddr);
INT fmiSM_Read_RA(UINT32 uPage, UINT32 ucColAddr);
INT fmiCheckInvalidBlock(FMI_SM_INFO_T *pSM, UINT32 BlockNo);
INT fmiSM_BlockErase(FMI_SM_INFO_T *pSM, UINT32 uBlock);
INT fmiSM_BlockEraseBad(FMI_SM_INFO_T *pSM, UINT32 uBlock);
INT fmiMarkBadBlock(FMI_SM_INFO_T *pSM, UINT32 BlockNo);
INT CheckBadBlockMark(FMI_SM_INFO_T *pSM, UINT32 block);
INT fmiSM_ChipErase(UINT32 uChipSel);
INT fmiSM_ChipEraseBad(UINT32 uChipSel);
INT fmiSM_Erase(UINT32 uChipSel, UINT32 start, UINT32 len);
INT fmiSM_EraseBad(UINT32 uChipSel, UINT32 start, UINT32 len);

INT fmiHWInit(void);
INT fmiNandInit(void);
INT fmiSM_Write_large_page(UINT32 uSector, UINT32 ucColAddr, UINT32 uSAddr);
INT fmiSM_Read_large_page(FMI_SM_INFO_T *pSM, UINT32 uPage, UINT32 uDAddr);

INT fmiSM_Write_large_page_oob(UINT32 uSector, UINT32 ucColAddr, UINT32 uSAddr,UINT32 oobsize);
INT fmiSM_Write_large_page_oob2(UINT32 uSector, UINT32 ucColAddr, UINT32 uSAddr);

#define MIN(a,b) ((a) < (b) ? (a) : (b))

#endif /* END __FMI_H__ */
