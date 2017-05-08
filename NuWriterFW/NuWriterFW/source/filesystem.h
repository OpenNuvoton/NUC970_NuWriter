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


         /*
//BPB ( BIOS Parameter Block )
typedef struct _BPB {

    UINT  bpbBytesPerSector;           //
    UCHAR bpbSectorsPerCluster;       //
    UINT  bpbReservedSectors;          //
    UCHAR bpbNumberOfFATs;            //
    UINT  bpbRootEntries;              //
    UINT  bpbTotalSectors;             //
    UCHAR bpbMedia;                   //
    UINT  bpbSectorsPerFAT;            //
    UINT  bpbSectorsPerTrack;          //
    UINT  bpbHeadsPerCylinder;         //
    ULONG bpbHiddenSectors;           //
    ULONG bpb32TotalSectors;          //

} BPB, *PBPB;
  */
    /*
//Partition Boot Sector
#pragma   pack(push,1)
typedef struct _PBS {
	
 UCHAR  pbsJumpCommand[3];          // 000h
 UCHAR  pbsManufacturerName[8];     // 003h

 USHORT  bpbBytesPerSector;          // 00Bh
 UCHAR  bpbSectorsPerCluster;       // 00Dh
 USHORT  bpbReservedSectors;         // 00Eh
 UCHAR  bpbNumberOfFATs;            // 010h
 USHORT  bpbRootEntries;             // 011h FAT12/FAT16, FAT32 is 0
 USHORT  bpbTotalSectors;            // 013h
 UCHAR  bpbMedia;                   // 015h
 USHORT  bpbSectorsPerFAT;           // 016h FAT12/FAT16, FAT32 is 0
 USHORT  bpbSectorsPerTrack;         // 018h
 USHORT  bpbHeadsPerCylinder;        // 01Ah
 ULONG bpbHiddenSectors;           // 01Ch
 ULONG bpb32TotalSectors;          // 020h
 ULONG bpb32SectorsPerFAT;         // 024h ---

 USHORT  bpb32ExtFlags;              // 028h
 USHORT  bpb32FSVer;                 // 02Ah
 ULONG bpb32RootCluster;           // 02Ch
 USHORT  bpb32InfoSector;            // 030h
 USHORT  bpb32BootBackupStart;       // 032h
 UCHAR  bpb32Reserved[12];          // 034h

 UCHAR  pbsReserved[0x1BE];         // 040h

 USHORT   pbsSignature;               // 1FEh = 0x55AA

} PBS, *PPBS;
  */
//File/Directory Entry
//
// Value of
// FirstByte   Meaning
// --------------------------------------------------------------------------
//  00h        Entry has never been used; no further entries follow this one.
//
//  05h        First character of the file name is actually E5h
//
//  2Eh        Entry is an alias for the current directory. If the next byte
//             is 2Eh, the directory entry's disFirstCluster contains the
//             cluster number of the parent of the current directory.
//
//  E5h        File erased
//
/*
#pragma Align_members(1)
typedef struct _DIR {

    UCHAR  dirFilename[8];             // Value of First byte : see below
    UCHAR  dirExtension[3];
    UCHAR  dirAttributes;              // Bit 0: Read-only
                                       //     1: Hidden file
                                       //     2: System file
                                       //     3: Volume label
                                       //     4: Subdirectory
                                       //     5: Archive bit
    UCHAR  dirReserved;
    UCHAR  dirCreateTime[3];
    UCHAR  dirCreateDate[2];
    UCHAR  dirAccessDate[2];
    USHORT dirFirstClusterH;           
    UCHAR  dirUpdateTime[2];
    UCHAR  dirUpdateDate[2];
    USHORT dirFirstCluster;            
    ULONG  dirFileSize;                // Little-Endian

} DIR, *PDIR;
*/
/*
typedef struct _FSPAR {

	ULONG bpbHiddenSectors;
	USHORT  bpbBytesPerSector;          
	UCHAR  bpbSectorsPerCluster;  
	UCHAR  bpbNumberOfFATs;

} FSPAR, *PFSPAR;
*/
/*
typedef struct _FATListforFILE {

	ULONG fatAdr;
	ULONG fatLen;

} FATLISTFORFILE, *PFATLISTFORFILE;
*/


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
#define NUM_SECTOR_BUFFER			16		/* !!at least 8!!                                     */

static UINT32 TotalSector=0;
#if (NUM_SECTOR_BUFFER < 8)
#error "NUM_SECTOR_BUFFER too small!!"
#endif

INT fmiSDWrite(UINT32 uStartSecN,UINT32 nCount,UINT8 *pucSecBuff);
PMBR create_mbr(UINT32 TotalSize,UINT32 HideSize);
INT32 FormatFat32(PMBR pmbr,UINT32 nCount);
#endif /*FILESYSTEM_H_*/
