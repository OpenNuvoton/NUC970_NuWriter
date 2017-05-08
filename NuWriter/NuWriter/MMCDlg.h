#pragma once
#include "afxwin.h"


// CMMCDlg dialog

class CMMCDlg : public CDialog
{
	DECLARE_DYNAMIC(CMMCDlg)

public:
	CMMCDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMMCDlg();

// Dialog Data
	enum { IDD = IDD_MMC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	LRESULT ShowStatus( WPARAM  pos, LPARAM message);
	DECLARE_MESSAGE_MAP()
public:
	
	CReportCtrl	m_imagelist;
	CString m_filename;	
	CString m_filename2;
	CString m_blocks;
	CString m_sblocks;
	CString m_imagename;
	HANDLE m_ExitEvent;
	CProgressCtrl	m_progress;

	BOOL XUSB_Pack(CString& portName,CString& m_pathName,int *len);
	BOOL XUSB_Burn(CString& portName,CString& m_pathName,int *len);
	static unsigned WINAPI Download_proc(void* args);
	void Download();

	BOOL XUSB_Verify(CString& portName,CString& m_pathName);
	static unsigned WINAPI Verify_proc(void* args);
	void Verify();
		
	BOOL XUSB_Read(CString& portName,CString& m_pathName,unsigned int addr,unsigned int len);
	static unsigned WINAPI Read_proc(void* args);
	void Read();
	
	BOOL XUSB_Format(CString& portName);
	static unsigned WINAPI Format_proc(void* args);
	void Format();


	afx_msg void OnBnClickedMmcDownload();
	afx_msg void OnBnClickedMmcFormat();
	afx_msg void OnBnClickedMmcVerify();
	afx_msg void OnBnClickedMmcRead();
	afx_msg void OnBnClickedMmcEraseall();
	afx_msg void OnBnClickedMmcBrowse();

	// FooButton
	FooButton m_format;
	FooButton m_eraseall;
	FooButton m_read;
	FooButton m_verify;
	FooButton m_burn;
	FooButton m_browse;
	afx_msg void OnBnClickedMmcEncE();
	CButton m_mmc_enc_check;
	CComboBox m_combo_encrypt;
	int m_type;

	CString m_startblock;
	CString m_execaddr;
	

	NORBOOT_MMC_HEAD *m_fhead;
	CString TmpOffset;
	int TmpOffsetFlag;
	BOOL InitFile(int flag);
	int InitFlag;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);	
	CString m_space;
	CStatic m_status;

	char *encbuf;
	unsigned int enclen;
};
