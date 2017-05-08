#ifndef __PACKDLG_H__
#define __PACKDLG_H__

#pragma once

#include "afxwin.h"
#include "NuWriterDlg.h"
#include <vector>
#include "afxcmn.h"

#include "PackTab1.h"
#include "PackTab2.h"
// CPACKDlg dialog

class CPACKDlg : public CDialog
{
	DECLARE_DYNAMIC(CPACKDlg)

public:
	CPACKDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPACKDlg();

// Dialog Data
	enum { IDD = IDD_PACK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPackAdd();
	afx_msg void OnBnClickedPackModify();
	afx_msg void OnBnClickedPackDelete();
	afx_msg void OnBnClickedPackOutput();
	//afx_msg void OnBnClickedPackEncE();

	CReportCtrl	m_imagelist;
	
	CString m_filename2;
	CString m_blocks;
	HANDLE m_ExitEvent;
	CProgressCtrl	m_progress;
	

	//CString m_filename;
	//CString	m_imagename;
	//CString	m_startblock;
	//CString	m_execaddr;
	//int		m_type;
	//CButton m_pack_enc_check;
	//CComboBox m_combo_encrypt;
	//CString m_encryptName;

	//FooButton m_browse;
	FooButton m_add;
	FooButton m_modify;
	FooButton m_delete;
	FooButton m_output;

	std::vector<CString> ImageName;
	std::vector<int> ImageType;
	std::vector<CString> ImageExec;	
	std::vector<CString> ImageStartblock;
	std::vector<CString> ImageEnc;	
	std::vector<CString> ImageEncName;

	CString TmpOffset;
	int TmpOffsetFlag;	
	int modifyflag;
	
	static unsigned WINAPI Output_proc(void* args);
	int Output();
	BOOL InitFile(int flag);
	int InitFlag;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnNMDblclkPackImagelist(NMHDR *pNMHDR, LRESULT *pResult);
	
	BOOL FileExist(CString strFileName);
	CStatic m_status;
	CTabCtrl m_pack_tabcontrol;

	CPackTab1 m_packtab1;
	CPackTab2 m_packtab2;
	void SwapTab(void);
	afx_msg void OnTcnSelchangePackTabcontrol(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif