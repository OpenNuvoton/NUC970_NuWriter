/***************************************************************************
 *                                                                         *
 * Copyright (c) 2012 Nuvoton Technolog. All rights reserved.              *
 *                                                                         *
 ***************************************************************************/
 
/****************************************************************************
* FILENAME
*   wb_aic.c
*
* VERSION
*   1.0
*
* DESCRIPTION
*   The AIC related functions of Nuvoton ARM9 MCU
*
* HISTORY
*   2008-06-25  Ver 1.0 draft by Min-Nan Cheng
*   2012-10-18  Ver 2.0 for NUC970
*
* REMARK
*   None
 **************************************************************************/
#include <stdio.h>
#include "wblib.h"

#define WB_MIN_INT_SOURCE  		1
#define WB_MAX_INT_SOURCE  		56
#define WB_NUM_OF_AICREG   		16

/* Global variables */
BOOL volatile _sys_bIsAICInitial = FALSE;
BOOL volatile _sys_bIsHWMode = TRUE;

/* declaration the function prototype */
extern void WB_Interrupt_Shell(void);

/* Define AIC source control register address */
UINT32 WB_AICRegAddr[WB_NUM_OF_AICREG] = {
				REG_AIC_SCR1,		REG_AIC_SCR2,		REG_AIC_SCR3,		REG_AIC_SCR4,
				REG_AIC_SCR5,		REG_AIC_SCR6,		REG_AIC_SCR7,		REG_AIC_SCR8,								
				REG_AIC_SCR9,		REG_AIC_SCR10,	REG_AIC_SCR11,	REG_AIC_SCR12,
				REG_AIC_SCR13,	REG_AIC_SCR14,	REG_AIC_SCR15
				};

/* Interrupt Handler Table */
typedef void (*sys_pvFunPtr)();   /* function pointer */
sys_pvFunPtr sysIrqHandlerTable[] = { 0,                /* 0 */
								  WB_Interrupt_Shell,	/* 1 */
								  WB_Interrupt_Shell,	/* 2 */
								  WB_Interrupt_Shell,	/* 3 */
								  WB_Interrupt_Shell,	/* 4 */
								  WB_Interrupt_Shell,	/* 5 */
								  WB_Interrupt_Shell,	/* 6 */
								  WB_Interrupt_Shell,	/* 7 */
								  WB_Interrupt_Shell,	/* 8 */
								  WB_Interrupt_Shell,	/* 9 */
								  WB_Interrupt_Shell,	/* 10 */
								  WB_Interrupt_Shell,	/* 11 */
								  WB_Interrupt_Shell,	/* 12 */
								  WB_Interrupt_Shell,	/* 13 */
								  WB_Interrupt_Shell,	/* 14 */
								  WB_Interrupt_Shell,	/* 15 */
								  WB_Interrupt_Shell,	/* 16 */
								  WB_Interrupt_Shell,	/* 17 */
								  WB_Interrupt_Shell,	/* 18 */
								  WB_Interrupt_Shell,	/* 19 */
								  WB_Interrupt_Shell,	/* 20 */
								  WB_Interrupt_Shell,	/* 21 */
								  WB_Interrupt_Shell,	/* 22 */
								  WB_Interrupt_Shell,	/* 23 */
								  WB_Interrupt_Shell,	/* 24 */
								  WB_Interrupt_Shell,	/* 25 */
								  WB_Interrupt_Shell,	/* 26 */
								  WB_Interrupt_Shell,	/* 27 */
								  WB_Interrupt_Shell,	/* 28 */
								  WB_Interrupt_Shell,	/* 29 */
								  WB_Interrupt_Shell,	/* 30 */
								  WB_Interrupt_Shell,	/* 31 */
								  WB_Interrupt_Shell,	/* 32 */
								  WB_Interrupt_Shell,	/* 33 */
								  WB_Interrupt_Shell,	/* 34 */
								  WB_Interrupt_Shell,	/* 35 */
								  WB_Interrupt_Shell,	/* 36 */
								  WB_Interrupt_Shell,	/* 37 */
								  WB_Interrupt_Shell,	/* 38 */
								  WB_Interrupt_Shell,	/* 39 */
								  WB_Interrupt_Shell,	/* 40 */
								  WB_Interrupt_Shell,	/* 41 */
								  WB_Interrupt_Shell,	/* 42 */
								  WB_Interrupt_Shell,	/* 43 */
								  WB_Interrupt_Shell,	/* 44 */
								  WB_Interrupt_Shell,	/* 45 */
								  WB_Interrupt_Shell,	/* 46 */
								  WB_Interrupt_Shell,	/* 47 */
								  WB_Interrupt_Shell,	/* 48 */
								  WB_Interrupt_Shell,	/* 49 */
								  WB_Interrupt_Shell,	/* 50 */
								  WB_Interrupt_Shell,	/* 51 */
								  WB_Interrupt_Shell,	/* 52 */
								  WB_Interrupt_Shell,	/* 53 */
								  WB_Interrupt_Shell,	/* 54 */
								  WB_Interrupt_Shell,	/* 55 */
								  WB_Interrupt_Shell	/* 56 */
								};

sys_pvFunPtr sysFiqHandlerTable[] = { 0,
								  WB_Interrupt_Shell,	/* 1 */
								  WB_Interrupt_Shell,	/* 2 */
								  WB_Interrupt_Shell,	/* 3 */
								  WB_Interrupt_Shell,	/* 4 */
								  WB_Interrupt_Shell,	/* 5 */
								  WB_Interrupt_Shell,	/* 6 */
								  WB_Interrupt_Shell,	/* 7 */
								  WB_Interrupt_Shell,	/* 8 */
								  WB_Interrupt_Shell,	/* 9 */
								  WB_Interrupt_Shell,	/* 10 */
								  WB_Interrupt_Shell,	/* 11 */
								  WB_Interrupt_Shell,	/* 12 */
								  WB_Interrupt_Shell,	/* 13 */
								  WB_Interrupt_Shell,	/* 14 */
								  WB_Interrupt_Shell,	/* 15 */
								  WB_Interrupt_Shell,	/* 16 */
								  WB_Interrupt_Shell,	/* 17 */
								  WB_Interrupt_Shell,	/* 18 */
								  WB_Interrupt_Shell,	/* 19 */
								  WB_Interrupt_Shell,	/* 20 */
								  WB_Interrupt_Shell,	/* 21 */
								  WB_Interrupt_Shell,	/* 22 */
								  WB_Interrupt_Shell,	/* 23 */
								  WB_Interrupt_Shell,	/* 24 */
								  WB_Interrupt_Shell,	/* 25 */
								  WB_Interrupt_Shell,	/* 26 */
								  WB_Interrupt_Shell,	/* 27 */
								  WB_Interrupt_Shell,	/* 28 */
								  WB_Interrupt_Shell,	/* 29 */
								  WB_Interrupt_Shell,	/* 30 */
								  WB_Interrupt_Shell,	/* 31 */
								  WB_Interrupt_Shell,	/* 32 */
								  WB_Interrupt_Shell,	/* 33 */
								  WB_Interrupt_Shell,	/* 34 */
								  WB_Interrupt_Shell,	/* 35 */
								  WB_Interrupt_Shell,	/* 36 */
								  WB_Interrupt_Shell,	/* 37 */
								  WB_Interrupt_Shell,	/* 38 */
								  WB_Interrupt_Shell,	/* 39 */
								  WB_Interrupt_Shell,	/* 40 */
								  WB_Interrupt_Shell,	/* 41 */
								  WB_Interrupt_Shell,	/* 42 */
								  WB_Interrupt_Shell,	/* 43 */
								  WB_Interrupt_Shell,	/* 44 */
								  WB_Interrupt_Shell,	/* 45 */
								  WB_Interrupt_Shell,	/* 46 */
								  WB_Interrupt_Shell,	/* 47 */
								  WB_Interrupt_Shell,	/* 48 */
								  WB_Interrupt_Shell,	/* 49 */
								  WB_Interrupt_Shell,	/* 50 */
								  WB_Interrupt_Shell,	/* 51 */
								  WB_Interrupt_Shell,	/* 52 */
								  WB_Interrupt_Shell,	/* 53 */
								  WB_Interrupt_Shell,	/* 54 */
								  WB_Interrupt_Shell,	/* 55 */
								  WB_Interrupt_Shell	/* 56 */
								};

/* Interrupt Handler */
__irq void sysIrqHandler()
{
	if (_sys_bIsHWMode)
	{
		UINT32 volatile _mIPER, _mISNR;

		_mIPER = (inpw(REG_AIC_IPER) >> 2) & 0x3f;
		_mISNR = inpw(REG_AIC_ISNR);
		if (_mISNR != 0)
			if (_mIPER == _mISNR)
				(*sysIrqHandlerTable[_mISNR])();
		outpw(REG_AIC_EOSCR, 1);
	}
	else
	{
		UINT32 volatile _mISR, _mISRH, i;

		_mISR = inpw(REG_AIC_ISR);
		_mISRH = inpw(REG_AIC_ISRH);
		
		for (i = 1; i <= 31; i++)
			if (_mISR & (1 << i))
				(*sysIrqHandlerTable[i])();

		for (i = 32; i <= WB_MAX_INT_SOURCE; i++)
			if (_mISRH & (1 << (i-32)))
				(*sysIrqHandlerTable[i])();
	}
}

__irq void sysFiqHandler()
{
	if (_sys_bIsHWMode)
	{
		UINT32 volatile _mIPER, _mISNR;
  
		_mIPER = (inpw(REG_AIC_IPER) >> 2) & 0x3f;
		_mISNR = inpw(REG_AIC_ISNR);
		sysprintf("ISNR = %d\n", _mISNR);
		if (_mISNR != 0)
			if (_mIPER == _mISNR)
				(*sysFiqHandlerTable[_mISNR])();
		outpw(REG_AIC_EOSCR, 1);
	}
	else
	{
		UINT32 volatile _mISR, _mISRH, i;

		_mISR = inpw(REG_AIC_ISR);
		_mISRH = inpw(REG_AIC_ISRH);
		
		for (i = 1; i <= 31; i++)
			if (_mISR & (1 << i))
				(*sysFiqHandlerTable[i])();

		for (i = 32; i <= WB_MAX_INT_SOURCE; i++)
			if (_mISRH & (1 << (i-32)))
				(*sysFiqHandlerTable[i])();
	}
}

void WB_Interrupt_Shell()
{
	sysprintf("ISR not found! ISNR=%d\n", inpw(REG_AIC_ISNR));
}

void sysInitializeAIC()
{
//	PVOID _mOldIrqVect, _mOldFiqVect;

//	_mOldIrqVect = *(PVOID volatile *)0x38;
	*(PVOID volatile *)0x38 = (PVOID volatile)sysIrqHandler;

//	_mOldFiqVect = *(PVOID volatile *)0x3C;
	*(PVOID volatile *)0x3C = (PVOID volatile)sysFiqHandler;
}


/* Interrupt library functions */
INT32 sysDisableInterrupt(INT_SOURCE_E eIntNo)
{
	if ((eIntNo > WB_MAX_INT_SOURCE) || (eIntNo < WB_MIN_INT_SOURCE))
		return Fail;

	if (eIntNo < 32)
		outpw(REG_AIC_MDCR, (1 << eIntNo));
	else
		outpw(REG_AIC_MDCRH, (1 << (eIntNo - 32)));
	
	return Successful;
}


INT32 sysEnableInterrupt(INT_SOURCE_E eIntNo)
{
   if ((eIntNo > WB_MAX_INT_SOURCE) || (eIntNo < WB_MIN_INT_SOURCE))
	  return Fail;

	if (eIntNo < 32)
		outpw(REG_AIC_MECR, (1 << eIntNo));
	else
		outpw(REG_AIC_MECRH, (1 << (eIntNo - 32)));

	return Successful;
}


PVOID sysInstallExceptionHandler(INT32 nExceptType, PVOID pvNewHandler)
{
   PVOID _mOldVect;
   
   switch (nExceptType)
   {
	  case WB_SWI:
		   _mOldVect = *(PVOID volatile *)0x28;
		   *(PVOID volatile *)0x28 = pvNewHandler;
		   break;

	  case WB_D_ABORT:
		   _mOldVect = *(PVOID volatile *)0x30;
		   *(PVOID volatile *)0x30 = pvNewHandler;
		   break;

	  case WB_I_ABORT:
		   _mOldVect = *(PVOID volatile *)0x2C;
		   *(PVOID volatile *)0x2C = pvNewHandler;
		   break;

	  case WB_UNDEFINE:
		   _mOldVect = *(PVOID volatile *)0x24;
		   *(PVOID volatile *)0x24 = pvNewHandler;
		   break;

	  default:
		   ;
   }
   return _mOldVect;
}

PVOID sysInstallFiqHandler(PVOID pvNewISR)
{
	PVOID _mOldVect;
   
	_mOldVect = *(PVOID volatile *)0x3C;
	*(PVOID volatile *)0x3C = pvNewISR;
	return _mOldVect;
}

PVOID sysInstallIrqHandler(PVOID pvNewISR)
{
	PVOID _mOldVect;
   
	_mOldVect = *(PVOID volatile *)0x38;
	*(PVOID volatile *)0x38 = pvNewISR;
	return _mOldVect;
}


PVOID sysInstallISR(INT32 nIntTypeLevel, INT_SOURCE_E eIntNo, PVOID pvNewISR)
{
	PVOID  	_mOldVect;
	UINT32 	_mRegAddr/*, _mRegValue*/;
	INT		shift;

	if (!_sys_bIsAICInitial)
	{
		sysInitializeAIC();
		_sys_bIsAICInitial = TRUE;
	}
   
	_mRegAddr = REG_AIC_SCR1 + ((eIntNo / 4) * 4); 
	shift = (eIntNo % 4) * 8;
	nIntTypeLevel &= 0xff;
	outpw(_mRegAddr, (inpw(_mRegAddr) & ~(0x07 << shift)) | (nIntTypeLevel << shift));

	if ((nIntTypeLevel & 0x7) == FIQ_LEVEL_0)
	{
		_mOldVect = (PVOID) sysFiqHandlerTable[eIntNo];
		sysFiqHandlerTable[eIntNo] = (sys_pvFunPtr)pvNewISR;
	}
	else
	{
	   _mOldVect = (PVOID) sysIrqHandlerTable[eIntNo];
	   sysIrqHandlerTable[eIntNo] = (sys_pvFunPtr)pvNewISR;
	}
   	return _mOldVect;
}


INT32 sysSetGlobalInterrupt(INT32 nIntState)
{
   switch (nIntState)
   {
	  case ENABLE_ALL_INTERRUPTS:
		   outpw(REG_AIC_MECR, 0xFFFFFFFF);
		   outpw(REG_AIC_MECRH, 0xFFFFFFFF);
		   break;

	  case DISABLE_ALL_INTERRUPTS:
		   outpw(REG_AIC_MDCR, 0xFFFFFFFF);
		   outpw(REG_AIC_MDCRH, 0xFFFFFFFF);
		   break;

	  default:
		   ;
   }
   return Successful;
}


INT32 sysSetInterruptPriorityLevel(INT_SOURCE_E eIntNo, UINT32 uIntLevel)
{
   	UINT32 	_mRegAddr/*, _mRegValue*/;
   	INT		shift;

   if ((eIntNo > WB_MAX_INT_SOURCE) || (eIntNo < WB_MIN_INT_SOURCE))
	  return Fail;

	_mRegAddr = REG_AIC_SCR1 + ((eIntNo / 4) * 4); 
	shift = (eIntNo % 4) * 8;
	uIntLevel &= 0x7;
	outpw(_mRegAddr, (inpw(_mRegAddr) & ~(0x07 << shift)) | (uIntLevel << shift));

   	return Successful;
}


INT32 sysSetInterruptType(INT_SOURCE_E eIntNo, UINT32 uIntSourceType)
{
   	UINT32 _mRegAddr/*, _mRegValue*/;
   	INT		shift;

   	if ((eIntNo > WB_MAX_INT_SOURCE) || (eIntNo < WB_MIN_INT_SOURCE))
	  	return Fail;

	_mRegAddr = REG_AIC_SCR1 + ((eIntNo / 4) * 4); 
	shift = (eIntNo % 4) * 8;
	uIntSourceType &= 0xC0;
	outpw(_mRegAddr, (inpw(_mRegAddr) & ~(0xC0 << shift)) | (uIntSourceType << shift));

   	return Successful;
}


INT32 sysSetLocalInterrupt(INT32 nIntState)
{
   INT32 temp;

   switch (nIntState)
   {
	  case ENABLE_IRQ:
	  case ENABLE_FIQ:
	  case ENABLE_FIQ_IRQ:
		   __asm
		   {
			   MRS    temp, CPSR
			   AND    temp, temp, nIntState
			   MSR    CPSR_c, temp
		   }
		   break;

	  case DISABLE_IRQ:
	  case DISABLE_FIQ:
	  case DISABLE_FIQ_IRQ:
		   __asm
		   {
			   MRS    temp, CPSR
			   ORR    temp, temp, nIntState
			   MSR    CPSR_c, temp
		   }
		   break;

	  default:
		   ;
   }
   return Successful;
}

INT32 sysSetAIC2SWMode()
{
	_sys_bIsHWMode = FALSE;
	return Successful;
}


UINT32	sysGetInterruptEnableStatus(void)
{
	return (inpw(REG_AIC_IMR));
}


UINT32	sysGetInterruptEnableStatusH(void)
{
	return (inpw(REG_AIC_IMRH));
}


BOOL sysGetIBitState()
{
	INT32 temp;

	__asm
	{
		MRS	temp, CPSR
	}

	if (temp & 0x80)
		return FALSE;
	else
		return TRUE;
}


