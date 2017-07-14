
#ifndef __RAMDLG_H__
#define __RAMDLG_H__
#pragma once


// CRAMDlg dialog
#include "NuWriterDlg.h"
#include "afxwin.h"

class CRAMDlg : public CDialog
{
	DECLARE_DYNAMIC(CRAMDlg)

public:
	CRAMDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRAMDlg();

// Dialog Data
	enum { IDD = IDD_SDRAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSdramDownload();
	afx_msg void OnBnClickedSdramBrowse();	
	CProgressCtrl	m_progress;
	CString	m_fileinfo;
	CString	m_dtbinfo;
	CString	m_address;
	CString m_dtbaddress;
	HANDLE m_ExitEvent;
	CString m_filename;
	CString m_dtbname;
	CStatic sdramstatus;
	int		m_autorun;

	BOOL XUSB(CString& portName,CString& m_pathName,CString address,int autorun);
	void Download();
	static unsigned WINAPI Download_proc(void* args);
	LRESULT ShowStatus( WPARAM  pos, LPARAM message);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	FooButton m_download;
	FooButton m_browse;
	FooButton m_dtbbrowse;
	CButton m_dtb_en;

	BOOL InitFile(int flag);
	int InitFlag;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedDtbBrowse();
	afx_msg void OnBnClickedDtbEn();
};

#endif
