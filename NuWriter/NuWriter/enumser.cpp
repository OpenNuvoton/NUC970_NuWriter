/////////////////////////////////  Includes  //////////////////////////////////

#include "stdafx.h"
#include "enumser.h"

#define CLASSNAME "MCUPorts"//"Ports" //"MCUPorts"

#ifndef NO_ENUMSERIAL_USING_SETUPAPI
  #ifndef _INC_SETUPAPI
  #pragma message("To avoid this message, please put setupapi.h in your PCH (normally stdafx.h)")
  #include <setupapi.h>
  #endif
#endif

#ifndef NO_ENUMSERIAL_USING_ENUMPORTS
  #ifndef _WINSPOOL_
  #pragma message("To avoid this message, please put winspool.h in your PCH (normally stdafx.h)")
  #include <winspool.h>
  #endif
#endif

#ifndef NO_ENUMSERIAL_USING_WMI
  #ifndef __IWbemLocator_FWD_DEFINED__
  #pragma message("To avoid this message, please put WBemCli.h in your PCH (normally stdafx.h)")
  #include <WbemCli.h>
  #endif
  #ifndef _INC_COMDEF
  #pragma message("To avoid this message, please put comdef.h in your PCH (normally stdafx.h)")
  #include <comdef.h>
  #endif
  #ifndef __AFXPRIV_H__
  #pragma message("To avoid this message, please put Afxpriv.h in your PCH (normally stdafx.h)")
  #include <afxpriv.h>
  #endif

  //Automatically pull in the library WbemUuid.Lib since we need the WBem Guids
  #pragma comment(lib, "WbemUuid.Lib")
#endif


///////////////////////////// Statics / Locals ////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
#define new DEBUG_NEW
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifndef NO_ENUMSERIAL_USING_SETUPAPI
  typedef HKEY (__stdcall SETUPDIOPENDEVREGKEY)(HDEVINFO, PSP_DEVINFO_DATA, DWORD, DWORD, DWORD, REGSAM);
  typedef BOOL (__stdcall SETUPDICLASSGUIDSFROMNAME)(LPCTSTR, LPGUID, DWORD, PDWORD);
  typedef BOOL (__stdcall SETUPDIDESTROYDEVICEINFOLIST)(HDEVINFO);
  typedef BOOL (__stdcall SETUPDIENUMDEVICEINFO)(HDEVINFO, DWORD, PSP_DEVINFO_DATA);
  typedef HDEVINFO (__stdcall SETUPDIGETCLASSDEVS)(LPGUID, LPCTSTR, HWND, DWORD);
  typedef BOOL (__stdcall SETUPDIGETDEVICEREGISTRYPROPERTY)(HDEVINFO, PSP_DEVINFO_DATA, DWORD, PDWORD, PBYTE, DWORD, PDWORD);
#endif


///////////////////////////// Implementation //////////////////////////////////

#ifndef NO_ENUMSERIAL_USING_CREATEFILE
BOOL CEnumerateSerial::UsingCreateFile(CUIntArray& ports)
{
  //Make sure we clear out any elements which may already be in the array
  ports.RemoveAll();

  //Up to 255 COM ports are supported so we iterate through all of them seeing
  //if we can open them or if we fail to open them, get an access denied or general error error.
  //Both of these cases indicate that there is a COM port at that number. 
  for (UINT i=1; i<256; i++)
  {
    //Form the Raw device name
    CString sPort;
    sPort.Format(_T("\\\\.\\COM%d"), i);

    //Try to open the port
    BOOL bSuccess = FALSE;
    HANDLE hPort = ::CreateFile(sPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    if (hPort == INVALID_HANDLE_VALUE)
    {
      DWORD dwError = GetLastError();

      //Check to see if the error was because some other app had the port open or a general failure
      if (dwError == ERROR_ACCESS_DENIED || dwError == ERROR_GEN_FAILURE || dwError == ERROR_SHARING_VIOLATION)
        bSuccess = TRUE;
    }
    else
    {
      //The port was opened successfully
      bSuccess = TRUE;

      //Don't forget to close the port, since we are going to do nothing with it anyway
      CloseHandle(hPort);
    }

    //Add the port number to the array which will be returned
    if (bSuccess)
      ports.Add(i);
  }

  //Return the success indicator
  return TRUE;
}
#endif

BOOL CEnumerateSerial::IsNumeric(LPCTSTR pszString, BOOL bIgnoreColon)
{
  int nLen = _tcslen(pszString);
  if (nLen == 0)
    return FALSE;

  //Assume the best
  BOOL bNumeric = TRUE;

  for (int i=0; i<nLen && bNumeric; i++)
  {
    bNumeric = (_istdigit(pszString[i]) != 0);
    if (bIgnoreColon && (pszString[i] == _T(':')))
      bNumeric = TRUE;
  }

  return bNumeric;
}

#ifndef NO_ENUMSERIAL_USING_QUERYDOSDEVICE
BOOL CEnumerateSerial::UsingQueryDosDevice(CUIntArray& ports)
{
  //Make sure we clear out any elements which may already be in the array
  ports.RemoveAll();

  //Determine what OS we are running on
  OSVERSIONINFO osvi;
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  BOOL bGetVer = GetVersionEx(&osvi);

  //On NT use the QueryDosDevice API
  if (bGetVer && (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT))
  {
    //Use QueryDosDevice to look for all devices of the form COMx. This is a better
    //solution as it means that no ports have to be opened at all.
    TCHAR szDevices[65535];
    DWORD dwChars = QueryDosDevice(NULL, szDevices, 65535);
    if (dwChars)
    {
      int i=0;
      while (szDevices[i] != _T('\0'))
      {
        //Get the current device name
        TCHAR* pszCurrentDevice = &szDevices[i];

        //If it looks like "COMX" then
        //add it to the array which will be returned
        int nLen = _tcslen(pszCurrentDevice);
        if (nLen > 3)
        {
          if ((_tcsnicmp(pszCurrentDevice, _T("COM"), 3) == 0) && IsNumeric(&pszCurrentDevice[3], FALSE))
          {
            //Work out the port number
            int nPort = _ttoi(&pszCurrentDevice[3]);
            ports.Add(nPort);
          }
        }

        //Go to next device name
        i += (nLen+1);
      }
    }
    else
    {
      TRACE(_T("Failed in call to QueryDosDevice, GetLastError:%d\n"), GetLastError());
      return FALSE;
    }
  }
  else
  {
    TRACE(_T("Calling QueryDosDevice on Win9x which does not support enumeration of serial ports\n"));
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }

  return TRUE;
}
#endif

#ifndef NO_ENUMSERIAL_USING_GETDEFAULTCOMMCONFIG
BOOL CEnumerateSerial::UsingGetDefaultCommConfig(CUIntArray& ports)
{
  //Make sure we clear out any elements which may already be in the array
  ports.RemoveAll();

  //Up to 255 COM ports are supported so we iterate through all of them seeing
  //if we can get the default configuration
  for (UINT i=1; i<256; i++)
  {
    //Form the Raw device name
    CString sPort;
    sPort.Format(_T("COM%d"), i);

    COMMCONFIG cc;
    DWORD dwSize = sizeof(COMMCONFIG);
    if (GetDefaultCommConfig(sPort, &cc, &dwSize))
    {
      ports.Add(i);
    }
  }

  //Return the success indicator
  return TRUE;
}
#endif

#ifndef NO_ENUMSERIAL_USING_SETUPAPI
BOOL CEnumerateSerial::UsingSetupAPI(CUIntArray& ports, CStringArray& sFriendlyNames)
{
  //Make sure we clear out any elements which may already be in the array
  ports.RemoveAll();

  //Get the function pointers to "SetupDiGetClassDevs", "SetupDiGetClassDevs", "SetupDiEnumDeviceInfo", "SetupDiOpenDevRegKey" 
  //and "SetupDiDestroyDeviceInfoList" in setupapi.dll
  HINSTANCE hSetupAPI = LoadLibrary(_T("SETUPAPI.DLL"));
  if (hSetupAPI == NULL)
    return FALSE;

  SETUPDIOPENDEVREGKEY* lpfnLPSETUPDIOPENDEVREGKEY = (SETUPDIOPENDEVREGKEY*) GetProcAddress(hSetupAPI, "SetupDiOpenDevRegKey");
  #ifdef _UNICODE
  SETUPDICLASSGUIDSFROMNAME* lpfnSETUPDICLASSGUIDSFROMNAME = (SETUPDICLASSGUIDSFROMNAME*) GetProcAddress(hSetupAPI, "SetupDiClassGuidsFromNameW");
  SETUPDIGETCLASSDEVS* lpfnSETUPDIGETCLASSDEVS = (SETUPDIGETCLASSDEVS*) GetProcAddress(hSetupAPI, "SetupDiGetClassDevsW");
  SETUPDIGETDEVICEREGISTRYPROPERTY* lpfnSETUPDIGETDEVICEREGISTRYPROPERTY = (SETUPDIGETDEVICEREGISTRYPROPERTY*) GetProcAddress(hSetupAPI, "SetupDiGetDeviceRegistryPropertyW");
  #else
  SETUPDICLASSGUIDSFROMNAME* lpfnSETUPDICLASSGUIDSFROMNAME = (SETUPDICLASSGUIDSFROMNAME*) GetProcAddress(hSetupAPI, "SetupDiClassGuidsFromNameA");
  SETUPDIGETCLASSDEVS* lpfnSETUPDIGETCLASSDEVS = (SETUPDIGETCLASSDEVS*) GetProcAddress(hSetupAPI, "SetupDiGetClassDevsA");
  SETUPDIGETDEVICEREGISTRYPROPERTY* lpfnSETUPDIGETDEVICEREGISTRYPROPERTY = (SETUPDIGETDEVICEREGISTRYPROPERTY*) GetProcAddress(hSetupAPI, "SetupDiGetDeviceRegistryPropertyA");
  #endif
  SETUPDIDESTROYDEVICEINFOLIST* lpfnSETUPDIDESTROYDEVICEINFOLIST = (SETUPDIDESTROYDEVICEINFOLIST*) GetProcAddress(hSetupAPI, "SetupDiDestroyDeviceInfoList");
  SETUPDIENUMDEVICEINFO* lpfnSETUPDIENUMDEVICEINFO = (SETUPDIENUMDEVICEINFO*) GetProcAddress(hSetupAPI, "SetupDiEnumDeviceInfo");

  if ((lpfnLPSETUPDIOPENDEVREGKEY == NULL) || (lpfnSETUPDICLASSGUIDSFROMNAME == NULL) || (lpfnSETUPDIDESTROYDEVICEINFOLIST == NULL) ||
      (lpfnSETUPDIENUMDEVICEINFO == NULL) || (lpfnSETUPDIGETCLASSDEVS == NULL) || (lpfnSETUPDIGETDEVICEREGISTRYPROPERTY == NULL))
  {
    //Unload the setup dll
    FreeLibrary(hSetupAPI);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

    return FALSE;
  }

  //First need to convert the name "Ports" to a GUID using SetupDiClassGuidsFromName
  DWORD dwGuids = 0;
  lpfnSETUPDICLASSGUIDSFROMNAME(_T(CLASSNAME), NULL, 0, &dwGuids);//Ports
  if (dwGuids == 0)
  {
    //Unload the setup dll
    FreeLibrary(hSetupAPI);

    return FALSE;
  }

  //Allocate the needed memory
  GUID* pGuids = new GUID[dwGuids];

  //Call the function again
  if (!lpfnSETUPDICLASSGUIDSFROMNAME(_T(CLASSNAME), pGuids, dwGuids, &dwGuids))
  {
    //Free up the memory before we return
    delete [] pGuids;

    //Unload the setup dll
    FreeLibrary(hSetupAPI);

    return FALSE;
  }

  //Now create a "device information set" which is required to enumerate all the ports
  HDEVINFO hDevInfoSet = lpfnSETUPDIGETCLASSDEVS(pGuids, NULL, NULL, DIGCF_PRESENT);
  if (hDevInfoSet == INVALID_HANDLE_VALUE)
  {
    //Free up the memory before we return
    delete [] pGuids;

    //Unload the setup dll
    FreeLibrary(hSetupAPI);

    return FALSE;
  }

  //Finished with the Guids by this time
  delete [] pGuids;
  pGuids = NULL;

  //Finally do the enumeration
  BOOL bMoreItems = TRUE;
  int nIndex = 0;
  SP_DEVINFO_DATA devInfo;
  while (bMoreItems)
  {
    //Enumerate the current device
    devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
    bMoreItems = lpfnSETUPDIENUMDEVICEINFO(hDevInfoSet, nIndex, &devInfo);
    if (bMoreItems)
    {
      //Did we find a serial port for this device
      BOOL bAdded = FALSE;

      //Get the registry key which stores the ports settings
      HKEY hDeviceKey = lpfnLPSETUPDIOPENDEVREGKEY(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
      if (hDeviceKey)
      {
        //Read in the name of the port
        TCHAR pszPortName[256];
        DWORD dwSize = sizeof(pszPortName);
        DWORD dwType = 0;
  	    if ((RegQueryValueEx(hDeviceKey, _T("PortName"), NULL, &dwType, (LPBYTE) pszPortName, &dwSize) == ERROR_SUCCESS) && (dwType == REG_SZ))
        {
          //If it looks like "COMX" then
          //add it to the array which will be returned
          int nLen = _tcslen(pszPortName);
          if (nLen > 3)
          {
            if ((_tcsnicmp(pszPortName, _T("COM"), 3) == 0) && IsNumeric(&pszPortName[3], FALSE))
            {
              //Work out the port number
              int nPort = _ttoi(&pszPortName[3]);
              ports.Add(nPort);

              bAdded = TRUE;
            }
          }
        }

        //Close the key now that we are finished with it
        RegCloseKey(hDeviceKey);
      }

      //If the port was a serial port, then also try to get its friendly name
      if (bAdded)
      {
        TCHAR pszFriendlyName[256];
        DWORD dwSize = sizeof(pszFriendlyName);
        DWORD dwType = 0;
        if (lpfnSETUPDIGETDEVICEREGISTRYPROPERTY(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType, (PBYTE)pszFriendlyName, dwSize, &dwSize) && (dwType == REG_SZ))
          sFriendlyNames.Add(pszFriendlyName);
        else
          sFriendlyNames.Add(_T(""));
      }
    }

    ++nIndex;
  }

  //Free up the "device information set" now that we are finished with it
  lpfnSETUPDIDESTROYDEVICEINFOLIST(hDevInfoSet);

  //Unload the setup dll
  FreeLibrary(hSetupAPI);

  //Return the success indicator
  return TRUE;
}
#endif

#ifndef NO_ENUMSERIAL_USING_ENUMPORTS
BOOL CEnumerateSerial::UsingEnumPorts(CUIntArray& ports)
{
  //Make sure we clear out any elements which may already be in the array
  ports.RemoveAll();

  //Call the first time to determine the size of the buffer to allocate
  DWORD cbNeeded = 0;
  DWORD dwPorts = 0;
  EnumPorts(NULL, 1, NULL, 0, &cbNeeded, &dwPorts);

  //What will be the return value
  BOOL bSuccess = FALSE;

  //Allocate the buffer and recall
  BYTE* pPorts = (BYTE*) _alloca(cbNeeded);
  bSuccess = EnumPorts(NULL, 1, pPorts, cbNeeded, &cbNeeded, &dwPorts);
  if (bSuccess)
  {
    PORT_INFO_1* pPortInfo = (PORT_INFO_1*) pPorts;
    for (DWORD i=0; i<dwPorts; i++)
    {
      //If it looks like "COMX" then
      //add it to the array which will be returned
      int nLen = _tcslen(pPortInfo->pName);
      if (nLen > 3)
      {
        if ((_tcsnicmp(pPortInfo->pName, _T("COM"), 3) == 0) && IsNumeric(&pPortInfo->pName[3], TRUE))
        {
          //Work out the port number
          int nPort = _ttoi(&pPortInfo->pName[3]);
          ports.Add(nPort);
        }
      }

      pPortInfo++;
    }
  }
  
  return bSuccess;
}
#endif


