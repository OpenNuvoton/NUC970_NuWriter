#pragma once
#include "afxwin.h"


// CSelChipDlg dialog

class CSelChipDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelChipDlg)

public:
	CSelChipDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelChipDlg();

// Dialog Data
	enum { IDD = IDD_TARGET_CHIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void DisplayFormat();
	DECLARE_MESSAGE_MAP()
public:
	CString szDirForDDR;	
	char *DDRBuf;

	afx_msg void OnBnClickedOk();
	CComboBox m_chip;
	CComboBox m_ddrtype;
	CStatic m_ddr_static;

	int Counter;
	CString TimeString;	
public:
	afx_msg void OnCbnSelchangeCombo1();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	FooButton m_countinue;
	FooButton m_quit;
	CButton m_check;

	afx_msg void OnBnClickedCheck1();
	afx_msg void OnCbnDropdownComboDdr();
	afx_msg void OnCbnDropdownCombo1();
	
};
