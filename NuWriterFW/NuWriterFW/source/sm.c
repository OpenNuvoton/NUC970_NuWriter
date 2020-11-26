#include <string.h>
#include <stdlib.h>
#include "NUC970_reg.h"
#include "usbd.h"
#include "fmi.h"
#include "wblib.h"
#include "wbfmi.h"
#include "sd.h"

/*-----------------------------------------------------------------------------
 * Define some constants for BCH
 *---------------------------------------------------------------------------*/
// define the total padding bytes for 512/1024 data segment
#define BCH_PADDING_LEN_512     32
#define BCH_PADDING_LEN_1024    64
// define the BCH parity code lenght for 512 bytes data pattern
#define BCH_PARITY_LEN_T4  8
#define BCH_PARITY_LEN_T8  15
#define BCH_PARITY_LEN_T12 23
#define BCH_PARITY_LEN_T15 29
// define the BCH parity code lenght for 1024 bytes data pattern
#define BCH_PARITY_LEN_T24 45

#define ERASE_WITH_0XF0

#define NAND_EXTRA_512          16
#define NAND_EXTRA_2K           64
#define NAND_EXTRA_4K           128
#define NAND_EXTRA_8K           376
/*-----------------------------------------------------------------------------*/

// global variables
UCHAR _fmi_ucBaseAddr1=0, _fmi_ucBaseAddr2=0;
FMI_SM_INFO_T SMInfo, *pSM;
FW_UPDATE_INFO_T FWInfo;
extern int volatile _usbd_IntraROM;
extern void SendAck(UINT32 status);
extern UINT32 g_uIsUserConfig;

INT fmiSMCheckRB()
{
    int volatile tick;

    tick = sysGetTicks(TIMER0);
    while(1) {
        if (inpw(REG_SMISR) & 0x400) {
            while(! (inpw(REG_SMISR) & 0x40000) );
            outpw(REG_SMISR, 0x400);
            return 1;
        }
        if ((sysGetTicks(TIMER0) - tick) > 2)
            return 0;
    }
}

// SM functions
INT fmiSM_Reset(void)
{
    UINT32 volatile i;
    outpw(REG_SMCMD, 0xff);
    for (i=100; i>0; i--);
    if (!fmiSMCheckRB())
        return Fail;
    return Successful;
}


VOID fmiSM_Initial(FMI_SM_INFO_T *pSM)
{
    outpw(REG_SMCSR,  inpw(REG_SMCSR) | 0x800080);  // enable ECC

    //--- Set register to disable Mask ECC feature
    outpw(REG_SMREACTL, inpw(REG_SMREACTL) & ~0xffff0000);

    //--- Set registers that depend on page size. According to FA95 sepc, the correct order is
    //--- 1. SMCR_BCH_TSEL  : to support T24, MUST set SMCR_BCH_TSEL before SMCR_PSIZE.
    //--- 2. SMCR_PSIZE     : set SMCR_PSIZE will auto change SMRE_REA128_EXT to default value.
    //--- 3. SMRE_REA128_EXT: to use non-default value, MUST set SMRE_REA128_EXT after SMCR_PSIZE.
    outpw(REG_SMCSR, (inpw(REG_SMCSR) & ~0x7c0000) | pSM->uNandECC);
    if (pSM->uPageSize == 8192)
        outpw(REG_SMCSR, (inpw(REG_SMCSR)&(~0x30000)) | 0x30000);
    else if (pSM->uPageSize == 4096)
        outpw(REG_SMCSR, (inpw(REG_SMCSR)&(~0x30000)) | 0x20000);
    else if (pSM->uPageSize == 2048)
        outpw(REG_SMCSR, (inpw(REG_SMCSR)&(~0x30000)) | 0x10000);
    else    // Page size should be 512 bytes
        outpw(REG_SMCSR, (inpw(REG_SMCSR)&(~0x30000)) | 0x00000);
    outpw(REG_SMREACTL, (inpw(REG_SMREACTL) & ~0x1ff) | pSM->uSpareSize);

    //TODO: if need protect --- config register for Region Protect
    outpw(REG_SMCSR, inpw(REG_SMCSR) & ~0x20);   // disable Region Protect

    // Disable Write Protect
    outpw(REG_NFECR, 0x01);
}

UINT32 Custom_uBlockPerFlash;
UINT32 Custom_uPagePerBlock;
INT fmiSM_ReadID(FMI_SM_INFO_T *pSM)
{
    UINT32 tempID[5],u32PowerOn,IsID=0;
    UINT8 name[6][16] = {"T24","T4","T8","T12","T15","XXX"};
    UINT8 BCHAlgoIdx;

    if (pSM->bIsInResetState == FALSE) {
        if (fmiSM_Reset() < 0)
            return Fail;
        pSM->bIsInResetState = TRUE;
    }
    pSM->bIsInResetState = FALSE;
    outpw(REG_SMCMD, 0x90);     // read ID command
    outpw(REG_SMADDR, 0x80000000);  // address 0x00

    tempID[0] = inpw(REG_SMDATA);
    tempID[1] = inpw(REG_SMDATA);
    tempID[2] = inpw(REG_SMDATA);
    tempID[3] = inpw(REG_SMDATA);
    tempID[4] = inpw(REG_SMDATA);

    sysprintf("SM ID [%x][%x][%x][%x]\n", tempID[0], tempID[1], tempID[2], tempID[3]);
    MSG_DEBUG("ID[4]=0x%2x\n",tempID[4]);

    /* Without Power-On-Setting for NAND */
    pSM->uPagePerBlock = 32;
    pSM->uPageSize = 512;
    pSM->uNandECC = BCH_T4;
    pSM->bIsMulticycle = TRUE;
    pSM->uSpareSize = 8;
    pSM->uBlockPerFlash  = Custom_uBlockPerFlash-1; // block index with 0-base. = physical blocks - 1
    pSM->uPagePerBlock   = Custom_uPagePerBlock;
    MSG_DEBUG("Default[0x%x 0x%x] -> BlockPerFlash=%d, PagePerBlock=%d, PageSize=%d\n", tempID[1], tempID[3], pSM->uBlockPerFlash, pSM->uPagePerBlock, pSM->uPageSize);

    switch (tempID[1]) {
        /* page size 512B */
    case 0x79:  // 128M v
        pSM->uBlockPerFlash = 8191;
        pSM->uPagePerBlock = 32;
        pSM->uSectorPerBlock = 32;
        pSM->uPageSize = 512;
        pSM->uNandECC = BCH_T4;
        pSM->bIsMulticycle = TRUE;
        pSM->uSpareSize = 16;
        break;

    case 0x76:  // 64M v
        pSM->uBlockPerFlash = 4095;
        pSM->uPagePerBlock = 32;
        pSM->uSectorPerBlock = 32;
        pSM->uPageSize = 512;
        pSM->uNandECC = BCH_T4;
        pSM->bIsMulticycle = TRUE;
        pSM->uSpareSize = 16;
        break;

    case 0x75:  // 32M v
        pSM->uBlockPerFlash = 2047;
        pSM->uPagePerBlock = 32;
        pSM->uSectorPerBlock = 32;
        pSM->uPageSize = 512;
        pSM->uNandECC = BCH_T4;
        pSM->bIsMulticycle = FALSE;
        pSM->uSpareSize = 16;
        break;

    case 0x73:  // 16M v
        pSM->uBlockPerFlash = 1023;
        pSM->uPagePerBlock = 32;
        pSM->uSectorPerBlock = 32;
        pSM->uPageSize = 512;
        pSM->uNandECC = BCH_T4;
        pSM->bIsMulticycle = FALSE;
        pSM->uSpareSize = 16;
        break;

        /* page size 2KB */
    case 0xf1:  // 128M v
    case 0xd1:  // 128M v
        pSM->uBlockPerFlash = 1023;
        pSM->uPagePerBlock = 64;
        pSM->uSectorPerBlock = 256;
        pSM->uPageSize = 2048;
        pSM->uNandECC = BCH_T4;
        pSM->bIsMulticycle = FALSE;
        pSM->uSpareSize = 64;
        break;

    case 0xda:  // 256M v
        if ((tempID[3] & 0x33) == 0x11) {
            pSM->uBlockPerFlash = 2047;
            pSM->uPagePerBlock = 64;
            pSM->uSectorPerBlock = 256;
        } else if ((tempID[3] & 0x33) == 0x21) {
            pSM->uBlockPerFlash = 1023;
            pSM->uPagePerBlock = 128;
            pSM->uSectorPerBlock = 512;
        } else { // Unrecognized ID[3]
            pSM->uBlockPerFlash = 2047;
            pSM->uPagePerBlock = 64;
            pSM->uSectorPerBlock = 256;
        }
        pSM->uPageSize = 2048;
        pSM->uNandECC = BCH_T4;
        pSM->bIsMulticycle = TRUE;
        pSM->uSpareSize = 64;
        break;

    case 0xdc:  // 512M v
        pSM->uBlockPerFlash = 64;
        if((tempID[0]==0x98) && (tempID[1]==0xDC) &&(tempID[2]==0x90)&&(tempID[3]==0x26)&&(tempID[4]==0x76)) {
            pSM->uBlockPerFlash = 2047;
            pSM->uPagePerBlock = 64;
            pSM->uSectorPerBlock = 256;
            pSM->uPageSize = 4096;
            pSM->uNandECC = BCH_T12;
            pSM->bIsMLCNand = TRUE;
            pSM->uSpareSize = 192;
            pSM->bIsMulticycle = TRUE;
            break;
        } else if ((tempID[3] & 0x33) == 0x11) {
            pSM->uBlockPerFlash = 4095;
            pSM->uPagePerBlock = 64;
            pSM->uSectorPerBlock = 256;
        } else if ((tempID[3] & 0x33) == 0x21) {
            pSM->uBlockPerFlash = 2047;
            pSM->uPagePerBlock = 128;
            pSM->uSectorPerBlock = 512;
        }
        pSM->uPageSize = 2048;
        pSM->uNandECC = BCH_T4;
        pSM->bIsMulticycle = TRUE;
        pSM->uSpareSize = 64;
        break;

    case 0xd3:  // 1024M v
        if ((tempID[3] & 0x33) == 0x32) {
            pSM->uBlockPerFlash = 2047;
            pSM->uPagePerBlock = 128;
            pSM->uSectorPerBlock = 1024;    /* 128x8 */
            pSM->uPageSize = 4096;
            pSM->uNandECC = BCH_T8;
            pSM->uSpareSize = 128;
        } else if ((tempID[3] & 0x33) == 0x11) {
            pSM->uBlockPerFlash = 8191;
            pSM->uPagePerBlock = 64;
            pSM->uSectorPerBlock = 256;
            pSM->uPageSize = 2048;
            pSM->uNandECC = BCH_T4;
            pSM->uSpareSize = 64;
        } else if ((tempID[3] & 0x33) == 0x21) {
            pSM->uBlockPerFlash = 4095;
            pSM->uPagePerBlock = 128;
            pSM->uSectorPerBlock = 512;
            pSM->uPageSize = 2048;
            pSM->uNandECC = BCH_T4;
            pSM->uSpareSize = 64;
        } else if ((tempID[3] & 0x3) == 0x3) {
            pSM->uBlockPerFlash = 4095;//?
            pSM->uPagePerBlock = 128;
            pSM->uSectorPerBlock = 512;//?
            pSM->uPageSize = 8192;
            pSM->uNandECC = BCH_T12;
            pSM->uSpareSize = 368;
        } else if ((tempID[3] & 0x33) == 0x22) {
            pSM->uBlockPerFlash = 4095;
            pSM->uPagePerBlock = 64;
            pSM->uSectorPerBlock = 512; /* 64x8 */
            pSM->uPageSize = 4096;
            pSM->uNandECC = BCH_T8;
            pSM->uSpareSize = 128;
        }
        pSM->bIsMulticycle = TRUE;
        break;

    case 0xd5:  // 2048M v
        // H27UAG8T2A
        if ((tempID[0]==0xAD)&&(tempID[2] == 0x94)&&(tempID[3] == 0x25)) {
            pSM->uBlockPerFlash = 4095;
            pSM->uPagePerBlock = 128;
            pSM->uSectorPerBlock = 1024;    /* 128x8 */
            pSM->uPageSize = 4096;
            pSM->uNandECC = BCH_T12;
            pSM->bIsMulticycle = TRUE;
            pSM->uSpareSize = 224;
            break;
        }
        // 2011/7/28, To support Hynix H27UAG8T2B NAND flash
        else if ((tempID[0]==0xAD)&&(tempID[2]==0x94)&&(tempID[3]==0x9A)) {
            pSM->uBlockPerFlash = 1023;        // block index with 0-base. = physical blocks - 1
            pSM->uPagePerBlock = 256;
            pSM->uPageSize = 8192;
            pSM->uSectorPerBlock = pSM->uPageSize / 512 * pSM->uPagePerBlock;
            pSM->uNandECC = BCH_T24;
            pSM->bIsMulticycle = TRUE;
            pSM->uSpareSize = 448;
            break;
        }
        // 2011/7/28, To support Toshiba TC58NVG4D2FTA00 NAND flash
        else if ((tempID[0]==0x98)&&(tempID[2]==0x94)&&(tempID[3]==0x32)) {
            pSM->uBlockPerFlash = 2075;        // block index with 0-base. = physical blocks - 1
            pSM->uPagePerBlock = 128;
            pSM->uPageSize = 8192;
            pSM->uSectorPerBlock = pSM->uPageSize / 512 * pSM->uPagePerBlock;
            pSM->uNandECC = BCH_T24;
            pSM->bIsMulticycle = TRUE;
            pSM->uSpareSize = 376;
            break;
        } else if ((tempID[3] & 0x33) == 0x32) {
            pSM->uBlockPerFlash = 4095;
            pSM->uPagePerBlock = 128;
            pSM->uSectorPerBlock = 1024;    /* 128x8 */
            pSM->uPageSize = 4096;
            pSM->uNandECC = BCH_T8;
            pSM->bIsMLCNand = TRUE;
            pSM->uSpareSize = 128;
        } else if ((tempID[3] & 0x33) == 0x11) {
            pSM->uBlockPerFlash = 16383;
            pSM->uPagePerBlock = 64;
            pSM->uSectorPerBlock = 256;
            pSM->uPageSize = 2048;
            pSM->uNandECC = BCH_T4;
            pSM->bIsMLCNand = FALSE;
            pSM->uSpareSize = 64;
        } else if ((tempID[3] & 0x33) == 0x21) {
            pSM->uBlockPerFlash = 8191;
            pSM->uPagePerBlock = 128;
            pSM->uSectorPerBlock = 512;
            pSM->uPageSize = 2048;
            pSM->uNandECC = BCH_T4;
            pSM->bIsMLCNand = TRUE;
            pSM->uSpareSize = 64;
        } else if ((tempID[3] & 0x3) == 0x3) {
            pSM->uBlockPerFlash = 8191;//?
            pSM->uPagePerBlock = 128;
            pSM->uSectorPerBlock = 512;//?
            pSM->uPageSize = 8192;
            pSM->uNandECC = BCH_T12;
            pSM->bIsMLCNand = TRUE;
            pSM->uSpareSize = 368;
        }
        pSM->bIsMulticycle = TRUE;
        break;

    default:
        // 2017/3/21, To support Micron MT29F32G08CBACA NAND flash
        if ((tempID[0]==0x2C)&&(tempID[1]==0x68)&&(tempID[2]==0x04)&&(tempID[3]==0x4A)&&(tempID[4]==0xA9)) {
            pSM->uBlockPerFlash  = 4095;        // block index with 0-base. = physical blocks - 1
            pSM->uPagePerBlock   = 256;
            pSM->uSectorPerBlock = pSM->uPageSize / 512 * pSM->uPagePerBlock;
            pSM->uPageSize       = 4096;
            pSM->uNandECC        = BCH_T24;
            pSM->bIsMulticycle   = TRUE;
            //pSM->uSpareSize      = 224;
            pSM->uSpareSize      = 188;
            pSM->bIsMLCNand      = TRUE;
            break;
        }
        IsID=1;
    }


    /* Using PowerOn setting*/
    u32PowerOn = inpw(REG_PWRON);
    //if((u32PowerOn&0x3C0)!=0x3C0 ) {
    if ((u32PowerOn & 0xC0) != 0xC0) { /* PageSize PWRON[7:6] */
        const UINT16 BCH12_SPARE[3] = { 92,184,368};/* 2K, 4K, 8K */
        const UINT16 BCH15_SPARE[3] = {116,232,464};/* 2K, 4K, 8K */
        const UINT16 BCH24_SPARE[3] = { 90,180,360};/* 2K, 4K, 8K */
        unsigned int volatile gu_fmiSM_PageSize;
        unsigned int volatile g_u32ExtraDataSize;

        sysprintf("Using PowerOn setting(0x%x): ", (u32PowerOn>>6)&0xf);
        gu_fmiSM_PageSize = 1024 << (((u32PowerOn >> 6) & 0x3) + 1);
        switch(gu_fmiSM_PageSize) {
        case 2048:
            sysprintf(" PageSize = 2KB ");
            pSM->uPagePerBlock   = 64;
            pSM->uNandECC        = BCH_T4;
            break;
        case 4096:
            sysprintf(" PageSize = 4KB ");
            pSM->uPagePerBlock   = 128;
            pSM->uNandECC        = BCH_T8;
            break;
        case 8192:
            sysprintf(" PageSize = 8KB ");
            pSM->uPagePerBlock   = 128;
            pSM->uNandECC        = BCH_T12;
            break;
        }

        if((u32PowerOn & 0x300) != 0x300) { /* ECC PWRON[9:8] */
            switch((u32PowerOn & 0x300)) {
            case 0x000:
                sysprintf(" ECC = T12\n");
                g_u32ExtraDataSize = BCH12_SPARE[gu_fmiSM_PageSize >> 12] + 8;
                pSM->uNandECC = BCH_T12;
                break;
            case 0x100:
                sysprintf(" ECC = T15\n");
                g_u32ExtraDataSize = BCH15_SPARE[gu_fmiSM_PageSize >> 12] + 8;
                pSM->uNandECC = BCH_T15;
                break;
            case 0x200:
                sysprintf(" ECC = T24\n");
                g_u32ExtraDataSize = BCH24_SPARE[gu_fmiSM_PageSize >> 12] + 8;
                pSM->uNandECC = BCH_T24;
                break;
            }
        } else {
            sysprintf(" ECC = XXX\n");
            switch(gu_fmiSM_PageSize) {
            case 512:
                g_u32ExtraDataSize = NAND_EXTRA_512;
                break;
            case 2048:
                g_u32ExtraDataSize = NAND_EXTRA_2K;
                break;
            case 4096:
                g_u32ExtraDataSize = NAND_EXTRA_4K;
                break;
            case 8192:
                g_u32ExtraDataSize = NAND_EXTRA_8K;
                break;
            }
        }

        if(g_uIsUserConfig == 1) {
            pSM->uBlockPerFlash = Custom_uBlockPerFlash-1;
            pSM->uPagePerBlock = Custom_uPagePerBlock;
            sysprintf("Custom_uBlockPerFlash= %d, Custom_uPagePerBlock= %d\n", Custom_uBlockPerFlash, Custom_uPagePerBlock);
        }

        pSM->uPageSize       = gu_fmiSM_PageSize;
        pSM->uSectorPerBlock = pSM->uPageSize / 512 * pSM->uPagePerBlock;
        //pSM->bIsMulticycle   = TRUE;
        pSM->uSpareSize      = g_u32ExtraDataSize;
        //pSM->bIsMLCNand      = TRUE;
        sysprintf("User Configure:\nBlockPerFlash= %d, PagePerBlock= %d\n", pSM->uBlockPerFlash, pSM->uPagePerBlock);

    } else {
        if(IsID==1) {
            sysprintf("SM ID not support!! [%x][%x][%x][%x]\n", tempID[0], tempID[1], tempID[2], tempID[3]);
            return Fail;
        } else {
            if(g_uIsUserConfig == 1) {
                pSM->uBlockPerFlash = Custom_uBlockPerFlash-1;
                pSM->uPagePerBlock = Custom_uPagePerBlock;
                sysprintf("Custom_uBlockPerFlash= %d, Custom_uPagePerBlock= %d\n", Custom_uBlockPerFlash, Custom_uPagePerBlock);
            }
            sysprintf("Auto Detect:\nBlockPerFlash= %d, PagePerBlock= %d\n", pSM->uBlockPerFlash, pSM->uPagePerBlock);
        }
    }
    switch(pSM->uNandECC) {
    case BCH_T24:
        BCHAlgoIdx = 0;
        break;
    case BCH_T4:
        BCHAlgoIdx = 1;
        break;
    case BCH_T8:
        BCHAlgoIdx = 2;
        break;
    case BCH_T12:
        BCHAlgoIdx = 3;
        break;
    case BCH_T15:
        BCHAlgoIdx = 4;
        break;
    default:
        BCHAlgoIdx = 5;
        break;
    }
    sysprintf("PageSize= %d, ECC= %s, ExtraDataSize= %d, SectorPerBlock= %d\n\n", pSM->uPageSize, name[BCHAlgoIdx], pSM->uSpareSize, pSM->uSectorPerBlock);
    MSG_DEBUG("SM ID [%x][%x][%x][%x]\n", tempID[0], tempID[1], tempID[2], tempID[3]);
    return Successful;
}

/*-----------------------------------------------------------------------------
 * 2011/7/28 by CJChen1@nuvoton.com, To issue command and address to NAND flash chip
 *  to order NAND flash chip to prepare the data or RA data at chip side and wait FMI to read actually.
 *  Support large page size 2K / 4K / 8K.
 *  INPUT: ucColAddr = 0 means prepare data from begin of page;
 *                   = <page size> means prepare RA data from begin of spare area.
 *---------------------------------------------------------------------------*/
INT fmiSM2BufferM_large_page(UINT32 uPage, UINT32 ucColAddr)
{
    // clear R/B flag
    while(!(inpw(REG_SMISR) & 0x40000));
    outpw(REG_SMISR, 0x400);

    outpw(REG_SMCMD, 0x00);     // read command
    outpw(REG_SMADDR, ucColAddr);                   // CA0 - CA7
    outpw(REG_SMADDR, (ucColAddr >> 8) & 0xFF);     // CA8 - CA11
    outpw(REG_SMADDR, uPage & 0xff);                // PA0 - PA7

    if (!pSM->bIsMulticycle)
        outpw(REG_SMADDR, ((uPage >> 8) & 0xff)|0x80000000);    // PA8 - PA15
    else {
        outpw(REG_SMADDR, (uPage >> 8) & 0xff);             // PA8 - PA15
        outpw(REG_SMADDR, ((uPage >> 16) & 0xff)|0x80000000);   // PA16 - PA18
    }
    outpw(REG_SMCMD, 0x30);     // read command

    if (!fmiSMCheckRB())
        return FMI_SM_RB_ERR;
    else
        return 0;
}


INT fmiSM_Read_RA(UINT32 uPage, UINT32 ucColAddr)
{
    return fmiSM2BufferM_large_page(uPage, ucColAddr);
}

INT fmiCheckInvalidBlockExcept0xF0(FMI_SM_INFO_T *pSM, UINT32 BlockNo)
{
    int volatile status=0;
    unsigned int volatile sector;
    unsigned char volatile data512=0xff, data517=0xff, blockStatus=0xff;

    if (pSM->bIsMLCNand == TRUE)
        sector = (BlockNo+1) * pSM->uPagePerBlock - 1;
    else
        sector = BlockNo * pSM->uPagePerBlock;

    status = fmiSM_Read_RA(sector, pSM->uPageSize);
    if (status < 0) {
        sysprintf("ERROR: fmiCheckInvalidBlock(), for block %d, return 0x%x\n", BlockNo, status);
        return -1;  // storage error
    }
    blockStatus = inpw(REG_SMDATA) & 0xff;
    if (blockStatus == 0xFF || blockStatus == 0xF0) {
        fmiSM_Reset();
        status = fmiSM_Read_RA(sector+1, pSM->uPageSize);
        if (status < 0) {
            sysprintf("ERROR: fmiCheckInvalidBlock(), for block %d, return 0x%x\n", BlockNo, status);
            return -1;  // storage error
        }
        blockStatus = inpw(REG_SMDATA) & 0xff;
        if (blockStatus != 0xFF && blockStatus != 0xF0 ) {
            sysprintf("ERROR: blockStatus != 0xFF(0x%2x)\n", blockStatus);
            fmiSM_Reset();
            return 1;   // invalid block
        }
    } else {
        fmiSM_Reset();
        return 1;   // invalid block
    }

    fmiSM_Reset();
    return 0;
}

INT fmiCheckInvalidBlock(FMI_SM_INFO_T *pSM, UINT32 BlockNo)
{
    int volatile status=0;
    unsigned int volatile sector;
    unsigned char volatile data512=0xff, data517=0xff, blockStatus=0xff;

    if (pSM->bIsMLCNand == TRUE)
        sector = (BlockNo+1) * pSM->uPagePerBlock - 1;
    else
        sector = BlockNo * pSM->uPagePerBlock;

    status = fmiSM_Read_RA(sector, pSM->uPageSize);
    if (status < 0) {
        sysprintf("ERROR: fmiCheckInvalidBlock(), for block %d, return 0x%x\n", BlockNo, status);
        return -1;  // storage error
    }
    blockStatus = inpw(REG_SMDATA) & 0xff;
    if (blockStatus == 0xFF) {
        fmiSM_Reset();
        status = fmiSM_Read_RA(sector+1, pSM->uPageSize);
        if (status < 0) {
            sysprintf("ERROR: fmiCheckInvalidBlock(), for block %d, return 0x%x\n", BlockNo, status);
            return -1;  // storage error
        }
        blockStatus = inpw(REG_SMDATA) & 0xff;
        if (blockStatus != 0xFF) {
            fmiSM_Reset();
            return 1;   // invalid block
        }
    } else {
        fmiSM_Reset();
        return 1;   // invalid block
    }

    fmiSM_Reset();
    return 0;
}


INT fmiSM_BlockErase(FMI_SM_INFO_T *pSM, UINT32 uBlock)
{
    UINT32 page_no;

#ifndef ERASE_WITH_0XF0
    if (fmiCheckInvalidBlock(pSM, uBlock) != 1)
#else
    if (fmiCheckInvalidBlockExcept0xF0(pSM, uBlock) != 1)
    //if (fmiCheckInvalidBlockExcept0xF0(pSM, uBlock) == 0)
#endif
    {
        page_no = uBlock * pSM->uPagePerBlock;      // get page address

        while(!(inpw(REG_SMISR) & 0x40000));
        outpw(REG_SMISR, 0x400);

        if (inpw(REG_SMISR) & 0x4) {
            sysprintf("erase: error sector !!\n");
            outpw(REG_SMISR, 0x4);
        }
        outpw(REG_SMCMD, 0x60);     // erase setup command

        outpw(REG_SMADDR, (page_no & 0xff));        // PA0 - PA7
        if (!pSM->bIsMulticycle)
            outpw(REG_SMADDR, ((page_no  >> 8) & 0xff)|0x80000000);     // PA8 - PA15
        else {
            outpw(REG_SMADDR, ((page_no  >> 8) & 0xff));        // PA8 - PA15
            outpw(REG_SMADDR, ((page_no  >> 16) & 0xff)|0x80000000);        // PA16 - PA17
        }

        outpw(REG_SMCMD, 0xd0);     // erase command
        if (!fmiSMCheckRB())
            return FMI_SM_RB_ERR;

        outpw(REG_SMCMD, 0x70);     // status read command
        if (inpw(REG_SMDATA) & 0x01)    // 1:fail; 0:pass
            return FMI_SM_STATUS_ERR;
    }
#ifndef ERASE_WITH_0XF0
    else if(fmiCheckInvalidBlock(pSM, uBlock) == -1)
#else
    else if(fmiCheckInvalidBlockExcept0xF0(pSM, uBlock) == -1)
#endif
    {
        sysprintf("ERROR: storage error\n");
        return -1;  // storage error
    } else {
        return Fail;
    }
    return Successful;
}

INT fmiSM_BlockEraseBad(FMI_SM_INFO_T *pSM, UINT32 uBlock)
{
    UINT32 page_no;

    page_no = uBlock * pSM->uPagePerBlock;      // get page address

    while(!(inpw(REG_SMISR) & 0x40000));
    outpw(REG_SMISR, 0x400);

    if (inpw(REG_SMISR) & 0x4) {
        sysprintf("erase: error sector !!\n");
        outpw(REG_SMISR, 0x4);
    }
    outpw(REG_SMCMD, 0x60);     // erase setup command

    outpw(REG_SMADDR, (page_no & 0xff));        // PA0 - PA7
    if (!pSM->bIsMulticycle)
        outpw(REG_SMADDR, ((page_no  >> 8) & 0xff)|0x80000000);     // PA8 - PA15
    else {
        outpw(REG_SMADDR, ((page_no  >> 8) & 0xff));        // PA8 - PA15
        outpw(REG_SMADDR, ((page_no  >> 16) & 0xff)|0x80000000);        // PA16 - PA17
    }

    outpw(REG_SMCMD, 0xd0);     // erase command
    if (!fmiSMCheckRB())
        return FMI_SM_RB_ERR;

    outpw(REG_SMCMD, 0x70);     // status read command
    if (inpw(REG_SMDATA) & 0x01)    // 1:fail; 0:pass
        return FMI_SM_STATUS_ERR;

    return Successful;
}


INT fmiMarkBadBlock(FMI_SM_INFO_T *pSM, UINT32 BlockNo)
{
    UINT32 uSector, ucColAddr;

    /* check if MLC NAND */
    if (pSM->bIsMLCNand == TRUE) {
        uSector = (BlockNo+1) * pSM->uPagePerBlock - 1; // write last page
        ucColAddr = pSM->uPageSize;

        // send command
        outpw(REG_SMCMD, 0x80);     // serial data input command
        outpw(REG_SMADDR, ucColAddr);   // CA0 - CA7
        outpw(REG_SMADDR, (ucColAddr >> 8) & 0xff); // CA8 - CA11
        outpw(REG_SMADDR, uSector & 0xff);  // PA0 - PA7
        if (!pSM->bIsMulticycle)
            outpw(REG_SMADDR, ((uSector >> 8) & 0xff)|0x80000000);      // PA8 - PA15
        else {
            outpw(REG_SMADDR, (uSector >> 8) & 0xff);       // PA8 - PA15
            outpw(REG_SMADDR, ((uSector >> 16) & 0xff)|0x80000000);     // PA16 - PA17
        }
        outpw(REG_SMDATA, 0xf0);    // mark bad block (use 0xf0 instead of 0x00 to differ from Old (Factory) Bad Blcok Mark)
        outpw(REG_SMCMD, 0x10);

        if (! fmiSMCheckRB())
            return FMI_SM_RB_ERR;

        fmiSM_Reset();
        return 0;
    }
    /* SLC check the 2048 byte of 1st or 2nd page per block */
    else {  // SLC
        uSector = BlockNo * pSM->uPagePerBlock;     // write lst page
        if (pSM->uPageSize == 512) {
            ucColAddr = 0;          // write 4096th byte
            goto _mark_512;
        } else
            ucColAddr = pSM->uPageSize;

        // send command
        outpw(REG_SMCMD, 0x80);     // serial data input command
        outpw(REG_SMADDR, ucColAddr);   // CA0 - CA7
        outpw(REG_SMADDR, (ucColAddr >> 8) & 0xff); // CA8 - CA11
        outpw(REG_SMADDR, uSector & 0xff);  // PA0 - PA7
        if (!pSM->bIsMulticycle)
            outpw(REG_SMADDR, ((uSector >> 8) & 0xff)|0x80000000);      // PA8 - PA15
        else {
            outpw(REG_SMADDR, (uSector >> 8) & 0xff);       // PA8 - PA15
            outpw(REG_SMADDR, ((uSector >> 16) & 0xff)|0x80000000);     // PA16 - PA17
        }
        outpw(REG_SMDATA, 0xf0);    // mark bad block (use 0xf0 instead of 0x00 to differ from Old (Factory) Bad Blcok Mark)
        outpw(REG_SMCMD, 0x10);

        if (! fmiSMCheckRB())
            return FMI_SM_RB_ERR;

        fmiSM_Reset();
        return 0;

_mark_512:

        outpw(REG_SMCMD, 0x50);     // point to redundant area
        outpw(REG_SMCMD, 0x80);     // serial data input command
        outpw(REG_SMADDR, ucColAddr);   // CA0 - CA7
        outpw(REG_SMADDR, uSector & 0xff);  // PA0 - PA7
        if (!pSM->bIsMulticycle)
            outpw(REG_SMADDR, ((uSector >> 8) & 0xff)|0x80000000);      // PA8 - PA15
        else {
            outpw(REG_SMADDR, (uSector >> 8) & 0xff);       // PA8 - PA15
            outpw(REG_SMADDR, ((uSector >> 16) & 0xff)|0x80000000);     // PA16 - PA17
        }

        outpw(REG_SMDATA, 0xf0);    // 512
        outpw(REG_SMDATA, 0xff);
        outpw(REG_SMDATA, 0xff);
        outpw(REG_SMDATA, 0xff);
        outpw(REG_SMDATA, 0xf0);    // 516
        outpw(REG_SMDATA, 0xf0);    // 517
        outpw(REG_SMCMD, 0x10);
        if (! fmiSMCheckRB())
            return FMI_SM_RB_ERR;

        fmiSM_Reset();
        return 0;
    }
}

INT fmiSM_Erase(UINT32 uChipSel,UINT32 start, UINT32 len)
{
    int i, status;
    int volatile badBlock=0;

    // erase all chip
    for (i=0; i<len; i++) {
#ifndef ERASE_WITH_0XF0
        if (fmiCheckInvalidBlock(pSM, i+start) != 1)
#else
        if (fmiCheckInvalidBlockExcept0xF0(pSM, i+start) != 1)
#endif
        {
            status = fmiSM_BlockErase(pSM, i+start);
            if (status < 0) {
                fmiMarkBadBlock(pSM, i+start);
                badBlock++;
            }
        } else
            badBlock++;

        /* send status */
        SendAck(((i+1)*100) /(len));
    }
    return badBlock;
}

INT fmiSM_EraseBad(UINT32 uChipSel,UINT32 start, UINT32 len)
{
    int i, status;
    int volatile badBlock=0;

    // erase all chip
    for (i=0; i<len; i++) {
        status = fmiSM_BlockEraseBad(pSM, i+start);
        if (status < 0) {
            fmiMarkBadBlock(pSM, i+start);
            badBlock++;
        }
        /* send status */
        SendAck(((i+1)*100) /(len));
    }
    return badBlock;
}

//-----------------------------------------------------
INT fmiSM_ChipErase(UINT32 uChipSel)
{
    int i, status;
    int volatile badBlock=0;

    // erase all chip
    for (i=0; i<=pSM->uBlockPerFlash; i++) {
#ifndef ERASE_WITH_0XF0
        //if (fmiCheckInvalidBlock(pSM, i) != 1)
        if (fmiCheckInvalidBlock(pSM, i) == 0)
#else
        //if (fmiCheckInvalidBlockExcept0xF0(pSM, i) != 1)
        if (fmiCheckInvalidBlockExcept0xF0(pSM, i) == 0)
#endif
        {
            status = fmiSM_BlockErase(pSM, i);
            if (status < 0) {
                fmiMarkBadBlock(pSM, i);
                badBlock++;
            }
            /* send status */
            SendAck((i*100) / pSM->uBlockPerFlash);
        }
#ifndef ERASE_WITH_0XF0
        else if (fmiCheckInvalidBlock(pSM, i) == -1)
#else
        else if (fmiCheckInvalidBlockExcept0xF0(pSM, i) == -1)
#endif
        {
            badBlock = -1;
            /* send status */
            SendAck(0xffff);
        } else {
            badBlock++;
            /* send status */
            SendAck((i*100) / pSM->uBlockPerFlash);
        }
    }
    return badBlock;
}

INT fmiSM_ChipEraseBad(UINT32 uChipSel)
{
    int i, status;
    int volatile badBlock=0;

    // erase all chip
    for (i=0; i<=pSM->uBlockPerFlash; i++) {
        status = fmiSM_BlockEraseBad(pSM, i);
        if (status < 0) {
            fmiMarkBadBlock(pSM, i);
            badBlock++;
        }
        /* send status */
        SendAck((i*100) / pSM->uBlockPerFlash);
    }
    return badBlock;
}

/*-----------------------------------------------------------------------------
 * Really write data and parity code to 2K/4K/8K page size NAND flash by NAND commands.
 *---------------------------------------------------------------------------*/
INT fmiSM_Write_large_page_oob(UINT32 uSector, UINT32 ucColAddr, UINT32 uSAddr,UINT32 oobsize)
{
    int k;
    outpw(REG_NAND_DMACSAR, uSAddr);// set DMA transfer starting address

    // write the spare area configuration
    for(k=0; k<pSM->uSpareSize; k+=4)
        outpw(REG_SMRA0+k, *(unsigned int *)(uSAddr+pSM->uPageSize+k));

    // clear R/B flag
    while(!(inpw(REG_SMISR) & 0x40000));
    outpw(REG_SMISR, 0x400);

    // send command
    outpw(REG_SMCMD, 0x80);                     // serial data input command
    outpw(REG_SMADDR, ucColAddr);               // CA0 - CA7
    outpw(REG_SMADDR, (ucColAddr >> 8) & 0x3f); // CA8 - CA12
    outpw(REG_SMADDR, uSector & 0xff);          // PA0 - PA7

    if (!pSM->bIsMulticycle)
        outpw(REG_SMADDR, ((uSector >> 8) & 0xff)|0x80000000);  // PA8 - PA15
    else {
        outpw(REG_SMADDR, (uSector >> 8) & 0xff);           // PA8 - PA15
        outpw(REG_SMADDR, ((uSector >> 16) & 0xff)|0x80000000); // PA16 - PA17
    }

    outpw(REG_SMISR, 0x1);          // clear DMA flag
    outpw(REG_SMISR, 0x4);          // clear ECC_FIELD flag
    outpw(REG_SMISR, 0x8);          // clear Region Protect flag
    outpw(REG_SMCSR, inpw(REG_SMCSR) | 0x10);    // auto write redundancy data to NAND after page data written
    outpw(REG_SMCSR, inpw(REG_SMCSR) | 0x4);     // begin to write one page data to NAND flash

    while(1) {
        if (inpw(REG_SMISR) & 0x1)         // wait to finish DMAC transfer.
            break;
    }

    outpw(REG_SMISR, 0x1);  // clear DMA flag
    outpw(REG_SMCMD, 0x10); // auto program command

    if (!fmiSMCheckRB())
        return FMI_SM_RB_ERR;

    //--- check Region Protect result
    if (inpw(REG_SMISR) & 0x8) {
        sysprintf("ERROR: fmiSM_Write_large_page(): region write protect detected!!\n");
        outpw(REG_SMISR, 0x8);      // clear Region Protect flag
        return Fail;
    }

    outpw(REG_SMCMD, 0x70);         // status read command
    if (inpw(REG_SMDATA) & 0x01) {  // 1:fail; 0:pass
        sysprintf("ERROR: fmiSM_Write_large_page(): data error!!\n");
        return FMI_SM_STATE_ERROR;
    }
    return 0;
}

INT fmiSM_Write_large_page_oob2(UINT32 uSector, UINT32 ucColAddr, UINT32 uSAddr)
{
    UINT32 readlen,i;

    readlen=pSM->uPageSize+pSM->uSpareSize;
    outpw(REG_SMCMD,  0x80);  // serial data input command
    outpw(REG_SMADDR, 0x00);  // CA0 - CA7
    outpw(REG_SMADDR, 0x00);  // CA8 - CA12

    outpw(REG_SMADDR, uSector & 0xff);                      // PA0 - PA7
    outpw(REG_SMADDR, ((uSector >> 8) & 0xff));  // PA8 - PA15
    outpw(REG_SMADDR, ((uSector >> 16) & 0xff));            // PA16 - PA17
    outpw(REG_SMADDR, ((uSector >> 24) & 0xff)|0x80000000);
    MSG_DEBUG("readlen=%d\n",readlen);
    for (i=0; i<readlen; i++)
        outpw(REG_SMDATA, ((unsigned char *)uSAddr)[i]);

    outpw(REG_SMCMD, 0x10);               // auto program command

    if (!fmiSMCheckRB())
        return FMI_SM_RB_ERR;

    return 0;
}

INT fmiSM_Write_large_page(UINT32 uSector, UINT32 ucColAddr, UINT32 uSAddr)
{
    outpw(REG_NAND_DMACSAR, uSAddr);    // set DMA transfer starting address

    // set the spare area configuration
    /* write byte 2050, 2051 as used page */
    memset((void *)REG_SMRA0, 0xFF, 64);
    outpw(REG_SMRA0, 0x0000FFFF);

    // clear R/B flag
    while(!(inpw(REG_SMISR) & 0x40000));
    outpw(REG_SMISR, 0x400);

    // send command
    outpw(REG_SMCMD, 0x80);                     // serial data input command
    outpw(REG_SMADDR, ucColAddr);               // CA0 - CA7
    outpw(REG_SMADDR, (ucColAddr >> 8) & 0x3f); // CA8 - CA12
    outpw(REG_SMADDR, uSector & 0xff);          // PA0 - PA7

    if (!pSM->bIsMulticycle)
        outpw(REG_SMADDR, ((uSector >> 8) & 0xff)|0x80000000);  // PA8 - PA15
    else {
        outpw(REG_SMADDR, (uSector >> 8) & 0xff);           // PA8 - PA15
        outpw(REG_SMADDR, ((uSector >> 16) & 0xff)|0x80000000); // PA16 - PA17
    }

    outpw(REG_SMISR, 0x1);          // clear DMA flag
    outpw(REG_SMISR, 0x4);          // clear ECC_FIELD flag
    outpw(REG_SMISR, 0x8);          // clear Region Protect flag
    outpw(REG_SMCSR, inpw(REG_SMCSR) | 0x10);    // auto write redundancy data to NAND after page data written
    outpw(REG_SMCSR, inpw(REG_SMCSR) | 0x4);     // begin to write one page data to NAND flash

    while(1) {
        if (inpw(REG_SMISR) & 0x1)         // wait to finish DMAC transfer.
            break;
    }

    outpw(REG_SMISR, 0x1);  // clear DMA flag
    outpw(REG_SMCMD, 0x10); // auto program command

    if (!fmiSMCheckRB())
        return FMI_SM_RB_ERR;

    //--- check Region Protect result
    if (inpw(REG_SMISR) & 0x8) {
        sysprintf("ERROR: fmiSM_Write_large_page(): region write protect detected!!\n");
        outpw(REG_SMISR, 0x8);      // clear Region Protect flag
        return Fail;
    }

    outpw(REG_SMCMD, 0x70);         // status read command
    if (inpw(REG_SMDATA) & 0x01) {  // 1:fail; 0:pass
        sysprintf("ERROR: fmiSM_Write_large_page(): data error!!\n");
        return FMI_SM_STATE_ERROR;
    }
    return 0;
}


static void fmiSM_CorrectData_BCH(UINT8 ucFieidIndex, UINT8 ucErrorCnt, UINT8* pDAddr)
{
    UINT32 uaData[24], uaAddr[24];
    UINT32 uaErrorData[4];
    UINT8  ii, jj;
    UINT32 uPageSize;
    UINT32 field_len, padding_len, parity_len;
    UINT32 total_field_num;
    UINT8  *smra_index;

    //--- assign some parameters for different BCH and page size
    switch (inpw(REG_SMCSR) & 0x7c0000) {
    case BCH_T24:
        field_len   = 1024;
        padding_len = BCH_PADDING_LEN_1024;
        parity_len  = BCH_PARITY_LEN_T24;
        break;
    case BCH_T15:
        field_len   = 512;
        padding_len = BCH_PADDING_LEN_512;
        parity_len  = BCH_PARITY_LEN_T15;
        break;
    case BCH_T12:
        field_len   = 512;
        padding_len = BCH_PADDING_LEN_512;
        parity_len  = BCH_PARITY_LEN_T12;
        break;
    case BCH_T8:
        field_len   = 512;
        padding_len = BCH_PADDING_LEN_512;
        parity_len  = BCH_PARITY_LEN_T8;
        break;
    case BCH_T4:
        field_len   = 512;
        padding_len = BCH_PADDING_LEN_512;
        parity_len  = BCH_PARITY_LEN_T4;
        break;
    default:
        sysprintf("ERROR: fmiSM_CorrectData_BCH(): invalid SMCR_BCH_TSEL = 0x%08X\n", (UINT32)(inpw(REG_SMCSR) & 0x7c0000));
        return;
    }

    uPageSize = inpw(REG_SMCSR) & 0x30000;
    switch (uPageSize) {
    case 0x30000:
        total_field_num = 8192 / field_len;
        break;
    case 0x20000:
        total_field_num = 4096 / field_len;
        break;
    case 0x10000:
        total_field_num = 2048 / field_len;
        break;
    case 0x00000:
        total_field_num =  512 / field_len;
        break;
    default:
        sysprintf("ERROR: fmiSM_CorrectData_BCH(): invalid SMCR_PSIZE = 0x%08X\n", uPageSize);
        return;
    }

    //--- got valid BCH_ECC_DATAx and parse them to uaData[]
    // got the valid register number of BCH_ECC_DATAx since one register include 4 error bytes
    jj = ucErrorCnt/4;
    jj ++;
    if (jj > 6)
        jj = 6;     // there are 6 BCH_ECC_DATAx registers to support BCH T24

    for(ii=0; ii<jj; ii++) {
        uaErrorData[ii] = inpw(REG_BCH_ECC_DATA0 + ii*4);
    }

    for(ii=0; ii<jj; ii++) {
        uaData[ii*4+0] = uaErrorData[ii] & 0xff;
        uaData[ii*4+1] = (uaErrorData[ii]>>8) & 0xff;
        uaData[ii*4+2] = (uaErrorData[ii]>>16) & 0xff;
        uaData[ii*4+3] = (uaErrorData[ii]>>24) & 0xff;
    }

    //--- got valid REG_BCH_ECC_ADDRx and parse them to uaAddr[]
    // got the valid register number of REG_BCH_ECC_ADDRx since one register include 2 error addresses
    jj = ucErrorCnt/2;
    jj ++;
    if (jj > 12)
        jj = 12;    // there are 12 REG_BCH_ECC_ADDRx registers to support BCH T24

    for(ii=0; ii<jj; ii++) {
        uaAddr[ii*2+0] = inpw(REG_BCH_ECC_ADDR0 + ii*4) & 0x07ff;   // 11 bits for error address
        uaAddr[ii*2+1] = (inpw(REG_BCH_ECC_ADDR0 + ii*4)>>16) & 0x07ff;
    }

    //--- pointer to begin address of field that with data error
    pDAddr += (ucFieidIndex-1) * field_len;

    //--- correct each error bytes
    for(ii=0; ii<ucErrorCnt; ii++) {
        // for wrong data in field
        if (uaAddr[ii] < field_len) {
            sysprintf("BCH error corrected for data: address 0x%08X, data [0x%02X] --> ",
                      pDAddr+uaAddr[ii], *(pDAddr+uaAddr[ii]));

            *(pDAddr+uaAddr[ii]) ^= uaData[ii];

            sysprintf("[0x%02X]\n", *(pDAddr+uaAddr[ii]));
        }
        // for wrong first-3-bytes in redundancy area
        else if (uaAddr[ii] < (field_len+3)) {
            uaAddr[ii] -= field_len;
            uaAddr[ii] += (parity_len*(ucFieidIndex-1));    // field offset

            sysprintf("BCH error corrected for 3 bytes: address 0x%08X, data [0x%02X] --> ",
                      (UINT8 *)REG_SMRA0+uaAddr[ii], *((UINT8 *)REG_SMRA0+uaAddr[ii]));

            *((UINT8 *)REG_SMRA0+uaAddr[ii]) ^= uaData[ii];

            sysprintf("[0x%02X]\n", *((UINT8 *)REG_SMRA0+uaAddr[ii]));
        }
        // for wrong parity code in redundancy area
        else {
            // BCH_ERR_ADDRx = [data in field] + [3 bytes] + [xx] + [parity code]
            //                                   |<--     padding bytes      -->|
            // The BCH_ERR_ADDRx for last parity code always = field size + padding size.
            // So, the first parity code = field size + padding size - parity code length.
            // For example, for BCH T12, the first parity code = 512 + 32 - 23 = 521.
            // That is, error byte address offset within field is
            uaAddr[ii] = uaAddr[ii] - (field_len + padding_len - parity_len);

            // smra_index point to the first parity code of first field in register SMRA0~n
            smra_index = (UINT8 *)
                         (REG_SMRA0 + (inpw(REG_SMREACTL) & 0x1ff) - // bottom of all parity code -
                          (parity_len * total_field_num)             // byte count of all parity code
                         );

            // final address = first parity code of first field +
            //                 offset of fields +
            //                 offset within field
            sysprintf("BCH error corrected for parity: address 0x%08X, data [0x%02X] --> ",
                      smra_index + (parity_len * (ucFieidIndex-1)) + uaAddr[ii],
                      *((UINT8 *)smra_index + (parity_len * (ucFieidIndex-1)) + uaAddr[ii]));
            *((UINT8 *)smra_index + (parity_len * (ucFieidIndex-1)) + uaAddr[ii]) ^= uaData[ii];
            sysprintf("[0x%02X]\n", *((UINT8 *)smra_index + (parity_len * (ucFieidIndex-1)) + uaAddr[ii]));
        }
    }   // end of for (ii<ucErrorCnt)
}


INT fmiSM_Read_move_data_ecc_check(UINT32 uDAddr)
{
    UINT32 uStatus;
    UINT32 uErrorCnt, ii, jj;
    volatile UINT32 uError = 0;
    UINT32 uLoop;

    //--- uLoop is the number of SM_ECC_STx should be check.
    //      One SM_ECC_STx include ECC status for 4 fields.
    //      Field size is 1024 bytes for BCH_T24 and 512 bytes for other BCH.
    switch (inpw(REG_SMCSR) & 0x30000) {
    case 0x10000:
        uLoop = 1;
        break;
    case 0x20000:
        if (inpw(REG_SMCSR) & 0x7c0000 == BCH_T24)
            uLoop = 1;
        else
            uLoop = 2;
        break;
    case 0x30000:
        if (inpw(REG_SMCSR) & 0x7c0000 == BCH_T24)
            uLoop = 2;
        else
            uLoop = 4;
        break;
    default:
        return -1;     // don't work for 512 bytes page
    }

    outpw(REG_NAND_DMACSAR, uDAddr);    // set DMA transfer starting address
    outpw(REG_SMISR, 0x1);              // clear DMA flag
    outpw(REG_SMISR, 0x4);              // clear ECC_FIELD flag
    outpw(REG_SMCSR, inpw(REG_SMCSR) | 0x2);    // begin to move data by DMA

    //--- waiting for DMA transfer stop since complete or ECC error
    // IF no ECC error, DMA transfer complete and make SMCR[DRD_EN]=0
    // IF ECC error, DMA transfer suspend     and make SMISR[ECC_FIELD_IF]=1 but keep keep SMCR[DRD_EN]=1
    //      If we clear SMISR[ECC_FIELD_IF] to 0, DMA transfer will resume.
    // So, we should keep wait if DMA not complete (SMCR[DRD_EN]=1) and no ERR error (SMISR[ECC_FIELD_IF]=0)
    while((inpw(REG_SMCSR) & 0x2) && ((inpw(REG_SMISR) & 0x4)==0))
        ;

    //--- DMA transfer completed or suspend by ECC error, check and correct ECC error
    while(1) {
        if (inpw(REG_SMISR) & 0x4) {
            for (jj=0; jj<uLoop; jj++) {
                uStatus = inpw(REG_SMECC_ST0+jj*4);
                if (!uStatus)
                    continue;   // no error on this register for 4 fields
                // ECC error !! Check 4 fields. Each field has 512 bytes data
                for (ii=1; ii<5; ii++) {
                    if (!(uStatus & 0x3)) {   // no error for this field
                        uStatus >>= 8;  // next field
                        continue;
                    }

                    if ((uStatus & 0x3)==0x01) { // correctable error in field (jj*4+ii)
                        // 2011/8/17 by CJChen1@nuvoton.com, mask uErrorCnt since Fx_ECNT just has 5 valid bits
                        uErrorCnt = (uStatus >> 2) & 0x1F;
                        fmiSM_CorrectData_BCH(jj*4+ii, uErrorCnt, (UINT8*)uDAddr);
                        sysprintf("Warning: Field %d have %d BCH error. Corrected!!\n", jj*4+ii, uErrorCnt);
                        break;
                    } else if (((uStatus & 0x3)==0x02) ||
                               ((uStatus & 0x3)==0x03)) { // uncorrectable error or ECC error in 1st field
                        sysprintf("ERROR: Field %d encountered uncorrectable BCH error!!\n", jj*4+ii);
                        uError = 1;
                        break;
                    }
                    uStatus >>= 8;  // next field
                }
            }
            outpw(REG_SMISR, 0x4);      // clear ECC_FIELD_IF to resume DMA transfer
        }

        if (inpw(REG_SMISR) & 0x1) {    // wait to finish DMAC transfer.
            if ( !(inpw(REG_SMISR) & 0x4) )
                break;
        }
    }   // end of while(1)

    if (uError)
        return -1;
    else
        return 0;
}


INT fmiSM_Read_large_page(FMI_SM_INFO_T *pSM, UINT32 uPage, UINT32 uDAddr)
{
    INT result;

    result = fmiSM2BufferM_large_page(uPage, 0);
    if (result != 0)
        return result;  // fail for FMI_SM_RB_ERR
    result = fmiSM_Read_move_data_ecc_check(uDAddr);
    if(result<0) {
        sysprintf("  ==> page_no=%d,address=0x%08x\n",uPage,uDAddr);
    }
    return result;
}


BOOL volatile _usbd_bIsFMIInit = FALSE;
INT fmiHWInit(void)
{
    if (_usbd_bIsFMIInit == FALSE) {
        // Enable SD Card Host Controller operation and driving clock.
        outpw(REG_HCLKEN, (inpw(REG_HCLKEN) | 0x700000)); /* enable FMI, NAND, SD clock */

        // DMAC Initial
        outpw(REG_NAND_DMACCSR, DMAC_CSR_SWRST | DMAC_CSR_EN);
        while(inpw(REG_NAND_DMACCSR) & DMAC_CSR_SWRST);

        outpw(REG_NAND_FMICSR, FMI_CSR_SWRST);      // reset FMI engine
        while(inpw(REG_NAND_FMICSR) & FMI_CSR_SWRST);

        _usbd_bIsFMIInit = TRUE;
    }
    return 0;
} /* end fmiHWInit */

INT fmiNandInit(void)
{
    /* select NAND function pins */
    if (inpw(REG_PWRON) & 0x08000000) {
        /* Set GPI for NAND */
        outpw(REG_MFP_GPI_L, 0x55555550);
        outpw(REG_MFP_GPI_H, 0x55555555);
    } else {
        /* Set GPC for NAND */
        outpw(REG_MFP_GPC_L, 0x55555555);
        outpw(REG_MFP_GPC_H, 0x05555555);
    }
    // enable SM
    outpw(REG_NAND_FMICSR, FMI_CSR_SM_EN);

    /* set page size = 512B (default) enable CS0, disable CS1 */
    outpw(REG_SMCSR, inpw(REG_SMCSR) & ~0x02030000 | 0x04000000);

    outpw(REG_SMTCR, 0x20305);
    outpw(REG_SMCSR, (inpw(REG_SMCSR) & ~0x30000) | 0x00000);   //512 byte
    outpw(REG_SMCSR, inpw(REG_SMCSR) |  0x100); //protect RA 3 byte
    outpw(REG_SMCSR, inpw(REG_SMCSR) | 0x10);

    memset((char *)&SMInfo, 0, sizeof(FMI_SM_INFO_T));
    pSM = &SMInfo;
    if (fmiSM_ReadID(pSM) < 0)
        return Fail;
    fmiSM_Initial(pSM);

    return 0;
} /* end fmiHWInit */
