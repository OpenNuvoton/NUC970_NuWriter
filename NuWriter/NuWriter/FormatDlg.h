#pragma once
#include "afxwin.h"


// CFormatDlg dialog

class CFormatDlg : public CDialog
{
	DECLARE_DYNAMIC(CFormatDlg)

public:
	CFormatDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFormatDlg();

// Dialog Data
	enum { IDD = IDD_FORMAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_reserve;
	afx_msg void OnBnClickedOk();
	CString space;
};
