
#ifndef NUCWINUSP_H
#define NUCWINUSP_H

// Include WinUSB headers
//#include "stdafx.h"
//#include "NuWriter.h"
//#include "NuWriterDlg.h"
#define STRSAFE_NO_DEPRECATE

extern "C" {
#include "enumser.h"
//#include "strsafe.h"
#include "winusb.h"
#include <Usb100.h>
#include <strsafe.h>
#include <Setupapi.h>
}

#include <algorithm>

#include <vector>

//using namespace std;

static const GUID OSR_DEVICE_INTERFACE[] =
{ 0xD696BFEB, 0x1734, 0x417d, { 0x8A, 0x04, 0x86,0xD0,0x10,0x71,0xC5,0x12 } };

#if 0
typedef enum _DOWNMETHOD {
    SDRAM=0,
    NOR,
    RAWNOR,
    NAND,
    RAWNAND,
    SPI,
    RAWSPI,
    NOR_IMAGELIST,
    NAND_IMAGELIST,
    SPI_IMAGELIST,
    NORTYPE

} DOWNMETHOD;
#endif

typedef struct PIPE_ID {
    UCHAR  PipeInId;
    UCHAR  PipeOutId;
} Pipe_Id,*PPipe_Id;

typedef struct WINUSBHANDLE {
    BOOL HandlesOpen; //add handlesopen for device open display
    HANDLE hDeviceHandle;
    WINUSB_INTERFACE_HANDLE hUSBHandle;
    Pipe_Id pipeid;
    CString DevicePath;
} SWinUsbHandle,*PSWinUsbHandle;

#define MAX_DEVICE_NUM 32
class CNucWinUsb {
public:
    CNucWinUsb();
    SWinUsbHandle WinUsbHandle[MAX_DEVICE_NUM];
    int CNucWinUsb::WinUsbNumber;
    int m_ActiveDevice;

    UCHAR DeviceSpeed;

    BOOL EnableWinUsbDevice(void);
    BOOL NUC_WritePipe(int id,UCHAR *buf,ULONG len);
    //BOOL NUC_WriteAck(int id,ULONG len);
    BOOL NUC_SetType(int id,USHORT type,UCHAR * ack,ULONG len);
    BOOL NUC_ReadPipe(int id,UCHAR *buf,ULONG len);
    void NUC_CloseHandle(void);
    BOOL NUC_CheckFw(int id);

    BOOL EnableOneWinUsbDevice(int id);
    BOOL OpenDevice( int id );
    BOOL CloseWinUsbDevice(int id);
    BOOL UsbDevice_Detect(void);
    BOOL NUC_SetDeviceLEDOn(int id);
    BOOL NUC_SetDeviceLEDOff(int id);
    BOOL CNucWinUsb::NUC_BulkOutTest(int id,UCHAR *buf,ULONG len);
    BOOL NUC_ResetFW(int id);
    HANDLE m_hReadEvent;
	int retrycnt[8];

protected:
    BOOL GetDeviceHandle(void);
    BOOL GetWinUSBHandle(void);
    BOOL GetUSBDeviceSpeed(void);
    BOOL QueryDeviceEndpoints(void);

    BOOL GetOneUSBDeviceSpeed( int id );
    BOOL QueryOneDeviceEndpoints( int id );

};

//static CNucWinUsb NucUsb; //shanchun
extern CNucWinUsb NucUsb; //shanchun
#endif