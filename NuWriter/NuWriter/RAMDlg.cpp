// RAMDlg.cpp : implementation file
//
#include "stdafx.h"
#include "NuWriter.h"
#include "NuWriterDlg.h"
#include "RAMDlg.h"


// CRAMDlg dialog

IMPLEMENT_DYNAMIC(CRAMDlg, CDialog)

CRAMDlg::CRAMDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRAMDlg::IDD, pParent)
{
	m_fileinfo = _T("");
	m_address = _T("");
	m_autorun = 0;
	InitFlag=0;
}

CRAMDlg::~CRAMDlg()
{
}

void CRAMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	

	DDX_Control(pDX, IDC_SDRAM_DOWNPROGRESS, m_progress);
	DDX_Text(pDX, IDC_SDRAM_FILENAME, m_fileinfo);
	DDX_Text(pDX, IDC_DTB_FILENAME, m_dtbinfo);
	DDX_Text(pDX, IDC_SDRAM_BUFFER_ADDRESS, m_address);
	DDX_Text(pDX, IDC_DTB_BUFFER_ADDRESS, m_dtbaddress);
	DDV_MaxChars(pDX, m_address, 8);
	DDV_MaxChars(pDX, m_dtbaddress, 8);
	DDX_Radio(pDX, IDC_SDRAM_AUTORUN, m_autorun);	
	DDX_Control(pDX, IDC_SDRAMSTATUS, sdramstatus);

	DDX_Control(pDX, IDC_SDRAM_DOWNLOAD, m_download);
	DDX_Control(pDX, IDC_SDRAM_BROWSE, m_browse);
	DDX_Control(pDX, IDC_DTB_BROWSE, m_dtbbrowse);
	DDX_Control(pDX, IDC_DTB_EN, m_dtb_en);
	
}



BEGIN_MESSAGE_MAP(CRAMDlg, CDialog)
	ON_BN_CLICKED(IDC_SDRAM_DOWNLOAD, &CRAMDlg::OnBnClickedSdramDownload)
	ON_BN_CLICKED(IDC_SDRAM_BROWSE, &CRAMDlg::OnBnClickedSdramBrowse)
	ON_MESSAGE(WM_SDRAM_PROGRESS,ShowStatus)
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_DTB_BROWSE, &CRAMDlg::OnBnClickedDtbBrowse)
	ON_BN_CLICKED(IDC_DTB_EN, &CRAMDlg::OnBnClickedDtbEn)
END_MESSAGE_MAP()


// CRAMDlg message handlers
BOOL CRAMDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ExitEvent=CreateEvent(NULL,TRUE,FALSE,NULL);	
	
	m_progress.SetRange(0,100);	
	m_progress.SetBkColor(COLOR_DOWNLOAD);
	m_fileinfo=_T(" ...");
	m_filename=_T("");
	m_address=_T("8000");

	m_dtbinfo=_T(" ...");
	m_dtbname=_T("");
	UpdateData(FALSE);

	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
	if(mainWnd->m_inifile.ReadFile())
	{
		CString _path=mainWnd->m_inifile.GetValue(_T("SDRAM"),_T("PATH"));
		GetDlgItem(IDC_SDRAM_FILENAME)->SetWindowText(_path);
		m_filename=_path;
	}

	COLORREF col = RGB(0xFF, 0x00, 0xFF);
	m_download.setBitmapId(IDB_WRITE_DEVICE, col);
	m_download.setGradient(true);
	m_browse.setBitmapId(IDB_BROWSE, col);
	m_browse.setGradient(true);	
	m_dtbbrowse.setBitmapId(IDB_BROWSE, col);
	m_dtbbrowse.setGradient(true);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CRAMDlg::InitFile(int flag)
{
	CString tmp;
	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
	if(!mainWnd->m_inifile.ReadFile()) return false;



    //if(mainWnd->DtbEn==1)
    //{
        GetDlgItem(IDC_DTB_EN)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_STATIC_DTB)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_DTB_FILENAME)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_DTB_BROWSE)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_DTB_BUFFER_ADDRESS)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_STATIC_DTB2)->ShowWindow(SW_SHOW);

    //}else{
    //    GetDlgItem(IDC_DTB_EN)->ShowWindow(SW_HIDE);
    //    GetDlgItem(IDC_STATIC_DTB)->ShowWindow(SW_HIDE);
    //    GetDlgItem(IDC_DTB_FILENAME)->ShowWindow(SW_HIDE);
    //    GetDlgItem(IDC_DTB_BROWSE)->ShowWindow(SW_HIDE);
    //    GetDlgItem(IDC_DTB_BUFFER_ADDRESS)->ShowWindow(SW_HIDE);
    //    GetDlgItem(IDC_STATIC_DTB2)->ShowWindow(SW_HIDE);
    //}

	switch(flag)
	{
		case 0:
			m_filename=mainWnd->m_inifile.GetValue(_T("SDRAM"),_T("PATH"));	
			GetDlgItem(IDC_SDRAM_FILENAME)->SetWindowText(m_filename);
			m_fileinfo=_T(" ")+m_filename;
			

			m_dtbname=mainWnd->m_inifile.GetValue(_T("SDRAM"),_T("DTBPATH"));	
			GetDlgItem(IDC_DTB_FILENAME)->SetWindowText(m_dtbname);
			m_dtbinfo=_T(" ")+m_dtbname;

			tmp=mainWnd->m_inifile.GetValue(_T("SDRAM"),_T("EXEADDR"));
			GetDlgItem(IDC_SDRAM_BUFFER_ADDRESS)->SetWindowText(tmp);
			m_address=tmp;

			tmp=mainWnd->m_inifile.GetValue(_T("SDRAM"),_T("DTBADDR"));
			GetDlgItem(IDC_DTB_BUFFER_ADDRESS)->SetWindowText(tmp);
			m_dtbaddress=tmp;

            tmp=mainWnd->m_inifile.GetValue(_T("SDRAM"),_T("DTBEN"));
			tmp=mainWnd->m_inifile.GetValue(_T("SDRAM"),_T("DTB_UIEN"));
            m_dtb_en.SetCheck(_wtoi(tmp));
            if(_wtoi(tmp)==1)
            {
                GetDlgItem(IDC_DTB_BROWSE)->EnableWindow(TRUE);
                GetDlgItem(IDC_DTB_BUFFER_ADDRESS)->EnableWindow(TRUE);
            }else{
                GetDlgItem(IDC_DTB_BROWSE)->EnableWindow(FALSE);
                GetDlgItem(IDC_DTB_BUFFER_ADDRESS)->EnableWindow(FALSE);
            }



            tmp=mainWnd->m_inifile.GetValue(_T("SDRAM"),_T("TYPE"));
            ((CButton *)GetDlgItem(IDC_SDRAM_AUTORUN))->SetCheck(FALSE);
            switch(_wtoi(tmp))
            {
                case 0: ((CButton *)GetDlgItem(IDC_SDRAM_AUTORUN))->SetCheck(TRUE); break;
                case 1: ((CButton *)GetDlgItem(IDC_SDRAM_NOAUTORUN))->SetCheck(TRUE); break;
            }

            break;
        case 1:
            tmp.Format(_T("%d"),m_autorun);
            mainWnd->m_inifile.SetValue(_T("SDRAM"),_T("TYPE"),tmp);
            mainWnd->m_inifile.SetValue(_T("SDRAM"),_T("PATH"),m_filename);
            mainWnd->m_inifile.SetValue(_T("SDRAM"),_T("DTBPATH"),m_dtbname);
            mainWnd->m_inifile.SetValue(_T("SDRAM"),_T("EXEADDR"),m_address);
            mainWnd->m_inifile.SetValue(_T("SDRAM"),_T("DTBADDR"),m_dtbaddress);

            tmp.Format(_T("%d"),m_dtb_en.GetCheck());
            mainWnd->m_inifile.SetValue(_T("SDRAM"),_T("DTBEN"),tmp);
			tmp.Format(_T("%d"),m_dtb_en.GetCheck());
            mainWnd->m_inifile.SetValue(_T("SDRAM"),_T("DTB_UIEN"),tmp);

            mainWnd->m_inifile.WriteFile();
            break;
    }
return true;
}

void CRAMDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);


    // TODO: Add your message handler code here
    if(InitFlag==0)
    {
        InitFlag=1;
        InitFile(0);
    }
    //UpdateData (TRUE);
}

unsigned WINAPI CRAMDlg:: Download_proc(void* args)
{
	CRAMDlg* pThis = reinterpret_cast<CRAMDlg*>(args);
	pThis->Download();
	return 0;
}

void CRAMDlg::OnBnClickedSdramDownload()
{

	CString dlgText;
	
	UpdateData(TRUE);
	InitFile(1);
	if(m_address.IsEmpty())
	{
		AfxMessageBox(_T("Please input execute address"));
		return;
	}

	GetDlgItem(IDC_SDRAM_DOWNLOAD)->GetWindowText(dlgText);

	if(dlgText.CompareNoCase(_T("Download"))==0)
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
		//m_vcom.Close();
		SetEvent(m_ExitEvent);
		GetDlgItem(IDC_SDRAM_DOWNLOAD)->EnableWindow(FALSE);
		m_progress.SetPos(0);
	}
}

void CRAMDlg::OnBnClickedSdramBrowse()
{
	//CAddFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,_T("Bin,Img Files  (*.bin;*.img;*.gz)|*.bin;*.img;*.gz|All Files (*.*)|*.*||"));
	CAddFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,_T("Bin Files (*.bin)|*.bin|All Files (*.*)|*.*||"));
	dlg.m_ofn.lpstrTitle=_T("Choose burning file...");

	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

	if(!mainWnd->m_inifile.ReadFile())
		dlg.m_ofn.lpstrInitialDir=_T("c:");
	else
	{
		CString _path;
		_path=mainWnd->m_inifile.GetValue(_T("SDRAM"),_T("PATH"));
		int len=_path.GetLength();		
		int i;
		for(i=len;i>0;i--)
		{
			if(_path.GetAt(i)=='\\')
			{			
				len=i;
				break;
			}
		}
		CString filepath=_path.Left(len);
		if(filepath.IsEmpty())
			dlg.m_ofn.lpstrInitialDir=_T("c:");
		else
			dlg.m_ofn.lpstrInitialDir=_path;
	}
		
	BOOLEAN ret=dlg.DoModal();

	if(ret==IDCANCEL)
	{
		return;
	}

	m_filename=dlg.GetPathName();
	m_fileinfo=_T(" ")+m_filename;
	UpdateData(FALSE);	

	mainWnd->m_inifile.SetValue(_T("SDRAM"),_T("PATH"),m_filename);
	mainWnd->m_inifile.WriteFile();
}

LRESULT CRAMDlg::ShowStatus( WPARAM  pos, LPARAM message)
{
    m_progress.SetPos((int)pos);
    return true;
}

void CRAMDlg:: Download()
{

	int i=0;
	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

	//UpdateData(FALSE);  //shanchun modified 20121003

	if(!m_filename.IsEmpty())
	{
		mainWnd->m_gtype.EnableWindow(FALSE);
		ResetEvent(m_ExitEvent);
		GetDlgItem(IDC_SDRAM_DOWNLOAD)->SetWindowText(_T("Abort"));
		GetDlgItem(IDC_SDRAM_DOWNLOAD)->EnableWindow(0);
		GetDlgItem(IDC_SDRAMSTATUS)->SetWindowText(_T("Download"));
		//load nuc970 ddr init 
		mainWnd->UpdateBufForDDR();

		if(mainWnd->DtbEn!=1 || m_autorun!=1){
			if(XUSB(mainWnd->m_portName,m_filename,m_address,m_autorun))
			{
				GetDlgItem(IDC_SDRAMSTATUS)->SetWindowText(_T("Download successfully"));
				//AfxMessageBox(_T("Download successfully"));
			}
			else
			{
				GetDlgItem(IDC_SDRAMSTATUS)->SetWindowText(_T("Download failed!! Please check device"));
				//AfxMessageBox("Download unsuccessfully!! Please check device");
			}
		}else{			
			if(!m_dtbname.IsEmpty() && m_dtb_en.GetCheck()!=FALSE){
				XUSB(mainWnd->m_portName,m_dtbname,m_dtbaddress,0);
			}
			XUSB(mainWnd->m_portName,m_filename,m_address,2);
		}
		GetDlgItem(IDC_SDRAM_DOWNLOAD)->EnableWindow(TRUE);
		GetDlgItem(IDC_SDRAM_DOWNLOAD)->SetWindowText(_T("Download"));
		GetDlgItem(IDC_SDRAM_DOWNLOAD)->EnableWindow(1);
		//this->UpdateData(FALSE);  //shanchun modified 20121003
		mainWnd->m_gtype.EnableWindow(TRUE);

	}
	else
		AfxMessageBox(_T("Please choose image file !"));
	
	return ;

}

BOOL CRAMDlg::PreTranslateMessage(MSG* pMsg)
{
    if((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==0x0d))
    {
        return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}

HBRUSH CRAMDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
     if (pWnd->GetDlgCtrlID() == IDC_SDRAMSTATUS)
     {
         CString word;
         GetDlgItem(IDC_SDRAMSTATUS)->GetWindowText(word);
         if(!word.Compare(_T("Download")))
            pDC->SetTextColor(RGB(255,0,0));
         else
            pDC->SetTextColor(RGB(0,0,0));

         if(!word.Compare(_T("Download failed!! Please check device")))
            pDC->SetTextColor(RGB(255,0,0));
         else
            pDC->SetTextColor(RGB(0,0,0));
     }

    return hbr;
}
void CRAMDlg::OnBnClickedDtbBrowse()
{
    //CAddFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,_T("Bin,Img Files  (*.bin;*.img;*.gz)|*.bin;*.img;*.gz|All Files (*.*)|*.*||"));
    CAddFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,_T("All Files (*.*)|*.*||"));
    dlg.m_ofn.lpstrTitle=_T("Choose burning file...");

    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    if(!mainWnd->m_inifile.ReadFile())
        dlg.m_ofn.lpstrInitialDir=_T("c:");
    else
    {
        CString _path;
        _path=mainWnd->m_inifile.GetValue(_T("SDRAM"),_T("DTBPATH"));
        int len=_path.GetLength();
        int i;
        for(i=len;i>0;i--)
        {
            if(_path.GetAt(i)=='\\')
            {
                len=i;
                break;
            }
        }
        CString filepath=_path.Left(len);
        if(filepath.IsEmpty())
            dlg.m_ofn.lpstrInitialDir=_T("c:");
        else
            dlg.m_ofn.lpstrInitialDir=_path;
    }

    BOOLEAN ret=dlg.DoModal();

    if(ret==IDCANCEL)
    {
        return;
    }

    m_dtbname=dlg.GetPathName();
    m_dtbinfo=_T(" ")+m_dtbname;
    UpdateData(FALSE);

	mainWnd->m_inifile.SetValue(_T("SDRAM"),_T("DTBPATH"),m_dtbname);
	mainWnd->m_inifile.WriteFile();
}

void CRAMDlg::OnBnClickedDtbEn()
{
	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    if(m_dtb_en.GetCheck())
    {
        GetDlgItem(IDC_DTB_BROWSE)->EnableWindow(TRUE);
        GetDlgItem(IDC_DTB_BUFFER_ADDRESS)->EnableWindow(TRUE);
		
		mainWnd->m_inifile.SetValue(_T("SDRAM"),_T("DTBEN"),_T("1"));
		mainWnd->m_inifile.SetValue(_T("SDRAM"),_T("DTB_UIEN"),_T("1"));
		
        mainWnd->m_inifile.WriteFile();
    }else{
        GetDlgItem(IDC_DTB_BROWSE)->EnableWindow(FALSE);
        GetDlgItem(IDC_DTB_BUFFER_ADDRESS)->EnableWindow(FALSE);
		mainWnd->m_inifile.SetValue(_T("SDRAM"),_T("DTBEN"),_T("0"));
		mainWnd->m_inifile.SetValue(_T("SDRAM"),_T("DTB_UIEN"),_T("0"));
        mainWnd->m_inifile.WriteFile();
    }
}
