#pragma once
#include "afxwin.h"


// CNANDDlg dialog

class CNANDDlg : public CDialog
{
	DECLARE_DYNAMIC(CNANDDlg)

public:
	CNANDDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNANDDlg();

// Dialog Data
	enum { IDD = IDD_NAND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	LRESULT ShowStatus( WPARAM  pos, LPARAM message);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedNandDownload();

	BOOL CNANDDlg::XUSB_Pack(CString& portName,CString& m_pathName,int *len);
	BOOL XUSB_Burn(CString& portName,CString& m_pathName,int *len,int *blockNum);
	BOOL XUSB_BurnWithOOB(CString& portName,CString& m_pathName,int *len,int *blockNum);	
	static unsigned WINAPI Download_proc(void* args);
	void Download();

	int XUSB_Verify(CString& portName,CString& m_pathName);
	static unsigned WINAPI Verify_proc(void* args);
	void Verify();
		
	BOOL XUSB_Read(CString& portName,CString& m_pathName,unsigned int addr,unsigned int len);
	BOOL XUSB_Read_Redunancy(CString& portName,CString& m_pathName,unsigned int addr,unsigned int len);
	static unsigned WINAPI Read_proc(void* args);
	void Read();
	
	BOOL XUSB_Erase(CString& portName);
	static unsigned WINAPI Erase_proc(void* args);
	void Erase();


	CReportCtrl	m_imagelist;
	CString m_filename;	
	CString m_filename2;
	CString m_blocks;
	CString m_sblocks;
	int m_erase_flag;
	CString m_imagename;
	HANDLE m_ExitEvent;
	CProgressCtrl	m_progress;
	FooButton m_burn;
	FooButton m_verify;
	FooButton m_read;
	FooButton m_eraseall;
	FooButton m_browse;
	afx_msg void OnBnClickedNandVerify();
	afx_msg void OnBnClickedNandRead();
	afx_msg void OnBnClickedNandEraseall();
	afx_msg void OnBnClickedNandBrowse();
	CButton m_nand_enc_check;
	CComboBox m_combo_encrypt;
	afx_msg void OnBnClickedNandEncE();
	int m_type;

	CString m_startblock;
	CString m_execaddr;

	NORBOOT_NAND_HEAD *m_fhead;
	CString TmpOffset;
	int TmpOffsetFlag;
	BOOL InitFile(int flag);
	int InitFlag;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	CStatic m_status;

	char *encbuf;
	unsigned int enclen;
	afx_msg void OnBnClickedNandTypeA();
	afx_msg void OnBnClickedNandUsrconfig();
	CButton m_nandflash_check;
};
