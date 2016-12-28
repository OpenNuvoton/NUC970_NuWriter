// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently
#ifndef __STDAFX_H__
#define __STDAFX_H__
#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//my project ------------------------------------------------
#define WM_SDRAM_PROGRESS	WM_USER+5
#define WM_SPI_PROGRESS		WM_USER+6
#define WM_MMC_PROGRESS		WM_USER+7
#define WM_NAND_PROGRESS	WM_USER+8
#define WM_PACK_PROGRESS	WM_USER+9
#define WM_MTP_PROGRESS	    WM_USER+10

#include <afxtempl.h>
#include <setupapi.h>
#include <winspool.h>
#include <WBemCli.h>
#include <comdef.h>
#include <Afxpriv.h>
#include <process.h>

#include <initguid.h>
#include <Dbt.h>
#include "Serial.h"
#include "ReportCtrl.h"
#include "AddFileDialog.h"
#include "FooButton.h"
#define COLOR_DOWNLOAD RGB(0, 255, 0)
#define COLOR_ERASE RGB(0, 0, 255)
#define COLOR_BURN RGB(255, 0, 0)
#define COLOR_VERIFY RGB(236, 190, 70)

//#define WDBG

#ifdef WDBG
#define DbgOut(Text) OutputDebugString(_T(Text))
#else
#define DbgOut(Text) 
#endif 


//#undef _MP //shanchun modified

//#pragma warning(disable: 4200)
//-----------------------------------------------------------




#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#endif