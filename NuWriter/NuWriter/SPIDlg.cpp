// SPIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NuWriter.h"
#include "NuWriterDlg.h"

// CSPIDlg dialog

IMPLEMENT_DYNAMIC(CSPIDlg, CDialog)

CSPIDlg::CSPIDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CSPIDlg::IDD, pParent)
{
    TmpOffsetFlag=0;
    InitFlag=0;
    m_type = 0;
    encbuf=NULL;
    m_fhead=(NORBOOT_NAND_HEAD *)malloc(sizeof(NORBOOT_NAND_HEAD));
    TRACE(_T("CSPIDlg  malloc %d \n"), sizeof(NORBOOT_NAND_HEAD));
}

CSPIDlg::~CSPIDlg()
{
    //TRACE(_T("~CSPIDlg  free %d \n"), sizeof(NORBOOT_NAND_HEAD));
    free(m_fhead);
}

void CSPIDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SPI_IMAGELIST, m_imagelist);
    DDX_Control(pDX, IDC_SPI_DOWNPROGRESS, m_progress);
    DDX_Text(pDX, IDC_SPI_IMAGENAME_A, m_imagename);
    DDX_Text(pDX, IDC_SPI_FLASHOFFSET_A, m_startblock);
    DDV_MaxChars(pDX, m_startblock, 8);
    DDX_Text(pDX, IDC_SPI_EXECADDR_A, m_execaddr);
    DDV_MaxChars(pDX, m_execaddr, 8);
    DDX_Radio(pDX, IDC_SPI_TYPE_A, m_type);
    DDX_Control(pDX, IDC_SPI_ENC_E, m_spi_enc_check);
    DDX_Control(pDX, IDC_SPI_COMBO_ENCRYPT, m_combo_encrypt);

    DDX_Control(pDX, IDC_SPI_DOWNLOAD, m_burn);
    DDX_Control(pDX, IDC_SPI_VERIFY, m_verify);
    DDX_Control(pDX, IDC_SPI_READ, m_read);
    DDX_Control(pDX, IDC_SPI_ERASEALL, m_eraseall);
    DDX_Control(pDX, IDC_SPI_BROWSE, m_browse);
    DDX_Control(pDX, IDC_SPI_STATUS, m_status);
}


BEGIN_MESSAGE_MAP(CSPIDlg, CDialog)
    ON_BN_CLICKED(IDC_SPI_DOWNLOAD, &CSPIDlg::OnBnClickedSpiDownload)
    ON_BN_CLICKED(IDC_SPI_VERIFY, &CSPIDlg::OnBnClickedSpiVerify)
    ON_BN_CLICKED(IDC_SPI_READ, &CSPIDlg::OnBnClickedSpiRead)
    ON_BN_CLICKED(IDC_SPI_ERASEALL, &CSPIDlg::OnBnClickedSpiEraseall)
    ON_BN_CLICKED(IDC_SPI_BROWSE, &CSPIDlg::OnBnClickedSpiBrowse)
    ON_BN_CLICKED(IDC_SPI_ENC_E, &CSPIDlg::OnBnClickedSpiEncE)
    ON_MESSAGE(WM_SPI_PROGRESS,ShowStatus)
    ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CSPIDlg message handlers

BOOL CSPIDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    //m_imagelist.SetHeadings(_T("Name, 80; Type, 50;Start offset, 100;End offset, 90"));
    //m_imagelist.SetHeadings(_T("Name, 80; Type, 40;Start offset, 75;End offset, 75"));
    m_imagelist.SetHeadings(_T("Name, 70; Type, 50;Start offset, 80;End offset, 80"));
    m_imagelist.SetGridLines(TRUE);


    m_ExitEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

    m_progress.SetRange(0,100);
    m_progress.SetBkColor(COLOR_DOWNLOAD);
    m_filename="";
    GetDlgItem(IDC_SPI_VERIFY)->EnableWindow(FALSE);

    COLORREF col = RGB(0xFF, 0x00, 0xFF);
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

void CSPIDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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

void CSPIDlg::Download()
{
    int i=0;
    BOOLEAN ret;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    //UpdateData(FALSE);

    if(!m_filename.IsEmpty()) {
        int len,startblock,endblock;
        mainWnd->m_gtype.EnableWindow(FALSE);
        GetDlgItem(IDC_SPI_VERIFY)->EnableWindow(FALSE);
        ResetEvent(m_ExitEvent);
        GetDlgItem(IDC_SPI_DOWNLOAD)->SetWindowText(_T("Abort"));

        if(m_type!=PACK) {
            ret=XUSB_Burn(mainWnd->m_portName,m_filename,&len);
            if(ret) {
                GetDlgItem(IDC_SPI_VERIFY)->EnableWindow(TRUE);
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
                //endblock=startblock+((len+(SPI64K-1))/(SPI64K))*(SPI64K);
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
                GetDlgItem(IDC_SPI_VERIFY)->EnableWindow(TRUE);
                AfxMessageBox(_T("Burn successfully"));
            }
        }

        GetDlgItem(IDC_SPI_DOWNLOAD)->EnableWindow(TRUE);

        GetDlgItem(IDC_SPI_DOWNLOAD)->SetWindowText(_T("Burn"));
        //UpdateData(FALSE);
        mainWnd->m_gtype.EnableWindow(TRUE);

        //if(ret)
        //{
        //    CSPIComDlg* parent=(CSPIComDlg*)GetParent();

        //    if(parent)
        //        parent->PostMessage(WM_SPI_UPDATE_LIST,0,0);
        //}

    } else
        AfxMessageBox(_T("Please choose image file !"));
    return ;

}

unsigned WINAPI CSPIDlg:: Download_proc(void* args)
{
    CSPIDlg* pThis = reinterpret_cast<CSPIDlg*>(args);
    pThis->Download();
    return 0;
}

void CSPIDlg::OnBnClickedSpiDownload()
{
    InitFile(1);
    CString dlgText;
    int _startblock=0;

    //UpdateData(TRUE);
    if(m_imagename.IsEmpty()) {
        AfxMessageBox(_T("Please input image file"));
        return;
    }

    if( (m_type!=UBOOT)&&((m_execaddr.IsEmpty())||(m_startblock.IsEmpty()) ) && (m_type!=PACK)) {

        AfxMessageBox(_T("Please input image information"));
        return;
    }

    _stscanf_s(m_startblock,_T("%x"),&_startblock);

    if((m_type!=UBOOT)&&(_startblock%(SPI64K)!=0) && (m_type!=PACK)) {
        AfxMessageBox(_T("Start offset must be 64K (0x10000) boundary !"));
        return;
    }

    GetDlgItem(IDC_SPI_DOWNLOAD)->GetWindowText(dlgText);


    if(dlgText.CompareNoCase(_T("Burn"))==0) {

        UpdateData(TRUE);

        if(::MessageBox(this->m_hWnd,_T("Do you confirm this operation ?"),_T("Nu Writer"),MB_OKCANCEL|MB_ICONWARNING)==IDCANCEL)
            return;

        unsigned Thread1;
        HANDLE hThread;

        m_progress.SetPos(0);
        m_progress.SetBkColor(COLOR_DOWNLOAD);

        hThread=(HANDLE)_beginthreadex(NULL,0,Download_proc,(void*)this,0,&Thread1);
        CloseHandle(hThread);
    } else {
        //m_vcom.Close();
        SetEvent(m_ExitEvent);
        GetDlgItem(IDC_SPI_DOWNLOAD)->EnableWindow(FALSE);
        m_progress.SetPos(0);
    }

}


unsigned WINAPI CSPIDlg:: Verify_proc(void* args)
{
    CSPIDlg* pThis = reinterpret_cast<CSPIDlg*>(args);
    pThis->Verify();
    return 0;
}

void CSPIDlg:: Verify()
{

    int i=0;
    BOOLEAN ret;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    //UpdateData(FALSE);

    if(!m_filename.IsEmpty()) {
        mainWnd->m_gtype.EnableWindow(FALSE);
        ResetEvent(m_ExitEvent);
        GetDlgItem(IDC_SPI_VERIFY)->SetWindowText(_T("Abort"));
        ret=XUSB_Verify(mainWnd->m_portName,m_filename);
        if(ret)
            AfxMessageBox(_T("Verify OK !"));
        else
            AfxMessageBox(_T("Verify Error !"));
        GetDlgItem(IDC_SPI_VERIFY)->EnableWindow(TRUE);
        GetDlgItem(IDC_SPI_VERIFY)->SetWindowText(_T("Verify"));
        //UpdateData(FALSE);
        mainWnd->m_gtype.EnableWindow(TRUE);


    } else
        AfxMessageBox(_T("Please choose comparing file !"));

    return ;

}
void CSPIDlg::OnBnClickedSpiVerify()
{
    CString dlgText;

    UpdateData(TRUE);

    GetDlgItem(IDC_SPI_VERIFY)->GetWindowText(dlgText);

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
        GetDlgItem(IDC_SPI_VERIFY)->EnableWindow(FALSE);
        m_progress.SetPos(0);
    }
}


void CSPIDlg:: Read()
{
    int i=0;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    //UpdateData(FALSE);

    if(!m_filename2.IsEmpty()) {
        mainWnd->m_gtype.EnableWindow(FALSE);
        ResetEvent(m_ExitEvent);
        GetDlgItem(IDC_SPI_READ)->EnableWindow(FALSE);

        int blocks,sblocks;
        _stscanf_s(m_blocks,_T("%d"),&blocks);
        _stscanf_s(m_sblocks,_T("%d"),&sblocks);
        if(XUSB_Read(mainWnd->m_portName,m_filename2,sblocks*64*1024,blocks*64*1024))
            AfxMessageBox(_T("Read OK !"));
        else
            AfxMessageBox(_T("Read Error !"));
        GetDlgItem(IDC_SPI_READ)->EnableWindow(TRUE);

        //UpdateData(FALSE);
        mainWnd->m_gtype.EnableWindow(TRUE);

    } else
        AfxMessageBox(_T("Please choose read file !"));

    return ;
}

unsigned WINAPI CSPIDlg::Read_proc(void* args)
{
    CSPIDlg* pThis = reinterpret_cast<CSPIDlg*>(args);
    pThis->Read();
    return 0;
}

void CSPIDlg::OnBnClickedSpiRead()
{
    CReadDlg read_dlg;
    //read_dlg.SizeName.Format(_T("blocks(1 block is 64  KB)"));
    //read_dlg.SizeName.Format(_T("blocks(1 block is erase size)"));
    read_dlg.SizeName.Format(_T("Blocks"));
    read_dlg.type=0; /* 0 => SPI, 1 =>NAND, 2=>MMC */
    if(read_dlg.DoModal()==IDCANCEL) return;
    m_filename2=read_dlg.m_filename2;
    m_blocks=read_dlg.block;
    m_sblocks=read_dlg.sblock;

    CString dlgText;

    UpdateData(TRUE);

    GetDlgItem(IDC_SPI_READ)->GetWindowText(dlgText);
    if(dlgText.CompareNoCase(_T("Read"))==0) {
        UpdateData(TRUE);
        unsigned Thread1;
        HANDLE hThread;
        m_progress.SetPos(0);
        m_progress.SetBkColor(COLOR_VERIFY);
        hThread=(HANDLE)_beginthreadex(NULL,0,Read_proc,(void*)this,0,&Thread1);
        CloseHandle(hThread);
    } else {
        //SetEvent(m_ExitEvent);
        //GetDlgItem(IDC_SPI_READ)->EnableWindow(FALSE);
        m_progress.SetPos(0);
    }
}

void CSPIDlg:: Erase()
{

    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    BOOLEAN ret;
    //UpdateData(FALSE);

    mainWnd->m_gtype.EnableWindow(FALSE);
    ResetEvent(m_ExitEvent);
    ret=XUSB_Erase(mainWnd->m_portName);
    if(ret)
        AfxMessageBox(_T("Erase successfully"));
    //else
    //	AfxMessageBox("Erase unsuccessfully!! Please check device");
    GetDlgItem(IDC_SPI_ERASEALL)->EnableWindow(TRUE);
    //UpdateData(FALSE);
    mainWnd->m_gtype.EnableWindow(TRUE);

    //if(ret)
    //{
    //    CSPIComDlg* parent=(CSPIComDlg*)GetParent();

    //    if(parent)
    //        parent->PostMessage(WM_SPI_UPDATE_LIST,0,0);

    //}
    return ;
}

unsigned WINAPI CSPIDlg::Erase_proc(void* args)
{
    CSPIDlg* pThis = reinterpret_cast<CSPIDlg*>(args);
    pThis->Erase();
    return 0;
}

void CSPIDlg::OnBnClickedSpiEraseall()
{
    CString dlgText;

#if 1
    CEraseDlg erase_dlg;
    //read_dlg.SizeName.Format(_T("Blocks(1 block is 0x%05x bytes)"),NAND_SIZE);
    erase_dlg.SizeName.Format(_T("blocks"));
    erase_dlg.type=0; /* 0 => SPI, 1 =>NAND, 2=>MMC */
    if(erase_dlg.DoModal()==IDCANCEL) return;

    m_blocks=erase_dlg.block;
    m_sblocks=erase_dlg.sblock;
    m_erase_flag=erase_dlg.m_erase_type;
#else
    if(::MessageBox(this->m_hWnd,_T("Do you confirm this operation ?"),_T("Nu Writer"),MB_OKCANCEL|MB_ICONWARNING)==IDCANCEL)
        return;
#endif
    UpdateData(TRUE);
    GetDlgItem(IDC_SPI_ERASEALL)->GetWindowText(dlgText);

    if(dlgText.CompareNoCase(_T("Erase"))==0) {

        UpdateData(TRUE);

        unsigned Thread1;
        HANDLE hThread;

        m_progress.SetPos(0);
        m_progress.SetBkColor(COLOR_ERASE);
        hThread=(HANDLE)_beginthreadex(NULL,0,Erase_proc,(void*)this,0,&Thread1);
        CloseHandle(hThread);
    } else {
        SetEvent(m_ExitEvent);
        GetDlgItem(IDC_SPI_ERASEALL)->EnableWindow(FALSE);
        m_progress.SetPos(0);
    }

}

void CSPIDlg::OnBnClickedSpiBrowse()
{
    UpdateData(TRUE);

    //CAddFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,_T("Bin,Img Files  (*.bin;*.img;*.gz)|*.bin;*.img;*.gz|All Files (*.*)|*.*||"));
    //CAddFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,_T("All Files (*.*)|*.*||"));
    CAddFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,_T("Bin Files (*.bin)|*.bin|All Files (*.*)|*.*||"));

    dlg.m_ofn.lpstrTitle=_T("Choose burning file...");
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    if(!mainWnd->m_inifile.ReadFile())
        dlg.m_ofn.lpstrInitialDir=_T("c:");
    else {
        CString _path;
        _path=mainWnd->m_inifile.GetValue(_T("SPI"),_T("PATH"));
        if(_path.IsEmpty())
            dlg.m_ofn.lpstrInitialDir=_T("c:");
        else
            dlg.m_ofn.lpstrInitialDir=_path;
    }


    BOOLEAN ret=dlg.DoModal();

    if(ret==IDCANCEL) {
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

    this->GetDlgItem(IDC_SPI_IMAGENAME_A)->SetWindowText(m_imagename);

    //UpdateData(FALSE);
    //CString filepath=m_filename.Left(m_filename.GetLength()-dlg.GetFileName().GetLength()-1);

    mainWnd->m_inifile.SetValue(_T("SPI"),_T("PATH"),m_filename);
    mainWnd->m_inifile.WriteFile();

}

void CSPIDlg::OnBnClickedSpiEncE()
{
    if(m_spi_enc_check.GetCheck()==TRUE) {
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

BOOL CSPIDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    //TmpOffset
    UpdateData(TRUE);
    if(m_type==UBOOT) {
        if(TmpOffsetFlag==0) {
            TmpOffsetFlag=1;
            GetDlgItem(IDC_SPI_FLASHOFFSET_A)->GetWindowText(TmpOffset);
            GetDlgItem(IDC_SPI_FLASHOFFSET_A)->SetWindowText(_T("0"));
            GetDlgItem(IDC_SPI_FLASHOFFSET_A)->EnableWindow(FALSE);
            GetDlgItem(IDC_SPI_EXECADDR_A)->EnableWindow(TRUE);

            GetDlgItem(IDC_SPI_ENC_E)->EnableWindow(TRUE);
            if(m_spi_enc_check.GetCheck()==TRUE)
                GetDlgItem(IDC_SPI_COMBO_ENCRYPT)->EnableWindow(TRUE);
            else
                GetDlgItem(IDC_SPI_COMBO_ENCRYPT)->EnableWindow(FALSE);
        }

    } else {

        if(TmpOffsetFlag==1) {
            TmpOffsetFlag=0;
            GetDlgItem(IDC_SPI_FLASHOFFSET_A)->SetWindowText(TmpOffset);
            GetDlgItem(IDC_SPI_FLASHOFFSET_A)->EnableWindow(TRUE);
        }


        if(m_type==PACK) {
            GetDlgItem(IDC_SPI_EXECADDR_A)->EnableWindow(FALSE);
            GetDlgItem(IDC_SPI_FLASHOFFSET_A)->EnableWindow(FALSE);
            GetDlgItem(IDC_SPI_ENC_E)->EnableWindow(FALSE);
            GetDlgItem(IDC_SPI_COMBO_ENCRYPT)->EnableWindow(FALSE);
        } else {
            GetDlgItem(IDC_SPI_EXECADDR_A)->EnableWindow(FALSE);
            GetDlgItem(IDC_SPI_FLASHOFFSET_A)->EnableWindow(TRUE);
            GetDlgItem(IDC_SPI_ENC_E)->EnableWindow(TRUE);
            if(m_spi_enc_check.GetCheck()==TRUE)
                GetDlgItem(IDC_SPI_COMBO_ENCRYPT)->EnableWindow(TRUE);
            else
                GetDlgItem(IDC_SPI_COMBO_ENCRYPT)->EnableWindow(FALSE);
        }
    }



    return CDialog::OnCommand(wParam, lParam);
}

LRESULT CSPIDlg::ShowStatus( WPARAM  pos, LPARAM message)
{
    m_progress.SetPos((int)pos);
    return true;
}


BOOL CSPIDlg::InitFile(int flag)
{

    CString tmp;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    if(!mainWnd->m_inifile.ReadFile()) return false;
    switch(flag) {
    case 0:
        tmp=mainWnd->m_inifile.GetValue(_T("SPI"),_T("PATH"));
        m_filename=tmp;

        tmp=m_filename.Mid(m_filename.ReverseFind('\\')+1, m_filename.GetLength());
        if(tmp.ReverseFind('.')>0)
            m_imagename=tmp.Mid(0,tmp.ReverseFind('.'));
        else
            m_imagename=tmp;
        if(m_imagename.GetLength()>16)
            m_imagename = m_imagename.Mid(0,15);
        GetDlgItem(IDC_SPI_IMAGENAME_A)->SetWindowText(m_imagename);

        tmp=mainWnd->m_inifile.GetValue(_T("SPI"),_T("TYPE"));
        ((CButton *)GetDlgItem(IDC_SPI_TYPE_A))->SetCheck(FALSE);
        switch(_wtoi(tmp)) {
        case 0:
            ((CButton *)GetDlgItem(IDC_SPI_TYPE_A))->SetCheck(TRUE);
            break;
        case 1:
            ((CButton *)GetDlgItem(IDC_SPI_TYPE_A2))->SetCheck(TRUE);
            break;
        case 2:
            ((CButton *)GetDlgItem(IDC_SPI_TYPE_A4))->SetCheck(TRUE);
            break;
        case 3:
            ((CButton *)GetDlgItem(IDC_SPI_TYPE_A3))->SetCheck(TRUE);
            break;
        }

        tmp=mainWnd->m_inifile.GetValue(_T("SPI"),_T("EXECADDR"));
        GetDlgItem(IDC_SPI_EXECADDR_A)->SetWindowText(tmp);
        m_execaddr=tmp;
        tmp=mainWnd->m_inifile.GetValue(_T("SPI"),_T("OFFSET"));
        GetDlgItem(IDC_SPI_FLASHOFFSET_A)->SetWindowText(tmp);
        tmp=mainWnd->m_inifile.GetValue(_T("SPI"),_T("ENC_EN"));
        if(_wtoi(tmp)==1) {
            tmp=mainWnd->m_inifile.GetValue(_T("SPI"),_T("ENC_SEL"));
            m_spi_enc_check.SetCheck(TRUE);
            OnBnClickedSpiEncE();
            int flg=_wtoi(tmp);
            if(flg>=0 && flg<m_combo_encrypt.GetCount()) {
                m_combo_encrypt.SetCurSel(flg);
            }
        }

        //m_startblock=tmp;
        break;
    case 1:
        mainWnd->m_inifile.SetValue(_T("SPI"),_T("PATH"),m_filename);
        tmp.Format(_T("%d"),m_type);
        mainWnd->m_inifile.SetValue(_T("SPI"),_T("TYPE"),tmp);
        mainWnd->m_inifile.SetValue(_T("SPI"),_T("EXECADDR"),m_execaddr);
        mainWnd->m_inifile.SetValue(_T("SPI"),_T("OFFSET"),m_startblock);

        tmp.Format(_T("%d"),m_spi_enc_check.GetCheck());
        mainWnd->m_inifile.SetValue(_T("SPI"),_T("ENC_EN"),tmp);
        tmp.Format(_T("%d"),m_combo_encrypt.GetCurSel());
        mainWnd->m_inifile.SetValue(_T("SPI"),_T("ENC_SEL"),tmp);
        mainWnd->m_inifile.WriteFile();
        break;
    default:
        break;
    }
    return true;
}



