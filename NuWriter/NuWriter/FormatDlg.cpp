// FormatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NuWriter.h"
#include "FormatDlg.h"


// CFormatDlg dialog

IMPLEMENT_DYNAMIC(CFormatDlg, CDialog)

CFormatDlg::CFormatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFormatDlg::IDD, pParent)
{

}

CFormatDlg::~CFormatDlg()
{
}

void CFormatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FORMAT_SPACE, m_reserve);
}


BEGIN_MESSAGE_MAP(CFormatDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CFormatDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CFormatDlg message handlers

void CFormatDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_reserve.GetWindowText(space);
	if(space.IsEmpty())
	{
		AfxMessageBox(_T("Please input reserve space"));
		return ;
	}
	
	OnOK();
}
