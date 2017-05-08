
#include "stdafx.h"
#include "NuWriterDlg.h"
#include "NucWinUsb.h"

// Linked libraries
#pragma comment (lib , "setupapi.lib" )
#pragma comment (lib , "winusb.lib" )


CNucWinUsb::CNucWinUsb()
{
   WinUsbNumber=0;
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
	if(id >= WinUsbNumber || id < 0) 
		return FALSE;
	for(int i=0x0;i<0x4000;i++);	
	WinUsb_ReadPipe(WinUsbHandle[id].hUSBHandle, WinUsbHandle[id].pipeid.PipeInId, (unsigned char*)buf, len, &nBytesRead, 0);

	if(nBytesRead!=len)
	{
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

	if(bResult==FALSE) return FALSE;

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
	//printf("Read from pipe 0x%x: Actual data read: %d.\n", PipeID.PipeInId, nBytesRead);
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
	//ULONG ack;
	if(id >= WinUsbNumber || id < 0) return FALSE;

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
    bResult = WinUsb_ControlTransfer(WinUsbHandle[id].hUSBHandle, SetupPacket, 0,0 , &nBytesSent, 0);

	if(bResult==FALSE) return FALSE;
	/********************************************/	
	nBytesSent=0;


	bResult=WinUsb_WritePipe(WinUsbHandle[id].hUSBHandle, WinUsbHandle[id].pipeid.PipeOutId, buf, len, &nBytesSent, 0);
	if(bResult==FALSE) return FALSE;

	if(nBytesSent!=len)
	{
		return FALSE;
	}
	return TRUE;
}

void CNucWinUsb::NUC_CloseHandle(void)
{
	for(int i=0;i<WinUsbNumber;i++)
	{
		CloseHandle(WinUsbHandle[i].hDeviceHandle);
		WinUsb_Free(WinUsbHandle[i].hUSBHandle);
		WinUsbHandle[i].hDeviceHandle=INVALID_HANDLE_VALUE;
		WinUsbHandle[i].hUSBHandle=INVALID_HANDLE_VALUE;
	}
	WinUsbNumber=0;
}
BOOL CNucWinUsb::EnableWinUsbDevice(void)
{
	//AfxMessageBox("Failed to open Nuvoton VCOM");
	BOOL bResult;
	bResult = GetDeviceHandle();
	if(!bResult){
		printf("GetDeviceHandle error\n");
		return bResult;
	}
	
	bResult = GetWinUSBHandle();
	if(!bResult){
		for(int i=0;i<WinUsbNumber;i++)		
			CloseHandle(WinUsbHandle[i].hDeviceHandle);
		WinUsbNumber=0;
		printf("GetWinUSBHandle error\n");
		return bResult;
	}
	
	bResult = GetUSBDeviceSpeed();
	if(!bResult){
		printf("GetUSBDeviceSpeed error\n");
		return bResult;
	}
	
	bResult = QueryDeviceEndpoints();
	if(!bResult){
		for(int i=0;i<WinUsbNumber;i++)
		{	
			CloseHandle(WinUsbHandle[i].hDeviceHandle);
			WinUsb_Free(WinUsbHandle[i].hUSBHandle);
		}
		WinUsbNumber=0;
		printf("QueryDeviceEndpoints error\n");
		return bResult;
	}
	Sleep(100);
	return bResult;
}

BOOL CNucWinUsb::GetDeviceHandle(void)
{
    if (OSR_DEVICE_INTERFACE[0]==GUID_NULL)
    {
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

    if (hDeviceInfo == INVALID_HANDLE_VALUE) 
    { 
        // ERROR 
        //printf("Error SetupDiGetClassDevs: %d.\n", GetLastError());
        goto done;
    }

    //Enumerate all the device interfaces in the device information set.
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (index = 0; SetupDiEnumDeviceInfo(hDeviceInfo, index, &DeviceInfoData); index++)
    {
        deviceInterfaceData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);

        //Get information about the device interface.
        bResult = SetupDiEnumDeviceInterfaces( 
           hDeviceInfo,
           &DeviceInfoData,
           &OSR_DEVICE_INTERFACE[0],
           0, 
           &deviceInterfaceData);

        // Check if last item
        if (GetLastError () == ERROR_NO_MORE_ITEMS)
        {
            break;
        }

        //Check for some other error
        if (!bResult) 
        {
            //printf("Error SetupDiEnumDeviceInterfaces: %d.\n", GetLastError());
            goto done;
        }

         //Reset for this iteration
        if (lpDevicePath)
        {
            LocalFree(lpDevicePath);
        }
        if (pInterfaceDetailData)
        {
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
        if (!bResult) 
        {
            if ((ERROR_INSUFFICIENT_BUFFER==GetLastError()) && (requiredLength>0))
            {
                //we got the size, allocate buffer
                pInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, requiredLength);
                
                if (!pInterfaceDetailData) 
                { 
                    // ERROR 
                    //printf("Error allocating memory for the device detail buffer.\n");
                    goto done;
                }
            }
            else
            {
                //printf("Error SetupDiEnumDeviceInterfaces: %d.\n", GetLastError());
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
        if (!bResult) 
        {
            //printf("Error SetupDiGetDeviceInterfaceDetail: %d.\n", GetLastError());
            goto done;
        }

        //copy device path
                
        size_t nLength = lstrlen(pInterfaceDetailData->DevicePath) + 1; 
        lpDevicePath = (TCHAR *) LocalAlloc (LPTR, nLength * sizeof(TCHAR));
        StringCchCopy(lpDevicePath, nLength, pInterfaceDetailData->DevicePath);
        lpDevicePath[nLength-1] = 0;
                        
        //printf("Device path:  %s\n", lpDevicePath);
		if (!lpDevicePath)
		{
			//printf("Error %d.", GetLastError());
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

		if (WinUsbHandle[index].hDeviceHandle == INVALID_HANDLE_VALUE)
		{        
			//printf("Error %d.", GetLastError());			
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
	for(i=0; i<WinUsbNumber ; i++)
	{
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
	for(i=0; i<WinUsbNumber ; i++)
	{
		if (!(&DeviceSpeed) || WinUsbHandle[i].hDeviceHandle==INVALID_HANDLE_VALUE)    
			return FALSE;

		bResult = WinUsb_QueryDeviceInformation(WinUsbHandle[i].hUSBHandle, DEVICE_SPEED, &length, &DeviceSpeed);
		if(!bResult)
		{
			//printf("Error getting device speed: %d.\n", GetLastError());
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
	for(i=0; i<WinUsbNumber ; i++)
	{

		hUSBHandle=WinUsbHandle[i].hUSBHandle;
		pipeid=&(WinUsbHandle[i].pipeid);
		if (hUSBHandle==INVALID_HANDLE_VALUE)
			return FALSE;
	    

		

		USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
		ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));

		WINUSB_PIPE_INFORMATION  Pipe;
		ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));

		bResult = WinUsb_QueryInterfaceSettings(hUSBHandle, 0, &InterfaceDescriptor);

		if (bResult)
		{
			for (int index = 0; index < InterfaceDescriptor.bNumEndpoints; index++)
			{
				bResult = WinUsb_QueryPipe(hUSBHandle, 0, index, &Pipe);

				if (bResult)
				{
					if (Pipe.PipeType == UsbdPipeTypeControl)
					{
						//printf("Endpoint index: %d Pipe type: Control<0x%x> Pipe ID: 0x%x.\n", index, Pipe.PipeType, Pipe.PipeId);
					}
					if (Pipe.PipeType == UsbdPipeTypeIsochronous)
					{
						//printf("Endpoint index: %d Pipe type: Isochronous<0x%x> Pipe ID: 0x%x.\n", index, Pipe.PipeType, Pipe.PipeId);
					}
					if (Pipe.PipeType == UsbdPipeTypeBulk)
					{
						if (USB_ENDPOINT_DIRECTION_IN(Pipe.PipeId))
						{
							//printf("Endpoint index: %d Pipe type: Bulk<0x%x> Pipe ID: 0x%x.\n", index, Pipe.PipeType, Pipe.PipeId);
							pipeid->PipeInId = Pipe.PipeId;
						}
						if (USB_ENDPOINT_DIRECTION_OUT(Pipe.PipeId))
						{
							//printf("Endpoint index: %d Pipe type: Bulk<0x%x> Pipe ID: 0x%x.\n", index, Pipe.PipeType, Pipe.PipeId);
							pipeid->PipeOutId = Pipe.PipeId;
						}

					}
					if (Pipe.PipeType == UsbdPipeTypeInterrupt)
					{
						//printf("Endpoint index: %d Pipe type: Interrupt<0x%x> Pipe ID: 0x%x.\n", index, Pipe.PipeType, Pipe.PipeId);
					}
				}
				else
				{
					continue;
				}
			}
		}

	}
//done:
    return bResult;
}