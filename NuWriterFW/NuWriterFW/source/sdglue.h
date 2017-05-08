/******************************************************************************
 *
 * Copyright (c) 2013 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: sdglue.h $
 *
 * $Author: schung $
 ******************************************************************************/
#ifndef __SDGLUE_H__
#define __SDGLUE_H__

#include "wbtypes.h"
#include "sd.h"

#define SD_SECTOR 512
#define SD_MUL 8
#define MMC_INFO_SECTOR 1

INT  fmiInitSDDevice(void);
INT  fmiSD_Read(UINT32 uSector, UINT32 uBufcnt, UINT32 uDAddr);
INT  fmiSD_Write(UINT32 uSector, UINT32 uBufcnt, UINT32 uSAddr);


void Burn_MMC_RAW(UINT32 len, UINT32 offset,UINT8 *ptr);
void Read_MMC_RAW(UINT32 len, UINT32 offset,UINT8 *ptr);


//------------------------------------------------------------------
int ChangeMMCImageType(UINT32 imageNo, UINT32 imageType);
int SetMMCImageInfo(FW_MMC_IMAGE_T *mmcImageInfo);
UINT32 GetMMCImageInfo(unsigned int *image);
UINT32 GetMMCReserveSpace(void);
void GetMMCImage(void);
int DelMMCImage(UINT32 imageNo);
#endif
