// PackTab2.cpp : implementation file
//

#include "stdafx.h"
#include "NuWriter.h"
#include "NuWriterDlg.h"
#include "PackTab2.h"


// CPackTab2 dialog

IMPLEMENT_DYNAMIC(CPackTab2, CDialog)

CPackTab2::CPackTab2(CWnd* pParent /*=NULL*/)
	: CDialog(CPackTab2::IDD, pParent)
{

}

CPackTab2::~CPackTab2()
{
}

void CPackTab2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PACK_MTP_MODIFY, m_modify);
	DDX_Control(pDX, IDC_PACK_MTP_COMBO_ENCRYPT, m_combo_encrypt);
	DDX_Radio(pDX, IDC_PACK_MTP_A0, m_mtp_option);
	DDX_Radio(pDX, IDC_PACK_MTP_M0, m_mtp_mode);
	DDX_Radio(pDX, IDC_PACK_MTP_ENABLE, m_mtp_encrypt);
}


BEGIN_MESSAGE_MAP(CPackTab2, CDialog)
	ON_BN_CLICKED(IDC_PACK_MTP_MODIFY, &CPackTab2::OnBnClickedPackMtpModify)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CPackTab2 message handlers
BOOL CPackTab2::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ExitEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

	//---Check ddr dirctory has how much *.ini-------------------------------
	m_combo_encrypt.ResetContent();
	CString szDirForKEY,KeyName;
	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
	mainWnd->GetExeDir(szDirForKEY);
	szDirForKEY=szDirForKEY+_T("key_cfg\\");
	CString keydir;
	vector<CString> sList;
	vector<CString>::iterator item;	
	mainWnd->LoadDirINI(szDirForKEY,_T(".dat"),sList);
	int count=0;
	for(item=sList.begin();item!=sList.end();item++)	
		m_combo_encrypt.AddString(*item);			
	m_combo_encrypt.SetCurSel(0);		
	//-----------------------------------------------------------------------

	COLORREF col = RGB(0xFF, 0x00, 0xFF);			
	m_modify.setBitmapId(IDB_ERASEALL, col);
	m_modify.setGradient(true);	
	//m_burn.setBitmapId(IDB_WRITE_DEVICE, col);
	//m_burn.setGradient(true);
	//InitFile(0);

	((CButton *)GetDlgItem(IDC_PACK_MTP_M1))->SetCheck(TRUE);
	((CButton *)GetDlgItem(IDC_PACK_MTP_A0))->SetCheck(TRUE);
	((CButton *)GetDlgItem(IDC_PACK_MTP_ENABLE))->SetCheck(TRUE);
	UpdateData(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPackTab2::OnBnClickedPackMtpModify()
{
	if(m_mtp_option==0)
	{
		CFileFind fFind;
		CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);		

		CString szDirForKEY,KeyName;
		mainWnd->GetExeDir(szDirForKEY);
		szDirForKEY=szDirForKEY+_T("key_cfg\\");	
		m_combo_encrypt.GetLBText(m_combo_encrypt.GetCurSel(),KeyName);
		szDirForKEY=szDirForKEY+KeyName;								

		
		if(fFind.FindFile(szDirForKEY))
		{
			ShellExecute(NULL, _T("open"),
					_T("notepad.exe"),
					szDirForKEY,        // program to launch
										// parms (will be %1 and %2 in the batch file)
					NULL,				// default dir for the batch
					SW_SHOWNORMAL		//SW_SHOWNORMAL,SW_HIDE         
					);	
		}
	}else{
			CString tmp;
			//CAddFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,_T("All Files (*.*)|*.*||"));
			CAddFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,_T("Bin Files (*.bin)|*.bin|All Files (*.*)|*.*||"));
			dlg.m_ofn.lpstrTitle=_T("Choose burning file...");
			CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

					
			BOOLEAN ret=dlg.DoModal();

			if(ret==IDCANCEL)
			{
				return;
			}
			
			m_aesfilename=dlg.GetPathName();	
			m_combo_encrypt.ResetContent();
			m_combo_encrypt.AddString(m_aesfilename);			
			m_combo_encrypt.SetCurSel(0);
	}
}

HBRUSH CPackTab2::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	 if (pWnd->GetDlgCtrlID() == IDC_TAB_LOCK_COMMENT)
     {
		 CString word;
		 GetDlgItem(IDC_TAB_LOCK_COMMENT)->GetWindowText(word);
		 pDC->SetTextColor(RGB(255,0,0)); 		
     } 
	 
	return hbr;
}

BOOL CPackTab2::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
    if(m_mtp_option==0)
	{
		//---Check ddr dirctory has how much *.ini-------------------------------
		m_combo_encrypt.ResetContent();
		CString szDirForKEY,KeyName;
		CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
		mainWnd->GetExeDir(szDirForKEY);
		szDirForKEY=szDirForKEY+_T("key_cfg\\");
		CString keydir;
		vector<CString> sList;
		vector<CString>::iterator item;	
		mainWnd->LoadDirINI(szDirForKEY,_T(".dat"),sList);
		int count=0;
		for(item=sList.begin();item!=sList.end();item++)	
			m_combo_encrypt.AddString(*item);			
		m_combo_encrypt.SetCurSel(0);		
		//-----------------------------------------------------------------------
		GetDlgItem(IDC_PACK_MTP_MODIFY)->SetWindowText(_T("Modify"));
		COLORREF col = RGB(0xFF, 0x00, 0xFF);			
		m_modify.setBitmapId(IDB_ERASEALL, col);
		m_modify.setGradient(true);	
	}else{

		GetDlgItem(IDC_PACK_MTP_MODIFY)->SetWindowText(_T("Open"));
		COLORREF col = RGB(0xFF, 0x00, 0xFF);			
		m_modify.setBitmapId(IDB_BROWSE, col);
		m_modify.setGradient(true);	
		m_combo_encrypt.ResetContent();
        m_combo_encrypt.AddString(m_aesfilename);			
		m_combo_encrypt.SetCurSel(0);
	}
	return CDialog::OnCommand(wParam, lParam);
}