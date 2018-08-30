#pragma once


// CNandInfoDlg 對話方塊

class CNandInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNandInfoDlg)

public:
	CNandInfoDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CNandInfoDlg();

// 對話方塊資料
	enum { IDD = IDD_DIALOG_NANDINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	CString m_BlockPerFlash;
	CString m_PagePerBlock;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	DWORD BlockPerFlash, PagePerBlock;
};
