/******************************************************************************
 * @file     filesystem.c
 * @version  V0.10
 * $Revision: 1 $
 * $Date: 13/07/10 1:17p $ 
 * @brief    NUC970 series filesystem source file
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/ 
#include "filesystem.h"
#include "wbio.h"
#include "wblib.h"
#include "sdglue.h"
#include "usbd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SectorSize 512
#define WriteLen  512
static UINT8 bpb_sample[]= {
    0xEB, 0x58, 0x90, 0x4D, 0x53, 0x44, 0x4F, 0x53, 0x35, 0x2E, 0x30, 0x00, 0x02, 0x08, 0x22, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x3F, 0x00, 0xFF, 0x00, 0x3F, 0x00, 0x00, 0x00,
    0xC1, 0x0F, 0x2F, 0x00, 0xBF, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x00, 0x29, 0x2F, 0x39, 0x6C, 0x80, 0x4E, 0x4F, 0x20, 0x4E, 0x41, 0x4D, 0x45, 0x20, 0x20,
    0x20, 0x20, 0x46, 0x41, 0x54, 0x33, 0x32, 0x20, 0x20, 0x20, 0x33, 0xC9, 0x8E, 0xD1, 0xBC, 0xF4,
    0x7B, 0x8E, 0xC1, 0x8E, 0xD9, 0xBD, 0x00, 0x7C, 0x88, 0x4E, 0x02, 0x8A, 0x56, 0x40, 0xB4, 0x08,
    0xCD, 0x13, 0x73, 0x05, 0xB9, 0xFF, 0xFF, 0x8A, 0xF1, 0x66, 0x0F, 0xB6, 0xC6, 0x40, 0x66, 0x0F,
    0xB6, 0xD1, 0x80, 0xE2, 0x3F, 0xF7, 0xE2, 0x86, 0xCD, 0xC0, 0xED, 0x06, 0x41, 0x66, 0x0F, 0xB7,
    0xC9, 0x66, 0xF7, 0xE1, 0x66, 0x89, 0x46, 0xF8, 0x83, 0x7E, 0x16, 0x00, 0x75, 0x38, 0x83, 0x7E,
    0x2A, 0x00, 0x77, 0x32, 0x66, 0x8B, 0x46, 0x1C, 0x66, 0x83, 0xC0, 0x0C, 0xBB, 0x00, 0x80, 0xB9,
    0x01, 0x00, 0xE8, 0x2B, 0x00, 0xE9, 0x48, 0x03, 0xA0, 0xFA, 0x7D, 0xB4, 0x7D, 0x8B, 0xF0, 0xAC,
    0x84, 0xC0, 0x74, 0x17, 0x3C, 0xFF, 0x74, 0x09, 0xB4, 0x0E, 0xBB, 0x07, 0x00, 0xCD, 0x10, 0xEB,
    0xEE, 0xA0, 0xFB, 0x7D, 0xEB, 0xE5, 0xA0, 0xF9, 0x7D, 0xEB, 0xE0, 0x98, 0xCD, 0x16, 0xCD, 0x19,
    0x66, 0x60, 0x66, 0x3B, 0x46, 0xF8, 0x0F, 0x82, 0x4A, 0x00, 0x66, 0x6A, 0x00, 0x66, 0x50, 0x06,
    0x53, 0x66, 0x68, 0x10, 0x00, 0x01, 0x00, 0x80, 0x7E, 0x02, 0x00, 0x0F, 0x85, 0x20, 0x00, 0xB4,
    0x41, 0xBB, 0xAA, 0x55, 0x8A, 0x56, 0x40, 0xCD, 0x13, 0x0F, 0x82, 0x1C, 0x00, 0x81, 0xFB, 0x55,
    0xAA, 0x0F, 0x85, 0x14, 0x00, 0xF6, 0xC1, 0x01, 0x0F, 0x84, 0x0D, 0x00, 0xFE, 0x46, 0x02, 0xB4,
    0x42, 0x8A, 0x56, 0x40, 0x8B, 0xF4, 0xCD, 0x13, 0xB0, 0xF9, 0x66, 0x58, 0x66, 0x58, 0x66, 0x58,
    0x66, 0x58, 0xEB, 0x2A, 0x66, 0x33, 0xD2, 0x66, 0x0F, 0xB7, 0x4E, 0x18, 0x66, 0xF7, 0xF1, 0xFE,
    0xC2, 0x8A, 0xCA, 0x66, 0x8B, 0xD0, 0x66, 0xC1, 0xEA, 0x10, 0xF7, 0x76, 0x1A, 0x86, 0xD6, 0x8A,
    0x56, 0x40, 0x8A, 0xE8, 0xC0, 0xE4, 0x06, 0x0A, 0xCC, 0xB8, 0x01, 0x02, 0xCD, 0x13, 0x66, 0x61,
    0x0F, 0x82, 0x54, 0xFF, 0x81, 0xC3, 0x00, 0x02, 0x66, 0x40, 0x49, 0x0F, 0x85, 0x71, 0xFF, 0xC3,
    0x4E, 0x54, 0x4C, 0x44, 0x52, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x0A, 0x52, 0x65,
    0x6D, 0x6F, 0x76, 0x65, 0x20, 0x64, 0x69, 0x73, 0x6B, 0x73, 0x20, 0x6F, 0x72, 0x20, 0x6F, 0x74,
    0x68, 0x65, 0x72, 0x20, 0x6D, 0x65, 0x64, 0x69, 0x61, 0x2E, 0xFF, 0x0D, 0x0A, 0x44, 0x69, 0x73,
    0x6B, 0x20, 0x65, 0x72, 0x72, 0x6F, 0x72, 0xFF, 0x0D, 0x0A, 0x50, 0x72, 0x65, 0x73, 0x73, 0x20,
    0x61, 0x6E, 0x79, 0x20, 0x6B, 0x65, 0x79, 0x20, 0x74, 0x6F, 0x20, 0x72, 0x65, 0x73, 0x74, 0x61,
    0x72, 0x74, 0x0D, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAC, 0xCB, 0xD8, 0x00, 0x00, 0x55, 0xAA
};
//__align(32) UINT8 g_MemBuff[256*1024]; 
#define get_timer_ticks()		sysGetTicks(TIMER0)

INT fmiSDWrite(UINT32 uStartSecN,UINT32 nCount,UINT8 *pucSecBuff)
{
    fmiSD_Write(uStartSecN, nCount, (UINT32)pucSecBuff);
    return 1;
}

void fsFreeSector(UINT8 *buff){;}

void MBR_DecodingCHS(UINT32 PartitionSize, UINT32 *CIdx, UINT32 *TIdx, UINT32 *SIdx)
{
    UINT32 u32Sector, u32Track, u32Cylinder;//, u32CHSValue;
    UINT32 u32CHS_S, u32CHS_T, u32CHS_C;
    UINT32 u32tempS, u32tempC;

    /*
        1 sector = 512 bytes
        1 Track = 63 sector
        1 Cylinder = 255 Track
    */
    u32Sector = PartitionSize;
    u32Track = u32Sector/63;
    u32tempS = u32Sector%63;

    u32Cylinder = u32Track/255;
    u32CHS_T = u32Track%255;

    MSG_DEBUG("u32Cylinder=0x%08x(%d), u32Track=0x%08x(%d), u32tempS=0x%08x(%d)\n", u32Cylinder, u32Cylinder, u32Track, u32Track, u32tempS, u32tempS);
    /*
        1st     2nd     3rd
        Track   Sector  Cylinder
    */

    u32CHS_C = u32Cylinder & 0xff;
    u32tempC = (u32Cylinder >> 8);
    u32CHS_S = (u32tempC << 6) | (u32tempS&0xff);

    MSG_DEBUG("u32CHS_T=0x%02x(%d), u32CHS_S=0x%04x(%d), u32CHS_C=0x%02x(%d)\n", u32CHS_T, u32CHS_T, u32CHS_S, u32CHS_S, u32CHS_C,u32CHS_C);

    //u32CHSValue = ((u32CHS_C&0xff)<<16) | ((u32CHS_S&0xff)<<8) | (u32CHS_T&0xff);
    MSG_DEBUG("u32CHSValue=0x%08x(%d)\n", u32CHSValue,u32CHSValue);

    *CIdx = u32CHS_C;
    *TIdx = u32CHS_T;
    *SIdx = u32CHS_S;
}

/*
    TotalSize(sectors) : total sector size in the Device
    myPmmcImage        : MMC struct
*/
PMBR create_mbr(UINT32 TotalSecSize, FW_MMC_IMAGE_T *myPmmcImage)
{
    UINT32 paraCIdx=0, paraTIdx=0, paraSIdx=0;
    UINT32 TotalSize=0, tmpRemainSectorSize = 0, tmpRemainSize;
    PMBR mbr=NULL;
    mbr=(PMBR)malloc(sizeof(MBR));
    if(mbr==NULL) return NULL;
    memset(mbr, 0, sizeof(MBR));

    MSG_DEBUG("PartitionNum =%d\n", myPmmcImage->PartitionNum);
    MSG_DEBUG("Partition1Size =%dMB,  SectorSize1=%d\n", myPmmcImage->Partition1Size, (myPmmcImage->Partition1Size)*2*1024);
    myPmmcImage->PartitionS1Size = (myPmmcImage->Partition1Size)*2*1024;///512*1024*1024;
    myPmmcImage->PartitionS2Size = (myPmmcImage->Partition2Size)*2*1024;//512*1024*1024;
    myPmmcImage->PartitionS3Size = (myPmmcImage->Partition3Size)*2*1024;//512*1024*1024;
    myPmmcImage->PartitionS4Size = (myPmmcImage->Partition4Size)*2*1024;//512*1024*1024;
    TotalSize = TotalSecSize*512/1024/1024;
    tmpRemainSectorSize = TotalSecSize - myPmmcImage->ReserveSize;
    tmpRemainSize = TotalSize- (myPmmcImage->ReserveSize*512/1024/1024);
    MSG_DEBUG("create_mbr RemainSize =%dMB,  RemainSectorSize=%d\n", tmpRemainSize, tmpRemainSectorSize);

    mbr->mbrPartition[0].pteStartHead    =0x00;
    mbr->mbrPartition[0].pteStartSector  =0x01;
    mbr->mbrPartition[0].pteStartCylinder=0x00;
    mbr->mbrPartition[0].pteSystemID     =0x0B; //FAT32

    MBR_DecodingCHS(myPmmcImage->PartitionS1Size, &paraCIdx, &paraTIdx, &paraSIdx);
    MSG_DEBUG("SectorP1Size:%d(0x%x) [0x%02x 0x%02x 0x%02x]\n", myPmmcImage->PartitionS1Size, myPmmcImage->PartitionS1Size, paraTIdx, paraSIdx, paraCIdx);
    mbr->mbrPartition[0].pteEndHead      =paraTIdx;
    mbr->mbrPartition[0].pteEndSector    =paraSIdx;
    mbr->mbrPartition[0].pteEndCylinder  =paraCIdx;
    mbr->mbrPartition[0].pteFirstSector  =myPmmcImage->ReserveSize;
    mbr->mbrPartition[0].ptePartitionSize=TotalSecSize-(myPmmcImage->ReserveSize);

    if(myPmmcImage->Partition2Size == 0 && myPmmcImage->Partition1Size != TotalSize)// && myPmmcImage->Partition4Size == 0)
    {
        sysprintf(">>> For Customer Partition\n");
        myPmmcImage->Partition2Size= tmpRemainSize - myPmmcImage->Partition1Size;
        myPmmcImage->PartitionS2Size = (myPmmcImage->Partition2Size)*2*1024;
        MSG_DEBUG("Partition1Size =%d MB,  SectorP1Size=%d\n", myPmmcImage->Partition1Size, (myPmmcImage->Partition1Size)*2*1024);
        MSG_DEBUG("Partition2Size =%d MB,  SectorP2Size=%d\n", myPmmcImage->Partition2Size, (myPmmcImage->Partition2Size)*2*1024);
        mbr->mbrPartition[0].ptePartitionSize=myPmmcImage->PartitionS1Size;
    }
    else
    {


    if (myPmmcImage->PartitionNum > 1) {
        MSG_DEBUG("Partition2Size =%dMB,  SectorSize2=%d\n", myPmmcImage->Partition2Size, (myPmmcImage->Partition2Size)*2*1024);

        mbr->mbrPartition[0].ptePartitionSize=myPmmcImage->PartitionS1Size;

        mbr->mbrPartition[1].pteSystemID     =0x0B; //FAT32
        MBR_DecodingCHS(myPmmcImage->PartitionS1Size, &paraCIdx, &paraTIdx, &paraSIdx);
        MSG_DEBUG("SectorSize1 =%d(0x%x) [0x%02x 0x%02x 0x%02x]\n", myPmmcImage->PartitionS1Size, myPmmcImage->PartitionS1Size, paraTIdx, paraSIdx, paraCIdx);

        mbr->mbrPartition[1].pteStartHead    =paraTIdx;
        mbr->mbrPartition[1].pteStartSector  =paraSIdx+1;
        mbr->mbrPartition[1].pteStartCylinder=paraCIdx;

        MBR_DecodingCHS(myPmmcImage->PartitionS2Size, &paraCIdx, &paraTIdx, &paraSIdx);
        MSG_DEBUG("SectorSize2 =%d(0x%x) [0x%02x 0x%02x 0x%02x]\n", myPmmcImage->PartitionS2Size, myPmmcImage->PartitionS2Size, paraTIdx, paraSIdx, paraCIdx);

        mbr->mbrPartition[1].pteEndHead      =paraTIdx;
        mbr->mbrPartition[1].pteEndSector    =paraSIdx;
        mbr->mbrPartition[1].pteEndCylinder  =paraCIdx;

        mbr->mbrPartition[1].pteFirstSector  =myPmmcImage->PartitionS1Size+myPmmcImage->ReserveSize;
        mbr->mbrPartition[1].ptePartitionSize=TotalSecSize - myPmmcImage->PartitionS1Size - (myPmmcImage->ReserveSize);
    }

    if (myPmmcImage->PartitionNum > 2) {
        MSG_DEBUG("Partition3Size =%dMB,  SectorSize3=%d\n", myPmmcImage->Partition3Size, (myPmmcImage->Partition3Size)*2*1024);
        mbr->mbrPartition[1].ptePartitionSize=myPmmcImage->PartitionS2Size;

        mbr->mbrPartition[2].pteSystemID     =0x0B; //FAT32

        mbr->mbrPartition[2].pteStartHead    =paraTIdx;
        mbr->mbrPartition[2].pteStartSector  =paraSIdx+1;
        mbr->mbrPartition[2].pteStartCylinder=paraCIdx;

        MBR_DecodingCHS(myPmmcImage->PartitionS3Size, &paraCIdx, &paraTIdx, &paraSIdx);
        MSG_DEBUG("SectorSize3 =%d(0x%x) [0x%02x 0x%02x 0x%02x]\n", myPmmcImage->PartitionS3Size, myPmmcImage->PartitionS3Size, paraTIdx, paraSIdx, paraCIdx);
        mbr->mbrPartition[2].pteEndHead      =paraTIdx;
        mbr->mbrPartition[2].pteEndSector    =paraSIdx;
        mbr->mbrPartition[2].pteEndCylinder  =paraCIdx;

        mbr->mbrPartition[2].pteFirstSector  =myPmmcImage->PartitionS1Size+myPmmcImage->PartitionS2Size+myPmmcImage->ReserveSize;
        mbr->mbrPartition[2].ptePartitionSize=TotalSecSize - myPmmcImage->PartitionS1Size - myPmmcImage->PartitionS2Size-myPmmcImage->ReserveSize;
    }

    if (myPmmcImage->PartitionNum > 3) {
        MSG_DEBUG("Partition4Size =%dMB,  SectorSize4=%d\n", myPmmcImage->Partition4Size, (myPmmcImage->Partition4Size)*2*1024);
        mbr->mbrPartition[2].ptePartitionSize=myPmmcImage->PartitionS3Size;

        mbr->mbrPartition[3].pteSystemID     =0x0B; //FAT32
        MBR_DecodingCHS(myPmmcImage->PartitionS3Size, &paraCIdx, &paraTIdx, &paraSIdx);
        MSG_DEBUG("SectorSize3 =%d(0x%x) [0x%02x 0x%02x 0x%02x]\n", myPmmcImage->PartitionS3Size, myPmmcImage->PartitionS3Size, paraTIdx, paraSIdx, paraCIdx);
        mbr->mbrPartition[3].pteStartHead    =paraTIdx;
        mbr->mbrPartition[3].pteStartSector  =paraSIdx+1;
        mbr->mbrPartition[3].pteStartCylinder=paraCIdx;

        MBR_DecodingCHS(myPmmcImage->PartitionS4Size, &paraCIdx, &paraTIdx, &paraSIdx);
        MSG_DEBUG("SectorSize4 =%d(0x%x) [0x%02x 0x%02x 0x%02x]\n", myPmmcImage->PartitionS4Size, myPmmcImage->PartitionS4Size, paraTIdx, paraSIdx, paraCIdx);
        mbr->mbrPartition[3].pteEndHead      =paraTIdx;
        mbr->mbrPartition[3].pteEndSector    =paraSIdx;
        mbr->mbrPartition[3].pteEndCylinder  =paraCIdx;

        mbr->mbrPartition[3].pteFirstSector  =myPmmcImage->PartitionS1Size+myPmmcImage->PartitionS2Size+myPmmcImage->PartitionS3Size+myPmmcImage->ReserveSize;
        mbr->mbrPartition[3].ptePartitionSize=TotalSecSize - myPmmcImage->PartitionS1Size - myPmmcImage->PartitionS2Size - myPmmcImage->PartitionS3Size-myPmmcImage->ReserveSize;
        }
    }

    MSG_DEBUG("[0][0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x %d %d]\n",
              mbr->mbrPartition[0].pteStartHead, mbr->mbrPartition[0].pteStartSector, mbr->mbrPartition[0].pteStartCylinder, mbr->mbrPartition[0].pteSystemID,
              mbr->mbrPartition[0].pteEndHead, mbr->mbrPartition[0].pteEndSector, mbr->mbrPartition[0].pteEndCylinder, mbr->mbrPartition[0].pteFirstSector, mbr->mbrPartition[0].ptePartitionSize);

    MSG_DEBUG("[1][0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x %d %d]\n",
              mbr->mbrPartition[1].pteStartHead, mbr->mbrPartition[1].pteStartSector, mbr->mbrPartition[1].pteStartCylinder, mbr->mbrPartition[1].pteSystemID,
              mbr->mbrPartition[1].pteEndHead, mbr->mbrPartition[1].pteEndSector, mbr->mbrPartition[1].pteEndCylinder, mbr->mbrPartition[1].pteFirstSector, mbr->mbrPartition[1].ptePartitionSize);

    MSG_DEBUG("[2][0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x %d %d]\n",
              mbr->mbrPartition[2].pteStartHead, mbr->mbrPartition[2].pteStartSector, mbr->mbrPartition[2].pteStartCylinder, mbr->mbrPartition[2].pteSystemID,
              mbr->mbrPartition[2].pteEndHead, mbr->mbrPartition[2].pteEndSector, mbr->mbrPartition[2].pteEndCylinder, mbr->mbrPartition[2].pteFirstSector, mbr->mbrPartition[2].ptePartitionSize);

    MSG_DEBUG("[3][0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x %d %d]\n",
              mbr->mbrPartition[3].pteStartHead, mbr->mbrPartition[3].pteStartSector, mbr->mbrPartition[3].pteStartCylinder, mbr->mbrPartition[3].pteSystemID,
              mbr->mbrPartition[3].pteEndHead, mbr->mbrPartition[3].pteEndSector, mbr->mbrPartition[3].pteEndCylinder, mbr->mbrPartition[3].pteFirstSector, mbr->mbrPartition[3].ptePartitionSize);


    mbr->mbrSignature=0xAA55;
    fmiSDWrite(0, 1, (UINT8 *)mbr);
    SendAck(10);
    MSG_DEBUG("10\n");
    return mbr;
}

INT32 FormatFat32(PMBR pmbr,UINT32 nCount)
{
    UINT8  *pucSecBuff=NULL, *pucPtr;
    INT     nSecPerClus;
    UINT32  uFatSize, uRsvSecNum=34, uRootClus=2;
    UINT32  uData1, uData2;
    UINT32  uLogSecNo;
    INT     nWrtSecNum, nStatus;
    UINT8   FAT32Rsv[12] = { 0xF8, 0xFF, 0xFF, 0x0F, 0xFF, 0xFF, 0xFF, 0x0F, 0xFF, 0xFF, 0xFF, 0x0F };
    UINT8   pucCode[] = { 0x04, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x0F,0xFF, 0xFF, 0xFF, 0x0F, 0xF8, 0xFF, 0xFF, 0x0F };
    /*
     * Clear Reserved sectors
    */
    pucSecBuff = (UINT8 *)((UINT32) DOWNLOAD_BASE | NON_CACHE);

    if(pucSecBuff==NULL) return NULL;
    memset(pucSecBuff, 0x0, 512*WriteLen);
    pucPtr=pucSecBuff;
    SendAck(20);
    MSG_DEBUG("20\n");

    if(nCount == 0) {
        for (uLogSecNo = pmbr->mbrPartition[nCount].pteFirstSector, uData1 = 0; uData1 < uRsvSecNum; ) {
            nWrtSecNum = MIN(WriteLen, (uRsvSecNum-uData1));
            nStatus = fmiSDWrite(uLogSecNo, nWrtSecNum, pucSecBuff);
            MSG_DEBUG("uRsvSecNum=%d  nWrtSecNum =%d  uLogSecNo=%d(0x%x)  uData1=%d\n", uRsvSecNum, nWrtSecNum, uLogSecNo, uLogSecNo, uData1);
            if (nStatus < 0) {
                fsFreeSector(pucSecBuff);
                return nStatus;
            }
            uLogSecNo += nWrtSecNum;
            uData1 += nWrtSecNum;
            MSG_DEBUG("uLogSecNo=%d(0x%x)  uData1 =%d \n", uLogSecNo, uLogSecNo, uData1);
        }
    }
    SendAck(25);
    MSG_DEBUG("25\n");
    /*
     * Create the BPB sector
     */
    memcpy(pucSecBuff, bpb_sample, sizeof(bpb_sample));
    pucPtr = pucSecBuff;

    /* determine the cluster size */
    if (pmbr->mbrPartition[nCount].ptePartitionSize < 16777216)
        nSecPerClus = 8; /* < 8G */
    else if (pmbr->mbrPartition[nCount].ptePartitionSize < 33554432)
        nSecPerClus = 16; /* 8G~16G */
    else if (pmbr->mbrPartition[nCount].ptePartitionSize < 67108864)
        nSecPerClus = 32; /* 16GB~32GB */
    else
        nSecPerClus = 64; /* >= 32GB */

    pucSecBuff[13] = nSecPerClus;

    //pmbr->mbrPartition[nCount].pteFirstSector
    /* sector per track */
    PUT16_L(pucPtr,24,pmbr->mbrPartition[nCount].pteEndSector);
    MSG_DEBUG("25-1\n");
    /* number of heads */
    PUT16_L(pucPtr,26,pmbr->mbrPartition[nCount].pteStartHead);
    MSG_DEBUG("25-2\n");
    /* number of hidden sectors preceding the partition */
    PUT32_L(pucPtr,28,pmbr->mbrPartition[nCount].pteStartSector);
    MSG_DEBUG("25-3\n");
    /* write FAT32 total sector number */
    PUT32_L(pucPtr,32,pmbr->mbrPartition[nCount].ptePartitionSize);
    MSG_DEBUG("25-4\n");
    /* calculate and write FAT size */
    uData1 = pmbr->mbrPartition[nCount].ptePartitionSize - (uRsvSecNum + 1);
    uData1 = ((uData1 + nSecPerClus - 1) / nSecPerClus) - 1;  // rough number of clusters
    uFatSize = 1;
    MSG_DEBUG("25-5\n");

    for (uData2 = 0; uData2 < uData1; uData2 += 128) {
        uFatSize ++; // FAT1 and FAT2
        if (((uFatSize * 2) % nSecPerClus) == 0)
            uData1--;
    }
    //_debug_msg("uFatSize=%d\n", uFatSize);
    PUT32_L(pucPtr,36,uFatSize);
    MSG_DEBUG("25-6\n");
    /* Volume serial number */
    PUT32_L(pucPtr,67, get_timer_ticks());
    MSG_DEBUG("25-7\n");
    /* root directory cluster number */
    PUT32_L(pucPtr,44,uRootClus);
    MSG_DEBUG("25-8\n");
    /* pucFSInfo sector */
    PUT16_L(pucPtr,48,1);
    MSG_DEBUG("25-9\n");
    /* BPB bakcup sector */
    PUT16_L(pucPtr,50,6);
    MSG_DEBUG("25-10\n");
    /* write bpb sector */

    nStatus = fmiSDWrite(pmbr->mbrPartition[nCount].pteFirstSector, 1, pucSecBuff);
    if (nStatus < 0) {
        fsFreeSector(pucSecBuff);
        return nStatus;
    }

    /* write backup bpb sector */
    nStatus = fmiSDWrite(pmbr->mbrPartition[nCount].pteFirstSector+6, 1, pucSecBuff);
    if (nStatus < 0) {
        fsFreeSector(pucSecBuff);
        return nStatus;
    }
    SendAck(30);
    MSG_DEBUG("30\n");
    /*
     * Create pucFSInfo sector
     */
    memset(pucSecBuff, 0x0, 512);
    pucSecBuff[510] = 0x55;
    pucSecBuff[511] = 0xAA;

    /* lead signature */
    PUT32_L(pucPtr,0,0x41615252);
    MSG_DEBUG("30-1\n");
    /* structure signature */
    PUT32_L(pucPtr,484,0x61417272);
    MSG_DEBUG("30-2\n");
    /* free cluster count */
    PUT32_L(pucPtr,488,0xFFFFFFFF);
    MSG_DEBUG("30-3\n");
    /* Next free cluster */
    PUT32_L(pucPtr,492,2);
    MSG_DEBUG("30-4\n");
    /* write FS_Info sector */
    nStatus = fmiSDWrite(pmbr->mbrPartition[nCount].pteFirstSector+1, 1, pucSecBuff);
    if (nStatus < 0) {
        fsFreeSector(pucSecBuff);
        return nStatus;
    }

    /* write backup FS_Info sector */
    nStatus = fmiSDWrite(pmbr->mbrPartition[nCount].pteFirstSector+7, 1, pucSecBuff);
    if (nStatus < 0) {
        fsFreeSector(pucSecBuff);
        return nStatus;
    }
    SendAck(40);
    MSG_DEBUG("40\n");
    /*
     * Clear FAT table. The start of FAT table is ptPartition->uStartSecN + uRsvSecNum
     */
    memset(pucSecBuff, 0x0, 512);
    uLogSecNo = pmbr->mbrPartition[nCount].pteFirstSector + uRsvSecNum;
    memcpy(pucSecBuff, pucCode, sizeof(pucCode));
    fmiSDWrite(uLogSecNo, 1, pucSecBuff);
    uLogSecNo++;

    memset(pucSecBuff, 0x0, 512*WriteLen);
    for (uData1 = 0; uData1 < uFatSize*2; ) {
        nWrtSecNum = MIN(WriteLen, (uFatSize*2 - uData1));
        //MSG_DEBUG("nWrtSecNum = %d\n", nWrtSecNum);
        nStatus = fmiSDWrite(uLogSecNo, nWrtSecNum, pucSecBuff);
        if (nStatus < 0) {
            fsFreeSector(pucSecBuff);
            return nStatus;
        }
        uLogSecNo += nWrtSecNum;
        uData1 += nWrtSecNum;
        //if((uData1*40/(uFatSize*2))%5==0)
        {
            SendAck(40+(uData1*40/(uFatSize*2)));
            MSG_DEBUG("%d\n",40+(uData1*40/(uFatSize*2)));
        }
    }
    SendAck(80);
    MSG_DEBUG("80\n");
    /*
     * Clear Root directory cluster
     */
    memset(pucSecBuff, 0x0, 512);
    MSG_DEBUG("80-1\n");
    uLogSecNo = pmbr->mbrPartition[nCount].pteFirstSector + uRsvSecNum + uFatSize * 2 + (uRootClus - 2) * nSecPerClus;
    MSG_DEBUG("80-2\t%d\n", uLogSecNo);
    for (uData1 = 0; uData1 < nSecPerClus; uData1++, uLogSecNo++)
        fmiSDWrite(uLogSecNo, 1, pucSecBuff);
    MSG_DEBUG("80-3\n");
    memcpy(pucSecBuff, FAT32Rsv, 12);
    MSG_DEBUG("80-4\n");
    fmiSDWrite(pmbr->mbrPartition[nCount].pteFirstSector + uRsvSecNum, 1, pucSecBuff);
    MSG_DEBUG("80-5\n");
    fsFreeSector(pucSecBuff);
    MSG_DEBUG("80-6\n");
    SendAck(90);
    MSG_DEBUG("90\n");
    return TRUE;
}
