// NANDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NuWriter.h"
#include "NuWriterDlg.h"
#include "NANDDlg.h"
#include "NandInfoDlg.h"


// CNANDDlg dialog

IMPLEMENT_DYNAMIC(CNANDDlg, CDialog)

CNANDDlg::CNANDDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CNANDDlg::IDD, pParent)
{
    TmpOffsetFlag=0;
    encbuf=NULL;
    enclen=0;
    InitFlag=0;
    m_type = 0;
    m_fhead=(NORBOOT_NAND_HEAD *)malloc(sizeof(NORBOOT_NAND_HEAD));
    TRACE(_T("CNANDDlg  malloc %d \n"), sizeof(NORBOOT_NAND_HEAD));
}

CNANDDlg::~CNANDDlg()
{
    //TRACE(_T("~CNANDDlg  free %d \n"), sizeof(NORBOOT_NAND_HEAD));
    free(m_fhead);
}

void CNANDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NAND_IMAGELIST, m_imagelist);
	DDX_Text(pDX, IDC_NAND_IMAGENAME_A, m_imagename);
	DDX_Control(pDX, IDC_NAND_PROGRESS, m_progress);

	DDX_Text(pDX, IDC_NAND_FLASHOFFSET_A, m_startblock);
	DDV_MaxChars(pDX, m_startblock, 8);
	DDX_Text(pDX, IDC_NAND_EXECADDR_A, m_execaddr);
	DDV_MaxChars(pDX, m_execaddr, 8);

	DDX_Control(pDX, IDC_NAND_DOWNLOAD, m_burn);
	DDX_Control(pDX, IDC_NAND_VERIFY, m_verify);
	DDX_Control(pDX, IDC_NAND_READ, m_read);
	DDX_Control(pDX, IDC_NAND_ERASEALL, m_eraseall);
	DDX_Control(pDX, IDC_NAND_BROWSE, m_browse);
	DDX_Control(pDX, IDC_NAND_ENC_E, m_nand_enc_check);
	DDX_Control(pDX, IDC_NAND_COMBO_ENCRYPT, m_combo_encrypt);
	DDX_Radio(pDX, IDC_NAND_TYPE_A, m_type);
	DDX_Control(pDX, IDC_NAND_STATUS, m_status);
	DDX_Control(pDX, IDC_NAND_USRCONFIG, m_nandflash_check);
}


BEGIN_MESSAGE_MAP(CNANDDlg, CDialog)
    ON_BN_CLICKED(IDC_NAND_DOWNLOAD, &CNANDDlg::OnBnClickedNandDownload)
    ON_BN_CLICKED(IDC_NAND_VERIFY, &CNANDDlg::OnBnClickedNandVerify)
    ON_BN_CLICKED(IDC_NAND_READ, &CNANDDlg::OnBnClickedNandRead)
    ON_BN_CLICKED(IDC_NAND_ERASEALL, &CNANDDlg::OnBnClickedNandEraseall)
    ON_BN_CLICKED(IDC_NAND_BROWSE, &CNANDDlg::OnBnClickedNandBrowse)
    ON_BN_CLICKED(IDC_NAND_ENC_E, &CNANDDlg::OnBnClickedNandEncE)
    ON_MESSAGE(WM_NAND_PROGRESS,ShowStatus)
    ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_NAND_USRCONFIG, &CNANDDlg::OnBnClickedNandUsrconfig)
END_MESSAGE_MAP()


// CNANDDlg message handlers
BOOL CNANDDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    //m_imagelist.SetHeadings(_T("Name, 80; Type, 40;Start offset, 75;End offset, 75"));
    //m_imagelist.SetHeadings(_T("Name, 70; Type, 50;Start,70;End,70;Block, 60"));
    m_imagelist.SetHeadings(_T("Name, 70; Type, 50;Start, 60;End, 60;Block, 40"));
    m_imagelist.SetGridLines(TRUE);


    m_ExitEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

    m_progress.SetRange(0,100);
    m_progress.SetBkColor(COLOR_DOWNLOAD);
    m_filename="";
    GetDlgItem(IDC_NAND_VERIFY)->EnableWindow(FALSE);

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

void CNANDDlg::Download()
{
    int i=0;
    BOOLEAN ret;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    //UpdateData(FALSE);

    if(!m_filename.IsEmpty()) {
        int len,startblock,endblock;
        mainWnd->m_gtype.EnableWindow(FALSE);
        GetDlgItem(IDC_NAND_VERIFY)->EnableWindow(FALSE);
        ResetEvent(m_ExitEvent);
        GetDlgItem(IDC_NAND_DOWNLOAD)->SetWindowText(_T("Abort"));

        if(m_type!=PACK) {
            int blockNum;
            if(mainWnd->ChipWriteWithOOB==0)
                ret=XUSB_Burn(mainWnd->m_portName,m_filename,&len,&blockNum);
            else
                ret=XUSB_BurnWithOOB(mainWnd->m_portName,m_filename,&len,&blockNum);

            if(ret) {
                GetDlgItem(IDC_NAND_VERIFY)->EnableWindow(TRUE);
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
                case IMAGE:
                    flagstr.Format(_T("FS"));
                    break;
                }
                _stscanf_s(m_startblock,_T("%x"),&startblock);
                //endblock=startblock+((len+(NAND_SIZE-1))/(NAND_SIZE))*(NAND_SIZE);
                endblock=startblock+len;

                CString _startblock,_endblock,_blockNum;
                _startblock.Format(_T("0x%x"),startblock);
                _endblock.Format(_T("0x%x"),endblock);
                _blockNum.Format(_T("0x%x"),blockNum);
                m_imagelist.InsertItem(0,m_imagename,flagstr,_startblock,_endblock,_blockNum);
            }
            //else
            //	AfxMessageBox("Burn unsuccessfully!! Please check device");
        } else {
            ret=XUSB_Pack(mainWnd->m_portName,m_filename,&len);
            if(ret) {
                GetDlgItem(IDC_NAND_VERIFY)->EnableWindow(TRUE);
                AfxMessageBox(_T("Burn successfully"));
            }
        }

        GetDlgItem(IDC_NAND_DOWNLOAD)->EnableWindow(TRUE);

        GetDlgItem(IDC_NAND_DOWNLOAD)->SetWindowText(_T("Burn"));
        //UpdateData(FALSE);
        mainWnd->m_gtype.EnableWindow(TRUE);

    } else
        AfxMessageBox(_T("Please choose image file !"));
    return ;

}

unsigned WINAPI CNANDDlg:: Download_proc(void* args)
{
    CNANDDlg* pThis = reinterpret_cast<CNANDDlg*>(args);
    pThis->Download();
    return 0;
}

void CNANDDlg::OnBnClickedNandDownload()
{
    // TODO: Add your control notification handler code here
    InitFile(1);

    CString dlgText;
    int _startblock=0;

    //UpdateData(TRUE);
    if(m_imagename.IsEmpty()) {
        AfxMessageBox(_T("Please input image file"));
        return;
    }

    if((m_type==IMAGE)&&(m_nand_enc_check.GetCheck()==TRUE)) {

        AfxMessageBox(_T("Write file system can not used encrypt"));
        return;
    }

    if((m_type==ENV)&&(m_nand_enc_check.GetCheck()==TRUE)) {

        AfxMessageBox(_T("Write Enviroment can not used encrypt"));
        return;
    }


    _stscanf_s(m_startblock,_T("%x"),&_startblock);
	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
	CNandInfoDlg* nandinfoWnd=(CNandInfoDlg*)(AfxGetApp()->m_pMainWnd);
	CString tmp;
	if(m_nandflash_check.GetCheck()==TRUE) // user configure
	{
		CString tmp;
	    tmp.Format(_T("NAND parameters by User Configure.\nBlockPerFlash =%d, PagePerBlock = %d"),mainWnd->m_info.Nand_uBlockPerFlash, mainWnd->m_info.Nand_uPagePerBlock);
		TRACE(_T("User Configure: %s\n"), tmp);
		AfxMessageBox(tmp);
	}
	else
	{
		TRACE(_T("Auto Detect: BlockPerFlash =%d, PagePerBlock = %d\n"),  mainWnd->m_info.Nand_uBlockPerFlash, mainWnd->m_info.Nand_uPagePerBlock);
	}


    unsigned int val=mainWnd->m_info.Nand_uPagePerBlock * mainWnd->m_info.Nand_uPageSize;

    if(mainWnd->ChipWriteWithOOB!=1) {
        if(val>0 && val<=0x800000) {
            if((m_type!=UBOOT)&&(_startblock%(val)!=0) && (m_type!=PACK)) {
                CString tmp;
                tmp.Format(_T("Start offset must be %dK (0x%X) boundary! Do you confirm this operation ?"),val/1024,val);
#if 0
                AfxMessageBox(tmp);
                return;
#else
                if(::MessageBox(this->m_hWnd,tmp,_T("Nu Writer"),MB_OKCANCEL|MB_ICONWARNING)==IDCANCEL)
                    return;
#endif

            }
            if( (m_type!=UBOOT)&&(((unsigned int)_startblock<(val*4)) ) && (m_type!=PACK)) {
                CString tmp;
                tmp.Format(_T("Between 0x0 ~ 0x%X use by uBOOT. Do you confirm this operation ?"),4*val-1);
#if 0
                AfxMessageBox(tmp);
                return;
#else
                if(::MessageBox(this->m_hWnd,tmp,_T("Nu Writer"),MB_OKCANCEL|MB_ICONWARNING)==IDCANCEL)
                    return;
#endif
            }
        }
    } else {

    }

    GetDlgItem(IDC_NAND_DOWNLOAD)->GetWindowText(dlgText);


    if(dlgText.CompareNoCase(_T("Burn"))==0) {

        UpdateData(TRUE);

        if(::MessageBox(this->m_hWnd,_T("Do you confirm this operation ?"),_T("Nu Writer"),MB_OKCANCEL|MB_ICONWARNING)==IDCANCEL)
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
        GetDlgItem(IDC_SPI_DOWNLOAD)->EnableWindow(FALSE);
        m_progress.SetPos(0);
    }
}

void CNANDDlg:: Verify()
{

    int i=0;
    int ret;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    //UpdateData(FALSE);

    if(!m_filename.IsEmpty()) {
        mainWnd->m_gtype.EnableWindow(FALSE);
        ResetEvent(m_ExitEvent);
        GetDlgItem(IDC_NAND_VERIFY)->SetWindowText(_T("Abort"));
        ret=XUSB_Verify(mainWnd->m_portName,m_filename);
        if(ret==1)
            AfxMessageBox(_T("Verify OK !"));
        else {
            if(ret==NAND_VERIFY_FILESYSTEM_ERROR)
                AfxMessageBox(_T("This File System can't verify"));
            else if(ret==NAND_VERIFY_PACK_ERROR)
                AfxMessageBox(_T("Pack image can't verify"));
            else
                AfxMessageBox(_T("Verify Error !"));
        }
        GetDlgItem(IDC_NAND_VERIFY)->EnableWindow(TRUE);
        GetDlgItem(IDC_NAND_VERIFY)->SetWindowText(_T("Verify"));
        //UpdateData(FALSE);
        mainWnd->m_gtype.EnableWindow(TRUE);


    } else
        AfxMessageBox(_T("Please choose comparing file !"));

    return ;

}
unsigned WINAPI CNANDDlg:: Verify_proc(void* args)
{
    CNANDDlg* pThis = reinterpret_cast<CNANDDlg*>(args);
    pThis->Verify();
    return 0;
}
void CNANDDlg::OnBnClickedNandVerify()
{
    CString dlgText;

    UpdateData(TRUE);

    GetDlgItem(IDC_NAND_VERIFY)->GetWindowText(dlgText);

    if(dlgText.CompareNoCase(_T("Verify"))==0) {

        UpdateData(TRUE);

        unsigned Thread1;
        HANDLE hThread;

        hThread=(HANDLE)_beginthreadex(NULL,0,Verify_proc,(void*)this,0,&Thread1);
        CloseHandle(hThread);
    } else {
        //m_vcom.Close();
        SetEvent(m_ExitEvent);
        GetDlgItem(IDC_NAND_VERIFY)->EnableWindow(FALSE);
        m_progress.SetPos(0);
    }
}

void CNANDDlg:: Read()
{
    int i=0;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    int blocksize = mainWnd->m_info.Nand_uPagePerBlock*mainWnd->m_info.Nand_uPageSize;
    m_progress.SetRange(0,100);
    if(blocksize==0) {
        AfxMessageBox(_T("Can't get NAND flash size, Please reconnet to device\n"));
        return;
    }
    //UpdateData(FALSE);

    if(!m_filename2.IsEmpty()) {
        mainWnd->m_gtype.EnableWindow(FALSE);
        ResetEvent(m_ExitEvent);
        GetDlgItem(IDC_NAND_READ)->EnableWindow(FALSE);

        int blocks,sblocks;
        _stscanf_s(m_blocks,_T("%d"),&blocks);
        _stscanf_s(m_sblocks,_T("%d"),&sblocks);

        if(mainWnd->ChipReadWithBad==0) { //read good block
            if(XUSB_Read(mainWnd->m_portName,m_filename2,sblocks,blocks*blocksize))
                AfxMessageBox(_T("Read OK !"));
            else
                AfxMessageBox(_T("Read Error !"));
        } else { //read redunancy data, good block and bad block
            if(XUSB_Read_Redunancy(mainWnd->m_portName,m_filename2,sblocks,blocks))
                AfxMessageBox(_T("Read OK !"));
            else
                AfxMessageBox(_T("Read Error !"));
        }
        GetDlgItem(IDC_NAND_READ)->EnableWindow(TRUE);

        //UpdateData(FALSE);
        mainWnd->m_gtype.EnableWindow(TRUE);

    } else
        AfxMessageBox(_T("Please choose read file !"));

    return ;
}

unsigned WINAPI CNANDDlg::Read_proc(void* args)
{
    CNANDDlg* pThis = reinterpret_cast<CNANDDlg*>(args);
    pThis->Read();
    return 0;
}

void CNANDDlg::OnBnClickedNandRead()
{
    CReadDlg read_dlg;
    //read_dlg.SizeName.Format(_T("Blocks(1 block is 0x%05x bytes)"),NAND_SIZE);
    read_dlg.SizeName.Format(_T("Blocks"));
    read_dlg.type=1; /* 0 => SPI, 1 =>NAND, 2=>MMC */
    if(read_dlg.DoModal()==IDCANCEL) return;
    m_filename2=read_dlg.m_filename2;
    m_blocks=read_dlg.block;
    m_sblocks=read_dlg.sblock;

    CString dlgText;

    UpdateData(TRUE);

    GetDlgItem(IDC_NAND_READ)->GetWindowText(dlgText);
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
        //GetDlgItem(IDC_NAND_READ)->EnableWindow(FALSE);
        m_progress.SetPos(0);
    }
}

void CNANDDlg:: Erase()
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
    //    AfxMessageBox("Erase unsuccessfully!! Please check device");
    GetDlgItem(IDC_NAND_ERASEALL)->EnableWindow(TRUE);
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

unsigned WINAPI CNANDDlg::Erase_proc(void* args)
{
    CNANDDlg* pThis = reinterpret_cast<CNANDDlg*>(args);
    pThis->Erase();
    return 0;
}

void CNANDDlg::OnBnClickedNandEraseall()
{
    CString dlgText;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
#if 1
    CEraseDlg erase_dlg;
    //read_dlg.SizeName.Format(_T("Blocks(1 block is 0x%05x bytes)"),NAND_SIZE);
    erase_dlg.SizeName.Format(_T("blocks"));
    erase_dlg.type=1; /* 0 => SPI, 1 =>NAND, 2=>MMC */
    erase_dlg.EraseInfo.Format(_T("1 block is %s bytes"),mainWnd->Nand_size);
    if(erase_dlg.DoModal()==IDCANCEL) return;

    m_blocks=erase_dlg.block;
    m_sblocks=erase_dlg.sblock;
    m_erase_flag=erase_dlg.m_erase_type;
#else
    if(::MessageBox(this->m_hWnd,_T("Do you confirm this operation ?"),_T("Nu Writer"),MB_OKCANCEL|MB_ICONWARNING)==IDCANCEL)
        return;
#endif
    UpdateData(TRUE);

    GetDlgItem(IDC_NAND_ERASEALL)->GetWindowText(dlgText);

    if(dlgText.CompareNoCase(_T("Erase"))==0) {

        UpdateData(TRUE);

        unsigned Thread1;
        HANDLE hThread;

        m_progress.SetPos(0);
        m_progress.SetBkColor(COLOR_ERASE);
        hThread=(HANDLE)_beginthreadex(NULL,0,Erase_proc,(void*)this,0,&Thread1);
        CloseHandle(hThread);
    } else {
        //m_vcom.Close();
        SetEvent(m_ExitEvent);
        GetDlgItem(IDC_NAND_ERASEALL)->EnableWindow(FALSE);
        m_progress.SetPos(0);
    }
}

void CNANDDlg::OnBnClickedNandBrowse()
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
        _path=mainWnd->m_inifile.GetValue(_T("NAND"),_T("PATH"));
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
    this->GetDlgItem(IDC_NAND_IMAGENAME_A)->SetWindowText(m_imagename);
    //UpdateData(FALSE); //shanchun modified 20121203
    //PostMessage(WM_UPDATE_MESSAGE,0,0);



    CString filepath=m_filename.Left(m_filename.GetLength()-dlg.GetFileName().GetLength()-1);

    mainWnd->m_inifile.SetValue(_T("NAND"),_T("PATH"),filepath);
    mainWnd->m_inifile.WriteFile();

}

void CNANDDlg::OnBnClickedNandEncE()
{
    //if(m_type==EXECUTE)
    //{
    if(m_nand_enc_check.GetCheck()==TRUE) {
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
    //}else
//    {
//        AfxMessageBox(_T("Only image type equal Execute"));
//        m_nand_enc_check.SetCheck(FALSE);
//    }
}

BOOL CNANDDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    //TmpOffset
    UpdateData(TRUE);
    if(m_type==UBOOT) {
        if(TmpOffsetFlag==0) {
            TmpOffsetFlag=1;
            GetDlgItem(IDC_NAND_FLASHOFFSET_A)->GetWindowText(TmpOffset);
            GetDlgItem(IDC_NAND_FLASHOFFSET_A)->SetWindowText(_T("0"));
            GetDlgItem(IDC_NAND_FLASHOFFSET_A)->EnableWindow(FALSE);
            GetDlgItem(IDC_NAND_EXECADDR_A)->EnableWindow(TRUE);
            GetDlgItem(IDC_NAND_ENC_E)->EnableWindow(TRUE);
            if(m_nand_enc_check.GetCheck()==(int)true)
                GetDlgItem(IDC_NAND_COMBO_ENCRYPT)->EnableWindow(TRUE);
            else
                GetDlgItem(IDC_NAND_COMBO_ENCRYPT)->EnableWindow(FALSE);

        }

    } else {

        if(TmpOffsetFlag==1) {
            TmpOffsetFlag=0;
            GetDlgItem(IDC_NAND_FLASHOFFSET_A)->SetWindowText(TmpOffset);
            GetDlgItem(IDC_NAND_FLASHOFFSET_A)->EnableWindow(TRUE);
        }


        if(m_type==PACK) {
            GetDlgItem(IDC_NAND_EXECADDR_A)->EnableWindow(FALSE);
            GetDlgItem(IDC_NAND_FLASHOFFSET_A)->EnableWindow(FALSE);
            GetDlgItem(IDC_NAND_ENC_E)->EnableWindow(FALSE);
            GetDlgItem(IDC_NAND_COMBO_ENCRYPT)->EnableWindow(FALSE);
        } else {
            GetDlgItem(IDC_NAND_EXECADDR_A)->EnableWindow(FALSE);
            GetDlgItem(IDC_NAND_FLASHOFFSET_A)->EnableWindow(TRUE);
            GetDlgItem(IDC_NAND_ENC_E)->EnableWindow(TRUE);
            if(m_nand_enc_check.GetCheck()==(int)true)
                GetDlgItem(IDC_NAND_COMBO_ENCRYPT)->EnableWindow(TRUE);
            else
                GetDlgItem(IDC_NAND_COMBO_ENCRYPT)->EnableWindow(FALSE);
        }
    }


    return CDialog::OnCommand(wParam, lParam);
}

LRESULT CNANDDlg::ShowStatus( WPARAM  pos, LPARAM message)
{
    m_progress.SetPos((int)pos);
    return true;
}


BOOL CNANDDlg::InitFile(int flag)
{

    CString tmp;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    if(!mainWnd->m_inifile.ReadFile()) return false;
    switch(flag) {
    case 0:
        tmp=mainWnd->m_inifile.GetValue(_T("NAND"),_T("PATH"));
        m_filename=tmp;

        tmp=m_filename.Mid(m_filename.ReverseFind('\\')+1, m_filename.GetLength());
        if(tmp.ReverseFind('.')>0)
            m_imagename=tmp.Mid(0,tmp.ReverseFind('.'));
        else
            m_imagename=tmp;
        if(m_imagename.GetLength()>16)
            m_imagename = m_imagename.Mid(0,15);
        GetDlgItem(IDC_NAND_IMAGENAME_A)->SetWindowText(m_imagename);

        tmp=mainWnd->m_inifile.GetValue(_T("NAND"),_T("TYPE"));
        ((CButton *)GetDlgItem(IDC_NAND_TYPE_A))->SetCheck(FALSE);
        switch(_wtoi(tmp)) {
        case 0:
            ((CButton *)GetDlgItem(IDC_NAND_TYPE_A))->SetCheck(TRUE);
            break;
        case 1:
            ((CButton *)GetDlgItem(IDC_NAND_TYPE_A2))->SetCheck(TRUE);
            break;
        case 2:
            ((CButton *)GetDlgItem(IDC_NAND_TYPE_A4))->SetCheck(TRUE);
            break;
        case 3:
            ((CButton *)GetDlgItem(IDC_NAND_TYPE_A3))->SetCheck(TRUE);
            break;
        case 4:
            ((CButton *)GetDlgItem(IDC_NAND_TYPE_A5))->SetCheck(TRUE);
            break;
        }

        tmp=mainWnd->m_inifile.GetValue(_T("NAND"),_T("EXECADDR"));
        GetDlgItem(IDC_NAND_EXECADDR_A)->SetWindowText(tmp);
        m_execaddr=tmp;
        tmp=mainWnd->m_inifile.GetValue(_T("NAND"),_T("OFFSET"));
        GetDlgItem(IDC_NAND_FLASHOFFSET_A)->SetWindowText(tmp);

        tmp=mainWnd->m_inifile.GetValue(_T("NAND"),_T("ENC_EN"));
        if(_wtoi(tmp)==1) {
            tmp=mainWnd->m_inifile.GetValue(_T("NAND"),_T("ENC_SEL"));
            m_nand_enc_check.SetCheck(TRUE);
            OnBnClickedNandEncE();
            int flg=_wtoi(tmp);
            if(flg>=0 && flg<m_combo_encrypt.GetCount()) {
                m_combo_encrypt.SetCurSel(flg);
            }
        }
        //m_startblock=tmp;
        break;
    case 1:
        mainWnd->m_inifile.SetValue(_T("NAND"),_T("PATH"),m_filename);
        tmp.Format(_T("%d"),m_type);
        mainWnd->m_inifile.SetValue(_T("NAND"),_T("TYPE"),tmp);
        mainWnd->m_inifile.SetValue(_T("NAND"),_T("EXECADDR"),m_execaddr);
        mainWnd->m_inifile.SetValue(_T("NAND"),_T("OFFSET"),m_startblock);

        tmp.Format(_T("%d"),m_nand_enc_check.GetCheck());
        mainWnd->m_inifile.SetValue(_T("NAND"),_T("ENC_EN"),tmp);

        tmp.Format(_T("%d"),m_combo_encrypt.GetCurSel());
        mainWnd->m_inifile.SetValue(_T("NAND"),_T("ENC_SEL"),tmp);

        mainWnd->m_inifile.WriteFile();
        break;
    default:
        break;
    }

    if(mainWnd->ChipWriteWithOOB==1) {
        ((CButton *)GetDlgItem(IDC_NAND_TYPE_A))->SetCheck(FALSE);
        ((CButton *)GetDlgItem(IDC_NAND_TYPE_A2))->SetCheck(FALSE);
        ((CButton *)GetDlgItem(IDC_NAND_TYPE_A4))->SetCheck(FALSE);
        ((CButton *)GetDlgItem(IDC_NAND_TYPE_A3))->SetCheck(FALSE);
        ((CButton *)GetDlgItem(IDC_NAND_TYPE_A5))->SetCheck(FALSE);
        ((CButton *)GetDlgItem(IDC_NAND_TYPE_A))->SetCheck(TRUE);

        ((CButton *)GetDlgItem(IDC_NAND_TYPE_A))->EnableWindow(FALSE);
        ((CButton *)GetDlgItem(IDC_NAND_TYPE_A2))->EnableWindow(FALSE);
        ((CButton *)GetDlgItem(IDC_NAND_TYPE_A4))->EnableWindow(FALSE);
        ((CButton *)GetDlgItem(IDC_NAND_TYPE_A3))->EnableWindow(FALSE);
        ((CButton *)GetDlgItem(IDC_NAND_TYPE_A5))->EnableWindow(FALSE);
    }
    return true;
}

void CNANDDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);

    // TODO: Add your message handler code here
    m_imagelist.DeleteAllItems();
    if(InitFlag==0) {
        InitFlag=1;
        InitFile(0);

    }

    //Alignment :
    //UpdateData (TRUE);
}

void CNANDDlg::OnBnClickedNandUsrconfig()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
	CNandInfoDlg nandinfo_dlg;
	
	if(m_nandflash_check.GetCheck()==TRUE)
	{
	    nandinfo_dlg.DoModal();
	}

	mainWnd->GetDlgItem(IDC_RECONNECT)->EnableWindow(FALSE);
	mainWnd->OneDeviceInfo(0);// Update nand parameters
}
