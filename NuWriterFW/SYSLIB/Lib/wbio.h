/***************************************************************************
 *                                                                         *
 * Copyright (c) 2008 Nuvoton Technology. All rights reserved.             *
 *                                                                         *
 ***************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *     WBIO.h
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *     This file contains I/O macros, and basic macros and inline functions.
 *
 * DATA STRUCTURES
 *     None
 *
 * FUNCTIONS
 *     None
 *
 * HISTORY
 *     03/28/02		 Ver 1.0 Created by PC30 YCHuang
 *
 * REMARK
 *     None
 **************************************************************************/

#ifndef _WBIO_H
#define _WBIO_H

#ifndef _WBTYPES_H_
#include "wbtypes.h"
#endif

/* W90N960 */
#define LITTLE_ENDIAN


#define outpb(port,value)     (*((UINT8 volatile *) (port))=value)
#define inpb(port)            (*((UINT8 volatile *) (port)))
#define outphw(port,value)    (*((UINT16 volatile *) (port))=value)
#define inphw(port)           (*((UINT16 volatile *) (port)))
#define outpw(port,value)     (*((UINT32 volatile *) (port))=value)
#define inpw(port)            (*((UINT32 volatile *) (port)))

#define readb(addr)           (*(UINT8 volatile *)(addr))
#define writeb(addr,x)        ((*(UINT8 volatile *)(addr)) = (UINT8 volatile)x)
#define readhw(addr)          (*(UINT16 volatile *)(addr))
#define writehw(addr,x)       ((*(UINT16 volatile *)(addr)) = (UINT16 volatile)x)
#define readw(addr)           (*(UINT32 volatile *)(addr))
#define writew(addr,x)        ((*(UINT32 volatile *)(addr)) = (UINT32 volatile)x)

#define Maximum(a,b)          (a>b ? a : b)
#define Minimum(a,b)          (a<b ? a : b)
#define Middle(a,b)           ((a+b)/2)
#define Equal(a,b)            (a==b ? 1 : 0)
#define NotEqual(a,b)         (a==b ? 0 : 1)
#define GreaterEqual(a,b)     (a>=b ? 1 : 0)
#define LittleEqual(a,b)      (a<=b ? 1 : 0) 

static __inline UINT16 Swap16(UINT16 val)
{
    return (val<<8) | (val>>8);
}

static __inline UINT32 Swap32(UINT32 val)
{
    return (val<<24) | ((val<<8)&0xff0000) | ((val>>8)&0xff00) | (val>>24);
}

static __inline UINT16 Get16(PUINT8 addr)
{
#ifdef LITTLE_ENDIAN
    return ((addr[1]<<8) | addr[0]);
#else	
    return ((addr[0]<<8) | addr[1]);
#endif    
}

static __inline UINT32 Get32(PUINT8 addr)
{
#ifdef LITTLE_ENDIAN
    return (addr[3]<<24) | (addr[2]<<16) | (addr[1]<<8) | addr[0];
#else
    return (addr[0]<<24) | (addr[1]<<16) | (addr[2]<<8) | addr[3];
#endif
}

static __inline void Set16(UINT16 val, PUINT8 addr)
{
#ifdef LITTLE_ENDIAN
    addr[0] = (UINT8) val;
    addr[1] = (UINT8)(val >> 8);
#else
    addr[0] = (UINT8)(val >> 8);
    addr[1] = (UINT8) val;
#endif    
}

static __inline void Set32(UINT32 val, PUINT8 addr)
{
#ifdef LITTLE_ENDIAN
    addr[0] = (UINT8) val;
    addr[1] = (UINT8)(val >> 8);
    addr[2] = (UINT8)(val >> 16);
    addr[3] = (UINT8)(val >> 24);
#else
    addr[0] = (UINT8)(val >> 24);
    addr[1] = (UINT8)(val >> 16);
    addr[2] = (UINT8)(val >> 8);
    addr[3] = (UINT8) val & 0xff;
#endif    
}

#endif /* _WBIO_H */

