// InitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NuWriter.h"
#include "NuWriterDlg.h"
#include "InitDlg.h"
#include "NucWinUsb.h"


// CInitDlg dialog

//IMPLEMENT_DYNAMIC(CInitDlg, CDialog)

CInitDlg::CInitDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CInitDlg::IDD, pParent)
{
    m_status = _T("");
}

//CInitDlg::~CInitDlg()
//{
//}

void CInitDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DOWNPROGRESS, m_progress);
    DDX_Text(pDX, IDC_DOWNLOADSTATUS, m_status);
}


BEGIN_MESSAGE_MAP(CInitDlg, CDialog)
    ON_MESSAGE(WM_CLOSEIT,Close)
    ON_MESSAGE(WM_SHOWPROGRESS,ShowProgress)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_DOWNPROGRESS, &CInitDlg::OnNMCustomdrawDownprogress)
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CInitDlg message handlers
LRESULT CInitDlg::ShowProgress( WPARAM  p1, LPARAM p2)
{
    INT i=(INT)p1;
    INT id=(INT)p2;
    //m_progress.SetPos(i);
    ((CProgressCtrl *)GetDlgItem(IDC_DOWNPROGRESS+id))->SetPos(i);
    return true;
}

LRESULT CInitDlg::Close( WPARAM  p1, LPARAM p2)
{
    DestroyWindow();
    EndDialog(0);
    return true;
}

void CInitDlg::SetData()
{
//	m_port=port;
}

DWORD GetRamAddress(FILE* fp)
{
    BINHEADER head;
    UCHAR SIGNATURE[]= {'W','B',0x5A,0xA5};

    fread((CHAR*)&head,sizeof(head),1,fp);

    if(head.signature==*(DWORD*)SIGNATURE) {
        return head.address;
    } else
        return 0xFFFFFFFF;

}

BOOL CInitDlg::DDRtoDevice(int id, char *buf,unsigned int len)
{
    BOOL bResult;
    char *pbuf;
    unsigned int scnt,rcnt,ack;
    AUTOTYPEHEAD head;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    head.address=mainWnd->DDRAddress;  // 0x10
    head.filelen=len;	// NUC972DF62Y.ini length = 368

    Sleep(40);
    bResult=NucUsb.NUC_WritePipe(id,(unsigned char*)&head,sizeof(AUTOTYPEHEAD));
    if(bResult==FALSE) {
        //TRACE(_T("CInitDlg::DDRtoDevice error   0x%x\n"), GetLastError());
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
BOOL CInitDlg::XUSB(int id, CString& m_pathName)
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
    ResetEvent(m_ExitEvent);
    /***********************************************/

    bResult=NucUsb.EnableOneWinUsbDevice(id);
    if(!bResult) {
        //AfxMessageBox(_T("InitDlg.cpp  XUSB Device Open error\n"));
        str.Format(_T("XXX (%d) InitDlg.cpp  XUSB Device Open error\n"),id);
        //AfxMessageBox(str);
        return FALSE;

    }

    int fw_flag;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    if(mainWnd->ShareDDRBuf[0] != 0) {
        fw_flag=DDRtoDevice(id, mainWnd->ShareDDRBuf, mainWnd->DDRLen);
        //TRACE(_T("ShareDDRBuf useful. flag = 0x%x\n"), fw_flag);
    } else {
        mainWnd->UpdateBufForDDR();
        //fw_flag=DDRtoDevice(id, mainWnd->DDRBuf, mainWnd->DDRLen);
		fw_flag=DDRtoDevice(id, mainWnd->ShareDDRBuf, mainWnd->DDRLen);
        //TRACE(_T("1st read DDR File. fw_flag = 0x%x\n"), fw_flag);
    }

    if(fw_flag==FALSE) {
        m_version = _T("xxxx");
        NucUsb.WinUsbNumber -= 1;
        NucUsb.CloseWinUsbDevice(id);
        return FALSE;
    }

    ULONG cbSize = 0;
    unsigned char* lpBuffer = new unsigned char[BUF_SIZE];
    fp=_wfopen(m_pathName,_T("rb"));

    if(!fp) {
        if(mainWnd->DDRBuf!=NULL)
            free(mainWnd->DDRBuf);
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }

    fread((char*)&xbinhead,sizeof(XBINHEAD),1,fp);
    version=xbinhead.version;

    //07121401 -> 測桶 2007/12/14 - 01 唳
    m_version.Format(_T(" 20%02x/%02x/%02x-V%02x"),(version>>24)&0xff,(version>>16)&0xff,\
                     (version>>8)&0xff,(version)&0xff);
    if(fw_flag==FW_CODE) {
        delete []lpBuffer;
        //NucUsb.CloseWinUsbDevice(id);
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
        AfxMessageBox(_T("File length is zero\n"));
        return FALSE;
    }

    fhead.filelen = file_len;
    fhead.address = GetRamAddress(fp);//0x8000;

    if(fhead.address==0xFFFFFFFF) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        AfxMessageBox(_T("Invalid Image !"));
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

    while(scnt>0) {
        fread(lpBuffer,BUF_SIZE,1,fp);
        bResult=NucUsb.NUC_WritePipe(id,lpBuffer,BUF_SIZE);
        if(bResult==TRUE) {
            total+=BUF_SIZE;

            pos=(int)(((float)(((float)total/(float)file_len))*100));
            posstr.Format(_T("%d%%"),pos);

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

        }

        scnt--;

        if(pos%5==0) {
            PostMessage(WM_SHOWPROGRESS,(LPARAM)pos, id);
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
        }

        pos=(int)(((float)(((float)total/(float)file_len))*100));

        if(pos>=100) {
            pos=100;
        }
        posstr.Format(_T("%d%%"),pos);

        if(pos%5==0) {
            PostMessage(WM_SHOWPROGRESS,(LPARAM)pos,id);
        }
    }

    delete []lpBuffer;
    NucUsb.CloseWinUsbDevice(id);
    fclose(fp);
    return TRUE;

}
/////////////////////////////////////////////////////////////////////////////
// CInitDlg message handlers

BOOL CInitDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    int i;
    int param = 0;
    int * pparam = &param;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    iDevice=0;
    // TODO: Add extra initialization here
    m_progress.SetRange(0,100);
    m_progress.SetBkColor(RGB(245, 92, 61));
    m_status="Initializing :";

    for(i=0; i<mainWnd->g_iDeviceNum; i++) {
        ((CProgressCtrl *)GetDlgItem(IDC_DOWNPROGRESS+i))->SetRange(0,100);
        ((CProgressCtrl *)GetDlgItem(IDC_DOWNPROGRESS+i))->SetBkColor(RGB(245, 92, 61));
    }

    UpdateData(FALSE);
    memset(mainWnd->ShareDDRBuf, 0, sizeof(mainWnd->ShareDDRBuf));
#if(1)
    if(mainWnd->g_iDeviceNum == 1) {
        hThread0=(HANDLE)_beginthreadex(NULL,0,Download_proc1,(void*)this,0,&Thread0);
        WaitForSingleObject(hThread0,INFINITE);
    } else if(mainWnd->g_iDeviceNum == 2) {
        hThread0=(HANDLE)_beginthreadex(NULL,0,Download_proc1,(void*)this,0,&Thread0);
        WaitForSingleObject(hThread0,INFINITE);
        hThread1=(HANDLE)_beginthreadex(NULL,0,Download_proc2,(void*)this,0,&Thread1);
        WaitForSingleObject(hThread1,INFINITE);
    } else if(mainWnd->g_iDeviceNum == 3) {
        hThread0=(HANDLE)_beginthreadex(NULL,0,Download_proc1,(void*)this,0,&Thread0);
        WaitForSingleObject(hThread0,INFINITE);

        hThread1=(HANDLE)_beginthreadex(NULL,0,Download_proc2,(void*)this,0,&Thread1);
        WaitForSingleObject(hThread1,INFINITE);

        hThread2=(HANDLE)_beginthreadex(NULL,0,Download_proc3,(void*)this,0,&Thread2);
        WaitForSingleObject(hThread2,INFINITE);

    } else if(mainWnd->g_iDeviceNum == 4) {
        hThread0=(HANDLE)_beginthreadex(NULL,0,Download_proc1,(void*)this,0,&Thread0);
        WaitForSingleObject(hThread0,INFINITE);

        hThread1=(HANDLE)_beginthreadex(NULL,0,Download_proc2,(void*)this,0,&Thread1);
        WaitForSingleObject(hThread1,INFINITE);

        hThread2=(HANDLE)_beginthreadex(NULL,0,Download_proc3,(void*)this,0,&Thread2);
        WaitForSingleObject(hThread2,INFINITE);

        hThread3=(HANDLE)_beginthreadex(NULL,0,Download_proc4,(void*)this,0,&Thread3);
        WaitForSingleObject(hThread3,INFINITE);

    } else if(mainWnd->g_iDeviceNum == 5) {
        hThread0=(HANDLE)_beginthreadex(NULL,0,Download_proc1,(void*)this,0,&Thread0);
        WaitForSingleObject(hThread0,INFINITE);

        hThread1=(HANDLE)_beginthreadex(NULL,0,Download_proc2,(void*)this,0,&Thread1);
        WaitForSingleObject(hThread1,INFINITE);

        hThread2=(HANDLE)_beginthreadex(NULL,0,Download_proc3,(void*)this,0,&Thread2);
        WaitForSingleObject(hThread2,INFINITE);

        hThread3=(HANDLE)_beginthreadex(NULL,0,Download_proc4,(void*)this,0,&Thread3);
        WaitForSingleObject(hThread3,INFINITE);

        hThread4=(HANDLE)_beginthreadex(NULL,0,Download_proc5,(void*)this,0,&Thread4);
        WaitForSingleObject(hThread4,INFINITE);

    } else if(mainWnd->g_iDeviceNum == 6) {
        hThread0=(HANDLE)_beginthreadex(NULL,0,Download_proc1,(void*)this,0,&Thread0);
        WaitForSingleObject(hThread0,INFINITE);

        hThread1=(HANDLE)_beginthreadex(NULL,0,Download_proc2,(void*)this,0,&Thread1);
        WaitForSingleObject(hThread1,INFINITE);

        hThread2=(HANDLE)_beginthreadex(NULL,0,Download_proc3,(void*)this,0,&Thread2);
        WaitForSingleObject(hThread2,INFINITE);

        hThread3=(HANDLE)_beginthreadex(NULL,0,Download_proc4,(void*)this,0,&Thread3);
        WaitForSingleObject(hThread3,INFINITE);

        hThread4=(HANDLE)_beginthreadex(NULL,0,Download_proc5,(void*)this,0,&Thread4);
        WaitForSingleObject(hThread4,INFINITE);

        hThread5=(HANDLE)_beginthreadex(NULL,0,Download_proc6,(void*)this,0,&Thread5);
        WaitForSingleObject(hThread5,INFINITE);

    } else if(mainWnd->g_iDeviceNum == 7) {
        hThread0=(HANDLE)_beginthreadex(NULL,0,Download_proc1,(void*)this,0,&Thread0);
        WaitForSingleObject(hThread0,INFINITE);

        hThread1=(HANDLE)_beginthreadex(NULL,0,Download_proc2,(void*)this,0,&Thread1);
        WaitForSingleObject(hThread1,INFINITE);

        hThread2=(HANDLE)_beginthreadex(NULL,0,Download_proc3,(void*)this,0,&Thread2);
        WaitForSingleObject(hThread2,INFINITE);

        hThread3=(HANDLE)_beginthreadex(NULL,0,Download_proc4,(void*)this,0,&Thread3);
        WaitForSingleObject(hThread3,INFINITE);

        hThread4=(HANDLE)_beginthreadex(NULL,0,Download_proc5,(void*)this,0,&Thread4);
        WaitForSingleObject(hThread4,INFINITE);

        hThread5=(HANDLE)_beginthreadex(NULL,0,Download_proc6,(void*)this,0,&Thread5);
        WaitForSingleObject(hThread5,INFINITE);

        hThread6=(HANDLE)_beginthreadex(NULL,0,Download_proc7,(void*)this,0,&Thread6);
        WaitForSingleObject(hThread6,INFINITE);

    } else if(mainWnd->g_iDeviceNum == 8) {
        hThread0=(HANDLE)_beginthreadex(NULL,0,Download_proc1,(void*)this,0,&Thread0);
        WaitForSingleObject(hThread0,INFINITE);

        hThread1=(HANDLE)_beginthreadex(NULL,0,Download_proc2,(void*)this,0,&Thread1);
        WaitForSingleObject(hThread1,INFINITE);

        hThread2=(HANDLE)_beginthreadex(NULL,0,Download_proc3,(void*)this,0,&Thread2);
        WaitForSingleObject(hThread2,INFINITE);

        hThread3=(HANDLE)_beginthreadex(NULL,0,Download_proc4,(void*)this,0,&Thread3);
        WaitForSingleObject(hThread3,INFINITE);

        hThread4=(HANDLE)_beginthreadex(NULL,0,Download_proc5,(void*)this,0,&Thread4);
        WaitForSingleObject(hThread4,INFINITE);

        hThread5=(HANDLE)_beginthreadex(NULL,0,Download_proc6,(void*)this,0,&Thread5);
        WaitForSingleObject(hThread5,INFINITE);

        hThread6=(HANDLE)_beginthreadex(NULL,0,Download_proc7,(void*)this,0,&Thread6);
        WaitForSingleObject(hThread6,INFINITE);

        hThread7=(HANDLE)_beginthreadex(NULL,0,Download_proc8,(void*)this,0,&Thread7);
        WaitForSingleObject(hThread7,INFINITE);

    }

#else

    // Loop until CheckKey terminates program or 1000 threads created.
    hThread=(HANDLE)_beginthreadex(NULL,0,Download_proc,(void*)this,0,&Thread1);

#endif

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CInitDlg:: Download(int id)
{
    int i=0;
	unsigned int address,value;
    TCHAR path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    CString temp=path;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
	CString filename=NULL;
    BOOL bResult;

	COLORREF col = RGB(0xFF, 0x00, 0xFF);
	ifstream Read;
	char *ptmp,tmp[256],cvt[128];
	Read.open(mainWnd->DDRFileFullPath,ios::binary | ios::in);

	if(!Read.is_open())
		AfxMessageBox(_T("open DDR initial file error\n"));
	while(!Read.eof())
	{
		Read.getline(tmp,256);
		ptmp=strchr(tmp,'=');
		if(ptmp==NULL)
		{
			AfxMessageBox(_T("DDR initial format error\n"));
			break;
		}
		strncpy(cvt,tmp,(unsigned int)ptmp-(unsigned int)tmp);
		cvt[(unsigned int)ptmp-(unsigned int)tmp]='\0';
		address=strtoul(cvt,NULL,0);

		strncpy(cvt,++ptmp,strlen(ptmp));
		cvt[strlen(ptmp)]='\0';
		value=strtoul(cvt,NULL,0);	

		if(address==0xB0001810){
			temp = temp.Left(temp.ReverseFind('\\') + 1);
			switch(value&0x7){
				case 4:
					filename.Format(_T("%sxusb16.bin"),temp);
					break;
				case 5:
					filename.Format(_T("%sxusb.bin"),temp);
					break;
				case 6:
					filename.Format(_T("%sxusb64.bin"),temp);
					break;
				case 7:
					filename.Format(_T("%sxusb128.bin"),temp);
					break;
			}
		}
	}
	Read.close();

    bResult = XUSB(id, filename);
#if(1)
    if(!bResult) {
	    TRACE(_T("XXX (%d) CInitDlg:: xusb Download ERROR\n"), id);
		mainWnd->ShowDeviceConnectState(0);//Disconnected
	    mainWnd->GetDlgItem(IDC_RECONNECT)->EnableWindow(TRUE);
        NucUsb.CloseWinUsbDevice(id);
    }
#endif
    return ;
}

#if(1)
unsigned WINAPI CInitDlg:: Download_proc(void* args)
{
    CInitDlg* pThis = reinterpret_cast<CInitDlg*>(args);

    int i, DevCount = 0;

    DevCount = NucUsb.UsbDevice_Detect();

    for(i=0; i< DevCount; i++) {
        //TRACE("Download_proc:idevice =%d\n",i);
        pThis->Download(i);
    }

    pThis->PostMessage(WM_CLOSE,0,0);

    return 0;
}

unsigned WINAPI CInitDlg:: Download_proc1(void* args)
{
    CInitDlg* pThis = reinterpret_cast<CInitDlg*>(args);

    pThis->Download(0);
    pThis->PostMessage(WM_CLOSE,0,0);

    return 0;
}

unsigned WINAPI CInitDlg:: Download_proc2(void* args)
{
    CInitDlg* pThis = reinterpret_cast<CInitDlg*>(args);

    pThis->Download(1);
    pThis->PostMessage(WM_CLOSE,0,0);

    return 0;

}

unsigned WINAPI CInitDlg:: Download_proc3(void* args)
{
    CInitDlg* pThis = reinterpret_cast<CInitDlg*>(args);

    pThis->Download(2);
    pThis->PostMessage(WM_CLOSE,0,0);

    return 0;

}

unsigned WINAPI CInitDlg:: Download_proc4(void* args)
{
    CInitDlg* pThis = reinterpret_cast<CInitDlg*>(args);

    pThis->Download(3);
    pThis->PostMessage(WM_CLOSE,0,0);

    return 0;

}

unsigned WINAPI CInitDlg:: Download_proc5(void* args)
{
    CInitDlg* pThis = reinterpret_cast<CInitDlg*>(args);

    pThis->Download(4);
    pThis->PostMessage(WM_CLOSE,0,0);

    return 0;

}

unsigned WINAPI CInitDlg:: Download_proc6(void* args)
{
    CInitDlg* pThis = reinterpret_cast<CInitDlg*>(args);

    pThis->Download(5);
    pThis->PostMessage(WM_CLOSE,0,0);

    return 0;

}

unsigned WINAPI CInitDlg:: Download_proc7(void* args)
{
    CInitDlg* pThis = reinterpret_cast<CInitDlg*>(args);

    pThis->Download(6);
    pThis->PostMessage(WM_CLOSE,0,0);

    return 0;

}

unsigned WINAPI CInitDlg:: Download_proc8(void* args)
{
    CInitDlg* pThis = reinterpret_cast<CInitDlg*>(args);

    pThis->Download(7);
    pThis->PostMessage(WM_CLOSE,0,0);

    return 0;

}

#endif

void CInitDlg::OnNMCustomdrawDownprogress(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    *pResult = 0;
}


void CInitDlg::OnClose()
{
    // TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

    SetEvent(m_ExitEvent);
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    TRACE("CInitDlg::OnClose()  %d\n", mainWnd->g_iDeviceNum);
#if(0)
    switch(mainWnd->g_iDeviceNum) {
    case 1:
        CloseHandle(hThread0);
        break;
    case 2: {
        CloseHandle(hThread0);
        CloseHandle(hThread1);
        break;
    }
    case 3: {
        CloseHandle(hThread0);
        CloseHandle(hThread1);
        CloseHandle(hThread2);
        break;
    }
    case 4: {
        CloseHandle(hThread0);
        CloseHandle(hThread1);
        CloseHandle(hThread2);
        CloseHandle(hThread3);
        break;
    }
    case 5: {
        CloseHandle(hThread0);
        CloseHandle(hThread1);
        CloseHandle(hThread2);
        CloseHandle(hThread3);
        CloseHandle(hThread4);
        break;
    }
    case 6: {
        CloseHandle(hThread0);
        CloseHandle(hThread1);
        CloseHandle(hThread2);
        CloseHandle(hThread3);
        CloseHandle(hThread4);
        CloseHandle(hThread5);
        break;
    }
    case 7: {
        CloseHandle(hThread0);
        CloseHandle(hThread1);
        CloseHandle(hThread2);
        CloseHandle(hThread3);
        CloseHandle(hThread4);
        CloseHandle(hThread5);
        CloseHandle(hThread6);
        break;
    }
    case 8: {
        CloseHandle(hThread0);
        CloseHandle(hThread1);
        CloseHandle(hThread2);
        CloseHandle(hThread3);
        CloseHandle(hThread4);
        CloseHandle(hThread5);
        CloseHandle(hThread6);
        CloseHandle(hThread7);
        break;
    }
    }

#endif
    CDialog::OnClose();
}
