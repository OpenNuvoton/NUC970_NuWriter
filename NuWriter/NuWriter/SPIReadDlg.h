
#ifndef __SPIREADDLG_H__
#define __SPIREADDLG_H__

#include "NuWriterDlg.h"
#include "afxwin.h"

#pragma once


// CSPIReadDlg dialog

class CReadDlg : public CDialog
{
	DECLARE_DYNAMIC(CReadDlg)

public:
	CReadDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CReadDlg();

// Dialog Data
	enum { IDD = IDD_READ };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();

	CString m_filename2; 
	CString m_fileinfo2; //savepath

    CEdit m_sblocks;  //start block
	CEdit m_blocks;   //end block
	afx_msg void OnBnClickedSpireadBroswse();
	FooButton m_browse;
	afx_msg void OnBnClickedOk();
	CString block;
	CString sblock;
		
	CString SizeName;	
	int type;  /* 0:RAWSPI 1:RAWNAND 2:RAWMMC */
	CStatic m_sizename;
	CString _path;
	FooButton m_cancel;
	FooButton m_ok;
	CStatic m_filepath;	
	CStatic m_startname;
	CString StartName;
};

#endif