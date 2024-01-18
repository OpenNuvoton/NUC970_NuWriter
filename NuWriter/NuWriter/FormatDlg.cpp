// FormatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NuWriter.h"
#include "FormatDlg.h"
#include "NuWriterDlg.h"

// CFormatDlg dialog

IMPLEMENT_DYNAMIC(CFormatDlg, CDialog)

CFormatDlg::CFormatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFormatDlg::IDD, pParent)
{
    m_bInit = FALSE;
    m_CtrlCnt = 0;
}

CFormatDlg::~CFormatDlg()
{
}

void CFormatDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_FORMAT_SPACE, m_reserve);

    DDX_Control(pDX, IDC_FORMAT_P1Size, m_Edit1);
    DDX_Control(pDX, IDC_FORMAT_P2Size, m_Edit2);
    DDX_Control(pDX, IDC_FORMAT_P3Size, m_Edit3);
    DDX_Control(pDX, IDC_FORMAT_P4Size, m_Edit4);

    DDX_Control(pDX, IDC_FORMAT_SLIDER1, m_Slider1);
    DDX_Control(pDX, IDC_FORMAT_SLIDER2, m_Slider2);
    DDX_Control(pDX, IDC_FORMAT_SLIDER3, m_Slider3);
    DDX_Control(pDX, IDC_FORMAT_SLIDER4, m_Slider4);

    DDX_Control(pDX, IDC_FORMAT_BTNADD, m_BtnAdd);
    DDX_Control(pDX, IDC_FORMAT_BTNSET, m_BtnSet);
    DDX_Control(pDX, IDC_FORMAT_BTNRST, m_BtnRst);
}


BEGIN_MESSAGE_MAP(CFormatDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CFormatDlg::OnBnClickedOk)

    ON_BN_CLICKED(IDC_FORMAT_BTNADD, &CFormatDlg::OnBnClickedFormatBtnadd)
    ON_BN_CLICKED(IDC_FORMAT_BTNSET, &CFormatDlg::OnBnClickedFormatBtnset)
    ON_BN_CLICKED(IDC_FORMAT_BTNRST, &CFormatDlg::OnBnClickedFormatBtnrst)
END_MESSAGE_MAP()


// CFormatDlg message handlers
void CFormatDlg::InitDialogWnd(bool bShow)
{
    // Disable All Member
    if(!m_bInit) {
        m_Edit1.EnableWindow(FALSE);
        m_Edit2.EnableWindow(FALSE);
        m_Edit3.EnableWindow(FALSE);
        m_Edit4.EnableWindow(FALSE);

        m_Slider1.EnableWindow(FALSE);
        m_Slider2.EnableWindow(FALSE);
        m_Slider3.EnableWindow(FALSE);
        m_Slider4.EnableWindow(FALSE);

        //m_Slider1.EnableWindow(TRUE);
        //m_Edit1.SetSlideLink( this, IDC_FORMAT_SLIDER1 );
        //m_Edit1.SetParams( 0, TotalMB, 1);
        //m_Edit1.SetValue(0);

        m_BtnAdd.EnableWindow(FALSE);
        m_BtnSet.EnableWindow(TRUE);
        m_BtnRst.EnableWindow(TRUE);
        Partition1Size = 0;
        Partition2Size = 0;
        Partition3Size = 0;
        Partition4Size = 0;
        resSizeMB = 0;
        ParTotalMB = 0;
        m_bInit = TRUE;
		strPartition1Size = _T("0");
		strPartition2Size = _T("0");
		strPartition3Size = _T("0");
		strPartition4Size = _T("0");
    }
}

void CFormatDlg::ButtonCTRL(bool bShow)
{
    m_BtnAdd.EnableWindow(bShow);
    m_BtnSet.EnableWindow(bShow);
    m_BtnRst.EnableWindow(bShow);
}


void CFormatDlg::SliderCTRL(bool bShow)
{

    if(m_CtrlCnt == 0) {
        m_Slider1.EnableWindow(bShow);
        m_Edit1.EnableWindow(bShow);
    } else if(m_CtrlCnt == 1) {
        m_Slider1.EnableWindow(bShow);
        m_Edit1.EnableWindow(bShow);
        m_Slider2.EnableWindow(bShow);
        m_Edit2.EnableWindow(bShow);
    } else if(m_CtrlCnt == 2) {
        m_Slider1.EnableWindow(bShow);
        m_Edit1.EnableWindow(bShow);
        m_Slider2.EnableWindow(bShow);
        m_Edit2.EnableWindow(bShow);
        m_Slider3.EnableWindow(bShow);
        m_Edit3.EnableWindow(bShow);
    } else if(m_CtrlCnt == 3) {
        m_Slider1.EnableWindow(bShow);
        m_Edit1.EnableWindow(bShow);
        m_Slider2.EnableWindow(bShow);
        m_Edit2.EnableWindow(bShow);
        m_Slider3.EnableWindow(bShow);
        m_Edit3.EnableWindow(bShow);
        m_Slider4.EnableWindow(bShow);
        m_Edit4.EnableWindow(bShow);
    }

}

void CFormatDlg::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    CString str;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    int ParNum;

    m_reserve.GetWindowText(strResSize);
    resSizeMB = (_wtoi(strResSize)/2/1024);

    if(strResSize.IsEmpty()) {
        AfxMessageBox(_T("Error! Please input reserve space"));
        return ;
    }

    ParTotalMB = TotalMB - resSizeMB;
    ParNum = _wtoi(strPartitionNum);
    if(ParNum == 1) {
        m_Edit1.GetWindowText(strPartition1Size);
        Partition1Size = _wtoi(strPartition1Size);
        if(Partition1Size != ParTotalMB) {
            //AfxMessageBox(_T("Error! Partition size != Total size !"));
            Partition1Size = ParTotalMB;
            strPartition1Size.Format(_T("%d"), Partition1Size);
            m_Edit1.SetWindowTextW(strPartition1Size);
            m_Slider1.SetSlidePos(Partition1Size);
        }
    } else if(ParNum == 2) {
        m_Edit1.GetWindowText(strPartition1Size);
        Partition1Size = _wtoi(strPartition1Size);
        m_Edit2.GetWindowText(strPartition2Size);
        Partition2Size = _wtoi(strPartition2Size);
        if((Partition1Size+Partition2Size) != ParTotalMB) {
            //AfxMessageBox(_T("Error! Partition size != Total size !"));
            Partition2Size = (ParTotalMB-Partition1Size);
            strPartition2Size.Format(_T("%d"), Partition2Size);
            m_Edit2.SetWindowTextW(strPartition2Size);
            m_Slider2.SetSlidePos(Partition2Size);
        }
    } else if(ParNum == 3) {
        m_Edit1.GetWindowText(strPartition1Size);
        Partition1Size = _wtoi(strPartition1Size);
        m_Edit2.GetWindowText(strPartition2Size);
        Partition2Size = _wtoi(strPartition2Size);
        m_Edit3.GetWindowText(strPartition3Size);
        Partition3Size = _wtoi(strPartition3Size);
        if((Partition1Size+Partition2Size+Partition3Size) != ParTotalMB) {
            //AfxMessageBox(_T("Error! Partition size != Total size !"));
            Partition3Size = (ParTotalMB-Partition1Size-Partition2Size);
            strPartition3Size.Format(_T("%d"), Partition3Size);
            m_Edit3.SetWindowTextW(strPartition3Size);
            m_Slider3.SetSlidePos(Partition3Size);
        }
    } else if(ParNum == 4) {
        m_Edit1.GetWindowText(strPartition1Size);
        Partition1Size = _wtoi(strPartition1Size);
        m_Edit2.GetWindowText(strPartition2Size);
        Partition2Size = _wtoi(strPartition2Size);
        m_Edit3.GetWindowText(strPartition3Size);
        Partition3Size = _wtoi(strPartition3Size);
        m_Edit4.GetWindowText(strPartition4Size);
        Partition4Size = _wtoi(strPartition4Size);
        if((Partition1Size+Partition2Size+Partition3Size+Partition4Size) != ParTotalMB) {
            //AfxMessageBox(_T("Error! Partition size != Total size !"));
            Partition4Size = (ParTotalMB-Partition1Size-Partition2Size-Partition3Size);
            strPartition4Size.Format(_T("%d"), Partition4Size);
            m_Edit4.SetWindowTextW(strPartition4Size);
            m_Slider4.SetSlidePos(Partition4Size);
        }
    }
    OnOK();
}


BOOL CFormatDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    CString str;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    TotalSize = 0;
    TotalSize = mainWnd->m_info.EMMC_uBlock;
    TotalMB = TotalSize/2/1024;

    if(TotalMB == 0)
        AfxMessageBox(_T("Error! Please reset device and press Re-connect button now !!!\n"));

    str.Format(_T("Total Size : %dMB(%d sectors)"), TotalMB, mainWnd->m_info.EMMC_uBlock);
    GetDlgItem(IDC_STATIC_FORMAT_TotalSize)->SetWindowTextW(str);

    InitDialogWnd(FALSE);
    UpdateData(FALSE);

    str.Format(_T("%f"),(float)TotalSize/2.0/1024.0);
    m_reserve.SetWindowTextW(_T(""));

    m_reserve.GetWindowText(strResSize);
    resSizeMB = (_wtoi(strResSize)/2/1024);
    ParTotalMB = TotalMB - resSizeMB;
    TRACE(_T("strResSize = %s , resSizeMB = %d\n"), strResSize, resSizeMB);

	GetDlgItem(IDOK)->EnableWindow(FALSE);
	m_BtnRst.EnableWindow(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
}

void CFormatDlg::OnBnClickedFormatBtnadd()
{
    m_BtnAdd.EnableWindow(FALSE);
    m_BtnSet.EnableWindow(TRUE);

    m_Edit1.GetWindowText(strPartition1Size);
    Partition1Size = _wtoi(strPartition1Size);
    if(Partition1Size >= ParTotalMB) {
        m_BtnSet.EnableWindow(FALSE);
        AfxMessageBox(_T("Error! Partition Size = Total size, Can't add partition.  Please press Reset"));
        return;
    }
    if(m_CtrlCnt == 4) {
        m_CtrlCnt = 0;
        SliderCTRL(FALSE);
        m_BtnAdd.EnableWindow(FALSE);
        m_BtnSet.EnableWindow(FALSE);
        m_BtnRst.EnableWindow(TRUE);
        AfxMessageBox(_T("Error! Partition number > 4, please press Reset"));
    }


    //m_CtrlCnt += 1;
    //SliderCTRL(TRUE);
    if(m_CtrlCnt == 1) {
        m_Slider1.EnableWindow(FALSE);
        m_Edit1.EnableWindow(FALSE);
        m_Edit2.EnableWindow(TRUE);
        m_Edit2.SetSlideLink( this, IDC_FORMAT_SLIDER2 );
        m_Edit2.SetParams(0, ParTotalMB-Partition1Size, 1);//Partition1Size+1
        m_Edit2.SetValue(0);//Partition1Size+1
        m_Slider2.SetSlidePos(0);
        m_Slider2.EnableWindow(TRUE);
    } else if(m_CtrlCnt == 2) {
        m_Edit1.GetWindowText(strPartition1Size);
        Partition1Size = _wtoi(strPartition1Size);

        m_Edit2.GetWindowText(strPartition2Size);
        Partition2Size = _wtoi(strPartition2Size);
        m_Slider2.EnableWindow(FALSE);

		if((Partition1Size+Partition2Size) >= ParTotalMB) {
            m_BtnSet.EnableWindow(FALSE);
            AfxMessageBox(_T("Error! Partition Size = Total size, Can't add partition.  Please press Reset"));
            return;
        }
        m_Slider3.EnableWindow(TRUE);
        m_Edit2.EnableWindow(FALSE);
        m_Edit3.EnableWindow(TRUE);
        m_Edit3.SetSlideLink( this, IDC_FORMAT_SLIDER3 );
        m_Edit3.SetParams(0, ParTotalMB-Partition1Size-Partition2Size, 1);
        m_Edit3.SetValue(0);
        m_Slider3.SetSlidePos(0);
    } else if(m_CtrlCnt == 3) {
        m_Edit1.GetWindowText(strPartition1Size);
        Partition1Size = _wtoi(strPartition1Size);
        m_Slider1.EnableWindow(FALSE);

        m_Edit2.GetWindowText(strPartition2Size);
        Partition2Size = _wtoi(strPartition2Size);
        m_Slider2.EnableWindow(FALSE);

        m_Edit3.GetWindowText(strPartition3Size);
        Partition3Size = _wtoi(strPartition3Size);
        m_Slider3.EnableWindow(FALSE);

		if((Partition1Size+Partition2Size+Partition3Size) >= ParTotalMB) {
            m_BtnSet.EnableWindow(FALSE);
            AfxMessageBox(_T("Error! Partition Size = Total size, Can't add partition.  Please press Reset"));
            return;
        }
        m_Slider4.EnableWindow(TRUE);
        m_Edit3.EnableWindow(FALSE);
        m_Edit4.EnableWindow(TRUE);
        m_Edit4.SetSlideLink( this, IDC_FORMAT_SLIDER4 );
        m_Edit4.SetParams(0, ParTotalMB-Partition1Size-Partition2Size-Partition3Size, 1);
        m_Edit4.SetValue(0);
        m_Slider4.SetSlidePos(0);
    } else {
        m_CtrlCnt = 0;
        SliderCTRL(FALSE);
        m_BtnAdd.EnableWindow(FALSE);
        m_BtnSet.EnableWindow(FALSE);
        m_BtnRst.EnableWindow(TRUE);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
        AfxMessageBox(_T("Error! Partition number > 4, please press Reset"));
    }

    //AfxMessageBox(_T("Error! Please press Set to apply parameter."));
}


void CFormatDlg::OnBnClickedFormatBtnset()
{
    CString str;

    //InitDialogWnd(FALSE);
    if(m_bInit) {
	
		m_reserve.GetWindowText(strResSize);
        resSizeMB = (_wtoi(strResSize)/2/1024);
        ParTotalMB = TotalMB - resSizeMB;
        TRACE(_T("Btnset strResSize = %s , resSizeMB = %d\n"), strResSize, resSizeMB);
		
        if(strResSize.IsEmpty()) {
            AfxMessageBox(_T("Error! Please input reserve space"));
            return ;
        }

        if(resSizeMB > ParTotalMB) {
            AfxMessageBox(_T("Error! Reserve space is bigger than Total size."));
            return ;
        }		

		m_BtnRst.EnableWindow(TRUE);
        m_BtnAdd.EnableWindow(FALSE);
        m_BtnSet.EnableWindow(TRUE);

        m_Slider1.EnableWindow(TRUE);
        m_Edit1.EnableWindow(TRUE);
        m_Edit1.SetSlideLink( this, IDC_FORMAT_SLIDER1 );
        m_Edit1.SetParams( 0, ParTotalMB, 1);
        m_Edit1.SetValue(0);
        m_bInit = FALSE;
		m_CtrlCnt = 0;
    } else {
		GetDlgItem(IDOK)->EnableWindow(TRUE);
        m_BtnRst.EnableWindow(TRUE);		
        m_BtnAdd.EnableWindow(TRUE);
        m_BtnSet.EnableWindow(FALSE);

        //m_CtrlCnt += 1;
        m_Edit1.GetWindowText(strPartition1Size);
        Partition1Size = _wtoi(strPartition1Size);

        if(Partition1Size == 0) {
            m_BtnAdd.EnableWindow(FALSE);
            m_BtnSet.EnableWindow(TRUE);
            m_Slider1.EnableWindow(TRUE);
            AfxMessageBox(_T("Error! Partition size = 0 !"));
            return;
        }

        SliderCTRL(FALSE);
        if(m_CtrlCnt == 0) {
            strPartitionNum.Format(_T("%d"), 1);
            m_CtrlCnt = 1;
        } else if(m_CtrlCnt == 1) {
            m_Edit2.GetWindowText(strPartition2Size);
            Partition2Size = _wtoi(strPartition2Size);
            strPartitionNum.Format(_T("%d"), 2);
            m_CtrlCnt = 2;
        } else if(m_CtrlCnt == 2) {
            m_Edit2.GetWindowText(strPartition2Size);
            Partition2Size = _wtoi(strPartition2Size);
            m_Edit3.GetWindowText(strPartition3Size);
            Partition3Size = _wtoi(strPartition3Size);

            strPartitionNum.Format(_T("%d"), 3);
            m_CtrlCnt = 3;
        } else {
            strPartitionNum.Format(_T("%d"), 4);
            m_CtrlCnt = 4;
			m_BtnAdd.EnableWindow(FALSE);
        }

        if((Partition1Size+Partition2Size+Partition3Size+Partition4Size) > ParTotalMB) {
            AfxMessageBox(_T("Error! Partition size > Total size !"));
            str.Format(_T("%d"), (ParTotalMB-Partition1Size-Partition2Size-Partition3Size));
            m_Edit4.SetWindowTextW(str);
            m_Slider4.SetSlidePos(Partition4Size);
        }
    }

}

void CFormatDlg::OnBnClickedFormatBtnrst()
{
    m_CtrlCnt = 0;
    SliderCTRL(TRUE);

	GetDlgItem(IDOK)->EnableWindow(FALSE);
    m_BtnAdd.EnableWindow(FALSE);
    m_BtnSet.EnableWindow(TRUE);
	m_BtnRst.EnableWindow(FALSE);

	m_bInit = TRUE;
    m_reserve.SetWindowTextW(_T("0"));
    m_reserve.GetWindowText(strResSize);
    resSizeMB = (_wtoi(strResSize)/2/1024);
    ParTotalMB = TotalMB - resSizeMB;
    TRACE(_T("strResSize = %s , resSizeMB = %d\n"), strResSize, resSizeMB);

    m_Edit1.SetValue(0);
    m_Slider1.SetSlidePos(0);
    m_Slider1.EnableWindow(TRUE);
    m_Edit1.EnableWindow(FALSE);
    m_Edit1.SetWindowText(_T("0"));
    Partition1Size = 0;
    strPartition1Size = _T("0");

    m_Edit2.SetValue(0);
    m_Slider2.SetSlidePos(0);
    m_Slider2.EnableWindow(FALSE);
    m_Edit2.EnableWindow(FALSE);
    m_Edit2.SetWindowText(_T("0"));
    Partition2Size = 0;
    strPartition2Size = _T("0");

    m_Edit3.SetValue(0);
    m_Slider3.SetSlidePos(0);
    m_Slider3.EnableWindow(FALSE);
    m_Edit3.EnableWindow(FALSE);
    m_Edit3.SetWindowText(_T("0"));
    Partition3Size = 0;
    strPartition3Size = _T("0");

    m_Edit4.SetValue(0);
    m_Slider4.SetSlidePos(0);
    m_Slider4.EnableWindow(FALSE);
    m_Edit4.EnableWindow(FALSE);
    m_Edit4.SetWindowText(_T("0"));
    Partition4Size = 0;
    strPartition4Size = _T("0");
}
