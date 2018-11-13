#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NUC970_reg.h"
#include "wblib.h"
#include "fmi.h"

typedef struct
{
	char PID;
	int (*SpiFlashWrite)(UINT32 address, UINT32 len, UCHAR *data);
} spiflash_t;


int wbSpiWrite(UINT32 addr, UINT32 len, UINT8 *buf);
int sstSpiWrite(UINT32 addr, UINT32 len, UINT8 *buf);

int usiEnable4ByteAddressMode(void);
int usiDisable4ByteAddressMode(void);

volatile unsigned char Enable4ByteFlag=0;

spiflash_t spiflash[]={
	{0xEF, wbSpiWrite},
	{0xBF, sstSpiWrite},
	{0}
};

/*****************************************/

INT32 volatile _spi_type = -1;

int usiActive()
{
	unsigned int volatile tick;

	outpw(REG_USI_CNTRL, inpw(REG_USI_CNTRL)|0x01);
	tick = sysGetTicks(TIMER0);
	while(inpw(REG_USI_CNTRL) & 0x01)
	{
		if ((sysGetTicks(TIMER0) - tick) > 2)
			return Fail;
	}

	return Successful;
}

int usiTxLen(int count, int bitLen)
{
	UINT32 reg;

	reg = inpw(REG_USI_CNTRL);

	if ((count < 0) || (count > 3))
		return Fail;

	if ((bitLen <= 0) || (bitLen > 32))
		return Fail;

	if (bitLen == 32)
		reg = reg & 0xffffff07;
	else
		reg = (reg & 0xffffff07) | (bitLen << 3);
	reg = (reg & 0xfffffcff) | (count << 8);

	outpw(REG_USI_CNTRL, reg);

	return Successful;
}

int usiCheckBusy()
{
	// check status
	outpw(REG_USI_SSR, inpw(REG_USI_SSR) | 0x01);	// CS0

	// status command
	outpw(REG_USI_Tx0, 0x05);
	usiTxLen(0, 8);
	usiActive();
	// get status
	while(1)
	{
		outpw(REG_USI_Tx0, 0xff);
		usiTxLen(0, 8);
		usiActive();
		if (((inpw(REG_USI_Rx0) & 0xff) & 0x01) != 0x01)
			break;
	}
	outpw(REG_USI_SSR, inpw(REG_USI_SSR) & 0xfe);	// CS0

	return Successful;
}

/*
	addr: memory address
	len: byte count
	buf: buffer to put the read back data
*/
int usiRead(UINT32 addr, UINT32 len, UINT8 *buf)
{
	int volatile i;
	if(Enable4ByteFlag==1)	usiEnable4ByteAddressMode(); 
	outpw(REG_USI_SSR, inpw(REG_USI_SSR) | 0x01);	// CS0

	// read command
	outpw(REG_USI_Tx0, 03);
	usiTxLen(0, 8);
	usiActive();

	// address
	outpw(REG_USI_Tx0, addr);
	if(Enable4ByteFlag==1)
		usiTxLen(0, 32);
	else
	        usiTxLen(0, 24);
	usiActive();

	// data
	for (i=0; i<len; i++)
	{
		outpw(REG_USI_Tx0, 0xff);
		usiTxLen(0, 8);
		usiActive();
		*buf++ = inpw(REG_USI_Rx0) & 0xff;
	}

	outpw(REG_USI_SSR, inpw(REG_USI_SSR) & 0xfe);	// CS0

	if(Enable4ByteFlag==1)	usiDisable4ByteAddressMode();
	return Successful;
}

int usiReadFast(UINT32 addr, UINT32 len, UINT8 *buf)
{
	int volatile i;
	if(Enable4ByteFlag==1)	usiEnable4ByteAddressMode();
	outpw(REG_USI_SSR, inpw(REG_USI_SSR) | 0x01);	// CS0

	// read command
	outpw(REG_USI_Tx0, 0x0b);
	usiTxLen(0, 8);
	usiActive();

	// address
	outpw(REG_USI_Tx0, addr);
	if(Enable4ByteFlag==1)
		usiTxLen(0, 32);
	else
	        usiTxLen(0, 24);
	usiActive();

	// dummy byte
	outpw(REG_USI_Tx0, 0xff);
	usiTxLen(0, 8);
	usiActive();

	// data
	for (i=0; i<len; i++)
	{
		outpw(REG_USI_Tx0, 0xff);
		usiTxLen(0, 8);
		usiActive();
		*buf++ = inpw(REG_USI_Rx0) & 0xff;
	}

	outpw(REG_USI_SSR, inpw(REG_USI_SSR) & 0xfe);	// CS0
 	if(Enable4ByteFlag==1)	usiDisable4ByteAddressMode();
	return Successful;
}

int usiEnable4ByteAddressMode(void)
{
	outpw(REG_USI_SSR, inpw(REG_USI_SSR) | 0x01);	// CS0

	outpw(REG_USI_Tx0, 0xB7);
	usiTxLen(0, 8);
	usiActive();

	outpw(REG_USI_SSR, inpw(REG_USI_SSR) & 0xfe);	// CS0

	return Successful;
}

int usiDisable4ByteAddressMode(void)
{
	outpw(REG_USI_SSR, inpw(REG_USI_SSR) | 0x01);	// CS0

	outpw(REG_USI_Tx0, 0xE9);
	usiTxLen(0, 8);
	usiActive();

	outpw(REG_USI_SSR, inpw(REG_USI_SSR) & 0xfe);	// CS0

	return Successful;
}

int usiWriteEnable()
{
	outpw(REG_USI_SSR, inpw(REG_USI_SSR) | 0x01);	// CS0

	outpw(REG_USI_Tx0, 0x06);
	usiTxLen(0, 8);
	usiActive();

	outpw(REG_USI_SSR, inpw(REG_USI_SSR) & 0xfe);	// CS0

	return Successful;
}

int usiWriteDisable()
{
	outpw(REG_USI_SSR, inpw(REG_USI_SSR) | 0x01);	// CS0

	outpw(REG_USI_Tx0, 0x04);
	usiTxLen(0, 8);
	usiActive();

	outpw(REG_USI_SSR, inpw(REG_USI_SSR) & 0xfe);	// CS0

	return Successful;
}

/*
	addr: memory address
	len: byte count
	buf: buffer with write data
*/
int usiWrite(UINT32 addr, UINT32 len, UINT8 *buf)
{
	int status;
	if(Enable4ByteFlag==1)	usiEnable4ByteAddressMode(); 
	status=spiflash[_spi_type].SpiFlashWrite(addr, len, buf);
	if(Enable4ByteFlag==1)	usiDisable4ByteAddressMode();
	return status;
}

int usiEraseSector(UINT32 addr, UINT32 secCount)
{
	int volatile i;
	if(Enable4ByteFlag==1)	usiEnable4ByteAddressMode();
	if ((addr % (64*1024)) != 0)
		return -1;

	for (i=0; i<secCount; i++)
	{
		usiWriteEnable();

		outpw(REG_USI_SSR, inpw(REG_USI_SSR) | 0x01);	// CS0

		// erase command
		outpw(REG_USI_Tx0, 0xd8);
		usiTxLen(0, 8);
		usiActive();

		// address
		outpw(REG_USI_Tx0, addr+i*64*1024);
		if(Enable4ByteFlag==1)
			usiTxLen(0, 32);
		else
		        usiTxLen(0, 24);
		usiActive();

		outpw(REG_USI_SSR, inpw(REG_USI_SSR) & 0xfe);	// CS0

		// check status
		usiCheckBusy();
	}

	if(Enable4ByteFlag==1)	usiDisable4ByteAddressMode();
	return Successful;
}

extern void SendAck(UINT32 status);
int usiEraseAll()
{
	unsigned int count;

	usiWriteEnable();

	outpw(REG_USI_SSR, inpw(REG_USI_SSR) | 0x01);	// CS0

	outpw(REG_USI_Tx0, 0xc7);
	usiTxLen(0, 8);
	usiActive();

	outpw(REG_USI_SSR, inpw(REG_USI_SSR) & 0xfe);	// CS0

	// check status
	//usiCheckBusy();
	sysDelay(10);
	outpw(REG_USI_SSR, inpw(REG_USI_SSR) | 0x01);	// CS0

	outpw(REG_USI_Tx0, 0x05);
	usiTxLen(0, 8);
	usiActive();

	// get status
	count=0;
	while(1)
	{
		outpw(REG_USI_Tx0, 0xff);
		usiTxLen(0, 8);
		usiActive();
		if (((inpw(REG_USI_Rx0) & 0xff) & 0x01) != 0x01) break;
		(count)++;
		if (count > 95) count = 95;
		sysDelay(100);
		SendAck(count);
		sysprintf("%2d\r",count);
	}
	sysprintf("  \r");
	outpw(REG_USI_SSR, inpw(REG_USI_SSR) & 0xfe);	// CS0
	sysDelay(100);
	return Successful;
}


INT16 usiReadID()
{
	UINT16 volatile id;
	int volatile i;

	outpw(REG_USI_SSR, inpw(REG_USI_SSR) | 0x01);	// CS0

	// command 8 bit
	outpw(REG_USI_Tx0, 0x90);
	usiTxLen(0, 8);
	if (usiActive() < 0)
		return Fail;

	// address 24 bit
	outpw(REG_USI_Tx0, 0x000000);
	usiTxLen(0, 24);
	usiActive();

	// data 16 bit
	outpw(REG_USI_Tx0, 0xffff);
	usiTxLen(0, 16);
	usiActive();
	id = inpw(REG_USI_Rx0) & 0xffff;

	outpw(REG_USI_SSR, inpw(REG_USI_SSR) & 0xfe);	// CS0

	/* find spi flash */
	i=0;
#if 0	// mark [2011/10/03], if not SST SPI flash, it should be WB-like
	while(spiflash[i].PID)
	{
		if( spiflash[i].PID == ((id & 0xff00) >> 8) )
		{
			_spi_type=i;
			break;
		}
		i++;
	}
#else
	if (((id & 0xff00) >> 8) == 0xBF)
		_spi_type = 1;
	else
		_spi_type = 0;

   sysprintf("ID=0x%08x  _spi_type =%d\n",id, _spi_type);
   if( (id & 0xffff) == 0xc218) Enable4ByteFlag=1;
   if( (id & 0xffff) == 0x1C18) Enable4ByteFlag=1;
   if( (id & 0xffff) == 0xEF18) Enable4ByteFlag=1; //Winbond 25q256fvfg
   //if( (id & 0xffff) == 0xC818) Enable4ByteFlag=1; //GD 32MB
   if(Enable4ByteFlag ==1)
   {
       sysprintf("Enable4ByteFlag  ID=0x%08x   _spi_type =%d\n",id, _spi_type);
   }
#endif
	return id;
}

UINT8 usiStatusRead()
{
	UINT32 status;

	outpw(REG_USI_SSR, inpw(REG_USI_SSR) | 0x01);	// CS0

	// status command
	outpw(REG_USI_Tx0, 0x05);
	usiTxLen(0, 8);
	usiActive();

	// get status
	outpw(REG_USI_Tx0, 0xff);
	usiTxLen(0, 8);
	usiActive();
	status = inpw(REG_USI_Rx0) & 0xff;

	outpw(REG_USI_SSR, inpw(REG_USI_SSR) & 0xfe);	// CS0

	return status;
}

int usiStatusWrite(UINT8 data)
{
	usiWriteEnable();

	outpw(REG_USI_SSR, inpw(REG_USI_SSR) | 0x01);	// CS0

	// status command
	outpw(REG_USI_Tx0, 0x01);
	usiTxLen(0, 8);
	usiActive();

	// write status
	outpw(REG_USI_Tx0, data);
	usiTxLen(0, 8);
	usiActive();

	outpw(REG_USI_SSR, inpw(REG_USI_SSR) & 0xfe);	// CS0

	return Successful;
}


BOOL volatile _usbd_bIsSPIInit = FALSE;
int usiInit()
{
	int volatile tick;
	if (_usbd_bIsSPIInit == FALSE)
	{
		#if 0
		outpw(REG_CLKEN, (inpw(REG_CLKEN) | 0x20000000)); /* enable SPI clock */
		outpw(REG_MFSEL, ((inpw(REG_MFSEL) & 0xFFFC3FFF) | 0x00028000)); /* select USI function pins */
		#endif

		outpw(REG_MFP_GPB_L, inpw(REG_MFP_GPB_L) | 0xBB000000);
		outpw(REG_MFP_GPB_H, inpw(REG_MFP_GPB_H) | 0x000000BB);
		//SPI0: B10, B11 = D[2], D[3]
		outpw(REG_MFP_GPB_H, inpw(REG_MFP_GPB_H) | 0x0000BB00);

		//SPI0 SS1: B0, H12
		outpw(REG_MFP_GPB_L, inpw(REG_MFP_GPB_L) | 0x0000000B);
		outpw(REG_MFP_GPH_H, inpw(REG_MFP_GPH_H) | 0x000B0000);

		outpw(REG_USI_DIVIDER, 1);
		outpw(REG_USI_SSR, 0x00);		// CS active low
		outpw(REG_USI_CNTRL, 0x04);		// Tx: falling edge, Rx: rising edge

		if (usiReadID() == Fail)
			return Fail;

		usiStatusWrite(0x00);	// clear block protect

		//tick = sysGetTicks(0);
		//while((sysGetTicks(0) - tick) < 10);
		sysDelay(50);

		_usbd_bIsSPIInit = TRUE;
	}
	return 0;

} /* end usiInit */

int DelSpiSector(UINT32 start, UINT32 len)
{
  int i;
  for(i=0;i<len;i++)
  {
    usiEraseSector((start+i)*64*1024, 1);
    SendAck(((i+1)*100)/len);
  }
  return Successful;
}

int DelSpiImage(UINT32 imageNo)
{
	int i, count;
	unsigned int startOffset=0, length=0;
	unsigned char *pbuf;
	unsigned int *ptr;
	UCHAR _fmi_ucBuffer[64*1024];

	pbuf = (UINT8 *)((UINT32)_fmi_ucBuffer | 0x80000000);
	ptr = (unsigned int *)((UINT32)_fmi_ucBuffer | 0x80000000);
	SendAck(10);
	usiRead( (SPI_HEAD_ADDR-1)*64*1024, 64*1024, (UINT8 *)pbuf);
	ptr = (unsigned int *)(pbuf + 63*1024);
	SendAck(30);
	if (((*(ptr+0)) == 0xAA554257) && ((*(ptr+3)) == 0x63594257))
	{
		count = *(ptr+1);
		*(ptr+1) = count - 1;	// del one image

		/* pointer to image information */
		ptr += 4;
		for (i=0; i<count; i++)
		{
			if ((*(ptr) & 0xffff) == imageNo)
			{
				startOffset = *(ptr + 1);
				length = *(ptr + 3);
				break;
			}
			/* pointer to next image */
			ptr = ptr+8;
		}
		memcpy((char *)ptr, (char *)(ptr+8), (count-i-1)*32);
	}
	SendAck(40);
	usiEraseSector((SPI_HEAD_ADDR-1)*64*1024, 1);	/* erase sector 0 */

	/* send status */
	SendAck(50);

	usiWrite((SPI_HEAD_ADDR-1)*64*1024, 64*1024, pbuf);
	SendAck(80);
	// erase the sector
	{
		int tmpCnt=0;
	if (startOffset != 0)
	{
		tmpCnt = length / (64 * 1024);
		if ((length % (64 * 1024)) != 0)
			tmpCnt++;

		usiEraseSector(startOffset, tmpCnt);
	}
	}
	SendAck(100);
	return Successful;
}

int ChangeSpiImageType(UINT32 imageNo, UINT32 imageType)
{
	int i, count;
	unsigned char *pbuf;
	unsigned int *ptr;
	UCHAR _fmi_ucBuffer[64*1024];

	pbuf = (UINT8 *)((UINT32)_fmi_ucBuffer | 0x80000000);
	ptr = (unsigned int *)((UINT32)_fmi_ucBuffer | 0x80000000);

	usiRead( (SPI_HEAD_ADDR-1)*64*1024, 64*1024, (UINT8 *)pbuf);
	ptr = (unsigned int *)(pbuf + 63*1024);

	if (((*(ptr+0)) == 0xAA554257) && ((*(ptr+3)) == 0x63594257))
	{
		count = *(ptr+1);

		/* pointer to image information */
		ptr += 4;
		for (i=0; i<count; i++)
		{
			if ((*ptr & 0xffff) == imageNo)
			{
				*ptr = ((imageType & 0xffff) << 16) | (imageNo & 0xffff);
				break;
			}
			/* pointer to next image */
			ptr = ptr+8;
		}
	}

	usiEraseSector((SPI_HEAD_ADDR-1)*64*1024, 1);	/* erase sector 0 */

	usiWrite((SPI_HEAD_ADDR-1)*64*1024, 64*1024, pbuf);

	return Successful;
}


/******************************************/
/* write function for different spi flash */
/******************************************/
int wbSpiWrite(UINT32 addr, UINT32 len, UINT8 *buf)
{
	int volatile count=0, page, i;
	count = len / 256;
	if ((len % 256) != 0)
		count++;

	for (i=0; i<count; i++)
	{
		// check data len
		if (len >= 256)
		{
			page = 256;
			len = len - 256;
		}
		else
			page = len;
		usiWriteEnable();
		outpw(REG_USI_SSR, inpw(REG_USI_SSR) | 0x01);	// CS0

		// write command
		outpw(REG_USI_Tx0, 0x02);
		usiTxLen(0, 8);
		usiActive();

		// address
		outpw(REG_USI_Tx0, addr+i*256);
		if(Enable4ByteFlag==1)
			usiTxLen(0, 32);
		else
			usiTxLen(0, 24);
		usiActive();

		// write data
		while (page-- > 0)
		{
			outpw(REG_USI_Tx0, *buf++);
			usiTxLen(0, 8);
			usiActive();
		}

		outpw(REG_USI_SSR, inpw(REG_USI_SSR) & 0xfe);	// CS0

		// check status
		usiCheckBusy();
	}

	return Successful;
}

int sstSpiWrite(UINT32 addr, UINT32 len, UINT8 *buf)
{
	while (len > 0)
	{
		usiWriteEnable();

		outpw(REG_USI_SSR, inpw(REG_USI_SSR) | 0x01);// CS0

		// write command
		outpw(REG_USI_Tx0, 0x02);
		usiTxLen(0, 8);
		usiActive();

		// address
		outpw(REG_USI_Tx0, addr++);
		usiTxLen(0, 24);
		usiActive();

		// write data
		outpw(REG_USI_Tx0, *buf++);
		usiTxLen(0, 8);
		usiActive();

		outpw(REG_USI_SSR, inpw(REG_USI_SSR) & 0xfe);// CS0

		// check status
		usiCheckBusy();

		len--;
	}

	return Successful;
}



