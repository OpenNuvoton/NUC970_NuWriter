#ifndef __SPIDLG_H__
#define __SPIDLG_H__

#pragma once
#include "afxwin.h"
#include "NuWriterDlg.h"


// CSPIDlg dialog

class CSPIDlg : public CDialog
{
	DECLARE_DYNAMIC(CSPIDlg)

public:
	CSPIDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSPIDlg();

// Dialog Data
	enum { IDD = IDD_SPI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	LRESULT ShowStatus( WPARAM  pos, LPARAM message);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSpiDownload();
	BOOL XUSB_Pack(CString& portName,CString& m_pathName,int *len);
	BOOL XUSB_Burn(CString& portName,CString& m_pathName,int *len);
	static unsigned WINAPI Download_proc(void* args);
	void Download();

	afx_msg void OnBnClickedSpiVerify();
	BOOL XUSB_Verify(CString& portName,CString& m_pathName);
	static unsigned WINAPI Verify_proc(void* args);
	void Verify();
	
	afx_msg void OnBnClickedSpiRead();
	BOOL XUSB_Read(CString& portName,CString& m_pathName,unsigned int addr,unsigned int len);
	//BOOL XUSB_Read(CString& portName,CString& m_pathName);	
	static unsigned WINAPI Read_proc(void* args);
	void Read();

	afx_msg void OnBnClickedSpiEraseall();
	BOOL XUSB_Erase(CString& portName);
	static unsigned WINAPI Erase_proc(void* args);
	void Erase();

	CReportCtrl	m_imagelist;
	CString m_filename;
	CString m_filename2;
	CString m_blocks;
	CString m_sblocks;
	int m_erase_flag;
	HANDLE m_ExitEvent;
	CProgressCtrl	m_progress;

	CString	m_imagename;	
	CString	m_startblock;
	CString	m_execaddr;
	int		m_type;

	CButton m_spi_enc_check;
	CComboBox m_combo_encrypt;

	NORBOOT_NAND_HEAD *m_fhead;	
	FooButton m_burn;
	FooButton m_verify;
	FooButton m_read;
	FooButton m_eraseall;	
	FooButton m_browse;		
	
	//CSPIReadDlg read_dlg;
	int nblock;
	afx_msg void OnBnClickedSpiBrowse();	
	afx_msg void OnBnClickedSpiEncE();

	CString TmpOffset;
	int TmpOffsetFlag;
	BOOL InitFile(int flag);
	int InitFlag;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	CStatic m_status;

	char *encbuf;
	unsigned int enclen;
};

#endif