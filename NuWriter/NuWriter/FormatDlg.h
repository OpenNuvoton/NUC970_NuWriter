#pragma once
#include "afxwin.h"
#include "SmartEdit.h"
#include "Sliders.h"

// CFormatDlg dialog

class CFormatDlg : public CDialog
{
	DECLARE_DYNAMIC(CFormatDlg)

public:
	CFormatDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFormatDlg();

// Dialog Data
	enum { IDD = IDD_FORMAT };

	CSmartEdit	m_Edit1;
    CSmartEdit	m_Edit2;
    CSmartEdit	m_Edit3;
	CSmartEdit	m_Edit4;

    CLinkSlider	m_Slider1;
    CLinkSlider	m_Slider2;
	CLinkSlider	m_Slider3;
    CLinkSlider	m_Slider4;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void HScroll( UINT ncode, UINT pos );

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bInit;
	CEdit m_reserve;
	afx_msg void OnBnClickedOk();
	CString strResSize, strPartition1Size, strPartition2Size, strPartition3Size, strPartition4Size, strPartitionNum;
	int m_PartitionNum;
    int TotalSize, TotalMB, resSizeMB, ParTotalMB;
	int m_CtrlCnt;
	int Partition1Size, Partition2Size, Partition3Size, Partition4Size, PartitionNum;
	virtual BOOL OnInitDialog();
	void InitDialogWnd(bool);
	void ButtonCTRL(bool bShow);
	void SliderCTRL(bool bShow);
	afx_msg void OnBnClickedFormatBtnadd();
	afx_msg void OnBnClickedFormatBtnset();
	afx_msg void OnBnClickedFormatBtnrst();
	CButton m_BtnAdd;
	CButton m_BtnSet;
	CButton m_BtnRst;

};
