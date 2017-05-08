// EraseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NuWriter.h"
#include "EraseDlg.h"


// CEraseDlg dialog

IMPLEMENT_DYNAMIC(CEraseDlg, CDialog)

CEraseDlg::CEraseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEraseDlg::IDD, pParent)
{
	m_erase_type=0;
}

CEraseDlg::~CEraseDlg()
{
}

void CEraseDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_ERASE_STARTSIZENAME, m_startname);
	DDX_Control(pDX, IDC_ERASE_SIZENAME, m_sizename);
	DDX_Control(pDX, IDC_ERASE_BLOCKS, m_blocks);
	DDX_Control(pDX, IDC_ERASE_STARTBLKS, m_sblocks);
	DDX_Radio(pDX, IDC_ERASE_RADIO1, m_erase_type);
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ERASE_INFO, m_erase_info);
}


BEGIN_MESSAGE_MAP(CEraseDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CEraseDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CEraseDlg message handlers

BOOL CEraseDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(!SizeName.IsEmpty())
		m_sizename.SetWindowText(SizeName);

	if(!StartName.IsEmpty())
		m_startname.SetWindowText(StartName);

	if(!EraseInfo.IsEmpty())
		m_erase_info.SetWindowText(EraseInfo);
	
	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
	CString tmp;
	switch(type){
		case 0: /* SPI */						
			m_blocks.SetWindowText(mainWnd->m_inifile.GetValue(_T("SPI"),_T("nEBlock")));			
			m_sblocks.SetWindowText(mainWnd->m_inifile.GetValue(_T("SPI"),_T("sEBlock")));			
			break;
		case 1: /* NAND */
			m_blocks.SetWindowText(mainWnd->m_inifile.GetValue(_T("NAND"),_T("nEBlock")));	
			m_sblocks.SetWindowText(mainWnd->m_inifile.GetValue(_T("NAND"),_T("sEBlock")));	
			break;
		case 2: /* MMC */
			m_blocks.SetWindowText(mainWnd->m_inifile.GetValue(_T("MMC"),_T("nEBlock")));	
			m_sblocks.SetWindowText(mainWnd->m_inifile.GetValue(_T("MMC"),_T("sEBlock")));	
			break;
		default:
			break;
	}

	
	//-------------------------------------------------------------
	COLORREF col = RGB(0xFF, 0x00, 0xFF);	
	m_ok.setBitmapId(IDB_CONTINUE, col);
	m_ok.setGradient(true);
	m_cancel.setBitmapId(IDB_EXIT, col);
	m_cancel.setGradient(true);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEraseDlg::OnBnClickedOk()
{
	m_sblocks.GetWindowText(sblock); //start 
	m_blocks.GetWindowText(block);  //len 

	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
	CString tmp;
	switch(type){
		case 0: /* SPI */
			m_blocks.GetWindowText(tmp);
			mainWnd->m_inifile.SetValue(_T("SPI"),_T("nEBlock"),tmp);
			mainWnd->m_inifile.WriteFile();

			m_sblocks.GetWindowText(tmp);
			mainWnd->m_inifile.SetValue(_T("SPI"),_T("sEBlock"),tmp);
			mainWnd->m_inifile.WriteFile();
			break;
		case 1: /* NAND */
			m_blocks.GetWindowText(tmp);
			mainWnd->m_inifile.SetValue(_T("NAND"),_T("nEBlock"),tmp);	
			mainWnd->m_inifile.WriteFile();

			m_sblocks.GetWindowText(tmp);
			mainWnd->m_inifile.SetValue(_T("NAND"),_T("sEBlock"),tmp);	
			mainWnd->m_inifile.WriteFile();
			break;
		case 2: /* MMC */
			m_blocks.GetWindowText(tmp);
			mainWnd->m_inifile.SetValue(_T("MMC"),_T("nEBlock"),tmp);	
			mainWnd->m_inifile.WriteFile();

			m_sblocks.GetWindowText(tmp);
			mainWnd->m_inifile.SetValue(_T("MMC"),_T("sEBlock"),tmp);	
			mainWnd->m_inifile.WriteFile();
			break;
		default:
			break;
	}
	OnOK();
}

BOOL CEraseDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	switch(m_erase_type)
	{
	case 0:
		m_blocks.EnableWindow(0);
		m_sblocks.EnableWindow(0);
		break;
	case 1:
		m_blocks.EnableWindow(1);
		m_sblocks.EnableWindow(1);
		break;
	}

	return CDialog::OnCommand(wParam, lParam);
}