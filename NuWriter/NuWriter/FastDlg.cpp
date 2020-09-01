// FastDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "NuWriter.h"
#include "FastDlg.h"
#include "afxdialogex.h"
#include "NuWriterDlg.h"
#include "NandInfoDlg.h"

// FastDlg 對話方塊
#define TYPE_NAND    0
#define TYPE_SPI     1
#define TYPE_EMMC    2

#define MMC_TIMEOUT_SEC 3600//200
#define SPI_TIMEOUT_SEC 360
#define NAND_TIMEOUT_SEC 120

IMPLEMENT_DYNAMIC(FastDlg, CDialog)

FastDlg::FastDlg(CWnd* pParent /*=NULL*/)
    : CDialog(FastDlg::IDD, pParent)
{
    InitFlag=0;
    m_type = 0;
    InitFlag=0;
    m_fhead_nand=(NORBOOT_NAND_HEAD *)malloc(sizeof(NORBOOT_NAND_HEAD));
    m_fhead_spi=(NORBOOT_NAND_HEAD *)malloc(sizeof(NORBOOT_NAND_HEAD));
    m_fhead_emmc=(NORBOOT_MMC_HEAD *)malloc(sizeof(NORBOOT_MMC_HEAD));
    //TRACE(_T("FastDlg malloc %d, %d, %d \n\n"), sizeof(NORBOOT_NAND_HEAD), sizeof(NORBOOT_NAND_HEAD), sizeof(NORBOOT_MMC_HEAD));
}

FastDlg::~FastDlg()
{
    free(m_fhead_nand);
    free(m_fhead_spi);
    free(m_fhead_emmc);
    //TRACE(_T("~FastDlg  free %d, %d, %d \n\n"), sizeof(NORBOOT_NAND_HEAD), sizeof(NORBOOT_NAND_HEAD), sizeof(NORBOOT_MMC_HEAD));
}

void FastDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FAST_IMAGENAME, m_imagename);
	DDX_Radio(pDX, IDC_RADIO_FAST_NAND, m_type);
	DDX_Control(pDX, IDC_COMBO_FAST_ID, m_FastDeviceID);
	DDX_Control(pDX, IDC_FAST_NAND_USRCONFIG, m_nandflashInfo_check);
}


BEGIN_MESSAGE_MAP(FastDlg, CDialog)
    ON_BN_CLICKED(IDC_BTN_FAST_START, &FastDlg::OnBnClickedBtnFastStart)
    ON_MESSAGE(WM_FAST_PROGRESS1,ShowStatusD1)
    ON_MESSAGE(WM_FAST_PROGRESS2,ShowStatusD2)
    ON_MESSAGE(WM_FAST_PROGRESS3,ShowStatusD3)
    ON_MESSAGE(WM_FAST_PROGRESS4,ShowStatusD4)
    ON_MESSAGE(WM_FAST_PROGRESS5,ShowStatusD5)
    ON_MESSAGE(WM_FAST_PROGRESS6,ShowStatusD6)
    ON_MESSAGE(WM_FAST_PROGRESS7,ShowStatusD7)
    ON_MESSAGE(WM_FAST_PROGRESS8,ShowStatusD8)
    ON_MESSAGE(WM_FAST_CLOSETHREAD,ThreadClose)
    ON_MESSAGE(WM_FAST_TIMEOUTCTL,TimeoutCtl)
    ON_BN_CLICKED(IDC_FAST_BROWSE, &FastDlg::OnBnClickedFastBrowse)
    ON_WM_SHOWWINDOW()
    ON_BN_CLICKED(IDC_BUTTON1, &FastDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &FastDlg::OnBnClickedButton2)
    ON_CBN_SELCHANGE(IDC_COMBO_FAST_ID, &FastDlg::OnCbnSelchangeComboFastId)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_RSTBTN, &FastDlg::OnBnClickedRstbtn)
    ON_BN_CLICKED(IDC_ALLRSTBTN, &FastDlg::OnBnClickedAllrstbtn)
    ON_BN_CLICKED(IDC_RADIO_FAST_NAND, &FastDlg::OnBnClickedRadioFastNand)
    ON_BN_CLICKED(IDC_RADIO_FAST_SPI, &FastDlg::OnBnClickedRadioFastSpi)
    ON_BN_CLICKED(IDC_RADIO_FAST_eMMC, &FastDlg::OnBnClickedRadioFastemmc)
	ON_BN_CLICKED(IDC_FAST_NAND_USRCONFIG, &FastDlg::OnBnClickedFastNandUsrconfig)
END_MESSAGE_MAP()


// FastDlg 訊息處理常式


BOOL FastDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此加入額外的初始化
    int i, DevNum;
    CString word;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    //InitFile(0);

    iId_Array[0]=IDC_FAST_PROGRESS1;
    iId_Array[1]=IDC_FAST_PROGRESS2;
    iId_Array[2]=IDC_FAST_PROGRESS3;
    iId_Array[3]=IDC_FAST_PROGRESS4;
    iId_Array[4]=IDC_FAST_PROGRESS5;
    iId_Array[5]=IDC_FAST_PROGRESS6;
    iId_Array[6]=IDC_FAST_PROGRESS7;
    iId_Array[7]=IDC_FAST_PROGRESS8;

    mainWnd->m_burntext.GetWindowText(word);

    if(!word.Compare(_T(" Disconnected"))) {
        return FALSE;
    }

    DevNum = NucUsb.UsbDevice_Detect();

    for(i=0; i< DevNum; i++) {
        ((CProgressCtrl *)GetDlgItem(iId_Array[i]))->ShowWindow(SW_SHOW);
        ((CStatic *)GetDlgItem((IDC_STATIC_FAST_MSG1+i)))->ShowWindow(SW_SHOW);
        ((CStatic *)GetDlgItem((IDC_STATIC_FAST_DEV1+i)))->ShowWindow(SW_SHOW);
        ((CProgressCtrl *)GetDlgItem(iId_Array[i]))->SetRange(0,100);
        ((CProgressCtrl *)GetDlgItem(iId_Array[i]))->SetBkColor(COLOR_DOWNLOAD);
        word.Format(_T("%d"),i);
        m_FastDeviceID.AddString(word);
    }


    m_FastDeviceID.SetCurSel(0);
	if(m_type == TYPE_NAND)	
	    GetDlgItem(IDC_FAST_NAND_USRCONFIG)->EnableWindow(TRUE);
	else
	    GetDlgItem(IDC_FAST_NAND_USRCONFIG)->EnableWindow(FALSE);
    UpdateData(FALSE);
    return TRUE;  // return TRUE unless you set the focus to a control

}

void FastDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);

    // TODO: Add your message handler code here
    if(InitFlag==0) {
        InitFlag=1;
        InitFile(0);

    }
}

LRESULT FastDlg::ShowStatusD1( WPARAM  pos, LPARAM message)
{
    ((CProgressCtrl *)GetDlgItem(iId_Array[0]))->SetPos((int)pos);
    return true;
}

LRESULT FastDlg::ShowStatusD2( WPARAM  pos, LPARAM message)
{
    ((CProgressCtrl *)GetDlgItem(iId_Array[1]))->SetPos((int)pos);
    return true;
}
LRESULT FastDlg::ShowStatusD3( WPARAM  pos, LPARAM message)
{
    ((CProgressCtrl *)GetDlgItem(iId_Array[2]))->SetPos((int)pos);
    return true;
}
LRESULT FastDlg::ShowStatusD4( WPARAM  pos, LPARAM message)
{
    ((CProgressCtrl *)GetDlgItem(iId_Array[3]))->SetPos((int)pos);
    return true;
}

LRESULT FastDlg::ShowStatusD5( WPARAM  pos, LPARAM message)
{
    ((CProgressCtrl *)GetDlgItem(iId_Array[4]))->SetPos((int)pos);
    return true;
}
LRESULT FastDlg::ShowStatusD6( WPARAM  pos, LPARAM message)
{
    ((CProgressCtrl *)GetDlgItem(iId_Array[5]))->SetPos((int)pos);
    return true;
}

LRESULT FastDlg::ShowStatusD7( WPARAM  pos, LPARAM message)
{
    ((CProgressCtrl *)GetDlgItem(iId_Array[6]))->SetPos((int)pos);
    return true;
}
LRESULT FastDlg::ShowStatusD8( WPARAM  pos, LPARAM message)
{
    ((CProgressCtrl *)GetDlgItem(iId_Array[7]))->SetPos((int)pos);
    return true;
}



LRESULT FastDlg::TimeoutCtl( WPARAM  id, LPARAM message)
{
    INT m_threadid=(INT)id;
    INT iflag=(INT)message;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    SetEvent(m_ExitEventBurn[m_threadid]);
    TRACE(_T("============== TimeoutCtl (%d)  %d\n"),id, mainWnd->g_iDeviceNum);
    return true;
}

LRESULT FastDlg::ThreadClose( WPARAM  id, LPARAM message)
{
    INT m_threadid=(INT)id;
    INT iflag=(INT)message;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    SetEvent(m_ExitEventBurn[m_threadid]);
    //TRACE(_T("============== ThreadClose (%d)  %d  %d\n"),id, mainWnd->g_iDeviceNum, m_Downloadcnt);
    CloseHandle(hThread[m_threadid]);
    m_Downloadcnt++;
    if(iflag) {
        m_Successcnt++;
    }

    if(m_Downloadcnt<mainWnd->g_iDeviceNum) {
        //TRACE(_T("(%d) aaaaaaaaaaaaaaaaaa  %d\n"), id, m_Downloadcnt);
        return true;
    } else {
        //GetDlgItem(IDC_BTN_FAST_START)->EnableWindow(TRUE);
        TRACE(_T("(%d) BBBBBBBBBBBBBBBBBBBBBBBBBBB  %d\n"), id, m_Downloadcnt);
        GetDlgItem(IDC_BTN_FAST_START)->SetWindowText(_T("Start"));
        GetDlgItem(IDC_BTN_FAST_START)->EnableWindow(FALSE);
        mainWnd->m_gtype.EnableWindow(TRUE);
        KillTimer(0);//停止編號0計時器
    }

    return true;
}

BOOL FastDlg::InitFile(int flag)
{
    CString tmp;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    if(!mainWnd->m_inifile.ReadFile()) return false;
    switch(flag) {
    case 0:
        tmp=mainWnd->m_inifile.GetValue(_T("Mass_Production"),_T("PATH"));
        m_filename=tmp;
        tmp=m_filename.Mid(m_filename.ReverseFind('\\')+1, m_filename.GetLength());
        if(tmp.ReverseFind('.')>0)
            m_imagename=tmp.Mid(0,tmp.ReverseFind('.'));
        else
            m_imagename=tmp;
        if(m_imagename.GetLength()>16)
            m_imagename = m_imagename.Mid(0,15);
        GetDlgItem(IDC_FAST_IMAGENAME)->SetWindowText(m_imagename);
        TRACE(_T("file name = %s,   %s\n"), tmp, m_imagename);
        tmp=mainWnd->m_inifile.GetValue(_T("Mass_Production"),_T("TYPE"));
        ((CButton *)GetDlgItem(IDC_RADIO_FAST_NAND))->SetCheck(FALSE);
        switch(_wtoi(tmp)) {
        case 0: {
            ((CButton *)GetDlgItem(IDC_RADIO_FAST_NAND))->SetCheck(TRUE);
			GetDlgItem(IDC_FAST_NAND_USRCONFIG)->EnableWindow(TRUE);
            timeoutsec = NAND_TIMEOUT_SEC;
			m_type = TYPE_NAND;
            break;
        }
        case 1: {
            ((CButton *)GetDlgItem(IDC_RADIO_FAST_SPI))->SetCheck(TRUE);
			GetDlgItem(IDC_FAST_NAND_USRCONFIG)->EnableWindow(FALSE);
            timeoutsec = SPI_TIMEOUT_SEC;
			m_type = TYPE_SPI;
            break;
        }
        case 2: {
            ((CButton *)GetDlgItem(IDC_RADIO_FAST_eMMC))->SetCheck(TRUE);
			GetDlgItem(IDC_FAST_NAND_USRCONFIG)->EnableWindow(FALSE);
            timeoutsec = MMC_TIMEOUT_SEC;
			m_type = TYPE_EMMC;
            break;
        }
        }
        TRACE(_T("InitFile(0) m_type = %s, timeoutsec = %d\n"), tmp, timeoutsec);
        break;
    case 1:
        mainWnd->m_inifile.SetValue(_T("Mass_Production"),_T("PATH"),m_filename);
        tmp.Format(_T("%d"),m_type);
        mainWnd->m_inifile.SetValue(_T("Mass_Production"),_T("TYPE"),tmp);
        mainWnd->m_inifile.WriteFile();
        switch (m_type) {
        case 0:
            timeoutsec = NAND_TIMEOUT_SEC;
			GetDlgItem(IDC_FAST_NAND_USRCONFIG)->EnableWindow(TRUE);
			m_type = TYPE_NAND;
            break;
        case 1:
            timeoutsec = SPI_TIMEOUT_SEC;
			GetDlgItem(IDC_FAST_NAND_USRCONFIG)->EnableWindow(FALSE);
			m_type = TYPE_SPI;
            break;
        case 2:
            timeoutsec = MMC_TIMEOUT_SEC;
			GetDlgItem(IDC_FAST_NAND_USRCONFIG)->EnableWindow(FALSE);
			m_type = TYPE_EMMC;
            break;
        }
        TRACE(_T("InitFile(1) m_type = %s, timeoutsec = %d\n"), tmp, timeoutsec);
        break;
    default:
        break;
    }

    return true;
}

BOOL FastDlg:: CheckImageSouce(CString& m_pathName)
{
    FILE* fp;
    unsigned int file_len;

    fp=_wfopen(m_pathName,_T("rb"));

    if(!fp) {
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }

    fseek(fp,0,SEEK_END);
    file_len=ftell(fp);
    fseek(fp,0,SEEK_SET);

    if(!file_len) {
        fclose(fp);
        AfxMessageBox(_T("File length is zero\n"));
        return FALSE;
    }

    unsigned int magic;
    fread((unsigned char *)&magic,4,1,fp);
    if(magic!=0x5) {
        fclose(fp);
        AfxMessageBox(_T("Pack Image Format Error\n"));
        return FALSE;
    }

    fclose(fp);
    return TRUE;
}

BOOL FastDlg:: FastErase(int id, int storagetype)
{
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    BOOLEAN ret;
    switch(storagetype) {
    case TYPE_SPI:
        ret=XUSB_FastSPIErase(id, mainWnd->m_portName);
        break;

    case TYPE_EMMC:
        ret=XUSB_FasteMMCErase(id, mainWnd->m_portName);
        break;

    case TYPE_NAND:
    default:
        ret=XUSB_FastNANDErase(id, mainWnd->m_portName);
        break;
    }

    if(!ret) {
        ((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetRange(0,0);
        return false;
    }

    return true;
}


BOOL FastDlg:: FastBurn(int id, int storagetype)
{
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    BOOLEAN ret;
    int len;

    //TRACE(_T("FastBurn (%d) %s\n"),id, m_filename);
    switch(storagetype) {
    case TYPE_SPI:
        ret=XUSB_FastSPIBurn(id,mainWnd->m_portName,m_filename,&len);
        break;

    case TYPE_EMMC:
        ret=XUSB_FasteMMCBurn(id,mainWnd->m_portName,m_filename,&len);
        break;

    case TYPE_NAND:
    default:
        ret=XUSB_FastNANDBurn(id,mainWnd->m_portName,m_filename,&len);
        break;
    }

    if(!ret) {
        ((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetRange(0,0);
        return false;
    }

    return ret;//true;
}

int FastDlg:: FastVerify(int id, int storagetype)
{
    int i=0;
    int ret;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    if(!m_filename.IsEmpty()) {
        switch(storagetype) {
        case TYPE_SPI:
            ret=XUSB_FastSPIVerify(id, mainWnd->m_portName,m_filename);
            break;

        case TYPE_EMMC:
            ret=XUSB_FasteMMCVerify(id, mainWnd->m_portName,m_filename);
            break;

        case TYPE_NAND:
        default:
            ret=XUSB_FastNANDVerify(id, mainWnd->m_portName,m_filename);
            break;
        }

        if(!ret) {
            ((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetRange(0,0);
            return false;
        }
    } else {
        ret = -1;
        //AfxMessageBox(_T("Please choose comparing file !"));
    }

    return ret;
}

unsigned WINAPI FastDlg:: Start_proc(void* args)
{
    FastDlg* pThis = reinterpret_cast<FastDlg*>(args);
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    if(pThis->m_DeviceId < mainWnd->g_iDeviceNum) {
        pThis->Start((pThis->m_DeviceId)++);
    }

    return 0;
}

void FastDlg::Start(int id)
{
    BOOLEAN ret, bSuccess = false, bResult;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    TRACE("FastDlg::Start thread (%d)\n",id);
    ResetEvent(m_ExitEventBurn[id]);
    retrycnt[id] = 0;

    while(timeoutflag[id] == false) {
        if (retrycnt[id] > 0) {
            /// Add Chip reset command and reconnection here.
            TRACE(_T("@@@@ ERROR RETRY(%d)  cnt=%d\n"), id, retrycnt[id]);
            bResult =NucUsb.NUC_ResetFW(id);
            Sleep(100);//for Mass production
            if(bResult == FALSE) {
                TRACE(_T("XXXX (%d) RESET NUC970 failed\n"), id);
                continue;
            }
            //else
            //    TRACE(_T("RESET(%d) NUC970 FW PASS\n"), id);

            Sleep(200);//for Mass production
            bResult = FWDownload(id);
            Sleep(200);//for Mass production
            if(bResult == FALSE) {
                TRACE(_T("XXXX (%d) Download FW failed\n"), id);
                continue;
            }
            //else
            //    TRACE(_T("Download (%d) FW PASS\n"), id);
        }

        retrycnt[id]++;
        //TRACE(_T("(%d) RETRY Count= %d, flag = %d\n"), id, retrycnt[id], timeoutflag[id]);
        ((CStatic *)GetDlgItem((IDC_STATIC_FAST_MSG1+id)))->SetWindowText(_T("Erase ..."));
		if(m_type != TYPE_SPI)
		{
            ret = FastErase(id, m_type);
            if (ret != 1) {
                continue;
            }
		}

		((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetRange(0,0);
		((CStatic *)GetDlgItem((IDC_STATIC_FAST_MSG1+id)))->SetWindowText(_T("Download ..."));
		ret = FastBurn(id, m_type);
        if (ret == ERR_PACK_FORMAT) { // Image is not pack format
			((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetRange(0,0);
            AfxMessageBox(_T("This file is not pack image\n"));
			goto _fail_stop;
        }
        else if (ret == ERR_DDRINI_DATACOM) { // DDR Init VS. pack image ini data compare error
			((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetRange(0,0);
            AfxMessageBox(_T("DDR Init select error\n"));
			goto _fail_stop;
        }		
		else if (ret != 1) {
            continue;
        }

        ((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetRange(0,0);
        ((CStatic *)GetDlgItem((IDC_STATIC_FAST_MSG1+id)))->SetWindowText(_T("Verify ..."));
        ret=FastVerify(id, m_type);
        if (ret == ERR_VERIFY_DATACOM) { // data compare error
			((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetRange(0,0);
            AfxMessageBox(_T("Verify error\n"));
			//break;
			goto _fail_stop;
        }
		else if (ret != 1) {
            continue;
        }

        if(ret==1) {
            ((CStatic *)GetDlgItem((IDC_STATIC_FAST_MSG1+id)))->SetWindowText(_T("Successfully"));
            bSuccess = true;
            timerctl[id] = false;
            TRACE(_T("OOO RETRY(%d) CNT= %d\n"), id, retrycnt[id]);
            break;
        }
    }

_fail_stop:
    if (bSuccess == FALSE) {
        ((CStatic *)GetDlgItem((IDC_STATIC_FAST_MSG1+id)))->SetWindowText(_T("Fail"));
    }

    PostMessage(WM_FAST_CLOSETHREAD,(LPARAM)id,1);

    return ;
}

void FastDlg::OnBnClickedBtnFastStart()
{
    // TODO: 在此加入控制項告知處理常式程式碼
    int i = 0, j = 0;
    CString dlgText, connectText;
    BOOL ret;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    InitFile(1);
    //TRACE(_T("timeoutsec =%d , m_type = %d\n"), timeoutsec, m_type);
    if(m_imagename.IsEmpty()) {
        AfxMessageBox(_T("Please input image file"));
        return;
    }

	if(m_type == TYPE_NAND)	
	    GetDlgItem(IDC_FAST_NAND_USRCONFIG)->EnableWindow(TRUE);
	else
	    GetDlgItem(IDC_FAST_NAND_USRCONFIG)->EnableWindow(FALSE);

    mainWnd->GetDlgItem(IDC_COMPORT)->GetWindowText(connectText);
    GetDlgItem(IDC_BTN_FAST_START)->GetWindowText(dlgText);
    m_DeviceId=0;
    if(connectText.CompareNoCase(_T(" Disconnected")) ==0) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
    } else {
        if(dlgText.CompareNoCase(_T("Start"))==0) {
            UpdateData(TRUE);
            if (mainWnd->g_iDeviceNum) {
                if(m_filename.IsEmpty()) {
                    AfxMessageBox(_T("Please choose image file !"));
                    return;
                }

                ret = CheckImageSouce(m_filename);
                if(!ret) {
                    return;
                }

                /*  ====== Erase, Burn and Verify ====== */
                m_Downloadcnt=0;
                m_Successcnt=0;
                for(i=0; i< mainWnd->g_iDeviceNum; i++) {
                    m_ExitEventBurn[i]=CreateEvent(NULL,TRUE,FALSE,NULL);
                }

                for(i=0; i< mainWnd->g_iDeviceNum; i++) {
                    hThread[i]=(HANDLE)_beginthreadex(NULL,0,Start_proc,(void*)this,0,&ThreadArray[i]);
                    timerctl[i] = true;
                    timeoutflag[i] = false;
                }
                timercnt = 0;
                SetTimer(0,1000,NULL);//啟動計時器編號0每一秒觸發一次
                GetDlgItem(IDC_BTN_FAST_START)->SetWindowText(_T("Abort"));
                mainWnd->m_gtype.EnableWindow(FALSE);
            } else {
                //AfxMessageBox(_T("Please reset device and press Re-connect button now !!!\n"));
            }

        } else {
            //TRACE("Abort burn. mainWnd->g_iDeviceNum = %d\n", mainWnd->g_iDeviceNum);
            for(j=0; j< mainWnd->g_iDeviceNum; j++) {
                SetEvent(m_ExitEventBurn[j]);
                ((CProgressCtrl *)GetDlgItem(iId_Array[j]))->SetRange(0,100);
                ((CProgressCtrl *)GetDlgItem(iId_Array[j]))->SetPos(0);
                ((CProgressCtrl *)GetDlgItem(iId_Array[j]))->SetBkColor(COLOR_DOWNLOAD);
                ((CStatic *)GetDlgItem((IDC_STATIC_FAST_MSG1+j)))->SetWindowText(_T("Fail"));
                timerctl[j] = false;
                timeoutflag[j] = true;
            }
            TRACE("Abort burn end\n");
            GetDlgItem(IDC_BTN_FAST_START)->EnableWindow(FALSE);
            //OnBnClickedAllrstbtn();
            GetDlgItem(IDC_BTN_FAST_START)->SetWindowText(_T("Start"));
            //KillTimer(0);//停止編號0計時器
            mainWnd->m_gtype.EnableWindow(TRUE);
            AfxMessageBox(_T("Please reset device and press Re-connect button now !!!\n"));
        }
    }
}

void FastDlg::OnBnClickedFastBrowse()
{
    // TODO: 在此加入控制項告知處理常式程式碼
    UpdateData(TRUE);
    UpdateData(FALSE);
    CString temp;

    CAddFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,_T("Bin Files (*.bin)|*.bin|All Files (*.*)|*.*||"));

    dlg.m_ofn.lpstrTitle=_T("Choose burning file...");
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    if(!mainWnd->m_inifile.ReadFile())
        dlg.m_ofn.lpstrInitialDir=_T("c:");
    else {
        CString _path;
        _path=mainWnd->m_inifile.GetValue(_T("Mass_Production"),_T("PATH"));
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
    this->GetDlgItem(IDC_FAST_IMAGENAME)->SetWindowText(m_imagename);

    CString filepath=m_filename.Left(m_filename.GetLength()-dlg.GetFileName().GetLength()-1);

    mainWnd->m_inifile.SetValue(_T("Mass_Production"),_T("PATH"),m_filename);
    mainWnd->m_inifile.WriteFile();
}


void FastDlg::OnBnClickedButton1()
{
    // TODO: 在此加入控制項告知處理常式程式碼
    int Num;
    char* lpBuffer;
    BOOL bResult;

    lpBuffer = new char[256];

    for(int i=0; i < 256; i++) {
        lpBuffer[i]= i;
    }
    //Num = GetDlgItemInt(IDC_USBEDIT1);
    Num = m_FastDeviceID.GetCurSel();
    //NucUsb.NUC_SetDeviceLEDOn(Num);
    bResult = NucUsb.NUC_BulkOutTest(Num,(UCHAR *)lpBuffer, 256);
    if(bResult == FALSE) {
        TRACE(_T("XXX OUT Data Failed. %d  %d  %d  %d\n"), lpBuffer[0], lpBuffer[1],lpBuffer[254], lpBuffer[255]);
    }

    delete []lpBuffer;
}


void FastDlg::OnBnClickedButton2()
{
    // TODO: 在此加入控制項告知處理常式程式碼
    int Num;
    //Num = GetDlgItemInt(IDC_USBEDIT1);
    Num = m_FastDeviceID.GetCurSel();
    NucUsb.NUC_SetDeviceLEDOff(Num);
}


void FastDlg::OnCbnSelchangeComboFastId()
{
    // TODO: 在此加入控制項告知處理常式程式碼
    int IdNum = 0;
    CString str;

    IdNum = m_FastDeviceID.GetCurSel();
    str.Format(_T("%d"),IdNum);
    //GetDlgItem(IDC_USBEDIT1)->SetWindowText(str);

    NucUsb.NUC_SetDeviceLEDOn(IdNum);
    NucUsb.NUC_SetDeviceLEDOff(IdNum);
    NucUsb.NUC_SetDeviceLEDOn(IdNum);
}


void FastDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    int i;

    timercnt++;
    if(timercnt % 10 == 0) {
        TRACE(_T("OnTimer timercnt =%d\n"), timercnt);
    }

    if(timercnt > timeoutsec) {
        TRACE(_T("OnTimer timercnt =%d\n"), timercnt);
        for(i=0; i< mainWnd->g_iDeviceNum; i++) {
            if(timerctl[i]) {
                ((CStatic *)GetDlgItem((IDC_STATIC_FAST_MSG1+i)))->SetWindowText(_T("Fail"));
                ((CProgressCtrl *)GetDlgItem(iId_Array[i]))->SetRange(0,0);
                PostMessage(WM_FAST_TIMEOUTCTL,(LPARAM)i,1);
                timeoutflag[i] = true;
            }
        }
        timercnt = 0;
        KillTimer(0);//停止編號0計時器
        GetDlgItem(IDC_BTN_FAST_START)->EnableWindow(TRUE);
        AfxMessageBox(_T("Time-out. Please reset device and Re-connect now !!!\n"));
    }
    CDialog::OnTimer(nIDEvent);
}


void FastDlg::OnBnClickedRstbtn()
{
    // TODO: 在此加入控制項告知處理常式程式碼
    int Num;
    BOOL bResult;

    Num = m_FastDeviceID.GetCurSel();
    bResult=NucUsb.EnableOneWinUsbDevice(Num);
    if(bResult == FALSE) {
        TRACE(_T("XXX (%d) USB Connect failed\n"), Num);
    }

    bResult =NucUsb.NUC_ResetFW(Num);
    Sleep(10);
    if(bResult == FALSE) {
        TRACE(_T("XXX RESET(%d) NUC970 failed\n"), Num);
    }

#if(1)
    Sleep(200);//for Mass production
    bResult = FWDownload(Num);
    Sleep(200);//for Mass production
    if(bResult == FALSE) {
        TRACE(_T("XXX Download(%d) FW failed\n"), Num);
    }
#endif
}


void FastDlg::OnBnClickedAllrstbtn()
{
    // TODO: 在此加入控制項告知處理常式程式碼
    int i;
    BOOL bResult;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    for(i=0; i< mainWnd->g_iDeviceNum; i++) {
        bResult =NucUsb.NUC_ResetFW(i);
        Sleep(20);
        if(bResult == FALSE) {
            TRACE(_T("XXX RESET(%d) NUC970 failed\n"), i);
        } else
            TRACE(_T("RESET(%d) NUC970 PASS\n"), i);
    }
}

BOOL FastDlg:: FWDownload(int id)
{
    int i=0;
    TCHAR path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    CString temp=path;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    BOOL bResult=1;

    temp = temp.Left(temp.ReverseFind('\\') + 1);
    CString filename=NULL;

    switch(mainWnd->DDRFileName.GetAt(8)) {
    case '5':
        filename.Format(_T("%sxusb.bin"),temp);
        break;
    case '6':
        filename.Format(_T("%sxusb64.bin"),temp);
        break;
    case '7':
        filename.Format(_T("%sxusb128.bin"),temp);
        break;
    default:
        filename.Format(_T("%sxusb16.bin"),temp);
        break;
    };

    bResult = XUSB(id, filename);

    if(!bResult) {
        NucUsb.CloseWinUsbDevice(id);
    }

    return bResult;
}

DWORD FWGetRamAddress(FILE* fp)
{
    BINHEADER head;
    UCHAR SIGNATURE[]= {'W','B',0x5A,0xA5};

    fread((CHAR*)&head,sizeof(head),1,fp);

    if(head.signature==*(DWORD*)SIGNATURE) {
        return head.address;
    } else
        return 0xFFFFFFFF;
}

BOOL FastDlg::DDRtoDevice(int id, char *buf,unsigned int len)
{
    BOOL bResult;
    char *pbuf;
    unsigned int scnt,rcnt,ack;
    AUTOTYPEHEAD head;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    head.address=mainWnd->DDRAddress;  // 0x10
    head.filelen=len;	// NUC972DF62Y.ini length = 368

    //TRACE(_T("(%d)  0x%x,  %d\n"), id, head.address, head.filelen);
    bResult=NucUsb.NUC_WritePipe(id,(unsigned char*)&head,sizeof(AUTOTYPEHEAD));
    if(bResult==FALSE) {
        TRACE(_T("XXX (%d) FastDlg::DDRtoDevice error.  0x%x\n"), id, GetLastError());
        goto failed;
    }
    Sleep(5);
    pbuf=buf;

    scnt=len/BUF_SIZE;
    rcnt=len%BUF_SIZE;
    while(scnt>0) {
        bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)pbuf,BUF_SIZE);
        if(bResult==TRUE) {
            bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);

            if(bResult==FALSE || (int)ack==(BUF_SIZE+1)) {

                if(bResult==TRUE && (int)ack==(BUF_SIZE+1)) {
                    // xub.bin is running on device
                    NucUsb.CloseWinUsbDevice(id);
                    return FW_CODE;
                } else
                    goto failed;
            }
        } else {
            NucUsb.CloseWinUsbDevice(id);
            //TRACE(_T("(%d) DDRtoDevice scnt%d error! 0x%x\n"), id, scnt, GetLastError());
            return FALSE;
        }

        scnt--;
        pbuf+=BUF_SIZE;
    }

    if(rcnt>0) {
        bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)pbuf,rcnt);
        if(bResult==TRUE) {
            bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
            if(bResult==FALSE || (int)ack==(BUF_SIZE+1)) {

                if(bResult==TRUE && (int)ack==(BUF_SIZE+1)) {
                    // xub.bin is running on device
                    NucUsb.CloseWinUsbDevice(id);
                    return FW_CODE;
                } else
                    goto failed;
            }
        } else {
            NucUsb.CloseWinUsbDevice(id);
            //TRACE(_T("XXX (%d) DDRtoDevice rcnt%d error! 0x%x\n"), id, rcnt, GetLastError());
            return FALSE;
        }
    }

    bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
    if(bResult==TRUE && ack==0x55AA55AA)
        return TRUE;

failed:
    NucUsb.CloseWinUsbDevice(id);
    return FALSE;
}

// FW bin download
BOOL FastDlg::XUSB(int id, CString& m_BinName)
{
    BOOL bResult;
    CString posstr, str;
    CString tempstr;
    int count=0;
    FILE* fp;
    int pos=0;
    AUTOTYPEHEAD fhead;
    XBINHEAD xbinhead;  // 16bytes for IBR using
    DWORD version;
    unsigned int total,file_len,scnt,rcnt,ack;

    /***********************************************/
    //TRACE(_T("FastDlg::XUSB (%d), %s\n"), id, m_BinName);
    bResult=NucUsb.EnableOneWinUsbDevice(id);
    if(!bResult) {
        //AfxMessageBox(_T("InitDlg.cpp  XUSB Device Open error\n"));
        str.Format(_T("XXX (%d) FastDlg.cpp  XUSB Device Open error\n"),id);
        TRACE("%s\n", str);
        //AfxMessageBox(str);
        return FALSE;
    }

    int fw_flag[8];
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    //mainWnd->UpdateBufForDDR();
    //TRACE(_T("(%d) DDRtoDevice  mainWnd->DDRBuf = 0x%x,  DDRLen= %d \n"), id, mainWnd->DDRBuf, mainWnd->DDRLen);
    fw_flag[id]=DDRtoDevice(id, mainWnd->ShareDDRBuf,mainWnd->DDRLen);

    if(fw_flag[id]==FALSE) {
        //m_version = _T("xxxx");
        NucUsb.WinUsbNumber -= 1;
        NucUsb.CloseWinUsbDevice(id);
        //TRACE(_T("XXX (%d) FastDlg::XUSB  m_version = xxxx \n"), id);
        return FALSE;
    }

    ULONG cbSize = 0;
    unsigned char* lpBuffer = new unsigned char[BUF_SIZE];
    fp=_wfopen(m_BinName,_T("rb"));

    if(!fp) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        AfxMessageBox(_T("Bin File Open error\n"));
        //TRACE(_T("XXX Bin File Open error\n"));
        return FALSE;
    }

    fread((char*)&xbinhead,sizeof(XBINHEAD),1,fp);
    version=xbinhead.version;

    if(fw_flag[id]==FW_CODE) {
        fclose(fp);
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        return TRUE;
    }

    //Get File Length
    fseek(fp,0,SEEK_END);
    file_len=ftell(fp)-sizeof(XBINHEAD);
    fseek(fp,0,SEEK_SET);

    if(!file_len) {
        fclose(fp);
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("File length is zero\n"));
        return FALSE;
    }

    fhead.filelen = file_len;
    fhead.address = FWGetRamAddress(fp);//0x8000;

    if(fhead.address==0xFFFFFFFF) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        TRACE(_T("Invalid Image !\n"));
        return FALSE;
    }

    memcpy(lpBuffer,(unsigned char*)&fhead,sizeof(fhead)); // 8 bytes
    bResult=NucUsb.NUC_WritePipe(id,(unsigned char*)&fhead,sizeof(fhead));
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        return FALSE;
    }
    scnt=file_len/BUF_SIZE;
    rcnt=file_len%BUF_SIZE;

    total=0;
    //TRACE(_T("(%d) FW scnt %d    rcnt = %d\n"), id, scnt, rcnt);
    while(scnt>0) {
        fread(lpBuffer,BUF_SIZE,1,fp);
        bResult=NucUsb.NUC_WritePipe(id,lpBuffer,BUF_SIZE);
        if(bResult==TRUE) {
            total+=BUF_SIZE;

            scnt--;

            bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
            if(bResult==FALSE || (int)ack!=BUF_SIZE) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                fclose(fp);

                if(bResult==TRUE && (int)ack==(BUF_SIZE+1)) {
                    // xub.bin is running on device
                    return TRUE;
                } else
                    return FALSE;
            }
        } else {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            fclose(fp);
            //TRACE(_T("XXX (%d) FW scnt error. 0x%x\n"), id, GetLastError());
            return FALSE;
        }
    }

    memset(lpBuffer,0x0,BUF_SIZE);
    if(rcnt>0) {
        fread(lpBuffer,rcnt,1,fp);
        bResult=NucUsb.NUC_WritePipe(id,lpBuffer,BUF_SIZE);
        if(bResult==TRUE) {
            total+=rcnt;
            bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                fclose(fp);
                return FALSE;
            }
        } else {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            fclose(fp);
            //TRACE(_T("XXX (%d) FW rcnt error. 0x%x\n"), id, GetLastError());
            return FALSE;
        }
    }

    delete []lpBuffer;
    NucUsb.CloseWinUsbDevice(id);
    fclose(fp);

    return TRUE;
}



void FastDlg::OnBnClickedRadioFastNand()
{
    // TODO: 在此加入控制項告知處理常式程式碼
    m_type = TYPE_NAND;
    timeoutsec = NAND_TIMEOUT_SEC;
	GetDlgItem(IDC_FAST_NAND_USRCONFIG)->EnableWindow(TRUE);
    TRACE(_T("Nand timeoutsec =%d, m_type = %d\n"), timeoutsec, m_type);
}


void FastDlg::OnBnClickedRadioFastSpi()
{
    // TODO: 在此加入控制項告知處理常式程式碼
    m_type = TYPE_SPI;
    timeoutsec = SPI_TIMEOUT_SEC;
	GetDlgItem(IDC_FAST_NAND_USRCONFIG)->EnableWindow(FALSE);
    TRACE(_T("SPI timeoutsec =%d, m_type = %d\n"), timeoutsec, m_type);
}


void FastDlg::OnBnClickedRadioFastemmc()
{
    // TODO: 在此加入控制項告知處理常式程式碼
    m_type = TYPE_EMMC;
    timeoutsec = MMC_TIMEOUT_SEC;
	GetDlgItem(IDC_FAST_NAND_USRCONFIG)->EnableWindow(FALSE);
    TRACE(_T("eMMC timeoutsec =%d, m_type = %d\n"), timeoutsec, m_type);
}


void FastDlg::OnBnClickedFastNandUsrconfig()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
	CNandInfoDlg nandinfo_dlg;   
    int i;

    TRACE(_T("FastDlg::OnBnClickedFastNandUsrconfig  mainWnd->g_iDeviceNum =%d\n"), mainWnd->g_iDeviceNum);    
	if(m_nandflashInfo_check.GetCheck()==TRUE)
	{
	    nandinfo_dlg.DoModal();
	}
    
    for(i = 0; i<mainWnd->g_iDeviceNum; i++) {
	    mainWnd->OneDeviceInfo(i);// Update nand parameters	
	}
}
