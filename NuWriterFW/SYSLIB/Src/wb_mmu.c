/***************************************************************************
 *                                                                         *
 * Copyright (c) 2008 Nuvoton Technolog. All rights reserved.              *
 *                                                                         *
 ***************************************************************************/ 
/****************************************************************************
 * 
 * FILENAME
 *     wb_mmu.c
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *     This file implement mmu functions.
 *
  * HISTORY
 *     2008-06-25  Ver 1.0 draft by Min-Nan Cheng
 *
 * REMARK
 *     None
 **************************************************************************/
#include "wblib.h"

#define  _CoarsePageSize 	64  //MB

typedef struct _coarse_table
{
	unsigned int page[256];
} _CTable;

__align(0x4000) unsigned int _mmuSectionTable[4096];
__align(1024) static _CTable _mmuCoarsePageTable[_CoarsePageSize]; 			// maximum 64MB for coarse pages
__align(1024) static _CTable _mmuCoarsePageTable_NonCache[_CoarsePageSize];	// Shadow SDRAM area for non-cacheable 

static BOOL _IsInitMMUTable = FALSE;
static int _MMUMappingMode = MMU_DIRECT_MAPPING;

extern INT32 sysGetSdramSizebyMB(void);
extern void sysSetupCP15(unsigned int);


unsigned int sysGetPhyPageAddr(unsigned int vaddr)
{
	int table_num, page_num;
	unsigned int base_addr, page_base, page_offset, phy_addr;
	volatile _CTable *PageTabPtr;
	
	if (vaddr & 0x80000000)
		PageTabPtr = (_CTable *) _mmuCoarsePageTable_NonCache; //non-cacheable virtual address
	else
		PageTabPtr = (_CTable *) _mmuCoarsePageTable;	//cache-able virtual address
		
	if (sysGetCacheState() == TRUE)
		PageTabPtr = (_CTable *) ((unsigned int)PageTabPtr | 0x80000000); //If cache is enable, must write page tables directly into SDRAM		
	
	base_addr = vaddr & 0x7FFFF000;
	table_num = base_addr / 0x100000;
	page_num = (base_addr & 0xFF000) >> 12;
	
	page_base = (*(PageTabPtr+table_num)).page[page_num] & 0xFFFFF000;
	page_offset = vaddr & 0xFFF;
	phy_addr = page_base + page_offset;

	return phy_addr;

} /* end sysGetPHYAddr */


int sysSetCachePages(unsigned int vaddr, int size, int cache_flag)
{
	int i, cnt, table_num, page_num, cache_mode;
	unsigned volatile int baseaddr, temp;
	volatile _CTable *PageTabPtr;
	
	if (vaddr & 0x80000000)
		PageTabPtr = (_CTable *) _mmuCoarsePageTable_NonCache; //non-cacheable virtual address
	else
		PageTabPtr = (_CTable *) _mmuCoarsePageTable;	//cache-able virtual address
		
	if (sysGetCacheState() == TRUE)
		PageTabPtr = (_CTable *) ((unsigned int)PageTabPtr | 0x80000000); //If cache is enable, must write page tables directly into SDRAM	
	
	vaddr &= 0x7FFFFFFF;	//ignore the non-cacheable bit 31	
	//if ( _IsInitMMUTable == FALSE ) return -1;	
	if ((vaddr + size) > (_CoarsePageSize << 20)) return -1;
	
	if (vaddr & 0xFFF) 	return -1;  /* MUST 4K Boundary */
	if (size % 4096)	return -1;  /* MUST 4K multiple size */		
		
	/* for flat mapping address */
	cnt = size / 4096;	

	if (cache_flag == CACHE_WRITE_BACK) /* write back mode */
		cache_mode = 0x0C; 
	else if (cache_flag == CACHE_WRITE_THROUGH) /* write through mode */
		cache_mode = 0x08; 
	else
		cache_mode = 0; /* Non-cacheable, non-buffered */
	
	for (i=0; i<cnt; i++)
	{
		baseaddr = vaddr + i * 4096;
		table_num = baseaddr / 0x100000;
		page_num =  (baseaddr & 0xFF000) >> 12; /* bits [19:12] for level two table index */
	
		temp = (*(PageTabPtr+table_num)).page[page_num] & 0xFFFFFFF3;			
		temp |= cache_mode; /* cache mode */			
		(*(PageTabPtr+table_num)).page[page_num] = temp;
	} 
	
	//sysFlushCache(D_CACHE);
	
	return 0;
	
} /* end sysSetCachePages */



int sysInitPageTable(unsigned int vaddr, unsigned int phy_addr, int size, int cache_flag, int rev_flag)
{
	int i, cnt, table_num, page_num, cache_mode, addr_offset;
	unsigned volatile int phy_base_addr, vbase_addr, temp;
	volatile _CTable *PageTabPtr;
	
	if (vaddr & 0x80000000)
		PageTabPtr = (_CTable *) _mmuCoarsePageTable_NonCache; //non-cacheable virtual address
	else
		PageTabPtr = (_CTable *) _mmuCoarsePageTable;	//cache-able virtual address
		
	if (sysGetCacheState() == TRUE)
		PageTabPtr = (_CTable *) ((unsigned int)PageTabPtr | 0x80000000); //If cache is enable, must write page tables directly into SDRAM		
	
	//if ( _IsInitMMUTable == FALSE ) return -1;	
	vaddr &= 0x7FFFFFFF;	//ignore the non-cacheable bit 31
	if ((vaddr + size) > (_CoarsePageSize << 20)) return -1;	
	if (vaddr & 0xFFFFF) 	return -1;  /* MUST 1M Boundary */
	if (size % 4096)	    return -1;  /* MUST 4K multiple size */		
						
	/* Pages count */
	cnt = size / 4096;	

	if (cache_flag == CACHE_WRITE_BACK) /* write back mode */
		cache_mode = 0x0C; 
	else if (cache_flag == CACHE_WRITE_THROUGH) /* write through mode */
		cache_mode = 0x08; 
	else
		cache_mode = 0; /* Non-cacheable, non-buffered */
		
		
	if (rev_flag == MMU_DIRECT_MAPPING)
		phy_base_addr = phy_addr;
	else
		phy_base_addr = phy_addr + size - 4096;		
	
	addr_offset = 4096;
	for (i=0; i<cnt; i++)
	{				
		vbase_addr = vaddr + i * 4096;
		table_num = vbase_addr / 0x100000;
		page_num =  (vbase_addr & 0xFF000) >> 12; /* bits [19:12] for level two table index */
	
		temp = phy_base_addr & 0xFFFFF000;
		temp |= 0xFF0; /* access permission, 11 for read/write */
		temp |= cache_mode; /* cache mode */
		temp |= 0x02;  /* small page */
		
		(*(PageTabPtr+table_num)).page[page_num] = temp;
		
		if (rev_flag == MMU_DIRECT_MAPPING)
			phy_base_addr += addr_offset;
		else
			phy_base_addr -= addr_offset;
	} 
		
	return 0;
	
} /* end sysInitPageTable */


int sysSetMMUMappingMethod(int mode)
{
	_MMUMappingMode = mode;
	
	return 0;

} /* end sysSetMMUMappingMethod */


int sysInitMMUTable(int cache_mode)
{
	unsigned volatile int temp;
	int i, size, ramsize;		
	
	if (_IsInitMMUTable == FALSE)		
	{
		ramsize = sysGetSdramSizebyMB();
		
		//flat mapping for 4GB, 4096 section table, each size is 1MB
		temp = 0xC00;   /* (11:10) access permission, R/W */
		temp |= 0x1E0;  /* (8:5) domain 15 */
		temp |= 0x10;   /* bit 4 must be 1 */
		temp |= 0x00;   /* bit 3:2 for cache control bits, cache disabled */
		temp |= 0x02;   /* set as 1Mb section */
  
		for (i=0; i<4096; i++)
		{
       		_mmuSectionTable[i] = (unsigned int)(temp | (i << 20));    
		}
  
	    //Inside SDRAM, divide each section into 256 small pages, each page size is 4KB
	    if (ramsize > _CoarsePageSize) size = _CoarsePageSize;	//maximum 64MB
	    else						   size = ramsize;
	
		/* first 1M always direct mapping */
		sysInitPageTable(0, 0, 0x100000, cache_mode, MMU_DIRECT_MAPPING);
		temp = ((unsigned int)_mmuCoarsePageTable  & 0xFFFFFC00); /*  coarse table base address */		
		temp |= 0x1E0;  /* (8:5) domain 15 */
		temp |= 0x10;   /* bit 4 must be 1 */
		temp |= 0x01;   /* Coarse page table */
		_mmuSectionTable[0] = temp;
		
		/* Create a shadow area at 0x80000000 for non-cacheable region */
		sysInitPageTable(0x80000000, 0x0, 0x100000, CACHE_DISABLE, MMU_DIRECT_MAPPING);
		temp = ((unsigned int)_mmuCoarsePageTable_NonCache  & 0xFFFFFC00); /*  coarse table base address */
		temp |= 0x1E0;  /* (8:5) domain 15 */
		temp |= 0x10;   /* bit 4 must be 1 */
		temp |= 0x01;   /* Coarse page table */
		_mmuSectionTable[0x800] = temp;
		
		/* Mapping the other memory */
		for (i=1; i< size; i++)
		{
			temp = (((unsigned int)_mmuCoarsePageTable + (unsigned int)i*1024) & 0xFFFFFC00); /*  coarse table base address */
			//temp = ((unsigned int)(0x604000 + i*1024) & 0xFFFFFC00); /* coarse table base address */
			temp |= 0x1E0;  /* (8:5) domain 15 */
			temp |= 0x10;   /* bit 4 must be 1 */
			temp |= 0x01;   /* Coarse page table */

			if (_MMUMappingMode == MMU_DIRECT_MAPPING)		
				sysInitPageTable((i << 20), (i << 20), 0x100000, cache_mode, MMU_DIRECT_MAPPING); /* direct mapping */
			else
				sysInitPageTable((i << 20), (i << 20), 0x100000, cache_mode, MMU_INVERSE_MAPPING); /* inverse mapping for each 1MB area */
				
			_mmuSectionTable[i] = temp;			
		}
				
		//Create shadow non-cacheabel region
		for (i=1; i< size; i++)
		{
			temp = (((unsigned int)_mmuCoarsePageTable_NonCache + (unsigned int)i*1024) & 0xFFFFFC00); /*  coarse table base address */
			//temp = ((unsigned int)(0x604000 + i*1024) & 0xFFFFFC00); /* coarse table base address */
			temp |= 0x1E0;  /* (8:5) domain 15 */
			temp |= 0x10;   /* bit 4 must be 1 */
			temp |= 0x01;   /* Coarse page table */

			if (_MMUMappingMode == MMU_DIRECT_MAPPING)		
				sysInitPageTable(((i << 20) | 0x80000000), (i << 20), 0x100000, CACHE_DISABLE, MMU_DIRECT_MAPPING); /* direct mapping */
			else
				sysInitPageTable(((i << 20) | 0x80000000), (i << 20), 0x100000, CACHE_DISABLE, MMU_INVERSE_MAPPING); /* inverse mapping for each 1MB area */
				
			_mmuSectionTable[0x800+i] = temp;			
		}
										
		_IsInitMMUTable = TRUE;
 	}
 	
 	//moved here by cmn [2007/01/27]
 	//set CP15 registers
	sysSetupCP15((unsigned int)_mmuSectionTable);
 	
 	return 0;
	
} /* end sysInitMMUTable */

