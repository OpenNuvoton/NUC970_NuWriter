
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
    int iDevice;
    unsigned Thread0, Thread1,Thread2, Thread3, Thread4, Thread5, Thread6, Thread7;//Thread1[8];
    HANDLE hThread0, hThread1,hThread2,hThread3,hThread4,hThread5,hThread6,hThread7;//hThread[8];
    HANDLE m_ExitEvent;//m_ExitEvent[8];

    VOID SetData(void);
    CString GetVersion() {
        return m_version;
    }
    static unsigned WINAPI Download_proc(void* args);
    static unsigned WINAPI Download_proc0(void* args);
    static unsigned WINAPI Download_proc1(void* args);
    static unsigned WINAPI Download_proc2(void* args);
    static unsigned WINAPI Download_proc3(void* args);
    static unsigned WINAPI Download_proc4(void* args);
    static unsigned WINAPI Download_proc5(void* args);
    static unsigned WINAPI Download_proc6(void* args);
    static unsigned WINAPI Download_proc7(void* args);
    static unsigned WINAPI Download_proc8(void* args);
    VOID Download(int id);
    LRESULT Close( WPARAM  p1, LPARAM p2);
    LRESULT ShowProgress( WPARAM  p1, LPARAM p2);
    BOOL XUSB(int id, CString& m_pathName);
    BOOL DDRtoDevice(int id, char *buf,unsigned int len);
    afx_msg void OnNMCustomdrawDownprogress(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnClose();
};

#endif