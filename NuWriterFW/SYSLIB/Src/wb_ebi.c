/****************************************************************************
*                                                                           *
* Copyright (c) 2008 Nuvoton Technolog. All rights reserved.                *
*                                                                           *
*****************************************************************************/

/****************************************************************************
* FILENAME
*   wb_ebi.c
*
* VERSION
*   1.0
*
* DESCRIPTION
*   The EBI related functions of Nuvoton ARM9 MCU
*
* HISTORY
*   2008-06-25  Ver 1.0 draft by Min-Nan Cheng
*
* REMARK
*   None
****************************************************************************/
#include <stdio.h>
#include "wblib.h"

INT32 sysSetExternalIO(INT extNo, UINT32 extBaseAddr, UINT32 extSize, INT extBusWidth)
{
    UINT32 volatile reg, extiobase;

	switch (extNo)
	{
		case EXT0:
					extiobase = REG_EXT0CON;
					break;
		case EXT1:
					extiobase = REG_EXT1CON;
					break;
		case EXT2:
					extiobase = REG_EXT2CON;
					break;			
		case EXT3:
					extiobase = REG_EXT3CON;
					break;
		case EXT4:
					extiobase = REG_EXT4CON;
					break;
		default:
					return Fail;
	
	}
	
	reg = inpw(extiobase);
    
    // Set Bus width
    switch (extBusWidth)
    {
    	case BUS_BIT_8:
                    reg = (reg & 0xfffffffc) | 0x01;
                    break;

        case BUS_BIT_16:
                    reg = (reg & 0xfffffffc) | 0x02;
                    break;

        case BUS_BIT_32:
                    reg = (reg & 0xfffffffc) | 0x03;
                    break;

        case BUS_DISABLE:
                    reg = reg & 0xfffffffc;
                    break;
    }

    // Set size
    switch (extSize)
    {
    	case SIZE_256K:
                    reg = reg & 0xfff8ffff;
                    break;

        case SIZE_512K:
                    reg = (reg & 0xfff8ffff)|0x00010000;
                    break;

        case SIZE_1M:
                    reg = (reg & 0xfff8ffff)|0x00020000;
                    break;

        case SIZE_2M:
                    reg = (reg & 0xfff8ffff)|0x00030000;
                    break;

        case SIZE_4M:
                    reg = (reg & 0xfff8ffff)|0x00040000;
                    break;

        case SIZE_8M:
                    reg = (reg & 0xfff8ffff)|0x00050000;
                    break;

        case SIZE_16M:
                    reg = (reg & 0xfff8ffff)|0x00060000;
                    break;

        case SIZE_32M:
                    reg = (reg & 0xfff8ffff)|0x00070000;
                    break;
    }

    // Set Base address
    extBaseAddr = (extBaseAddr << 1) & 0xfff80000;
    reg = reg | extBaseAddr;

    // set the reg value into register
    outpw(extiobase, reg);
    
    return Successful;	
}

INT32 sysSetExternalIOTiming1(INT extNo, INT tACC, INT tACS)
{
    UINT32 volatile reg, extiobase;

	switch (extNo)
	{
		case EXT0:
					extiobase = REG_EXT0CON;
					break;
		case EXT1:
					extiobase = REG_EXT1CON;
					break;
		case EXT2:
					extiobase = REG_EXT2CON;
					break;			
		case EXT3:
					extiobase = REG_EXT3CON;
					break;
		case EXT4:
					extiobase = REG_EXT4CON;
					break;
		default:
					return Fail;
	
	}

   	reg = inpw(extiobase);
    
    if ((tACC >= 0) && (tACC <= 0xf)){
                reg = (reg & 0xffff87ff) | (tACC << 11);
    }
  
            
    if ((tACS >= 0) && (tACS <= 0x7)){
                reg = (reg & 0xffffff1f) | (tACS << 5);
    }
    
    outpw(extiobase, reg);
    
    return Successful;
         
}

INT32 sysSetExternalIOTiming2(INT extNo, INT tCOH, INT tCOS)
{
    UINT32 volatile reg, extiobase;

	switch (extNo)
	{
		case EXT0:
					extiobase = REG_EXT0CON;
					break;
		case EXT1:
					extiobase = REG_EXT1CON;
					break;
		case EXT2:
					extiobase = REG_EXT2CON;
					break;			
		case EXT3:
					extiobase = REG_EXT3CON;
					break;
		case EXT4:
					extiobase = REG_EXT4CON;
					break;
		default:
					return Fail;
	
	}

    reg = inpw(extiobase);

    if ((tCOH >= 0) && (tCOH <= 0x7))
            reg = (reg & 0xfffff8ff) | (tCOH << 8);

    if ((tCOS >= 0) && (tCOS <= 0x7))
            reg = (reg & 0xffffffe3) | (tCOS << 2);

    outpw(extiobase, reg);
    
    return Successful;
           
}
