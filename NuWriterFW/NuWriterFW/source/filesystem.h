/******************************************************************************
 *
 * Copyright (c) 2013 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: filesystem.h $
 *
 * $Author: schung $
 ******************************************************************************/

#ifndef __FILESYSTEM_H_
#define __FILESYSTEM_H_

#include "wbio.h"
#include "fmi.h"

#define BOOT_SECTOR_SIGNATURE  0x55AA

//Partition Table Entry
#pragma   pack(push,1)
typedef struct _PTE {
    UINT8 pteBootIndicator;
    UINT8 pteStartHead;
    UINT8 pteStartSector;
    UINT8 pteStartCylinder;
    UINT8 pteSystemID;
    UINT8 pteEndHead;
    UINT8 pteEndSector;
    UINT8 pteEndCylinder;
    UINT32 pteFirstSector;     // First Partition Sector
    UINT32 ptePartitionSize;
} PTE, *PPTE;
#pragma pack(pop)


//Master Boot Record (Sector)
#pragma   pack(push,1)
typedef struct _MBR {
    UCHAR  mbrBootCommand[446];      // Offset
    // First Partition
    PTE    mbrPartition[4];            //[0]:1BEh, [1]:1CEh, [2]:1DEh, [3]:1EEh
    UINT16 mbrSignature;               // 1FEh ~ 1FFh
} MBR, *PMBR;
#pragma pack(pop)

//CHS Value
#pragma   pack(push,1)
typedef struct _CHS {
    UCHAR  chsTrack;
    UCHAR  chsSector;
    UCHAR  chsCylinder;
} CHS, *PCHS;
#pragma pack(pop)


#define GET16_L(bptr,n)   	(bptr[n] | (bptr[n+1] << 8))
#define GET32_L(bptr,n)   	(bptr[n] | (bptr[n+1] << 8) | (bptr[n+2] << 16) | (bptr[n+3] << 24))
#define PUT16_L(bptr,n,val)	bptr[n] = val & 0xFF;				\
							bptr[n+1] = (val >> 8) & 0xFF;
#define PUT32_L(bptr,n,val)	bptr[n] = val & 0xFF;				\
							bptr[n+1] = (val >> 8) & 0xFF;		\
							bptr[n+2] = (val >> 16) & 0xFF;		\
							bptr[n+3] = (val >> 24) & 0xFF;

#define GET16_B(bptr,n)   	((bptr[n]) << 8 | bptr[n+1])
#define GET32_B(bptr,n)   	((bptr[n] << 24) | (bptr[n+1] << 16) | (bptr[n+2] << 8) | bptr[n+3])
#define PUT16_B(bptr,n,val)	bptr[n+1] = val & 0xFF;				\
							bptr[n] = (val >> 8) & 0xFF;
#define PUT32_B(bptr,n,val)	bptr[n+3] = val & 0xFF;				\
							bptr[n+2] = (val >> 8) & 0xFF;		\
							bptr[n+1] = (val >> 16) & 0xFF;		\
							bptr[n] = (val >> 24) & 0xFF;
#define	min(x,y)			(((x) <	(y)) ? (x) : (y))
//#define	MIN(x,y)			(((x) <	(y)) ? (x) : (y))
#define	max(x,y)			(((x) >	(y)) ? (x) : (y))
#define	MAX(x,y)			(((x) >	(y)) ? (x) : (y))

/*----------------------------------------------------------------------------------------*/
/*  Number of FAT sector buffer                                                           */
/*----------------------------------------------------------------------------------------*/
#define NUM_SECTOR_BUFFER			16		/* !!at least 8!!  */

static UINT32 TotalSector=0;
#if (NUM_SECTOR_BUFFER < 8)
#error "NUM_SECTOR_BUFFER too small!!"
#endif

INT fmiSDWrite(UINT32 uStartSecN,UINT32 nCount,UINT8 *pucSecBuff);
//PMBR create_mbr(UINT32 TotalSize,UINT32 HideSize);
PMBR create_mbr(UINT32 TotalSize, FW_MMC_IMAGE_T *myPmmcImage);
INT32 FormatFat32(PMBR pmbr,UINT32 nCount);
void MBR_DecodingCHS(UINT32 PartitionSize, UINT32 *CIdx, UINT32 *TIdx, UINT32 *SIdx);
#endif /*FILESYSTEM_H_*/
