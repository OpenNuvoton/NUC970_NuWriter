/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: flash.h $
 *
 * $Author: Hpchen0 $
 ******************************************************************************/
/*
 * $History: flash.h $
 * 
 * *****************  Version 1  *****************
 * User: Hpchen0      Date: 07/11/30   Time: 1:57p
 * Created in $/W90P910/Emulation Test Programs/UDC/UPGRADE
 * 
 * *****************  Version 1  *****************
 * User: Hpchen0      Date: 07/11/30   Time: 1:52p
 * Created in $/W90P910/Emulation Test Programs/UDC/UPGRADE
 * 
 * *****************  Version 1  *****************
 * User: Hpchen0      Date: 07/11/21   Time: 9:47a
 * Created in $/W90P910/Emulation Test Programs/UDC/UDC/Upgrade
 * 
 * *****************  Version 6  *****************
 * User: Wschang0     Date: 04/03/19   Time: 5:07p
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Inc
 * 
 * *****************  Version 5  *****************
 * User: Wschang0     Date: 03/08/28   Time: 5:34p
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Inc
 * Add VSS header
 */ 
#ifndef FLASH_H
#define FLASH_H
//---------------------------------------------------------------------------
#include "wbtypes.h"
 

// if no platform.h
#ifndef _PLATFORM_H

//#define FLASH_BASE		(0xAF000000)
extern unsigned int volatile FLASH_BASE;

#define VPint(x)	(*((volatile unsigned int*)(x)))
#define VPshort(x)	(*((volatile unsigned short*)(x)))
#define VPchar(x)	(*((volatile unsigned char*)(x)))
#define ROMCON	VPint(0xB0001004)
#define EXT3CON VPint(0xB0001024)
#if 0 //by cmn
#define CAHCNF	VPint(0xFFF02000)	
#define CAHCON	VPint(0xFFF02004)
#endif

//#define inph(x)		VPshort(x)
//#define outph(x,y)	VPshort(x)=(y);
//#define inpb(x)		VPchar(x)
//#define outpb(x,y)	VPchar(x)=(y);

#endif

// CFI definitions - start
#define _CFI_
#define SELECT_QUERY_MODE(_base_)      do{*(unsigned short volatile *)(((_base_) & 0xFFF00000) + (0x55 << 1)) = 0x98;}while(0)
#define SST_SELECT_QUERY_MODE(_base_)      do{*(unsigned short volatile *)(((_base_) & 0xFFF00000) + (0x555 << 1)) = 0xAA;\
											  *(unsigned short volatile *)(((_base_) & 0xFFF00000) + (0x2AA << 1)) = 0x55;\
											  *(unsigned short volatile *)(((_base_) & 0xFFF00000) + (0x555 << 1)) = 0x98;}while(0)
#define DESELECT_QUERY_MODE(_base_)    do{*(unsigned short volatile *)((_base_) & 0xFFF00000) = 0xF0;\
                                          *(unsigned short volatile *)((_base_) & 0xFFF00000) = 0xFF;}while(0)

#define CFI_READ(_base_, _offset_, _var_)     do{_var_ = *(unsigned short volatile *)(((_base_) & 0xFFF00000) + ((_offset_) << 1));}while(0)
#define CFI_WRITE(_base_, _offset_, _var_)     do{*(unsigned short volatile *)(((_base_) /*& 0xFFF00000*/) + ((_offset_) << 1)) = (_var_);}while(0)

#define SST_CMD_SET      0x0701
#define AMD_CMD_SET      0x0002
#define INTEL_CMD_SET    0x0003

struct cfi_erase_block_region_info {
	unsigned int size;
	unsigned int num;
};


struct cfi_command{
	int (*write) (unsigned int address, unsigned char *data, unsigned int size);
	int (*erase) (unsigned int address, unsigned int size);
};

#ifdef _CFI_
extern struct cfi_command cfiCmd;
extern unsigned int cfiGetFlashSize(void);
extern int cfiGetFlashInfo(void);
extern unsigned int cfiGetBlockSize(unsigned int address);
#endif

// CFI definitions - end

#ifdef _CFI_
extern unsigned int volatile FLASH_BLOCK_SIZE;
#else
#define FLASH_BLOCK_SIZE	(0x10000)
#endif

#define FLASH_NAME_SIZE	16

#define BLOCK_LOCK 0
#define BLOCK_UNLOCK 1


typedef struct
{
	char PID0;
	char PID1;
	char name[FLASH_NAME_SIZE];
	int (*BlockSize)(UINT32 address);
	int (*BlockErase)(UINT32 address,UINT32 size);
	int (*BlockWrite)(UINT32 address, UCHAR * data, UINT32 size);
	int (*ReadPID)(UINT32 address, UCHAR *PID0, UCHAR *PID1 );
	int (*BlockLock)(UINT32 address, UINT32 op);
} flash_t;


#define IMAGE_ACTIVE	0x1		// Only the active image will be processed by bootloader
#define IMAGE_COPY2RAM	0x2		// copy this image to ram
#define IMAGE_EXEC		0x4		// execute this image
#define IMAGE_FILE		0x8		// file image
#define IMAGE_COMPRESSED 0x10	// compressed image, bootloader will unzip it
#define IMAGE_ROMFS 	0x20 


#define SIGNATURE_WORD	0xA0FFFF9F

typedef struct t_footer
{
	UINT32 num;
	UINT32 base;
	UINT32 length;
	UINT32 load_address;
	UINT32 exec_address;
	CHAR name[16];
	UINT32 image_checksum;
	UINT32 signature;
	UINT32 type;
	UINT32 checksum;
} tfooter;


#define MAX_FOOTER_NUM	8
#define MAX_IMAGE_NUM MAX_FOOTER_NUM

typedef struct t_free
{
	UINT32 address;
	UINT32 length;
} tfree;

#define MAX_FREE_NUM 4

#define BOOTER_BLOCK_LENGTH	FLASH_BLOCK_SIZE
extern flash_t flash[];// The supported flash types
extern char * _flash_buffer;

extern UINT32 FlashSize(void); // return the flash size
extern INT FindFlash(void); // function to identify the flash type
extern INT FindFooter(tfooter *** image_footer); //function to find the image footers
extern INT FindImage(UINT32 image_num, tfooter ** image_footer); // function to find image according to image number
extern INT WriteImage(tfooter * image_footer, UINT32 image_source);//function to write image
extern INT CorruptCheck(tfooter * image_footer);//function to check if data corrupt
extern INT DelBlock(UINT32 block);// function to delete a block size of flash memory
extern INT DelImage(UINT32 image_num);// function to delete an image
extern INT WriteFile2Image(tfooter * image_footer, UINT32 buffer, INT file, INT (*read_func)(INT, VOID *, UINT32) );// flash write from file
extern INT ChangeImageType(INT image, INT type);
extern INT ImageCheck(INT image);

//---------------------------------------------------------------------------
#endif
