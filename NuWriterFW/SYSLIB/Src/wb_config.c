
/***************************************************************************
 *                                                                         *
 * Copyright (c) 2008 Nuvoton Technolog. All rights reserved.              *
 *                                                                         *
 ***************************************************************************/
/****************************************************************************
 *
 * FILENAME : wb_config.c
 *
 * VERSION  : 1.0
 *
 * DESCRIPTION : 
 *               PLL control functions of Nuvoton ARM9 MCU
 *
 * HISTORY
 *               2008-06-25  Ver 1.0 draft by Min-Nan Cheng
 *
 ****************************************************************************/
#include "wblib.h"

WB_PLL_T	_sysClock;


int sysSetPLLConfig(WB_PLL_T *sysClk)
{
#if 0
  unsigned volatile int loop;
    
  outpw(REG_CLKSEL,  inpw(REG_CLKSEL)|CPU_FROM_EXTAL15M); /* system clock source is from external clock */  

  outpw(REG_PLLCON0, sysClk->pll0);	//PLL0 output
  outpw(REG_PLLCON1, sysClk->pll1);	//PLL1 output
  
  outpw(REG_CLKDIV, inpw(REG_CLKDIV)|(sysClk->ahb_clk<<24)|(sysClk->apb_clk<<26)); //clcok divider
  
  for (loop=0; loop<0x10000; loop++) ; //waits for 1~2 ms
  
  if (sysClk->cpu_src == CPU_FROM_PLL0)
  {
  		//if (sysClk->pll0 == PLL_200MHZ)
  		//	outpw(REG_CKSKEW, (inpw(REG_CKSKEW)&0xFFFFFF00)|0x64);
  		if ((sysClk->pll0 == PLL_133MHZ) && (sysClk->ahb_clk == AHB_CPUCLK_1_1))
  			outpw(REG_CKSKEW, (inpw(REG_CKSKEW)&0xFFFFFF00)|0x44);
  		else
  			outpw(REG_CKSKEW, (inpw(REG_CKSKEW)&0xFFFFFF00)|0x48);
  
  }
  else if (sysClk->cpu_src == CPU_FROM_PLL1)
  {
		//if (sysClk->pll1 == PLL_200MHZ)
  		//	outpw(REG_CKSKEW, (inpw(REG_CKSKEW)&0xFFFFFF00)|0x64);
  		//else
  		if ((sysClk->pll1 == PLL_133MHZ) && (sysClk->ahb_clk == AHB_CPUCLK_1_1))
  			outpw(REG_CKSKEW, (inpw(REG_CKSKEW)&0xFFFFFF00)|0x44);
  		else
  			outpw(REG_CKSKEW, (inpw(REG_CKSKEW)&0xFFFFFF00)|0x48);
  
  }
  else
  	outpw(REG_CKSKEW, (inpw(REG_CKSKEW)&0xFFFFFF00)|0x48);
  
  
/*  
  if ((sysClk->cpu_src == CPU_FROM_PLL0) && (sysClk->pll0 == PLL_200MHZ))
  		outpw(REG_CKSKEW, (inpw(REG_CKSKEW)&0xFFFFFF00)|0x64);
  else if ((sysClk->cpu_src == CPU_FROM_PLL1) && (sysClk->pll1 == PLL_200MHZ))
  		outpw(REG_CKSKEW, (inpw(REG_CKSKEW)&0xFFFFFF00)|0x64);
  else
  		outpw(REG_CKSKEW, (inpw(REG_CKSKEW)&0xFFFFFF00)|0x68);
*/
  outpw(REG_CLKSEL, (inpw(REG_CLKSEL)&0xFFFFFFFC)|sysClk->cpu_src);
  
  /* added by cmn [09/11/2007] */
  _sysClock.pll0 = sysClk->pll0;
  _sysClock.pll1 = sysClk->pll1;
  _sysClock.cpu_src = sysClk->cpu_src;
  _sysClock.ahb_clk = sysClk->ahb_clk;
  _sysClock.apb_clk = sysClk->apb_clk;
#endif
  
  return 0;
  
} /* end sysSetPLLConfig */



int sysGetPLLConfig(WB_PLL_T *sysClk)
{

  sysClk->pll0 = _sysClock.pll0;
  sysClk->pll1 = _sysClock.pll1;
  sysClk->cpu_src = _sysClock.cpu_src;
  sysClk->ahb_clk = _sysClock.ahb_clk;
  sysClk->apb_clk = _sysClock.apb_clk;

  return 0;

} /* end sysGetPLLConfig */
