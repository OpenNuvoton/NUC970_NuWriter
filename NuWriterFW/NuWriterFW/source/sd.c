/******************************************************************************
 * @file     SD.c
 * @version  V0.10
 * $Revision: 1 $
 * $Date: 13/07/10 1:17p $
 * @brief    NUC970 series SD driver source file
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include "nuc970_reg.h"
#include "wblib.h"
#include "sd.h"
#include "usbd.h"


#define SD_BLOCK_SIZE   512

//#define SD_INITCOUNT 2000
//#define SD_TICKCOUNT  1000

// global variables
// For response R3 (such as ACMD41, CRC-7 is invalid; but SD controller will still
//      calculate CRC-7 and get an error result, software should ignore this error and clear SDISR [CRC_IF] flag
//      _sdio_uR3_CMD is the flag for it. 1 means software should ignore CRC-7 error
UINT32 _sd_uR3_CMD=0;
UINT32 _sd_uR7_CMD=0;

UINT8 *_sd_pSDHCBuffer;
UINT32 _sd_ReferenceClock;

__align(4096) UINT8 _sd_ucSDHCBuffer[64];

void SD_CheckRB()
{
    while(1)
    {
        outpw(REG_NAND_SDCSR, inpw(REG_NAND_SDCSR)|SD_CSR_CLK8_OE);
        while(inpw(REG_NAND_SDCSR) & SD_CSR_CLK8_OE);
        if (inpw(REG_NAND_SDISR) & SD_ISR_DATA0)
            break;
    }
}


int SD_SDCommand(FMI_SD_INFO_T *pSD, UINT8 ucCmd, UINT32 uArg)
{
    outpw(REG_NAND_SDARG, uArg);
    outpw(REG_NAND_SDCSR, (inpw(REG_NAND_SDCSR)&(~SD_CSR_CMD_MASK))|(ucCmd << 8)|(SD_CSR_CO_EN));

    while(inpw(REG_NAND_SDCSR) & SD_CSR_CO_EN);
    return Successful;
}


int SD_SDCmdAndRsp(FMI_SD_INFO_T *pSD, UINT8 ucCmd, UINT32 uArg, int ntickCount)
{
    outpw(REG_NAND_SDARG, uArg);
    outpw(REG_NAND_SDCSR, (inpw(REG_NAND_SDCSR)&(~SD_CSR_CMD_MASK))|(ucCmd << 8)|(SD_CSR_CO_EN | SD_CSR_RI_EN));

    if (ntickCount > 0)
    {
        while(inpw(REG_NAND_SDCSR) & SD_CSR_RI_EN)
        {
            if(ntickCount-- == 0) {
                outpw(REG_NAND_SDCSR, inpw(REG_NAND_SDCSR)|SD_CSR_SWRST);   // reset SD engine
                return 2;
            }
        }
    }
    else
    {
        while(inpw(REG_NAND_SDCSR) & SD_CSR_RI_EN);
    }

    if (_sd_uR7_CMD)
    {
        if (((inpw(REG_NAND_SDRSP1) & 0xff) != 0x55) && ((inpw(REG_NAND_SDRSP0) & 0xf) != 0x01))
        {
            _sd_uR7_CMD = 0;
            return SD_CMD8_ERROR;
        }
    }

    if (!_sd_uR3_CMD)
    {
        if (inpw(REG_NAND_SDISR) & SD_ISR_CRC7_OK)      // check CRC7
            return Successful;
        else
            return SD_CRC7_ERROR;
    }
    else    // ignore CRC error for R3 case
    {
        _sd_uR3_CMD = 0;
        outpw(REG_NAND_SDISR, SD_ISR_CRC_IF);
        return Successful;
    }
}


int SD_Swap32(int val)
{
#if 1
    int buf;

    buf = val;
    val <<= 24;
    val |= (buf<<8)&0xff0000;
    val |= (buf>>8)&0xff00;
    val |= (buf>>24)&0xff;
    return val;

#else
    return ((val<<24) | ((val<<8)&0xff0000) | ((val>>8)&0xff00) | (val>>24));
#endif
}

// Get 16 bytes CID or CSD
int SD_SDCmdAndRsp2(FMI_SD_INFO_T *pSD, UINT8 ucCmd, UINT32 uArg, UINT32 *puR2ptr)
{
    unsigned int i;
    unsigned int tmpBuf[5];

    outpw(REG_NAND_SDARG, uArg);
    outpw(REG_NAND_SDCSR, (inpw(REG_NAND_SDCSR)&(~SD_CSR_CMD_MASK))|(ucCmd << 8)|(SD_CSR_CO_EN | SD_CSR_R2_EN));

    while(inpw(REG_NAND_SDCSR) & SD_CSR_R2_EN);

    if (inpw(REG_NAND_SDISR) & SD_ISR_CRC7_OK)
    {
        for (i=0; i<5; i++)
            tmpBuf[i] = SD_Swap32(inpw(REG_NAND_FB0+i*4));

        for (i=0; i<4; i++)
            *puR2ptr++ = ((tmpBuf[i] & 0x00ffffff)<<8) | ((tmpBuf[i+1] & 0xff000000)>>24);
        return Successful;
    }
    else
        return SD_CRC7_ERROR;
}


int SD_SDCmdAndRspDataIn(FMI_SD_INFO_T *pSD, UINT8 ucCmd, UINT32 uArg)
{
    volatile int buf;

    outpw(REG_NAND_SDARG, uArg);
    outpw(REG_NAND_SDCSR, (inpw(REG_NAND_SDCSR)&(~SD_CSR_CMD_MASK))|(ucCmd << 8)|(SD_CSR_CO_EN | SD_CSR_RI_EN | SD_CSR_DI_EN));

    while (inpw(REG_NAND_SDCSR) & SD_CSR_RI_EN);

    while (inpw(REG_NAND_SDCSR) & SD_CSR_DI_EN);

    if (!(inpw(REG_NAND_SDISR) & SD_ISR_CRC7_OK)) {     // check CRC7
        return SD_CRC7_ERROR;
    }

    if (!(inpw(REG_NAND_SDISR) & SD_ISR_CRC16_OK)) {    // check CRC16
        return SD_CRC16_ERROR;
    }
    return Successful;
}

/*-----------------------------------------------------------------------------
 * 2011/6/24 by CJChen1@nuvoton.com, To set up the clock for SD_CLK
 *      SD_CLK = UPLL / ((CLKDIV2[SD_N0] + 1) * (CLKDIV2[SD_N1] + 1))
 *      Support both FPGA board and real chip.
 *          FPGA board don't support CLKDIV2[SD_N0] divider.
 *      INPUT: sd_clock_khz: the SD clock you wanted with unit KHz.
 *---------------------------------------------------------------------------*/
// there are 3 bits for divider N0, maximum is 8
#define SD_CLK_DIV0_MAX     8
// there are 8 bits for divider N1, maximum is 256
#define SD_CLK_DIV1_MAX     256

void SD_Set_clock(UINT32 sd_clock_khz)
{
    UINT32 rate, div0, div1, i;

    rate = _sd_ReferenceClock / sd_clock_khz;

    // choose slower clock if system clock cannot divisible by wanted clock
    if (_sd_ReferenceClock % sd_clock_khz != 0)
        rate++;

    if (rate >= (SD_CLK_DIV0_MAX * SD_CLK_DIV1_MAX)) { // the maximum divider for SD_CLK is (SD_CLK_DIV0_MAX * SD_CLK_DIV1_MAX)
        rate = SD_CLK_DIV0_MAX * SD_CLK_DIV1_MAX;
    }

    for (div0 = SD_CLK_DIV0_MAX; div0 > 0; div0--) {  // choose the maximum value if can exact division
        if (rate % div0 == 0)
            break;
    }
    if (div0 == 0) { // cannot exact division
        // if rate <= SD_CLK_DIV1_MAX, set div0 to 1 since div1 can exactly divide input clock
        div0 = (rate <= SD_CLK_DIV1_MAX) ? 1 : SD_CLK_DIV0_MAX;
    }

    //--- calculate the second divider
    div1 = rate / div0;
    div1 &= 0xFF;

    MSG_DEBUG("fmiSD_Set_clock(): wanted clock=%d, rate=%d, div0=%d, div1=%d\n", sd_clock_khz, rate, div0, div1);

    //--- setup register
    //outpw(REG_CLKDIV3, (inpw(REG_CLKDIV3) & ~0x18) | (0x0 << 3));       // SD clock from XIN [4:3]
    outpw(REG_CLKDIV3, (inpw(REG_CLKDIV3) & ~0x18) | (0x3 << 3));       // SD clock from UPLL [4:3]
    outpw(REG_CLKDIV3, (inpw(REG_CLKDIV3) & ~0x7) | (div0-1));                 // SD clock divided by CLKDIV3[SD_N] [2:0]
    outpw(REG_CLKDIV3, (inpw(REG_CLKDIV3) & ~0xff00) | ((div1-1) << 8)); // SD clock divided by CLKDIV3[SD_N] [15:8]
    for(i=0; i<1000; i++);  // waiting for clock become stable
    MSG_DEBUG("clock: 0x%x\n", inpw(REG_CLKDIV3));
    return;
}

// Initial
int SD_Init(FMI_SD_INFO_T *pSD)
{
    int volatile i, status;
    unsigned int resp;
    unsigned int CIDBuffer[4];
    unsigned int volatile u32CmdTimeOut;

    // set the clock to 200KHz
    SD_Set_clock(300);
    //SD_Set_clock(50);
    // power ON 74 clock
    outpw(REG_NAND_SDCSR, inpw(REG_NAND_SDCSR) | SD_CSR_CLK74_OE);
    while(inpw(REG_NAND_SDCSR) & SD_CSR_CLK74_OE);

    SD_SDCommand(pSD, 0, 0);        // reset all cards
    for (i=0x100; i>0; i--);

    // initial SDHC
    _sd_uR7_CMD = 1;
    u32CmdTimeOut = 5000;

    i = SD_SDCmdAndRsp(pSD, 8, 0x00000155, u32CmdTimeOut);
    MSG_DEBUG("CMD8 i=0x%08x,Successful=0x%08x\n",i,Successful);
    if (i == Successful)
    {
        // SD 2.0
        SD_SDCmdAndRsp(pSD, 55, 0x00, u32CmdTimeOut);
        _sd_uR3_CMD = 1;
        SD_SDCmdAndRsp(pSD, 41, 0x40ff8000, u32CmdTimeOut); // 2.7v-3.6v
        resp = inpw(REG_NAND_SDRSP0);
        while (!(resp & 0x00800000)) {      // check if card is ready
            SD_SDCmdAndRsp(pSD, 55, 0x00, u32CmdTimeOut);
            _sd_uR3_CMD = 1;
            SD_SDCmdAndRsp(pSD, 41, 0x40ff8000, u32CmdTimeOut); // 3.0v-3.4v
            resp = inpw(REG_NAND_SDRSP0);
        }
        if (resp & 0x00400000)
            pSD->uCardType = SD_TYPE_SD_HIGH;
        else
            pSD->uCardType = SD_TYPE_SD_LOW;
    }
    else
    {
        // SD 1.1				 
        SD_SDCommand(pSD, 0, 0);        // reset all cards
        for (i=0x100; i>0; i--);

        i = SD_SDCmdAndRsp(pSD, 55, 0x00, u32CmdTimeOut);
        if (i == 2) {   // MMC memory
            SD_SDCommand(pSD, 0, 0);        // reset
            for (i=0x100; i>0; i--);

            _sd_uR3_CMD = 1;
            if (SD_SDCmdAndRsp(pSD, 1, 0x40ff8080, u32CmdTimeOut) != 2) // eMMC memory
            {
                resp = inpw(REG_NAND_SDRSP0);
                while (!(resp & 0x00800000))        // check if card is ready
                {
                    _sd_uR3_CMD = 1;
                    SD_SDCmdAndRsp(pSD, 1, 0x40ff8080, u32CmdTimeOut);      // high voltage
                    resp = inpw(REG_NAND_SDRSP0);
                }
                pSD->uCardType = SD_TYPE_EMMC;
            }
            else
            {							  
                SD_SDCommand(pSD, 0, 0);        // reset
                for (i=0x100; i>0; i--){;}
                i=SD_SDCmdAndRsp(pSD, 1, 0x80ff8000, u32CmdTimeOut);
                MSG_DEBUG("i=SD_SDCmdAndRs i=%d\n",i);
                if ( i!= 2) // MMC memory
                {
                    resp = inpw(REG_NAND_SDRSP0);
                    while (!(resp & 0x00800000)) {      // check if card is ready
                        _sd_uR3_CMD = 1;
                        SD_SDCmdAndRsp(pSD, 1, 0x80ff8000, u32CmdTimeOut);      // high voltage
                        resp = inpw(REG_NAND_SDRSP0);
                    }
                    pSD->uCardType = SD_TYPE_MMC;
                }else{
                    pSD->uCardType = SD_TYPE_UNKNOWN;
                    return SD_ERR_DEVICE;
                }
            }
        }
        else if (i == 0)    // SD Memory
        {
					  int count=10;
            _sd_uR3_CMD = 1;
            SD_SDCmdAndRsp(pSD, 41, 0x00ff8000, u32CmdTimeOut); // 3.0v-3.4v
            resp = inpw(REG_NAND_SDRSP0);
            while (!(resp & 0x00800000) && count>0) {      // check if card is ready
                SD_SDCmdAndRsp(pSD, 55, 0x00,u32CmdTimeOut);
                _sd_uR3_CMD = 1;
                SD_SDCmdAndRsp(pSD, 41, 0x00ff8000, u32CmdTimeOut); // 3.0v-3.4v
                resp = inpw(REG_NAND_SDRSP0);
                count--;
            }
            if(count<=0) {
                pSD->uCardType = SD_TYPE_UNKNOWN;
                return SD_INIT_ERROR;
            }
            pSD->uCardType = SD_TYPE_SD_LOW;
        } else {
            pSD->uCardType = SD_TYPE_UNKNOWN;
            return SD_INIT_ERROR;
        }
    }

    // CMD2, CMD3
    if (pSD->uCardType != SD_TYPE_UNKNOWN)
    {
        SD_SDCmdAndRsp2(pSD, 2, 0x00, CIDBuffer);
        if ((pSD->uCardType == SD_TYPE_MMC) || (pSD->uCardType == SD_TYPE_EMMC))
        {
            if ((status = SD_SDCmdAndRsp(pSD, 3, 0x10000, 0)) != Successful)        // set RCA
                return status;
            pSD->uRCA = 0x10000;
        }
        else
        {
            if ((status = SD_SDCmdAndRsp(pSD, 3, 0x00, 0)) != Successful)       // get RCA
                return status;
            else
                pSD->uRCA = (inpw(REG_NAND_SDRSP0) << 8) & 0xffff0000;
        }
    }

    if (pSD->uCardType == SD_TYPE_SD_HIGH)
        MSG_DEBUG("This is high capacity SD memory card\n");
    if (pSD->uCardType == SD_TYPE_SD_LOW)
        MSG_DEBUG("This is standard capacity SD memory card\n");
    if (pSD->uCardType == SD_TYPE_MMC)
        MSG_DEBUG("This is MMC memory card\n");
    if (pSD->uCardType == SD_TYPE_EMMC)
        MSG_DEBUG("This is eMMC memory card\n");

    SD_Set_clock(50000);
    return Successful;
}


int SD_SwitchToHighSpeed(FMI_SD_INFO_T *pSD)
{
    int volatile status=0;
    UINT16 current_comsumption, busy_status0;

    outpw(REG_NAND_DMACSAR, (UINT32)_sd_pSDHCBuffer);   // set DMA transfer starting address
    outpw(REG_NAND_SDBLEN, 63); // 512 bit

    if ((status = SD_SDCmdAndRspDataIn(pSD, 6, 0x00ffff01)) != Successful)
        return Fail;

    current_comsumption = _sd_pSDHCBuffer[0]<<8 | _sd_pSDHCBuffer[1];
    if (!current_comsumption)
        return Fail;

    busy_status0 = _sd_pSDHCBuffer[28]<<8 | _sd_pSDHCBuffer[29];

    if (!busy_status0) { // function ready
        outpw(REG_NAND_DMACSAR, (UINT32)_sd_pSDHCBuffer);   // set DMA transfer starting address
        outpw(REG_NAND_SDBLEN, 63); // 512 bit

        if ((status = SD_SDCmdAndRspDataIn(pSD, 6, 0x80ffff01)) != Successful)
            return Fail;

        // function change timing: 8 clocks
        outpw(REG_NAND_SDCSR, inpw(REG_NAND_SDCSR)|SD_CSR_CLK8_OE);
        while(inpw(REG_NAND_SDCSR) & SD_CSR_CLK8_OE);

        current_comsumption = _sd_pSDHCBuffer[0]<<8 | _sd_pSDHCBuffer[1];
        if (!current_comsumption)
            return Fail;

        return Successful;
    }
    else
        return Fail;
}


int SD_SelectCardType(FMI_SD_INFO_T *pSD)
{
    int volatile status=0;

    if ((status = SD_SDCmdAndRsp(pSD, 7, pSD->uRCA, 0)) != Successful)
        return status;

    SD_CheckRB();

    // if SD card set 4bit
    if (pSD->uCardType == SD_TYPE_SD_HIGH)
    {
        _sd_pSDHCBuffer = (UINT8 *)((UINT32)_sd_ucSDHCBuffer);
        outpw(REG_NAND_DMACSAR, (UINT32)_sd_pSDHCBuffer);   // set DMA transfer starting address
        outpw(REG_NAND_SDBLEN, 7);  // 64 bit

        if ((status = SD_SDCmdAndRsp(pSD, 55, pSD->uRCA, 0)) != Successful)
            return status;
        if ((status = SD_SDCmdAndRspDataIn(pSD, 51, 0x00)) != Successful)
            return status;

        if ((_sd_ucSDHCBuffer[0] & 0xf) == 0x2)
        {
            status = SD_SwitchToHighSpeed(pSD);
            if (status == Successful)
            {
                /* divider */
                //SD_Set_clock(SDHC_FREQ);
            }
        }

        if ((status = SD_SDCmdAndRsp(pSD, 55, pSD->uRCA, 0)) != Successful)
            return status;
        if ((status = SD_SDCmdAndRsp(pSD, 6, 0x02, 0)) != Successful)   // set bus width
            return status;

        outpw(REG_NAND_SDCSR, inpw(REG_NAND_SDCSR)|SD_CSR_DBW_4BIT);
    }
    else if (pSD->uCardType == SD_TYPE_SD_LOW)
    {
        _sd_pSDHCBuffer = (UINT8 *)((UINT32)_sd_ucSDHCBuffer);
        outpw(REG_NAND_DMACSAR, (UINT32)_sd_pSDHCBuffer);   // set DMA transfer starting address
        outpw(REG_NAND_SDBLEN, 7);  // 64 bit

        if ((status = SD_SDCmdAndRsp(pSD, 55, pSD->uRCA, 0)) != Successful)
            return status;
        if ((status = SD_SDCmdAndRspDataIn(pSD, 51, 0x00)) != Successful)
            return status;

        // set data bus width. ACMD6 for SD card, SDCR_DBW for host.
        if ((status = SD_SDCmdAndRsp(pSD, 55, pSD->uRCA, 0)) != Successful)
            return status;

        if ((status = SD_SDCmdAndRsp(pSD, 6, 0x02, 0)) != Successful)   // set bus width
            return status;

        outpw(REG_NAND_SDCSR, inpw(REG_NAND_SDCSR)|SD_CSR_DBW_4BIT);
    }
    else if ((pSD->uCardType == SD_TYPE_MMC) ||(pSD->uCardType == SD_TYPE_EMMC))
    {
        outpw(REG_NAND_SDCSR, inpw(REG_NAND_SDCSR) & ~SD_CSR_DBW_4BIT);
    }

    if ((status = SD_SDCmdAndRsp(pSD, 16, SD_BLOCK_SIZE, 0)) != Successful) // set block length
        return status;
    outpw(REG_NAND_SDBLEN, SD_BLOCK_SIZE - 1);           // set the block size

    SD_SDCommand(pSD, 7, 0);

    outpw(REG_NAND_SDIER, inpw(REG_NAND_SDIER)|SD_IER_BLKD_IE);

    return Successful;
}

/*-----------------------------------------------------------------------------
 * sdioSD_Read_in_blksize(), To read data with black size "blksize"
 *---------------------------------------------------------------------------*/
int SD_Read_in_blksize(FMI_SD_INFO_T *pSD, UINT32 uSector, UINT32 uBufcnt, UINT32 uDAddr, UINT32 blksize)
{
    char volatile bIsSendCmd=FALSE, buf;
    unsigned int volatile reg;
    int volatile i, loop, status;

    //--- check input parameters
    if (uBufcnt == 0)
    {
        return SD_SELECT_ERROR;
    }

    if ((status = SD_SDCmdAndRsp(pSD, 7, pSD->uRCA, 0)) != Successful)
        return status;
    SD_CheckRB();

    outpw(REG_NAND_SDBLEN, blksize - 1);   // the actual byte count is equal to (SDBLEN+1)
    outpw(REG_NAND_SDARG, uSector);
    outpw(REG_NAND_DMACSAR, uDAddr);

    loop = uBufcnt / 255;
    for (i=0; i<loop; i++)
    {
        reg = (inpw(REG_NAND_SDCSR) & ~SD_CSR_CMD_MASK) | 0xff0000;
        if (bIsSendCmd == FALSE)
        {
            outpw(REG_NAND_SDCSR, reg|(18<<8)|(SD_CSR_CO_EN | SD_CSR_RI_EN | SD_CSR_DI_EN));
            bIsSendCmd = TRUE;
        }
        else
            outpw(REG_NAND_SDCSR, reg | SD_CSR_DI_EN);

        while(1)
        {
            if (inpw(REG_NAND_SDISR) & SD_ISR_BLKD_IF)
            {
                outpw(REG_NAND_SDISR, SD_ISR_BLKD_IF);
                break;
            }
        }

        if (!(inpw(REG_NAND_SDISR) & SD_ISR_CRC7_OK)) {     // check CRC7
            //printf("sdioSD_Read_in_blksize(): response error!\n");
            return SD_CRC7_ERROR;
        }

        if (!(inpw(REG_NAND_SDISR) & SD_ISR_CRC16_OK)) {    // check CRC16
            //printf("sdioSD_Read_in_blksize() :read data error!\n");
            return SD_CRC16_ERROR;
        }
    }

    loop = uBufcnt % 255;
    if (loop != 0)
    {
        reg = inpw(REG_NAND_SDCSR) & (~SD_CSR_CMD_MASK);
        reg = reg & (~SD_CSR_BLK_CNT_MASK);
        reg |= (loop << 16);    // setup SDCR_BLKCNT

        if (bIsSendCmd == FALSE)
        {
            outpw(REG_NAND_SDCSR, reg|(18<<8)|(SD_CSR_CO_EN | SD_CSR_RI_EN | SD_CSR_DI_EN));
            bIsSendCmd = TRUE;
        }
        else
            outpw(REG_NAND_SDCSR, reg | SD_CSR_DI_EN);

        while(1)
        {
            if (inpw(REG_NAND_SDISR) & SD_ISR_BLKD_IF)
            {
                outpw(REG_NAND_SDISR, SD_ISR_BLKD_IF);
                break;
            }
        }

        if (!(inpw(REG_NAND_SDISR) & SD_ISR_CRC7_OK)) {     // check CRC7
            //printf("sdioSD_Read_in_blksize(): response error!\n");
            return SD_CRC7_ERROR;
        }

        if (!(inpw(REG_NAND_SDISR) & SD_ISR_CRC16_OK)) {    // check CRC16
            //printf("sdioSD_Read_in_blksize(): read data error!\n");
            return SD_CRC16_ERROR;
        }
    }

    if (SD_SDCmdAndRsp(pSD, 12, 0, 0)) {    // stop command
        //printf("stop command fail !!\n");
        return SD_CRC7_ERROR;
    }
    SD_CheckRB();

    SD_SDCommand(pSD, 7, 0);

    return Successful;
}

/*-----------------------------------------------------------------------------
 * sdioSD_Read_in(), To read data with default black size SD_BLOCK_SIZE
 *---------------------------------------------------------------------------*/
int SD_Read_in(FMI_SD_INFO_T *pSD, UINT32 uSector, UINT32 uBufcnt, UINT32 uDAddr)
{
    return SD_Read_in_blksize(pSD, uSector, uBufcnt, uDAddr, SD_BLOCK_SIZE);
}

/*-----------------------------------------------------------------------------
 * sdioSD_Write_in(), To write data with static black size SD_BLOCK_SIZE
 *---------------------------------------------------------------------------*/
int SD_Write_in(FMI_SD_INFO_T *pSD, UINT32 uSector, UINT32 uBufcnt, UINT32 uSAddr)
{
    char volatile bIsSendCmd = FALSE;
    unsigned int volatile reg;
    int volatile i, loop, status;

    //--- check input parameters
    if (uBufcnt == 0)
    {
        return SD_SELECT_ERROR;
    }

    if ((status = SD_SDCmdAndRsp(pSD, 7, pSD->uRCA, 0)) != Successful)
        return status;

    SD_CheckRB();

    // According to SD Spec v2.0, the write CMD block size MUST be 512, and the start address MUST be 512*n.
    outpw(REG_NAND_SDBLEN, SD_BLOCK_SIZE - 1);           // set the block size
    outpw(REG_NAND_SDARG, uSector);
    outpw(REG_NAND_DMACSAR, uSAddr);
    loop = uBufcnt / 255;
    for (i=0; i<loop; i++)
    {
        reg = (inpw(REG_NAND_SDCSR) & 0xff00c080)|0xff0000;
        if (!bIsSendCmd)
        {
            outpw(REG_NAND_SDCSR, reg|(25<<8)|(SD_CSR_CO_EN | SD_CSR_RI_EN | SD_CSR_DO_EN));
            bIsSendCmd = TRUE;
        }
        else
            outpw(REG_NAND_SDCSR, reg | SD_CSR_DO_EN);

        while(1)
        {
            if (inpw(REG_NAND_SDISR) & SD_ISR_BLKD_IF)
            {
                outpw(REG_NAND_SDISR, SD_ISR_BLKD_IF);
                break;
            }
        }

        if ((inpw(REG_NAND_SDISR) & SD_ISR_CRC_IF) != 0) {      // check CRC
            outpw(REG_NAND_SDISR, SD_ISR_CRC_IF);
            return SD_CRC_ERROR;
        }
    }

    loop = uBufcnt % 255;
    if (loop != 0)
    {
        reg = (inpw(REG_NAND_SDCSR) & 0xff00c080) | (loop << 16);
        if (!bIsSendCmd)
        {
            outpw(REG_NAND_SDCSR, reg|(25<<8)|(SD_CSR_CO_EN | SD_CSR_RI_EN | SD_CSR_DO_EN));
            bIsSendCmd = TRUE;
        }
        else
            outpw(REG_NAND_SDCSR, reg | SD_CSR_DO_EN);

        while(1)
        {
            if (inpw(REG_NAND_SDISR) & SD_ISR_BLKD_IF)
            {
                outpw(REG_NAND_SDISR, SD_ISR_BLKD_IF);
                break;
            }
        }

        if ((inpw(REG_NAND_SDISR) & SD_ISR_CRC_IF) != 0) {      // check CRC
            outpw(REG_NAND_SDISR, SD_ISR_CRC_IF);
            return SD_CRC_ERROR;
        }
    }
    outpw(REG_NAND_SDISR, SD_ISR_CRC_IF);

    if (SD_SDCmdAndRsp(pSD, 12, 0, 0))      // stop command
    {
        return SD_CRC7_ERROR;
    }
    SD_CheckRB();

    SD_SDCommand(pSD, 7, 0);

    return Successful;
}


void SD_Get_SD_info(FMI_SD_INFO_T *pSD, DISK_DATA_T *_info)
{
    unsigned int i;
    unsigned int R_LEN, C_Size, MULT, size;
    unsigned int Buffer[4];
    unsigned char *ptr;

    SD_SDCmdAndRsp2(pSD, 9, pSD->uRCA, Buffer);

    if ((Buffer[0] & 0xc0000000) && (pSD->uCardType != SD_TYPE_MMC) && (pSD->uCardType != SD_TYPE_EMMC))
    {
        C_Size = ((Buffer[1] & 0x0000003f) << 16) | ((Buffer[2] & 0xffff0000) >> 16);
        size = (C_Size+1) * 512;    // Kbytes

        _info->diskSize = size;
        _info->totalSectorN = size << 1;
    }
    else
    {
        R_LEN = (Buffer[1] & 0x000f0000) >> 16;
        C_Size = ((Buffer[1] & 0x000003ff) << 2) | ((Buffer[2] & 0xc0000000) >> 30);
        MULT = (Buffer[2] & 0x00038000) >> 15;
        size = (C_Size+1) * (1<<(MULT+2)) * (1<<R_LEN);

        _info->diskSize = size / 1024;
        _info->totalSectorN = size / 512;
    }
    _info->sectorSize = 512;

    SD_SDCmdAndRsp2(pSD, 10, pSD->uRCA, Buffer);

    _info->vendor[0] = (Buffer[0] & 0xff000000) >> 24;
    ptr = (unsigned char *)Buffer;
    ptr = ptr + 4;
    for (i=0; i<5; i++)
        _info->product[i] = *ptr++;
    ptr = ptr + 10;
    for (i=0; i<4; i++)
        _info->serial[i] = *ptr++;
}

void SD_SetReferenceClock(UINT32 uClock)
{
    _sd_ReferenceClock = uClock;    // kHz
}


int SD_ChipErase(FMI_SD_INFO_T *pSD, DISK_DATA_T *_info)
{
    int status=0;

    status = SD_SDCmdAndRsp(pSD, 32, 512, 6000);
    if (status < 0)
    {
        return status;
    }
    status = SD_SDCmdAndRsp(pSD, 33, _info->totalSectorN*512, 6000);
    if (status < 0)
    {
        return status;
    }
    status = SD_SDCmdAndRsp(pSD, 38, 0, 6000);
    if (status < 0)
    {
        return status;
    }
    SD_CheckRB();

    return 0;
}

