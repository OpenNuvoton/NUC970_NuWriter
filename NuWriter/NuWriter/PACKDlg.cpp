// PACKDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NuWriter.h"
#include "NuWriterDlg.h"
#include "PACKDlg.h"
#include "PackTab1.h"

// CPACKDlg dialog
#define PACK_PAR 0
#define PACK_MTP 1


#define PACK_Mode(val)          (((val)&0xF   )>> 0)
#define PACK_Option(val)        (((val)&0xF0  )>> 4)
#define PACK_Encrypt(val)       (((val)&0xF00 )>> 8)
#define PACK_Enable(val)        (((val)&0xF000)>>12)

#define Enc_PACK_Mode(val)      (((val)&0xF)<< 0)
#define Enc_PACK_Option(val)    (((val)&0xF)<< 4)
#define Enc_PACK_Encrypt(val)   (((val)&0xF)<< 8)
#define Enc_PACK_Enable(val)    (((val)&0xF)<<12)


IMPLEMENT_DYNAMIC(CPACKDlg, CDialog)

CPACKDlg::CPACKDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CPACKDlg::IDD, pParent)
{
    TmpOffsetFlag=0;
    InitFlag=0;
    modifyflag=0;
}

CPACKDlg::~CPACKDlg()
{
}

void CPACKDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_PACK_ADD, m_add);
    DDX_Control(pDX, IDC_PACK_MODIFY, m_modify);
    DDX_Control(pDX, IDC_PACK_DELETE, m_delete);
    DDX_Control(pDX, IDC_PACK_OUTPUT, m_output);
    DDX_Control(pDX, IDC_PACK_DOWNPROGRESS, m_progress);
    DDX_Control(pDX, IDC_PACK_IMAGELIST, m_imagelist);
    DDX_Control(pDX, IDC_PACK_TABCONTROL, m_pack_tabcontrol);
    DDX_Control(pDX, IDC_PACK_STATUS, m_status);
}


BEGIN_MESSAGE_MAP(CPACKDlg, CDialog)
    ON_BN_CLICKED(IDC_PACK_ADD, &CPACKDlg::OnBnClickedPackAdd)
    ON_BN_CLICKED(IDC_PACK_MODIFY, &CPACKDlg::OnBnClickedPackModify)
    ON_BN_CLICKED(IDC_PACK_DELETE, &CPACKDlg::OnBnClickedPackDelete)
    ON_BN_CLICKED(IDC_PACK_OUTPUT, &CPACKDlg::OnBnClickedPackOutput)
    ON_WM_SHOWWINDOW()
    ON_NOTIFY(NM_DBLCLK, IDC_PACK_IMAGELIST, &CPACKDlg::OnNMDblclkPackImagelist)
    ON_NOTIFY(TCN_SELCHANGE, IDC_PACK_TABCONTROL, &CPACKDlg::OnTcnSelchangePackTabcontrol)
END_MESSAGE_MAP()


// CPACKDlg message handlers
BOOL CPACKDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    m_imagelist.SetHeadings(_T("Name, 150 ; Type,50 ; Encrypt,100 ; Start Address(Hex),110 ; End Address(Hex),110 ; Exec Address(Hex),110"));
    m_imagelist.SetGridLines(TRUE);

    COLORREF col = RGB(0xFF, 0x00, 0xFF);
    m_add.setBitmapId(IDB_ADD, col);
    m_add.setGradient(true);
    m_modify.setBitmapId(IDB_MODIFY, col);
    m_modify.setGradient(true);
    m_delete.setBitmapId(IDB_DELETE, col);
    m_delete.setGradient(true);
    m_output.setBitmapId(IDB_OUTPUT, col);
    m_output.setGradient(true);
    //m_browse.setBitmapId(IDB_BROWSE, col);
    //m_browse.setGradient(true);

    m_packtab1.Create(CPackTab1::IDD, GetDlgItem(IDC_PACK_TABCONTROL));
    m_packtab2.Create(CPackTab2::IDD, GetDlgItem(IDC_PACK_TABCONTROL));

    m_pack_tabcontrol.InsertItem(0, _T("Parameter"));
    m_pack_tabcontrol.InsertItem(1, _T("MTP"));

    SwapTab();
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CPACKDlg::SwapTab(void)
{
    CRect rTab, rItem;
    m_pack_tabcontrol.GetItemRect(0, &rItem);
    m_pack_tabcontrol.GetClientRect(&rTab);
    int x = rItem.left;
    int y = rItem.bottom + 1;
    int cx = rTab.right - rItem.left - 3;
    int cy = rTab.bottom - y -2;
    int tab = m_pack_tabcontrol.GetCurSel();
    m_packtab1.SetWindowPos(NULL, x, y, cx, cy, SWP_HIDEWINDOW);
    m_packtab2.SetWindowPos(NULL, x, y, cx, cy, SWP_HIDEWINDOW);

    switch(tab) {
    case 0:
        m_packtab1.SetWindowPos(NULL, x, y, cx, cy, SWP_SHOWWINDOW);
        break;
    case 1:
        m_packtab2.SetWindowPos(NULL, x, y, cx, cy, SWP_SHOWWINDOW);
        break;
    }

}

void CPACKDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);


    // TODO: Add your message handler code here
    if(InitFlag==0) {
        InitFlag=1;
        InitFile(0);
    }
    //UpdateData (TRUE);
}

extern unsigned char * DDR2Buf(char *buf,int buflen,int *ddrlen);

void CPACKDlg::OnBnClickedPackAdd()
{

    if(m_pack_tabcontrol.GetCurSel()==PACK_PAR) {
        m_packtab1.UpdateData(TRUE);

        // TODO: Add your control notification handler code here
        CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
        //szFileName.Format(_T("%s%s"),filename,ext);
        //sList.push_back(szFileName); // 將完整路徑加到 sList 裡頭
        if(m_packtab1.m_imagename.IsEmpty()) {
            AfxMessageBox(_T("Please input image file"));
            return;
        }

        if((m_packtab1.m_type!=UBOOT)&&((m_packtab1.m_execaddr.IsEmpty())||(m_packtab1.m_startblock.IsEmpty()) )) {

            AfxMessageBox(_T("Please input image information"));
            return;
        }


        if((m_packtab1.m_type==IMAGE)&&(m_packtab1.m_pack_enc_check.GetCheck()==TRUE)) {

            AfxMessageBox(_T("Write Image file can not use encrypt"));
            return;
        }

        if((m_packtab1.m_type==ENV)&&(m_packtab1.m_pack_enc_check.GetCheck()==TRUE)) {

            AfxMessageBox(_T("Write Enviroment can not use encrypt"));
            return;
        }

        ImageName.push_back(m_packtab1.m_filename);


        CString enc;
        ImageType.push_back(m_packtab1.m_type);
        if(m_packtab1.m_pack_enc_check.GetCheck()) {
            CString szDirForKEY,KeyName;
            mainWnd->GetExeDir(szDirForKEY);
            szDirForKEY=szDirForKEY+_T("key_cfg\\");
            m_packtab1.m_combo_encrypt.GetLBText(m_packtab1.m_combo_encrypt.GetCurSel(),KeyName);
            szDirForKEY=szDirForKEY+KeyName;
            enc=KeyName;
            ImageEnc.push_back(szDirForKEY);
        } else {
            ImageEnc.push_back(_T(""));
            enc=_T("");
        }

        ImageExec.push_back(m_packtab1.m_execaddr);
        ImageStartblock.push_back(m_packtab1.m_startblock);

        CString flagstr;
        switch(m_packtab1.m_type) {
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
        case IMAGE :
            flagstr.Format(_T("FS"));
            break;
        }
        CString len;
        {
            FILE* rfp;
            int total;
            int startblock;
            rfp=_wfopen(m_packtab1.m_filename,_T("rb"));
            fseek(rfp,0,SEEK_END);
            _stscanf_s(m_packtab1.m_startblock,_T("%x"),&startblock);
            total=ftell(rfp)+startblock;
            if(m_packtab1.m_type==UBOOT) {
                int ddrlen;
                UCHAR * ddrbuf;
                //ddrbuf=DDR2Buf(mainWnd->DDRBuf,mainWnd->DDRLen,&ddrlen);
                ddrbuf=DDR2Buf(mainWnd->ShareDDRBuf,mainWnd->DDRLen,&ddrlen);
                total+=(ddrlen+16);
            }

            len.Format(_T("%x"),total);
            fclose(rfp);
        }

        switch(m_packtab1.m_type) {
        case DATA:
        case ENV:
            m_imagelist.InsertItem(m_imagelist.GetItemCount(),m_packtab1.m_imagename,flagstr,enc,m_packtab1.m_startblock,len,_T(""));
            break;
        case UBOOT:
            m_imagelist.InsertItem(m_imagelist.GetItemCount(),m_packtab1.m_imagename,flagstr,enc,m_packtab1.m_startblock,len,m_packtab1.m_execaddr);
            break;
        }
        //m_imagelist.InsertItem(m_imagelist.GetItemCount(),m_packtab1.m_imagename,flagstr,enc,m_packtab1.m_startblock,len,m_packtab1.m_execaddr);
    } else if(m_pack_tabcontrol.GetCurSel()==PACK_MTP) {
        m_packtab2.UpdateData(TRUE);
        CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);


        vector<CString>::iterator itemName;
        for(itemName=ImageName.begin(); itemName!=ImageName.end(); itemName++) {
            if((*itemName).Compare(_T("MTPKEY"))==0) {
                AfxMessageBox(_T("Only one  MTPKEY"));
                return ;
            }

        }


        ImageName.push_back(_T("MTPKEY"));
        ImageType.push_back(PMTP);

        CString enc;
        CString option;
        if(m_packtab2.m_mtp_option==0) {
            CString szDirForKEY,KeyName;
            mainWnd->GetExeDir(szDirForKEY);
            szDirForKEY=szDirForKEY+_T("key_cfg\\");
            m_packtab2.m_combo_encrypt.GetLBText(m_packtab2.m_combo_encrypt.GetCurSel(),KeyName);
            szDirForKEY=szDirForKEY+KeyName;
            enc=KeyName;
            option=_T("AES");
            ImageEnc.push_back(szDirForKEY);
        } else {
            CString temp,KeyName;
            temp=m_packtab2.m_shafilename;
            if(temp.ReverseFind('.')>0)
                temp=temp.Mid(0,temp.ReverseFind('.'));
            else
                temp=temp;


            KeyName = temp.Mid(temp.ReverseFind('\\')+1,temp.GetLength());
            ImageEnc.push_back(m_packtab2.m_shafilename);
            enc=KeyName;
            option=_T("SHA");
        }


        CString mtp_val;
        int val=0;
        int enable;
        enable=((CButton*)m_packtab2.GetDlgItem(IDC_PACK_CHECK_LOCK_ENABLE))->GetCheck();
        val= Enc_PACK_Mode(m_packtab2.m_mtp_mode)		|
             Enc_PACK_Option(m_packtab2.m_mtp_option)	|
             Enc_PACK_Encrypt(m_packtab2.m_mtp_encrypt) |
             Enc_PACK_Enable(enable);


        mtp_val.Format(_T("%x"),val);
        ImageExec.push_back(mtp_val);
        ImageStartblock.push_back(_T(""));
        m_imagelist.InsertItem( m_imagelist.GetItemCount(),_T("MTPKEY"),option,enc,_T(""),_T(""),_T("") );
    }



    //vector<CString>::iterator item;
    //for(item=ImageExec.begin();item!=ImageExec.end();item++)
    //{
    //	AfxMessageBox(*item);
    //}
}

void CPACKDlg::OnBnClickedPackModify()
{

    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    vector<CString>::iterator itemName;
    vector<int>::iterator itemType;
    vector<CString>::iterator itemEnc;
    vector<CString>::iterator itemExec;
    vector<CString>::iterator itemStartblock;

    if(m_pack_tabcontrol.GetCurSel()==PACK_PAR) {
        m_packtab1.UpdateData(TRUE);

        if(m_packtab1.m_imagename.IsEmpty()) {
            AfxMessageBox(_T("Please input image file"));
            return;
        }

        if((m_packtab1.m_type!=UBOOT)&&((m_packtab1.m_execaddr.IsEmpty())||(m_packtab1.m_startblock.IsEmpty()) )) {
            CString tmp;
            tmp.Format(_T("%d 0x%08x 0x%08x"),m_packtab1.m_type,m_packtab1.m_execaddr,m_packtab1.m_startblock);
            AfxMessageBox(tmp);

            //AfxMessageBox(_T("Please input image information"));
            return;
        }

        if((m_packtab1.m_type==IMAGE)&&(m_packtab1.m_pack_enc_check.GetCheck()==TRUE)) {

            AfxMessageBox(_T("Write Image file can not used encrypt"));
            return;
        }


        int i;
        int modify_idx;
        int imagelen=m_imagelist.GetItemCount();

        for(i=0; i<imagelen; i++) {
            if(m_imagelist.IsItemSelected(i)==TRUE)
                break;
        }
        if(i==imagelen || imagelen==0) {
            AfxMessageBox(_T("Please select image item to modify first"));
            return;
        }
        modify_idx=i;
        //------------------------------------------------------------------------------
        itemName=ImageName.begin()+modify_idx;
        itemType=ImageType.begin()+modify_idx;
        itemEnc=ImageEnc.begin()+modify_idx;
        itemExec=ImageExec.begin()+modify_idx;
        itemStartblock=ImageStartblock.begin()+modify_idx;
        *itemName=m_packtab1.m_filename;
        *itemExec=m_packtab1.m_execaddr;
        if(m_packtab1.m_pack_enc_check.GetCheck()) {
            CString szDirForKEY,KeyName;
            mainWnd->GetExeDir(szDirForKEY);
            szDirForKEY=szDirForKEY+_T("key_cfg\\");
            m_packtab1.m_combo_encrypt.GetLBText(m_packtab1.m_combo_encrypt.GetCurSel(),KeyName);
            szDirForKEY=szDirForKEY+KeyName;
            *itemEnc=szDirForKEY;
        } else {
            *itemEnc=_T("");
        }
        *itemType=m_packtab1.m_type;
        *itemExec=m_packtab1.m_execaddr;
        *itemStartblock=m_packtab1.m_startblock;
    } else if(m_pack_tabcontrol.GetCurSel()==PACK_MTP) {
        m_packtab2.UpdateData(TRUE);

        vector<CString>::iterator itemName;
        for(itemName=ImageName.begin(); itemName!=ImageName.end(); itemName++) {
            if((*itemName).Compare(_T("MTPKEY"))==0) {
                AfxMessageBox(_T("Only one  MTPKEY"));
                return ;
            }

        }

        int i;
        int modify_idx;
        int imagelen=m_imagelist.GetItemCount();

        for(i=0; i<imagelen; i++) {
            if(m_imagelist.IsItemSelected(i)==TRUE)
                break;
        }
        if(i==imagelen || imagelen==0) {
            AfxMessageBox(_T("Please select image item to modify first"));
            return;
        }
        modify_idx=i;
        //------------------------------------------------------------------------------
        itemName=ImageName.begin()+modify_idx;
        itemType=ImageType.begin()+modify_idx;
        itemEnc=ImageEnc.begin()+modify_idx;
        itemExec=ImageExec.begin()+modify_idx;
        itemStartblock=ImageStartblock.begin()+modify_idx;

        *itemName=_T("MTPKEY");
        *itemType=PMTP;


        CString enc;
        CString szDirForKEY,KeyName;
        mainWnd->GetExeDir(szDirForKEY);
        szDirForKEY=szDirForKEY+_T("key_cfg\\");
        m_packtab2.m_combo_encrypt.GetLBText(m_packtab2.m_combo_encrypt.GetCurSel(),KeyName);
        szDirForKEY=szDirForKEY+KeyName;
        enc=KeyName;
        *itemEnc=szDirForKEY;

        CString mtp_val;
        int val=0;
        int enable;
        enable=((CButton*)m_packtab2.GetDlgItem(IDC_PACK_CHECK_LOCK_ENABLE))->GetCheck();
        val= Enc_PACK_Mode(m_packtab2.m_mtp_mode)		|
             Enc_PACK_Option(m_packtab2.m_mtp_option)	|
             Enc_PACK_Encrypt(m_packtab2.m_mtp_encrypt) |
             Enc_PACK_Enable(enable);
        mtp_val.Format(_T("%x"),val);
        *itemExec=mtp_val;

        *itemStartblock=_T("");
    }


    //------------------------------------------------------------------------------
    int imagelen=m_imagelist.GetItemCount();
    CString flagstr,enc;
    m_imagelist.DeleteAllItems();
    for(int i=0; i<imagelen; i++) {
        itemName=ImageName.begin()+i;
        itemType=ImageType.begin()+i;
        itemEnc=ImageEnc.begin()+i;
        itemExec=ImageExec.begin()+i;
        itemStartblock=ImageStartblock.begin()+i;

        CString filename,tmp;
        tmp=(*itemName).Mid((*itemName).ReverseFind('\\')+1, (*itemName).GetLength());
        if(tmp.ReverseFind('.')>0)
            filename=tmp.Mid(0,tmp.ReverseFind('.'));
        else
            filename=tmp;
        if(filename.GetLength()>16)
            filename = filename.Mid(0,15);

        switch(*itemType) {
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
        case IMAGE :
            flagstr.Format(_T("FS"));
            break;
        }

        if((*itemEnc).IsEmpty())
            enc.Format(_T(""));
        else
            enc=(*itemEnc).Mid((*itemEnc).ReverseFind('\\')+1, (*itemEnc).GetLength());


        CString len;
        if(*itemType!=PMTP) {
            FILE* rfp;
            int total;
            int startblock;
            rfp=_wfopen(*itemName,_T("rb"));
            fseek(rfp,0,SEEK_END);
            _stscanf_s(*itemStartblock,_T("%x"),&startblock);
            total=ftell(rfp)+startblock;
            len.Format(_T("%x"),total);
            fclose(rfp);
        }
        if(*itemType!=PMTP) {
            switch(*itemType) {
            case DATA:
            case ENV:
                m_imagelist.InsertItem(m_imagelist.GetItemCount(),filename,flagstr,enc,*itemStartblock,len,_T(""));
                break;
            case UBOOT:
                m_imagelist.InsertItem(m_imagelist.GetItemCount(),filename,flagstr,enc,*itemStartblock,len,*itemExec);
                break;
            }
            //m_imagelist.InsertItem(m_imagelist.GetItemCount(),filename,flagstr,enc,*itemStartblock,len,*itemExec);
        } else
            m_imagelist.InsertItem(m_imagelist.GetItemCount(),filename,flagstr,enc,_T(""),len,_T(""));
    }

}

void CPACKDlg::OnBnClickedPackDelete()
{
#if 1
    m_packtab1.UpdateData(TRUE);

    vector<CString>::iterator itemName;
    vector<int>::iterator itemType;
    vector<CString>::iterator itemEnc;
    vector<CString>::iterator itemExec;
    vector<CString>::iterator itemStartblock;
    int modify_idx;
    int i;
    int imagelen=m_imagelist.GetItemCount();

    for(i=0; i<imagelen; i++) {
        if(m_imagelist.IsItemSelected(i)==TRUE)
            break;
    }
    if(i==imagelen || imagelen==0) {
        AfxMessageBox(_T("Please select image item to modify first"));
        return;
    }
    modify_idx=i;
    ImageName.erase(ImageName.begin() + modify_idx);
    ImageType.erase(ImageType.begin() + modify_idx);
    ImageEnc.erase(ImageEnc.begin() + modify_idx);
    ImageExec.erase(ImageExec.begin() + modify_idx);
    ImageStartblock.erase(ImageStartblock.begin() + modify_idx);


    //--- re-show image list ---
    m_imagelist.DeleteAllItems();

    i=0;
    for(itemName=ImageName.begin(); itemName!=ImageName.end(); itemName++) {
        itemType=(ImageType.begin()+i);
        itemEnc=(ImageEnc.begin()+i);
        itemExec=(ImageExec.begin()+i);
        itemStartblock=(ImageStartblock.begin()+i);
        i++;

        CString flagstr,enc;
        switch(*itemType) {
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
        case IMAGE :
            flagstr.Format(_T("FS"));
            break;
        }

        if((*itemEnc).IsEmpty()) {
            enc.Format(_T(""));
        } else {
            CString tmp;
            tmp=(*itemEnc).Mid((*itemEnc).ReverseFind('\\')+1, (*itemEnc).GetLength());
            if(tmp.ReverseFind('.')>0)
                enc=tmp.Mid(0,tmp.ReverseFind('.'));
            else
                enc=tmp;
            if(enc.GetLength()>16)
                enc = enc.Mid(0,15);
        }


        CString filename,tmp;
        tmp=(*itemName).Mid((*itemName).ReverseFind('\\')+1, (*itemName).GetLength());
        if(tmp.ReverseFind('.')>0)
            filename=tmp.Mid(0,tmp.ReverseFind('.'));
        else
            filename=tmp;
        if(filename.GetLength()>16)
            filename = filename.Mid(0,15);

        CString len;
        if(*itemType!=PMTP) {
            FILE* rfp;
            int total;
            int startblock;
            rfp=_wfopen(*itemName,_T("rb"));
            fseek(rfp,0,SEEK_END);
            _stscanf_s(*itemStartblock,_T("%x"),&startblock);
            total=ftell(rfp)+startblock;
            len.Format(_T("%x"),total);
            fclose(rfp);
        }
        if(*itemType!=PMTP) {
            if(*itemType==UBOOT)
                m_imagelist.InsertItem(m_imagelist.GetItemCount(),filename,flagstr,enc,*itemStartblock,len,*itemExec);
            else
                m_imagelist.InsertItem(m_imagelist.GetItemCount(),filename,flagstr,enc,*itemStartblock,len,_T(""));
        } else
            m_imagelist.InsertItem(m_imagelist.GetItemCount(),filename,flagstr,enc,_T(""),len,_T(""));
    }
#else
    ImageName.clear();
    ImageType.clear();
    ImageExec.clear();
    ImageStartblock.clear();
    m_imagelist.DeleteAllItems();
#endif
}

extern unsigned char * DDR2Buf(char *buf,int buflen,int *ddrlen);
int CPACKDlg::Output()
{
    int i;
    //m_packtab1.UpdateData(TRUE);
    PostMessage(WM_PACK_PROGRESS,(LPARAM)0,0);
    m_progress.SetBkColor(COLOR_DOWNLOAD);
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    FILE* wfp,*rfp;
    //-----------------------------------
    wfp=_wfopen(m_filename2,_T("w+b"));
    if(!wfp) {
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }
    //-----------------------------------
    int storageType=0;
    int storageSize=64*1024;
    vector<CString>::iterator itemName;
    vector<int>::iterator itemType;
    vector<CString>::iterator itemExec;
    vector<CString>::iterator itemStartblock;
    vector<CString>::iterator itemEnc;

    int imagelen=m_imagelist.GetItemCount();

    //-----------------------------------------
    for(i=0; i<imagelen; i++) {
        itemType=(ImageType.begin()+i);
        if(*itemType==UBOOT) break;
    }
    if(i==imagelen) {
        AfxMessageBox(_T("Can't find uBoot image, please input uboot for startup"));
    }
    //-----------------------------------------

    int total=0;
    for(i=0; i<imagelen; i++) {
        itemType=(ImageType.begin()+i);
        if(*itemType!=PMTP) {
            itemName=(ImageName.begin()+i);
            rfp=_wfopen(*itemName,_T("rb"));
            fseek(rfp,0,SEEK_END);
            total+=((ftell(rfp)+storageSize-1)/storageSize)*storageSize;
            fclose(rfp);
        } else
            total+=(256/8);
    }

    PACK_HEAD pack_head;
    memset((char *)&pack_head,0xff,sizeof(pack_head));
    pack_head.actionFlag=PACK_ACTION;
    pack_head.fileLength=total;
    pack_head.num=imagelen;

    //write  pack_head
    fwrite((char *)&pack_head,sizeof(PACK_HEAD),1,wfp);

    PACK_CHILD_HEAD child;
    for(i=0; i<imagelen; i++) {
        itemName=(ImageName.begin()+i);
        itemType=(ImageType.begin()+i);
        itemExec=(ImageExec.begin()+i);
        itemEnc =(ImageEnc.begin() +i);
        if(*itemType!=PMTP) {
            itemStartblock=(ImageStartblock.begin()+i);
            rfp=_wfopen(*itemName,_T("rb"));
            fseek(rfp,0,SEEK_END);
            unsigned int len= ftell(rfp);
            fseek(rfp,0,SEEK_SET);

            char *pBuffer=NULL;
            char magic[4]= {' ','T','V','N'};
            switch(*itemType) {
            case UBOOT: {
                int ddrlen;
                unsigned char *ddrbuf;

                ddrbuf=DDR2Buf(mainWnd->ShareDDRBuf,mainWnd->DDRLen,&ddrlen);
                memset((char *)&child,0xff,sizeof(PACK_CHILD_HEAD));

                char *encbuf=NULL;
                if(!((CString)(*itemEnc)).IsEmpty() ) {
                    fclose(rfp);
                    encbuf=mainWnd->AES_Encrpt(*itemEnc,*itemName,&len);
                    rfp=_wfopen(*itemName,_T("rb"));
                }

                //write  pack_child_head
                child.filelen=len+ddrlen+16;
                child.startaddr=0;
                child.imagetype=UBOOT;
                fwrite((char *)&child,1,sizeof(PACK_CHILD_HEAD),wfp);

                //write uboot head
                unsigned int tmp;
                fwrite((char *)magic,1,sizeof(magic),wfp);
                _stscanf_s(*itemExec,_T("%x"),&tmp);
                fwrite((char *)&tmp,1,4,wfp);
                fwrite((char *)&len,1,4,wfp);
                tmp=0xffffffff;
                fwrite((char *)&tmp,1,4,wfp);

                //write DDR
                fwrite(ddrbuf,1,ddrlen,wfp);

                pBuffer=(char *)malloc(len);

                // Modified DDR2Buf to solved memory leak
                //free(ddrbuf);

                if( ((CString)(*itemEnc)).IsEmpty() ) {
                    fread(pBuffer,1,len,rfp);
                    fwrite((char *)pBuffer,1,len,wfp);
                } else {
                    fwrite((char *)encbuf,1,len,wfp);
                }
            }
            break;
            case ENV  :
                memset((char *)&child,0xff,sizeof(PACK_CHILD_HEAD));
                child.filelen=0x10000;
                child.imagetype=ENV;
                pBuffer=(char *)malloc(0x10000);
                memset(pBuffer,0x0,0x10000);
                _stscanf_s(*itemStartblock,_T("%x"),&child.startaddr);
                //-----------------------------------------------
                fwrite((char *)&child,sizeof(PACK_CHILD_HEAD),1,wfp);
#if 0
                fread(pBuffer+4,1,len,rfp);
#else
                {
                    char line[256];
                    char* ptr=(char *)(pBuffer+4);
                    while (1) {
                        if (fgets(line,256, rfp) == NULL) break;
                        if(line[strlen(line)-2]==0x0D || line[strlen(line)-1]==0x0A) {
                            strncpy(ptr,line,strlen(line)-1);
                            ptr[strlen(line)-2]=0x0;
                            ptr+=(strlen(line)-1);
                        } else {
                            strncpy(ptr,line,strlen(line));
                            ptr+=(strlen(line));
                        }
                    }

                }
#endif
                *(unsigned int *)pBuffer=mainWnd->CalculateCRC32((unsigned char *)(pBuffer+4),0x10000-4);
                fwrite((char *)pBuffer,1,0x10000,wfp);
                break;
            case DATA : {
                memset((char *)&child,0xff,sizeof(PACK_CHILD_HEAD));

                char *encbuf=NULL;
                if(!((CString)(*itemEnc)).IsEmpty() ) {
                    fclose(rfp);
                    encbuf=mainWnd->AES_Encrpt(*itemEnc,*itemName,&len);
                    rfp=_wfopen(*itemName,_T("rb"));
                }

                child.filelen=len;
                child.imagetype=DATA;
                pBuffer=(char *)malloc(child.filelen);
                _stscanf_s(*itemStartblock,_T("%x"),&child.startaddr);
                //-----------------------------------------------
                fwrite((char *)&child,sizeof(PACK_CHILD_HEAD),1,wfp);
                if( ((CString)(*itemEnc)).IsEmpty() ) {
                    fread(pBuffer,1,len,rfp);
                    fwrite((char *)pBuffer,1,len,wfp);
                } else {
                    fwrite((char *)encbuf,1,len,wfp);
                }
            }
            break;
            case IMAGE:
                memset((char *)&child,0xff,sizeof(PACK_CHILD_HEAD));
                child.filelen=len;
                child.imagetype=IMAGE;
                pBuffer=(char *)malloc(child.filelen);
                _stscanf_s(*itemStartblock,_T("%x"),&child.startaddr);
                //-----------------------------------------------
                fwrite((char *)&child,sizeof(PACK_CHILD_HEAD),1,wfp);
                fread(pBuffer,1,len,rfp);
                fwrite((char *)pBuffer,1,len,wfp);
                break;
            }
            fclose(rfp);
            if(pBuffer!=NULL) free(pBuffer);
        } else { /* MTPKEY */
            int val,len;
            char *buf;
            memset((char *)&child,0xff,sizeof(PACK_CHILD_HEAD));

            _stscanf_s(*itemExec,_T("%x"),&val);

            if(PACK_Option(val)==0) {
                buf=mainWnd->Get_OTP_KEY(*itemEnc,&len);
                child.filelen=len;
            } else {
                char data[32];
                buf=(char *)mainWnd->CalculateSHA(*itemEnc);
                for(int i1=0; i1<8; i1++) {
                    for(int j1=0; j1<4; j1++)
                        data[i1*4+j1]=buf[i1*4+(3-j1)];
                }
                len=32;
                child.filelen=len;
                memcpy((char *)buf,(char *)data,32);
            }
            child.imagetype=PMTP;
            child.reserve[0]=0xFFFFFFFF;

            child.startaddr=val;

            fwrite((char *)&child,sizeof(PACK_CHILD_HEAD),1,wfp);
            fwrite((char *)buf,sizeof(unsigned char),len,wfp);

        }
    }
    fclose(wfp);

    AfxMessageBox(_T("Ouput finish "));


    return TRUE;
}


unsigned WINAPI CPACKDlg:: Output_proc(void* args)
{
    CPACKDlg* pThis = reinterpret_cast<CPACKDlg*>(args);
    pThis->Output();
    return 0;
}

void CPACKDlg::OnBnClickedPackOutput()
{
    InitFile(1);
    if(m_imagelist.GetItemCount()==0) {
        AfxMessageBox(_T("Please add image item to output binary file\n"));
        return;
    }

    if( ((CButton*)m_packtab2.GetDlgItem(IDC_PACK_CHECK_LOCK_ENABLE))->GetCheck() )
        if(::MessageBox(this->m_hWnd,_T("You have select lock mtp key, once mtp key is locked can not be program any more."),_T("Nu Writer"),MB_OKCANCEL|MB_ICONWARNING)==IDCANCEL)
            return;

    //CAddFileDialog dlg(TRUE,NULL,NULL,OFN_CREATEPROMPT | OFN_HIDEREADONLY ,_T("Bin,Img Files  (*.bin)|*.bin|All Files (*.*)|*.*||"));
    CAddFileDialog dlg(FALSE,NULL,NULL,OFN_CREATEPROMPT | OFN_HIDEREADONLY ,_T("Bin Files (*.bin)|*.bin|All Files (*.*)|*.*||"));
    //CAddFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,_T("Bin Files (*.bin)|*.bin|All Files (*.*)|*.*||"));

    dlg.m_ofn.lpstrTitle=_T("Choose burning file...");
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    if(!mainWnd->m_inifile.ReadFile())
        dlg.m_ofn.lpstrInitialDir=_T("c:");
    else {
        CString _path;
        _path=mainWnd->m_inifile.GetValue(_T("PACK"),_T("SAVEPATH"));
        if(_path.IsEmpty())
            dlg.m_ofn.lpstrInitialDir=_T("c:");
        else
            dlg.m_ofn.lpstrInitialDir=_path;
    }

    BOOLEAN ret=dlg.DoModal();

    if(ret==IDCANCEL) {
        return;
    }

    m_filename2=dlg.GetPathName();
    //AfxMessageBox(m_filename2);

    unsigned Thread1;
    HANDLE hThread;
    hThread=(HANDLE)_beginthreadex(NULL,0,Output_proc,(void*)this,0,&Thread1);

}

BOOL CPACKDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
#if 0
    //TmpOffset
    UpdateData(TRUE);
    if(m_packtab1.m_type==UBOOT) {
        if(TmpOffsetFlag==0) {
            TmpOffsetFlag=1;
            m_packtab1.GetDlgItem(IDC_PACK_FLASHOFFSET_A)->GetWindowText(TmpOffset);
            m_packtab1.GetDlgItem(IDC_PACK_FLASHOFFSET_A)->SetWindowText(_T("0"));
            m_packtab1.GetDlgItem(IDC_PACK_FLASHOFFSET_A)->EnableWindow(FALSE);
            m_packtab1.GetDlgItem(IDC_PACK_EXECADDR_A)->EnableWindow(TRUE);
        }

    } else {

        if(TmpOffsetFlag==1) {
            TmpOffsetFlag=0;
            m_packtab1.GetDlgItem(IDC_PACK_FLASHOFFSET_A)->SetWindowText(TmpOffset);
            m_packtab1.GetDlgItem(IDC_PACK_FLASHOFFSET_A)->EnableWindow(TRUE);
        }


        if(m_packtab1.m_type==PACK) {
            m_packtab1.GetDlgItem(IDC_PACK_EXECADDR_A)->EnableWindow(FALSE);
            m_packtab1.GetDlgItem(IDC_PACK_FLASHOFFSET_A)->EnableWindow(FALSE);
        } else {
            m_packtab1.GetDlgItem(IDC_PACK_EXECADDR_A)->EnableWindow(FALSE);
            m_packtab1.GetDlgItem(IDC_PACK_FLASHOFFSET_A)->EnableWindow(TRUE);
        }
    }
#endif
    return CDialog::OnCommand(wParam, lParam);

}

BOOL CPACKDlg::FileExist(CString strFileName)
{
    CFileFind fFind;
    return fFind.FindFile(strFileName);
}

BOOL CPACKDlg::InitFile(int flag)
{

    CString tName,tType,tEnc,tExec,tStartblock;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    vector<CString>::iterator itemName;
    vector<int>::iterator itemType;
    vector<CString>::iterator itemEnc;
    vector<CString>::iterator itemExec;
    vector<CString>::iterator itemStartblock;

    if(!mainWnd->m_inifile.ReadFile()) return false;
    switch(flag) {
    case 0: {
        int imagelen=_wtoi( mainWnd->m_inifile.GetValue(_T("PACK"),_T("IMAGENUM")) );
        CString NameTemp;
        int counter=0;
        for(int i=0; i<imagelen; i++) {
            tName.Format(_T("NAME%02d"),i);
            NameTemp = mainWnd->m_inifile.GetValue(_T("PACK"),tName);


            if(FileExist(NameTemp)==NULL && NameTemp.Compare(_T("MTPKEY"))!=0 )
                continue;

            ImageName.push_back( NameTemp );

            tType.Format(_T("TYPE%02d"),i);
            ImageType.push_back( _wtoi(mainWnd->m_inifile.GetValue(_T("PACK"),tType)) );

            tEnc.Format(_T("ENC%02d"),i);
            ImageEnc.push_back( mainWnd->m_inifile.GetValue(_T("PACK"),tEnc) );

            tExec.Format(_T("EXEC%02d"),i);
            ImageExec.push_back( mainWnd->m_inifile.GetValue(_T("PACK"),tExec) );


            tStartblock.Format(_T("STARTBLOCK%02d"),i);
            ImageStartblock.push_back( mainWnd->m_inifile.GetValue(_T("PACK"),tStartblock) );


            itemName=(ImageName.begin()+i);
            itemType=(ImageType.begin()+i);
            itemEnc=(ImageEnc.begin()+i);
            itemExec=(ImageExec.begin()+i);
            itemStartblock=(ImageStartblock.begin()+i);


            CString flagstr,enc;
            switch(*itemType) {
            case DATA	:
                flagstr.Format(_T("DATA"));
                break;
            case ENV	:
                flagstr.Format(_T("ENV"));
                break;
            case UBOOT	:
                flagstr.Format(_T("uBOOT"));
                break;
            case PACK	:
                flagstr.Format(_T("Pack"));
                break;
            case IMAGE	:
                flagstr.Format(_T("FS"));
                break;
            }

            if((*itemEnc).IsEmpty())
                enc.Format(_T(""));
            else
                enc=(*itemEnc).Mid((*itemEnc).ReverseFind('\\')+1, (*itemEnc).GetLength());




            CString filename,tmp;
            tmp=(*itemName).Mid((*itemName).ReverseFind('\\')+1, (*itemName).GetLength());
            if(tmp.ReverseFind('.')>0)
                filename=tmp.Mid(0,tmp.ReverseFind('.'));
            else
                filename=tmp;
            if(filename.GetLength()>16)
                filename = filename.Mid(0,15);

            CString len;
            if(*itemType!=PMTP) {
                FILE* rfp;
                int total;
                int startblock;
                rfp=_wfopen(*itemName,_T("rb"));
                fseek(rfp,0,SEEK_END);
                _stscanf_s(*itemStartblock,_T("%x"),&startblock);
                total=ftell(rfp)+startblock;

                if(*itemType==UBOOT) {
                    int ddrlen;
                    UCHAR * ddrbuf;
                    //ddrbuf=DDR2Buf(mainWnd->DDRBuf,mainWnd->DDRLen,&ddrlen);
                    ddrbuf=DDR2Buf(mainWnd->ShareDDRBuf,mainWnd->DDRLen,&ddrlen);
                    total+=(ddrlen+16);
                }

                len.Format(_T("%x"),total);
                fclose(rfp);
            }

            if(*itemType!=PMTP) {
                switch(*itemType) {
                case DATA:
                case ENV:
                    m_imagelist.InsertItem(m_imagelist.GetItemCount(),filename,flagstr,enc,*itemStartblock,len,_T(""));
                    break;
                case UBOOT:
                    m_imagelist.InsertItem(m_imagelist.GetItemCount(),filename,flagstr,enc,*itemStartblock,len,*itemExec);
                    break;
                }
                //m_imagelist.InsertItem(m_imagelist.GetItemCount(),filename,flagstr,enc,*itemStartblock,len,*itemExec);
            } else {
                unsigned int exec;
                _stscanf_s(*itemExec,_T("%x"),&exec);
                if(PACK_Option(exec)==0)
                    m_imagelist.InsertItem(m_imagelist.GetItemCount(),filename,_T("AES"),enc,_T(""),len,_T(""));
                else
                    m_imagelist.InsertItem(m_imagelist.GetItemCount(),filename,_T("SHA"),enc,_T(""),len,_T(""));
                //m_imagelist.InsertItem(m_imagelist.GetItemCount(),m_imagename,flagstr,enc,m_startblock,m_execaddr);
                counter++;
            }
        }
    }
    break;

    case 1: {
        int imagelen=m_imagelist.GetItemCount();
        CString tmp;
        tmp.Format(_T("%d\n"),imagelen);
        mainWnd->m_inifile.SetValue(_T("PACK"),_T("IMAGENUM"),tmp);

        for(int i=0; i<imagelen; i++) {

            itemName=(ImageName.begin()+i);
            itemType=(ImageType.begin()+i);
            itemEnc=(ImageEnc.begin()+i);
            itemExec=(ImageExec.begin()+i);
            itemStartblock=(ImageStartblock.begin()+i);

            tName.Format(_T("NAME%02d"),i);
            mainWnd->m_inifile.SetValue(_T("PACK"),tName,*itemName);

            tType.Format(_T("TYPE%02d"),i);
            tmp.Format(_T("%d"),*itemType);
            mainWnd->m_inifile.SetValue(_T("PACK"),tType,tmp);

            tEnc.Format(_T("ENC%02d"),i);
            mainWnd->m_inifile.SetValue(_T("PACK"),tEnc,*itemEnc);

            tExec.Format(_T("EXEC%02d"),i);
            mainWnd->m_inifile.SetValue(_T("PACK"),tExec,*itemExec);

            tStartblock.Format(_T("STARTBLOCK%02d"),i);
            mainWnd->m_inifile.SetValue(_T("PACK"),tStartblock,*itemStartblock);
        }
        mainWnd->m_inifile.WriteFile();
    }
    break;
    }
    return true;
}
void CPACKDlg::OnNMDblclkPackImagelist(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE *>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

    vector<CString>::iterator itemName;
    vector<int>::iterator itemType;
    vector<CString>::iterator itemEnc;
    vector<CString>::iterator itemExec;
    vector<CString>::iterator itemStartblock;

    int i;
    int modify_idx;
    int imagelen=m_imagelist.GetItemCount();

    for(i=0; i<imagelen; i++) {
        if(m_imagelist.IsItemSelected(i)==TRUE)
            break;
    }
    if(i==imagelen || imagelen==0) {
        AfxMessageBox(_T("Please select image item to modify first"));
        return;
    }
    modify_idx=i;

    CString tmp;
    //-----------------------------------------------------------------------------
    itemName=ImageName.begin()+modify_idx;
    m_packtab1.m_filename=(*itemName);
    tmp=m_packtab1.m_filename.Mid(m_packtab1.m_filename.ReverseFind('\\')+1, m_packtab1.m_filename.GetLength());
    if(tmp.ReverseFind('.')>0)
        m_packtab1.m_imagename=tmp.Mid(0,tmp.ReverseFind('.'));
    else
        m_packtab1.m_imagename=tmp;
    if(m_packtab1.m_imagename.GetLength()>16)
        m_packtab1.m_imagename = m_packtab1.m_imagename.Mid(0,15);

    if(m_packtab1.m_imagename.Compare(_T("MTPKEY"))!=0) {
        m_pack_tabcontrol.SetCurSel(PACK_PAR);
        SwapTab();
        m_packtab1.GetDlgItem(IDC_PACK_IMAGENAME_A)->SetWindowText(m_packtab1.m_imagename);
        //-----------------------------------------------------------------------------
        itemType=ImageType.begin()+modify_idx;
        ((CButton *)m_packtab1.GetDlgItem(IDC_PACK_TYPE_A))->SetCheck(FALSE);
        ((CButton *)m_packtab1.GetDlgItem(IDC_PACK_TYPE_A2))->SetCheck(FALSE);
        ((CButton *)m_packtab1.GetDlgItem(IDC_PACK_TYPE_A4))->SetCheck(FALSE);
        ((CButton *)m_packtab1.GetDlgItem(IDC_PACK_TYPE_A3))->SetCheck(FALSE);
        ((CButton *)m_packtab1.GetDlgItem(IDC_PACK_TYPE_A5))->SetCheck(FALSE);
        switch((*itemType)) {
        case 0:
            ((CButton *)m_packtab1.GetDlgItem(IDC_PACK_TYPE_A ))->SetCheck(TRUE);
            break;
        case 1:
            ((CButton *)m_packtab1.GetDlgItem(IDC_PACK_TYPE_A2))->SetCheck(TRUE);
            break;
        case 2:
            ((CButton *)m_packtab1.GetDlgItem(IDC_PACK_TYPE_A4))->SetCheck(TRUE);
            break;
        case 3:
            ((CButton *)m_packtab1.GetDlgItem(IDC_PACK_TYPE_A3))->SetCheck(TRUE);
            break;
        case 4:
            ((CButton *)m_packtab1.GetDlgItem(IDC_PACK_TYPE_A5))->SetCheck(TRUE);
            break;
        }
        //-----------------------------------------------------------------------------
        itemExec=ImageExec.begin()+modify_idx;
        m_packtab1.GetDlgItem(IDC_PACK_EXECADDR_A)->SetWindowText(*itemExec);
        //-----------------------------------------------------------------------------
        itemStartblock=ImageStartblock.begin()+modify_idx;
        m_packtab1.GetDlgItem(IDC_PACK_FLASHOFFSET_A)->SetWindowText(*itemStartblock);
        //------------------------------------------------------------------------------
        itemEnc=ImageEnc.begin()+modify_idx;
        if(!(*itemEnc).IsEmpty()) {
            CString enc;
            m_packtab1.m_pack_enc_check.SetCheck(1);
            m_packtab1.OnBnClickedPackEncE();
            enc=(*itemEnc).Mid((*itemEnc).ReverseFind('\\')+1, (*itemEnc).GetLength());
            m_packtab1.m_combo_encrypt.SetWindowText(enc);
        } else {
            m_packtab1.m_pack_enc_check.SetCheck(0);
            m_packtab1.OnBnClickedPackEncE();
        }
        //------------------------------------------------------------------------------
    } else { //MTP Key
        int mtp_val,enable;
        m_pack_tabcontrol.SetCurSel(PACK_MTP);
        SwapTab();
        itemExec=ImageExec.begin()+modify_idx;
        _stscanf_s(*itemExec,_T("%x"),&mtp_val);

        m_packtab2.m_mtp_mode=PACK_Mode(mtp_val);
        ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_M0))->SetCheck(FALSE);
        ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_M1))->SetCheck(FALSE);
        ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_M2))->SetCheck(FALSE);
        ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_M3))->SetCheck(FALSE);
        ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_M11))->SetCheck(FALSE);
        ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_M12))->SetCheck(FALSE);
        switch(m_packtab2.m_mtp_mode) {
        case 0: /*  USB */
            ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_M0))->SetCheck(TRUE);
            break;
        case 1: /*  eMMC/USB without SHA  */
            ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_M1))->SetCheck(TRUE);
            ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_M11))->SetCheck(TRUE);
            break;
        case 2: /*  NAND  */
            ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_M2))->SetCheck(TRUE);
            break;
        case 3: /*  SPI Flash/USB with SHA  */
            ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_M3))->SetCheck(TRUE);
            ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_M12))->SetCheck(TRUE);
            break;
        }

        /* AES, SHA */
        m_packtab2.m_mtp_option=PACK_Option(mtp_val);
        ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_A0))->SetCheck(FALSE);
        ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_A1))->SetCheck(FALSE);
        ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_A2))->SetCheck(FALSE);
        GetDlgItem(IDC_MTP_COMBO_ENCRYPT)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_MTP_SHAFILENAME)->ShowWindow(SW_HIDE);
        switch(m_packtab2.m_mtp_option) {
        case 0:
            ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_A0))->SetCheck(TRUE);
            GetDlgItem(IDC_MTP_COMBO_ENCRYPT)->ShowWindow(SW_SHOW);
            break;
        case 1:
            ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_A1))->SetCheck(TRUE);
            GetDlgItem(IDC_MTP_SHAFILENAME)->ShowWindow(SW_SHOW);
            break;
        case 2:
            ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_A2))->SetCheck(TRUE);
            break;
        }

        /* ENABLE, DISABLE */
        m_packtab2.m_mtp_encrypt=PACK_Encrypt(mtp_val);
        ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_ENABLE ))->SetCheck(FALSE);
        ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_DISABLE))->SetCheck(FALSE);
        switch(m_packtab2.m_mtp_encrypt) {
        case 0:
            ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_ENABLE ))->SetCheck(TRUE);
            break;
        case 1:
            ((CButton *)m_packtab2.GetDlgItem(IDC_PACK_MTP_DISABLE))->SetCheck(TRUE);
            break;
        }


        enable=PACK_Enable(mtp_val);
        ((CButton*)m_packtab2.GetDlgItem(IDC_PACK_CHECK_LOCK_ENABLE))->SetCheck(enable);
        m_packtab2.UpdateData(TRUE);
        m_packtab2.OnCommand(IDC_PACK_MTP_M1,NULL);

#if 0
#define Enc_PACK_Mode(val)       (((val)&0xF)<< 0)
#define Enc_PACK_Option(val)     (((val)&0xF)<< 4)
#define Enc_PACK_Encrypt(val)    (((val)&0xF)<< 8)
#define Enc_PACK_Enable(val)     (((val)&0xF)<<16)
        DDX_Radio(pDX, IDC_PACK_MTP_A0, m_mtp_option);
        DDX_Radio(pDX, IDC_PACK_MTP_M0, m_mtp_mode);
        DDX_Radio(pDX, IDC_PACK_MTP_ENABLE, m_mtp_encrypt);
#endif

    }

}

void CPACKDlg::OnTcnSelchangePackTabcontrol(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    *pResult = 0;
    SwapTab();
}


