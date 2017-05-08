#pragma once
#include "NuWriterDlg.h"
#include "afxwin.h"

// CEraseDlg dialog

class CEraseDlg : public CDialog
{
	DECLARE_DYNAMIC(CEraseDlg)

public:
	CEraseDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEraseDlg();

// Dialog Data
	enum { IDD = IDD_ERASE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
    CEdit m_sblocks;  //start block
	CEdit m_blocks;   //end block

	CString SizeName;
	CString StartName;
	int type;  /* 0:RAWSPI 1:RAWNAND 2:RAWMMC */
	FooButton m_cancel;
	FooButton m_ok;
	CStatic m_sizename;
	CStatic m_startname;

	CString block;
	CString sblock;
	int m_erase_type;

	CString	EraseInfo;
	CStatic m_erase_info;
};
