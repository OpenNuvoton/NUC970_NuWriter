/***************************************************************************
 *                                                                         *
 * Copyright (c) 2008 Nuvoton Technolog. All rights reserved.              *
 *                                                                         *
 ***************************************************************************/

/****************************************************************************
* FILENAME
*   wb_power.c
*
* VERSION
*   1.0
*
* DESCRIPTION
*   The power managemnet related function of Nuvoton ARM9 MCU
*
* DATA STRUCTURES
*   None
*
* FUNCTIONS
*   sysDisableAllPM_IRQ
*   sysEnablePM_IRQ
*   sysPMStart
*
* HISTORY
*   2008-07-24  Ver 1.0 Modified by Min-Nan Cheng
*
* REMARK
*   When enter PD or MIDLE mode, the sysPMStart function will disable cache
*   (in order to access SRAM) and then recovery it after wake up.
****************************************************************************/
#include <string.h>
#include "wblib.h"

extern BOOL	sysGetCacheState(void);
extern INT32 sysGetCacheMode(void);
extern INT32 _sysLockCode(UINT32, INT32);
extern INT32 _sysUnLockCode(void);

static UINT32 volatile _sys_PM_IRQ = 0;

/****************************************************************************
* FUNCTION
*   sysEnablePM_IRQ
*
* DESCRIPTION
*   This function is use to set _sys_PM_IRQ variable. The _sys_PM_IRQ variable
*   save the IRQ channel which use to wake up the system form IDLE, MIDLE, and
*   PD mode
*
* INPUTS
*   irq_no         The interrupt channel
*
* OUTPUTS
*   Successful              The interrupt channel saved successfully
*   WB_PM_INVALID_IRQ_NUM   The interrupt channel number out of range
*
****************************************************************************/
INT sysEnablePM_IRQ(INT irq_no) // set the PM needed IRQ flag
{
    if ((irq_no > 31) || (irq_no < 1))
    {
        return WB_PM_INVALID_IRQ_NUM;
    }

    _sys_PM_IRQ = _sys_PM_IRQ | (UINT32)(1<<irq_no);
    return Successful;
}

/****************************************************************************
* FUNCTION
*   sysDisableAllPM_IRQ
*
* DESCRIPTION
*   This function is use to reset the _sys_PM_IRQ variable.
*
* INPUTS
*   None
*
* OUTPUTS
*   None
*
****************************************************************************/
void sysDisableAllPM_IRQ(void)  // clear the PM IRQ flag
{
    _sys_PM_IRQ = 0;
}


/****************************************************************************
* FUNCTION
*   _sysEnterPowerSavingMode
*
* DESCRIPTION
*   This function is use to enter idle or power down mode
*
* INPUTS
*   None
*
* OUTPUTS
*   None
*
****************************************************************************/
void _sysEnterPowerSavingMode(INT mode, UINT32 irqno)
{
	volatile UINT32 i;
		
    outpw(REG_AIC_MECR, irqno); /* interrupt source to return from power saving mode */            
    outpw(REG_PMCON, mode);     /* write control register to enter power saving mode */             
    for (i=0; i<0x100; i++) ;
}


/****************************************************************************
* FUNCTION
*   sysPMStart
*
* DESCRIPTION
*   This function is use to enter power saving mode.
*
* INPUTS
*   pd_type     WB_PM_IDLE/WB_PM_MIDLE/WB_PM_PD
*               The Power saving mode
*
* OUTPUTS
*   Successful          Enter power saving mode and wake up successfully
*   WB_PM_PD_IRQ_Fail   Enter PD mode without enable one of USB DEVICE, KPI, 
*                       RTC or nIRQ
*                       which supports to wake up the system from PD mode
*   WB_PM_Type_Fail     pd_type error
*   WB_PM_CACHE_OFF     cache is disabled that can not lock necessary 
*                       instructions into cache
*
****************************************************************************/
INT sysPMStart(INT pd_type)
{
    INT32    (*wb_func)(UINT32, INT32);
    BOOL     cache_status;
    WB_PLL_T _oldsysClk, _newsysClk;
    UINT32   aic_status = 0;

    if(pd_type != WB_PM_IDLE && pd_type != WB_PM_MIDLE && pd_type != WB_PM_PD)
    {
        return WB_PM_Type_Fail;
    }
    
    if(pd_type == WB_PM_PD)
    {
        // Enter PD mode, but IRQ setting error
        // one of nIRQ0~7, KPI, RTC or USBD_INT must enable when enter PD mode
        if (!(_sys_PM_IRQ & (1<<IRQ_GROUP0 | 1<<IRQ_GROUP1 | 1<<IRQ_USBD | 1<<IRQ_KPI | 1<<IRQ_RTC)))
        {
            return WB_PM_PD_IRQ_Fail;
        }
    }
    
    if ((pd_type == WB_PM_MIDLE) || (pd_type == WB_PM_PD))
    {
    	// Check cache status. We need to run the necessary instructions from cache to
    	// let CPU enter power saving mode.
    	cache_status = sysGetCacheState();
    	if (cache_status == FALSE)   return WB_PM_CACHE_OFF;
    }

    aic_status = inpw(REG_AIC_IMR);
    outpw(REG_AIC_MDCR, 0xFFFFFFFF);
    
    if ((pd_type == WB_PM_MIDLE) || (pd_type == WB_PM_PD))
    {
    	// Lock the power-down codes into cache
        wb_func = (INT32(*)(UINT32, INT32)) (0x80000000 | (UINT32) _sysLockCode);
        wb_func((UINT32)_sysEnterPowerSavingMode, 256);      		
    }

	// Select CPU clock from external clock source
	sysGetPLLConfig(&_oldsysClk);
    
    _newsysClk.pll0 = PLL_DISABLE;
	_newsysClk.pll1 = PLL_DISABLE;
	_newsysClk.cpu_src = CPU_FROM_EXTAL15M;
	_newsysClk.ahb_clk = AHB_CPUCLK_1_1;
	_newsysClk.apb_clk = APB_AHB_1_2;
	sysSetPLLConfig(&_newsysClk);

	// Entering Power saving mode
	_sysEnterPowerSavingMode(pd_type, _sys_PM_IRQ);

	// Restore CPU clock source
	sysSetPLLConfig(&_oldsysClk);

	if ((pd_type == WB_PM_MIDLE) || (pd_type == WB_PM_PD))
	 	_sysUnLockCode();
	 	
    outpw(REG_AIC_MDCR, 0xFFFFFFFF);    // Disable all interrupt
    outpw(REG_AIC_MECR, aic_status);    // Restore AIC setting
    return Successful;

}
