#pragma once


// CNandInfoDlg ��ܤ��

class CNandInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNandInfoDlg)

public:
	CNandInfoDlg(CWnd* pParent = NULL);   // �зǫغc�禡
	virtual ~CNandInfoDlg();

// ��ܤ�����
	enum { IDD = IDD_DIALOG_NANDINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

	DECLARE_MESSAGE_MAP()
public:
	CString m_BlockPerFlash;
	CString m_PagePerBlock;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	DWORD BlockPerFlash, PagePerBlock;
};
