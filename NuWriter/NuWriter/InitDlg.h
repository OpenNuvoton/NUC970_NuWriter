
#ifndef __INITDLG_H__
#define __INITDLG_H__
#pragma once

#include "stdafx.h"
#include "winusb.h"

#define WM_CLOSEIT WM_USER+6
#define WM_SHOWPROGRESS WM_USER+7
#define FW_CODE 2

// CInitDlg dialog

class CInitDlg : public CDialog
{
//	DECLARE_DYNAMIC(CInitDlg)

public:
	CInitDlg(CWnd* pParent = NULL);   // standard constructor
	//virtual ~CInitDlg();

// Dialog Data
	enum { IDD = IDD_INIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
		CString m_port;
//		CSerial	m_vcom;
		CProgressCtrl	m_progress;
		CString	m_status;
		CString m_version;

		VOID SetData(void);
		CString GetVersion() { return m_version; }
		static unsigned WINAPI Download_proc(void* args);
		VOID Download();
		LRESULT Close( WPARAM  p1, LPARAM p2);
		LRESULT ShowProgress( WPARAM  p1, LPARAM p2);
		BOOL XUSB(CString& m_pathName);
		int DDRtoDevice(char *buf,unsigned int len);
		afx_msg void OnNMCustomdrawDownprogress(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif