// MTPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NuWriter.h"
#include "NuWriterDlg.h"
#include "MTPDlg.h"


// CMTPDlg dialog

IMPLEMENT_DYNAMIC(CMTPDlg, CDialog)

CMTPDlg::CMTPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMTPDlg::IDD, pParent)
	, m_shafilename(_T(""))
{
	m_mtp_option=0;
	m_mtp_mode=0;
	m_aesfilename=_T("");
}

CMTPDlg::~CMTPDlg()
{
}

void CMTPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MTP_DOWNLOAD, m_burn);
	DDX_Control(pDX, IDC_MTP_MODIFY, m_modify);
	DDX_Control(pDX, IDC_MTP_COMBO_ENCRYPT, m_combo_encrypt);
	DDX_Radio(pDX, IDC_MTP_A0, m_mtp_option);
	DDX_Radio(pDX, IDC_MTP_M0, m_mtp_mode);
	DDX_Radio(pDX, IDC_MTP_ENABLE, m_mtp_encrypt);
	DDX_Control(pDX, IDC_MTP_DOWNPROGRESS, m_progress);
	DDX_Control(pDX, IDC_MTP_PSTATIC, m_pstatus);
	DDX_Control(pDX, IDC_MTP_STATUS, m_mtp_status);
	DDX_Text(pDX, IDC_MTP_SHAFILENAME, m_shafilename);
}


BEGIN_MESSAGE_MAP(CMTPDlg, CDialog)
	ON_BN_CLICKED(IDC_MTP_DOWNLOAD, &CMTPDlg::OnBnClickedMtpDownload)
	ON_BN_CLICKED(IDC_MTP_MODIFY, &CMTPDlg::OnBnClickedMtpModify)
	ON_MESSAGE(WM_MTP_PROGRESS,ShowStatus)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CMTPDlg message handlers
BOOL CMTPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ExitEvent=CreateEvent(NULL,TRUE,FALSE,NULL);



	COLORREF col = RGB(0xFF, 0x00, 0xFF);
	m_modify.setBitmapId(IDB_ERASEALL, col);
	m_modify.setGradient(true);
	m_burn.setBitmapId(IDB_WRITE_DEVICE, col);
	m_burn.setGradient(true);
	InitFile(0);
	UpdateData(TRUE);

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
	//m_combo_encrypt.GetLBText(0, m_aesfilename);

	if(m_mtp_option==0)
	{
#if(0)
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
#endif
		//-----------------------------------------------------------------------
		GetDlgItem(IDC_MTP_MODIFY)->SetWindowText(_T("Modify"));
		COLORREF col = RGB(0xFF, 0x00, 0xFF);
		m_modify.setBitmapId(IDB_ERASEALL, col);
		m_modify.setGradient(true);
	}else{

		GetDlgItem(IDC_MTP_MODIFY)->SetWindowText(_T("Open"));
		COLORREF col = RGB(0xFF, 0x00, 0xFF);
		m_modify.setBitmapId(IDB_BROWSE, col);
		m_modify.setGradient(true);
		//m_combo_encrypt.ResetContent();
		//m_combo_encrypt.AddString(m_aesfilename);
		//m_combo_encrypt.SetCurSel(0);
	}

	OnCommand(NULL,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMTPDlg:: Download()
{

	int i=0;
	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
	CString tmp;

	//UpdateData(FALSE);  //shanchun modified 20121003
	if(m_mtp_option==0) // AES
	{
		CString szDirForKEY,KeyName;
		mainWnd->GetExeDir(szDirForKEY);
		szDirForKEY=szDirForKEY+_T("key_cfg\\");
		m_combo_encrypt.GetLBText(m_combo_encrypt.GetCurSel(),KeyName);
		szDirForKEY=szDirForKEY+KeyName;
		m_filename = szDirForKEY;
	}
	else
		m_filename = m_shafilename;

	InitFile(1);
	TRACE(_T("m_filename = %s\n"), m_filename);

	if(!m_filename.IsEmpty())
	{
		mainWnd->m_gtype.EnableWindow(FALSE);
		ResetEvent(m_ExitEvent);
		GetDlgItem(IDC_MTP_DOWNLOAD)->SetWindowText(_T("Abort"));

		#if 0
		if(XUSB_Burn(mainWnd->m_portName,m_filename))
			AfxMessageBox(_T("Burn successfully"));
		#else
		XUSB_Burn(mainWnd->m_portName,m_filename);
		#endif
		//else
		//	AfxMessageBox("Download unsuccessfully!! Please check device");
		GetDlgItem(IDC_MTP_DOWNLOAD)->EnableWindow(TRUE);
		GetDlgItem(IDC_MTP_DOWNLOAD)->SetWindowText(_T("Burn"));
		//this->UpdateData(FALSE);  //shanchun modified 20121003
		mainWnd->m_gtype.EnableWindow(TRUE);

	}
	else
		AfxMessageBox(_T("Please choose image file !"));

	return ;

}

unsigned WINAPI CMTPDlg:: Download_proc(void* args)
{
	CMTPDlg* pThis = reinterpret_cast<CMTPDlg*>(args);
	pThis->Download();
	return 0;
}

void CMTPDlg::OnBnClickedMtpDownload()
{
	CString dlgText;

	UpdateData(TRUE);

	GetDlgItem(IDC_MTP_DOWNLOAD)->GetWindowText(dlgText);

	if( ((CButton*)GetDlgItem(IDC_CHECK_LOCK_ENABLE))->GetCheck() )
		if(::MessageBox(this->m_hWnd,_T("You have select lock mtp key, once mtp key is locked can not be program any more."),_T("Nu Writer"),MB_OKCANCEL|MB_ICONWARNING)==IDCANCEL)
			return;

	if(m_mtp_option==1)
	{
		if(m_shafilename.IsEmpty())
		{
			AfxMessageBox(_T("Please input binary file to do SHA\n"));
			return;
		}

	}

	if(dlgText.CompareNoCase(_T("Burn"))==0)
	{

		UpdateData(TRUE);

		unsigned Thread1;
		HANDLE hThread;

	 	m_progress.SetPos(0);
		m_progress.SetBkColor(COLOR_DOWNLOAD);
		hThread=(HANDLE)_beginthreadex(NULL,0,Download_proc,(void*)this,0,&Thread1);
		CloseHandle(hThread);
	}
	else
	{
		SetEvent(m_ExitEvent);
		GetDlgItem(IDC_MTP_DOWNLOAD)->EnableWindow(FALSE);
		m_progress.SetPos(0);
	}

}

void CMTPDlg::OnBnClickedMtpModify()
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

		// m_aesfilename=dlg.GetPathName();
		// m_combo_encrypt.ResetContent();
		// m_combo_encrypt.AddString(m_aesfilename);
		// m_combo_encrypt.SetCurSel(0);
		m_shafilename = dlg.GetPathName();
		GetDlgItem(IDC_MTP_SHAFILENAME)->SetWindowText(m_shafilename);
	}
}

LRESULT CMTPDlg::ShowStatus( WPARAM  pos, LPARAM message)
{
	m_progress.SetPos((int)pos);
	return true;
}


BOOL CMTPDlg::InitFile(int flag)
{
	CString tmp;
	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
	if(!mainWnd->m_inifile.ReadFile()) return false;
	switch(flag)
	{
		case 0:
			/* USB, eMMC, NAND, SPI FLASH / USB without SHA, USB with SHA */
			tmp=mainWnd->m_inifile.GetValue(_T("MTP"),_T("MTPMode"));
			((CButton *)GetDlgItem(IDC_MTP_M0))->SetCheck(FALSE);
			switch(_wtoi(tmp))
			{
			    /*  USB */
				case 0: ((CButton *)GetDlgItem(IDC_MTP_M0))->SetCheck(TRUE); break;
				/*  eMMC/USB without SHA  */
				case 1: ((CButton *)GetDlgItem(IDC_MTP_M1))->SetCheck(TRUE);
						((CButton *)GetDlgItem(IDC_MTP_M11))->SetCheck(TRUE); break;
				/*  NAND  */
				case 2: ((CButton *)GetDlgItem(IDC_MTP_M2))->SetCheck(TRUE);
						((CButton *)GetDlgItem(IDC_MTP_M11))->SetCheck(TRUE); break;
				/*  SPI Flash/USB with SHA  */
				case 3: ((CButton *)GetDlgItem(IDC_MTP_M3))->SetCheck(TRUE);
						((CButton *)GetDlgItem(IDC_MTP_M12))->SetCheck(TRUE); break;
			}
			/* AES, SHA */
			tmp=mainWnd->m_inifile.GetValue(_T("MTP"),_T("MTPOption"));
			((CButton *)GetDlgItem(IDC_MTP_A0))->SetCheck(FALSE);
			switch(_wtoi(tmp))
			{
				case 0: ((CButton *)GetDlgItem(IDC_MTP_A0))->SetCheck(TRUE);
						GetDlgItem(IDC_MTP_COMBO_ENCRYPT)->ShowWindow(SW_SHOW); break;
				case 1: ((CButton *)GetDlgItem(IDC_MTP_A1))->SetCheck(TRUE);
						GetDlgItem(IDC_MTP_SHAFILENAME)->ShowWindow(SW_SHOW); break;
			}
			/* ENABLE, DISABLE */
			tmp=mainWnd->m_inifile.GetValue(_T("MTP"),_T("MTPEncrypt"));
			((CButton *)GetDlgItem(IDC_MTP_ENABLE))->SetCheck(FALSE);
			switch(_wtoi(tmp))
			{
				case 0: ((CButton *)GetDlgItem(IDC_MTP_ENABLE))->SetCheck(TRUE); break;
				case 1: ((CButton *)GetDlgItem(IDC_MTP_DISABLE))->SetCheck(TRUE); break;
			}
			break;
		case 1:
			tmp.Format(_T("%d"),m_mtp_option);
			mainWnd->m_inifile.SetValue(_T("MTP"),_T("MTPOption"),tmp);
			tmp.Format(_T("%d"),m_mtp_mode);
			mainWnd->m_inifile.SetValue(_T("MTP"),_T("MTPMode"),tmp);
			tmp.Format(_T("%d"),m_mtp_encrypt);
			mainWnd->m_inifile.SetValue(_T("MTP"),_T("MTPEncrypt"),tmp);
			mainWnd->m_inifile.WriteFile();
			break;
	}
	return true;
}
HBRUSH CMTPDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if (pWnd->GetDlgCtrlID() == IDC_LOCK_COMMENT)
	{
		CString word;
		GetDlgItem(IDC_LOCK_COMMENT)->GetWindowText(word);
		pDC->SetTextColor(RGB(255,0,0));
	}

	return hbr;
}

BOOL CMTPDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);

	if(((CButton *)GetDlgItem(IDC_MTP_ENABLE))->GetCheck())
		m_mtp_encrypt = 0;
	else
	    m_mtp_encrypt = 1;

    if(m_mtp_option==0) // AES
	{
		GetDlgItem(IDC_MTP_COMBO_ENCRYPT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_MTP_SHAFILENAME)->ShowWindow(SW_HIDE);
		//---Check ddr dirctory has how much *.ini-------------------------------
		#if 0
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
		#endif
		//-----------------------------------------------------------------------
		GetDlgItem(IDC_MTP_MODIFY)->SetWindowText(_T("Modify"));
		COLORREF col = RGB(0xFF, 0x00, 0xFF);
		m_modify.setBitmapId(IDB_ERASEALL, col);
		m_modify.setGradient(true);

		//--------------------------------------------------------
#if(0)
		CRect r;
		GetDlgItem(IDC_MTP_M1)->GetWindowRect(&r);
		ScreenToClient(&r);
		r.right=0xa3;
		GetDlgItem(IDC_MTP_M1)->SetWindowText(_T("eMMC"));
		GetDlgItem(IDC_MTP_M1)->MoveWindow(r,TRUE);

		GetDlgItem(IDC_MTP_M2)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_MTP_M3)->GetWindowRect(&r);
		ScreenToClient(&r);
		r.right=0x130;
		GetDlgItem(IDC_MTP_M3)->MoveWindow(r,TRUE);
		GetDlgItem(IDC_MTP_M3)->SetWindowText(_T("SPIFLASH"));
#endif
		GetDlgItem(IDC_MTP_M11)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MTP_M12)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MTP_M1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_MTP_M2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_MTP_M3)->ShowWindow(SW_SHOW);
		TRACE(_T("AES m_mtp_option = %d, m_mtp_mode = %d, m_mtp_encrypt= %d\n"), m_mtp_option, m_mtp_mode, m_mtp_encrypt);
		//--------------------------------------------------------
	}else {  // SHA
		GetDlgItem(IDC_MTP_COMBO_ENCRYPT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MTP_SHAFILENAME)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_MTP_MODIFY)->SetWindowText(_T("Open"));
		COLORREF col = RGB(0xFF, 0x00, 0xFF);
		m_modify.setBitmapId(IDB_BROWSE, col);
		m_modify.setGradient(true);
		//m_combo_encrypt.ResetContent();
		//m_combo_encrypt.AddString(m_aesfilename);
		//m_combo_encrypt.SetCurSel(0);

		//--------------------------------------------------------
#if(0)
		if(m_mtp_mode==2) {
			m_mtp_mode=1;
			UpdateData(FALSE);
		}
		CRect r;
		GetDlgItem(IDC_MTP_M1)->GetWindowRect(&r);
		ScreenToClient(&r);
		r.right=0xa3+50;
		GetDlgItem(IDC_MTP_M1)->MoveWindow(r,TRUE);
		GetDlgItem(IDC_MTP_M1)->SetWindowText(_T("USB without SHA"));

		GetDlgItem(IDC_MTP_M2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MTP_M3)->GetWindowRect(&r);
		ScreenToClient(&r);
		r.right=0x130+50;
		GetDlgItem(IDC_MTP_M3)->MoveWindow(r,TRUE);
		GetDlgItem(IDC_MTP_M3)->SetWindowText(_T("USB with SHA"));
#endif
		if(((CButton *)GetDlgItem(IDC_MTP_M11))->GetCheck())
			m_mtp_mode = 1;
		else
			m_mtp_mode = 3;

		GetDlgItem(IDC_MTP_M1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MTP_M2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MTP_M3)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_MTP_M11)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_MTP_M12)->ShowWindow(SW_SHOW);
		TRACE(_T("SHA m_mtp_option = %d, m_mtp_mode = %d, m_mtp_encrypt= %d\n"), m_mtp_option, m_mtp_mode, m_mtp_encrypt);

		//--------------------------------------------------------
	}
	return CDialog::OnCommand(wParam, lParam);
}