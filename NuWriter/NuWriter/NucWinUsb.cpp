
#include "stdafx.h"
#include "NuWriterDlg.h"
#include "NucWinUsb.h"
#include <iostream>
#include <exception>
// Linked libraries
#pragma comment (lib , "setupapi.lib" )
#pragma comment (lib , "winusb.lib" )



CNucWinUsb NucUsb;

CNucWinUsb::CNucWinUsb()
{
    //WinUsbNumber=0;
}

BOOL CNucWinUsb::NUC_CheckFw(int id)
{
#if 0
    BOOL bResult;
    unsigned int fhead[2];
    unsigned char buf[BUF_SIZE];
    unsigned int ack;
    fhead[0]=4096;
    fhead[1]=0x40;
    if(id >= WinUsbNumber || id < 0)
        return FALSE;
    bResult=NUC_WritePipe(id,(unsigned char*)fhead,8);
    if(bResult==FALSE) goto done;

    bResult=NUC_WritePipe(id,(unsigned char *)buf,BUF_SIZE);
    if(bResult==FALSE) goto done;

    bResult=NUC_ReadPipe(id,(UCHAR *)&ack,4);
    if(bResult==FALSE) goto done;

    if(ack==(BUF_SIZE+1))
        return TRUE;
    else
        goto done;

done:
    CTurboburnDlg* mainWnd=(CTurboburnDlg*)(AfxGetApp()->m_pMainWnd);
    mainWnd->m_burntext.SetWindowText(" Disconnected");
    mainWnd->Invalidate();
    return FALSE;
#else
    return true;
#endif
}

BOOL CNucWinUsb::NUC_ReadPipe(int id,UCHAR *buf,ULONG len)
{
    ULONG nBytesRead=0;
    BOOL bResult;
    //TRACE(_T("NUC_ReadPipe  WinUsbNumber = %d, id =(%d)\n"), WinUsbNumber, id);
    if(id >= WinUsbNumber || id < 0)
        return FALSE;
#if(0)
    if (FALSE == WinUsbHandle[id].HandlesOpen ) {
        //TRACE("NUC_ReadPipe WinUsbHandle[%d] is not Open.\n",id);
        return FALSE;
    }
#endif
    for(int i=0x0; i<0x4000; i++);
    bResult = WinUsb_ReadPipe(WinUsbHandle[id].hUSBHandle, WinUsbHandle[id].pipeid.PipeInId, (unsigned char*)buf, len, &nBytesRead, 0);
    if(bResult==FALSE) {
        TRACE(_T("XXXX IN(%d) pipeId=0x%x   len=%d    nBytesRead=%d\n"), id, WinUsbHandle[id].pipeid.PipeOutId, len, nBytesRead);
        TRACE(_T("Error WinUsb_ReadPipe: 0x%x.\n"), GetLastError());
        return FALSE;
    }

    if(nBytesRead!=len) {
        return FALSE;
    }
    return TRUE;
}

BOOL CNucWinUsb::NUC_SetType(int id,USHORT type,UCHAR * ack,ULONG len)
{
    ULONG nBytesRead=0;
    BOOL bResult;
    WINUSB_SETUP_PACKET SetupPacket;
    if(id >= WinUsbNumber || id < 0) return FALSE;

    do {
        ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));
        //Create the setup packet
        SetupPacket.RequestType = 0x40;
        SetupPacket.Request = BURN;
        SetupPacket.Value = BURN_TYPE+(UCHAR)type;
        SetupPacket.Index = 0;
        SetupPacket.Length = 0;
        bResult = WinUsb_ControlTransfer(WinUsbHandle[id].hUSBHandle, SetupPacket, 0, 0, &nBytesRead, 0);
        if(bResult==FALSE) {
            TRACE(_T("Error WinUsb_ControlTransfer: %d.\n"), GetLastError());
            return FALSE;
        }

        ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));
        //Create the setup packet
        SetupPacket.RequestType = 0xC0;
        SetupPacket.Request = BURN;
        SetupPacket.Value = BURN_TYPE+(UCHAR)type;
        SetupPacket.Index = 0;
        SetupPacket.Length = (USHORT)len;
        bResult = WinUsb_ControlTransfer(WinUsbHandle[id].hUSBHandle, SetupPacket, (unsigned char *)ack, len, &nBytesRead, 0);

        if(bResult==FALSE) return FALSE;

    } while((UCHAR)(*ack&0xFF)!=SetupPacket.Value);
    return TRUE;
}

#if 0
BOOL CNucWinUsb::NUC_WriteAck(int id,ULONG len)
{
    ULONG nBytesRead=0;
    unsigned int ack;
    if(id >= WinUsbNumber || id < 0) return FALSE;
    WinUsb_ReadPipe(WinUsbHandle[id].hUSBHandle,WinUsbHandle[id].pipeid.PipeInId, (unsigned char*)&ack, 4, &nBytesRead, 0);
    //TRACE(_T("Read from pipe 0x%x: Actual data read: %d.\n"), PipeID.PipeInId, nBytesRead);
    if(nBytesRead!=4)
        return FALSE;
    else
        return TRUE;

    if(ack!=len)
        return FALSE;
    else
        return TRUE;
}
#endif

BOOL CNucWinUsb::NUC_WritePipe(int id,UCHAR *buf,ULONG len)
{
    ULONG nBytesSent=0;
    BOOL bResult;
    //TRACE(_T("NUC_WritePipe  WinUsbNumber = %d, id =(%d)\n"), WinUsbNumber, id);

    if(id >= WinUsbNumber || id < 0) return FALSE;
#if(0)
    if (FALSE == WinUsbHandle[id].HandlesOpen ) {
        TRACE("WinUsb_ControlTransfer WinUsbHandle[%d] is not Open.\n",id);
        return FALSE;
    }
#endif
    //TRACE(_T("NUC_WritePipe (%d)  aaaaaaaaaaaaa \n"), id);
    /********************************************/
    /* send vendor command first                */
    WINUSB_SETUP_PACKET SetupPacket;
    ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));
    //Create the setup packet
    SetupPacket.RequestType = 0x40;
    SetupPacket.Request = 0xA0;
    SetupPacket.Value = 0x12;
    SetupPacket.Index = (USHORT)len;
    SetupPacket.Length = 0;
    bResult = WinUsb_ControlTransfer(WinUsbHandle[id].hUSBHandle, SetupPacket, 0, 0, &nBytesSent, 0);

    if(bResult==FALSE) {
        //TRACE(_T("XXXX CTRL(%d) hUSBHandle=0x%x,  nBytesSent=%d\n"), id, WinUsbHandle[id].pipeid.PipeOutId, nBytesSent);
        //TRACE(_T("              0x%x  0x%x  0x%x  0x%x  0x%x\n"), SetupPacket.RequestType, SetupPacket.Request, SetupPacket.Value, SetupPacket.Index, SetupPacket.Length);
        return FALSE;
    }
    /********************************************/
#if(0)
    if (FALSE == WinUsbHandle[id].HandlesOpen ) {
        TRACE("WinUsb_WritePipe xxxxx  WinUsbHandle[%d] is not Open.\n",id);
        return FALSE;
    }
#endif
    nBytesSent=0;

    bResult=WinUsb_WritePipe(WinUsbHandle[id].hUSBHandle, WinUsbHandle[id].pipeid.PipeOutId, buf, len, &nBytesSent, 0);
    if(bResult==FALSE) {
        TRACE(_T("XXXXX Error WinUsb_WritePipe: 0x%x.\n"), GetLastError());
        //TRACE(_T("XXXX #189  OUT(%d) pipeId=0x%x   len=%d    nBytesSent=%d\n"), id, WinUsbHandle[id].pipeid.PipeOutId, len, nBytesSent);
        //TRACE(_T("           0x%x  0x%x  0x%x  0x%x  0x%x  0x%x  0x%x  0x%x\n"), buf[0],buf[1],buf[2],buf[3], buf[len-1],buf[len-2],buf[len-3],buf[len-4]);
        return FALSE;
    }

    if(nBytesSent!=len) {
        return FALSE;
    }
    return TRUE;
}

void CNucWinUsb::NUC_CloseHandle(void)
{
    while(WinUsbNumber) {
        WinUsbNumber--;
        CloseHandle(WinUsbHandle[WinUsbNumber].hDeviceHandle);
        WinUsb_Free(WinUsbHandle[WinUsbNumber].hUSBHandle);
        WinUsbHandle[WinUsbNumber].hDeviceHandle=INVALID_HANDLE_VALUE;
        WinUsbHandle[WinUsbNumber].hUSBHandle=INVALID_HANDLE_VALUE;

    }
}
BOOL CNucWinUsb::EnableWinUsbDevice(void)
{
    //AfxMessageBox("Failed to open Nuvoton VCOM");
    BOOL bResult;
    bResult = GetDeviceHandle();
    if(!bResult) {
        TRACE(_T("XXX GetDeviceHandle error\n"));
        return bResult;
    }

    bResult = GetWinUSBHandle();
    if(!bResult) {
        for(int i=0; i<WinUsbNumber; i++)
            CloseHandle(WinUsbHandle[i].hDeviceHandle);
        WinUsbNumber=0;
        TRACE(_T("XXX GetWinUSBHandle error\n"));
        return bResult;
    }

    bResult = GetUSBDeviceSpeed();
    if(!bResult) {
        TRACE(_T("XXX GetUSBDeviceSpeed error\n"));
        return bResult;
    }

    bResult = QueryDeviceEndpoints();
    if(!bResult) {
        for(int i=0; i<WinUsbNumber; i++) {
            CloseHandle(WinUsbHandle[i].hDeviceHandle);
            WinUsb_Free(WinUsbHandle[i].hUSBHandle);
        }
        WinUsbNumber=0;
        TRACE(_T("XXX QueryDeviceEndpoints error\n"));
        return bResult;
    }
    Sleep(100);
    return bResult;
}

BOOL CNucWinUsb::GetDeviceHandle(void)
{
    if (OSR_DEVICE_INTERFACE[0]==GUID_NULL) {
        return FALSE;
    }

    BOOL bResult = TRUE;
    HDEVINFO hDeviceInfo;
    SP_DEVINFO_DATA DeviceInfoData;

    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA pInterfaceDetailData = NULL;

    ULONG requiredLength=0;

    LPTSTR lpDevicePath = NULL;

    DWORD index = 0;

    // Get information about all the installed devices for the specified
    // device interface class.
    hDeviceInfo = SetupDiGetClassDevs(
                      &OSR_DEVICE_INTERFACE[0],
                      NULL,
                      NULL,
                      DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (hDeviceInfo == INVALID_HANDLE_VALUE) {
        // ERROR
        //printf("Error SetupDiGetClassDevs: %d.\n", GetLastError());
        goto done;
    }

    //Enumerate all the device interfaces in the device information set.
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (index = 0; SetupDiEnumDeviceInfo(hDeviceInfo, index, &DeviceInfoData); index++) {
        deviceInterfaceData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);

        //Get information about the device interface.
        bResult = SetupDiEnumDeviceInterfaces(
                      hDeviceInfo,
                      &DeviceInfoData,
                      &OSR_DEVICE_INTERFACE[0],
                      0,
                      &deviceInterfaceData);

        // Check if last item
        if (GetLastError () == ERROR_NO_MORE_ITEMS) {
            goto done;
        }

        //Check for some other error
        if (!bResult) {
            //TRACE(_T("Error SetupDiEnumDeviceInterfaces:0x%x.\n"), GetLastError());
            goto done;
        }

        //Reset for this iteration
        if (lpDevicePath) {
            LocalFree(lpDevicePath);
        }
        if (pInterfaceDetailData) {
            LocalFree(pInterfaceDetailData);
        }

        //Interface data is returned in SP_DEVICE_INTERFACE_DETAIL_DATA
        //which we need to allocate, so we have to call this function twice.
        //First to get the size so that we know how much to allocate
        //Second, the actual call with the allocated buffer

        bResult = SetupDiGetDeviceInterfaceDetail(
                      hDeviceInfo,
                      &deviceInterfaceData,
                      NULL, 0,
                      &requiredLength,
                      NULL);


        //Check for some other error
        if (!bResult) {
            if ((ERROR_INSUFFICIENT_BUFFER==GetLastError()) && (requiredLength>0)) {
                //we got the size, allocate buffer
                pInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, requiredLength);

                if (!pInterfaceDetailData) {
                    // ERROR
                    //TRACE(_T("Error allocating memory for the device detail buffer.\n"));
                    goto done;
                }
            } else {
                //TRACE(_T("Error SetupDiEnumDeviceInterfaces: 0x%x.\n"), GetLastError());
                goto done;
            }
        }

        //get the interface detailed data
        pInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        //Now call it with the correct size and allocated buffer
        bResult = SetupDiGetDeviceInterfaceDetail(
                      hDeviceInfo,
                      &deviceInterfaceData,
                      pInterfaceDetailData,
                      requiredLength,
                      NULL,
                      &DeviceInfoData);

        //Check for some other error
        if (!bResult) {
            //TRACE(_T("Error SetupDiGetDeviceInterfaceDetail: 0x%x.\n"), GetLastError());
            goto done;
        }

        //copy device path

        size_t nLength = lstrlen(pInterfaceDetailData->DevicePath) + 1;
        lpDevicePath = (TCHAR *) LocalAlloc (LPTR, nLength * sizeof(TCHAR));
        StringCchCopy(lpDevicePath, nLength, pInterfaceDetailData->DevicePath);
        lpDevicePath[nLength-1] = 0;

        //TRACE(_T("Device path:  %s\n"), lpDevicePath);
        if (!lpDevicePath) {
            //TRACE(_T("Error 0x%x."), GetLastError());
            goto done;
        }

        //Open the device
        WinUsbHandle[index].hDeviceHandle = CreateFile (
                                                lpDevicePath,
                                                GENERIC_READ | GENERIC_WRITE,
                                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                                NULL,
                                                OPEN_EXISTING,
                                                FILE_FLAG_OVERLAPPED,
                                                NULL);

        if (WinUsbHandle[index].hDeviceHandle == INVALID_HANDLE_VALUE) {
            //TRACE(_T("Error 0x%x."), GetLastError());
            goto done;
        }
        WinUsbHandle[index].DevicePath.Format(_T("%s"),lpDevicePath);
        WinUsbNumber=index+1;
    }


done:
    LocalFree(lpDevicePath);
    LocalFree(pInterfaceDetailData);
    bResult = SetupDiDestroyDeviceInfoList(hDeviceInfo);
    return bResult;
}

BOOL CNucWinUsb::GetWinUSBHandle(void)
{
    int i=0;
    BOOL bResult=FALSE;
    for(i=0; i<WinUsbNumber ; i++) {
        if (WinUsbHandle[i].hDeviceHandle == INVALID_HANDLE_VALUE)
            return FALSE;
        bResult = WinUsb_Initialize(WinUsbHandle[i].hDeviceHandle, &WinUsbHandle[i].hUSBHandle);
        if(!bResult)
            return FALSE;
    }
    return bResult;
}


BOOL CNucWinUsb::GetUSBDeviceSpeed()
{
    int i=0;
    BOOL bResult = TRUE;
    ULONG length = sizeof(UCHAR);
    UCHAR DeviceSpeed;
    for(i=0; i<WinUsbNumber ; i++) {
        if (!(&DeviceSpeed) || WinUsbHandle[i].hDeviceHandle==INVALID_HANDLE_VALUE)
            return FALSE;

        bResult = WinUsb_QueryDeviceInformation(WinUsbHandle[i].hUSBHandle, DEVICE_SPEED, &length, &DeviceSpeed);
        if(!bResult) {
            //TRACE(_T("Error getting device speed: 0x%x.\n"), GetLastError());
            return bResult;
        }
    }
    return bResult;
    //if(*pDeviceSpeed == LowSpeed)
    //if(*pDeviceSpeed == FullSpeed)
    //if(*pDeviceSpeed == HighSpeed)
}


BOOL CNucWinUsb::QueryDeviceEndpoints(void)
{
    int i=0;
    PIPE_ID* pipeid;
    BOOL bResult = TRUE;
    WINUSB_INTERFACE_HANDLE hUSBHandle;
    //ULONG timeout = 20000;
    ULONG timeout = 10000;
    for(i=0; i<WinUsbNumber ; i++) {
        hUSBHandle=WinUsbHandle[i].hUSBHandle;
        pipeid=&(WinUsbHandle[i].pipeid);
        if (hUSBHandle==INVALID_HANDLE_VALUE)
            return FALSE;

        USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
        ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));

        WINUSB_PIPE_INFORMATION  Pipe;
        ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));

        bResult = WinUsb_QueryInterfaceSettings(hUSBHandle, 0, &InterfaceDescriptor);

        if (bResult) {
            for (int index = 0; index < InterfaceDescriptor.bNumEndpoints; index++) {
                bResult = WinUsb_QueryPipe(hUSBHandle, 0, index, &Pipe);

                if (bResult) {
                    if (Pipe.PipeType == UsbdPipeTypeControl) {
                        //TRACE(_T("Endpoint index: %d Pipe type: Control<0x%x> Pipe ID: 0x%x.\n"), index, Pipe.PipeType, Pipe.PipeId);
                    }
                    if (Pipe.PipeType == UsbdPipeTypeIsochronous) {
                        //TRACE(_T("Endpoint index: %d Pipe type: Isochronous<0x%x> Pipe ID: 0x%x.\n"), index, Pipe.PipeType, Pipe.PipeId);
                    }
                    if (Pipe.PipeType == UsbdPipeTypeBulk) {
                        if (USB_ENDPOINT_DIRECTION_IN(Pipe.PipeId)) {
                            //TRACE(_T("Endpoint index: %d Pipe type: Bulk<0x%x> Pipe ID: 0x%x.\n"), index, Pipe.PipeType, Pipe.PipeId);
                            pipeid->PipeInId = Pipe.PipeId;
                            WinUsb_SetPipePolicy(hUSBHandle, Pipe.PipeId, PIPE_TRANSFER_TIMEOUT, sizeof(timeout), &timeout);
                        }
                        if (USB_ENDPOINT_DIRECTION_OUT(Pipe.PipeId)) {
                            //TRACE(_T("Endpoint index: %d Pipe type: Bulk<0x%x> Pipe ID: 0x%x.\n"), index, Pipe.PipeType, Pipe.PipeId);
                            pipeid->PipeOutId = Pipe.PipeId;
                            WinUsb_SetPipePolicy(hUSBHandle, Pipe.PipeId, PIPE_TRANSFER_TIMEOUT, sizeof(timeout), &timeout);							
                        }

                    }
                    if (Pipe.PipeType == UsbdPipeTypeInterrupt) {
                        //TRACE(_T("Endpoint index: %d Pipe type: Interrupt<0x%x> Pipe ID: 0x%x.\n"), index, Pipe.PipeType, Pipe.PipeId);
                    }
                } else {
                    continue;
                }
            }
        }

    }
//done:
    return bResult;
}

BOOL CNucWinUsb::CloseWinUsbDevice( int id )
{
    BOOL bResult = TRUE;
    //TRACE("CloseWinUsbDevice id=%d\n",id);
    if (FALSE == WinUsbHandle[id].HandlesOpen ) {
        // Called on an uninitialized DeviceData
        //TRACE(_T("WinUsbHandle[%d] is not Open.\n"),id);
        return FALSE;
    }
    CloseHandle(WinUsbHandle[id].hDeviceHandle);
    WinUsb_Free(WinUsbHandle[id].hUSBHandle);
    WinUsbHandle[id].hDeviceHandle=INVALID_HANDLE_VALUE;
    WinUsbHandle[id].hUSBHandle=INVALID_HANDLE_VALUE;
    WinUsbHandle[id].HandlesOpen = FALSE;

//Sleep(150);
    return TRUE;
}

BOOL CNucWinUsb::EnableOneWinUsbDevice(int id)
{
    BOOL bResult = TRUE;
    CString str;

    //TRACE("#### EnableOneWinUsbDevice id=%d\n",id);
    if(WinUsbHandle[id].HandlesOpen == TRUE) {
        TRACE(_T("#### Device %d has been enabled by other thread\n"),id);
        //CloseHandle(WinUsbHandle[id].hDeviceHandle);
        //WinUsb_Free(WinUsbHandle[id].hUSBHandle);
#if(0)
        CloseHandle(WinUsbHandle[id].hDeviceHandle);
        WinUsb_Free(WinUsbHandle[id].hUSBHandle);
        WinUsbHandle[id].hDeviceHandle=INVALID_HANDLE_VALUE;
        WinUsbHandle[id].hUSBHandle=INVALID_HANDLE_VALUE;
        WinUsbHandle[id].HandlesOpen = FALSE;
#endif
        return FALSE;
    }

    bResult = OpenDevice(id);
    if(!bResult) {
        TRACE(_T("#### (%d)  OpenDevice: Try again\n"),id);
        Sleep(500);
        bResult = OpenDevice(id);
        if(!bResult) {
            TRACE(_T("#### (%d) XXX NucWinUsb.cpp  OpenDevice Error\n"),id);
            return bResult;
        }
    }

    //TRACE(_T(" WinUsbNumber WinUsbNumber WinUsbNumber %d\n"),WinUsbNumber);
    Sleep(200);//for Mass production
    bResult = GetOneUSBDeviceSpeed(id);
    if(!bResult) {
        TRACE(_T("#### (%d) XXX GetOneUSBDeviceSpeed\n"),id);
        CloseWinUsbDevice(id);
        return bResult;
    }
    Sleep(200);//for Mass production
    bResult = QueryOneDeviceEndpoints( id );
    if(!bResult) {
        TRACE(_T("#### (%d) XXX QueryOneDeviceEndpoints\n"),id);
        CloseWinUsbDevice(id);
        return bResult;
    }
    Sleep(200);

    //TRACE(_T("#### (%d) EnableOneWinUsbDevice  bResult=%d\n"),id, bResult);
    return bResult;
}

BOOL CNucWinUsb::GetOneUSBDeviceSpeed( int id )
{
    BOOL bResult = TRUE;
    ULONG length = sizeof(UCHAR);
    UCHAR DeviceSpeed;

    if (!(&DeviceSpeed) || WinUsbHandle[id].hDeviceHandle==INVALID_HANDLE_VALUE)
        return FALSE;

    bResult = WinUsb_QueryDeviceInformation(WinUsbHandle[id].hUSBHandle, DEVICE_SPEED, &length, &DeviceSpeed);
    if(!bResult) {
        TRACE("XXX Error getting device speed: 0x%x.\n", GetLastError());
        return bResult;
    }
    if(DeviceSpeed == LowSpeed) {
        //TRACE("Device speed: %d (Low speed).\n", DeviceSpeed);
        goto done;
    }
    if(DeviceSpeed == FullSpeed) {
        //TRACE("Device speed: %d (Full speed).\n", DeviceSpeed);
        goto done;
    }
    if(DeviceSpeed == HighSpeed) {
        //TRACE("Device speed: %d (High speed).\n", DeviceSpeed);
        goto done;
    }

done:
    return bResult;
}

BOOL CNucWinUsb::QueryOneDeviceEndpoints( int id )
{
    PIPE_ID* pipeid;
    BOOL bResult = TRUE, nStatus;
    WINUSB_INTERFACE_HANDLE hUSBHandle;
    // Set the transfer time-out interval for endpoint 0 to 10000 milliseconds.
    ULONG timeout = 10000;

    hUSBHandle=WinUsbHandle[id].hUSBHandle;
    pipeid=&(WinUsbHandle[id].pipeid);
    if (hUSBHandle==INVALID_HANDLE_VALUE)
        return FALSE;
    USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
    ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));

    WINUSB_PIPE_INFORMATION  Pipe;
    ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));

    bResult = WinUsb_QueryInterfaceSettings(hUSBHandle, 0, &InterfaceDescriptor);

    if (bResult) {
        for (int index = 0; index < InterfaceDescriptor.bNumEndpoints; index++) {
            bResult = WinUsb_QueryPipe(hUSBHandle, 0, index, &Pipe);

            if (bResult) {
                if (Pipe.PipeType == UsbdPipeTypeControl) {
                    //TRACE("Endpoint index: %d Pipe type: Control<0x%x> Pipe ID: 0x%x.\n", index, Pipe.PipeType, Pipe.PipeId);
                }
                if (Pipe.PipeType == UsbdPipeTypeIsochronous) {
                    //TRACE("Endpoint index: %d Pipe type: Isochronous<0x%x> Pipe ID: 0x%x.\n", index, Pipe.PipeType, Pipe.PipeId);
                }
                if (Pipe.PipeType == UsbdPipeTypeBulk) {
                    if (USB_ENDPOINT_DIRECTION_IN(Pipe.PipeId)) {
                        //TRACE("Endpoint index: %d Pipe type: Bulk IN<0x%x> Pipe ID: 0x%x.\n", index, Pipe.PipeType, Pipe.PipeId);
                        pipeid->PipeInId = Pipe.PipeId;
                        nStatus = WinUsb_SetPipePolicy(hUSBHandle,
                                                       Pipe.PipeId, PIPE_TRANSFER_TIMEOUT,
                                                       sizeof(timeout), &timeout);
                        if (nStatus == FALSE) {
                            TRACE(_T("XXX Bulk IN  errorType = 0x%x\n"), GetLastError());
                            return FALSE;
                        }
                    }
                    if (USB_ENDPOINT_DIRECTION_OUT(Pipe.PipeId)) {
                        //TRACE("Endpoint index: %d Pipe type: Bulk OUT<0x%x> Pipe ID: 0x%x.\n", index, Pipe.PipeType, Pipe.PipeId);
                        pipeid->PipeOutId = Pipe.PipeId;
                        nStatus = WinUsb_SetPipePolicy(hUSBHandle,
                                                       Pipe.PipeId, PIPE_TRANSFER_TIMEOUT,
                                                       sizeof(timeout), &timeout);
                        if (nStatus == FALSE) {
                            TRACE(_T("XXX Bulk OUT  errorType = 0x%x\n"), GetLastError());
                            return FALSE;
                        }
                    }
                }
                if (Pipe.PipeType == UsbdPipeTypeInterrupt) {
                    //TRACE("Endpoint index: %d Pipe type: Interrupt<0x%x> Pipe ID: 0x%x.\n", index, Pipe.PipeType, Pipe.PipeId);
                }
            } else {
                continue;
            }
        }
    }
    //done:
    return bResult;
}

BOOL CNucWinUsb::OpenDevice( int id )
{
    //BOOL bResult = TRUE;
	BOOL bResult = FALSE;
    HDEVINFO hDeviceInfo;
    SP_DEVINFO_DATA DeviceInfoData;

    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA pInterfaceDetailData = NULL;

    ULONG requiredLength=0;
    LPTSTR lpDevicePath = NULL;

    if (OSR_DEVICE_INTERFACE[0]==GUID_NULL) {
        return FALSE;
    }
    WinUsbHandle[id].HandlesOpen = FALSE;
    // Get information about all the installed devices for the specified
    // device interface class.
    hDeviceInfo = SetupDiGetClassDevs(
                      &OSR_DEVICE_INTERFACE[0],
                      NULL,
                      NULL,
                      DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (hDeviceInfo == INVALID_HANDLE_VALUE) {
        // ERROR
        //TRACE(_T("Error SetupDiGetClassDevs: 0x%x.\n"), GetLastError());
        goto done;
    }
    //Enumerate all the device interfaces in the device information set.
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    bResult = SetupDiEnumDeviceInfo(hDeviceInfo, id, &DeviceInfoData);
    if(!bResult) {
        //TRACE(_T("Error OpenDevice SetupDiEnumDeviceInfo: 0x%x.\n"), GetLastError());
        goto done;
    }
    deviceInterfaceData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);
    //Get information about the device interface.
    bResult = SetupDiEnumDeviceInterfaces(
                  hDeviceInfo,
                  &DeviceInfoData,
                  &OSR_DEVICE_INTERFACE[0],
                  0,
                  &deviceInterfaceData);
    // Check if last item
    if (GetLastError () == ERROR_NO_MORE_ITEMS) {
        goto done;
    }
    //Check for some other error
    if (!bResult) {
        //TRACE(_T("Error SetupDiEnumDeviceInterfaces: 0x%x.\n"), GetLastError());
        goto done;
    }

    //Reset for this iteration
    if (lpDevicePath) {
        LocalFree(lpDevicePath);
    }
    if (pInterfaceDetailData) {
        LocalFree(pInterfaceDetailData);
    }

    //Interface data is returned in SP_DEVICE_INTERFACE_DETAIL_DATA
    //which we need to allocate, so we have to call this function twice.
    //First to get the size so that we know how much to allocate
    //Second, the actual call with the allocated buffer

    bResult = SetupDiGetDeviceInterfaceDetail(
                  hDeviceInfo,
                  &deviceInterfaceData,
                  NULL, 0,
                  &requiredLength,
                  NULL);


    //Check for some other error
    if (!bResult) {
        if ((ERROR_INSUFFICIENT_BUFFER==GetLastError()) && (requiredLength>0)) {
            //we got the size, allocate buffer
            pInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, requiredLength);

            if (!pInterfaceDetailData) {
                // ERROR
                //TRACE(_T("Error allocating memory for the device detail buffer.\n"));
                goto done;
            }
        } else {
            //TRACE(_T("Error SetupDiEnumDeviceInterfaces: 0x%x.\n"), GetLastError());
            goto done;
        }
    }

    //get the interface detailed data
    pInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

    //Now call it with the correct size and allocated buffer
    bResult = SetupDiGetDeviceInterfaceDetail(
                  hDeviceInfo,
                  &deviceInterfaceData,
                  pInterfaceDetailData,
                  requiredLength,
                  NULL,
                  &DeviceInfoData);

    //Check for some other error
    if (!bResult) {
        //TRACE(_T("Error SetupDiGetDeviceInterfaceDetail:0x%x.\n"), GetLastError());
        goto done;
    }

    //copy device path

    size_t nLength = lstrlen(pInterfaceDetailData->DevicePath) + 1;
    lpDevicePath = (TCHAR *) LocalAlloc (LPTR, nLength * sizeof(TCHAR));
    StringCchCopy(lpDevicePath, nLength, pInterfaceDetailData->DevicePath);
    lpDevicePath[nLength-1] = 0;

    //TRACE(_T("Device path:  %s\n"), lpDevicePath);
    if (!lpDevicePath) {
        //TRACE(_T("Error 0x%x."), GetLastError());
        goto done;
    }

    //Open the device
    WinUsbHandle[id].hDeviceHandle = CreateFile (
                                         lpDevicePath,
                                         GENERIC_READ | GENERIC_WRITE,
                                         FILE_SHARE_READ | FILE_SHARE_WRITE,
                                         NULL,
                                         OPEN_EXISTING,
                                         FILE_FLAG_OVERLAPPED,
                                         NULL);

    if (WinUsbHandle[id].hDeviceHandle == INVALID_HANDLE_VALUE) {
        //TRACE(_T("Error 0x%x."), GetLastError());
        goto done;
    }
    WinUsbHandle[id].DevicePath.Format(_T("%s"),lpDevicePath);

    bResult = WinUsb_Initialize(WinUsbHandle[id].hDeviceHandle,
                                &WinUsbHandle[id].hUSBHandle);
    if (FALSE == bResult) {
        CloseHandle(WinUsbHandle[id].hDeviceHandle);
        goto done;
        //TRACE(_T("WinUsb_Initialize Error\n"));
    }
    WinUsbHandle[id].HandlesOpen = TRUE;



done:
    LocalFree(lpDevicePath);
    LocalFree(pInterfaceDetailData);
    SetupDiDestroyDeviceInfoList(hDeviceInfo);
    return bResult;
}

int CNucWinUsb::UsbDevice_Detect( void )
{
    BOOL bResult = TRUE;
    int id = 0;

    while( id < 8 ) {
        bResult=NucUsb.EnableOneWinUsbDevice(id);
        if(!bResult) {
            break;
        }
        NucUsb.CloseWinUsbDevice(id);
        id++;
    }
    WinUsbNumber = id;
    //TRACE(_T("CNucWinUsb::UsbDevice_Detect  WinUsbNumber %d\n"),WinUsbNumber);

    return (WinUsbNumber);
}

BOOL CNucWinUsb::NUC_SetDeviceLEDOn(int id)
{
    BOOL bResult = FALSE;
    ULONG nBytesRead=0;
    WINUSB_SETUP_PACKET SetupPacket;

    bResult=NucUsb.EnableOneWinUsbDevice(id);

    if(bResult) {
        ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));
        SetupPacket.RequestType = 0x51;//WM_LED_ON;
        SetupPacket.Request = 0;
        SetupPacket.Value = 0;
        SetupPacket.Index = 0;
        SetupPacket.Length = 0;

        WinUsb_ControlTransfer(WinUsbHandle[id].hUSBHandle, SetupPacket, 0, 0, &nBytesRead, 0);
        NucUsb.CloseWinUsbDevice(id);
    } else {
        NucUsb.CloseWinUsbDevice(id);
        return FALSE;
    }

    return TRUE;
}

BOOL CNucWinUsb::NUC_SetDeviceLEDOff(int id)
{
    BOOL bResult = FALSE;
    ULONG nBytesRead=0;
    WINUSB_SETUP_PACKET SetupPacket;

    bResult=NucUsb.EnableOneWinUsbDevice(id);

    if(bResult) {
        ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));
        SetupPacket.RequestType = 0x50;//WM_LED_OFF;
        SetupPacket.Request = 0;
        SetupPacket.Value = 0;
        SetupPacket.Index = 0;
        SetupPacket.Length = 0;

        WinUsb_ControlTransfer(WinUsbHandle[id].hUSBHandle, SetupPacket, 0, 0, &nBytesRead, 0);
        NucUsb.CloseWinUsbDevice(id);
    } else {
        NucUsb.CloseWinUsbDevice(id);
        return FALSE;
    }

    return TRUE;
}

BOOL CNucWinUsb::NUC_ResetFW(int id)
{
    BOOL bResult = FALSE;
    ULONG nBytesRead=0;
    WINUSB_SETUP_PACKET SetupPacket;

    bResult=NucUsb.EnableOneWinUsbDevice(id);

    if(bResult) {
        ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));
        SetupPacket.RequestType = 0x46;//WM_RESET;
        SetupPacket.Request = 0;
        SetupPacket.Value = 0;
        SetupPacket.Index = 0;
        SetupPacket.Length = 0;

        WinUsb_ControlTransfer(WinUsbHandle[id].hUSBHandle, SetupPacket, 0, 0, &nBytesRead, 0);
        NucUsb.CloseWinUsbDevice(id);
    } else {
        NucUsb.CloseWinUsbDevice(id);
        return FALSE;
    }

    return TRUE;
}

BOOL CNucWinUsb::NUC_BulkOutTest(int id,UCHAR *buf,ULONG len)
{
    BOOL bResult = FALSE;
    ULONG nBytesSent=0;

    bResult=NucUsb.EnableOneWinUsbDevice(id);

    if(id >= WinUsbNumber || id < 0) return FALSE;

    if (FALSE == WinUsbHandle[id].HandlesOpen ) {
        TRACE(_T("WinUsb_ControlTransfer WinUsbHandle[%d] is not Open.\n"),id);
        return FALSE;
    }

    nBytesSent=0;
    bResult=WinUsb_WritePipe(WinUsbHandle[id].hUSBHandle, WinUsbHandle[id].pipeid.PipeOutId, buf, len, &nBytesSent, 0);
    if(bResult==FALSE) {
        //TRACE(_T("XXXX OUT(%d) pipeId=0x%x   len=%d    nBytesSent=%d\n"), id, WinUsbHandle[id].pipeid.PipeOutId, len, nBytesSent);
        NucUsb.CloseWinUsbDevice(id);
        return FALSE;
    }

    NucUsb.CloseWinUsbDevice(id);

    if(nBytesSent!=len) {
        return FALSE;
    }
    return TRUE;
}

