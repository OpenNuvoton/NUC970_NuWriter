// SelChipDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NuWriterDlg.h"
#include "SelChipDlg.h"


// CSelChipDlg dialog

IMPLEMENT_DYNAMIC(CSelChipDlg, CDialog)

CSelChipDlg::CSelChipDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelChipDlg::IDD, pParent)
{

}

CSelChipDlg::~CSelChipDlg()
{
}

void CSelChipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_chip);
	DDX_Control(pDX, IDC_COMBO_DDR, m_ddrtype);
	DDX_Control(pDX, IDC_STATIC_DDR_PAR, m_ddr_static);
	DDX_Control(pDX, IDOK, m_countinue);
	DDX_Control(pDX, IDC_CHECK1, m_check);
	DDX_Control(pDX, IDCANCEL, m_quit);
}


BEGIN_MESSAGE_MAP(CSelChipDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSelChipDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CSelChipDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_CHECK1, &CSelChipDlg::OnBnClickedCheck1)
	ON_WM_TIMER()
	ON_CBN_DROPDOWN(IDC_COMBO_DDR, &CSelChipDlg::OnCbnDropdownComboDdr)
	ON_CBN_DROPDOWN(IDC_COMBO1, &CSelChipDlg::OnCbnDropdownCombo1)
END_MESSAGE_MAP()


// CSelChipDlg message handlers
inline BOOL CSelChipDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowPos(&this->wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

	this->SetWindowText(PROJECT_NAME);

	m_chip.AddString(_T("NUC970/N9H30 series"));
	m_chip.SetCurSel(0);

	//init DDR & key dirctory------------------------------------------------
	DDRBuf=NULL;
	mainWnd->GetExeDir(szDirForDDR);
	szDirForDDR=szDirForDDR+_T("sys_cfg\\");
	//---Check ddr dirctory has how much *.ini-------------------------------
	vector<CString> sList;
	vector<CString>::iterator item;

	mainWnd->LoadDirINI(szDirForDDR,_T(".ini"),sList);
	int count=0;
	for(item=sList.begin();item!=sList.end();item++)
		m_ddrtype.AddString(*item);
	m_ddrtype.SetCurSel(mainWnd->DDR_Idx);
	//-----------------------------------------------------------------------
	DisplayFormat();

	TimeString=_T("Continue(%d)");



	if(mainWnd->TimeEn)
	{
		CString tmp;
		Counter=mainWnd->Timeus/1000;
		m_check.SetCheck(1);
		tmp.Format(TimeString,Counter);
		m_countinue.SetWindowText(tmp);
		if(mainWnd->Timeus<1000)
			SetTimer(0,mainWnd->Timeus,NULL);  //1 second to interrupt
		else
			SetTimer(0,1000,NULL);  //1 second to interrupt
	}else
		m_check.SetCheck(0);

	COLORREF col = RGB(0xFF, 0x00, 0xFF);
	m_countinue.setBitmapId(IDB_CONTINUE, col);
	m_countinue.setGradient(true);
	m_quit.setBitmapId(IDB_EXIT, col);
	m_quit.setGradient(true);
	return TRUE;
}


void CSelChipDlg::DisplayFormat()
{
#if 1
	m_ddr_static.EnableWindow(1);
	m_ddrtype.EnableWindow(1);
#else
	switch(m_chip.GetCurSel())
	{
		case NUC900_SERIES:
			m_ddr_static.EnableWindow(0);
			m_ddrtype.EnableWindow(0);
			break;

		case NUC970_SERIES:
			m_ddr_static.EnableWindow(1);
			m_ddrtype.EnableWindow(1);
			break;
	}
#endif
}

void CSelChipDlg::OnBnClickedOk()
{
	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
	CString ddrName;
	mainWnd->TargetChip=m_chip.GetCurSel();
	mainWnd->DDR_Idx=m_ddrtype.GetCurSel();

	m_ddrtype.GetLBText(m_ddrtype.GetCurSel(),ddrName);
	mainWnd->DDRFileFullPath.Format(_T("%s%s"),szDirForDDR,ddrName);
	mainWnd->DDRFileName.Format(_T("%s"),ddrName);
	if(m_check.GetCheck()==TRUE)
		mainWnd->TimeEn=1;
	else
		mainWnd->TimeEn=0;

	OnOK();
}

void CSelChipDlg::OnCbnSelchangeCombo1()
{
	DisplayFormat();
}

void CSelChipDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(Counter>0)
	{
		CString tmp;
		Counter--;
		tmp.Format(TimeString,Counter);
		m_countinue.SetWindowText(tmp);
	}else
	{
		KillTimer(0);
		OnBnClickedOk();
	}


	CDialog::OnTimer(nIDEvent);
}

void CSelChipDlg::OnBnClickedCheck1()
{
	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

	if(m_check.GetCheck()==TRUE)
	{
		mainWnd->TimeEn=TRUE;
		CString tmp;
		Counter=mainWnd->Timeus/1000;
		m_check.SetCheck(TRUE);
		tmp.Format(TimeString,Counter);
		m_countinue.SetWindowText(tmp);
		SetTimer(0,1000,NULL);  //1 second to interrupt

	}else{
		mainWnd->TimeEn=FALSE;
		KillTimer(FALSE);
		m_countinue.SetWindowText(_T("Continue"));
	}

}

void CSelChipDlg::OnCbnDropdownComboDdr()
{
	KillTimer(0);
}

void CSelChipDlg::OnCbnDropdownCombo1()
{
	KillTimer(0);
}