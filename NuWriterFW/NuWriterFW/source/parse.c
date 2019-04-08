/******************************************************************************
 *
 * Copyright (c) 2013 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: parse.c $
 *
 * $Author: schung $
 ******************************************************************************/
#include "config.h"
#include "usbd.h"
#include "fmi.h"
#include "sdglue.h"
#include "filesystem.h"
#include "stdio.h"
#include "stdlib.h"

#define BATCH_BRUN

#define TRANSFER_LEN 4096

#define SPI_BLOCK_SIZE (64*1024)

UCHAR NandBlockSize=0;
INT32 eMMCBlockSize=0;

unsigned char *pImageList;
unsigned char imageList[512];

FW_NOR_IMAGE_T spiImage;
FW_NOR_IMAGE_T *pSpiImage;

//-------------------------
extern int usiInit(void);
extern int usiEraseAll(void);
extern int usiWrite(UINT32 addr, UINT32 len, UINT16 *buf);
extern int usiRead(UINT32 addr, UINT32 len, UINT8 *buf);
extern int usiEraseSector(UINT32 addr, UINT32 secCount);

extern int ChangeSpiImageType(UINT32 imageNo, UINT32 imageType);
extern int DelSpiImage(UINT32 imageNo);
extern int DelSpiSector(UINT32 start, UINT32 len);

extern INT16 usiReadID(void);
extern UINT32 Custom_uBlockPerFlash;
extern UINT32 Custom_uPagePerBlock;
UINT32 g_uIsUserConfig;
extern volatile unsigned char Enable4ByteFlag;
//-------------------------
#define PACK_Mode(val)      (((val)&0xF   )>> 0)
#define PACK_Option(val)    (((val)&0xF0  )>> 4)
#define PACK_Encrypt(val)   (((val)&0xF00 )>> 8)
#define PACK_Enable(val)    (((val)&0xF000)>>12)

#define WRITER_MODE   0
#define MODIFY_MODE   1
#define ERASE_MODE    2
#define VERIFY_MODE   3
#define READ_MODE     4
#define PACK_MODE     5
#define FORMAT_MODE   6
#define PACK_VERIFY_MODE   7

#define DATA      0
#define ENV       1
#define UBOOT     2
#define PACK      3
#define IMAGE     4
#define DATA_OOB  5
#define YAFFS2  41
#define UBIFS   42
#define PMTP    15

void usleep(int count)
{
    int volatile i=0;

    for(i=0; i<count; i++);
}

UINT32 fmiGetSPIImageInfo(unsigned int *image)
{
    UINT32 volatile bmark, emark;
    int volatile i, imageCount=0;
    unsigned char volatile *pbuf;
    unsigned int volatile *ptr;
    UCHAR _fmi_ucBuffer[1024];
    pbuf = (UINT8 *)((UINT32)_fmi_ucBuffer | 0x80000000);
    ptr = (unsigned int *)((UINT32)_fmi_ucBuffer | 0x80000000);
    usiRead(((SPI_HEAD_ADDR-1)*64+63)*1024, 1024, (UINT8 *)pbuf);  /* offset, len, address */
    bmark = *(ptr+0);
    emark = *(ptr+3);

    if ((bmark == 0xAA554257) && (emark == 0x63594257)) {
        imageCount = *(ptr+1);

        /* pointer to image information */
        ptr = ptr+4;
        for (i=0; i<imageCount; i++) {
            /* fill into the image list buffer */
            *image = 0;     // action flag, dummy
            *(image+1) = 0; // file len, dummy
            *(image+2) = *(ptr) & 0xffff;
            memcpy((char *)(image+3), (char *)(ptr+4), 16);
            *(image+7) = (*(ptr) >> 16) & 0xffff;
            *(image+8) = (*(ptr+1)) + (*(ptr+3));
            *(image+9) = *(ptr+1);
            *(image+10) = *(ptr+3);

//MSG_DEBUG("\nNo[%d], Flag[%d], name[%s]\n\n",
//  *(image+2), *(image+7), (char *)(image+3));
            /* pointer to next image */
            image += 11;
            ptr = ptr+8;
        }
    } else
        imageCount = 0;

    return imageCount;
}

void addMagicHeader(unsigned int address, unsigned int length)
{
    int volatile i;
    unsigned int *ptr;
    unsigned char *magic;
    unsigned char MagicWord[4] = {0x20, 'T', 'V', 'N'}; /* 1st word - Magic word */
    ptr =((unsigned int*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
    magic =((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));

    for (i=0; i<sizeof(MagicWord); i++)
        *magic++ = MagicWord[i];

    *(ptr+1) = address;          /* 2nd word - Target Address */
    *(ptr+2) = length;           /* 3rd word - Total length     */
    *(ptr+3) = 0xFFFFFFFF;
}

void GetSPIImage()
{
    int count;

    MSG_DEBUG("Get SPI flash Image ...\n");
    pImageList=((unsigned char*)(((unsigned int)imageList)|NON_CACHE));
    memset(pImageList, 0, sizeof(imageList));

    if (usiInit() < 0)
        count = 0;
    else {
        /* send image info to host */
        *(unsigned int *)pImageList = fmiGetSPIImageInfo((unsigned int *)(pImageList+4));

        count = *(unsigned int *)pImageList;
        if (count < 0)
            count = 0;
    }
    usleep(1000);
    usb_send(pImageList, count*(sizeof(FW_NOR_IMAGE_T))+4);

    MSG_DEBUG("finish get spi image [%d]!!\n", count);
}

/******************************************************************************
 *
 *  MTP Functions
 *
 ******************************************************************************/

/********************* Bit definition of OTP_KEYEN  ************************/
#define OTP_KEYEN_EN          ((UINT32)0x00000001)      /*!<AES DMA finish interrupt flag enable */

/********************* Bit definition of OTP_MODE  *************************/
#define OTP_MODE_MASK         ((UINT32)0x00000003)      /*!<OTP mode control bits mask */
#define OTP_MODE_IDLE         ((UINT32)0x00000000)      /*!<OTP idle mode */
#define OTP_MODE_PROGRAM      ((UINT32)0x00000002)      /*!<OTP program key mode */
#define OTP_MODE_LOCK         ((UINT32)0x00000003)      /*!<OTP lock key mode */

/********************* Bit definition of OTP_START  *************************/
#define OTP_START_START       ((UINT32)0x00000001)      /*!<OTP start  */

/********************* Bit definition of OTP_STATUS  ************************/
#define OTP_STATUS_ENABLED    ((UINT32)0x00000001)      /*!<OTP is enabled   */
#define OTP_STATUS_KEY_VALID  ((UINT32)0x00000002)      /*!<last key in OTP is valid */
#define OTP_STATUS_NO_PROG    ((UINT32)0x00000004)      /*!<No key programmed */
#define OTP_STATUS_LOCKED     ((UINT32)0x00000008)      /*!<OTP is locked  */
#define OTP_STATUS_PROG_FAIL  ((UINT32)0x00000010)      /*!<OTP program fail   */
#define OTP_STATUS_BUSY       ((UINT32)0x00000010)      /*!<OTP program fail   */

/********************* Bit definition of OTP_PROTECT  ***********************/
#define OTP_PROTECT_UNLOCKED    ((UINT32)0x00000001)      /*!<OTP unlocked   */

/****************************************************************************/
#define POOLING_LOOP        0x8000000
#define MTP_KEY_SUCCESS     0x80
#define MTP_NO_KEY          0x81
#define MTP_LOCK_FAILED     0x82
#define MTP_LOCK_KEY        0x83
#define MTP_LOCK_SUCCESS    0x84
#define MTP_ENABLE_SUCCESS  0x85

/****************************************************************************/
void MTP_Check_Key(UINT32  key[8])
{
    int  i;

    for (i = 0; i < 8; i++) {
        MSG_DEBUG("Key %d, 0x%x 0x%x\n", i, key[i], inpw(0xB800C000+0x60+i*4));
    }
}
int MTP_Enable()
{
    int  loop;
    outpw(REG_OTP_PROTECT, 0x59);
    outpw(REG_OTP_PROTECT, 0x16);
    outpw(REG_OTP_PROTECT, 0x88);
    if (inpw(REG_OTP_PROTECT) == 0x1) {
        outpw(REG_OTP_KEYEN, inpw(REG_OTP_KEYEN) | 0x1);
        /* check if secure boot */
        for (loop = 0; loop < 0x100000; loop++) {
            if (inpw(REG_OTP_STATUS) & 0x1) {
                if (((inpw(REG_OTP_STATUS) >> 16) & 0x1f) == 0) {
                    if (inpw(REG_OTP_STATUS) & 0x04) {
                        MSG_DEBUG("No Key\n");
                        for (loop = 0; loop < 0x100; loop++);
                        return MTP_ENABLE_SUCCESS;
                    }
                } else {
                    if (inpw(REG_OTP_STATUS) & 0x02) {
                        MSG_DEBUG("Key OK\n");
                        for (loop = 0; loop < 0x100; loop++);
                        return MTP_ENABLE_SUCCESS;
                    }
                }
            }
        }
    }
    return -1;
}
#define MTP_DELAY 0x100000
int MTP_Program_Key(UINT8 *wbuf,UINT32 len,UINT8 option,UINT8 mode)
{
    UINT8 BootSrc=0;
    UINT32  otp_key[8];
    UINT32 * g_sha_digest =(UINT32 *)wbuf;
    int  loop,i,j;
    UINT8 data[4];
    outpw(REG_PCLKEN1, inpw(REG_PCLKEN1) | (1 << 26));

    MTP_Enable();
    outpw(REG_OTP_MODE, (inpw(REG_OTP_MODE) & 0x3) | 0x2);
    outpw(REG_OTP_CYCLE, 0x60AE * 8);

    /* 8b0c0f24 fa2fc0ef 7501698e 7c2ba644 81bebd41 6d446548 32d3aa5b 733518b9 */
#if 0
    for(i=0; i<8; i++) {
        for(j=0; j<4; j++)
            data[j]=wbuf[i*4+(3-j)];
        MSG_DEBUG("0x%08x=0x%08x\n",REG_OTP_VALUE0+i*4,*((UINT32 *)(data)) );
        g_sha_digest[i]=*((UINT32 *)data);
    }
#endif

#if 0
    for(i=0; i<8; i++) {
        MSG_DEBUG("0x%08x=0x%08x\n",REG_OTP_VALUE0+i*4,g_sha_digest[i] );
    }
#endif

    switch(option & 0x3) {
    case 1:  /*AES*/
        MSG_DEBUG("[AES] MODE\n");
        BootSrc     = mode;
        for(i=0; i<8; i++) {
            for(j=0; j<4; j++)
                data[j]=wbuf[i*4+(3-j)];
            g_sha_digest[i]=*((UINT32 *)data);
        }
        break;
    case 2:  /*SHA*/
        MSG_DEBUG("[SHA] MODE\n");
        BootSrc =0x0;
        break;
    default:
        break;
    }

    switch(BootSrc) {
    case 0x0:
        MSG_DEBUG("Boot from UNKNOW (0x0)\n");
        otp_key[0] = g_sha_digest[0];
        otp_key[1] = g_sha_digest[1];
        otp_key[2] = g_sha_digest[2];
        otp_key[3] = g_sha_digest[3];
        otp_key[4] = g_sha_digest[4];
        otp_key[5] = g_sha_digest[5];
        otp_key[6] = g_sha_digest[6];
        otp_key[7] = g_sha_digest[7];
        break;
    case 0x1 :
        MSG_DEBUG("Boot from eMMC flash\n");
        otp_key[0] = g_sha_digest[6];
        otp_key[1] = g_sha_digest[7];
        otp_key[2] = g_sha_digest[0];
        otp_key[3] = g_sha_digest[1];
        otp_key[4] = g_sha_digest[2];
        otp_key[5] = g_sha_digest[3];
        otp_key[6] = g_sha_digest[4];
        otp_key[7] = g_sha_digest[5];
        break;
    case 0x2:
        MSG_DEBUG("Boot from NAND flash\n");
        otp_key[0] = g_sha_digest[4];
        otp_key[1] = g_sha_digest[5];
        otp_key[2] = g_sha_digest[6];
        otp_key[3] = g_sha_digest[7];
        otp_key[4] = g_sha_digest[0];
        otp_key[5] = g_sha_digest[1];
        otp_key[6] = g_sha_digest[2];
        otp_key[7] = g_sha_digest[3];
        break;
    case 0x3 :
        MSG_DEBUG("Boot from SPI flash\n");
        otp_key[0] = g_sha_digest[2];
        otp_key[1] = g_sha_digest[3];
        otp_key[2] = g_sha_digest[4];
        otp_key[3] = g_sha_digest[5];
        otp_key[4] = g_sha_digest[6];
        otp_key[5] = g_sha_digest[7];
        otp_key[6] = g_sha_digest[0];
        otp_key[7] = g_sha_digest[1];
        break;
    default:
        MSG_DEBUG("Incorrect boot mode setting!! BootSrc=%d\n",mode);
        while (1);
    }

    for(i=0; i<8; i++) {
        outpw(REG_OTP_VALUE0+i*4, otp_key[i]);
        MSG_DEBUG("0x%08x=0x%08x\n",REG_OTP_VALUE0+i*4,otp_key[i] );
    }

    MSG_DEBUG("option=0x%08x\n",option);
    outpw(REG_OTP_OPTION, option);
    MSG_DEBUG("REG_OTP_OPTION=0x%08x\n",inpw(REG_OTP_OPTION));
    MSG_DEBUG("REG_OTP_STATUS=0x%08x\n",inpw(REG_OTP_STATUS));

    outpw(REG_OTP_START, 0x1);

    for (loop = 0; loop < 0x1000000; loop++) {
        if (inpw(REG_OTP_START) == 0)
            break;
    }
    if (inpw(REG_OTP_STATUS) & 0x10) {
        MSG_DEBUG("Program error\n");
    }
    return 0;
}

int MTP_IsLock()
{
    outpw(REG_PCLKEN1, inpw(REG_PCLKEN1) | (1 << 26));
    if (MTP_Enable() < 0)
        return -1;
    if (inpw(REG_OTP_STATUS) & OTP_STATUS_LOCKED) {
        MSG_DEBUG("No key in MTP.\n");
        return 1;
    }
    return 0;
}
int MTP_Lock()
{
    int  loop;

    if (MTP_Enable() < 0)
        return -1;

    if (inpw(REG_OTP_STATUS) & OTP_STATUS_NO_PROG) {
        sysprintf("No key in MTP.\n");
        return MTP_NO_KEY;
    }

    if (inpw(REG_OTP_STATUS) & OTP_STATUS_LOCKED) {
        sysprintf("It is Locked in MTP.\n");
        return MTP_LOCK_KEY;
    }

    outpw(REG_OTP_MODE,(inpw(REG_OTP_MODE) & OTP_MODE_MASK ) | OTP_MODE_LOCK );
    outpw(REG_OTP_CYCLE,0x60AE * 8);
    outpw(REG_OTP_START,0x1);

    for (loop = 0; loop < POOLING_LOOP; loop++) {
        if (inpw(REG_OTP_START) == 0)
            break;
    }
    if (loop >= POOLING_LOOP) {
        MSG_DEBUG("Failed to start OTP!\n");
        return MTP_LOCK_FAILED;
    }

    MTP_Enable();

    if(( inpw(REG_OTP_STATUS) & (OTP_STATUS_ENABLED | OTP_STATUS_KEY_VALID | OTP_STATUS_LOCKED)) !=
            (OTP_STATUS_ENABLED | OTP_STATUS_KEY_VALID | OTP_STATUS_LOCKED)) {
        MSG_DEBUG("MTP lock failed, OPT_STATUS = 0x%x!\n", inpw(REG_OTP_STATUS));
        return MTP_LOCK_FAILED;
    }

    if(MTP_Enable()> 0)
        return MTP_LOCK_SUCCESS;
    else
        return MTP_LOCK_FAILED;
}
/******************************************************************************
 *  MTP Functions End
 ******************************************************************************/

int Burn_SPI(UINT32 len,UINT32 imageoffset)
{
    int volatile tmplen=0;
    int i, offset=0, blockCount, spiSourceAddr;
    //unsigned char infoBuf[1024];
    //unsigned char *pInfo;
    //pInfo = (UINT8 *)((UINT32)infoBuf | NON_CACHE);

    /* set up interface */
    if (usiInit() < 0) {
        SendAck(0xFFFFFFFF);
        return Fail;
    }

    spiSourceAddr = (DOWNLOAD_BASE | NON_CACHE);
    blockCount = len / SPI_BLOCK_SIZE;
    offset = imageoffset;
    for (i=0; i<blockCount; i++) {
	    MSG_DEBUG("Burn_SPI  offset=0x%x(%d)\n", offset, offset);
        Enable4ByteFlag = 0;
        // 4Byte Address Mode (>16MByte)
        if((offset + SPI_BLOCK_SIZE) > (16*1024*1024))
            Enable4ByteFlag = 1;
        if(len > (16*1024*1024))
            Enable4ByteFlag = 1;

        if(Enable4ByteFlag)
            MSG_DEBUG("Enable4ByteFlag %d:  offset=0x%08x(%d)\n", Enable4ByteFlag, offset, offset);

        usiEraseSector(offset, 1);
        usiWrite(offset, SPI_BLOCK_SIZE, (UINT16 *)spiSourceAddr);

        spiSourceAddr += SPI_BLOCK_SIZE;
        offset += SPI_BLOCK_SIZE;

        //ack status
        tmplen += SPI_BLOCK_SIZE;
        SendAck((tmplen * 95) / len);
    }
    if ((len % (SPI_BLOCK_SIZE)) != 0) {
	    MSG_DEBUG("446 Burn_SPI  offset=0x%x(%d)\n", offset, offset);
        // 4Byte Address Mode (>16MByte)
		Enable4ByteFlag = 0;
        if((offset + SPI_BLOCK_SIZE) > (16*1024*1024))
            Enable4ByteFlag = 1;
        if(offset > (16*1024*1024))
            Enable4ByteFlag = 1;

        if(Enable4ByteFlag)
            MSG_DEBUG("Enable4ByteFlag %d:  offset=0x%08x(%d)\n", Enable4ByteFlag, offset, offset);		
        len = len - blockCount*SPI_BLOCK_SIZE;
        usiEraseSector(offset, 1);
        usiWrite(offset, len, (UINT16 *)spiSourceAddr);
    }
    //ack status
    SendAck(100);

    return 0;
}


int Burn_FastSPI(UINT32 len,UINT32 imageoffset)
{
    int volatile tmplen=0;
    int i, offset=0, blockCount, spiSourceAddr;
    //unsigned char infoBuf[1024];
    //unsigned char *pInfo;
    //pInfo = (UINT8 *)((UINT32)infoBuf | NON_CACHE);
    MSG_DEBUG("Burn_FastSPI: len=%d     imageoffset= 0x%08x(%d)\n", len, imageoffset, imageoffset);
    /* set up interface */
    if (usiInit() < 0) {
        SendAck(0xFFFFFFFF);
        return Fail;
    }

    spiSourceAddr = (DOWNLOAD_BASE | NON_CACHE);
    blockCount = len / SPI_BLOCK_SIZE;
    offset = imageoffset;
    for (i=0; i<blockCount; i++) {
        //usiEraseSector(offset, 1);
        //sysprintf("Burn_FastSPI  offset=0x%x(%d)\n", offset, offset);
        Enable4ByteFlag = 0;
        // 4Byte Address Mode (>16MByte)
        if((offset + SPI_BLOCK_SIZE) > (16*1024*1024))
        {
            Enable4ByteFlag = 1;
            //_spi_type = 0;
        }
        if(len > (16*1024*1024))
        {
            Enable4ByteFlag = 1;
            //_spi_type = 0;
        }

        if(Enable4ByteFlag)
            MSG_DEBUG("Burn_FastSPI Enable4ByteFlag %d  blockCount:%d offset=0x%08x(%d)\n", Enable4ByteFlag, i, offset, offset);

        usiWrite(offset, SPI_BLOCK_SIZE, (UINT16 *)spiSourceAddr);

        spiSourceAddr += SPI_BLOCK_SIZE;
        offset += SPI_BLOCK_SIZE;

        //ack status
        tmplen += SPI_BLOCK_SIZE;
        SendAck((tmplen * 95) / len);
    }
    if ((len % (SPI_BLOCK_SIZE)) != 0) {

        // 4Byte Address Mode (>16MByte)
		Enable4ByteFlag = 0;
        if((offset + SPI_BLOCK_SIZE) > (16*1024*1024))
        {
            Enable4ByteFlag = 1;
            //_spi_type = 0;
        }
        if(offset > (16*1024*1024))
        {
            Enable4ByteFlag = 1;
            //_spi_type = 0;
        }

        if(Enable4ByteFlag)
            MSG_DEBUG("Burn_FastSPI Enable4ByteFlag %d: len=0x%x  offset=0x%08x(%d)\n", Enable4ByteFlag, len, offset, offset);

        len = len - blockCount*SPI_BLOCK_SIZE;
        //usiEraseSector(offset, 1);
        usiWrite(offset, len, (UINT16 *)spiSourceAddr);
    }
    //ack status
    SendAck(100);

    return 0;
}

void UXmodem_SPI(void)
{
    int len,i;
    unsigned char *ptr;
    unsigned char buf[80];
    unsigned char *_ch;
    unsigned int *_ack;
    unsigned int offset=0;
    unsigned int remain, ddrlen;
    PACK_HEAD pack;
    PACK_CHILD_HEAD ppack;
    unsigned int ret;

    MSG_DEBUG("download image to SPI flash...\n");
    memset((char *)&spiImage, 0, sizeof(FW_NOR_IMAGE_T));
    pSpiImage = (FW_NOR_IMAGE_T *)&spiImage;
    usiInit();  //Init SPI
    _ch=((unsigned char*)(((unsigned int)buf)|NON_CACHE));
    _ack=((unsigned int*)(((unsigned int)buf)|NON_CACHE));
    ptr=_ch;

    while(1) {
        if(Bulk_Out_Transfer_Size>0) {
            usb_recv(ptr,sizeof(FW_NOR_IMAGE_T));
            memcpy(pSpiImage,(unsigned char *)ptr,sizeof(FW_NOR_IMAGE_T));
            break;
        }
    }

    MSG_DEBUG("action flag %d, image %d, len=%d exec=0x%08x type=%d\n",
              pSpiImage->actionFlag, pSpiImage->imageNo,pSpiImage->fileLength,pSpiImage->executeAddr,pSpiImage->imageType);
    usiInit();
    switch(pSpiImage->actionFlag) {
    case WRITER_MODE: { // normal write
        MSG_DEBUG("SPI normal write !!!\n");

        /* for debug or delay */
        {
            usleep(1000);
            *_ack=0x85;
            usb_send((unsigned char*)_ack,4);//send ack to PC
        }

        _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
        ptr=_ch;

        if (pSpiImage->imageType == UBOOT) {    // uboot image
            pSpiImage->imageNo = 0;
            pSpiImage->flashOffset = 0;
            addMagicHeader(pSpiImage->executeAddr, pSpiImage->fileLength);
            ptr += 16;  // except the 16 bytes magic header
            offset=16;
        } else
            offset=0;

        do {
            if(Bulk_Out_Transfer_Size>0) {

                len=Bulk_Out_Transfer_Size;
                usb_recv(ptr,len);  //recv data from PC
                ptr+=len;
                *_ack=len;
                usb_send((unsigned char*)_ack,4);//send ack to PC
            }
        } while((ptr-_ch)<(pSpiImage->fileLength+offset+((FW_NOR_IMAGE_T *)pSpiImage)->initSize));

        Burn_SPI(pSpiImage->fileLength+(((FW_NOR_IMAGE_T *)pSpiImage)->initSize)+offset,pSpiImage->flashOffset);
    }
    break;

    case MODIFY_MODE: { // modify
        MSG_DEBUG("SPI modify !!!\n");
        {
            int state;
            state = ChangeSpiImageType(pSpiImage->imageNo, pSpiImage->imageType);
            if (state < 0) {
                //MSG_DEBUG("error!!\n");
                SendAck(0xFFFFFFFF);
            } else {
                //MSG_DEBUG("OK!!\n");
                SendAck(100);
            }
        }
    }
    break;

    case ERASE_MODE: { // erase
        /* for debug or delay */
        {
            usleep(1000);
            *_ack=0x85;
            usb_send((unsigned char*)_ack,4);//send ack to PC
        }

        MSG_DEBUG("SPI erase !!! imageNo=%d\n",pSpiImage->imageNo);
        MSG_DEBUG("\nstart=%d\n",pSpiImage->flashOffset);       //start block
        MSG_DEBUG("length=%d\n",pSpiImage->executeAddr);  //block length
        MSG_DEBUG("type=%d\n",pSpiImage->imageType);      //0: chip erase, 1: erase accord start and length blocks.
        MSG_DEBUG("type=0x%08x\n",pSpiImage->imageNo);

        if (pSpiImage->imageNo != 0xFFFFFFFF)
            DelSpiSector(pSpiImage->flashOffset,pSpiImage->executeAddr);
        else {
            // erase all
            ret = usiEraseAll();
            if(ret == 0) {
                MSG_DEBUG("erase all done \n");
                SendAck(100);
            } else {
                MSG_DEBUG("XXXXX erase all fail \n");
                SendAck(0xffffff);
            }
        }
    }
    break;

    case VERIFY_MODE: { // verify
        MSG_DEBUG("SPI normal verify !!!\n");

        /* for debug or delay */
        {
            usleep(1000);
            *_ack=0x85;
            usb_send((unsigned char*)_ack,4);//send ack to PC
        }

        _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
        ptr=_ch;
        MSG_DEBUG("offset=0x%08x(%d),flashOffset=0x%08x(%d),fileLength=%d\n",offset, offset, pSpiImage->flashOffset, pSpiImage->flashOffset, pSpiImage->fileLength);
        memset(ptr, 0, pSpiImage->fileLength);
        offset=0;
        do {
            //4Byte Address Mode (>16MByte)
			Enable4ByteFlag = 0;
            // if(pSpiImage->fileLength >= (16*1024*1024))
                // Enable4ByteFlag = 1;
            if((pSpiImage->flashOffset + offset + TRANSFER_LEN) > (16*1024*1024))
                Enable4ByteFlag = 1;

            if(Enable4ByteFlag)
                MSG_DEBUG("VERIFY_MODE  Enable4ByteFlag %d:  offset=0x%08x(%d)\n", Enable4ByteFlag, offset, offset);

            if (pSpiImage->flashOffset == 0)
                usiRead(pSpiImage->flashOffset+16+offset+((FW_NOR_IMAGE_T *)pSpiImage)->initSize, TRANSFER_LEN, (UINT8 *)(DOWNLOAD_BASE));
            else
                usiRead(pSpiImage->flashOffset+offset, TRANSFER_LEN, (UINT8 *)(DOWNLOAD_BASE));
            usb_send(ptr,TRANSFER_LEN); //send data to PC

            usb_recv((unsigned char*)_ack,4);   //recv data from PC
            //ptr += (*_ack);
            if(*_ack==0)
                break;
            else
                offset+=(*_ack);
        } while(offset<pSpiImage->fileLength);
    }
    break;
    case PACK_VERIFY_MODE: { // verify
        MSG_DEBUG("SPI PACK verify !!!\n");

        /* for debug or delay */
        {
            usleep(1000);
            *_ack=0x85;
            usb_send((unsigned char*)_ack,4);//send ack to PC
        }

        _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
        ptr=_ch;
        MSG_DEBUG("offset=%d,flashOffset=%d,fileLength=%d\n",offset,pSpiImage->flashOffset,pSpiImage->fileLength);
        memset(ptr, 0, pSpiImage->fileLength);
        offset=0;


        for(i=0; i<pSpiImage->imageNo; i++) {
            while(1) {
                if(Bulk_Out_Transfer_Size>=sizeof(PACK_CHILD_HEAD)) {
                    usb_recv(ptr,sizeof(PACK_CHILD_HEAD));
                    memcpy(&ppack,(unsigned char *)ptr,sizeof(PACK_CHILD_HEAD));
                    usleep(1000);
                    *_ack=0x85;
                    usb_send((unsigned char*)_ack,4);//send ack to PC
                    break;
                }
            }
            pSpiImage->fileLength = ppack.filelen;
            pSpiImage->flashOffset = ppack.startaddr;
            offset = 0;
            MSG_DEBUG("imageNum = %d, offset=0x%x, flashOffset=0x%x, fileLength=0x%x\n",i, offset,pSpiImage->flashOffset,pSpiImage->fileLength);
            // Get DDR parameter length
            if (ppack.imagetype == UBOOT) {   // system image
                usb_recv(ptr,4);
                memcpy(&ddrlen,(unsigned char *)ptr,4);
                MSG_DEBUG("ddrlen = %d\n", ddrlen);
                usleep(1000);
                *_ack=0x85;
                usb_send((unsigned char*)_ack,4);//send ack to PC
            }

            MSG_DEBUG("=======> imageNo=%d, pSpiImage->flashOffset =0x%x [%d]\n",i, pSpiImage->flashOffset, pSpiImage->flashOffset);
            do {

                //4Byte Address Mode (>16MByte)
				Enable4ByteFlag = 0;
                if(pSpiImage->fileLength > (16*1024*1024))
                    Enable4ByteFlag = 1;
                if((pSpiImage->flashOffset + offset + TRANSFER_LEN) > (16*1024*1024))
                    Enable4ByteFlag = 1;

                if(Enable4ByteFlag)
                    MSG_DEBUG("PACK_VERIFY_MODE  Enable4ByteFlag %d:  pSpiImage->flashOffset=0x%08x(%d)  offset=0x%08x(%d)\n", Enable4ByteFlag, pSpiImage->flashOffset, pSpiImage->flashOffset, offset, offset);

                if (offset == 0) {
                    if (ppack.imagetype == UBOOT) {   // system image
                        pSpiImage->fileLength = pSpiImage->fileLength - 16 - ddrlen;
                        usiRead(pSpiImage->flashOffset+16+ddrlen+offset+((FW_NOR_IMAGE_T *)pSpiImage)->initSize, TRANSFER_LEN, (UINT8 *)(DOWNLOAD_BASE));
                        //sysprintf("@0000 %d, %d, %d, ((FW_NOR_IMAGE_T *)pSpiImage)->initSize->%d, spi read addr = 0x%x \n", pSpiImage->flashOffset,ddrlen, offset, ((FW_NOR_IMAGE_T *)pSpiImage)->initSize, pSpiImage->flashOffset+16+ddrlen+offset+((FW_NOR_IMAGE_T *)pSpiImage)->initSize);
                    } else {
                        usiRead(pSpiImage->flashOffset+offset, TRANSFER_LEN, (UINT8 *)(DOWNLOAD_BASE));
                        //sysprintf("0000@ spi read size = 0x%x \n", offset);
                    }
                } else {

                    if (ppack.imagetype == UBOOT) {   // system image
                        usiRead(pSpiImage->flashOffset+16+ddrlen+offset+((FW_NOR_IMAGE_T *)pSpiImage)->initSize, TRANSFER_LEN, (UINT8 *)(DOWNLOAD_BASE));
                        //sysprintf("@1111 %d, %d, %d, ((FW_NOR_IMAGE_T *)pSpiImage)->initSize->%d, spi read addr = 0x%x \n", pSpiImage->flashOffset,ddrlen, offset, ((FW_NOR_IMAGE_T *)pSpiImage)->initSize, pSpiImage->flashOffset+16+ddrlen+offset+((FW_NOR_IMAGE_T *)pSpiImage)->initSize);
                    } else {
                        usiRead(pSpiImage->flashOffset+offset, TRANSFER_LEN, (UINT8 *)(DOWNLOAD_BASE));
                        //sysprintf("1111@ spi read size = 0x%x \n", offset);
                    }
                }

                usb_send(ptr,TRANSFER_LEN); //send data to PC
                //sysprintf("@2222 SPI usb_send = 0x%x(%d), len = 0x%x(%d) \n", offset, offset, pSpiImage->fileLength, pSpiImage->fileLength);
                usb_recv((unsigned char*)_ack,4);   //recv data from PC
                //sysprintf("@3333 SPI usb_recv = 0x%x \n", (*_ack));
                //ptr += (*_ack);
                if(*_ack==0)
                    break;
                else
                    offset+=(*_ack);
            } while(offset<pSpiImage->fileLength);
        }
    }
    break;
    case READ_MODE: { // read
        MSG_DEBUG("SPI normal read !!!\n");
        MSG_DEBUG("offset=%d,flashOffset=%d,fileLength=%d\n",offset,pSpiImage->flashOffset,pSpiImage->fileLength);
        _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
        ptr=_ch;

        /* for debug or delay */
        {
            usleep(1000);
            *_ack=0x85;
            usb_send((unsigned char*)_ack,4);//send ack to PC
        }


        remain=pSpiImage->fileLength;
        offset=pSpiImage->flashOffset;
        while(TRANSFER_LEN<remain) {

			Enable4ByteFlag = 0;
            // if(pSpiImage->fileLength > (16*1024*1024))
                // Enable4ByteFlag = 1;
            if((offset + TRANSFER_LEN) > (16*1024*1024))
                Enable4ByteFlag = 1;

            if(Enable4ByteFlag)
                MSG_DEBUG("Enable4ByteFlag %d:  offset=0x%08x(%d)\n", Enable4ByteFlag, offset, offset);
            memset(ptr, 0, TRANSFER_LEN);
            usiRead(offset, TRANSFER_LEN , ptr);
            usb_send(ptr,TRANSFER_LEN); //send data to PC
            usb_recv((unsigned char*)_ack,4);   //recv data from PC
            remain-=TRANSFER_LEN;
            offset+=TRANSFER_LEN;
        }

        if(remain>0) {
            //4Byte Address Mode (>16MByte)
			Enable4ByteFlag = 0;
            // if(pSpiImage->fileLength > (16*1024*1024))
                // Enable4ByteFlag = 1;
            if((offset + TRANSFER_LEN) > (16*1024*1024))
                Enable4ByteFlag = 1;

            if(Enable4ByteFlag)
                MSG_DEBUG("Enable4ByteFlag %d:  offset=0x%08x(%d)\n", Enable4ByteFlag, offset, offset);		
            memset(ptr, 0, TRANSFER_LEN);
            usiRead(offset, remain , ptr);
            usb_send(ptr,TRANSFER_LEN); //send data to PC
            usb_recv((unsigned char*)_ack,4);   //recv data from PC
        }
    }
    break;
    case PACK_MODE: {
        MSG_DEBUG("SPI pack mode !!!\n");
        /* for debug or delay */
        {
            usleep(1000);
            *_ack=0x85;
            usb_send((unsigned char*)_ack,4);//send ack to PC
        }

        _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
        ptr=_ch;
        {
            PACK_HEAD pack;
            PACK_CHILD_HEAD ppack;
            while(1) {
                if(Bulk_Out_Transfer_Size>=sizeof(PACK_HEAD)) {
                    usb_recv(ptr,sizeof(PACK_HEAD));
                    memcpy(&pack,(unsigned char *)ptr,sizeof(PACK_HEAD));
                    *_ack=sizeof(PACK_CHILD_HEAD);
                    usb_send((unsigned char*)_ack,4);//send ack to PC
                    break;
                }
            }
            MSG_DEBUG("pack.actionFlag=0x%x, pack.fileLength=0x%08x pack.num=%d!!!\n",pack.actionFlag,pack.fileLength,pack.num);
            for(i=0; i<pack.num; i++) {
                while(1) {
                    if(Bulk_Out_Transfer_Size>=sizeof(PACK_CHILD_HEAD)) {
                        usb_recv(ptr,sizeof(PACK_CHILD_HEAD));
                        memcpy(&ppack,(unsigned char *)ptr,sizeof(PACK_CHILD_HEAD));
                        break;
                    }
                }
                MSG_DEBUG("ppack.filelen=0x%x, ppack.startaddr=0x%08x!!!\n",ppack.filelen,ppack.startaddr);
                ptr=_ch;
                do {

                    //if(Bulk_Out_Transfer_Size>=MIN(4096,ppack.filelen))
                    if(Bulk_Out_Transfer_Size>0) {
                        len=Bulk_Out_Transfer_Size;
                        usb_recv(ptr,len);  //recv data from PC
                        ptr+=len;
                        *_ack=len;
                        usb_send((unsigned char*)_ack,4);//send ack to PC
                    }
                } while((ptr-_ch)<ppack.filelen);
                if(ppack.imagetype!=PMTP) {
                    Burn_SPI(ppack.filelen,((ppack.startaddr+SPI_BLOCK_SIZE-1)/SPI_BLOCK_SIZE)*SPI_BLOCK_SIZE);
                    //Burn_FastSPI(ppack.filelen,((ppack.startaddr+SPI_BLOCK_SIZE-1)/SPI_BLOCK_SIZE)*SPI_BLOCK_SIZE);

                } else { /* if(ppack.imagetype==PMTP) */
                    UINT32 m_mtp_mode,m_mtp_option,m_mtp_encrypt,m_mtp_enable,j;
                    FW_OTP_IMAGE_T MTPImage;
                    m_mtp_mode=PACK_Mode(ppack.startaddr);
                    m_mtp_option=PACK_Option(ppack.startaddr);
                    m_mtp_encrypt=PACK_Encrypt(ppack.startaddr);
                    m_mtp_enable=PACK_Enable(ppack.startaddr);
                    ptr=_ch;
                    MTPImage.Mode = m_mtp_mode;
                    MTPImage.Option=(1<<m_mtp_option) | ((1-m_mtp_encrypt)<<2);
                    MTPImage.KeyLen = ppack.filelen;
                    MTPImage.Lock = m_mtp_enable;
                    memcpy(&MTPImage.KeyOTP[0],_ch,MTPImage.KeyLen);
                    MSG_DEBUG("Mode %d, Option 0x%x, Lock %d\n", m_mtp_mode, m_mtp_option, m_mtp_enable);
                    MSG_DEBUG("KeyMTP Len = 0x%08x \n", MTPImage.KeyLen);
                    for(j=0; j<8; j++)
                        MSG_DEBUG("KeyMTP[%d] = 0x%08x \n",j, MTPImage.KeyOTP[j]);
                    if(!MTP_IsLock()) {
                        MTP_Program_Key((UINT8 *)(&MTPImage.KeyOTP[0]),MTPImage.KeyLen,MTPImage.Option,MTPImage.Mode);
                        if(MTPImage.Lock==1)
                            MTP_Lock();
                    }
                    SendAck(100);
                }
            }
        }
    }
    break;
    default:
        ;
    }
    MSG_DEBUG("finish SPI flash image download\n");
}

extern void fw_dtbfunc(int,int,int,int);
int Run_SDRAM(UINT32 address,UINT32 offset,UINT32 tmpAddr,UINT32 dtbEn,UINT32 dtbAddr)
{
    unsigned int i;
    int volatile status = 0;
    void    (*fw_func)(int, int, int);
    fw_func = (void(*)(int, int, int))address;
#if 1
    /* Disable all interrupts */
    sysSetGlobalInterrupt(DISABLE_ALL_INTERRUPTS);
    sysSetLocalInterrupt(DISABLE_FIQ_IRQ);

    sysFlushCache(I_D_CACHE);
//  outpw(REG_USBD_PHY_CTL, inpw(REG_USBD_PHY_CTL) & ~0x200);    // offset 0x704

    outpw(REG_AHBIPRST,1<<19);  //USBD reset
    outpw(REG_AHBIPRST,0<<19);
    outpw(REG_USBD_PHY_CTL, inpw(REG_USBD_PHY_CTL) & ~0x100);
    outpw(REG_HCLKEN, inpw(REG_HCLKEN) & ~0x80000);
    MSG_DEBUG("run ... %x\n", address);

    UNLOCKREG();
    outpw(REG_HCLKEN, 0x00000527);
    outpw(REG_PCLKEN0, 0);
    outpw(REG_PCLKEN1, 0);
    for(i=(unsigned int)REG_MFP_GPA_L; i<=(unsigned int)REG_MFP_GPI_H; i+=4)
        outpw(i,0x00);
    LOCKREG();
#endif
    //outpw(REG_PCLKEN0, 1<<16);  //enable uart0

    if(address<offset)
        memcpy((unsigned char *)address,(unsigned char *)tmpAddr,offset);

    if(dtbEn==1)
        fw_dtbfunc(0,0,dtbAddr,address);
    else
        fw_func(0, ChipID, 0x100);
    return status;
}

void UXmodem_SDRAM(void)
{
    unsigned int actionFlag=0, fileAddr=0, dtbAddr=0, fileSize=0, exeFlag=0, dtbFlag=0;
    int len;
    unsigned char *ptr;
    unsigned char buf[80];
    unsigned char *_ch;
    unsigned int *_ack;

    int Otag;
    unsigned int offset=0x40;
    unsigned int tmpAddr=((EXEADDR-0x100000)-offset);

    MSG_DEBUG("download image to SDRAM...\n");

    _ch=((unsigned char*)(((unsigned int)buf)|NON_CACHE));
    _ack=((unsigned int*)(((unsigned int)buf)|NON_CACHE));
    ptr=_ch;

    while(1) {
        if(Bulk_Out_Transfer_Size>0) {
            usb_recv(ptr,16);

            actionFlag=*((unsigned int*)ptr);
            fileSize=*((unsigned int*)(ptr+4));
            fileAddr=*((unsigned int*)(ptr+8));
            dtbAddr=*((unsigned int*)(ptr+12));
            break;
        }
    }

    /* for debug or delay */
    {
        *_ack=0x80;
        usb_send((unsigned char*)_ack,4);//send ack to PC
    }

    MSG_DEBUG("action flag %d, address 0x%x, len %d, dtbAddr 0x%x\n", actionFlag, fileAddr, fileSize, dtbAddr);
    /* check if the image need execute */
    if (fileAddr & 0x80000000) {
        exeFlag = 1;
        fileAddr = fileAddr & 0x7FFFFFFF;
    }

    if (dtbAddr & 0x80000000) {
        dtbFlag = 1;
        dtbAddr = dtbAddr & 0x7FFFFFFF;
    }

    _ch=((unsigned char*)(((unsigned int)fileAddr | NON_CACHE)));
    if(fileAddr<offset && exeFlag == 1) {
        Otag=1;
        ptr=_ch+offset;
    } else {
        Otag=0;
        ptr=_ch;
    }

    do {
        if(Bulk_Out_Transfer_Size>0) {
            len=Bulk_Out_Transfer_Size;
            usb_recv(ptr,len);  //recv data from PC
            if(Otag==1) {
                memcpy((unsigned char *)tmpAddr,ptr,offset);
                memcpy(ptr,ptr+offset,len-offset);
                ptr+=(len-offset);
                Otag=0;
            } else {
                ptr+=len;
            }
            *_ack=len;
            usb_send((unsigned char*)_ack,4);//send ack to PC
        }
    } while((ptr-_ch)<fileSize);

    MSG_DEBUG("\nfinish SDRAM download ...\n");

    if (exeFlag == 1) { /* execute image */

#ifdef MSG_DEBUG_EN
        MSG_DEBUG("execute image ...\n");
#else
        sysDelay(100); /* Waiting for application recevie ack */
#endif

        if(dtbFlag == 1) {
            dtbAddr = dtbAddr & 0x7FFFFFFF;
            Run_SDRAM(fileAddr,offset,tmpAddr,1,dtbAddr);
        }
        Run_SDRAM(fileAddr,offset,tmpAddr,0,0);
    } else
        MSG_DEBUG("only download ...\n");
}

/******************************************************************************
 *
 *  eMMC Functions
 *
 ******************************************************************************/
extern FW_MMC_IMAGE_T mmcImage;
extern FW_MMC_IMAGE_T *pmmcImage;
extern void GetMMCImage(void);

void Burn_MMC_RAW(UINT32 len, UINT32 offset,UINT8 *ptr)
{
    MSG_DEBUG("offset=%d,len=%d\n",offset>>9,len>>9);
    fmiSD_Write(offset>>9, len>>9, (UINT32)ptr);
}

void Read_MMC_RAW(UINT32 len, UINT32 offset,UINT8 *ptr)
{
    MSG_DEBUG("offset=%d,len=%d\n",offset>>9,len>>9);
    fmiSD_Read(offset>>9, len>>9, (UINT32)ptr);
}
#if defined(BATCH_BRUN)
int BatchBurn_MMC_BOOT(UINT32 len,UINT32 offset)
{
    int volatile tmplen=0;
    UINT32 TotalBlkCount,blockCount, mmcSourceAddr;
    UINT8 infoBuf[512],*ptr;

    unsigned char *_ch;
    unsigned int *_ack,ack=0;
    unsigned int reclen,remainlen;

    _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
    _ack=((unsigned int*)(((unsigned int)ack)|NON_CACHE));

    /* set up interface */
    if (fmiInitSDDevice() < 0) {
        SendAck(0xFFFFFFFF);
        return Fail;
    }

    /* check image 0 / offset 0 and back up */
    //if (pmmcImage->imageType == UBOOT)  // system image, burn nomal image
    //    fmiSD_Read(MMC_INFO_SECTOR,1,(UINT32)pInfo);

    mmcSourceAddr = (DOWNLOAD_BASE | NON_CACHE);
    ptr = (UINT8 *)mmcSourceAddr;
    TotalBlkCount = blockCount = (len+((SD_SECTOR)-1))/(SD_SECTOR);

    while(blockCount>=SD_MUL) {
        ptr=_ch;
        remainlen=(SD_SECTOR*SD_MUL);
        do {
            if(Bulk_Out_Transfer_Size>0) {
                reclen=MIN(TRANSFER_LEN,remainlen);
                usb_recv(ptr,reclen);  //recv data from PC
                ptr+=reclen;
                remainlen-=reclen;
                *_ack=reclen;//|((pagetmp * 95) / total)<<16;
                usb_send((unsigned char*)_ack,4);//send ack to PC
            }
        } while((ptr-_ch)<(remainlen!=0));

        MSG_DEBUG("offset=0x%08x,ptr_addr=0x%08x,ptr=%d\n",offset,(UINT32)ptr,*(ptr));
        fmiSD_Write(offset,SD_MUL,(UINT32)ptr);
        blockCount-=SD_MUL;
        offset+=SD_MUL;
        /* ack status */
        tmplen += (SD_SECTOR*SD_MUL);
        //SendAck((tmplen * 95) / len);
    }
    if(blockCount!=0) {
        ptr=_ch;
        remainlen=len-((SD_SECTOR)*(TotalBlkCount-blockCount));
        do {
            if(Bulk_Out_Transfer_Size>0) {
                reclen=MIN(TRANSFER_LEN,remainlen);
                usb_recv(ptr,reclen);  //recv data from PC
                ptr+=reclen;
                remainlen-=reclen;
                *_ack=reclen;//|((pagetmp * 95) / total)<<16;
                usb_send((unsigned char*)_ack,4);//send ack to PC
            }
        } while((ptr-_ch)<(remainlen!=0));
        fmiSD_Write(offset,blockCount,(UINT32)ptr);
    }
    /* restore image 0 and offset 0 */
    if (pmmcImage->imageType == 3) { // system image, burn nomal image
        if((pmmcImage)->macaddr[7]==1)
            memcpy(infoBuf+0x190,(UCHAR *)((pmmcImage)->macaddr),6);  // MAC Address
    }

    if ((pmmcImage->flashOffset != 2) || (pmmcImage->imageType == 3)) {
        /* set MMC information */
        MSG_DEBUG("SetMMCImageInfo\n");
        SetMMCImageInfo(pmmcImage);
    }
    //ack status
    SendAck(100);

    return 0;
}
#endif

#if defined(BATCH_BRUN)
int BatchBurn_MMC(UINT32 len,UINT32 offset,UINT32 HeaderFlag)
{
    int volatile tmplen=0;
    UINT32 blockCount=0, mmcSourceAddr;
    UINT8 infoBuf[512],*ptr;

    unsigned char *_ch;
    unsigned int *_ack,ack=0;
    unsigned int reclen,remainlen;
    unsigned int headlen;

    if(HeaderFlag==1)
        headlen=16;
    else
        headlen=0;

    _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
    _ack=((unsigned int*)(((unsigned int)ack)|NON_CACHE));

    /* set up interface */
    if (fmiInitSDDevice() < 0) {
        SendAck(0xFFFFFFFF);
        return Fail;
    }

    /* check image 0 / offset 0 and back up */
    //if (pmmcImage->imageType == UBOOT)  // system image, burn nomal image
    //    fmiSD_Read(MMC_INFO_SECTOR,1,(UINT32)pInfo);

    mmcSourceAddr = (DOWNLOAD_BASE | NON_CACHE);
    ptr = (UINT8 *)mmcSourceAddr;
    tmplen=len;
    //sysprintf("debug 1111  blockCount=%d   tmplen=%d    _ack = 0x%x\n",blockCount, tmplen, *_ack);
    while(tmplen>TRANSFER_LEN) {
        ptr=_ch+headlen;
        remainlen=(SD_SECTOR*SD_MUL);
        do {
            if(Bulk_Out_Transfer_Size>0) {
                reclen=MIN(TRANSFER_LEN,remainlen);
                usb_recv(ptr,reclen);  //recv data from PC
                ptr+=reclen;
                remainlen-=reclen;
                *_ack=reclen;//|((pagetmp * 95) / total)<<16;
                usb_send((unsigned char*)_ack,4);//send ack to PC
            }
        } while(remainlen!=0);
        ptr=_ch;
        MSG_DEBUG("tmplen=0x%08x,ptr_addr=0x%08x,ptr=%d\n",tmplen,(UINT32)ptr,*(ptr));
        fmiSD_Write(offset,SD_MUL,(UINT32)ptr);
        blockCount-=SD_MUL;
        offset+=SD_MUL;
        tmplen -= (SD_SECTOR*SD_MUL);
        memcpy(_ch,_ch+(SD_SECTOR*SD_MUL),headlen);
    }
    if(tmplen!=0) {
        ptr=_ch+headlen;
        remainlen=tmplen;
        do {
            if(Bulk_Out_Transfer_Size>0) {
                reclen=MIN(TRANSFER_LEN,remainlen);
                usb_recv(ptr,reclen);  //recv data from PC
                ptr+=reclen;
                remainlen-=reclen;
                *_ack=reclen;//|((pagetmp * 95) / total)<<16;
                usb_send((unsigned char*)_ack,4);//send ack to PC
            }
        } while(remainlen!=0);
        ptr=_ch;
        MSG_DEBUG("tmplen=0x%08x,ptr_addr=0x%08x,ptr=%d\n",tmplen,(UINT32)ptr,*(ptr));
        fmiSD_Write(offset,(tmplen+SD_SECTOR-1)/SD_SECTOR,(UINT32)ptr);
    }
    /* restore image 0 and offset 0 */
    if (pmmcImage->imageType == 3) { // system image, burn nomal image
        if((pmmcImage)->macaddr[7]==1)
            memcpy(infoBuf+0x190,(UCHAR *)((pmmcImage)->macaddr),6);  // MAC Address
    }

    if ((pmmcImage->flashOffset != 2) || (pmmcImage->imageType == 3)) {
        /* set MMC information */
        MSG_DEBUG("SetMMCImageInfo\n");
        SetMMCImageInfo(pmmcImage);
    }
    //ack status
    SendAck(100);

    return 0;
}
#endif

int Burn_MMC(UINT32 len,UINT32 offset)
{
    int volatile tmplen=0;
    UINT32 blockCount, mmcSourceAddr;
    UINT8 infoBuf[512],*ptr;
    //UINT8 *pInfo;

    //pInfo = (UINT8 *)((UINT32)infoBuf | NON_CACHE);

    /* set up interface */
    if (fmiInitSDDevice() < 0) {
        SendAck(0xFFFFFFFF);
        return Fail;
    }

    /* check image 0 / offset 0 and back up */
    //if (pmmcImage->imageType == UBOOT)  // system image, burn nomal image
    //    fmiSD_Read(MMC_INFO_SECTOR,1,(UINT32)pInfo);

    mmcSourceAddr = (DOWNLOAD_BASE | NON_CACHE);
    ptr = (UINT8 *)mmcSourceAddr;
    blockCount = (len+((SD_SECTOR)-1))/(SD_SECTOR);
    while(blockCount>=SD_MUL) {
        MSG_DEBUG("offset=0x%08x,ptr_addr=0x%08x,ptr=%d\n",offset,(UINT32)ptr,*(ptr));
        fmiSD_Write(offset,SD_MUL,(UINT32)ptr);
        ptr+=(SD_SECTOR*SD_MUL);
        blockCount-=SD_MUL;
        offset+=SD_MUL;
        /* ack status */
        tmplen += (SD_SECTOR*SD_MUL);
        SendAck((tmplen * 95) / len);
    }
    if(blockCount!=0) {
        fmiSD_Write(offset,blockCount,(UINT32)ptr);
    }
    /* restore image 0 and offset 0 */
    if (pmmcImage->imageType == 3) { // system image, burn nomal image
        if((pmmcImage)->macaddr[7]==1)
            memcpy(infoBuf+0x190,(UCHAR *)((pmmcImage)->macaddr),6);  // MAC Address
    }

    if ((pmmcImage->flashOffset != 2) || (pmmcImage->imageType == 3)) {
        /* set MMC information */
        MSG_DEBUG("SetMMCImageInfo\n");
        SetMMCImageInfo(pmmcImage);
    }
    //ack status
    SendAck(100);

    return 0;
}

void UXmodem_MMC()
{
    int len,i, j, ret;
    PMBR pmbr;
    unsigned char *ptr;
    unsigned char buf[80];
    unsigned char *_ch;
    unsigned int *_ack;
    unsigned int blockCount,offset=0;
    PACK_HEAD pack;
    PACK_CHILD_HEAD ppack;
    unsigned int ddrlen;

    MSG_DEBUG("download image to eMMC flash...\n");
    /* initial eMMC */
    fmiInitSDDevice();

    memset((char *)&mmcImage, 0, sizeof(FW_MMC_IMAGE_T));
    pmmcImage = (FW_MMC_IMAGE_T *)&mmcImage;
    _ch=((unsigned char*)(((unsigned int)buf)|NON_CACHE));
    _ack=((unsigned int*)(((unsigned int)buf)|NON_CACHE));
    ptr=_ch;
    while(1) {
        if(Bulk_Out_Transfer_Size>0) {
            usb_recv(ptr,sizeof(FW_MMC_IMAGE_T));
            memcpy(pmmcImage,(unsigned char *)ptr,sizeof(FW_MMC_IMAGE_T));
            break;
        }
    }
    MSG_DEBUG("action flag %d, image %d, len=%d exec=0x%08x\n",
              pmmcImage->actionFlag, pmmcImage->imageNo,pmmcImage->fileLength,pmmcImage->executeAddr);

    switch(pmmcImage->actionFlag) {
    case WRITER_MODE: /* normal write */
        MSG_DEBUG("eMMC normal write !!!\n");
        /* for debug or delay */
        {
            usleep(1000);
            *_ack=(USBD_BURN_TYPE | USBD_FLASH_MMC);
            usb_send((unsigned char*)_ack,4);//send ack to PC
        }
        MSG_DEBUG("Wait for USBD !!!\n");
        _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
        ptr=_ch;

        if (pmmcImage->imageType == UBOOT) {    // system image

            pmmcImage->imageNo = 0;
            pmmcImage->flashOffset = 0x400;
            addMagicHeader(pmmcImage->executeAddr, pmmcImage->fileLength);
            ptr += 16;  // except the 16 bytes magic header
#if 0
            pmmcImage->executeAddr = 0x1200000;  /* 18MB */
#endif
        }
        MSG_DEBUG("Bulk_Out_Transfer_Size pmmcImage->fileLength=0x%08x!!!\n",pmmcImage->fileLength);
#if !defined(BATCH_BRUN)
        do {
            if(Bulk_Out_Transfer_Size>0) {

                len=Bulk_Out_Transfer_Size;
                MSG_DEBUG("usb_recv len=0x%08x!!!\n",len);
                usb_recv(ptr,len);  //recv data from PC
                ptr+=len;
                *_ack=len;
                MSG_DEBUG("usb_send *_ack=0x%08x!!!\n",*_ack);
                usb_send((unsigned char*)_ack,4);//send ack to PC
            }
        } while((ptr-_ch)<(pmmcImage->fileLength+pmmcImage->initSize));
#endif
        MSG_DEBUG("Burn_MMC!!!\n");
        if (pmmcImage->imageType == UBOOT) {
#if !defined(BATCH_BRUN)
            Burn_MMC(pmmcImage->fileLength+pmmcImage->initSize+16,(pmmcImage->flashOffset/SD_SECTOR));
#else
            ret = BatchBurn_MMC(pmmcImage->fileLength+pmmcImage->initSize+16,(pmmcImage->flashOffset/SD_SECTOR),1);
            if(ret == 1) {
                //sysprintf("XXXXX BatchBurn_MMC Device UBOOT image error !!! \n");
                return;
            }
#endif
        } else {
#if !defined(BATCH_BRUN)
            Burn_MMC(pmmcImage->fileLength,(pmmcImage->flashOffset/SD_SECTOR));
#else
            ret = BatchBurn_MMC(pmmcImage->fileLength,(pmmcImage->flashOffset/SD_SECTOR),0);
            if(ret == 1) {
                //sysprintf("XXXXX BatchBurn_MMC Device others image error !!! \n");
                return;
            }
#endif
        }

        break;
    case MODIFY_MODE: /* modify */
        MSG_DEBUG("eMMC modify !!!\n");
        {
            int state;
            state = ChangeMMCImageType(pmmcImage->imageNo, pmmcImage->imageType);
            if (state < 0) {
                //MSG_DEBUG("error!!\n");
                SendAck(0xFFFFFFFF);
            } else {
                //MSG_DEBUG("OK!!\n");
                SendAck(100);
            }
        }
        break;
    case ERASE_MODE: /* erase */
        MSG_DEBUG("eMMC erase !!!\n");
        /* for debug or delay */
        {
            usleep(1000);
            *_ack=(USBD_BURN_TYPE | USBD_FLASH_MMC);
            usb_send((unsigned char*)_ack,4);//send ack to PC
        }
        //if pmmcImage->imageNo = 0xFFFFFFFF then erase all
        DelMMCImage(pmmcImage->imageNo);
        break;
    case VERIFY_MODE: { // verify
        MSG_DEBUG("MMC normal verify !!!\n");
        /* for debug or delay */
        {
            usleep(1000);
            *_ack=(USBD_BURN_TYPE | USBD_FLASH_MMC);
            usb_send((unsigned char*)_ack,4);//send ack to PC
        }

        _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
        ptr=_ch;
        if(pmmcImage->imageType == UBOOT)
            blockCount = (pmmcImage->fileLength+16+pmmcImage->initSize+((SD_SECTOR)-1))/(SD_SECTOR);
        else
            blockCount = (pmmcImage->fileLength+((SD_SECTOR)-1))/(SD_SECTOR);
#if !defined(BATCH_BRUN)
        blockCount = (blockCount+8-1)/8;
        offset = pmmcImage->flashOffset/SD_SECTOR;
        while(blockCount) {
            fmiSD_Read(offset,8,(UINT32)ptr);
            ptr+=TRANSFER_LEN;
            MSG_DEBUG("offset=0x%08x,ptr_addr=0x%08x,ptr=%d\n",offset,(UINT32)ptr,*(ptr));
            offset+=8;
            blockCount--;
        }

        ptr=_ch;
        if(pmmcImage->imageType==UBOOT)
            ptr+=(16+pmmcImage->initSize);
        len=pmmcImage->fileLength;
        while(len>0) {
            usb_send(ptr,TRANSFER_LEN); //send data to PC
            usb_recv((unsigned char*)_ack,4);   //recv data from PC
            ptr+=TRANSFER_LEN;
            len-=TRANSFER_LEN;
        }
#else
        ptr=_ch;
        len=pmmcImage->fileLength;
        blockCount = (blockCount+8-1)/8;
        offset = pmmcImage->flashOffset/SD_SECTOR;

        if(pmmcImage->imageType==UBOOT) {
            fmiSD_Read(offset,8,(UINT32)ptr);
            offset+=8;
            if(blockCount==1) {
                ptr=_ch+(16+pmmcImage->initSize);
                usb_send(ptr,TRANSFER_LEN); //send data to PC
                usb_recv((unsigned char*)_ack,4);   //recv data from PC
            } else {
                INT32 mvlen,tranferlen;
                tranferlen=len;
                mvlen=(8*SD_SECTOR)-(16+pmmcImage->initSize);
                memmove(_ch,_ch+(16+pmmcImage->initSize),mvlen);
                for(i=1; i<blockCount; i++) {
                    ptr=_ch+mvlen;
                    fmiSD_Read(offset,8,(UINT32)ptr);
                    ptr=_ch;
                    usb_send(ptr,TRANSFER_LEN); //send data to PC
                    usb_recv((unsigned char*)_ack,4);   //recv data from PC
                    tranferlen-=TRANSFER_LEN;
                    //memcpy(ptr,_ch+(8*SD_SECTOR),(16+pmmcImage->initSize));
                    memmove(_ch,_ch+(8*SD_SECTOR),mvlen);
                    offset+=8;
                }
                if(tranferlen>0) {
                    usb_send(ptr,TRANSFER_LEN); //send data to PC
                    usb_recv((unsigned char*)_ack,4);   //recv data from PC
                }
            }
        } else {
            for(i=0; i<blockCount; i++) {
                fmiSD_Read(offset,8,(UINT32)ptr);
                usb_send(ptr,TRANSFER_LEN); //send data to PC
                usb_recv((unsigned char*)_ack,4);   //recv data from PC
                offset+=8;
            }
        }
#endif
    }
    break;
    case PACK_VERIFY_MODE: { // verify
        MSG_DEBUG("eMMC PACK verify !!!\n");

        /* for debug or delay */
        {
            usleep(1000);
            *_ack=(USBD_BURN_TYPE | USBD_FLASH_MMC);
            usb_send((unsigned char*)_ack,4);//send ack to PC
        }

        _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
        ptr=_ch;

        for(j=0; j<pmmcImage->imageNo; j++) {
            MSG_DEBUG("loop %d:  pmmcImage->imageNo = %d,  initSize=0x%08x\n", j, pmmcImage->imageNo, pmmcImage->initSize);
            while(1) {
                if(Bulk_Out_Transfer_Size>=sizeof(PACK_CHILD_HEAD)) {
                    usb_recv(ptr,sizeof(PACK_CHILD_HEAD));
                    memcpy(&ppack,(unsigned char *)ptr,sizeof(PACK_CHILD_HEAD));
                    usleep(1000);
                    *_ack=(USBD_BURN_TYPE | USBD_FLASH_MMC);
                    usb_send((unsigned char*)_ack,4);//send ack to PC
                    break;
                }
            }

            pmmcImage->imageType = ppack.imagetype;
            pmmcImage->fileLength = ppack.filelen;
            pmmcImage->flashOffset = ppack.startaddr;
            MSG_DEBUG("pmmcImage->fileLength = %d(0x%x)\n", pmmcImage->fileLength, pmmcImage->fileLength);
            MSG_DEBUG("pmmcImage->flashOffset = %d(0x%x)\n", pmmcImage->flashOffset, pmmcImage->flashOffset);
            MSG_DEBUG("pmmcImage->imageType = %d(0x%x)\n", pmmcImage->imageType, pmmcImage->imageType);

            if(pmmcImage->imageType == UBOOT) {
                blockCount = (pmmcImage->fileLength+16+pmmcImage->initSize+((SD_SECTOR)-1))/(SD_SECTOR);
                pmmcImage->flashOffset = 0x400;
                MSG_DEBUG("blockCount = %d\n", blockCount);
                MSG_DEBUG("pmmcImage->flashOffset = %d(0x%x)\n", pmmcImage->flashOffset, pmmcImage->flashOffset);

                // Get DDR parameter length
                usb_recv(ptr,4);
                memcpy(&ddrlen,(unsigned char *)ptr,4);
                MSG_DEBUG("ddrlen = %d\n", ddrlen);
                usleep(1000);
                *_ack=(USBD_BURN_TYPE | USBD_FLASH_MMC);
                usb_send((unsigned char*)_ack,4);//send ack to PC

            } else {
                blockCount = (pmmcImage->fileLength+((SD_SECTOR)-1))/(SD_SECTOR);
                MSG_DEBUG("blockCount = %d\n", blockCount);
            }

            ptr=_ch;
            len=pmmcImage->fileLength;
            blockCount = (blockCount+8-1)/8;
            offset = pmmcImage->flashOffset/SD_SECTOR;
            if(pmmcImage->imageType==UBOOT) {
                fmiSD_Read(offset,8,(UINT32)ptr);
                offset+=8;
                if(blockCount==1) {
                    ptr=_ch+(16+pmmcImage->initSize);
                    usb_send(ptr,TRANSFER_LEN); //send data to PC
                    usb_recv((unsigned char*)_ack,4);   //recv data from PC
                } else {
                    INT32 mvlen,tranferlen;
                    tranferlen=len;
                    mvlen=(8*SD_SECTOR)-(16+pmmcImage->initSize)-ddrlen;
                    memmove(_ch,_ch+(16+pmmcImage->initSize)+ddrlen,mvlen);
                    MSG_DEBUG("ddrlen = %d (0x%x)\n", ddrlen, ddrlen);
                    // mvlen=(8*SD_SECTOR)-(16+pmmcImage->initSize)-384;
                    // memmove(_ch,_ch+(16+pmmcImage->initSize)+384,mvlen);
                    // MSG_DEBUG("(8*SD_SECTOR)-(16+pmmcImage->initSize) = %d\n", (8*SD_SECTOR)-(16+pmmcImage->initSize));
                    for(i=1; i<blockCount; i++) {
                        ptr=_ch+mvlen;
                        fmiSD_Read(offset,8,(UINT32)ptr);
                        MSG_DEBUG("blockCount = %d, mvlen = %d, ptr[0~3] = 0x%x  0x%x  0x%x  0x%x, offset =%d\n",
                                  blockCount, mvlen, ptr[0], ptr[1], ptr[2], ptr[3], offset);
                        ptr=_ch;
                        usb_send(ptr,TRANSFER_LEN); //send data to PC
                        usb_recv((unsigned char*)_ack,4);   //recv data from PC
                        tranferlen-=TRANSFER_LEN;
                        //memcpy(ptr,_ch+(8*SD_SECTOR),(16+pmmcImage->initSize));
                        memmove(_ch,_ch+(8*SD_SECTOR),mvlen);
                        offset+=8;
                    }
                    if(tranferlen>0) {
                        usb_send(ptr,TRANSFER_LEN); //send data to PC
                        usb_recv((unsigned char*)_ack,4);   //recv data from PC
                    }
                }
            } else {
                for(i=0; i<blockCount; i++) {
                    fmiSD_Read(offset,8,(UINT32)ptr);
                    usb_send(ptr,TRANSFER_LEN); //send data to PC
                    usb_recv((unsigned char*)_ack,4);   //recv data from PC
                    offset+=8;
                }
            }
        }
    }
    break;
    case FORMAT_MODE: { /* Format */
        unsigned int *ptr;
        UCHAR _fmi_ucBuffer[512];
        ptr = (unsigned int *)((UINT32)_fmi_ucBuffer | 0x80000000);
        MSG_DEBUG("eMMC format !!!\n");

        /* for debug or delay */
        {
            usleep(1000);
            *_ack=(USBD_BURN_TYPE | USBD_FLASH_MMC);
            MSG_DEBUG("#1510 *_ack =0x%x\n", *_ack);
            usb_send((unsigned char*)_ack,4);//send ack to PC
            MSG_DEBUG("#1512 \n");
        }

        MSG_DEBUG("pmmcImage->ReserveSize = %d !!!\n",pmmcImage->ReserveSize);
        if(eMMCBlockSize>0) {
            pmbr=create_mbr(eMMCBlockSize,pmmcImage->ReserveSize);
            FormatFat32(pmbr,0);
            fmiSD_Read(MMC_INFO_SECTOR,1,(UINT32)ptr);
            *(ptr+125)=0x11223344;
            *(ptr+126)=pmmcImage->ReserveSize;
            *(ptr+127)=0x44332211;
            fmiSD_Write(MMC_INFO_SECTOR,1,(UINT32)ptr);
            MSG_DEBUG("#1524 \n");
            SendAck(100);
            MSG_DEBUG("#1526 \n");
            MSG_DEBUG("100!\n");
        } else
            SendAck(0xFFFFFFFF);
        MSG_DEBUG("#1530 END END END END END \n");
    }
    break;
    case READ_MODE: {
        MSG_DEBUG("MMC normal read !!!\n");

        /* for debug or delay */
        {
            usleep(1000);
            *_ack=(USBD_BURN_TYPE | USBD_FLASH_MMC);
            usb_send((unsigned char*)_ack,4);//send ack to PC
        }


        _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
        ptr=_ch;
        offset = pmmcImage->flashOffset;
        blockCount = (pmmcImage->fileLength+((SD_SECTOR)-1))/(SD_SECTOR);
        blockCount = (blockCount+8-1)/8;
        while(blockCount) {

            fmiSD_Read(offset,8,(UINT32)ptr);
            MSG_DEBUG("offset=0x%08x,ptr_addr=0x%08x,ptr=%d\n",offset,(UINT32)ptr,*(ptr));
            offset+=8;
            blockCount--;
            usb_send(ptr,4096); //send data to PC
            usb_recv((unsigned char*)_ack,4);   //recv data from PC
        }
    }
    break;
    case PACK_MODE: {
        MSG_DEBUG("MMC normal pack !!!\n");

        /* for debug or delay */
        {
            usleep(1000);
            *_ack=(USBD_BURN_TYPE | USBD_FLASH_MMC);
            usb_send((unsigned char*)_ack,4);//send ack to PC
        }

        _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
        ptr=_ch;
        {
            PACK_HEAD pack;
            PACK_CHILD_HEAD ppack;
            while(1) {
                if(Bulk_Out_Transfer_Size>=sizeof(PACK_HEAD)) {
                    usb_recv(ptr,sizeof(PACK_HEAD));
                    memcpy(&pack,(unsigned char *)ptr,sizeof(PACK_HEAD));
                    *_ack=sizeof(PACK_HEAD);
                    usb_send((unsigned char*)_ack,4);//send ack to PC
                    break;
                }
            }
            MSG_DEBUG("pack.actionFlag=0x%x, pack.fileLength=0x%08x pack.num=%d!!!\n",pack.actionFlag,pack.fileLength,pack.num);
            for(i=0; i<pack.num; i++) {
                while(1) {
                    if(Bulk_Out_Transfer_Size>=sizeof(PACK_CHILD_HEAD)) {
                        usb_recv(ptr,sizeof(PACK_CHILD_HEAD));
                        memcpy(&ppack,(unsigned char *)ptr,sizeof(PACK_CHILD_HEAD));
                        *_ack=sizeof(PACK_CHILD_HEAD);
                        usb_send((unsigned char*)_ack,4);//send ack to PC
                        break;
                    }
                }
                MSG_DEBUG("ppack.filelen=0x%x, ppack.startaddr=0x%08x!!!\n",ppack.filelen,ppack.startaddr);
#if !defined(BATCH_BRUN)
                ptr=_ch;
                do {
                    //if(Bulk_Out_Transfer_Size>=MIN(4096,ppack.filelen))
                    if(Bulk_Out_Transfer_Size>0) {
                        len=Bulk_Out_Transfer_Size;
                        usb_recv(ptr,len);  //recv data from PC
                        ptr+=len;
                        *_ack=len;
                        usb_send((unsigned char*)_ack,4);//send ack to PC
                    }
                } while((ptr-_ch)<ppack.filelen);
#endif
#if !defined(BATCH_BRUN)
                Burn_MMC(ppack.filelen,ppack.startaddr/SD_SECTOR);
#else
                if(ppack.imagetype!=PMTP) {
                    if(ppack.imagetype==UBOOT)
                        ppack.startaddr=0x400;
                    BatchBurn_MMC(ppack.filelen,ppack.startaddr/SD_SECTOR,0);
                } else { /* if(ppack.imagetype==PMTP) */
                    UINT32 m_mtp_mode,m_mtp_option,m_mtp_encrypt,m_mtp_enable,j;
                    FW_OTP_IMAGE_T MTPImage;
                    m_mtp_mode=PACK_Mode(ppack.startaddr);
                    m_mtp_option=PACK_Option(ppack.startaddr);
                    m_mtp_encrypt=PACK_Encrypt(ppack.startaddr);
                    m_mtp_enable=PACK_Enable(ppack.startaddr);
                    ptr=_ch;
                    do {
                        if(Bulk_Out_Transfer_Size>0) {
                            len=Bulk_Out_Transfer_Size;
                            usb_recv(ptr,len);  //recv data from PC
                            ptr+=len;
                            *_ack=len;
                            usb_send((unsigned char*)_ack,4);//send ack to PC
                        }
                    } while((ptr-_ch)<ppack.filelen);
                    MTPImage.Mode = m_mtp_mode;
                    MTPImage.Option=(1<<m_mtp_option) | ((1-m_mtp_encrypt)<<2);
                    MTPImage.KeyLen = ppack.filelen;
                    MTPImage.Lock = m_mtp_enable;
                    memcpy(&MTPImage.KeyOTP[0],_ch,MTPImage.KeyLen);
                    MSG_DEBUG("Mode %d, Option 0x%x, Lock %d\n", m_mtp_mode, m_mtp_option, m_mtp_enable);
                    MSG_DEBUG("KeyMTP Len = 0x%08x \n", MTPImage.KeyLen);
                    for(j=0; j<8; j++)
                        MSG_DEBUG("KeyMTP[%d] = 0x%08x \n",j, MTPImage.KeyOTP[j]);
                    if(!MTP_IsLock()) {
                        MTP_Program_Key((UINT8 *)(&MTPImage.KeyOTP[0]),MTPImage.KeyLen,MTPImage.Option,MTPImage.Mode);
                        if(MTPImage.Lock==1)
                            MTP_Lock();
                    }
                    SendAck(100);
                }
#endif
            }
        }
    }
    break;
    }


}

/******************************************************************************
 *
 *  NAND Functions
 *
 ******************************************************************************/
FW_NAND_IMAGE_T nandImage;
FW_NAND_IMAGE_T *pNandImage;

void Burn_NAND_RAW(UINT32 len, UINT8 *ptr)
{
    MSG_DEBUG("len=%d\n",len);
}

void Read_NAND_RAW(UINT32 len, UINT8 *ptr)
{
    MSG_DEBUG("len=%d\n", len);
}

#define DISABLE_YAFFS2
#if defined(BATCH_BRUN)
int BatchBurn_NAND(UINT32 len,UINT32 blockNo,UINT32 type)
{
    int volatile status = 0;
    int volatile page_count, block_count, page, addr, blockNum, total;
    int volatile i, j, pagetmp=0;
    unsigned int address = DOWNLOAD_BASE;

    unsigned int sparesize;  //Put yaffs2 tag on the oob(sparesize).

    unsigned char *_ch,*ptr;
    unsigned int *_ack,ack=0;
    unsigned int reclen,remainlen;
    _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
    _ack=((unsigned int*)(((unsigned int)ack)|NON_CACHE));
    ptr=_ch;

#ifndef DISABLE_YAFFS2
    if(type==YAFFS2 && (len%512)!=0)
        sparesize=pSM->uSpareSize;
    else
#endif
        sparesize=0;

    page_count = len / (pSM->uPageSize+sparesize);
    if(len%(pSM->uPageSize+sparesize)!=0) page_count++;
    // erase needed blocks
    block_count = page_count / (pSM->uPagePerBlock);


    // write into flash
    blockNum = blockNo;
    total = len;
    MSG_DEBUG("blockNum=%d,total=%d\n",blockNum,total);
    for (j=0; j<block_count; j++) {
        MSG_DEBUG("%d,%d\n",j,block_count);
        ptr=_ch;
        remainlen=MIN(total,pSM->uPagePerBlock*(pSM->uPageSize+sparesize));
        do {
            if(Bulk_Out_Transfer_Size>0) {
                MSG_DEBUG("total=%08d,remainlen=%08d\n",total,remainlen);
                reclen=MIN(TRANSFER_LEN,remainlen);
                usb_recv(ptr,reclen);  //recv data from PC
                ptr+=reclen;
                remainlen-=reclen;
                total-=reclen;
                *_ack=reclen;//|((pagetmp * 95) / total)<<16;
                usb_send((unsigned char*)_ack,4);//send ack to PC
            }
        } while(remainlen!=0);

        MSG_DEBUG("remainlen OK\n");
_retry_1:
        addr = address | NON_CACHE;
        page = pSM->uPagePerBlock * (blockNum);
        status = fmiSM_BlockErase(pSM, blockNum); // block erase
        //if (status != 0) {
        if (status == 1) {
            fmiMarkBadBlock(pSM, blockNum);
            blockNum++;
            goto _retry_1;
        } else if (status == -1) {
            MSG_DEBUG("XXXXXXX device error !! \n");

            usb_recv(ptr,Bulk_Out_Transfer_Size);  //recv data from PC
            //SendAck(0xffff);
            *_ack=0xffff;
            usb_send((unsigned char*)_ack,4);//send ack to PC
            return status;
        }
        // write block
        for (i=0; i<pSM->uPagePerBlock; i++) {
#ifndef DISABLE_YAFFS2
            if(type==YAFFS2 && (len%512)!=0) {
                status = fmiSM_Write_large_page_oob(page+i, 0, addr,pSM->uSpareSize);
            } else
#endif
                status = fmiSM_Write_large_page(page+i, 0, addr);
            if (status != 0) {
                fmiMarkBadBlock(pSM, blockNum);
                blockNum++;
                goto _retry_1;
            }
            addr += pSM->uPageSize;
#ifndef DISABLE_YAFFS2
            if(type==YAFFS2 && (len%512)!=0)
                addr += (pSM->uPageSize>>5);
#endif
            pagetmp++;
            //SendAck((pagetmp * 95) / total);
        }
        blockNum++;
    }
    MSG_DEBUG("page_count=%d,pSM->uPagePerBlock=%d\n",page_count,pSM->uPagePerBlock);
    if ((page_count % pSM->uPagePerBlock) != 0) {
        MSG_DEBUG("Cnt=%d\n",page_count % pSM->uPagePerBlock);
        memset(_ch,0xff,pSM->uPagePerBlock*(pSM->uPageSize+sparesize));
        ptr=_ch;
        remainlen=total;//(len-block_count*(pSM->uPagePerBlock*(pSM->uPageSize+sparesize)));
        do {
            if(Bulk_Out_Transfer_Size>0) {
                MSG_DEBUG("remainlen=%d\n",remainlen);
                reclen=MIN(TRANSFER_LEN,remainlen);
                usb_recv(ptr,reclen);  //recv data from PC
                ptr+=reclen;
                remainlen-=reclen;
                total-=reclen;
                *_ack=reclen;//|((pagetmp * 95) / total)<<16;
                usb_send((unsigned char*)_ack,4);//send ack to PC
            }
        } while(remainlen!=0);
        page_count = page_count - block_count * pSM->uPagePerBlock;
        MSG_DEBUG("page_count=%d\n",page_count);
_retry_2:
        addr = address | NON_CACHE;
        page = pSM->uPagePerBlock * (blockNum);
        status = fmiSM_BlockErase(pSM, blockNum);      // erase block
        //if (status != 0) {
        if (status == 1) {
            fmiMarkBadBlock(pSM, blockNum);
            blockNum++;
            goto _retry_2;
        } else if (status == -1) {
            MSG_DEBUG("device error !! \n");
            SendAck(0xffff);
            return status;
        }
        // write block
        MSG_DEBUG("page_count=%d,type=%d\n",page_count,type);
        for (i=0; i<page_count; i++) {
            MSG_DEBUG("i=%d\n",i);
#ifndef DISABLE_YAFFS2
            if(type==YAFFS2 && (len%512)!=0)
                status = fmiSM_Write_large_page_oob(page+i, 0, addr,pSM->uPageSize>>5);
            else
#endif
                status = fmiSM_Write_large_page(page+i, 0, addr);

            if (status != 0) {
                fmiMarkBadBlock(pSM, blockNum);
                blockNum++;
                goto _retry_2;
            }
            addr += pSM->uPageSize;
#ifndef DISABLE_YAFFS2
            if(type==YAFFS2 && (len%512)!=0)
                addr += (pSM->uPageSize>>5);
#endif
            pagetmp++;
            //SendAck((pagetmp * 95) / total);
        }
        blockNum++;
    }

    //SendAck(100);
    SendAck(blockNum-blockNo);
    return status;
}
#endif


int BatchBurn_NAND_Data_OOB(UINT32 len,UINT32 blockNo,UINT32 type)
{
    int volatile status = 0;
    int volatile page_count, block_count, page, addr, blockNum, total;
    int volatile i, j, pagetmp=0;
    unsigned int address = DOWNLOAD_BASE;
    unsigned int BurnLen;
    unsigned int sparesize;  //Put yaffs2 tag on the oob(sparesize).

    unsigned char *_ch,*ptr;
    unsigned int *_ack,ack=0;
    unsigned int reclen,remainlen;
    _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
    _ack=((unsigned int*)(((unsigned int)ack)|NON_CACHE));
    ptr=_ch;

    sparesize=pSM->uSpareSize;
    page_count = len / (pSM->uPageSize+sparesize);
    if(len%(pSM->uPageSize+sparesize)!=0) page_count++;
    // erase needed blocks
    block_count = page_count / (pSM->uPagePerBlock);


    // write into flash
    blockNum = blockNo;
    total = len;
    BurnLen=(pSM->uPageSize+sparesize);
    MSG_DEBUG("blockNum=%d,total=%d,block_count=%d\n",blockNum,total,block_count);
    for (j=0; j<block_count; j++) {
        MSG_DEBUG("%d,%d\n",j,block_count);
        ptr=_ch;
        remainlen=MIN(total,pSM->uPagePerBlock*(pSM->uPageSize+sparesize));
        do {
            if(Bulk_Out_Transfer_Size>0) {
                MSG_DEBUG("total=%08d,remainlen=%08d\n",total,remainlen);
                reclen=MIN(BurnLen,remainlen);
                usb_recv(ptr,reclen);  //recv data from PC
                ptr+=reclen;
                remainlen-=reclen;
                total-=reclen;
                *_ack=reclen;//|((pagetmp * 95) / total)<<16;
                usb_send((unsigned char*)_ack,4);//send ack to PC
            }
        } while(remainlen!=0);

        MSG_DEBUG("remainlen OK\n");
//_retry_1:
        addr = address | NON_CACHE;
        page = pSM->uPagePerBlock * (blockNum);
        status = fmiSM_BlockEraseBad(pSM, blockNum); // block erase
        // write block
        for (i=0; i<pSM->uPagePerBlock; i++) {
            status = fmiSM_Write_large_page_oob2(page+i, 0, addr);
//             if (status != 0)
//             {
//                 fmiMarkBadBlock(pSM, blockNum);
//                 blockNum++;
//                 goto _retry_1;
//             }
            addr += pSM->uPageSize+sparesize;
            pagetmp++;
            //SendAck((pagetmp * 95) / total);
        }
        blockNum++;
    }
    MSG_DEBUG("page_count=%d,pSM->uPagePerBlock=%d\n",page_count,pSM->uPagePerBlock);

    //SendAck(100);
    SendAck(blockNum-blockNo);
    return status;
}

int Burn_NAND(UINT32 len,UINT32 blockNo,UINT32 type)
{
    int volatile status = 0;
    int volatile page_count, block_count, page, addr, blockNum, total;
    int volatile i, j, pagetmp=0;
    unsigned int address = DOWNLOAD_BASE;
    unsigned int *_ack;
    volatile char *ptr;

    page_count = len / pSM->uPageSize;
    if ((len % pSM->uPageSize) != 0)
        page_count++;

    // write into flash
    blockNum = blockNo;
    total = page_count;

    // define for USB ack
    addr = address | NON_CACHE;

    // erase needed blocks
    block_count = page_count / pSM->uPagePerBlock;

    for (j=0; j<block_count; j++) {
        // block erase
_retry_1:
        page = pSM->uPagePerBlock * (blockNum);
        status = fmiSM_BlockErase(pSM, blockNum);
        //if (status != 0) {
        if (status == 1) {
            fmiMarkBadBlock(pSM, blockNum);
            blockNum++;
            goto _retry_1;
        } else if (status == -1) {
            MSG_DEBUG("Burn_NAND device error !! \n");
            //SendAck(0xffff);
            usb_recv(ptr, Bulk_Out_Transfer_Size);  //recv data from PC
            *_ack = 0xFFFF;
            usb_send((unsigned char*)_ack,4);//send ack to PC
            return status;
        }

        // write block
        for (i=0; i<pSM->uPagePerBlock; i++) {
#ifndef DISABLE_YAFFS2
            if(type==YAFFS2 && (len%512)!=0) {
                status = fmiSM_Write_large_page_oob(page+i, 0, addr,pSM->uSpareSize);
            } else
#endif
                status = fmiSM_Write_large_page(page+i, 0, addr);

            if (status != 0) {
                fmiMarkBadBlock(pSM, blockNum);
                blockNum++;
                addr = (address + j * pSM->uPagePerBlock * pSM->uPageSize) | NON_CACHE;
                goto _retry_1;
            }
            addr += pSM->uPageSize;
#ifndef DISABLE_YAFFS2
            if(type==YAFFS2 && (len%512)!=0)
                addr += (pSM->uPageSize>>5);
#endif

            pagetmp++;
            SendAck((pagetmp * 95) / total);
        }
        blockNum++;
    }

    if ((page_count % pSM->uPagePerBlock) != 0) {
        page_count = page_count - block_count * pSM->uPagePerBlock;
        block_count++;
        // erase block
_retry_2:
        page = pSM->uPagePerBlock * (blockNum);
        status = fmiSM_BlockErase(pSM, blockNum);
        if (status != 0) {
            fmiMarkBadBlock(pSM, blockNum);
            blockNum++;
            goto _retry_2;
        }

        // write block
        for (i=0; i<page_count; i++) {
#ifndef DISABLE_YAFFS2
            if(type==YAFFS2 && (len%512)!=0)
                status = fmiSM_Write_large_page_oob(page+i, 0, addr,pSM->uPageSize>>5);
            else
#endif
                status = fmiSM_Write_large_page(page+i, 0, addr);

            if (status != 0) {
                fmiMarkBadBlock(pSM, blockNum);
                blockNum++;
                addr = (address + block_count * pSM->uPagePerBlock * pSM->uPageSize) | NON_CACHE;
                goto _retry_2;
            }
            addr += pSM->uPageSize;
#ifndef DISABLE_YAFFS2
            if(type==YAFFS2 && (len%512)!=0)
                addr += (pSM->uPageSize>>5);
#endif
            pagetmp++;
            SendAck((pagetmp * 95) / total);
        }
        blockNum++;
    }

    SendAck(100);
    SendAck(blockNum-blockNo);
    return status;
}

int Read_Nand(UINT32 dst_adr,UINT32 blockNo, UINT32 len)
{
    volatile char *ptr;
    int spareSize;
    int volatile status = 0;
    int volatile page_count, block_count, blockNum;
    int volatile page_no, count, i=0, j=0, k;

    page_count = len / pSM->uPageSize;
    if ((len % pSM->uPageSize) != 0) page_count++;
    blockNum = blockNo;
    count = len;
    while(1) {
        MSG_DEBUG("blockNum=%d to start reading...\n",blockNum);
        if (fmiCheckInvalidBlock(pSM, blockNum) != 1) {
            for (i=0; i<pSM->uPagePerBlock; i++) {
                page_no = blockNum * pSM->uPagePerBlock + i;

                //--- read redunancy area to register SMRAx
                spareSize = inpw(REG_SMREACTL) & 0x1ff;
                ptr = (volatile char *)REG_SMRA0;
                fmiSM_Read_RA(page_no, pSM->uPageSize);
                for (k=0; k<spareSize; k++)
                    *ptr++ = inpw(REG_SMDATA) & 0xff;                   // copy RA data from NAND to SMRA by SW
                fmiSM_Read_large_page(pSM, page_no, (UINT32)dst_adr);
                dst_adr += pSM->uPageSize;
                count -= pSM->uPageSize;
            }
            j++;
        } else {
            sysprintf("block%d is bad block...\n",blockNum);
        }
        blockNum++;
        if ((j >= block_count) || (count <= 0))
            break;
    }
    return 0;
}

int Read_Nand_Redunancy(UINT32 dst_adr,UINT32 blockNo, UINT32 len)
{
    volatile char *ptr;
    volatile char *dst_redunancy_adr;
    int spareSize;
    int volatile status = 0;
    int volatile page_count, block_count, blockNum;
    int volatile page_no, count, i=0, j=0, k;

    page_count = len / pSM->uPageSize;
    if ((len % pSM->uPageSize) != 0) page_count++;
    blockNum = blockNo;
    block_count = page_count / pSM->uPagePerBlock;
    if ((page_count % pSM->uPagePerBlock) != 0)
        block_count++;
    count = len;
    while(1) {
        MSG_DEBUG("blockNum=%d to start reading...\n",blockNum);
        for (i=0; i<pSM->uPagePerBlock; i++) {
            dst_redunancy_adr=(volatile char *)(dst_adr + pSM->uPageSize);
            page_no = blockNum * pSM->uPagePerBlock + i;
            //--- read redunancy area to register SMRAx
            spareSize = inpw(REG_SMREACTL) & 0x1ff;
            ptr = (volatile char *)REG_SMRA0;
            fmiSM_Read_RA(page_no, pSM->uPageSize);
            for (k=0; k<spareSize; k++) {
                *dst_redunancy_adr = inpw(REG_SMDATA) & 0xff;                   // copy RA data from NAND to SMRA by SW
                *ptr++ = *dst_redunancy_adr;
                dst_redunancy_adr++;
            }
            fmiSM_Read_large_page(pSM, page_no, (UINT32)dst_adr);
            dst_adr += (pSM->uPageSize+pSM->uSpareSize);
            count -= (pSM->uPageSize+pSM->uSpareSize);
        }
        j++;
        blockNum++;
        if ((j >= block_count) || (count <= 0))
            break;
    }
    return 0;
}
#if defined(BATCH_BRUN)
int BatchBurn_NAND_BOOT(UINT32 len,UINT32 blockNo,UINT32 blockLen,UINT32 HeaderFlag)
{
    int volatile status = 0;
    int volatile page_count, block_count, page, addr, blockNum, total;
    int volatile blkindx,i, j, pagetmp=0;
    unsigned int address = DOWNLOAD_BASE;

    unsigned char *_ch,*ptr;
    unsigned int *_ack,ack=0;
    unsigned int reclen,remainlen;
    unsigned int headlen;

    if(HeaderFlag==1)
        headlen=16;
    else
        headlen=0;

    _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
    _ack=((unsigned int*)(((unsigned int)ack)|NON_CACHE));
    ptr=_ch;
    page_count = len / pSM->uPageSize;
    if ((len % pSM->uPageSize) != 0)
        page_count++;

    // write into flash
    blockNum = blockNo;
    total = page_count*blockLen;

    // define for USB ack
    addr = address | NON_CACHE;

    // erase needed blocks
    block_count = page_count / pSM->uPagePerBlock;

//   for (j=0; j<block_count; j++)
//   {
//       ptr=_ch+headlen;
//       remainlen=(pSM->uPagePerBlock*(pSM->uPageSize));
//       do{
//         if(Bulk_Out_Transfer_Size>0)
//         {
//             reclen=MIN(Bulk_Out_Transfer_Size,remainlen);
//             usb_recv(ptr,reclen);  //recv data from PC
//             ptr+=reclen;
//             remainlen-=reclen;
//             *_ack=reclen|((pagetmp * 95) / total)<<16;
//
//             usb_send((unsigned char*)_ack,4);//send ack to PC
//         }
//       }while(remainlen!=0);
//     // block erase
// //_retry_1:
//     for(blkindx=blockNum;blkindx<blockLen;blkindx++)
//     {
//       addr = address | NON_CACHE;
//       page = pSM->uPagePerBlock * (blkindx);
//       status = fmiSM_BlockErase(pSM, blkindx);
//       if (status != 0)
//       {
//          fmiMarkBadBlock(pSM, blkindx);
//          MSG_DEBUG("bad block = %d\n",blkindx);
//          //blockNum++;
//          continue;
//       }

//       // write block
//       for (i=0; i<pSM->uPagePerBlock; i++)
//       {
//         status = fmiSM_Write_large_page(page+i, 0, addr);
//         if (status != 0)
//         {
//           fmiMarkBadBlock(pSM, blkindx);
//           MSG_DEBUG("bad block = %d\n",blkindx);
//           //blockNum++;
//           addr = (address + j * pSM->uPagePerBlock * pSM->uPageSize) | NON_CACHE;
//           continue;
//         }
//         addr += pSM->uPageSize;
//         pagetmp++;
//       }
//     }
//     blockNum++;
//     memcpy(_ch,_ch+(pSM->uPagePerBlock * pSM->uPageSize),headlen);
//   }

    if (page_count <= pSM->uPagePerBlock) {
        //page_count = page_count - block_count * pSM->uPagePerBlock;
        memset(_ch+headlen,0xff,pSM->uPagePerBlock*(pSM->uPageSize));
        ptr=_ch+headlen;
        remainlen=len-(block_count*pSM->uPagePerBlock*pSM->uPageSize)-headlen;
        MSG_DEBUG("remainlen=%d,block_count=%d\n",remainlen,block_count);
        do {
            if(Bulk_Out_Transfer_Size>0) {
                reclen=MIN(Bulk_Out_Transfer_Size,remainlen);
                usb_recv(ptr,reclen);  //recv data from PC
                ptr+=reclen;
                remainlen-=reclen;
                *_ack=reclen|((pagetmp * 95) / total)<<16;
                usb_send((unsigned char*)_ack,4);//send ack to PC
            }
        } while(remainlen!=0);
        // erase block
//_retry_2:
        for(blkindx=blockNum; blkindx<blockLen; blkindx++) {
            addr = address | NON_CACHE;
            page = pSM->uPagePerBlock * (blkindx);
            status = fmiSM_BlockErase(pSM, blkindx);
            if (status != 0) {
                fmiMarkBadBlock(pSM, blkindx);
                //blockNum++;
                //goto _retry_2;
                continue;
            }

            // write block
            for (i=0; i<page_count; i++) {
                status = fmiSM_Write_large_page(page+i, 0, addr);
                if (status != 0) {
                    fmiMarkBadBlock(pSM, blkindx);
                    //blockNum++;
                    addr = (address + block_count * pSM->uPagePerBlock * pSM->uPageSize) | NON_CACHE;
                    //goto _retry_2;
                    continue;
                }
                addr += pSM->uPageSize;
                pagetmp++;
            }
        }
        blockNum++;
    } else {
        //MSG_DEBUG("Device image Error !!!\n");
        usb_recv(ptr, Bulk_Out_Transfer_Size);  //recv data from PC
        *_ack = 0xFFFF;
        usb_send((unsigned char*)_ack,4);//send ack to PC
        return -1;
    }
    SendAck(block_count+blockLen);
    return status;
}
#endif

int Burn_NAND_BACKUP(UINT32 len,UINT32 blockNo)
{
    int volatile status = 0;
    int volatile page_count, block_count, page, addr, blockNum, total;
    int volatile i, j, pagetmp=0;
    unsigned int address = DOWNLOAD_BASE;
    page_count = len / pSM->uPageSize;
    if ((len % pSM->uPageSize) != 0)
        page_count++;

    // write into flash
    blockNum = blockNo;
    total = page_count;

    // define for USB ack
    addr = address | NON_CACHE;

    // erase needed blocks
    block_count = page_count / pSM->uPagePerBlock;

    for (j=0; j<block_count; j++) {
        // block erase
//_retry_1:
        page = pSM->uPagePerBlock * (blockNum);
        status = fmiSM_BlockErase(pSM, blockNum);
        if (status != 0) {
            fmiMarkBadBlock(pSM, blockNum);
            MSG_DEBUG("bad block = %d\n",blockNum);
            blockNum++;
            continue;
        }

        // write block
        for (i=0; i<pSM->uPagePerBlock; i++) {
            status = fmiSM_Write_large_page(page+i, 0, addr);
            if (status != 0) {
                fmiMarkBadBlock(pSM, blockNum);
                MSG_DEBUG("bad block = %d\n",blockNum);
                blockNum++;
                addr = (address + j * pSM->uPagePerBlock * pSM->uPageSize) | NON_CACHE;
                continue;
            }
            addr += pSM->uPageSize;
            pagetmp++;
        }
        blockNum++;
    }

    if ((page_count % pSM->uPagePerBlock) != 0) {
        page_count = page_count - block_count * pSM->uPagePerBlock;
        block_count++;
        // erase block
//_retry_2:
        page = pSM->uPagePerBlock * (blockNum);
        status = fmiSM_BlockErase(pSM, blockNum);
        if (status != 0) {
            fmiMarkBadBlock(pSM, blockNum);
            blockNum++;
            //goto _retry_2;
            return status;
        }

        // write block
        for (i=0; i<page_count; i++) {
            status = fmiSM_Write_large_page(page+i, 0, addr);
            if (status != 0) {
                fmiMarkBadBlock(pSM, blockNum);
                blockNum++;
                addr = (address + block_count * pSM->uPagePerBlock * pSM->uPageSize) | NON_CACHE;
                //goto _retry_2;
                return status;
            }
            addr += pSM->uPageSize;
            pagetmp++;
        }
        blockNum++;
    }
    return status;
}

void UXmodem_NAND()
{
    int i,offset=0, ret;
    unsigned char *ptr;
    unsigned char buf[80];
    unsigned char *_ch;
    unsigned int *_ack;
    unsigned int len;
    unsigned int remainlen;
    PACK_HEAD pack;
    PACK_CHILD_HEAD ppack;
    unsigned int ddrlen;

    MSG_DEBUG("download image to NAND flash...\n");
    /* TODO: initial NAND */
    fmiNandInit();
    sysprintf("g_uIsUserConfig=%d\n",g_uIsUserConfig);
    sysprintf("UXmodem_NAND BlockPerFlash=%d\n",pSM->uBlockPerFlash);
    sysprintf("UXmodem_NAND PagePerBlock=%d\n",pSM->uPagePerBlock);
    sysprintf("UXmodem_NAND PageSize=%d\n",pSM->uPageSize);
    memset((char *)&nandImage, 0, sizeof(FW_NAND_IMAGE_T));
    pNandImage = (FW_NAND_IMAGE_T *)&nandImage;

    _ch=((unsigned char*)(((unsigned int)buf)|NON_CACHE));
    _ack=((unsigned int*)(((unsigned int)buf)|NON_CACHE));
    ptr=_ch;
    while(1) {
        if(Bulk_Out_Transfer_Size>0) {
            usb_recv(ptr,sizeof(FW_NAND_IMAGE_T));
            memcpy(pNandImage, (unsigned char *)ptr, sizeof(FW_NAND_IMAGE_T));
            break;
        }
    }
    MSG_DEBUG("action flag %d, blockNo %d len %d\n", pNandImage->actionFlag,pNandImage->blockNo, pNandImage->fileLength);

    switch (pNandImage->actionFlag) {
    case WRITER_MODE: { // normal write
        MSG_DEBUG("NAND normal write !!!\n");
        /* for debug or delay */
        {
            usleep(1000);
            *_ack=0x83;
            usb_send((unsigned char*)_ack,4);//send ack to PC
        }
        MSG_DEBUG("NAND normal write2 !!!\n");
        _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
        ptr=_ch;

        if (pNandImage->imageType == UBOOT) { // system image
            pNandImage->imageNo = 0;
            pNandImage->blockNo = 0;
            addMagicHeader(pNandImage->executeAddr, pNandImage->fileLength);
            ptr += 16;  // except the 16 bytes magic header
            offset = 16;
        } else
            offset = 0;

        len=pSM->uPagePerBlock*pSM->uPageSize;
#if !defined(BATCH_BRUN)
        do {
            if(Bulk_Out_Transfer_Size>0) {
                len=Bulk_Out_Transfer_Size;
                usb_recv(ptr,len);  //recv data from PC
                ptr+=len;
                *_ack=len;
                usb_send((unsigned char*)_ack,4);//send ack to PC
            }
            //MSG_DEBUG("%d\n",(ptr-_ch));
        } while((ptr-_ch)<(pNandImage->fileLength + offset + pNandImage->initSize));
#endif
        MSG_DEBUG("NAND normal Burn_NAND !!!\n");
        if (pNandImage->imageType == UBOOT) {   // system image
#if !defined(BATCH_BRUN)
            for(i=1; i<4; i++)
                Burn_NAND_BACKUP(pNandImage->fileLength + offset +((FW_NAND_IMAGE_T *)pNandImage)->initSize,i);
            Burn_NAND(pNandImage->fileLength + offset +((FW_NAND_IMAGE_T *)pNandImage)->initSize,0,pNandImage->imageType);
#else
            BatchBurn_NAND_BOOT(pNandImage->fileLength + offset +((FW_NAND_IMAGE_T *)pNandImage)->initSize,0,4,1);
#endif
        } else {
            if(pNandImage->imageType!=IMAGE) {
                if(pNandImage->imageType == DATA_OOB) {
                    MSG_DEBUG("DATA_OOB type\n");
                    BatchBurn_NAND_Data_OOB(pNandImage->fileLength + offset +((FW_NAND_IMAGE_T *)pNandImage)->initSize,pNandImage->blockNo/(pSM->uPagePerBlock*pSM->uPageSize),pNandImage->imageType);
                } else {
#if !defined(BATCH_BRUN)
                    Burn_NAND(pNandImage->fileLength + offset +((FW_NAND_IMAGE_T *)pNandImage)->initSize,pNandImage->blockNo/(pSM->uPagePerBlock*pSM->uPageSize),pNandImage->imageType);
                    Burn_NAND(pNandImage->fileLength + offset +((FW_NAND_IMAGE_T *)pNandImage)->initSize,pNandImage->blockNo/(pSM->uPagePerBlock*pSM->uPageSize),pNandImage->imageType);
#else
                    BatchBurn_NAND(pNandImage->fileLength + offset +((FW_NAND_IMAGE_T *)pNandImage)->initSize,pNandImage->blockNo/(pSM->uPagePerBlock*pSM->uPageSize),pNandImage->imageType);
#endif
                }
            } else {
                MSG_DEBUG("_ch[0]=%c,_ch[1]=%c,_ch[2]=%c,_ch[3]=%c\n",(char)_ch[0],(char)_ch[1],(char)_ch[2],(char)_ch[3]);
                if(((char)_ch[0])=='U' && ((char)_ch[1])=='B' && ((char)_ch[2])=='I') {
#if !defined(BATCH_BRUN)
                    Burn_NAND(pNandImage->fileLength + offset +((FW_NAND_IMAGE_T *)pNandImage)->initSize,pNandImage->blockNo/(pSM->uPagePerBlock*pSM->uPageSize),UBIFS);
#else
                    BatchBurn_NAND(pNandImage->fileLength + offset +((FW_NAND_IMAGE_T *)pNandImage)->initSize,pNandImage->blockNo/(pSM->uPagePerBlock*pSM->uPageSize),UBIFS);
#endif
                } else {
#if !defined(BATCH_BRUN)
                    Burn_NAND(pNandImage->fileLength + offset +((FW_NAND_IMAGE_T *)pNandImage)->initSize,pNandImage->blockNo/(pSM->uPagePerBlock*pSM->uPageSize),YAFFS2);
#else
                    BatchBurn_NAND(pNandImage->fileLength + offset +((FW_NAND_IMAGE_T *)pNandImage)->initSize,pNandImage->blockNo/(pSM->uPagePerBlock*pSM->uPageSize),YAFFS2);
#endif
                }
            }
        }
    }
    break;

    case MODIFY_MODE: { // modify
        MSG_DEBUG("NAND modify !!!\n");
    }
    break;

    case ERASE_MODE: {  // erase
        MSG_DEBUG("NAND erase !!!\n");
        /* for debug or delay */
        {
            usleep(1000);
            *_ack=0x83;
            usb_send((unsigned char*)_ack,4);//send ack to PC
        }

        MSG_DEBUG("\nstart=%d\n",pNandImage->blockNo);       //start block
        MSG_DEBUG("length=%d\n",pNandImage->executeAddr);  //block length
        MSG_DEBUG("type=%d\n",pNandImage->imageType);      //0: chip erase, 1: erase accord start and length blocks.
        MSG_DEBUG("imageNo=0x%08x\n",pNandImage->imageNo);

        if(pNandImage->imageType==0) { //chip erase
            if (pNandImage->imageNo == 0xFFFFFFFF) {
                int bb;
                bb = fmiSM_ChipErase(0);
                if (bb < 0) {
                    sysprintf("ERROR: %d bad block\n", bb); // storage error
                    *_ack=0xffff;
                    usb_send((unsigned char*)_ack,4);//send ack to PC
                } else
                    sysprintf("total %d bad block\n", bb);
            } else {
                fmiSM_ChipEraseBad(0);
            }
        } else {
            if (pNandImage->imageNo == 0xFFFFFFFF) {
                int bb;
                bb = fmiSM_Erase(0,pNandImage->blockNo,pNandImage->executeAddr);
                sysprintf("total %d bad block\n", bb);
            } else {
                MSG_DEBUG("fmiSM_EraseBad\n");
                fmiSM_EraseBad(0,pNandImage->blockNo,pNandImage->executeAddr);
            }
        }
    }
    break;

    case VERIFY_MODE: { // verify
        MSG_DEBUG("NAND normal verify !!!\n");
        /* for debug or delay */
        {
            usleep(1000);
            *_ack=0x83;
            usb_send((unsigned char*)_ack,4);//send ack to PC
        }
#if !defined(BATCH_BRUN)
        _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
        ptr=_ch;
        if (pNandImage->imageType == UBOOT) {   // system image
            memset(ptr, 0, pNandImage->fileLength+16+pNandImage->initSize);
            Read_Nand(DOWNLOAD_BASE,pNandImage->blockNo/(pSM->uPagePerBlock*pSM->uPageSize),pNandImage->fileLength+16+pNandImage->initSize);
            ptr+=(16+pNandImage->initSize);
        } else {
            memset(ptr, 0, pNandImage->fileLength);
            Read_Nand(DOWNLOAD_BASE,pNandImage->blockNo/(pSM->uPagePerBlock*pSM->uPageSize),pNandImage->fileLength);
        }
        do {
            usb_send(ptr,TRANSFER_LEN); //send data to PC
            usb_recv((unsigned char*)_ack,4); //recv data from PC
            ptr += (*_ack);
        } while((ptr-_ch)<pNandImage->fileLength);
#else
        {
            if (pNandImage->imageType == UBOOT) {   // system image
                int offblk=0;
                _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
                ptr=_ch;
                while(fmiCheckInvalidBlock(pSM, pNandImage->blockNo/(pSM->uPagePerBlock*pSM->uPageSize)+offblk) == 1) {
                    offblk++;
                }
                Read_Nand(DOWNLOAD_BASE,pNandImage->blockNo/(pSM->uPagePerBlock*pSM->uPageSize)+offblk,pNandImage->fileLength+16+pNandImage->initSize);
                memmove(_ch,_ch+16+pNandImage->initSize,pNandImage->fileLength);
                do {
                    usb_send(ptr,TRANSFER_LEN); //send data to PC
                    while(Bulk_Out_Transfer_Size==0) {}
                    usb_recv((unsigned char*)_ack,4); //recv data from PC
                    ptr += (*_ack);
                } while((ptr-_ch)<(pNandImage->fileLength));
            } else {
                int total,offblk=0;
                total=pNandImage->fileLength;
                do {
                    _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
                    ptr=_ch;
                    len=MIN(total,pSM->uPagePerBlock*pSM->uPageSize);
                    memset(ptr, 0xff, pSM->uPagePerBlock*pSM->uPageSize);
                    while(fmiCheckInvalidBlock(pSM, pNandImage->blockNo/(pSM->uPagePerBlock*pSM->uPageSize)+offblk) == 1) {
                        offblk++;
                    }
                    Read_Nand(DOWNLOAD_BASE,pNandImage->blockNo/(pSM->uPagePerBlock*pSM->uPageSize)+offblk,len);
                    MSG_DEBUG("Read_NAND offblk=%d,len=%d\n",offblk,len);
                    do {
                        usb_send(ptr,TRANSFER_LEN); //send data to PC
                        while(Bulk_Out_Transfer_Size==0) {}
                        usb_recv((unsigned char*)_ack,4); //recv data from PC
                        ptr += (*_ack);
                        MSG_DEBUG("read size=0x%08x\n",(unsigned int)(ptr-_ch));
                    } while((int)(ptr-_ch)<len);
                    total-=len;
                    offblk+=1;
                    MSG_DEBUG("total=%d len=%d\n",total,len);
                } while(total!=0);
            }
        }
#endif
    }
    break;
    case PACK_VERIFY_MODE: { // verify
        MSG_DEBUG("\n NAND PACK verify !!!\n");
        /* for debug or delay */
        {
            usleep(1000);
            *_ack=0x83;
            usb_send((unsigned char*)_ack,4);//send ack to PC
        }

#if !defined(BATCH_BRUN)

#else
        //sysprintf("PACK_VERIFY_MODE  PACK_VERIFY_MODE  PACK_VERIFY_MODE pNandImage->imageNo= %d\n", pNandImage->imageNo);
        for(i=0; i<pNandImage->imageNo; i++) {
            while(1) {
                if(Bulk_Out_Transfer_Size>=sizeof(PACK_CHILD_HEAD)) {
                    usb_recv(ptr,sizeof(PACK_CHILD_HEAD));
                    memcpy(&ppack,(unsigned char *)ptr,sizeof(PACK_CHILD_HEAD));
                    usleep(1000);
                    *_ack=0x83;
                    usb_send((unsigned char*)_ack,4);//send ack to PC
                    break;
                }
            }

            if (ppack.imagetype == UBOOT) {   // system image
                int offblk=0;
                int rawfilelen = 0;
                _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
                ptr=_ch;
                while(fmiCheckInvalidBlock(pSM, pNandImage->blockNo/(pSM->uPagePerBlock*pSM->uPageSize)+offblk) == 1) {
                    offblk++;
                }

                // Get DDR parameter length
                usb_recv(ptr,4);
                memcpy(&ddrlen,(unsigned char *)ptr,4);
                //sysprintf("ddrlen = %d\n", ddrlen);
                //usleep(1000);
                *_ack=0x83;
                usb_send((unsigned char*)_ack,4);//send ack to PC

                Read_Nand(DOWNLOAD_BASE,offblk,ppack.filelen+16+ppack.startaddr);
                MSG_DEBUG("offblk = %d, ptr[0~3] = 0x%x  0x%x  0x%x  0x%x\n", offblk, ptr[0], ptr[1], ptr[2], ptr[3]);
                //sysprintf("ptr[4096~4099] = 0x%x  0x%x  0x%x  0x%x\n", ptr[4096], ptr[4097], ptr[4098], ptr[4099]);
                memmove(_ch,_ch+16+ddrlen+ppack.startaddr,ppack.filelen);
                rawfilelen = ppack.filelen - (ddrlen+16+ppack.startaddr);
                MSG_DEBUG("ppack.filelen = (%d) 0x%x,  rawfilelen = (%d)0x%x\n", ppack.filelen, ppack.filelen, rawfilelen, rawfilelen);
                do {
                    usb_send(ptr,TRANSFER_LEN); //send data to PC
                    usb_recv((unsigned char*)_ack,4); //recv data from PC
                    ptr += (*_ack);
                } while((ptr-_ch)<(rawfilelen));

            } else {
                int total,offblk=0;
                total=ppack.filelen;
                pNandImage->blockNo = ppack.startaddr;
                do {
                    _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
                    ptr=_ch;
                    //sysprintf("total = %d, ptr[0~3] = 0x%x  0x%x  0x%x  0x%x\n", total, ptr[0], ptr[1], ptr[2], ptr[3]);
                    len=MIN(total,pSM->uPagePerBlock*pSM->uPageSize);
                    memset(ptr, 0xff, pSM->uPagePerBlock*pSM->uPageSize);
                    while(fmiCheckInvalidBlock(pSM, pNandImage->blockNo/(pSM->uPagePerBlock*pSM->uPageSize)+offblk) == 1) {
                        offblk++;
                    }
                    Read_Nand(DOWNLOAD_BASE,pNandImage->blockNo/(pSM->uPagePerBlock*pSM->uPageSize)+offblk,len);
                    //sysprintf("len=(%d)  pNandImage->blockNo=(%d)   offblk=(%d)  pSM->uPagePerBlock=(%d)\n", len, pNandImage->blockNo, offblk, pSM->uPagePerBlock*pSM->uPageSize);
                    MSG_DEBUG("Read_NAND offblk=%d,len=%d\n",offblk,len);
                    do {
                        usb_send(ptr,TRANSFER_LEN); //send data to PC
                        usb_recv((unsigned char*)_ack,4); //recv data from PC
                        ptr += (*_ack);
                        MSG_DEBUG("read size=0x%08x\n",(unsigned int)(ptr-_ch));
                    } while((int)(ptr-_ch)<len);
                    total-=len;
                    offblk+=1;
                    MSG_DEBUG("total=%d len=%d\n",total,len);
                } while(total!=0);
            }
        }
#endif
    }
    break;
    case READ_MODE: { // read
        MSG_DEBUG("NAND normal read !!!\n");
        /* for debug or delay */
        {
            usleep(1000);
            *_ack=0x83;
            usb_send((unsigned char*)_ack,4);//send ack to PC
        }

        MSG_DEBUG("offset=%d,blockNo=%d,fileLength=%d\n",offset,pNandImage->blockNo,pNandImage->fileLength);
#if defined(BATCH_BRUN)
        {
            unsigned int total,offblk=0;
            total=pNandImage->fileLength;
            do {
                _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
                ptr=_ch;
                if(pNandImage->initSize==0) { //0: read good block , others : read redunancy area,good block and bad block
                    len=MIN(total,pSM->uPagePerBlock*pSM->uPageSize);
                    memset(ptr, 0, len);
                    while(fmiCheckInvalidBlock(pSM, pNandImage->blockNo+offblk) == 1) {
                        offblk++;
                    }
                    Read_Nand(DOWNLOAD_BASE,pNandImage->blockNo+offblk,len);
                    do {
                        usb_send(ptr,4096); //send data to PC
                        while(Bulk_Out_Transfer_Size==0) {}
                        usb_recv((unsigned char*)_ack,4); //recv data from PC
                        ptr += (*_ack);
                    } while((ptr-_ch)<len);
                    total-=len;
                    offblk+=1;
                } else {
                    len=MIN(total,pSM->uPagePerBlock*(pSM->uPageSize+pSM->uSpareSize));
                    memset(ptr, 0, len);
                    Read_Nand_Redunancy(DOWNLOAD_BASE,pNandImage->blockNo+offblk,len);
                    do {
                        usb_send(ptr,pSM->uPageSize); //send data to PC
                        while(Bulk_Out_Transfer_Size==0) {}
                        usb_recv((unsigned char*)_ack,4); //recv data from PC
                        ptr += (*_ack);
                        usb_send(ptr,pSM->uSpareSize); //send redunancy data to PC
                        while(Bulk_Out_Transfer_Size==0) {}
                        usb_recv((unsigned char*)_ack,4); //recv redunancy data from PC
                        ptr += (*_ack);
                    } while((ptr-_ch)<len);
                    total-=(len);
                    offblk+=1;
                }
            } while(total!=0);
        }
#else
        _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
        ptr=_ch;
        memset(ptr, 0, pNandImage->fileLength);
        Read_Nand(DOWNLOAD_BASE,pNandImage->blockNo,pNandImage->fileLength);
        do {
            usb_send(ptr,4096);//send data to PC
            while(Bulk_Out_Transfer_Size==0) {}
            usb_recv((unsigned char*)_ack,4); //recv data from PC
            ptr += (*_ack);
        } while((ptr-_ch)<pNandImage->fileLength);
#endif
    }
    break;
    case PACK_MODE: {
        MSG_DEBUG("NAND pack mode !!!\n");
        /* for debug or delay */
        {
            usleep(1000);
            *_ack=0x83;
            usb_send((unsigned char*)_ack,4);//send ack to PC
        }

        _ch=((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
        ptr=_ch;
        {
            PACK_HEAD pack;
            PACK_CHILD_HEAD ppack;
            while(1) {
                if(Bulk_Out_Transfer_Size>=sizeof(PACK_HEAD)) {
                    usb_recv(ptr,sizeof(PACK_HEAD));
                    memcpy(&pack,(unsigned char *)ptr,sizeof(PACK_HEAD));
                    *_ack=sizeof(PACK_HEAD);
                    usb_send((unsigned char*)_ack,4);//send ack to PC
                    break;
                }
            }
            MSG_DEBUG("pack.actionFlag=0x%x, pack.fileLength=0x%08x pack.num=%d!!!\n",pack.actionFlag,pack.fileLength,pack.num);
            for(i=0; i<pack.num; i++) {
                while(1) {
                    if(Bulk_Out_Transfer_Size>=sizeof(PACK_CHILD_HEAD)) {
                        usb_recv(ptr,sizeof(PACK_CHILD_HEAD));
                        memcpy(&ppack,(unsigned char *)ptr,sizeof(PACK_CHILD_HEAD));
                        *_ack=sizeof(PACK_CHILD_HEAD);
                        usb_send((unsigned char*)_ack,4);//send ack to PC
                        break;
                    }
                }
                MSG_DEBUG("%d ppack.filelen=0x%x, ppack.startaddr=0x%08x!!!\n",i,ppack.filelen,ppack.startaddr);
#if !defined(BATCH_BRUN)
                ptr=_ch;
                do {
                    if(Bulk_Out_Transfer_Size>0)
                        //if(Bulk_Out_Transfer_Size>=MIN(4096,ppack.filelen))
                    {
                        len=Bulk_Out_Transfer_Size;
                        usb_recv(ptr,len);  //recv data from PC
                        ptr+=len;
                        *_ack=len;
                        usb_send((unsigned char*)_ack,4);//send ack to PC
                    }
                } while((ptr-_ch)<ppack.filelen);
                MSG_DEBUG("ppack OK\n");
#endif
                if(ppack.imagetype!=PMTP) {
                    if(ppack.imagetype==UBOOT) {
#if !defined(BATCH_BRUN)
                        int j;
                        for(j=1; j<4; j++)
                            Burn_NAND_BACKUP(ppack.filelen,i);
                        Burn_NAND(ppack.filelen,ppack.startaddr/(pSM->uPageSize*pSM->uPagePerBlock),pNandImage->imageType);
#else
                        ret = BatchBurn_NAND_BOOT(ppack.filelen,0,4,0);
                        if(ret == -1) {
                            sysprintf("Nand Device image error !!! \n");
                            return;
                        }
#endif
                    } else {
                        if(ppack.imagetype!=IMAGE) {
#if !defined(BATCH_BRUN)
                            Burn_NAND(ppack.filelen,ppack.startaddr/(pSM->uPageSize*pSM->uPagePerBlock),pNandImage->imageType);
#else
                            BatchBurn_NAND(ppack.filelen,ppack.startaddr/(pSM->uPageSize*pSM->uPagePerBlock),pNandImage->imageType);
#endif
                        } else {
                            MSG_DEBUG("_ch[0]=%c,_ch[1]=%c,_ch[2]=%c,_ch[3]=%c\n",(char)_ch[0],(char)_ch[1],(char)_ch[2],(char)_ch[3]);
                            if(((char)_ch[0])=='U' && ((char)_ch[1])=='B' && ((char)_ch[2])=='I') {
#if !defined(BATCH_BRUN)
                                Burn_NAND(ppack.filelen,ppack.startaddr/(pSM->uPageSize*pSM->uPagePerBlock),UBIFS);
#else
                                ret = BatchBurn_NAND(ppack.filelen,ppack.startaddr/(pSM->uPageSize*pSM->uPagePerBlock),UBIFS);
                                if(ret == -1) {
                                    sysprintf("BatchBurn_NAND Device image error !!! \n");
                                    return;
                                }
#endif
                            } else {
#if !defined(BATCH_BRUN)
                                Burn_NAND(ppack.filelen,ppack.startaddr/(pSM->uPageSize*pSM->uPagePerBlock),YAFFS2);
#else
                                BatchBurn_NAND(ppack.filelen,ppack.startaddr/(pSM->uPageSize*pSM->uPagePerBlock),YAFFS2);
#endif
                            }
                        }
                    }
                } else { /* if(ppack.imagetype==PMTP) */
                    UINT32 m_mtp_mode,m_mtp_option,m_mtp_encrypt,m_mtp_enable,j;
                    FW_OTP_IMAGE_T MTPImage;
                    m_mtp_mode=PACK_Mode(ppack.startaddr);
                    m_mtp_option=PACK_Option(ppack.startaddr);
                    m_mtp_encrypt=PACK_Encrypt(ppack.startaddr);
                    m_mtp_enable=PACK_Enable(ppack.startaddr);
                    ptr=_ch;
                    do {
                        if(Bulk_Out_Transfer_Size>0) {
                            len=Bulk_Out_Transfer_Size;
                            usb_recv(ptr,len);  //recv data from PC
                            ptr+=len;
                            *_ack=len;
                            usb_send((unsigned char*)_ack,4);//send ack to PC
                        }
                    } while((ptr-_ch)<ppack.filelen);
                    MTPImage.Mode = m_mtp_mode;
                    MTPImage.Option=(1<<m_mtp_option) | ((1-m_mtp_encrypt)<<2);
                    MTPImage.KeyLen = ppack.filelen;
                    MTPImage.Lock = m_mtp_enable;
                    memcpy(&MTPImage.KeyOTP[0],_ch,MTPImage.KeyLen);
                    MSG_DEBUG("Mode %d, Option 0x%x, Lock %d\n", m_mtp_mode, m_mtp_option, m_mtp_enable);
                    MSG_DEBUG("KeyMTP Len = 0x%08x \n", MTPImage.KeyLen);
                    for(j=0; j<8; j++)
                        MSG_DEBUG("KeyMTP[%d] = 0x%08x \n",j, MTPImage.KeyOTP[j]);
                    if(!MTP_IsLock()) {
                        MTP_Program_Key((UINT8 *)(&MTPImage.KeyOTP[0]),MTPImage.KeyLen,MTPImage.Option,MTPImage.Mode);
                        if(MTPImage.Lock==1)
                            MTP_Lock();
                    }
                    SendAck(100);
                }
            }
        }
    }
    break;
    default:
        ;
        break;
    }
    MSG_DEBUG("finish NAND image download\n");
}

void UXmodem_MTP()
{
    int i;
    unsigned char *ptr;
    unsigned char buf[512];
    unsigned char *_ch;
    unsigned int *_ack;
    FW_OTP_IMAGE_T OTPImage;
    FW_OTP_IMAGE_T * pOTPImage;
    pOTPImage = &OTPImage;

    MSG_DEBUG("Download MTP KEY to Regsters...\n");

    _ch=((unsigned char*)(((unsigned int)buf)|NON_CACHE));
    _ack=((unsigned int*)(((unsigned int)buf)|NON_CACHE));
    ptr=_ch;

    while(1) {
        if(Bulk_Out_Transfer_Size>0) {
            usb_recv(ptr,sizeof(FW_OTP_IMAGE_T));
            memcpy(pOTPImage,(unsigned char *)ptr,sizeof(FW_OTP_IMAGE_T));
            break;
        }
    }

    MSG_DEBUG("Mode %d, Option 0x%x, Lock %d\n", pOTPImage->Mode, pOTPImage->Option, pOTPImage->Lock);
    MSG_DEBUG("KeyLen %d \n", pOTPImage->KeyLen);
    for(i=0; i<8; i++)
        MSG_DEBUG("KeyMTP[%d] = 0x%08x \n",i, pOTPImage->KeyOTP[i]);

    if(MTP_IsLock()) {
        *_ack=MTP_LOCK_KEY;
        usb_send((unsigned char*)_ack,4);//send ack to PC
        return;
    }


    MTP_Program_Key((UINT8 *)(&pOTPImage->KeyOTP[0]),pOTPImage->KeyLen,pOTPImage->Option,pOTPImage->Mode);

    MSG_DEBUG("\nfinish MTP key download ...\n");


    if(pOTPImage->Lock==1)
        *_ack = MTP_Lock();
    else
        *_ack = MTP_KEY_SUCCESS;

    usb_send((unsigned char*)_ack,4);//send ack to PC
}

typedef struct _info {
    UINT32  Nand_uPagePerBlock;
    UINT32  Nand_uPageSize;
    UINT32  Nand_uSectorPerBlock;
    UINT32  Nand_uBlockPerFlash;
    UINT32  Nand_uBadBlockCount;
    UINT32  Nand_uSpareSize;
    UINT32  Nand_uIsUserConfig;
    UINT32  SPI_ID;
    UINT32  EMMC_uBlock;
    UINT32  EMMC_uReserved;
    UINT32  MTP_uNumber;
} INFO_T;


void UXmodem_INFO()
{
    //int i;
    INFO_T info;
    UINT8 *ptr=(UINT8 *)&info;
    memset((char *)&info,0x0,sizeof(INFO_T));

    MSG_DEBUG("Receive INFO flash Image ...\n");
    while(1) {
        if(Bulk_Out_Transfer_Size>=sizeof(INFO_T)) {
            usb_recv(ptr,sizeof(INFO_T));
            break;
        }
    }

    if(info.Nand_uIsUserConfig == 1)
        g_uIsUserConfig = 1;
    else
        g_uIsUserConfig = 0;

    Custom_uBlockPerFlash = info.Nand_uBlockPerFlash;
    Custom_uPagePerBlock = info.Nand_uPagePerBlock;
    MSG_DEBUG("g_uIsUserConfig %d -> Nand_uBlockPerFlash=%d, Nand_uPagePerBlock=%d\n", g_uIsUserConfig, Custom_uBlockPerFlash, Custom_uPagePerBlock);
    MSG_DEBUG("Get INFO flash Image ...\n");

    if (usiInit()) {
        info.SPI_ID=usiReadID();
    }
    if(!fmiNandInit()) {
        info.Nand_uBlockPerFlash=pSM->uBlockPerFlash;
        info.Nand_uPagePerBlock=pSM->uPagePerBlock;
        info.Nand_uPageSize=pSM->uPageSize;
        info.Nand_uBadBlockCount=pSM->uBadBlockCount;
        info.Nand_uSpareSize=pSM->uSpareSize;
#ifndef MSG_DEBUG_EN
        sysDelay(5);
#else
        sysprintf("BlockPerFlash=%d, PagePerBlock=%d, PageSize=%d\n",info.Nand_uBlockPerFlash, info.Nand_uPagePerBlock, info.Nand_uPageSize);
#endif
    }

    if(eMMCBlockSize>0) {
        fmiInitSDDevice();
        info.EMMC_uReserved=GetMMCReserveSpace();
        MSG_DEBUG("EMMC_uReserved =%d ...\n",info.EMMC_uReserved);
        info.EMMC_uBlock=eMMCBlockSize;
    }

    {
        outpw(REG_PCLKEN1, inpw(REG_PCLKEN1) | (1 << 26));
        MTP_Enable();
        info.MTP_uNumber = (inpw(REG_OTP_STATUS) & 0xf<<16)>>16;
    }

    usb_send((UINT8 *)&info, sizeof(INFO_T));
    MSG_DEBUG("finish get INFO!!\n");
}
INT ParseFlashType()
{
    switch (_usbd_flash_type) {
    case USBD_FLASH_SDRAM: {
        UXmodem_SDRAM();
        _usbd_flash_type = -1;
    }
    break;
    case USBD_FLASH_MMC: {
        UXmodem_MMC();
        _usbd_flash_type = -1;
    }
    break;
    case USBD_FLASH_NAND: {
        UXmodem_NAND();
        _usbd_flash_type = -1;
    }
    break;
    case USBD_FLASH_SPI: {
        UXmodem_SPI();
        _usbd_flash_type = -1;
    }
    break;
    case USBD_MTP: {
        UXmodem_MTP();
        _usbd_flash_type = -1;
    }
    break;
    case USBD_INFO: {
        UXmodem_INFO();
        _usbd_flash_type = -1;
    }
#if 0
    case USBD_FLASH_NOR_RAW: {
        //UXmodem_NOR_RAW();
        _usbd_flash_type = -1;
    }
    break;

    case USBD_FLASH_NOR: {
        //UXmodem_NOR();
        _usbd_flash_type = -1;
    }
    break;
    case USBD_FLASH_MMC_RAW: {
        //UXmodem_MMC_RAW();
        _usbd_flash_type = -1;
    }
    break;
    case USBD_FLASH_NAND_RAW: {
        //UXmodem_NAND_RAW();
        _usbd_flash_type = -1;
    }
    break;
    case USBD_FLASH_SPI_RAW: {
        //UXmodem_SPI_RAW();
        _usbd_flash_type = -1;
    }
    break;
    case USBD_NOR_IMAGE_LIST: {
        //GetNorImage();
        _usbd_flash_type = -1;
    }
    break;

    case USBD_MMC_IMAGE_LIST: {
        //GetMMCImage();
        _usbd_flash_type = -1;
    }
    break;

    case USBD_NAND_IMAGE_LIST: {
        _usbd_flash_type = -1;
        //GetNandImage();
    }
    break;
    case USBD_SPI_IMAGE_LIST: {
        //GetSPIImage();
        _usbd_flash_type = -1;
    }
    break;
    case USBD_NOR_TYPE: {
        //GetNorType();
        _usbd_flash_type = -1;
    }
    break;
#endif
#if 1
    case -1 : { // load xusb.bin again
        if(Bulk_Out_Transfer_Size>0) {
            unsigned char *ptr;
            unsigned int *_ack=NULL,len;
            ptr =((unsigned char*)(((unsigned int)DOWNLOAD_BASE)|NON_CACHE));
            len=Bulk_Out_Transfer_Size;
            usb_recv(ptr,len);
            do {
                len=Bulk_Out_Transfer_Size;
                if(len>0) {
                    len=Bulk_Out_Transfer_Size;
                    usb_recv(ptr,len);
                    *_ack=4097;
                    usb_send((unsigned char*)_ack,4);//send ack to PC
                    break;
                }
            } while(1);
        }
    }
    break;
#endif
    default:
        break;
    }

    return 0;
}
