// AddFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AddFileDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddFileDialog dialog

//IMPLEMENT_DYNAMIC(CAddFileDialog, CFileDialog)

CAddFileDialog::CAddFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
                               DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
    CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}



BEGIN_MESSAGE_MAP(CAddFileDialog, CFileDialog)
    //{{AFX_MSG_MAP(CAddFileDialog)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddFileDialog message handlers


void CAddFileDialog::OnInitDone()
{

    GetParent()->GetDlgItem(IDOK)->SetWindowText(_T("Choose"));
    GetParent()->GetDlgItem(IDCANCEL)->SetWindowText(_T("Cancel"));

}
