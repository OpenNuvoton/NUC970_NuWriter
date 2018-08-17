#ifndef __PACTAB2_H__
#define __PACTAB2_H__

#pragma once

#include "afxwin.h"
#include "NuWriterDlg.h"
#include <vector>
#include "afxcmn.h"
// CPackTab2 dialog

class CPackTab2 : public CDialog
{
	DECLARE_DYNAMIC(CPackTab2)

public:
	CPackTab2(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPackTab2();

// Dialog Data
	enum { IDD = IDD_PACK_TAB2 };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPackMtpModify();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	int m_mtp_option;
	int m_mtp_mode;
	int m_mtp_encrypt;
	CComboBox m_combo_encrypt;
	HANDLE m_ExitEvent;
	FooButton m_modify;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CString m_aesfilename;
	CString m_shafilename;
};

#endif //__PACTAB2_H__