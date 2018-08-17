// SPIReadDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NuWriter.h"
#include "SPIReadDlg.h"


// CSPIReadDlg dialog

IMPLEMENT_DYNAMIC(CReadDlg, CDialog)

CReadDlg::CReadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReadDlg::IDD, pParent)
{
	//blocks(1 block is 64  KB)
	//CString tmp;
	//tmp.Format(_T("blocks(1 block is %s"),SizeName);
	//m_sizename.SetWindowText(tmp);
}

CReadDlg::~CReadDlg()
{
}

void CReadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_READ_BLOCKS, m_blocks);
	DDX_Text(pDX, IDC_READ_FILENAME2, m_fileinfo2);
	DDX_Control(pDX, IDC_READ_BROSWSE, m_browse);
	DDX_Control(pDX, IDC_SIZENAME, m_sizename);
	DDX_Control(pDX, IDCANCEL, m_cancel);
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDC_READ_FILENAME2, m_filepath);
	DDX_Control(pDX, IDC_READ_STARTBLKS, m_sblocks);
	DDX_Control(pDX, IDC_STARTSIZENAME, m_startname);
}


BEGIN_MESSAGE_MAP(CReadDlg, CDialog)
	ON_BN_CLICKED(IDC_READ_BROSWSE, &CReadDlg::OnBnClickedSpireadBroswse)
	ON_BN_CLICKED(IDOK, &CReadDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSPIReadDlg message handlers
BOOL CReadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(!SizeName.IsEmpty())
		m_sizename.SetWindowText(SizeName);

	if(!StartName.IsEmpty())
		m_startname.SetWindowText(StartName);

	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
	CString tmp;
	switch(type){
		case 0: /* SPI */
			m_filepath.SetWindowText(mainWnd->m_inifile.GetValue(_T("SPI"),_T("SAVEPATH")));
			m_blocks.SetWindowText(mainWnd->m_inifile.GetValue(_T("SPI"),_T("nBlock")));
			m_sblocks.SetWindowText(mainWnd->m_inifile.GetValue(_T("SPI"),_T("sBlock")));
			break;
		case 1: /* NAND */
			m_filepath.SetWindowText(mainWnd->m_inifile.GetValue(_T("NAND"),_T("SAVEPATH")));
			m_blocks.SetWindowText(mainWnd->m_inifile.GetValue(_T("NAND"),_T("nBlock")));
			m_sblocks.SetWindowText(mainWnd->m_inifile.GetValue(_T("NAND"),_T("sBlock")));
			break;
		case 2: /* MMC */
			m_filepath.SetWindowText(mainWnd->m_inifile.GetValue(_T("MMC"),_T("SAVEPATH")));
			m_blocks.SetWindowText(mainWnd->m_inifile.GetValue(_T("MMC"),_T("nBlock")));
			m_sblocks.SetWindowText(mainWnd->m_inifile.GetValue(_T("MMC"),_T("sBlock")));
			break;
		default:
			_path==_T("c:");
			break;
	}


	//-------------------------------------------------------------
	COLORREF col = RGB(0xFF, 0x00, 0xFF);
	m_browse.setBitmapId(IDB_BROWSE, col);
	m_browse.setGradient(true);
	m_ok.setBitmapId(IDB_CONTINUE, col);
	m_ok.setGradient(true);
	m_cancel.setBitmapId(IDB_EXIT, col);
	m_cancel.setGradient(true);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CReadDlg::OnBnClickedSpireadBroswse()
{
	//CAddFileDialog dlg(TRUE,NULL,NULL,OFN_CREATEPROMPT | OFN_HIDEREADONLY ,_T("Bin,Img Files  (*.bin)|*.bin|All Files (*.*)|*.*||"));
	//CAddFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,_T("Bin Files (*.bin)|*.bin|All Files (*.*)|*.*||"));
	CAddFileDialog dlg(TRUE,NULL,NULL, 0, _T("Bin Files (*.bin)|*.bin|All Files (*.*)|*.*||"));
	dlg.m_ofn.lpstrTitle=_T("Choose burning file...");
	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

	if(!mainWnd->m_inifile.ReadFile())
		dlg.m_ofn.lpstrInitialDir=_T("c:");
	else
	{
		CString _path;
		m_filepath.GetWindowText(_path);
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

	m_filename2=dlg.GetPathName();
	m_filepath.SetWindowText(m_filename2);
	//UpdateData(FALSE);

	//CString filepath=m_filename.Left(m_filename2.GetLength()-dlg.GetFileName().GetLength()-1);

	//mainWnd->m_inifile.SetValue(_T("RAWSPI"),_T("SAVEPATH"),filepath);
	//mainWnd->m_inifile.WriteFile();
}

void CReadDlg::OnBnClickedOk()
{
	m_sblocks.GetWindowText(sblock); //start
	m_blocks.GetWindowText(block);  //len
	m_filepath.GetWindowText(m_filename2);
	if(block.IsEmpty() || sblock.IsEmpty())
	{
		AfxMessageBox(_T("Read blocks can't empty"));
		return;
	}

	if(m_filename2.IsEmpty())
	{
		AfxMessageBox(_T("Please select save file"));
		return;
	}

	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
	CString tmp;
	switch(type){
		case 0: /* SPI */
			m_filepath.GetWindowText(tmp);
			mainWnd->m_inifile.SetValue(_T("SPI"),_T("SAVEPATH"),tmp);
			m_blocks.GetWindowText(tmp);
			mainWnd->m_inifile.SetValue(_T("SPI"),_T("nBlock"),tmp);
			mainWnd->m_inifile.WriteFile();

			m_sblocks.GetWindowText(tmp);
			mainWnd->m_inifile.SetValue(_T("SPI"),_T("sBlock"),tmp);
			mainWnd->m_inifile.WriteFile();
			break;
		case 1: /* NAND */
			m_filepath.GetWindowText(tmp);
			mainWnd->m_inifile.SetValue(_T("NAND"),_T("SAVEPATH"),tmp);
			m_blocks.GetWindowText(tmp);
			mainWnd->m_inifile.SetValue(_T("NAND"),_T("nBlock"),tmp);
			mainWnd->m_inifile.WriteFile();

			m_sblocks.GetWindowText(tmp);
			mainWnd->m_inifile.SetValue(_T("NAND"),_T("sBlock"),tmp);
			mainWnd->m_inifile.WriteFile();
			break;
		case 2: /* MMC */
			m_filepath.GetWindowText(tmp);
			mainWnd->m_inifile.SetValue(_T("MMC"),_T("SAVEPATH"),tmp);
			m_blocks.GetWindowText(tmp);
			mainWnd->m_inifile.SetValue(_T("MMC"),_T("nBlock"),tmp);
			mainWnd->m_inifile.WriteFile();

			m_sblocks.GetWindowText(tmp);
			mainWnd->m_inifile.SetValue(_T("MMC"),_T("sBlock"),tmp);
			mainWnd->m_inifile.WriteFile();
			break;
		default:
			_path==_T("c:");
			break;
	}
	OnOK();
}
