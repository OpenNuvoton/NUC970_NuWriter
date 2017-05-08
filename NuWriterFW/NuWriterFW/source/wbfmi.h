#ifndef _FMILIB_H_
#define _FMILIB_H_

#define FMI_SD_CARD0	0
#define FMI_SD_CARD1	1
#define FMI_SM_CARD		2
#define FMI_MS_CARD0	3
#define FMI_MS_CARD1	4

#define FMI_MS_TYPE		0
#define FMI_MSPRO_TYPE	1

#define FMI_SD_WRITE_PROTECT	-1
#define FMI_NO_SD_CARD			-2
#define FMI_NO_SM_CARD			-3
#define FMI_SD_INIT_ERROR		-4
#define FMI_SM_INIT_ERROR		-5
#define FMI_SD_CRC7_ERROR		-6
#define FMI_SD_CRC16_ERROR		-7
#define FMI_SD_CRC_ERROR		-8
#define FMI_SM_STATE_ERROR		-9
#define FMI_SM_ECC_ERROR		-10
#define FMI_ERR_DEVICE			-11
#define FMI_TIMEOUT				-12
#define FMI_SD_INIT_TIMEOUT		-13
#define FMI_SD_SELECT_ERROR		-14
#define FMI_MS_INIT_ERROR		-15
#define FMI_MS_INT_TIMEOUT		-16
#define FMI_MS_BUSY_TIMEOUT		-17
#define FMI_MS_CRC_ERROR		-18
#define FMI_MS_INT_CMDNK		-19
#define FMI_MS_INT_ERR			-20
#define FMI_MD_INT_BREQ			-21
#define FMI_MS_INT_CED_ERR		-22
#define FMI_MS_READ_PAGE_ERROR	-23
#define FMI_MS_COPY_PAGE_ERR	-24
#define FMI_MS_ALLOC_ERR		-25
#define FMI_MS_WRONG_SEGMENT	-26
#define FMI_MS_WRONG_PHYBLOCK	-27
#define FMI_MS_WRONG_TYPE		-28
#define FMI_MS_WRITE_DISABLE	-29
#define FMI_NO_MS_CARD			-30
#define FMI_SM_RB_ERR			-31
#define FMI_SM_STATUS_ERR		-32

#define FMI_SD_CMD8_ERROR		-33

// function prototype
VOID fmiInitDevice(void);
VOID fmiSetFMIReferenceClock(UINT32 uClock);

INT  fmiSD_Read(UINT32 uChipSel, UINT32 uSector, UINT32 uBufcnt, UINT32 uDAddr);
INT  fmiSD_Write(UINT32 uChipSel, UINT32 uSector, UINT32 uBufcnt, UINT32 uSAddr);
INT  fmiSM_Read(UINT32 uChipSel, UINT32 uSector, UINT32 uBufcnt, UINT32 uDAddr);
INT  fmiSM_Write(UINT32 uChipSel, UINT32 uSector, UINT32 uBufcnt, UINT32 uSAddr);
INT  fmiSM_ChipErase(UINT32 uChipSel);
INT  fmiMS_Read(UINT32 uChipSel, UINT32 uSector, UINT32 uBufcnt, UINT32 uDAddr);
INT  fmiMS_Write(UINT32 uChipSel, UINT32 uSector, UINT32 uBufcnt, UINT32 uSAddr);

// for file system
INT  fmiInitSMDevice(UINT32 uChipSel);
INT  fmiInitSDDevice(UINT32 uChipSel);
INT  fmiInitMSDevice(UINT32 uChipSel);
//PDISK_T *fmiGetpDisk(UINT32 uCard);

// callback function
VOID fmiSetCallBack(UINT32 uCard, PVOID pvRemove, PVOID pvInsert);

#endif //_FMILIB_H_
