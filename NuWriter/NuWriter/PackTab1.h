#ifndef __PACTAB1_H__
#define __PACTAB1_H__

#pragma once

#include "afxwin.h"
#include "NuWriterDlg.h"
#include <vector>
#include "afxcmn.h"
// CPackTab1 dialog

class CPackTab1 : public CDialog
{
	DECLARE_DYNAMIC(CPackTab1)

public:
	CPackTab1(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPackTab1();

// Dialog Data
	enum { IDD = IDD_PACK_TAB1 };

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedPackBrowse();
    afx_msg void OnBnClickedPackEncE();
	FooButton m_browse;
	CString m_filename;
	CString	m_imagename;
	CString	m_startblock;
	CString	m_execaddr;
	int		m_type;
	CButton m_pack_enc_check;
	CComboBox m_combo_encrypt;	
	CString m_encryptName;

	CString TmpOffset;
	int TmpOffsetFlag;	
	int modifyflag;
};

#endif //__PACTAB1_H__