#pragma once
#include "afxwin.h"


// CMTPDlg dialog

class CMTPDlg : public CDialog
{
	DECLARE_DYNAMIC(CMTPDlg)

public:
	CMTPDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMTPDlg();

// Dialog Data
	enum { IDD = IDD_MTP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()	
public:
	FooButton m_burn;
	FooButton m_modify;
	afx_msg void OnBnClickedMtpBrowse();
	afx_msg void OnBnClickedMtpDownload();
	afx_msg void OnBnClickedMtpModify();
	CComboBox m_combo_encrypt;
	HANDLE m_ExitEvent;
	int m_mtp_option;
	int m_mtp_mode;
	int m_mtp_encrypt;
	CProgressCtrl	m_progress;
	CString m_filename;
	LRESULT ShowStatus( WPARAM  pos, LPARAM message);
	BOOL XUSB_Burn(CString& portName,CString& m_pathName);
	static unsigned WINAPI Download_proc(void* args);
	VOID Download();
	BOOL InitFile(int flag);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CStatic m_pstatus;
	CString m_aesfilename;
	CStatic m_mtp_status;
	CString m_shafilename;
};
