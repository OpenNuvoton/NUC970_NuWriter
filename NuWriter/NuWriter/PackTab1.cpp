// PackTab1.cpp : implementation file
//

#include "stdafx.h"
#include "NuWriter.h"
#include "NuWriterDlg.h"
#include "PackTab1.h"


// CPackTab1 dialog

IMPLEMENT_DYNAMIC(CPackTab1, CDialog)

CPackTab1::CPackTab1(CWnd* pParent /*=NULL*/)
	: CDialog(CPackTab1::IDD, pParent)
{
	TmpOffsetFlag=0;
	modifyflag=0;
	m_type = 0;
}

CPackTab1::~CPackTab1()
{
}

BOOL CPackTab1::OnInitDialog()
{
	CDialog::OnInitDialog();
	//((CButton *)GetDlgItem(IDC_PACK_TYPE_A3))->EnableWindow(0);
	COLORREF col = RGB(0xFF, 0x00, 0xFF);
	m_browse.setBitmapId(IDB_BROWSE, col);
	m_browse.setGradient(true);
	OnCommand(NULL,NULL);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CPackTab1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PACK_BROWSE, m_browse);
	DDX_Text(pDX, IDC_PACK_IMAGENAME_A, m_imagename);	
	DDX_Text(pDX, IDC_PACK_FLASHOFFSET_A, m_startblock);
	DDV_MaxChars(pDX, m_startblock, 8);
	DDX_Text(pDX, IDC_PACK_EXECADDR_A, m_execaddr);
	DDV_MaxChars(pDX, m_execaddr, 8);
	DDX_Radio(pDX, IDC_PACK_TYPE_A, m_type);	
	DDX_Control(pDX, IDC_PACK_ENC_E, m_pack_enc_check);
	DDX_Control(pDX, IDC_PACK_COMBO_ENCRYPT, m_combo_encrypt);
}


BEGIN_MESSAGE_MAP(CPackTab1, CDialog)
	ON_BN_CLICKED(IDC_PACK_BROWSE, &CPackTab1::OnBnClickedPackBrowse)
	ON_BN_CLICKED(IDC_PACK_ENC_E, &CPackTab1::OnBnClickedPackEncE)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CPackTab1 message handlers

void CPackTab1::OnBnClickedPackBrowse()
{
	UpdateData(TRUE);

	//CAddFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,_T("Bin,Img Files  (*.bin;*.img;*.gz)|*.bin;*.img;*.gz|All Files (*.*)|*.*||"));
	//CAddFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,_T("All Files (*.*)|*.*||"));
	CAddFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,_T("Bin Files (*.bin)|*.bin|All Files (*.*)|*.*||"));

	dlg.m_ofn.lpstrTitle=_T("Choose burning file...");
	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

	if(!mainWnd->m_inifile.ReadFile())
		dlg.m_ofn.lpstrInitialDir=_T("c:");
	else
	{
		CString _path;
		_path=mainWnd->m_inifile.GetValue(_T("PACK"),_T("PATH"));
		if(_path.IsEmpty())
			dlg.m_ofn.lpstrInitialDir=_T("c:");
		else
			dlg.m_ofn.lpstrInitialDir=_path;
	}

		
	BOOLEAN ret=dlg.DoModal();

	if(ret==IDCANCEL)
	{
		return;
	}

	CString temp;

	m_filename=dlg.GetPathName();
	temp=dlg.GetFileName();
	if(temp.ReverseFind('.')>0)
		m_imagename=temp.Mid(0,temp.ReverseFind('.'));
	else
		m_imagename=temp;

	if(m_imagename.GetLength()>16)
		m_imagename = m_imagename.Mid(0,15);

	this->GetDlgItem(IDC_PACK_IMAGENAME_A)->SetWindowText(m_imagename);
	
	//UpdateData(FALSE);
	//CString filepath=m_filename.Left(m_filename.GetLength()-dlg.GetFileName().GetLength()-1);

	mainWnd->m_inifile.SetValue(_T("PACK"),_T("PATH"),m_filename);
	mainWnd->m_inifile.WriteFile();
}


void CPackTab1::OnBnClickedPackEncE()
{
		if(m_pack_enc_check.GetCheck()==TRUE){
			CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
			CString szDirForKEY;
			//init key dirctory------------------------------------------------		
			m_combo_encrypt.ResetContent();
			mainWnd->GetExeDir(szDirForKEY);
			szDirForKEY=szDirForKEY+_T("key_cfg\\");
			//---Check ddr dirctory has how much *.dat-------------------------------
			vector<CString> sList;
			vector<CString>::iterator item;	
			
			mainWnd->LoadDirINI(szDirForKEY,_T(".dat"),sList);	
			int count=0;
			for(item=sList.begin();item!=sList.end();item++)	
				m_combo_encrypt.AddString(*item);				
			m_combo_encrypt.SetCurSel(0);
			//-----------------------------------------------------------------------
			m_combo_encrypt.EnableWindow(TRUE);
		}
		else
			m_combo_encrypt.EnableWindow(FALSE);
}

BOOL CPackTab1::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//TmpOffset
	UpdateData(TRUE);
	if(m_type==UBOOT)
	{								
		if(TmpOffsetFlag==0)
		{
			TmpOffsetFlag=1;
			GetDlgItem(IDC_PACK_FLASHOFFSET_A)->GetWindowText(TmpOffset);
			GetDlgItem(IDC_PACK_FLASHOFFSET_A)->SetWindowText(_T("0"));						
			GetDlgItem(IDC_PACK_FLASHOFFSET_A)->EnableWindow(FALSE);
			GetDlgItem(IDC_PACK_EXECADDR_A)->EnableWindow(TRUE);
		}
		
	}
	else
	{

		if(TmpOffsetFlag==1)
		{
			TmpOffsetFlag=0;
			GetDlgItem(IDC_PACK_FLASHOFFSET_A)->SetWindowText(TmpOffset);
			GetDlgItem(IDC_PACK_FLASHOFFSET_A)->EnableWindow(TRUE);
		}
				

		if(m_type==PACK)
		{
			GetDlgItem(IDC_PACK_EXECADDR_A)->EnableWindow(FALSE);
			GetDlgItem(IDC_PACK_FLASHOFFSET_A)->EnableWindow(FALSE);
		}else{
			GetDlgItem(IDC_PACK_EXECADDR_A)->EnableWindow(FALSE);
			GetDlgItem(IDC_PACK_FLASHOFFSET_A)->EnableWindow(TRUE);

			CString tmp;
			GetDlgItem(IDC_PACK_EXECADDR_A)->GetWindowText(tmp);
			if(tmp.IsEmpty())
				GetDlgItem(IDC_PACK_EXECADDR_A)->SetWindowText(_T("0"));
		}
	}
	return CDialog::OnCommand(wParam, lParam);
}


void CPackTab1::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
}