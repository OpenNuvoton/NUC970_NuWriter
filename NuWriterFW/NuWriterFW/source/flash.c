/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: flash.c $
 *
 * $Author: Hpchen0 $
 ******************************************************************************/
/*
 * $History: flash.c $
 * 
 * *****************  Version 2  *****************
 * User: Hpchen0      Date: 07/12/17   Time: 11:00a
 * Updated in $/W90P910/Emulation Test Programs/UDC/UPGRADE
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
 * *****************  Version 2  *****************
 * User: Yachen       Date: 06/10/13   Time: 1:41p
 * Updated in $/W90P710/Applications/710bootloader/WBLv1_1/Src
 * Exit  software ID mode after read PID is flash is SST39VF6401. Add
 * SST39VF1601 support. 
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/WBLv1_1/Src
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 14  *****************
 * User: Wschang0     Date: 04/06/11   Time: 9:39a
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Add 4Mx32 (16MB) SDRAM support,
 * Add Intel 28F128,28F640 flash types
 * 
 * *****************  Version 13  *****************
 * User: Wschang0     Date: 04/04/20   Time: 10:18a
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Add unlock to all intel flash style
 * 
 * *****************  Version 12  *****************
 * User: Wschang0     Date: 04/04/07   Time: 9:22a
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Add intel flash types
 * 
 * *****************  Version 11  *****************
 * User: Wschang0     Date: 04/03/19   Time: 4:54p
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Add W19L320ST, W19L320SB
 * Arrange the flash array to faster detection
 * 
 * *****************  Version 10  *****************
 * User: Wschang0     Date: 04/01/07   Time: 11:34a
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Increase the timeout count in polling16 function to avoid early timeout
 * in W29LV800BT
 * 
 * *****************  Version 9  *****************
 * User: Wschang0     Date: 03/12/25   Time: 4:06p
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Add W29LV800BT, W29LV160DT, W29LV320DT
 * Remove the FLASH_NUM
 * 
 * *****************  Version 8  *****************
 * User: Wschang0     Date: 03/12/03   Time: 5:16p
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Add MX29LV160BT/TT
 * Fixed MX28F160C3BT/TT.
 * The MX28F160C3 sectors are default to be locked, thus it needs unlock
 * it before write/erase it.
 * 
 * *****************  Version 7  *****************
 * User: Wschang0     Date: 03/11/05   Time: 11:03a
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Add MX28F160C3T & MX28F160C3B
 * 
 * *****************  Version 6  *****************
 * User: Wschang0     Date: 03/09/26   Time: 2:30p
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Correct the flash name of SST39VF160 in the flash[] table
 * 
 * *****************  Version 5  *****************
 * User: Wschang0     Date: 03/08/27   Time: 1:41p
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Add FlushDCache to Write/Erase functions to avoid data cache
 * incohereance after Write/Erase flash.
 * 
 * *****************  Version 4  *****************
 * User: Wschang0     Date: 03/08/27   Time: 11:28a
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Add SST 39VF160 flash type
 * 
 * *****************  Version 3  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:39p
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 11:53a
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Add VSS header
 */

//#include "platform.h"
#include "wbio.h"
#include "flash.h"

extern void SendAck(UINT32 status);

#ifdef _CFI_

//added by cmn [2007/11/30]
#define ROM_SIZE_128K	0x0
#define ROM_SIZE_256K	0x0
#define ROM_SIZE_512K	0x02
#define ROM_SIZE_1M		0x04
#define ROM_SIZE_2M		0x06
#define ROM_SIZE_4M		0x08
#define ROM_SIZE_8M		0x0A
#define ROM_SIZE_16M	0x0C
#define ROM_SIZE_32M	0x0E
#define ROM_SIZE_64M	0x01
#define ROM_SIZE_128M	0x03
#define ROM_SIZE_256M	0x05

#define ROM_8_BIT		0x0
#define ROM_16_BIT		0x01
#define ROM_32_BIT		0x02

//added by cmn [2007/11/30]
static INT SetEBIConfig(int size, int bus_width)
{
	unsigned int value;
	
	value = (unsigned int)(FLASH_BASE << 1  |
						   size << 15	  |
						   bus_width << 2 |						
						   0x0080			);
						   
    outpw(0xB0001004, value);
    
    return 0;
}

static INT ParseDevSize(unsigned int devSize)
{
	int size;
	
	size = devSize / 1024;
	switch (size)
	{
		case 128:
		case 256:
			SetEBIConfig(ROM_SIZE_256K, ROM_16_BIT);
			return 0;
		case 512:
			SetEBIConfig(ROM_SIZE_512K, ROM_16_BIT);
			return 0;
	}

	size = size / 1024;
	switch (size)
	{
		case 1:
			SetEBIConfig(ROM_SIZE_1M, ROM_16_BIT);
			break;
		case 2:
			SetEBIConfig(ROM_SIZE_2M, ROM_16_BIT);
			break;
		case 4:
			SetEBIConfig(ROM_SIZE_4M, ROM_16_BIT);
			break;
		case 8:
			SetEBIConfig(ROM_SIZE_8M, ROM_16_BIT);
			break;
		case 16:
			SetEBIConfig(ROM_SIZE_16M, ROM_16_BIT);
			break;
		case 32:
			SetEBIConfig(ROM_SIZE_32M, ROM_16_BIT);
			break;
		case 64:
			SetEBIConfig(ROM_SIZE_64M, ROM_16_BIT);
			break;
		case 128:
			SetEBIConfig(ROM_SIZE_128M, ROM_16_BIT);
			break;
		case 256:
			SetEBIConfig(ROM_SIZE_256M, ROM_16_BIT);
			break;
	}

	return 0;
}

#define MAX_REGION_NUM	10
unsigned int volatile FLASH_BLOCK_SIZE=0;

static unsigned short cfiExtTab = 0;
unsigned short cfiCmdSet = 0;
unsigned int cfiDevSize = 0;
static int cfiBootFlag = 0; // ==2, bottom boot, ==3, top boot, ==4,5 uniform 
static unsigned short cfiNumEraseBlkRegion = 0;

static struct cfi_erase_block_region_info cfiEraseBlkRegionInfo[MAX_REGION_NUM];
int dummyErase(unsigned int address, unsigned int size);
int dummyWrite(unsigned int address, unsigned char *data, unsigned int size);
struct cfi_command cfiCmd = {dummyWrite, dummyErase};

int sst_flash = 0;


/* Retrun 0: OK, otherwise: CFI not found */
int cfiCheckIdent(void)
{
	unsigned short i[3];
	
	CFI_READ(FLASH_BASE, 0x10, i[0]);
	CFI_READ(FLASH_BASE, 0x11, i[1]);
	CFI_READ(FLASH_BASE, 0x12, i[2]);
	
	if(i[0] == 'Q' && i[1] == 'R' && i[2] == 'Y')
		return(0);
	else {
		//uprintf("%02x %02x %02x\n", i[0], i[1], i[2]);
		return(-1);	
	}
}

unsigned int cfiGetFlashSize(void)
{
	return(cfiDevSize);
}	

unsigned int cfiGetBlockSize(unsigned int address)
{
	int i = 0, offset = 0;
	
	address = (address | 0x80000000) - FLASH_BASE;
	//uprintf("addr %x\n", address);

	if(sst_flash == 1)
		i = 1;
	for(; i < cfiNumEraseBlkRegion; i++) {
		if(address < (offset += (cfiEraseBlkRegionInfo[i].num * cfiEraseBlkRegionInfo[i].size)))
			return(cfiEraseBlkRegionInfo[i].size);	
	}
	//uprintf("get size error\n");
	
	return(0);

}

static void flushDCache(void)
{
    return;
	
}

#if 0
static int polling16(unsigned int addr, unsigned short data)
{
	unsigned short rdata;
	int timeout = 0x600000;
	
	rdata = *(unsigned short volatile *)(addr);
	while( rdata != data )
	{
		rdata = *(unsigned short volatile *)(addr);
		if( !(timeout--) )
		{	
			rdata = *(unsigned short volatile *)(addr);
			if( rdata != data )
			{
				//printf("timeout\n");
				return -1;	
			}
		}
	}

	return 0;
}
#else

static int polling16(unsigned int addr, unsigned short data)
{
	unsigned short	rdata;
	int timeout=0x600000;
	
	rdata=*(unsigned short volatile *)(addr);
	while( rdata != data )
	{
		rdata=*(unsigned short volatile *)(addr);
		if(sst_flash == 1) {		
			if(!(timeout--) ) // don't use D5 for SST flash
			{	
				rdata=*(unsigned short volatile *)(addr);
				if( rdata != data )
				{
					//printf("polling time-out: %x\n",rdata);
					return -1;	//polling time-out
				}
			}		
		} else {
			if( (rdata&0x20) || !(timeout--) )
			{	
				rdata=*(unsigned short volatile *)(addr);
				if( rdata != data )
				{
					//printf("polling time-out: %x\n",rdata);
					return -1;	//polling time-out
				}
			}
		}
	}
//	printf("time: %d\n", timeout);
	return 0;
}
#endif

int dummyWrite(unsigned int address, unsigned char *data, unsigned int size)
{
	return(-1);
}	
int intelWrite(unsigned int address, unsigned char *data, unsigned int size)
{
	unsigned int i;
	int status = 0;
	
	address |= 0x80000000;
	for(i = address; i < address + size; i += 2)
	{
		CFI_WRITE(i, 0, 0x40);
		CFI_WRITE(i, 0, *(unsigned short *)data);
		CFI_WRITE(address, 0, 0x70);
		while(!( *(unsigned short *)(address) & 0x80));	
		if(*(unsigned short *)(address) & 0x19) {
			status = -1;
			goto exit;
		}	
		data+=2;
	}

exit:
	CFI_WRITE(address,0,0xFF);	
	flushDCache();	
	return(status);
}

int amdWrite(unsigned int address, unsigned char *data, unsigned int size)
{
	int status;
	unsigned int i;
	
	address |= 0x80000000;
	for(i = address; i < address+size; i += 2)
	{
		CFI_WRITE(FLASH_BASE, 0x555, 0xAA);
		CFI_WRITE(FLASH_BASE, 0x2AA, 0x55);
		CFI_WRITE(FLASH_BASE, 0x555, 0xA0);
		CFI_WRITE(i, 0, *(unsigned short *)data);
		status = polling16( i, *(unsigned short *)data);
		if(status < 0)
			return(status); // time-out
		data+=2;
	}	
	CFI_WRITE(FLASH_BASE, 0, 0xF0);
	flushDCache();
	
	return(0);

}

int sstWrite(unsigned int address, unsigned char *data, unsigned int size)
{
	int status;
	unsigned int i;
	
	address |= 0x80000000;
	for(i = address; i < address+size; i += 2)
	{
		CFI_WRITE(FLASH_BASE, 0x5555, 0xAA);
		CFI_WRITE(FLASH_BASE, 0x2AAA, 0x55);
		CFI_WRITE(FLASH_BASE, 0x5555, 0xA0);
		CFI_WRITE(i, 0, *(unsigned short *)data);
		status = polling16( i, *(unsigned short *)data);
		if(status < 0)
			return(status); // time-out
		data+=2;
	}	
	CFI_WRITE(FLASH_BASE, 0, 0xF0);
	flushDCache();
	
	return(0);

} 

int intelErase(unsigned int address, unsigned int size)
{
	int status = 0;

	address|=0x80000000;
	
	CFI_WRITE(address, 0, 0x50); // Clear sttaus register
	CFI_WRITE(address, 0, 0x20);
	CFI_WRITE(address, 0, 0xD0);
	
	CFI_WRITE(address, 0, 0x70);
	while(!( *(unsigned short *)(address) & 0x80));
	if(*(unsigned short *)(address) & 0x39)
		status = -1;

	CFI_WRITE(address, 0, 0xFF);
	flushDCache();	
	return(status);
}

int dummyErase(unsigned int address, unsigned int size)
{
	return(-1);	
}	
int amdErase(unsigned int address, unsigned int size)
{
	int status;
	
	
	if((address & (size - 1)) != 0x0)
		return -1;// not in the start of a block
	address |= 0x80000000;
	//printf("erase addr: %08x\n", address);
	CFI_WRITE(FLASH_BASE, 0x555, 0xAA);
	CFI_WRITE(FLASH_BASE, 0x2AA, 0x55);
	CFI_WRITE(FLASH_BASE, 0x555, 0x80);
	CFI_WRITE(FLASH_BASE, 0x555, 0xAA);
	CFI_WRITE(FLASH_BASE, 0x2AA, 0x55);
	CFI_WRITE(address, 0, 0x30);
	status = polling16(address, 0xFFFF);
	CFI_WRITE(FLASH_BASE, 0, 0xF0);
	flushDCache();
	return(status);

}

int sstErase(unsigned int address, unsigned int size)
{
	int status;
	
	
	if((address & (size - 1)) != 0x0)
		return -1;// not in the start of a block
	address |= 0x80000000;
	//printf("erase addr: %08x\n", address);
	CFI_WRITE(FLASH_BASE, 0x5555, 0xAA);
	CFI_WRITE(FLASH_BASE, 0x2AAA, 0x55);
	CFI_WRITE(FLASH_BASE, 0x5555, 0x80);
	CFI_WRITE(FLASH_BASE, 0x5555, 0xAA);
	CFI_WRITE(FLASH_BASE, 0x2AAA, 0x55);
	//CFI_WRITE(address, 0, 0x50);  // sector erase
	CFI_WRITE(address, 0, 0x30);  // block erase
	status = polling16(address, 0xFFFF);
	CFI_WRITE(FLASH_BASE, 0, 0xF0);
	flushDCache();
	return(status);

}
int cfiGetFlashInfo(void)
{

	int i, size = 0;
	static int ok = 0;
	
	if(ok == 1)
		return(0);

	// goes into query mode
	DESELECT_QUERY_MODE(FLASH_BASE);
	SELECT_QUERY_MODE(FLASH_BASE);

	if(cfiCheckIdent() != 0) {
		SST_SELECT_QUERY_MODE(FLASH_BASE);
		if(cfiCheckIdent() != 0) {
			//uprintf("No CFI information found\n");
			return(-2);
		} else {
			sst_flash = 1;
		}		
	}
		
	CFI_READ(FLASH_BASE, 0x13, cfiCmdSet);
	CFI_READ(FLASH_BASE, 0x15, cfiExtTab);
	CFI_READ(FLASH_BASE, 0x27, cfiDevSize);
	CFI_READ(FLASH_BASE, cfiExtTab + 0xf, cfiBootFlag);
	cfiDevSize = 1 << cfiDevSize;
	ParseDevSize(cfiDevSize);	// set EBI config
	
	CFI_READ(FLASH_BASE, 0x2C, cfiNumEraseBlkRegion);	
	//cfiEraseBlkRegionInfo = (struct cfi_erase_block_region_info *)malloc(sizeof(struct cfi_erase_block_region_info) * cfiNumEraseBlkRegion);
	
	if(cfiNumEraseBlkRegion > MAX_REGION_NUM) {
		//uprintf("Out of memory\n");
		return(-3);	
	}

	if(cfiBootFlag == 3) { // Top Boot
		int j;
		for(j = 0, i = (cfiNumEraseBlkRegion - 1) * 4; i >= 0; j++, i -= 4) {
			unsigned short s1, s2;
			CFI_READ(FLASH_BASE, 0x2D + i, s1);
			CFI_READ(FLASH_BASE, 0x2E + i, s2);
		
			cfiEraseBlkRegionInfo[j].num = s1 + (s2 << 8) + 1;
			//printf("num %d ", (cfiEraseBlkRegionInfo + i/4)->num);
		
			CFI_READ(FLASH_BASE, 0x2F + i, s1);
			CFI_READ(FLASH_BASE, 0x30 + i, s2);	
		
			cfiEraseBlkRegionInfo[j].size = (s1 + (s2 << 8)) * 256;
			//printf("size %x \n", (cfiEraseBlkRegionInfo + i/4)->size);
	        size += cfiEraseBlkRegionInfo[j].num * cfiEraseBlkRegionInfo[j].size;
			
	        if(size > cfiDevSize) {
	            // For SST flash, block & sector locates in different "Block regions", first is sector, next is block
	 //           cfiEraseBlkRegionInfo[j].size = 0;
	 //           cfiEraseBlkRegionInfo[j].num = 0;
	            sst_flash = 1;
	            break; 
	        }			
	
		}	
	
	} else {
		for(i = 0; i < cfiNumEraseBlkRegion * 4; i += 4) {
			unsigned short s1, s2;
			CFI_READ(FLASH_BASE, 0x2D + i, s1);
			CFI_READ(FLASH_BASE, 0x2E + i, s2);
		
			cfiEraseBlkRegionInfo[i/4].num = s1 + (s2 << 8) + 1;
			//printf("num %d ", (cfiEraseBlkRegionInfo + i/4)->num);
		
			CFI_READ(FLASH_BASE, 0x2F + i, s1);
			CFI_READ(FLASH_BASE, 0x30 + i, s2);	
		
			cfiEraseBlkRegionInfo[i/4].size = (s1 + (s2 << 8)) * 256;
			//printf("size %x \n", (cfiEraseBlkRegionInfo + i/4)->size);
			size += cfiEraseBlkRegionInfo[i/4].num * cfiEraseBlkRegionInfo[i/4].size;

	        if(size > cfiDevSize) {
	   //         cfiEraseBlkRegionInfo[i/4].size = 0;
	   //         cfiEraseBlkRegionInfo[i/4].num = 0;
	            sst_flash = 1;
	            break; 
	        }			
	
		}
	}
	
	DESELECT_QUERY_MODE(FLASH_BASE);

	for(i=0; i < cfiNumEraseBlkRegion; i++)
	{
		if (cfiEraseBlkRegionInfo[i].size > FLASH_BLOCK_SIZE)
			FLASH_BLOCK_SIZE = cfiEraseBlkRegionInfo[i].size;	
	}

	if(cfiCmdSet == AMD_CMD_SET) {
		cfiCmd.write = amdWrite;
		cfiCmd.erase = amdErase;
	}else  if(cfiCmdSet == INTEL_CMD_SET) {

		cfiCmd.write = intelWrite;
		cfiCmd.erase = intelErase;	

	}
#if 0
	 else 	if(cfiCmdSet == SST_CMD_SET) {
		cfiCmd.write = sstWrite;
		cfiCmd.erase = sstErase;
	
	}
#endif	 
	else {
		//uprintf("Command set %04x not support yet!\n", cfiCmdSet);
		return(-1);
	}


//exit:
	ok = 1;
	return(0);

}
#else

#define DELAY_1US 20

UINT32 _flash_size;

extern void SendAck(UINT32 status);
/* W28J800TT */
INT BlockSize_W28J800TT(UINT32 address);
INT BlockSize_W28J800BT(UINT32 address);
INT BlockErase_W28J800TT(UINT32 address,UINT32 size);
INT BlockWrite_W28J800TT(UINT32 address, UCHAR * data, UINT32 size);
INT ReadPID_W28J800TT(UINT32 address, UCHAR *PID0, UCHAR *PID1);
INT BlockLock_W28J800TT(UINT32 address, UINT32 op);
/* W28J160TT */
INT BlockSize_W28J160TT(UINT32 address);
INT ReadPID_W28J160TT(UINT32 address, UCHAR *PID0, UCHAR *PID1);
/* W28J160BT */
INT BlockSize_W28J160BT(UINT32 address);
/* W28J320TT */
INT BlockSize_W28J320TT(UINT32 address);
INT ReadPID_W28J320TT(UINT32 address, UCHAR *PID0, UCHAR *PID1);
/* W28J320BT */
INT BlockSize_W28J320BT(UINT32 address);

/* M29W320DB */
INT ReadPID_M29WL320DB(UINT32 address, UCHAR *pid0, UCHAR *pid1);
INT BlockWrite_M29W320DT(UINT32 address, UCHAR * data, UINT32 size);
INT BlockSize_M29W320DT(UINT32 address);

/* W39L010 */
INT BlockSize_W39L010(UINT32 address);
INT BlockErase_W39L010(UINT32 address,UINT32 size);
INT BlockWrite_W39L010(UINT32 address, UCHAR * data, UINT32 size);
INT ReadPID_W39L010(UINT32 address, UCHAR *PID0, UCHAR *PID1);
INT BlockLock_W39L010(UINT32 address, UINT32 op);
/* W29EE011 */
INT BlockSize_W29EE011(UINT32 address);
INT BlockErase_W29EE011(UINT32 address,UINT32 size);
INT BlockWrite_W29EE011(UINT32 address, UCHAR * data, UINT32 size);
INT ReadPID_W29EE011(UINT32 address, UCHAR *PID0, UCHAR *PID1);
INT BlockLock_W29EE011(UINT32 address, UINT32 op);
/* AM29LV800BB */
INT BlockSize_AM29LV800BB(UINT32 address);
INT BlockErase_AM29LV800BB(UINT32 address,UINT32 size);
INT BlockWrite_AM29LV800BB(UINT32 address, UCHAR * data, UINT32 size);
INT ReadPID_AM29LV800BB(UINT32 address, UCHAR *PID0, UCHAR *PID1);
INT BlockLock_AM29LV800BB(UINT32 address, UINT32 op);

/* AM29LV800BT */
INT BlockSize_AM29LV800BT(UINT32 address);
/* AM29LV160BB */
INT ReadPID_AM29LV160DB(UINT32 address, UCHAR *PID0, UCHAR *PID1);
INT BlockSize_AM29LV160DB(UINT32 address);
/* AM29LV160BT */
INT BlockSize_AM29LV160DT(UINT32 address);

/* AM29LV320DB */
INT ReadPID_AM29LV320DB(UINT32 address, UCHAR *PID0, UCHAR *PID1);
INT BlockSize_AM29LV320DB(UINT32 address);
/* AM29LV320DT */
INT BlockSize_AM29LV320DT(UINT32 address);

/* SST39VF6401 */
INT ReadPID_SST39VF6401(UINT32 address, UCHAR *pid0, UCHAR *pid1);
INT BlockErase_SST39VF6401(UINT32 address,UINT32 size);
INT BlockWrite_SST39VF6401(UINT32 address, UCHAR * data, UINT32 size);
/* Intel E28F640 */
INT BlockSize_E28F640(UINT32 address);
INT BlockErase_E28F640(UINT32 address,UINT32 size);
INT BlockWrite_E28F640(UINT32 address, UCHAR * data, UINT32 size);
INT ReadPID_E28F640(UINT32 address, UCHAR *PID0, UCHAR *PID1);
INT BlockLock_E28F640(UINT32 address, UINT32 op);
/* Intel E28F320 */
INT ReadPID_E28F320(UINT32 address, UCHAR *PID0, UCHAR *PID1);
/* Intel E28F128 */
INT ReadPID_E28F128(UINT32 address, UCHAR *PID0, UCHAR *PID1);

/* SST 39VF160 */
INT BlockSize_SST39VF160(UINT32 address);
INT ReadPID_SST39VF160(UINT32 address, UCHAR *PID0, UCHAR *PID1);
INT BlockErase_SST39VF160(UINT32 address,UINT32 size);
/* MX28F160C3B */
INT BlockErase_MX28F160C3B(UINT32 address,UINT32 size);
INT BlockWrite_MX28F160C3B(UINT32 address, UCHAR * data, UINT32 size);
INT ReadPID_SST39VF1601(UINT32 address, UCHAR *pid0, UCHAR *pid1);


flash_t flash[]={
#if 0
{0xDA,0x2A,"W19L320SB",BlockSize_AM29LV320DB,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV320DB,BlockLock_AM29LV800BB},
{0x01,0xF9,"AM29LV320DB",BlockSize_AM29LV320DB,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV320DB,BlockLock_AM29LV800BB},
{0x01,0xF6,"AM29LV320DT",BlockSize_AM29LV320DT,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV320DB,BlockLock_AM29LV800BB},
{0x01,0x5B,"AM29LV800BB",BlockSize_AM29LV800BB,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV800BB,BlockLock_AM29LV800BB},
{0x01,0xDA,"AM29LV800BT",BlockSize_AM29LV800BT,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV800BB,BlockLock_AM29LV800BB},
{0x01,0x49,"AM29LV160DB",BlockSize_AM29LV160DB,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV160DB,BlockLock_AM29LV800BB},
{0x01,0xC4,"AM29LV160DT",BlockSize_AM29LV160DT,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV160DB,BlockLock_AM29LV800BB},
{0x7F,0x49,"EN29LV160AB",BlockSize_AM29LV160DB,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV160DB,BlockLock_AM29LV800BB},
{0x7F,0xC4,"EN29LV160AT",BlockSize_AM29LV160DT,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV160DB,BlockLock_AM29LV800BB},
{0xBF,0x82,"SST39VF160",BlockSize_SST39VF160,BlockErase_SST39VF160,BlockWrite_AM29LV800BB,ReadPID_SST39VF160,BlockLock_AM29LV800BB},
{0xAD,0x49,"HY29LV160",BlockSize_AM29LV160DB,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV160DB,BlockLock_AM29LV800BB},
{0xC2,0xC2,"MX28F160C3T",BlockSize_W28J160TT,BlockErase_MX28F160C3B,BlockWrite_MX28F160C3B,ReadPID_W28J160TT,BlockLock_W28J800TT},
{0xC2,0xC3,"MX28F160C3B",BlockSize_W28J160BT,BlockErase_MX28F160C3B,BlockWrite_MX28F160C3B,ReadPID_W28J160TT,BlockLock_W28J800TT},
{0xC2,0x49,"MX29LV160BT",BlockSize_AM29LV160DB,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV160DB,BlockLock_AM29LV800BB},
{0x04,0x49,"MBM29LV160BE",BlockSize_AM29LV160DB,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV160DB,BlockLock_AM29LV800BB},
{0x04,0xC4,"MBM29LV160TE",BlockSize_AM29LV160DT,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV160DB,BlockLock_AM29LV800BB},
{0xDA,0x92,"W19B322MB",BlockSize_AM29LV320DB,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV320DB,BlockLock_AM29LV800BB},
{0x20,0xCA,"M29WL320DT",/*BlockSize_AM29LV320DT*/BlockSize_M29W320DT,BlockErase_AM29LV800BB,BlockWrite_M29W320DT,ReadPID_M29WL320DB,BlockLock_AM29LV800BB},
#endif
//{0x20,0xCB,"M29WL320DB",BlockSize_AM29LV320DB,BlockErase_AM29LV800BB,BlockWrite_M29W320DT,ReadPID_M29WL320DB,BlockLock_AM29LV800BB},
//{0xDA,0x10,"W19B322MT",BlockSize_AM29LV320DT,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV320DB,BlockLock_AM29LV800BB},
//{0xDA,0x13,"W19B323MT",BlockSize_AM29LV320DT,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV320DB,BlockLock_AM29LV800BB},
//{0xDA,0x16,"W19B324MT",BlockSize_AM29LV320DT,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV320DB,BlockLock_AM29LV800BB},
{0xDA,0xBA,"W19L320ST",BlockSize_AM29LV320DT,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV320DB,BlockLock_AM29LV800BB},
{0xDA,0x7E,"W19B320ABT",BlockSize_AM29LV320DB,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV320DB,BlockLock_AM29LV800BB},
{0xDA,0x2A,"W19B320BBT",BlockSize_AM29LV320DB,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV320DB,BlockLock_AM29LV800BB},
{0xB0,0xEC,"W28J800TT",BlockSize_W28J800TT,BlockErase_W28J800TT,BlockWrite_W28J800TT,ReadPID_W28J800TT,BlockLock_W28J800TT},
{0xB0,0xED,"W28J800BT",BlockSize_W28J800BT,BlockErase_W28J800TT,BlockWrite_W28J800TT,ReadPID_W28J800TT,BlockLock_W28J800TT},
{0xB0,0xE8,"W28J160TT",BlockSize_W28J160TT,BlockErase_W28J800TT,BlockWrite_W28J800TT,ReadPID_W28J160TT,BlockLock_W28J800TT},
{0xB0,0xE9,"W28J160BT",BlockSize_W28J160BT,BlockErase_W28J800TT,BlockWrite_W28J800TT,ReadPID_W28J160TT,BlockLock_W28J800TT},
{0xB0,0xE2,"W28J320TT",BlockSize_W28J320TT,BlockErase_W28J800TT,BlockWrite_W28J800TT,ReadPID_W28J320TT,BlockLock_W28J800TT},
{0xB0,0xE3,"W28J320BT",BlockSize_W28J320BT,BlockErase_W28J800TT,BlockWrite_W28J800TT,ReadPID_W28J320TT,BlockLock_W28J800TT},
{0x89,0x16,"INTEL E28F320",BlockSize_E28F640,BlockErase_E28F640,BlockWrite_E28F640,ReadPID_E28F320,BlockLock_E28F640},
{0x89,0x17,"INTEL E28F640",BlockSize_E28F640,BlockErase_E28F640,BlockWrite_E28F640,ReadPID_E28F640,BlockLock_E28F640},
{0xBF,0x6B,"SST39VF6401",BlockSize_E28F640,BlockErase_SST39VF6401,BlockWrite_SST39VF6401,ReadPID_SST39VF6401,BlockLock_AM29LV800BB},
{0xBF,0x4B,"SST39VF1601",BlockSize_E28F640,BlockErase_SST39VF6401,BlockWrite_SST39VF6401,ReadPID_SST39VF1601,BlockLock_AM29LV800BB},
{0x89,0x18,"INTEL E28F128",BlockSize_E28F640,BlockErase_E28F640,BlockWrite_E28F640,ReadPID_E28F128,BlockLock_E28F640},
{0x89,0xC0,"28F800C3-T",BlockSize_W28J800TT,BlockErase_W28J800TT,BlockWrite_W28J800TT,ReadPID_W28J800TT,BlockLock_W28J800TT},
{0x89,0xC1,"28F800C3-B",BlockSize_W28J800BT,BlockErase_W28J800TT,BlockWrite_W28J800TT,ReadPID_W28J800TT,BlockLock_W28J800TT},
{0x89,0xC2,"28F160C3-T",BlockSize_W28J160TT,BlockErase_W28J800TT,BlockWrite_W28J800TT,ReadPID_W28J160TT,BlockLock_W28J800TT},
{0x89,0xC3,"28F160C3-B",BlockSize_W28J160BT,BlockErase_W28J800TT,BlockWrite_W28J800TT,ReadPID_W28J160TT,BlockLock_W28J800TT},
{0x89,0xC4,"28F320C3-T",BlockSize_W28J320TT,BlockErase_W28J800TT,BlockWrite_W28J800TT,ReadPID_W28J320TT,BlockLock_W28J800TT},
{0x89,0xC5,"28F320C3-B",BlockSize_W28J320BT,BlockErase_W28J800TT,BlockWrite_W28J800TT,ReadPID_W28J320TT,BlockLock_W28J800TT},
{0xDA,0x31,"W39L010", BlockSize_W39L010, BlockErase_W39L010, BlockWrite_W39L010, ReadPID_W39L010 ,BlockLock_W39L010 },
{0xDA,0xC1,"W29EE011",BlockSize_W29EE011,BlockErase_W29EE011,BlockWrite_W29EE011,ReadPID_W29EE011,BlockLock_W29EE011},
{0}
};


#define SET_ROM_TYPE

#ifdef SET_ROM_TYPE
//added by cmn [2007/11/30]
//#define ROM_BASE 	0xAF000000

#define ROM_SIZE_128K	0x0
#define ROM_SIZE_256K	0x0
#define ROM_SIZE_512K	0x02
#define ROM_SIZE_1M		0x04
#define ROM_SIZE_2M		0x06
#define ROM_SIZE_4M		0x08
#define ROM_SIZE_8M		0x0A
#define ROM_SIZE_16M	0x0C
#define ROM_SIZE_32M	0x0E
#define ROM_SIZE_64M	0x01
#define ROM_SIZE_128M	0x03
#define ROM_SIZE_256M	0x05

#define ROM_8_BIT		0x0
#define ROM_16_BIT		0x01
#define ROM_32_BIT		0x02
#endif

#define TIMEOUT		8000000UL
static int normal_polling(unsigned int addr, unsigned short mask)
{
	unsigned short	rdata;
	unsigned int cnt;
	
	rdata=inphw(addr)&mask;
	cnt=0;
	while( rdata != mask )
	{
		rdata=inphw(addr)&mask;
		if( cnt++ > TIMEOUT ) return -1; // time-out
	}

	return 0;
}

static int polling16(unsigned int addr, unsigned short data)
{
	unsigned short	rdata;
	int timeout=0x600000;
	
	rdata=inphw(addr);
	while( rdata != data )
	{
		rdata=inphw(addr);
		if( (rdata&0x20) || !(timeout--) )
		{	
			rdata=inphw(addr);
			if( rdata != data )
			{
				//printf("polling time-out: %x\n",rdata);
				return -1;	//polling time-out
			}
		}
	}

	return 0;
}

static int polling16_(unsigned int addr, unsigned short data)
{
	unsigned short	rdata;
	int timeout=0x600000;
	
	rdata=inphw(addr);
	while( rdata != data )
	{
		rdata=inphw(addr);
		if( !(timeout--) )  // SST flash doesn't use D5
		{	
			rdata=inphw(addr);
			if( rdata != data )
			{
			//	printf("polling time-out: %x\n",rdata);
				return -1;	//polling time-out
			}
		}
	}

	return 0;
}


//added by cmn [2007/11/30]
static INT SetEBIConfig(int size, int bus_width)
{
	unsigned int value;
	
	value = (unsigned int)(FLASH_BASE << 1  |
						   size << 15	  |
						   bus_width << 2 |						
						   0x0080			);
						   
    outpw(0xB0001004, value);
    
    return 0;
}


static INT CheckDataWidth(INT w)
{
	INT extio_flag=0;
	// check if the platform (the Flash ROM is not in 0x7F000000)
	if( (ROMCON&0xFF000000)==0xFC000000 )extio_flag=1;

	switch(w)
	{
		case 8:
			if(extio_flag)
			{
				if( (EXT3CON&0x3)== 0x1 )return 1;
				else return 0;
			} 
			else
			{
				if( (ROMCON&0xC)== 0x0 )return 1;
				else return 0;
			}
		case 16:
			if(extio_flag)
			{
				if( (EXT3CON&0x3)== 0x2 )return 1;
				else return 0;
			} 
			else
			{
				if( (ROMCON&0xC)== 0x4 )return 1;
				else return 0;
			}
		case 32:
			if(extio_flag)
			{
				if( (EXT3CON&0x3)== 0x3 )return 1;
				else return 0;
			} 
			else
			{
				if( (ROMCON&0xC)== 0x8 )return 1;
				else return 0;
			}
		default:
			return 0;
	}

}


void FlashDelay(UINT32 delay)
{
	volatile UINT32 i;
	for(i=0;i<delay*DELAY_1US;i++);	
}

static __inline void FlushDCache()
{
#if 0 //for testing
	/* Flush Entire DCache */
	if( CAHCNF & 0x6 ) /* If write buffer or data cache is enabled */
	{
		CAHCON=0x86;
		while( CAHCON );
	}
#endif	
}



INT BlockLock_W28J800TT(UINT32 address, UINT32 op)
{
	address|=0x80000000;

	if( op==BLOCK_LOCK )
	{	
		outphw(address,0x70);
		while( !(inphw(address)&0x80) );
		outphw(address,0x60);
		outphw(address,0x01);
		while( !(inphw(address)&0x80) );
		outphw(address,0xFFFF);
	}
	else if( op==BLOCK_UNLOCK )
	{
		outphw(address,0x70);
		while( !(inphw(address)&0x80) );
		outphw(address,0x60);
		outphw(address,0xD0);
		while( !(inphw(address)&0x80) );
		outphw(address,0xFFFF);
	}
	return 0;
}

INT BlockSize_W28J800TT(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address >= 0xF0000 )
	{
		if( address < _flash_size )
			return 0x2000;
		else
			return 0;
	}
	else
		return 0x10000;
}

INT BlockSize_W28J800BT(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < 0x10000 )
		return 0x2000;
	else
	{
		if( address <  _flash_size )
			return 0x10000;
		else
			return 0;
	}
}


INT BlockErase_W28J800TT(UINT32 address,UINT32 size)
{
	int status;
	if( (address&(size-1))!=0x0 )return -1;// not in the start of a block

	BlockLock_W28J800TT(address,BLOCK_UNLOCK); // The intel flash sector is default to be locked


	address|=0x80000000;
	outphw(address,0x70);
	status=normal_polling( address, 0x80 );
	if( status < 0 )return -1; // polling time-out
	outphw(address,0x20);
	outphw(address,0xD0);
	status=normal_polling( address, 0x80 );
	if( status < 0 )return -1; // polling time-out
	outphw(address,0xFFFF);
	FlushDCache();
	
	return 0;
}

INT BlockWrite_W28J800TT(UINT32 address, UCHAR * data, UINT32 size)
{
	UINT32 i;
	int status;	
	address|=0x80000000;
	
	BlockLock_W28J800TT(address,BLOCK_UNLOCK); // The MX28F160C3 sector is default to be locked
	
	for(i=address;i<address+size;i+=2)
	{
		outphw(i,0x40);
		outphw(i,*((UINT16 *)data));
		//while( !(inphw(i)&0x80) );
		status=normal_polling( i, 0x80 );
		if( status < 0 )return -1; // polling time-out
		data+=2;
	}
	outphw(address,0xFFFF);
	FlushDCache();

	return 0;
}

INT ReadPID_Winbond(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	if( !CheckDataWidth(16) )return -1;

	address|=0x80000000;
	outphw(address,0x70);
	FlashDelay(50000); // delay 50ms
	outphw(address,0x90);
	*pid0=inphw(address);
	*pid1=inphw(address+2);
	outphw(address,0xFFFF);

	return 0;
}

INT ReadPID_W28J800TT(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	int status;

#ifdef SET_ROM_TYPE
	SetEBIConfig(ROM_SIZE_1M, ROM_16_BIT);
#endif
	
	status=ReadPID_Winbond(address,pid0,pid1);
	if( status < 0 )return -1; // not x16 flash
	_flash_size=0x100000;
	return 0;
}

INT ReadPID_W28J800BT(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	int status;

#ifdef SET_ROM_TYPE
	SetEBIConfig(ROM_SIZE_1M, ROM_16_BIT);
#endif	
	
	status=ReadPID_Winbond(address,pid0,pid1);
	if( status < 0 )return -1; // not x16 flash
	_flash_size=0x100000;
	return 0;
}

INT ReadPID_W28J160TT(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	int status;
	
#ifdef SET_ROM_TYPE
	SetEBIConfig(ROM_SIZE_2M, ROM_16_BIT);
#endif

	status=ReadPID_Winbond(address,pid0,pid1);
	if( status < 0 )return -1; // not x16 flash
	_flash_size=0x200000;
	return 0;
}

INT ReadPID_W28J320TT(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	int status;

#ifdef SET_ROM_TYPE
	SetEBIConfig(ROM_SIZE_4M, ROM_16_BIT);
#endif

	status=ReadPID_Winbond(address,pid0,pid1);
	if( status < 0 )return -1; // not x16 flash
	_flash_size=0x400000;
	return 0;
}




INT BlockSize_W28J320TT(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address >= 0x3F0000 )
	{
		if( address < _flash_size )
			return 0x2000;
		else
			return 0;
	}
	else
		return 0x10000;
}


INT BlockSize_W28J320BT(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < 0x10000 )
		return 0x2000;
	else
	{
		if( address <  _flash_size )
			return 0x10000;
		else
			return 0;
	}
}



INT BlockSize_W28J160BT(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < 0x10000 )
		return 0x2000;
	else
	{
		if( address <  _flash_size )
			return 0x10000;
		else
			return 0;
	}
}

INT BlockSize_W28J160TT(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address >= 0x1F0000 )
	{
		if( address < 0x200000 )
			return 0x2000;
		else
			return 0;
	}
	else
		return 0x10000;
}


INT BlockLock_W39L010(UINT32 address, UINT32 op)
{
	return -1;
}

INT BlockSize_W39L010(UINT32 address)
{
	address-=FLASH_BASE;
	address&=0x7FFFFFFF;
	if( address	< _flash_size )
		return 0x20000;
	else
		return 0;
}

INT BlockErase_W39L010(UINT32 address,UINT32 size)
{
	UINT32 addr1,addr2;

	if( (address&(0x20000-1))!=0x0 )return -1;// not in the start of flash

	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555);
	addr2=(address&0xFFF00000)+(0x2AAA);
	
	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0x80);
	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0x10);
	while( (inpb(address)&0x80)!=0x80 );

	FlushDCache();
	
	return 0;
}

INT BlockWrite_W39L010(UINT32 address, UCHAR * data, UINT32 size)
{
	UINT32 i;
	UINT32 addr1, addr2;

	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555);
	addr2=(address&0xFFF00000)+(0x2AAA);
	for(i=address;i<address+size;i++)
	{
		outpb(addr1,0xAA);
		outpb(addr2,0x55);
		outpb(addr1,0xA0);
		outpb(i,*data);
		while( !((inpb(i)&0x80)==(*data&0x80)) );		
		data++;
	}
	FlushDCache();

	return 0;
}

INT ReadPID_W39L010(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	UINT32 addr1, addr2;

#ifdef SET_ROM_TYPE
	SetEBIConfig(ROM_SIZE_128K, ROM_8_BIT);
#endif


	if( !CheckDataWidth(8) )return -1;



	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555);
	addr2=(address&0xFFF00000)+(0x2AAA);

	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0x80);
	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0x60);
	*pid0=inpb(address);
	*pid1=inpb(address+1);
	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0xF0);
	outpb(address,0xFF);

	_flash_size=0x20000;

	return 0;
}



INT BlockLock_W29EE011(UINT32 address, UINT32 op)
{
	return -1;
}


INT BlockSize_W29EE011(UINT32 address)
{
	address-=FLASH_BASE;
	address&=0x7FFFFFFF;
	if( address < _flash_size )
		return 0x80;
	else
		return 0;
}

INT BlockErase_W29EE011(UINT32 address,UINT32 size)
{
	UINT32 addr1,addr2;
	static INT flag=0;

	if( flag )return 0;

	flag=1;		

	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555);
	addr2=(address&0xFFF00000)+(0x2AAA);
	
	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0x80);
	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0x10);
	FlashDelay(50000); // delay 50ms

	FlushDCache();

	return 0;
}

INT BlockWrite_W29EE011(UINT32 address, UCHAR * data, UINT32 size)
{
	UINT32 i;
	UINT32 addr1, addr2;

	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555);
	addr2=(address&0xFFF00000)+(0x2AAA);
	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0xA0);
	for(i=0;i<size;i++)
		outpb(address+i,*(data+i));
	while( inpb(address+i-1)!=*(data+i-1) );
	FlushDCache();

	return 0;
}

INT ReadPID_W29EE011(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	UINT32 addr1, addr2;
	
#ifdef SET_ROM_TYPE
	SetEBIConfig(ROM_SIZE_128K, ROM_8_BIT);
#endif

	
	if( !CheckDataWidth(8) )return -1;


	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555);
	addr2=(address&0xFFF00000)+(0x2AAA);

	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0x80);
	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0x60);
	FlashDelay(10);//delay 10 us
	*pid0=inpb(address);
	*pid1=inpb(address+1);
	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0xF0);
	FlashDelay(10);//delay 10 us
	outpb(address,0xFF);

	_flash_size=0x20000;

	return 0;
}



INT BlockLock_AM29LV800BB(UINT32 address, UINT32 op)
{
	return -1;
}


INT BlockSize_AM29LV800BB(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < 0x4000 )
		return 0x4000;
	else if( address < 0x6000 )
		return 0x2000;
	else if( address < 0x8000 )
		return 0x2000;
	else if( address < 0x10000 )
		return 0x8000;
	else
	{
		if( address <  _flash_size )
			return 0x10000;
		else
			return 0;
	}
}

INT BlockSize_AM29LV800BT(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < 0x0F0000 )
		return 0x10000;
	else if( address < 0xF8000 )
		return 0x8000;
	else if( address < 0xFA000 )
		return 0x2000;
	else if( address < 0xFC000 )
		return 0x2000;
	else
	{
		if( address <  _flash_size )
			return 0x4000;
		else
			return 0;
	}
}



INT BlockErase_AM29LV800BB(UINT32 address,UINT32 size)
{
	UINT32 addr1,addr2;
	INT status;

	if( (address&(size-1))!=0x0 )return -1;// not in the start of a block

	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555<<1);
	addr2=(address&0xFFF00000)+(0x2AAA<<1);
	
	outphw(addr1,0xAA);
	outphw(addr2,0x55);
	outphw(addr1,0x80);
	outphw(addr1,0xAA);
	outphw(addr2,0x55);
	outphw(address,0x30);
	//while( (inphw(address))!=0xFFFF );
	status=polling16(address, 0xFFFF);

	FlushDCache();

	return status;
}

INT BlockWrite_AM29LV800BB(UINT32 address, UCHAR * data, UINT32 size)
{
	UINT32 i;
	UINT32 addr1, addr2;
	INT status;

	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555<<1);
	addr2=(address&0xFFF00000)+(0x2AAA<<1);
	
	for(i=address;i<address+size;i+=2)
	{
		outphw(addr1,0xAA);
		outphw(addr2,0x55);
		outphw(addr1,0xA0);
		outphw(i,*((UINT16*)data));
		status=polling16( i, (*(UINT16 *)data));
		if( status < 0 )return status; // time-out
		data+=2;
	}

	outphw(address,0xFFFF);
	FlushDCache();

	return status;
}


INT ReadPID_AM29LV(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	UINT32 addr1, addr2;

	if( !CheckDataWidth(16) )return -1;


	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555<<1);
	addr2=(address&0xFFF00000)+(0x2AAA<<1);

	outphw(addr1,0xAA);
	outphw(addr2,0x55);
	outphw(addr1,0x90);
	*pid0=(char)inphw(address);
	*pid1=(char)inphw((address+2));
	outphw(address,0xFFFF);

	return 0;
}


INT ReadPID_AM29LV800BB(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	
#ifdef SET_ROM_TYPE
	SetEBIConfig(ROM_SIZE_1M, ROM_16_BIT);
#endif
	
	
	ReadPID_AM29LV(address, pid0, pid1);
	_flash_size=0x100000;

	return 0;
}

INT ReadPID_AM29LV160DB(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{

#ifdef SET_ROM_TYPE
	SetEBIConfig(ROM_SIZE_2M, ROM_16_BIT);
#endif

	ReadPID_AM29LV(address, pid0, pid1);
	_flash_size=0x200000;

	return 0;
}

INT ReadPID_AM29LV320DB(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{

#ifdef SET_ROM_TYPE
	SetEBIConfig(ROM_SIZE_4M, ROM_16_BIT);
#endif


	ReadPID_AM29LV(address, pid0, pid1);
	_flash_size=0x400000;

	return 0;
}
INT BlockErase_SST39VF6401(UINT32 address,UINT32 size)
{
	UINT32 addr1,addr2;
	INT status;

	if( (address&(size-1))!=0x0 )return -1;// not in the start of a block

	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555<<1);
	addr2=(address&0xFFF00000)+(0x2AAA<<1);
	
	outphw(addr1,0xAA);
	outphw(addr2,0x55);
	outphw(addr1,0x80);
	outphw(addr1,0xAA);
	outphw(addr2,0x55);
	outphw(address,0x50);
	//while( (inphw(address))!=0xFFFF );
	status=polling16_(address, 0xFFFF);

	FlushDCache();

	return status;
}

INT BlockWrite_SST39VF6401(UINT32 address, UCHAR * data, UINT32 size)
{
	UINT32 i;
	UINT32 addr1, addr2;
	INT status;

	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555<<1);
	addr2=(address&0xFFF00000)+(0x2AAA<<1);
	
	for(i=address;i<address+size;i+=2)
	{
		outphw(addr1,0xAA);
		outphw(addr2,0x55);
		outphw(addr1,0xA0);
		outphw(i,*((UINT16*)data));
		status=polling16_( i, (*(UINT16 *)data));
		if( status < 0 )return status; // time-out
		data+=2;
	}

	outphw(address,0xFFFF);
	FlushDCache();

	return status;
}
INT ReadPID_SST39VF6401(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	UINT32 addr1, addr2;

#ifdef SET_ROM_TYPE
	SetEBIConfig(ROM_SIZE_8M, ROM_16_BIT);
#endif


	if( !CheckDataWidth(16) )return -1;


	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555<<1);
	addr2=(address&0xFFF00000)+(0x2AAA<<1);

	outphw(addr1,0xAA);
	outphw(addr2,0x55);
	outphw(addr1,0x90);
	*pid0=(char)inphw(address);
	*pid1=(char)inphw((address+2));
	//outphw(address,0xFFFF);
	outphw(address, 0x00F0 ); /* Exit Software ID mode */

	_flash_size=0x800000;

	return 0;

}


INT ReadPID_SST39VF1601(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	UINT32 addr1, addr2;

#ifdef SET_ROM_TYPE
	SetEBIConfig(ROM_SIZE_2M, ROM_16_BIT);
#endif


	if( !CheckDataWidth(16) )return -1;


	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555<<1);
	addr2=(address&0xFFF00000)+(0x2AAA<<1);

	outphw(addr1,0xAA);
	outphw(addr2,0x55);
	outphw(addr1,0x90);
	*pid0=(char)inphw(address);
	*pid1=(char)inphw((address+2));
	//outphw(address,0xFFFF);
	outphw(address, 0x00F0 ); /* Exit Software ID mode */
	_flash_size=0x200000;

	return 0;

}
INT BlockSize_M29W320DT(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < 0x3F0000 )
		return 0x10000;
	else
	{
		if( address < 0x3F8000 )
			return 0x8000;
		else
			if(address < 0x3FC000)
				return 0x2000;
		else
			if(address < _flash_size)
				return 0x4000;
		else		
			return 0;
	}
}

INT BlockWrite_M29W320DT(UINT32 address, UCHAR * data, UINT32 size)
{
	UINT32 i;
	UINT32 addr1, addr2;
	INT status;

	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555<<1);
	addr2=(address&0xFFF00000)+(0x2AAA<<1);
	
	for(i=address;i<address+size;i+=2)
	{
		outphw(addr1,0xAA);
		outphw(addr2,0x55);
		outphw(addr1,0xA0);
		outphw(i,*((UINT16*)data));
		status=polling16( i, (*(UINT16 *)data));
		if( status < 0 )return status; // time-out
		data+=2;
	}

	//outphw(address,0x00F0);
	FlushDCache();

	return status;
}

INT ReadPID_M29WL320DB(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	UINT32 addr1, addr2;

#ifdef SET_ROM_TYPE
	SetEBIConfig(ROM_SIZE_4M, ROM_16_BIT);
#endif


	if( !CheckDataWidth(16) )return -1;


	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555<<1);
	addr2=(address&0xFFF00000)+(0x2AAA<<1);

	outphw(addr1,0xAA);
	outphw(addr2,0x55);
	outphw(addr1,0x90);
	*pid0=(char)inphw(address);
	*pid1=(char)inphw((address+2));
	outphw(address,0x00F0);
	_flash_size=0x400000;
	return 0;
}


INT BlockSize_AM29LV160DB(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < 0x4000 )
		return 0x4000;
	else if( address < 0x6000 )
		return 0x2000;
	else if( address < 0x8000 )
		return 0x2000;
	else if( address < 0x10000 )
		return 0x8000;
	else
	{
		if( address <  _flash_size )
			return 0x10000;
		else
			return 0;
	}
}


INT BlockSize_AM29LV160DT(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < 0x1F0000 )
		return 0x10000;
	else if( address < 0x1F8000 )
		return 0x8000;
	else if( address < 0x1FA000 )
		return 0x2000;
	else if( address < 0x1FC000 )
		return 0x2000;
	else
	{
		if( address <  _flash_size )
			return 0x4000;
		else
			return 0;
	}
}


INT BlockSize_AM29LV320DB(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < 0x10000 )
		return 0x2000;
	else
	{
		if( address < _flash_size )
			return 0x10000;
		else
			return 0;
	}
}

INT BlockSize_AM29LV320DT(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < 0x3F0000 )
		return 0x10000;
	else
	{
		if( address < _flash_size )
			return 0x2000;
		else
			return 0;
	}
}

INT BlockLock_E28F640(UINT32 address, UINT32 op)
{

	address|=0x80000000;

	if( op==BLOCK_LOCK )
	{	
		outphw(address,0x60);
		outphw(address,0x01);
		while( !(inphw(address)&0x80) );
		outphw(address,0xFFFF);
	}
	else if( op==BLOCK_UNLOCK )
	{
		outphw(address,0x60);
		outphw(address,0xD0);
		while( !(inphw(address)&0x80) );
		outphw(address,0xFFFF);
	}

	return 0;
}

INT BlockSize_E28F640(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < _flash_size )
		return 0x10000;
	else
		return 0;
}

INT BlockErase_E28F640(UINT32 address,UINT32 size)
{
	UINT32 i,j,tmp;
	CHAR * buffer=_flash_buffer;
	
	address|=0x80000000;

	// backup the data
	j=0xFFFF;
	if( (address & FLASH_BLOCK_SIZE) )
	{
		
		for(i=0;i<FLASH_BLOCK_SIZE/2;i++)
		{
			tmp=*((volatile unsigned short *)(address-FLASH_BLOCK_SIZE)+i);
			*((volatile unsigned short *)buffer+i)=tmp;
			j&=tmp;
		}
	}
	else
	{
		for(i=0;i<FLASH_BLOCK_SIZE/2;i++)
		{
			tmp=*((volatile unsigned short *)(address+FLASH_BLOCK_SIZE)+i);
			*((volatile unsigned short *)buffer+i)=tmp;
			j&=tmp;
		}
	}
	

	outphw(address,0x20);
	outphw(address,0xD0);
	while( !(inphw(address)&0x80) );
	outphw(address,0xFFFF);

	
	// write back the data
	if( (j&0xFFFF)!=0xFFFF )
	{
		if( (address & FLASH_BLOCK_SIZE) )
			BlockWrite_E28F640(address-FLASH_BLOCK_SIZE, (UCHAR *)buffer, FLASH_BLOCK_SIZE);
		else
			BlockWrite_E28F640(address+FLASH_BLOCK_SIZE, (UCHAR *)buffer, FLASH_BLOCK_SIZE);
	}
	FlushDCache();

	return 0;
}

INT BlockWrite_E28F640(UINT32 address, UCHAR * data, UINT32 size)
{
	UINT32 i;
#if 0
	UINT32 pdata,j;

	address|=0x80000000;
	
	do{
		outphw(address, 0xE8);
	}while( !(inphw(address)&0x80)	);

	i=size;
	while(i)
	{
		if( i < 32 )
		{
			outphw(address, i);
			for(j=0;j<i/2;j++)
				outphw(address+j, *((unsigned short *)pdata+j));
			pdata+=i;
			i=0;
		}
		else
		{
			outphw(address, 32);
			for(j=0;j<16;j++)
				outphw(address+j, *((unsigned short *)pdata+j));
			pdata+=32;
			i-=32;
		}
		outphw(address, 0xD0);
		while( !(inphw(address)&0x80) );	
	}

#else		
	address|=0x80000000;

	for(i=address;i<address+size;i+=2)
	{
		outphw(i,0x40);
		outphw(i,*((UINT16 *)data));
		while( !(inphw(i)&0x80) );
		data+=2;
	}

	outphw(address,0xFFFF);
#endif

	FlushDCache();
	return 0;
}

INT ReadPID_E28F640(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{

#ifdef SET_ROM_TYPE
	SetEBIConfig(ROM_SIZE_8M, ROM_16_BIT);
#endif


	if( !CheckDataWidth(16) )return -1;

	outphw(address,0x90);
	*pid0=inphw(address);
	*pid1=inphw(address+2);
	outphw(address,0xFFFF);

	_flash_size=0x800000;

	return 0;
}

INT ReadPID_E28F128(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{

#ifdef SET_ROM_TYPE
	SetEBIConfig(ROM_SIZE_16M, ROM_16_BIT);
#endif

	if( !CheckDataWidth(16) )return -1;

	outphw(address,0x90);
	*pid0=inphw(address);
	*pid1=inphw(address+2);
	outphw(address,0xFFFF);

	_flash_size=0x1000000;

	return 0;
}

INT ReadPID_E28F320(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{

#ifdef SET_ROM_TYPE
	SetEBIConfig(ROM_SIZE_4M, ROM_16_BIT);
#endif


	if( !CheckDataWidth(16) )return -1;

	outphw(address,0x90);
	*pid0=inphw(address);
	*pid1=inphw(address+2);
	outphw(address,0xFFFF);

	_flash_size=0x400000;

	return 0;
}


INT BlockSize_SST39VF160(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address <  _flash_size )
		return 0x10000;
	else
		return 0;

}


INT ReadPID_SST39VF160(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{

#ifdef SET_ROM_TYPE
	SetEBIConfig(ROM_SIZE_2M, ROM_16_BIT);
#endif

	ReadPID_AM29LV(address, pid0, pid1);
	outphw(address, 0x00F0 ); /* Exit Software ID mode */
	_flash_size=0x200000;

	return 0;
}

INT BlockErase_SST39VF160(UINT32 address,UINT32 size)
{
	UINT32 addr1,addr2;
	INT status;

	if( (address&(size-1))!=0x0 )return -1;// not in the start of a block

	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555<<1);
	addr2=(address&0xFFF00000)+(0x2AAA<<1);
	
	outphw(addr1,0xAA);
	outphw(addr2,0x55);
	outphw(addr1,0x80);
	outphw(addr1,0xAA);
	outphw(addr2,0x55);
	outphw(address,0x50);
	status=polling16(address, 0xFFFF);

	FlushDCache();

	return status;
}

INT BlockErase_MX28F160C3B(UINT32 address,UINT32 size)
{
	int status;
	if( (address&(size-1))!=0x0 )return -1;// not in the start of a block
	
	BlockLock_W28J800TT(address,BLOCK_UNLOCK); // The MX28F160C3 sector is default to be locked

	address|=0x80000000;
	outphw(address,0x70);
	status=normal_polling( address, 0x80 );
	if( status < 0 )return -1; // polling time-out
	outphw(address,0x20);
	outphw(address,0xD0);
	status=normal_polling( address, 0x80 );
	if( status < 0 )return -1; // polling time-out
	outphw(address,0xFFFF);
	FlushDCache();
	
	return 0;
}

INT BlockWrite_MX28F160C3B(UINT32 address, UCHAR * data, UINT32 size)
{
	UINT32 i;
	int status;	

	BlockLock_W28J800TT(address,BLOCK_UNLOCK); // The MX28F160C3 sector is default to be locked

	address|=0x80000000;
	for(i=address;i<address+size;i+=2)
	{
		outphw(i,0x40);
		outphw(i,*((UINT16 *)data));
		//while( !(inphw(i)&0x80) );
		status=normal_polling( i, 0x80 );
		if( status < 0 )return -1; // polling time-out
		data+=2;
	}
	outphw(address,0xFFFF);
	FlushDCache();

	return 0;
}



UINT32 FlashSize()
{
	return _flash_size;
}

#endif // _CFI_


CHAR * _flash_buffer=(CHAR *)0x300000; // must equal to BL_BUFFER_BASE
INT32 volatile _usbd_nor_type = -1;

#ifndef _CFI_
INT FindFlash(void)
{
	INT i;
	UCHAR pid0=0,pid1=0;
	
	if( _usbd_nor_type == -1 )
	{
		i=0;
		while(flash[i].PID0 | flash[i].PID0)
		{
			pid0=pid1=0;
			flash[i].ReadPID(FLASH_BASE, &pid0, &pid1);

#ifdef DEBUG
		printf("PID Check [%d]: TYPE:%s ID:0x%02x 0x%02x\tFlash ID:0x%02x 0x%02x \n",i,flash[i].name,flash[i].PID0,flash[i].PID1,pid0, pid1);
#endif		

			if( (flash[i].PID0 == pid0) && (flash[i].PID1 == pid1) )
			{
				_usbd_nor_type=i;
				break;		
			}
			i++;
		}
	}
	return _usbd_nor_type;
}
#endif


INT FindFooter(tfooter *** image_footer)
{
	static tfooter * image_footers[MAX_FOOTER_NUM];
	UINT32 footer_num=0;
	UINT32 blocksize,addr,i;
	tfooter * footer;
	UINT32 * p;
	unsigned long long sum;
	
	*image_footer=image_footers;
	
#ifdef _CFI_
	if( (_usbd_nor_type=cfiGetFlashInfo())<0 )return -1;
	addr=FLASH_BASE+FLASH_BLOCK_SIZE;	// the 64kb of flash was preservied to boot loader
	blocksize=cfiGetBlockSize(addr);	
#else		
	if( (_usbd_nor_type=FindFlash())<0 )return -1;
	addr=FLASH_BASE+BOOTER_BLOCK_LENGTH;	// the 64kb of flash was preservied to boot loader
	blocksize=flash[_usbd_nor_type].BlockSize(addr);
#endif	
	while( blocksize!= 0 )
	{
		addr+=blocksize;
		footer=(tfooter *)(addr-sizeof(tfooter));
		if(footer->signature==SIGNATURE_WORD)
		{
			p=(UINT32 *)footer;
			sum=0;
			for(i=0;i<sizeof(tfooter)/4-1;i++)// Not include the checksum
			{
				sum+=*(p+i);
			}
			sum = ~((sum&(-1LU))+(sum>>32));
			if( (UINT32)sum == footer->checksum )					
			{
				if( footer_num < MAX_FOOTER_NUM )
					image_footers[footer_num++]=footer;
			}
		}
#ifdef _CFI_
		blocksize=cfiGetBlockSize(addr);
#else				
		blocksize=flash[_usbd_nor_type].BlockSize(addr);
#endif
		
	}
	
	return footer_num;
}

INT WriteImage(tfooter * image_footer, UINT32 image_source)
{
	int tmplen=0;
	UINT32 blockSize,i,src,dest;
	unsigned long long sum;
	UINT32 k;
	UINT32 footer_addr=0;
	
	if( (image_footer->base & (FLASH_BLOCK_SIZE-1)) )
		return -1;	// not block alignment
	
	// check if word aligment
	if( (image_footer->length & 0x3) )
		image_footer->length=(image_footer->length&(~0x3))+4;
	
	// check write corrupt
	if( !CorruptCheck( image_footer )	)
	{
#ifdef _CFI_
		_usbd_nor_type=cfiGetFlashInfo();	
		if(_usbd_nor_type < 0)
			return(-1);
#else		
		_usbd_nor_type=FindFlash();
#endif
		// Write program
		i=image_footer->length;
		src=(UINT32)image_source;
		dest=image_footer->base;
		sum=0;
		while(i)
		{
#ifdef _CFI_
			blockSize=cfiGetBlockSize(dest);		
#else			
			blockSize=flash[_usbd_nor_type].BlockSize(dest);
#endif
			if( i <= blockSize )
			{
#ifdef _CFI_
				cfiCmd.erase(dest, blockSize);
				cfiCmd.write(dest, (UCHAR *)src, i);
#else				
				flash[_usbd_nor_type].BlockErase(dest, blockSize);
				flash[_usbd_nor_type].BlockWrite(dest, (UCHAR *)src, i);
#endif				
				for(k=0;k<i/4;k++)sum+=*((UINT32 *)src+k);
				sum = ~((sum&(-1LU))+(sum>>32));
				image_footer->image_checksum=(UINT32)sum; // checksum of image
				sum=0;
				for(k=0;k<sizeof(tfooter)/4-1;k++)sum+=*((UINT32 *)image_footer+k);
				sum = ~((sum&(-1LU))+(sum>>32));
				image_footer->checksum=(UINT32)sum;		// checksum of footer
				if( ((blockSize-i)<sizeof(tfooter)) ) // Create a new block for footer
				{
#ifdef _CFI_
					cfiCmd.erase(dest+blockSize, blockSize);
					cfiCmd.write(dest+2*blockSize-sizeof(tfooter), (UCHAR *)image_footer, sizeof(tfooter));
#else				
					flash[_usbd_nor_type].BlockErase(dest+blockSize, blockSize);
					flash[_usbd_nor_type].BlockWrite(dest+2*blockSize-sizeof(tfooter), (UCHAR *)image_footer, sizeof(tfooter));
#endif
					footer_addr=dest+2*blockSize-sizeof(tfooter);
				}
				else	// Create the footer at current block
				{
#ifdef _CFI_
					cfiCmd.write(dest+blockSize-sizeof(tfooter), (UCHAR *)image_footer, sizeof(tfooter));
#else
					flash[_usbd_nor_type].BlockWrite(dest+blockSize-sizeof(tfooter), (UCHAR *)image_footer, sizeof(tfooter));
#endif
					footer_addr=dest+blockSize-sizeof(tfooter);
				}
		
				blockSize=i; // Check if > a block size
			}
			else
			{
#ifdef _CFI_
				cfiCmd.erase(dest, blockSize);
				cfiCmd.write(dest, (UCHAR *)src, blockSize);
#else							
				flash[_usbd_nor_type].BlockErase(dest, blockSize);
				flash[_usbd_nor_type].BlockWrite(dest, (UCHAR *)src, blockSize);
#endif
				for(k=0;k<blockSize/4;k++)sum+=*((UINT32 *)src+k);
			}
			src+=blockSize;
			dest+=blockSize;
			i-=blockSize;

			//ack status
			tmplen += blockSize;
			SendAck((tmplen * 95) / image_footer->length);
		}
	}
	else
		return -1;

	// image write finished, check image checksum
	
	sum=0;
	for( i=image_footer->base;i< image_footer->base + image_footer->length ;i+=4 )sum+=*((UINT32 *)i);
	sum = ~((sum&(-1LU))+(sum>>32));
	
	if( (UINT32)sum != image_footer->image_checksum )
		return -1; // image check sum error
		
	// check footer check sum	
	sum=0;
	for( i=footer_addr;i<footer_addr+sizeof(tfooter)-4;i+=4)sum+=*((UINT32 *)i);
	sum = ~((sum&(-1LU))+(sum>>32));
	
	if( (UINT32)sum != image_footer->checksum )
		return -1; // footer check sum error
	
	return 0;

}

INT CorruptCheck(tfooter * image_footer)
{
	UINT32 footer_num;
	tfooter ** footer;
	INT i;
	UINT32 a0,a1,b0,b1;

	
#ifdef _CFI_
	if( image_footer->base < (FLASH_BASE+FLASH_BLOCK_SIZE) )return 1;
	if( image_footer->base+image_footer->length > (FLASH_BASE + cfiGetFlashSize()) )return 1;
#else	
	if( image_footer->base < (FLASH_BASE+BOOTER_BLOCK_LENGTH) )return 1;
	if( image_footer->base+image_footer->length > (FLASH_BASE+FlashSize()) )return 1;
#endif	
	footer_num=FindFooter(&footer);
	for(i=0;i<footer_num;i++)
	{
		if( image_footer->num == footer[i]->num )
			return 1;
	}
	
	
	b0=image_footer->base;
	if( FLASH_BLOCK_SIZE-(image_footer->length%FLASH_BLOCK_SIZE) < sizeof(tfooter) ) 
		b1=image_footer->base+image_footer->length+FLASH_BLOCK_SIZE;
	else
		b1=image_footer->base+image_footer->length;
	for(i=0;i<footer_num;i++)
	{
		a0=footer[i]->base;
		if( (FLASH_BLOCK_SIZE-(footer[i]->length%FLASH_BLOCK_SIZE) < sizeof(tfooter)) || !(footer[i]->length&(FLASH_BLOCK_SIZE-1)) ) 
			a1=footer[i]->base+footer[i]->length+FLASH_BLOCK_SIZE;
		else
			a1=footer[i]->base+footer[i]->length;
		
		if( (b0 >= a0) && (b0 < a1) )return 1;
		if( (b1 >= a0) && (b1 < a1) )return 1;
		if( (a0 > b0) && (a1 < b1) )return 1;
	
	}	

	return 0;
}

INT FindImage(UINT32 image_num, tfooter ** image_footer)
{
	INT footer_num;
	tfooter ** footer;
	INT i;
	
	footer_num=FindFooter(&footer);
	
	for(i=0;i<footer_num;i++)
	{
		if( footer[i]->num == image_num )
		{
			*image_footer=footer[i];
			return 1;
		}
	}
	return 0;
}

INT DelImage(UINT32 image_num)
{
	tfooter * footer;
	UINT32 addr;
	UINT32 blocksize=0;
	UINT32 end, len, tmplen=0;
	
	if( FindImage( image_num, &footer) )
	{
#ifdef _CFI_
		_usbd_nor_type=cfiGetFlashInfo();
		if(_usbd_nor_type < 0) return(-1);
#else	
		_usbd_nor_type=FindFlash();
#endif
		addr=footer->base;
		end=footer->base+footer->length;
		len=(footer->length<blocksize)?blocksize:footer->length;
#ifdef _CFI_
		blocksize=cfiGetBlockSize(/*addr*/end);
#else		
		blocksize=flash[_usbd_nor_type].BlockSize(addr);
#endif
		//end=footer->base+footer->length;
		 // if there are appended block for footer usage
		if((footer->length & (blocksize-1)) + sizeof(tfooter)  > blocksize)
		    end+=blocksize;
		while(addr < end)
		{
#ifdef _CFI_
			blocksize=cfiGetBlockSize(addr);
			cfiCmd.erase(addr,blocksize);
#else					
			blocksize=flash[_usbd_nor_type].BlockSize(addr);
			flash[_usbd_nor_type].BlockErase(addr,blocksize);
#endif
			//ack status
			tmplen += blocksize;
			SendAck((tmplen * 95) / len);

			addr+=blocksize;
		}
		SendAck(100);
		return 0;
	}

	/* send status */
	SendAck(0xFFFFFFFF);
	return -1;
}


INT ChangeImageType(int image, int type)
{
	unsigned int block_base, blocksize;
	tfooter * footer;
	int i;
	long long sum;
	extern INT memcpy(CHAR *dest, CHAR *src, INT size);

	if( FindImage( image, &footer ) )
	{
		block_base = (footer->base + footer->length) & (~(FLASH_BLOCK_SIZE-1));
				 // if there are appended block for footer usage
		blocksize=cfiGetBlockSize(footer->base + footer->length);		 
		if((footer->length & (blocksize-1)) + sizeof(tfooter)  > blocksize)
		    block_base+=blocksize;

		/* A buffer is required to backup the un-change date */
		memcpy( _flash_buffer, (char *)block_base, FLASH_BLOCK_SIZE);

		/* Modify the image attributes */
		footer = (tfooter *) (_flash_buffer + FLASH_BLOCK_SIZE - sizeof(tfooter));
		footer->type = type;

		/* Re-calculate footer check sum */
		sum=0;
		for(i=0;i<(sizeof(tfooter)-4)/4;i++)
		{
			sum+=*((unsigned int *)footer+i);
		}
		footer->checksum=~( (UINT)(sum>>32) + (UINT)sum );

		/* Write-back the new footer */
#ifdef _CFI_
		_usbd_nor_type=cfiGetFlashInfo();
		if(_usbd_nor_type < 0)
			return(-1);
		cfiCmd.erase(block_base, FLASH_BLOCK_SIZE);
		cfiCmd.write(block_base, (UCHAR *)_flash_buffer, FLASH_BLOCK_SIZE);
#else		
		_usbd_nor_type=FindFlash();
		flash[_usbd_nor_type].BlockErase(block_base, FLASH_BLOCK_SIZE);
		flash[_usbd_nor_type].BlockWrite(block_base, (UCHAR *)_flash_buffer, FLASH_BLOCK_SIZE);
#endif
		/* Check the written date */
		for(i=0;i<FLASH_BLOCK_SIZE/4;i++)
		{
			if( *((UINT *)block_base + i)!= *((UINT *)_flash_buffer + i) )
				return -1;
		}
	}
	else
		return -1;
	
	return 0;
}



