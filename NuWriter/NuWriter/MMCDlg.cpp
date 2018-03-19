// MMCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NuWriter.h"
#include "NuWriterDlg.h"


// CMMCDlg dialog

IMPLEMENT_DYNAMIC(CMMCDlg, CDialog)

CMMCDlg::CMMCDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CMMCDlg::IDD, pParent)
{
    TmpOffsetFlag=0;
    encbuf=NULL;
    enclen=0;
    InitFlag=0;
    m_type = 0;
    m_fhead=(NORBOOT_MMC_HEAD *)malloc(sizeof(NORBOOT_MMC_HEAD));
    TRACE(_T("CMMCDlg  malloc %d \n"), sizeof(NORBOOT_MMC_HEAD));
}

CMMCDlg::~CMMCDlg()
{
    //TRACE(_T("~CMMCDlg  free %d \n\n"), sizeof(NORBOOT_MMC_HEAD));
    free(m_fhead);
}

void CMMCDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MMC_IMAGELIST, m_imagelist);
    DDX_Text(pDX, IDC_MMC_IMAGENAME_A, m_imagename);
    DDX_Control(pDX, IDC_MMC_PROGRESS, m_progress);

    DDX_Text(pDX, IDC_MMC_FLASHOFFSET_A, m_startblock);
    DDV_MaxChars(pDX, m_startblock, 8);
    DDX_Text(pDX, IDC_MMC_EXECADDR_A, m_execaddr);
    DDV_MaxChars(pDX, m_execaddr, 8);


    DDX_Control(pDX, IDC_MMC_FORMAT, m_format);
    DDX_Control(pDX, IDC_MMC_READ, m_read);
    DDX_Control(pDX, IDC_MMC_VERIFY, m_verify);
    DDX_Control(pDX, IDC_MMC_DOWNLOAD, m_burn);
    DDX_Control(pDX, IDC_MMC_BROWSE, m_browse);

    DDX_Radio(pDX, IDC_MMC_TYPE_A, m_type);
    DDX_Control(pDX, IDC_MMC_ENC_E, m_mmc_enc_check);
    DDX_Control(pDX, IDC_MMC_COMBO_ENCRYPT, m_combo_encrypt);
    DDX_Control(pDX, IDC_MMC_STATUS, m_status);
}


BEGIN_MESSAGE_MAP(CMMCDlg, CDialog)
    ON_BN_CLICKED(IDC_MMC_DOWNLOAD, &CMMCDlg::OnBnClickedMmcDownload)
    ON_BN_CLICKED(IDC_MMC_FORMAT, &CMMCDlg::OnBnClickedMmcFormat)
    ON_BN_CLICKED(IDC_MMC_VERIFY, &CMMCDlg::OnBnClickedMmcVerify)
    ON_BN_CLICKED(IDC_MMC_READ, &CMMCDlg::OnBnClickedMmcRead)
    ON_BN_CLICKED(IDC_MMC_BROWSE, &CMMCDlg::OnBnClickedMmcBrowse)
    ON_BN_CLICKED(IDC_MMC_ENC_E, &CMMCDlg::OnBnClickedMmcEncE)
    ON_MESSAGE(WM_MMC_PROGRESS,ShowStatus)
    ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CMMCDlg message handlers
BOOL CMMCDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    m_imagelist.SetHeadings(_T("Name, 70; Type, 50;Start offset, 80;End offset, 80"));
    m_imagelist.SetGridLines(TRUE);


    m_ExitEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

    m_progress.SetRange(0,100);
    m_progress.SetBkColor(COLOR_DOWNLOAD);
    m_filename="";
    GetDlgItem(IDC_MMC_VERIFY)->EnableWindow(FALSE);

    COLORREF col = RGB(0xFF, 0x00, 0xFF);
    m_format.setBitmapId(IDB_FORMAT, col);
    m_format.setGradient(true);
    m_eraseall.setBitmapId(IDB_ERASEALL, col);
    m_eraseall.setGradient(true);
    m_read.setBitmapId(IDB_READ_DEVICE, col);
    m_read.setGradient(true);
    m_verify.setBitmapId(IDB_VERIFY, col);
    m_verify.setGradient(true);
    m_burn.setBitmapId(IDB_WRITE_DEVICE, col);
    m_burn.setGradient(true);
    m_browse.setBitmapId(IDB_BROWSE, col);
    m_browse.setGradient(true);
    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CMMCDlg::Download()
{
    int i=0;
    BOOLEAN ret;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    //UpdateData(FALSE);

    if(!m_filename.IsEmpty()) {
        int len,startblock,endblock;
        mainWnd->m_gtype.EnableWindow(FALSE);
        GetDlgItem(IDC_MMC_VERIFY)->EnableWindow(FALSE);
        ResetEvent(m_ExitEvent);
        GetDlgItem(IDC_MMC_DOWNLOAD)->SetWindowText(_T("Abort"));
        if(m_type!=PACK) {
            m_progress.SetRange(0,100);
            ret=XUSB_Burn(mainWnd->m_portName,m_filename,&len);
            if(ret) {
                GetDlgItem(IDC_MMC_VERIFY)->EnableWindow(TRUE);
                InitFile(1);
                AfxMessageBox(_T("Burn successfully"));
                CString flagstr;
                switch(m_type) {
                case DATA :
                    flagstr.Format(_T("DATA"));
                    break;
                case ENV  :
                    flagstr.Format(_T("ENV"));
                    break;
                case UBOOT:
                    flagstr.Format(_T("uBOOT"));
                    break;
                case PACK :
                    flagstr.Format(_T("Pack"));
                    break;
                }
                _stscanf_s(m_startblock,_T("%x"),&startblock);
                //endblock=startblock+((len+(MMC512B-1))/(MMC512B))*(MMC512B);
                endblock=startblock+len;

                CString _startblock,_endblock;
                _startblock.Format(_T("0x%x"),startblock);
                _endblock.Format(_T("0x%x"),endblock);
                m_imagelist.InsertItem(0,m_imagename,flagstr,_startblock,_endblock);
            }
            //else
            //	AfxMessageBox("Burn unsuccessfully!! Please check device");
        } else {
            ret=XUSB_Pack(mainWnd->m_portName,m_filename,&len);
            if(ret) {
                GetDlgItem(IDC_MMC_VERIFY)->EnableWindow(TRUE);
                AfxMessageBox(_T("Burn successfully"));
            }
        }
        GetDlgItem(IDC_MMC_DOWNLOAD)->EnableWindow(TRUE);

        GetDlgItem(IDC_MMC_DOWNLOAD)->SetWindowText(_T("Burn"));
        //UpdateData(FALSE);
        mainWnd->m_gtype.EnableWindow(TRUE);

        //if(ret)
        //{
        //	CSPIComDlg* parent=(CSPIComDlg*)GetParent();

        //	if(parent)
        //		parent->PostMessage(WM_SPI_UPDATE_LIST,0,0);
        //}

    } else
        AfxMessageBox(_T("Please choose image file !"));
    return ;

}

unsigned WINAPI CMMCDlg:: Download_proc(void* args)
{
    CMMCDlg* pThis = reinterpret_cast<CMMCDlg*>(args);
    pThis->Download();
    return 0;
}


void CMMCDlg::OnBnClickedMmcDownload()
{
    CString dlgText;
    int _startblock=0;

//	UpdateData(TRUE);

    if((m_type!=UBOOT)&&((m_execaddr.IsEmpty())||(m_startblock.IsEmpty())) && (m_type!=PACK) ) {

        AfxMessageBox(_T("Please input image information"));
        return;
    }

    //swscanf_s(m_imageno,_T("%x"),&_no);
    swscanf_s(m_startblock,_T("%x"),&_startblock);

    if((m_type!=UBOOT)&&(_startblock<0x600) && (m_type!=PACK)) {
        CString tmp;
        tmp.Format(_T("Offset address < 0x600 for uBoot. Do you confirm this operation ?"));
#if 0
        AfxMessageBox(_T(tmp));
        return;
#else
        if(::MessageBox(this->m_hWnd,tmp,_T("Nu Writer"),MB_OKCANCEL|MB_ICONWARNING)==IDCANCEL)
            return;
#endif
    }

    GetDlgItem(IDC_MMC_DOWNLOAD)->GetWindowText(dlgText);


    if(dlgText.CompareNoCase(_T("Burn"))==0) {

        UpdateData(TRUE);

        if(::MessageBox(this->m_hWnd,_T("Do you confirm this operation ?"),_T("Turbo Writer"),MB_OKCANCEL|MB_ICONWARNING)==IDCANCEL)
            return;

        unsigned Thread1;
        HANDLE hThread;

        m_progress.SetPos(0);
        m_progress.SetRange(0,100);
        m_progress.SetBkColor(COLOR_DOWNLOAD);
        hThread=(HANDLE)_beginthreadex(NULL,0,Download_proc,(void*)this,0,&Thread1);
        CloseHandle(hThread);
    } else {
        //m_vcom.Close();
        SetEvent(m_ExitEvent);
        GetDlgItem(IDC_MMC_DOWNLOAD)->EnableWindow(FALSE);
        m_progress.SetPos(0);
    }
}

void CMMCDlg:: Format()
{

    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    BOOLEAN ret;
    //UpdateData(FALSE);  //shanchun moedified 20121003

    mainWnd->m_gtype.EnableWindow(FALSE);
    ResetEvent(m_ExitEvent);
    ret=XUSB_Format(mainWnd->m_portName);
    if(ret)
        AfxMessageBox(_T("Format successfully"));
    GetDlgItem(IDC_MMC_FORMAT)->EnableWindow(TRUE);
    //UpdateData(FALSE); //shanchun moedified 20121003
    mainWnd->m_gtype.EnableWindow(TRUE);

    //if(ret)
    //{
    //    CMMCDlg* parent=(CMMCDlg*)GetParent();
    //    if(parent)
    //    parent->PostMessage(WM_MMC_UPDATE_LIST,0,0);
    //}

    return ;
}

unsigned WINAPI CMMCDlg:: Format_proc(void* args)
{
    CMMCDlg* pThis = reinterpret_cast<CMMCDlg*>(args);
    pThis->Format();
    return 0;
}

void CMMCDlg::OnBnClickedMmcFormat()
{
    // TODO: Add your control notification handler code here
    CFormatDlg format_dlg;
    if(format_dlg.DoModal()==IDCANCEL) return;
    m_space=format_dlg.space;

    CString dlgText;

    UpdateData(TRUE);

    GetDlgItem(IDC_MMC_FORMAT)->GetWindowText(dlgText);


    if(dlgText.CompareNoCase(_T("Format"))==0) {

        UpdateData(TRUE);

        unsigned Thread1;
        HANDLE hThread;

        m_progress.SetPos(0);
        m_progress.SetBkColor(COLOR_ERASE);
        hThread=(HANDLE)_beginthreadex(NULL,0,Format_proc,(void*)this,0,&Thread1);
        CloseHandle(hThread);
    } else {
        SetEvent(m_ExitEvent);
        GetDlgItem(IDC_MMC_FORMAT)->EnableWindow(FALSE);
        m_progress.SetPos(0);
    }


}

void CMMCDlg::OnBnClickedMmcBrowse()
{
    UpdateData(TRUE);
    UpdateData(FALSE);
    CString temp;
    //CAddFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,_T("Bin,Img Files  (*.bin;*.img;*.gz)|*.bin;*.img;*.gz|All Files (*.*)|*.*||"));
    //CAddFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,_T("All Files (*.*)|*.*||"));
    CAddFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,_T("Bin Files (*.bin)|*.bin|All Files (*.*)|*.*||"));

    dlg.m_ofn.lpstrTitle=_T("Choose burning file...");
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    if(!mainWnd->m_inifile.ReadFile())
        dlg.m_ofn.lpstrInitialDir=_T("c:");
    else {
        CString _path;
        _path=mainWnd->m_inifile.GetValue(_T("eMMC"),_T("PATH"));
        if(_path.IsEmpty())
            dlg.m_ofn.lpstrInitialDir=_T("c:");
        else
            dlg.m_ofn.lpstrInitialDir=_path;
    }


    BOOLEAN ret=dlg.DoModal();

    if(ret==IDCANCEL) {
        return;
    }

    m_filename=dlg.GetPathName();
    temp=dlg.GetFileName();
    if(temp.ReverseFind('.')>0)
        m_imagename=temp.Mid(0,temp.ReverseFind('.'));
    else
        m_imagename=temp;

    if(m_imagename.GetLength()>16)
        m_imagename = m_imagename.Mid(0,15);
    this->GetDlgItem(IDC_MMC_IMAGENAME_A)->SetWindowText(m_imagename);
    //UpdateData(FALSE); //shanchun modified 20121203
    //PostMessage(WM_UPDATE_MESSAGE,0,0);



    CString filepath=m_filename.Left(m_filename.GetLength()-dlg.GetFileName().GetLength()-1);

    mainWnd->m_inifile.SetValue(_T("eMMC"),_T("PATH"),filepath);
    mainWnd->m_inifile.WriteFile();

}


void CMMCDlg:: Read()
{
    int i=0;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    //UpdateData(FALSE);

    if(!m_filename2.IsEmpty()) {
        mainWnd->m_gtype.EnableWindow(FALSE);
        ResetEvent(m_ExitEvent);
        GetDlgItem(IDC_MMC_READ)->EnableWindow(FALSE);

        int blocks,sblocks;
        _stscanf_s(m_blocks,_T("%d"),&blocks);
        _stscanf_s(m_sblocks,_T("%d"),&sblocks);
        if(XUSB_Read(mainWnd->m_portName,m_filename2,sblocks,blocks*(0x200)))
            AfxMessageBox(_T("Read OK !"));
        else
            AfxMessageBox(_T("Read Error !"));
        GetDlgItem(IDC_MMC_READ)->EnableWindow(TRUE);

        //UpdateData(FALSE);
        mainWnd->m_gtype.EnableWindow(TRUE);

    } else
        AfxMessageBox(_T("Please choose read file !"));

    return ;
}

unsigned WINAPI CMMCDlg::Read_proc(void* args)
{
    CMMCDlg* pThis = reinterpret_cast<CMMCDlg*>(args);
    pThis->Read();
    return 0;
}
void CMMCDlg::OnBnClickedMmcRead()
{
    CReadDlg read_dlg;
    read_dlg.StartName.Format(_T("sectors"));
    read_dlg.SizeName.Format(_T("sectors(1 sector is 512 bytes)"));
    read_dlg.type=2; /* 0 => SPI, 1 =>NAND, 2=>MMC */
    if(read_dlg.DoModal()==IDCANCEL)
        return;
    m_filename2=read_dlg.m_filename2;
    m_blocks=read_dlg.block;
    m_sblocks=read_dlg.sblock;

    CString dlgText;

    UpdateData(TRUE);

    GetDlgItem(IDC_MMC_READ)->GetWindowText(dlgText);
    if(dlgText.CompareNoCase(_T("Read"))==0) {
        UpdateData(TRUE);
        unsigned Thread1;
        HANDLE hThread;
        m_progress.SetPos(0);
        m_progress.SetRange(0,100);
        m_progress.SetBkColor(COLOR_VERIFY);
        hThread=(HANDLE)_beginthreadex(NULL,0,Read_proc,(void*)this,0,&Thread1);
        CloseHandle(hThread);
    } else {
        //SetEvent(m_ExitEvent);
        //GetDlgItem(IDC_SPI_READ)->EnableWindow(FALSE);
        m_progress.SetPos(0);
    }
}


void CMMCDlg::Verify()
{
    int i=0;
    BOOLEAN ret;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    //UpdateData(FALSE);

    if(!m_filename.IsEmpty()) {
        mainWnd->m_gtype.EnableWindow(FALSE);
        ResetEvent(m_ExitEvent);
        GetDlgItem(IDC_MMC_VERIFY)->SetWindowText(_T("Abort"));
        ret=XUSB_Verify(mainWnd->m_portName,m_filename);
        if(ret)
            AfxMessageBox(_T("Verify OK !"));
        else
            AfxMessageBox(_T("Verify Error !"));
        GetDlgItem(IDC_MMC_VERIFY)->EnableWindow(TRUE);
        GetDlgItem(IDC_MMC_VERIFY)->SetWindowText(_T("Verify"));
        //UpdateData(FALSE);
        mainWnd->m_gtype.EnableWindow(TRUE);
    } else
        AfxMessageBox(_T("Please choose comparing file !"));

    return ;
}

unsigned WINAPI CMMCDlg:: Verify_proc(void* args)
{
    CMMCDlg* pThis = reinterpret_cast<CMMCDlg*>(args);
    pThis->Verify();
    return 0;
}

void CMMCDlg::OnBnClickedMmcVerify()
{
    CString dlgText;

    UpdateData(TRUE);

    GetDlgItem(IDC_MMC_VERIFY)->GetWindowText(dlgText);

    if(m_type==PACK) {

        AfxMessageBox(_T("Pack image can't verify"));
        return;
    }

    if(dlgText.CompareNoCase(_T("Verify"))==0) {

        UpdateData(TRUE);

        unsigned Thread1;
        HANDLE hThread;

        hThread=(HANDLE)_beginthreadex(NULL,0,Verify_proc,(void*)this,0,&Thread1);
        CloseHandle(hThread);
    } else {
        //m_vcom.Close();
        SetEvent(m_ExitEvent);
        GetDlgItem(IDC_MMC_VERIFY)->EnableWindow(FALSE);
        m_progress.SetPos(0);
    }
}

void CMMCDlg::OnBnClickedMmcEncE()
{
    if(m_mmc_enc_check.GetCheck()==TRUE) {
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
        for(item=sList.begin(); item!=sList.end(); item++)
            m_combo_encrypt.AddString(*item);
        m_combo_encrypt.SetCurSel(0);
        //-----------------------------------------------------------------------
        m_combo_encrypt.EnableWindow(TRUE);
    } else
        m_combo_encrypt.EnableWindow(FALSE);
}


BOOL CMMCDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    //TmpOffset
    UpdateData(TRUE);


#if 1
    if(m_type==UBOOT) {
        if(TmpOffsetFlag==0) {
            TmpOffsetFlag=1;
            GetDlgItem(IDC_MMC_FLASHOFFSET_A)->GetWindowText(TmpOffset);
            GetDlgItem(IDC_MMC_FLASHOFFSET_A)->SetWindowText(_T("400"));
            GetDlgItem(IDC_MMC_FLASHOFFSET_A)->EnableWindow(FALSE);
            GetDlgItem(IDC_MMC_EXECADDR_A)->EnableWindow(TRUE);
            GetDlgItem(IDC_MMC_ENC_E)->EnableWindow(TRUE);
            if(m_mmc_enc_check.GetCheck()==(int)true)
                GetDlgItem(IDC_MMC_COMBO_ENCRYPT)->EnableWindow(TRUE);
            else
                GetDlgItem(IDC_MMC_COMBO_ENCRYPT)->EnableWindow(FALSE);
        }

    } else {

        if(TmpOffsetFlag==1) {
            TmpOffsetFlag=0;
            GetDlgItem(IDC_MMC_FLASHOFFSET_A)->SetWindowText(TmpOffset);
            GetDlgItem(IDC_MMC_FLASHOFFSET_A)->EnableWindow(TRUE);
        }

        if(m_type==PACK) {
            GetDlgItem(IDC_MMC_EXECADDR_A)->EnableWindow(FALSE);
            GetDlgItem(IDC_MMC_FLASHOFFSET_A)->EnableWindow(FALSE);
            GetDlgItem(IDC_MMC_ENC_E)->EnableWindow(FALSE);
            GetDlgItem(IDC_MMC_COMBO_ENCRYPT)->EnableWindow(FALSE);
        } else {
            GetDlgItem(IDC_MMC_EXECADDR_A)->EnableWindow(FALSE);
            GetDlgItem(IDC_MMC_FLASHOFFSET_A)->EnableWindow(TRUE);
            GetDlgItem(IDC_MMC_ENC_E)->EnableWindow(TRUE);
            if(m_mmc_enc_check.GetCheck()==(int)true)
                GetDlgItem(IDC_MMC_COMBO_ENCRYPT)->EnableWindow(TRUE);
            else
                GetDlgItem(IDC_MMC_COMBO_ENCRYPT)->EnableWindow(FALSE);
        }
    }

#endif
    return CDialog::OnCommand(wParam, lParam);
}

LRESULT CMMCDlg::ShowStatus( WPARAM  pos, LPARAM message)
{
    m_progress.SetPos((int)pos);
    return true;
}


BOOL CMMCDlg::InitFile(int flag)
{

    CString tmp;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    if(!mainWnd->m_inifile.ReadFile()) return false;
    switch(flag) {
    case 0:
        tmp=mainWnd->m_inifile.GetValue(_T("MMC"),_T("PATH"));
        m_filename=tmp;
        tmp=m_filename.Mid(m_filename.ReverseFind('\\')+1, m_filename.GetLength());
        if(tmp.ReverseFind('.')>0)
            m_imagename=tmp.Mid(0,tmp.ReverseFind('.'));
        else
            m_imagename=tmp;
        if(m_imagename.GetLength()>16)
            m_imagename = m_imagename.Mid(0,15);
        GetDlgItem(IDC_MMC_IMAGENAME_A)->SetWindowText(m_imagename);

        tmp=mainWnd->m_inifile.GetValue(_T("MMC"),_T("TYPE"));
        ((CButton *)GetDlgItem(IDC_MMC_TYPE_A))->SetCheck(FALSE);
        switch(_wtoi(tmp)) {
        case 0:
            ((CButton *)GetDlgItem(IDC_MMC_TYPE_A))->SetCheck(TRUE);
            break;
        case 1:
            ((CButton *)GetDlgItem(IDC_MMC_TYPE_A2))->SetCheck(TRUE);
            break;
        case 2:
            ((CButton *)GetDlgItem(IDC_MMC_TYPE_A4))->SetCheck(TRUE);
            break;
        case 3:
            ((CButton *)GetDlgItem(IDC_MMC_TYPE_A3))->SetCheck(TRUE);
            break;
        }

        tmp=mainWnd->m_inifile.GetValue(_T("MMC"),_T("EXECADDR"));
        GetDlgItem(IDC_MMC_EXECADDR_A)->SetWindowText(tmp);
        m_execaddr=tmp;
        tmp=mainWnd->m_inifile.GetValue(_T("MMC"),_T("OFFSET"));
        GetDlgItem(IDC_MMC_FLASHOFFSET_A)->SetWindowText(tmp);

        tmp=mainWnd->m_inifile.GetValue(_T("MMC"),_T("ENC_EN"));
        if(_wtoi(tmp)==1) {
            tmp=mainWnd->m_inifile.GetValue(_T("MMC"),_T("ENC_SEL"));
            m_mmc_enc_check.SetCheck(TRUE);
            OnBnClickedMmcEncE();
            int flg=_wtoi(tmp);
            if(flg>=0 && flg<m_combo_encrypt.GetCount()) {
                m_combo_encrypt.SetCurSel(flg);
            }
        }
        //m_startblock=tmp;
        break;
    case 1:
        mainWnd->m_inifile.SetValue(_T("MMC"),_T("PATH"),m_filename);
        tmp.Format(_T("%d"),m_type);
        mainWnd->m_inifile.SetValue(_T("MMC"),_T("TYPE"),tmp);
        mainWnd->m_inifile.SetValue(_T("MMC"),_T("EXECADDR"),m_execaddr);
        mainWnd->m_inifile.SetValue(_T("MMC"),_T("OFFSET"),m_startblock);
        tmp.Format(_T("%d"),m_mmc_enc_check.GetCheck());
        mainWnd->m_inifile.SetValue(_T("MMC"),_T("ENC_EN"),tmp);
        tmp.Format(_T("%d"),m_combo_encrypt.GetCurSel());
        mainWnd->m_inifile.SetValue(_T("MMC"),_T("ENC_SEL"),tmp);
        mainWnd->m_inifile.WriteFile();
        break;
    default:
        break;
    }
    return true;
}

void CMMCDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);


    // TODO: Add your message handler code here
    m_imagelist.DeleteAllItems();
    if(InitFlag==0) {
        InitFlag=1;
        InitFile(0);
    }
    //UpdateData (TRUE);
}

