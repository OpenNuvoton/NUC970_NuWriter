#include "stdafx.h"
#include "NuWriter.h"
#include "NuWriterDlg.h"
#include "NucWinUsb.h"
//#include "enumser.h"

//#include "strsafe.h"
#define BATCH_BURN

#define EMMC_RW_PIPE_TIMEOUT    (1)    ///< timeout for 1 secs

unsigned char * DDR2Buf(char *buf,int buflen,int *ddrlen);

//const UCHAR usb_count=sizeof(usbdesc)/sizeof(USB_DESC);

int GetFileLength(CString& filename)
{
    FILE* rfp;
    int fileSize;
    if( (rfp=_wfopen(filename, _T("rb")))==NULL ) {
        AfxMessageBox(_T("ERROR: Can't open file !"));
        fclose(rfp);
        return -1;
    }

    fseek(rfp, 0,SEEK_END);
    fileSize=ftell(rfp);
    fclose(rfp);

    return fileSize;

}

DWORD atox(CString& str)
{
    DWORD value;
    swscanf_s(str,_T("%x"),&value);
    return value;
}


MaxOffset GetMaxOffset(CIniFile& inifile,CString keyname)
{
    MaxOffset maxdata;
    CString nostr;
    UINT32 offset;
    int image_index=inifile.FindKeyX(_T("Image "));

    maxdata.maxoffset=0;

    for(int k=7; k>=0; k--) {
        if((1<<k)&image_index) {
            nostr.Format(_T("Image %d"),k);
            offset=atox(inifile.GetValue(nostr,keyname));
            if(offset>=maxdata.maxoffset) {
                maxdata.maxoffset=offset;
                maxdata.imageno=k;
            }
        }

    }

    return maxdata;
}

BOOL OffsetLess (MaxOffset e1, MaxOffset e2 )
{
    return e1.maxoffset < e2.maxoffset;
}


VOID SortbyOffset(CIniFile& inifile,CString keyname,vector<MaxOffset>& vect)
{
    MaxOffset maxdata;
    CString nostr;
    UINT32 offset;
    int image_index=inifile.FindKeyX(_T("Image "));

    maxdata.maxoffset=0;

    for(int k=7; k>=0; k--) {
        if((1<<k)&image_index) {
            nostr.Format(_T("Image %d"),k);
            offset=atox(inifile.GetValue(nostr,keyname));

            maxdata.maxoffset=offset;
            maxdata.imageno=k;

            vect.push_back(maxdata);

        }

    }
    sort(vect.begin(), vect.end(), OffsetLess);

    return ;
}

BOOL Auto_Detect(CString& portName,CString& tempName)
{
    BOOL bResult = TRUE;
    BOOL bDev = FALSE;
    bResult=NucUsb.EnableWinUsbDevice();

    if(bResult==TRUE) {
        tempName.Format(_T(" Connected"));
        portName.Format(_T("Nuvoton VCOM"));
    }
    NucUsb.NUC_CloseHandle();
    return (bResult);
}

BOOL Device_Detect(CString& portName,CString& tempName)
{
    int iDeviceNum= 0;
    BOOL bResult = FALSE;
    iDeviceNum = NucUsb.UsbDevice_Detect();
    NucUsb.WinUsbNumber = iDeviceNum;

    if(iDeviceNum) {
        //tempName.Format(_T(" %d Device Connected"),iDeviceNum);
        tempName.Format(_T("Device Connected"));
        portName.Format(_T("Nuvoton VCOM"));
        bResult = TRUE;
    }
    //TRACE("Device_Detect: NucUsb.WinUsbNumber=%d\n",NucUsb.WinUsbNumber);
    return (bResult);
}

BOOLEAN DataCompare(char* base,char* src,int len)
{
    int i=0;
    for(i=0; i<len; i++) {
        if(base[i]!=src[i])
            return FALSE;
    }

    return TRUE;
}


unsigned char * DDR2Buf(char *buf,int buflen,int *ddrlen)
{
#if(0)  // memory leak
    unsigned char *ddrbuf;
    *ddrlen=((buflen+8+15)/16)*16;
    ddrbuf=(unsigned char *)malloc(sizeof(unsigned char)*(*ddrlen));
    memset(ddrbuf,0x0,*ddrlen);
    *(ddrbuf+0)=0x55;
    *(ddrbuf+1)=0xAA;
    *(ddrbuf+2)=0x55;
    *(ddrbuf+3)=0xAA;
    *((unsigned int *)(ddrbuf+4))=(buflen/8);        /* len */
    memcpy((ddrbuf+8),buf,buflen);
    //TRACE(_T("DDR2Buf --> ddrlen = %d\n"), *ddrlen);
    return ddrbuf;
#else
    //unsigned char ddrbuf[384];
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    *ddrlen=((buflen+8+15)/16)*16;
    memset(mainWnd->DDR2Buf, 0, 512);

    mainWnd->DDR2Buf[0]=0x55;
    mainWnd->DDR2Buf[1]=0xAA;
    mainWnd->DDR2Buf[2]=0x55;
    mainWnd->DDR2Buf[3]=0xAA;
    *((unsigned int *)(mainWnd->DDR2Buf+4))=(buflen/8);        /* len */
    memcpy(&mainWnd->DDR2Buf[8],buf,buflen);
    TRACE(_T("DDR2Buf --> ddrlen = %d\n"), *ddrlen);
    return (unsigned char *)mainWnd->DDR2Buf;
#endif
}

BOOL CheckDDRiniData(char *buf, int filelen)
{
	CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
	INT i, j, ini_idx, ddr_cnt, start_idx = 0, ddr_len = 0;

	// Check DDR *ini
	i = PACK_FORMAT_HEADER; // PACK Header 16
	ini_idx = 0;
	ddr_cnt = 0;
	// Find DDR Initial Marker
	while(i < filelen) 
	{
		if((buf[i] == 0x20) && (buf[i+1] == 'T') && (buf[i+2] == 'V') && (buf[i+3] == 'N'))
		{			
			if((buf[i+BOOT_HEADER] == 0x55) && (buf[i+BOOT_HEADER+1] == 0xffffffaa) && (buf[i+BOOT_HEADER+2] == 0x55) && (buf[i+BOOT_HEADER+3] == 0xffffffaa))
			{
				ini_idx = (i+BOOT_HEADER); // Found DDR
				ddr_cnt = ((buf[ini_idx+7]&0xff) << 24 | (buf[ini_idx+6]&0xff) << 16 | (buf[ini_idx+5]&0xff) << 8 | (buf[ini_idx+4]&0xff));
				TRACE(_T("ini_idx:0x%x(%d)  ddr_cnt =0x%x(%d)\n"), ini_idx, ini_idx, ddr_cnt, ddr_cnt);
				break;
			}
		}
	    i++;
	}

	j = 0;
	// Compare DDR *ini content
	start_idx = ini_idx+DDR_INITIAL_MARKER+DDR_COUNTER;//ini_idx+8
	ddr_len = ddr_cnt*8;
	for(i = start_idx; i < (start_idx + ddr_len); i++)
	{
		if(buf[i] != mainWnd->ShareDDRBuf[j++])
		{
            TRACE(_T("DDR parameter error! buf[%d]= 0x%x, mainWnd->ShareDDRBuf[%d]=0x%x\n"), i, buf[i], j, mainWnd->ShareDDRBuf[j]);
			return FALSE;
		}
	}

    return TRUE;
}

/************** SDRAM Begin ************/

//BOOL CRAMDlg::XUSB(CString& portName,CString& m_pathName)
BOOL CRAMDlg::XUSB(CString& portName,CString& m_pathName,CString address,int autorun)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    FILE* fp;
    int pos=0;
    SDRAM_RAW_TYPEHEAD fhead;
    unsigned int total,file_len,scnt,rcnt,ack;
    char* lpBuffer;

    bResult=NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack=0x0;
    bResult=NucUsb.NUC_SetType(0,SDRAM,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        AfxMessageBox(_T("typeack failed !!!\n"));
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }

    lpBuffer = new char[BUF_SIZE];

    fp=_wfopen(m_pathName,_T("rb"));

    if(!fp) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
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

    fhead.flag=WRITE_ACTION;
    fhead.filelen=file_len;
    fhead.dtbaddress = 0;
    swscanf_s(address,_T("%x"),&fhead.address);

    if(autorun) {
        fhead.address|=NEED_AUTORUN;
    }

    if(autorun==2) {
        swscanf_s(m_dtbaddress,_T("%x"),&fhead.dtbaddress);
        fhead.dtbaddress |= NEED_AUTORUN;
    }

    memcpy(lpBuffer,(unsigned char*)&fhead,sizeof(SDRAM_RAW_TYPEHEAD));
    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(SDRAM_RAW_TYPEHEAD));
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("Write SDRAM head error\n"));
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("SDRAM head ack error\n"));
        return FALSE;
    }

    scnt=file_len/BUF_SIZE;
    rcnt=file_len%BUF_SIZE;

    total=0;
    while(scnt>0) {

        fread(lpBuffer,BUF_SIZE,1,fp);
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer,BUF_SIZE);

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            fclose(fp);
            return FALSE;
        }

        if(bResult==TRUE) {
            total+=BUF_SIZE;

            pos=(int)(((float)(((float)total/(float)file_len))*100));
            posstr.Format(_T("%d%%"),pos);
            bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
            if(bResult==FALSE || ack!=BUF_SIZE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("ACK error !"));
                return FALSE;
            }

        } else {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            fclose(fp);
            return FALSE;
        }

        scnt--;

        if(pos%5==0) {
            PostMessage(WM_SDRAM_PROGRESS,(LPARAM)pos,0);
        }


    }

    if(rcnt>0) {
        fread(lpBuffer,rcnt,1,fp);
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer,rcnt);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            fclose(fp);
            return FALSE;
        }

        if(bResult==TRUE) {
            total+=rcnt;
            bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("ACK error !"));
                return FALSE;
            }

        } else {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            fclose(fp);
            return FALSE;
        }

        pos=(int)(((float)(((float)total/(float)file_len))*100));

        if(pos>=100) {
            pos=100;
        }
        posstr.Format(_T("%d%%"),pos);

        if(pos%5==0) {
            PostMessage(WM_SDRAM_PROGRESS,(LPARAM)pos,0);
        }

    }

    delete []lpBuffer;
    fclose(fp);
    NucUsb.NUC_CloseHandle();
    return TRUE;

}
/************** SDRAM End ************/

/************** SPI Begin ************/
BOOL CSPIDlg::XUSB_Pack(CString& portName,CString& m_pathName,int *len)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    FILE* fp;
    int pos=0;
    UCHAR burn_pos=0;
    unsigned int total,file_len,scnt,rcnt;
    ULONG ack;
    //NORBOOT_NAND_HEAD fhead;
    char* lpBuffer;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    bResult = NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack=0x0;
    bResult=NucUsb.NUC_SetType(0,SPI,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }


    fp=_wfopen(m_pathName,_T("rb"));

    if(!fp) {
        //delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }

    fseek(fp,0,SEEK_END);
    file_len=ftell(fp);
    fseek(fp,0,SEEK_SET);

    if(!file_len) {
        //delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("File length is zero\n"));
        return FALSE;
    }

    lpBuffer = new char[file_len]; //read file to buffer
    memset(lpBuffer,0x00,file_len);

    memset((unsigned char*)m_fhead,0,sizeof(NORBOOT_NAND_HEAD));
    total=0;
    m_fhead->flag=PACK_ACTION;
    m_fhead->type=m_type;
    m_fhead->filelen=file_len;
    memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_NAND_HEAD));
    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));

    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("Write SPI head error\n"));
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("ACK error !"));
        return FALSE;
    }

    fread(lpBuffer,m_fhead->filelen,1,fp);
    if(lpBuffer[0]!=0x5) {
        AfxMessageBox(_T("This file is not pack image"));
		delete []lpBuffer;
        //NucUsb.NUC_CloseHandle();
        fclose(fp);
        return FALSE;
    }
    fclose(fp);

	// Check DDR *ini
	bResult=CheckDDRiniData(lpBuffer, m_fhead->filelen);
	if(bResult==FALSE) {
		AfxMessageBox(_T("DDR Init select error\n"));
		delete []lpBuffer;
		//NucUsb.NUC_CloseHandle();
		return FALSE;
    }

    char *pbuf = lpBuffer;
    PACK_HEAD *ppackhead=(PACK_HEAD *)lpBuffer;
    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf, sizeof(PACK_HEAD));
    Sleep(5);
    if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT)	return FALSE;
    if(bResult!=TRUE) return FALSE;
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
	    delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }
    total+= sizeof(PACK_HEAD);
    pbuf+= sizeof(PACK_HEAD);

    PACK_CHILD_HEAD child;
    m_progress.SetRange(0,short(ppackhead->num*200));
    int posnum=0;
    for(int i=0; i<(int)(ppackhead->num); i++) {
        memcpy(&child,(char *)pbuf,sizeof(PACK_CHILD_HEAD));
        //Sleep(20);
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf, sizeof(PACK_CHILD_HEAD));
		if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
			delete []lpBuffer;
			return FALSE;
		}
        if(bResult!=TRUE) {
			delete []lpBuffer;
			return FALSE;
		}
        total+= sizeof(PACK_CHILD_HEAD);
        pbuf+= sizeof(PACK_CHILD_HEAD);

        scnt=child.filelen/BUF_SIZE;
        rcnt=child.filelen%BUF_SIZE;

        while(scnt>0) {
            //Sleep(20);
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf, BUF_SIZE);
            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                //fclose(fp);
				delete []lpBuffer;
                return FALSE;
            }

            if(bResult==TRUE) {
                pbuf+=BUF_SIZE;
                total+=BUF_SIZE;

                pos=(int)(((float)(((float)total/(float)file_len))*100));
                posstr.Format(_T("%d%%"),pos);

                DbgOut("SPI wait ack");
                bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
                DbgOut("SPI wait ack end");

                if(bResult==FALSE) {
                    delete []lpBuffer;
                    NucUsb.NUC_CloseHandle();
                    //fclose(fp);
                    AfxMessageBox(_T("ACK error !"));
                    return FALSE;
                }

            }

            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
			    delete []lpBuffer;
                return FALSE;
            }

            scnt--;

            if(pos%5==0) {
                PostMessage(WM_SPI_PROGRESS,(LPARAM)(posnum+pos),0);
            }

        }

        if(rcnt>0) {
            //Sleep(20);
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf,rcnt);
            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }

            //printf("upload %%.2f\r",((float)total/file_len)*100);
            if(bResult==TRUE) {
                pbuf+=rcnt;
                total+=rcnt;
                DbgOut("SPI wait ack");
                bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
                DbgOut("SPI wait ack end");

                if(bResult==FALSE) {
                    delete []lpBuffer;
                    NucUsb.NUC_CloseHandle();
                    //fclose(fp);
                    AfxMessageBox(_T("ACK error !"));
                    return FALSE;

                }

            }

            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                //fclose(fp);
				delete []lpBuffer;
                return FALSE;
            }

            pos=(int)(((float)(((float)total/(float)file_len))*100));

            if(pos>=100) {
                pos=100;
            }
            posstr.Format(_T("%d%%"),pos);

            if(pos%5==0) {

                PostMessage(WM_SPI_PROGRESS,(LPARAM)(posnum+pos),0);
            }

        }



        posnum+=100;
        //fclose(fp);
        //burn progress...
        burn_pos=0;
        //PostMessage(WM_SPI_PROGRESS,(LPARAM)0,0);
        m_progress.SetBkColor(COLOR_BURN);

        while(burn_pos!=100) {
            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                return FALSE;
            }

            DbgOut("SPI wait burn ack");
            bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                AfxMessageBox(_T("ACK error !"));
                return FALSE;
            }
            DbgOut("SPI wait burn ack end");
            if(!((ack>>16)&0xffff)) {
                burn_pos=(UCHAR)(ack&0xffff);
                PostMessage(WM_SPI_PROGRESS,(LPARAM)(posnum+burn_pos),0);
            } else {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                AfxMessageBox(_T("Burn error"));
                return FALSE;
            }
        }
        posnum+=100;
    }

    delete []lpBuffer;
    NucUsb.NUC_CloseHandle();
    return TRUE;

}

BOOL CSPIDlg::XUSB_Burn(CString& portName,CString& m_pathName,int *len)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    FILE* fp;
    int pos=0;
    UCHAR burn_pos=0;
    unsigned int total,file_len,scnt,rcnt;
    ULONG ack;
    //NORBOOT_NAND_HEAD fhead;
    char* lpBuffer;
    unsigned char *ddrbuf;
    int ddrlen;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    m_progress.SetRange(0,100);
    bResult = NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack=0x0;
    bResult=NucUsb.NUC_SetType(0,SPI,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }


    fp=_wfopen(m_pathName,_T("rb"));

    if(!fp) {
        //delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }

    fseek(fp,0,SEEK_END);
    file_len=ftell(fp);
    fseek(fp,0,SEEK_SET);

    if(!file_len) {
        //delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("File length is zero\n"));
        return FALSE;
    }


    memset((unsigned char*)m_fhead,0,sizeof(NORBOOT_NAND_HEAD));

    m_fhead->flag=WRITE_ACTION;
    ((NORBOOT_NAND_HEAD *)m_fhead)->initSize=0;

    //check AES------------------------------
    if(m_spi_enc_check.GetCheck()==TRUE)
        file_len=((file_len+15)/16)*16;
    m_fhead->filelen=file_len;

#if 1
    switch(m_type) {
    case DATA:
    case PACK:
        swscanf_s(_T("0"),_T("%d"),&m_fhead->no);
        swscanf_s(m_execaddr,_T("%x"),&m_fhead->execaddr);
        swscanf_s(m_startblock,_T("%x"),&m_fhead->flashoffset);
        *len=file_len;
        lpBuffer = new char[file_len+sizeof(NORBOOT_NAND_HEAD)]; //read file to buffer
        memset(lpBuffer,0xff,file_len+sizeof(NORBOOT_NAND_HEAD));
        ((NORBOOT_NAND_HEAD *)m_fhead)->macaddr[7]=0;
        wcstombs( m_fhead->name, (wchar_t *)m_imagename.GetBuffer(), 16);
        m_fhead->type=m_type;
        memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_NAND_HEAD));
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));
        break;
    case ENV:
        swscanf_s(_T("0"),_T("%d"),&m_fhead->no);
        swscanf_s(m_execaddr,_T("%x"),&m_fhead->execaddr);
        swscanf_s(m_startblock,_T("%x"),&m_fhead->flashoffset);
        if(file_len>(0x10000-4)) {
            //delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            AfxMessageBox(_T("The environment file size is less then 64KB\n"));
            return FALSE;
        }
        lpBuffer = new char[0x10000]; //read file to buffer
        memset(lpBuffer,0x00,0x10000);

        ((NORBOOT_NAND_HEAD *)m_fhead)->macaddr[7]=0;

        wcstombs( m_fhead->name, (wchar_t *)m_imagename.GetBuffer(), 16);
        m_fhead->filelen=0x10000;
        m_fhead->type=m_type;
        memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_NAND_HEAD));
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));
        break;
    case UBOOT:
        swscanf_s(_T("1"),_T("%d"),&m_fhead->no);
        swscanf_s(m_execaddr,_T("%x"),&m_fhead->execaddr);
        swscanf_s(m_startblock,_T("%x"),&m_fhead->flashoffset);
        //-------------------DDR---------------------
        //ddrbuf=DDR2Buf(mainWnd->DDRBuf,mainWnd->DDRLen,&ddrlen);
        ddrbuf=DDR2Buf(mainWnd->ShareDDRBuf,mainWnd->DDRLen,&ddrlen);
        file_len=file_len+ddrlen;
        ((NORBOOT_NAND_HEAD *)m_fhead)->initSize=ddrlen;
        //-------------------------------------------
        *len=file_len;
        lpBuffer = new char[file_len]; //read file to buffer
        memset(lpBuffer,0xff,file_len);
        ((NORBOOT_NAND_HEAD *)m_fhead)->macaddr[7]=0;


        wcstombs( m_fhead->name, (wchar_t *)m_imagename.GetBuffer(), 16);

        m_fhead->type=m_type;
        memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_NAND_HEAD));
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));
        break;
    }
#else
    if(m_type!=UBOOT) {
        swscanf_s(_T("0"),_T("%d"),&m_fhead->no);
        swscanf_s(m_execaddr,_T("%x"),&m_fhead->execaddr);
        swscanf_s(m_startblock,_T("%x"),&m_fhead->flashoffset);
        *len=file_len;

    } else {
        swscanf_s(_T("1"),_T("%d"),&m_fhead->no);
        swscanf_s(m_execaddr,_T("%x"),&m_fhead->execaddr);
        swscanf_s(m_startblock,_T("%x"),&m_fhead->flashoffset);
        //-------------------DDR---------------------
        ddrbuf=DDR2Buf(mainWnd->DDRBuf,mainWnd->DDRLen,&ddrlen);
        file_len=file_len+ddrlen;
        ((NORBOOT_NAND_HEAD *)m_fhead)->initSize=ddrlen;
        //-------------------------------------------
        *len=file_len;
    }


    lpBuffer = new char[file_len]; //read file to buffer
    memset(lpBuffer,0xff,file_len);

    CString mactmp;
    ((NORBOOT_NAND_HEAD *)m_fhead)->macaddr[7]=0;


    //	for(int i=0;i<6;i++)
    //	  ((NORBOOT_NAND_HEAD2 *)m_fhead)->macaddr[i]=i*10;


    wcstombs( m_fhead->name, (wchar_t *)m_imagename.GetBuffer(), 16);

    m_fhead->type=m_type;
    memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_NAND_HEAD));
    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));
#endif
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("Write SPI head error\n"));
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("ACK error !"));
        return FALSE;
    }
#if 1
    switch(m_type) {
    case DATA:
        if(m_spi_enc_check.GetCheck()!=TRUE)
            fread(lpBuffer,m_fhead->filelen,1,fp);
        else {
            CString szDirForKEY,KeyName;
            mainWnd->GetExeDir(szDirForKEY);
            szDirForKEY=szDirForKEY+_T("key_cfg\\");
            fclose(fp);
            m_combo_encrypt.GetLBText(m_combo_encrypt.GetCurSel(),KeyName);
            szDirForKEY=szDirForKEY+KeyName;
            if(encbuf!=NULL) {
                free(encbuf);
                encbuf=NULL;
            }
            encbuf=mainWnd->AES_Encrpt(szDirForKEY,m_pathName,&enclen);
            if(encbuf==NULL) {
                if(lpBuffer!=NULL) delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("AES encrpt failed or AES value equal to zero"));
                return FALSE;
            }
            memcpy(lpBuffer,encbuf,enclen);
        }
        break;
    case PACK:
        fread(lpBuffer,m_fhead->filelen,1,fp);
        break;
    case ENV:
#if 0
        fread(lpBuffer+4,file_len,1,fp);
#else
        {
            char line[256];
            char* ptr=(char *)(lpBuffer+4);
            while (1) {
                if (fgets(line,256, fp) == NULL) break;
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

        *(unsigned int *)lpBuffer=mainWnd->CalculateCRC32((unsigned char *)(lpBuffer+4),0x10000-4);
        *len=file_len=0x10000;
        if(mainWnd->envbuf!=NULL) free(mainWnd->envbuf);
        mainWnd->envbuf=(unsigned char *)malloc(0x10000);
        memcpy(mainWnd->envbuf,lpBuffer,0x10000);
        break;
    case UBOOT:
        memcpy(lpBuffer,ddrbuf,ddrlen);
        //------AES--------------------------------------------
        if(m_spi_enc_check.GetCheck()!=TRUE)
            fread(lpBuffer+ddrlen,m_fhead->filelen,1,fp);
        else {
            CString szDirForKEY,KeyName;
            mainWnd->GetExeDir(szDirForKEY);
            szDirForKEY=szDirForKEY+_T("key_cfg\\");
            fclose(fp);
            m_combo_encrypt.GetLBText(m_combo_encrypt.GetCurSel(),KeyName);
            szDirForKEY=szDirForKEY+KeyName;
            if(encbuf!=NULL) {
                free(encbuf);
                encbuf=NULL;
            }
            encbuf=mainWnd->AES_Encrpt(szDirForKEY,m_pathName,&enclen);
            if(encbuf==NULL) {
                if(lpBuffer!=NULL) delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("AES encrpt failed or AES value equal to zero"));
                return FALSE;
            }
            memcpy(lpBuffer+ddrlen,encbuf,enclen);
        }
        break;
    }
#else
    if(m_type==UBOOT) {
        memcpy(lpBuffer,ddrbuf,ddrlen);
        //------AES--------------------------------------------
        if(m_spi_enc_check.GetCheck()!=TRUE)
            fread(lpBuffer+ddrlen,m_fhead->filelen,1,fp);
        else {
            char *encbuf;
            unsigned int len;
            CString szDirForKEY,KeyName;
            mainWnd->GetExeDir(szDirForKEY);
            szDirForKEY=szDirForKEY+_T("key_cfg\\");
            fclose(fp);
            m_combo_encrypt.GetLBText(m_combo_encrypt.GetCurSel(),KeyName);
            szDirForKEY=szDirForKEY+KeyName;
            encbuf=mainWnd->AES_Encrpt(szDirForKEY,m_pathName,&len);
            memcpy(lpBuffer+ddrlen,encbuf,len);
        }
        //-----------------------------------------------------
    } else {
        fread(lpBuffer,m_fhead->filelen,1,fp);
    }
#endif

    m_progress.SetRange(0,100);
    scnt=file_len/BUF_SIZE;
    rcnt=file_len%BUF_SIZE;

    total=0;
    char *pbuf = lpBuffer;
    while(scnt>0) {
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf, BUF_SIZE);
        pbuf+=BUF_SIZE;
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            fclose(fp);
            return FALSE;
        }

        if(bResult==TRUE) {
            total+=BUF_SIZE;

            pos=(int)(((float)(((float)total/(float)file_len))*100));
            posstr.Format(_T("%d%%"),pos);

            DbgOut("SPI wait ack");
            bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
            DbgOut("SPI wait ack end");

            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("ACK error !"));
                return FALSE;
            }

        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            fclose(fp);
            return FALSE;
        }

        scnt--;

        if(pos%5==0) {
            PostMessage(WM_SPI_PROGRESS,(LPARAM)pos,0);
        }

    }

    if(rcnt>0) {
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf,rcnt);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            fclose(fp);
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }

        //printf("upload %%.2f\r",((float)total/file_len)*100);


        if(bResult==TRUE) {
            total+=rcnt;
            DbgOut("SPI wait ack");
            bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
            DbgOut("SPI wait ack end");

            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("ACK error !"));
                return FALSE;

            }

        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            fclose(fp);
            return FALSE;
        }

        pos=(int)(((float)(((float)total/(float)file_len))*100));

        if(pos>=100) {
            pos=100;
        }
        posstr.Format(_T("%d%%"),pos);

        if(pos%5==0) {

            PostMessage(WM_SPI_PROGRESS,(LPARAM)pos,0);
        }

    }

    delete []lpBuffer;
    fclose(fp);
//burn progress...
    burn_pos=0;
    PostMessage(WM_SPI_PROGRESS,(LPARAM)0,0);
    m_progress.SetBkColor(COLOR_BURN);

    while(burn_pos!=100) {
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            return FALSE;
        }

        DbgOut("SPI wait burn ack");
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
        if(bResult==FALSE) {
            NucUsb.NUC_CloseHandle();
            AfxMessageBox(_T("ACK error !"));
            return FALSE;
        }
        DbgOut("SPI wait burn ack end");
        if(!((ack>>16)&0xffff)) {
            burn_pos=(UCHAR)(ack&0xffff);
            PostMessage(WM_SPI_PROGRESS,(LPARAM)burn_pos,0);
        } else {
            NucUsb.NUC_CloseHandle();
            AfxMessageBox(_T("Burn error"));
            return FALSE;
        }

    }
    NucUsb.NUC_CloseHandle();
    return TRUE;

}

//SPI Verify
BOOL CSPIDlg::XUSB_Verify(CString& portName,CString& m_pathName)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    FILE* fp;
    int pos=0;
    //NORBOOT_NAND_HEAD fhead;
    unsigned int total=0,file_len,scnt,rcnt,ack;
    char* lpBuffer;
    char temp[BUF_SIZE];
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    PostMessage(WM_SPI_PROGRESS,(LPARAM)0,0);
    m_progress.SetBkColor(COLOR_VERIFY);

    /***********************************************/
    bResult = NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack;
    bResult=NucUsb.NUC_SetType(0,SPI,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }

    lpBuffer = new char[BUF_SIZE];
    fp=_wfopen(m_pathName,_T("rb"));

    if(!fp) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }

    if(m_type!=ENV) {
        if(m_spi_enc_check.GetCheck()!=TRUE) {
            fseek(fp,0,SEEK_END);
            file_len=ftell(fp);
            fseek(fp,0,SEEK_SET);
        } else
            file_len=enclen;
    } else {
        file_len=0x10000;
    }

    if(!file_len) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("File length is zero\n"));
        return FALSE;
    }

    m_fhead->flag=VERIFY_ACTION;

    memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_NAND_HEAD));
    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));

    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("Write SPI head error\n"));
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("ACK error !"));
        return FALSE;
    }

	m_progress.SetRange(0,100);
    scnt=file_len/BUF_SIZE;
    rcnt=file_len%BUF_SIZE;

    total=0;
    while(scnt>0) {

        if(m_type!=ENV) {
            if(m_spi_enc_check.GetCheck()!=TRUE)
                fread(temp,BUF_SIZE,1,fp);
            else {
                memcpy(temp,encbuf+total,BUF_SIZE);
            }
        } else {
            memcpy(temp,mainWnd->envbuf+total,BUF_SIZE);
        }
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)lpBuffer, BUF_SIZE);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            fclose(fp);
            return FALSE;
        }

        if(bResult==TRUE) {
            total+=BUF_SIZE;

            pos=(int)(((float)(((float)total/(float)file_len))*100));
            posstr.Format(_T("%d%%"),pos);

            if(DataCompare(temp,lpBuffer,BUF_SIZE))
                ack=BUF_SIZE;
            else
                ack=0;//compare error
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)&ack,4);
            if((bResult==FALSE)||(!ack)) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                return FALSE;
            }

        } else {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            fclose(fp);
            return FALSE;
        }

        scnt--;

        if(pos%5==0) {
            PostMessage(WM_SPI_PROGRESS,(LPARAM)pos,0);
        }


    }

    if(rcnt>0) {
        if(m_type!=ENV) {
            if(m_spi_enc_check.GetCheck()!=TRUE)
                fread(temp,rcnt,1,fp);
            else {
                memcpy(temp,encbuf+total,rcnt);
            }
        } else {
            memcpy(temp,mainWnd->envbuf+total,rcnt);
        }
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)lpBuffer,BUF_SIZE);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            fclose(fp);
            return FALSE;
        }

        if(bResult==TRUE) {
            total+=rcnt;
            if(DataCompare(temp,lpBuffer,rcnt))
                ack=BUF_SIZE;
            else
                ack=0;//compare error
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)&ack, 4);
            if((bResult==FALSE)||(!ack)) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                return FALSE;

            }

        } else {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            fclose(fp);
            return FALSE;
        }

        pos=(int)(((float)(((float)total/(float)file_len))*100));

        if(pos>=100) {
            pos=100;
        }
        posstr.Format(_T("%d%%"),pos);

        if(pos%5==0) {
            PostMessage(WM_SPI_PROGRESS,(LPARAM)pos,0);
        }

    }

    delete []lpBuffer;
    fclose(fp);
    NucUsb.NUC_CloseHandle();
    return TRUE;

}

// xusb read
BOOL CSPIDlg::XUSB_Read(CString& portName,CString& m_pathName,unsigned int addr,unsigned int len)
//BOOL CSPIDlg::XUSB_Read(CString& portName,CString& m_pathName)
{
#if 1
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    int pos=0;
    //NORBOOT_NAND_HEAD fhead;
    unsigned int total=0,scnt,rcnt,ack;
    char* lpBuffer;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    PostMessage(WM_SPI_PROGRESS,(LPARAM)0,0);
    m_progress.SetBkColor(COLOR_VERIFY);

    /***********************************************/
    bResult = NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack;
    bResult=NucUsb.NUC_SetType(0,SPI,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }

    lpBuffer = new char[BUF_SIZE];
    FILE* tempfp;
    //-----------------------------------
    tempfp=_wfopen(m_pathName,_T("w+b"));
    //-----------------------------------
    if(!tempfp) {
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }

    m_fhead->flag=READ_ACTION;
    m_fhead->flashoffset=addr;
    m_fhead->filelen=len;
    memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_NAND_HEAD));
    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));

    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("Write SPI head error\n"));
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("ACK error !"));
        return FALSE;
    }

	m_progress.SetRange(0,100);
    scnt=len/BUF_SIZE;
    rcnt=len%BUF_SIZE;
    total=0;
    while(scnt>0) {
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)lpBuffer,BUF_SIZE);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            return FALSE;
        }
        if(bResult==TRUE) {
            total+=BUF_SIZE;
            pos=(int)(((float)(((float)total/(float)len))*100));
            posstr.Format(_T("%d%%"),pos);
            fwrite(lpBuffer,BUF_SIZE,1,tempfp);
            ack=BUF_SIZE;
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)&ack,4);
            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }
        } else {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            return FALSE;
        }

        scnt--;
        if(pos%5==0) {
            PostMessage(WM_SPI_PROGRESS,(LPARAM)pos,0);
        }
    }

    if(rcnt>0) {
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)lpBuffer,BUF_SIZE);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            return FALSE;
        }
        if(bResult==TRUE) {
            total+=rcnt;
            fwrite(lpBuffer,rcnt,1,tempfp);
            ack=BUF_SIZE;
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)&ack,4);
            if(bResult!=TRUE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }
        } else {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            return FALSE;
        }
        pos=(int)(((float)(((float)total/(float)len))*100));
        if(pos>=100) {
            pos=100;
        }
        posstr.Format(_T("%d%%"),pos);
        if(pos%5==0) {
            PostMessage(WM_SPI_PROGRESS,(LPARAM)pos,0);
        }
    }
    delete []lpBuffer;
    NucUsb.NUC_CloseHandle();
    fclose(tempfp);
    /********************************************/
    return TRUE;

#else //org

    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    int pos=0;
    //SDRAM_RAW_TYPEHEAD fhead;
    unsigned int total,file_len,scnt,rcnt,ack;
    char* lpBuffer;

    PostMessage(WM_SPI_PROGRESS,(LPARAM)0,0);
    m_progress.SetBkColor(COLOR_DOWNLOAD);
    /***********************************************/
    bResult = NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack;
    bResult=NucUsb.NUC_SetType(0,RAWSPI,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }
    lpBuffer = new char[BUF_SIZE];

    FILE* tempfp;
    //-----------------------------------
    tempfp=_wfopen(m_pathName,_T("w+b"));
    //-----------------------------------
    if(!tempfp) {
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }
    //--------------------
    int nblock=_wtoi(m_blocks);
    file_len=nblock*64*1024;
    //---------------------
    //file_len=2*1024*1024;
    m_frawhead.flag=VERIFY_ACTION;
    m_frawhead.filelen=file_len;
    m_frawhead.address=0;
    memcpy(lpBuffer,(unsigned char*)&m_frawhead,sizeof(SDRAM_RAW_TYPEHEAD));
    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(SDRAM_RAW_TYPEHEAD));
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("Write RAW NOR head error\n"));
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("ACK error !"));
        return FALSE;
    }
    scnt=file_len/BUF_SIZE;
    rcnt=file_len%BUF_SIZE;
    total=0;
    while(scnt>0) {
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)lpBuffer,BUF_SIZE);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            return FALSE;
        }
        if(bResult==TRUE) {
            total+=BUF_SIZE;
            pos=(int)(((float)(((float)total/(float)file_len))*100));
            posstr.Format(_T("%d%%"),pos);
            fwrite(lpBuffer,BUF_SIZE,1,tempfp);
            ack=BUF_SIZE;
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)&ack,4);
            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }
        } else {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            return FALSE;
        }

        scnt--;
        if(pos%5==0) {
            PostMessage(WM_SPI_PROGRESS,(LPARAM)pos,0);
        }
    }

    if(rcnt>0) {
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)lpBuffer,BUF_SIZE);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            return FALSE;
        }
        if(bResult==TRUE) {
            total+=rcnt;
            fwrite(lpBuffer,rcnt,1,tempfp);
            ack=BUF_SIZE;
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)&ack,4);
            if(bResult!=4) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }
        } else {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            return FALSE;
        }
        pos=(int)(((float)(((float)total/(float)file_len))*100));
        if(pos>=100) {
            pos=100;
        }
        posstr.Format(_T("%d%%"),pos);
        if(pos%5==0) {
            PostMessage(WM_SPI_PROGRESS,(LPARAM)pos,0);
        }
    }
    delete []lpBuffer;
    NucUsb.NUC_CloseHandle();
    fclose(tempfp);
    /********************************************/
    return TRUE;
#endif
}

BOOL CSPIDlg::XUSB_Erase(CString& portName)
{
    BOOL bResult;
    CString tempstr;
    int count=0;
    unsigned int ack,erase_pos=0;
    NORBOOT_NAND_HEAD *fhead;
    char* lpBuffer;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    fhead=(NORBOOT_NAND_HEAD *)malloc(sizeof(NORBOOT_NAND_HEAD));

    /***********************************************/
    bResult = NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        free(fhead);
        return FALSE;
    }
#endif
    USHORT typeack;
    bResult=NucUsb.NUC_SetType(0,SPI,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        free(fhead);
        return FALSE;
    }

    lpBuffer = new char[BUF_SIZE];

    memset((unsigned char*)fhead,0,sizeof(NORBOOT_NAND_HEAD));
    fhead->flag=ERASE_ACTION;
    fhead->flashoffset = _wtoi(m_sblocks); //start erase block
    fhead->execaddr=_wtoi(m_blocks);  //erase block length
    fhead->type=m_erase_flag; // Decide chip erase mode or erase mode
    if(m_erase_flag==0)
        fhead->no=0xffffffff;//erase all
    else
        fhead->no=0x0;//erase all
    memcpy(lpBuffer,(unsigned char*)fhead,sizeof(NORBOOT_NAND_HEAD));
    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));
    free(fhead);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("Write SPI head error\n"));
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE || ack!=0x85) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        CString msg;
        msg.Format(_T("ACK error 0x%08x\n"),ack);
        AfxMessageBox(msg);
        return FALSE;
    }

    m_progress.SetRange(0,100);
    erase_pos=0;
    int wait_pos=0;
    while(erase_pos!=100) {
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            AfxMessageBox(_T("WaitForSingleObject error !"));
            return FALSE;
        }

        DbgOut("SPI wait erase ack");
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
        if(bResult=FALSE) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            AfxMessageBox(_T("ACK error !"));
            return FALSE;
        }

        DbgOut("SPI wait erase ack end");
        if(!((ack>>16)&0xffff)) {
            erase_pos=ack&0xffff;
            PostMessage(WM_SPI_PROGRESS,(LPARAM)erase_pos,0);
        } else {

            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            CString msg;
            msg.Format(_T("Erase error ack=0x%08x\n"),ack);
            AfxMessageBox(msg);
            return FALSE;
        }

        if(erase_pos==95) {
            wait_pos++;
            if(wait_pos>100) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                AfxMessageBox(_T("Erase error"));
                return FALSE;
            }

        }

    }


    delete []lpBuffer;
    NucUsb.NUC_CloseHandle();
    return TRUE;

}
/************** SPI End ************/

/************** MMC Begin ************/
BOOL CMMCDlg::XUSB_Pack(CString& portName,CString& m_pathName,int *len)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    FILE* fp;
    int pos=0;
    UCHAR burn_pos=0;
    unsigned int total,file_len,scnt,rcnt;
    ULONG ack;
    //NORBOOT_NAND_HEAD fhead;
    char* lpBuffer;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    bResult = NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack=0x0;
    bResult=NucUsb.NUC_SetType(0,MMC,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }


    fp=_wfopen(m_pathName,_T("rb"));

    if(!fp) {
        //delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }

    fseek(fp,0,SEEK_END);
    file_len=ftell(fp);
    fseek(fp,0,SEEK_SET);

    if(!file_len) {
        //delete []lpBuffer;
        //NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("File length is zero\n"));
        return FALSE;
    }

    unsigned int magic;
    fread((unsigned char *)&magic,4,1,fp);
    if(magic!=0x5) {
        //delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("Pack Image Format Error\n"));
        return FALSE;
    }
    fseek(fp,0,SEEK_SET);

    lpBuffer = new char[file_len]; //read file to buffer
    memset(lpBuffer,0x00,file_len);

    memset((unsigned char*)m_fhead,0,sizeof(NORBOOT_MMC_HEAD));
    total=0;
    m_fhead->flag=PACK_ACTION;
    m_fhead->type=m_type;
    m_fhead->filelen=file_len;
    memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_MMC_HEAD));
    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_MMC_HEAD));

    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("Write eMMC head error\n"));
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("ACK error !"));
        return FALSE;
    }

    fread(lpBuffer,m_fhead->filelen,1,fp);
    if(lpBuffer[0]!=0x5) {
        delete []lpBuffer;
        //NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("This file is not pack image"));
    }
    fclose(fp);

	// Check DDR *ini
	bResult=CheckDDRiniData(lpBuffer, m_fhead->filelen);
	if(bResult==FALSE) {
		AfxMessageBox(_T("DDR Init select error\n"));
		delete []lpBuffer;
		//NucUsb.NUC_CloseHandle();
		return FALSE;
    }

    char *pbuf = lpBuffer;
    PACK_HEAD *ppackhead=(PACK_HEAD *)lpBuffer;
    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf, sizeof(PACK_HEAD));
    Sleep(5);
    if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT)	return FALSE;
    if(bResult!=TRUE) return FALSE;
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }
    total+= sizeof(PACK_HEAD);
    pbuf+= sizeof(PACK_HEAD);

    PACK_CHILD_HEAD child;
    m_progress.SetRange(0,short(ppackhead->num*100));
    int posnum=0;
#if !defined(BATCH_BURN)
    for(int i=0; i<(int)(ppackhead->num); i++) {
        memcpy(&child,(char *)pbuf,sizeof(PACK_CHILD_HEAD));
        //Sleep(20);
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf, sizeof(PACK_CHILD_HEAD));
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT)	return FALSE;
        if(bResult!=TRUE) return FALSE;
        total+= sizeof(PACK_CHILD_HEAD);
        pbuf+= sizeof(PACK_CHILD_HEAD);

        scnt=child.filelen/BUF_SIZE;
        rcnt=child.filelen%BUF_SIZE;

        while(scnt>0) {
            //Sleep(20);
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf, BUF_SIZE);
            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                //fclose(fp);
                delete []lpBuffer;
                return FALSE;
            }

            if(bResult==TRUE) {
                pbuf+=BUF_SIZE;
                total+=BUF_SIZE;

                pos=(int)(((float)(((float)total/(float)file_len))*100));
                posstr.Format(_T("%d%%"),pos);

                DbgOut("eMMC wait ack");
                bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
                DbgOut("eMMC wait ack end");

                if(bResult==FALSE) {
                    delete []lpBuffer;
                    NucUsb.NUC_CloseHandle();
                    //fclose(fp);
                    AfxMessageBox(_T("ACK error !"));
                    return FALSE;
                }

            }

            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }

            scnt--;

            if(pos%5==0) {
                PostMessage(WM_MMC_PROGRESS,(LPARAM)(posnum+pos),0);
            }

        }

        if(rcnt>0) {
            //Sleep(20);
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf,rcnt);
            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }

            //printf("upload %%.2f\r",((float)total/file_len)*100);
            if(bResult==TRUE) {
                pbuf+=rcnt;
                total+=rcnt;
                DbgOut("eMMC wait ack");
                bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
                DbgOut("eMMC wait ack end");

                if(bResult==FALSE) {
                    delete []lpBuffer;
                    NucUsb.NUC_CloseHandle();
                    //fclose(fp);
                    AfxMessageBox(_T("ACK error !"));
                    return FALSE;

                }

            }

            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                //fclose(fp);
                return FALSE;
            }

            pos=(int)(((float)(((float)total/(float)file_len))*100));

            if(pos>=100) {
                pos=100;
            }
            posstr.Format(_T("%d%%"),pos);

            if(pos%5==0) {

                PostMessage(WM_MMC_PROGRESS,(LPARAM)(posnum+pos),0);
            }

        }

        //fclose(fp);
        //burn progress...
        burn_pos=0;
        //PostMessage(WM_MMC_PROGRESS,(LPARAM)0,0);
        m_progress.SetBkColor(COLOR_BURN);
        posnum+=100;
        while(burn_pos!=100) {
            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                return FALSE;
            }

            DbgOut("eMMC wait burn ack");
            bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
            if(bResult==FALSE) {
                NucUsb.NUC_CloseHandle();
                AfxMessageBox(_T("ACK error !"));
                return FALSE;
            }
            DbgOut("eMMC wait burn ack end");
            if(!((ack>>16)&0xffff)) {
                burn_pos=(UCHAR)(ack&0xffff);
                PostMessage(WM_MMC_PROGRESS,(LPARAM)(posnum+burn_pos),0);
            } else {
                NucUsb.NUC_CloseHandle();
                AfxMessageBox(_T("Burn error"));
                return FALSE;
            }
        }
        posnum+=100;
    }
#else
    unsigned int blockNum;
    int prepos=0;
    for(int i=0; i<(int)(ppackhead->num); i++) {
        memcpy(&child,(char *)pbuf,sizeof(PACK_CHILD_HEAD));
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf, sizeof(PACK_CHILD_HEAD));
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT)	return FALSE;
        if(bResult==FALSE) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }

        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
        if(bResult==FALSE) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }
        //total+= sizeof(PACK_CHILD_HEAD);
        pbuf+= sizeof(PACK_CHILD_HEAD);
        //Sleep(1);
        scnt=child.filelen/BUF_SIZE;
        rcnt=child.filelen%BUF_SIZE;
        total=0;
        while(scnt>0) {
            //Sleep(20);
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf, BUF_SIZE);
            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                //fclose(fp);
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }

            if(bResult==TRUE) {
                pbuf+=BUF_SIZE;
                total+=BUF_SIZE;

                pos=(int)(((float)(((float)total/(float)child.filelen))*100));
                posstr.Format(_T("%d%%"),pos);

                DbgOut("eMMC wait ack");
                bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
                DbgOut("eMMC wait ack end");

                if(bResult==FALSE) {
                    delete []lpBuffer;
                    NucUsb.NUC_CloseHandle();
                    //fclose(fp);
                    CString tmp;
                    tmp.Format(_T("ACK error %d!"),i);
                    AfxMessageBox(tmp);
                    return FALSE;
                }

            }

            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }

            scnt--;

            if((pos!=prepos) || (pos==100)) {
                prepos=pos;
                PostMessage(WM_MMC_PROGRESS,(LPARAM)(posnum+pos),0);
            }

        }

        if(rcnt>0) {
            //Sleep(20);
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf,rcnt);
            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }

            //printf("upload %%.2f\r",((float)total/file_len)*100);
            if(bResult==TRUE) {
                pbuf+=rcnt;
                total+=rcnt;
                DbgOut("NAND wait ack");
                bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
                DbgOut("NAND wait ack end");

                if(bResult==FALSE) {
                    delete []lpBuffer;
                    NucUsb.NUC_CloseHandle();
                    //fclose(fp);
                    CString tmp;
                    tmp.Format(_T("ACK error(rcnt>0) %d!"),i);
                    AfxMessageBox(tmp);
                    return FALSE;

                }

            }

            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                //fclose(fp);
                return FALSE;
            }

            pos=(int)(((float)(((float)total/(float)child.filelen))*100));

            if(pos>=100) {
                pos=100;
            }
            posstr.Format(_T("%d%%"),pos);

            if((pos!=prepos)||pos==100) {
                prepos=pos;
                PostMessage(WM_MMC_PROGRESS,(LPARAM)(posnum+pos),0);
            }

        }
        posnum+=100;

        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&blockNum,4);
        if(bResult==FALSE) {
            NucUsb.NUC_CloseHandle();
            AfxMessageBox(_T("read block error !"));
            return FALSE;
        }
    }//for(int i=0;i<(int)(ppackhead->num);i++)
#endif
    delete []lpBuffer;
    NucUsb.NUC_CloseHandle();
    return TRUE;

}

BOOL CMMCDlg::XUSB_Burn(CString& portName,CString& m_pathName,int *len)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    FILE* fp;
    int pos=0;
    UCHAR burn_pos=0;
    unsigned int total,file_len,scnt,rcnt;
    ULONG ack;
    //NORBOOT_NAND_HEAD fhead;
    char* lpBuffer;
    unsigned char *ddrbuf;
    int ddrlen;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    m_progress.SetRange(0,100);
    bResult = NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack=0x0;
    bResult=NucUsb.NUC_SetType(0,MMC,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }


    fp=_wfopen(m_pathName,_T("rb"));

    if(!fp) {
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }

    fseek(fp,0,SEEK_END);
    file_len=ftell(fp);
    fseek(fp,0,SEEK_SET);

    if(!file_len) {
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("File length is zero\n"));
        return FALSE;
    }


    memset((unsigned char*)m_fhead,0,sizeof(NORBOOT_MMC_HEAD));

    m_fhead->flag=WRITE_ACTION;
    ((NORBOOT_MMC_HEAD *)m_fhead)->initSize=0;

    //check AES------------------------------
    if(m_mmc_enc_check.GetCheck()==TRUE)
        file_len=((file_len+15)/16)*16;
    m_fhead->filelen=file_len;
#if 1
    switch(m_type) {
    case DATA:
    case PACK:
        swscanf_s(_T("0"),_T("%d"),&m_fhead->no);
        swscanf_s(m_execaddr,_T("%x"),&m_fhead->execaddr);
        swscanf_s(m_startblock,_T("%x"),&m_fhead->flashoffset);
        *len=file_len;
        lpBuffer = new char[file_len+sizeof(NORBOOT_MMC_HEAD)]; //read file to buffer
        memset(lpBuffer,0xff,file_len+sizeof(NORBOOT_MMC_HEAD));
        ((NORBOOT_MMC_HEAD *)m_fhead)->macaddr[7]=0;
        wcstombs( m_fhead->name, (wchar_t *)m_imagename.GetBuffer(), 16);
        m_fhead->type=m_type;
        memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_MMC_HEAD));
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_MMC_HEAD));
        break;
    case ENV:
        swscanf_s(_T("0"),_T("%d"),&m_fhead->no);
        swscanf_s(m_execaddr,_T("%x"),&m_fhead->execaddr);
        swscanf_s(m_startblock,_T("%x"),&m_fhead->flashoffset);
        if(file_len>(0x10000-4)) {
            //delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            AfxMessageBox(_T("The environment file size is less then 64KB\n"));
            return FALSE;
        }
        lpBuffer = new char[0x10000]; //read file to buffer
        memset(lpBuffer,0x00,0x10000);

        ((NORBOOT_NAND_HEAD *)m_fhead)->macaddr[7]=0;

        wcstombs( m_fhead->name, (wchar_t *)m_imagename.GetBuffer(), 16);
        m_fhead->filelen=0x10000;
        m_fhead->type=m_type;
        memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_MMC_HEAD));
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_MMC_HEAD));
        break;
    case UBOOT:
        swscanf_s(_T("1"),_T("%d"),&m_fhead->no);
        swscanf_s(m_execaddr,_T("%x"),&m_fhead->execaddr);
        swscanf_s(m_startblock,_T("%x"),&m_fhead->flashoffset);
        //-------------------DDR---------------------
        //ddrbuf=DDR2Buf(mainWnd->DDRBuf,mainWnd->DDRLen,&ddrlen);
        ddrbuf=DDR2Buf(mainWnd->ShareDDRBuf,mainWnd->DDRLen,&ddrlen);
		TRACE(_T("ddrlen=0x%x(%d)\n"), ddrlen, ddrlen);
        file_len=file_len+ddrlen+16;
        ((NORBOOT_MMC_HEAD *)m_fhead)->initSize=ddrlen;
        //-------------------------------------------
        *len=file_len;
        lpBuffer = new char[file_len]; //read file to buffer
        memset(lpBuffer,0xff,file_len);
        ((NORBOOT_MMC_HEAD *)m_fhead)->macaddr[7]=0;


        wcstombs( m_fhead->name, (wchar_t *)m_imagename.GetBuffer(), 16);

        m_fhead->type=m_type;
        memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_MMC_HEAD));
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_MMC_HEAD));
        break;
    }
#else
    if(m_type!=UBOOT) {
        swscanf_s(_T("0"),_T("%d"),&m_fhead->no);
        swscanf_s(m_execaddr,_T("%x"),&m_fhead->execaddr);
        swscanf_s(m_startblock,_T("%x"),&m_fhead->flashoffset);
        *len=file_len;
    } else {
        swscanf_s(_T("1"),_T("%d"),&m_fhead->no);
        swscanf_s(m_execaddr,_T("%x"),&m_fhead->execaddr);
        swscanf_s(m_startblock,_T("%x"),&m_fhead->flashoffset);
        //-------------------DDR---------------------
        ddrbuf=DDR2Buf(mainWnd->DDRBuf,mainWnd->DDRLen,&ddrlen);
        file_len=file_len+ddrlen;
        ((NORBOOT_MMC_HEAD *)m_fhead)->initSize=ddrlen;
        //-------------------------------------------
        *len=file_len;
    }

    lpBuffer = new char[file_len]; //read file to buffer
    memset(lpBuffer,0x00,file_len);
    CString mactmp;
    ((NORBOOT_MMC_HEAD *)m_fhead)->macaddr[7]=0;


    //	for(int i=0;i<6;i++)
    //	  ((NORBOOT_NAND_HEAD2 *)m_fhead)->macaddr[i]=i*10;


    wcstombs( m_fhead->name, (wchar_t *)m_imagename.GetBuffer(), 16);

    m_fhead->type=m_type;
    memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_MMC_HEAD));
    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_MMC_HEAD));
#endif
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("Write eMMC head error\n"));
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("ACK error !"));
        return FALSE;
    }
#if 1
    switch(m_type) {
    case DATA:
        if(m_mmc_enc_check.GetCheck()!=TRUE)
            fread(lpBuffer,m_fhead->filelen,1,fp);
        else {
            CString szDirForKEY,KeyName;
            mainWnd->GetExeDir(szDirForKEY);
            szDirForKEY=szDirForKEY+_T("key_cfg\\");
            fclose(fp);
            m_combo_encrypt.GetLBText(m_combo_encrypt.GetCurSel(),KeyName);
            szDirForKEY=szDirForKEY+KeyName;
            if(encbuf!=NULL) {
                free(encbuf);
                encbuf=NULL;
            }
            encbuf=mainWnd->AES_Encrpt(szDirForKEY,m_pathName,&enclen);
            if(encbuf==NULL) {
                if(lpBuffer!=NULL) delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("AES encrpt failed or AES value equal to zero"));
                return FALSE;
            }
            memcpy(lpBuffer,encbuf,enclen);
        }
        break;
    case PACK:
        fread(lpBuffer,m_fhead->filelen,1,fp);
        break;
    case ENV:
#if 0
        fread(lpBuffer+4,file_len,1,fp);
#else
        {
            char line[256];
            char* ptr=(char *)(lpBuffer+4);
            while (1) {
                if (fgets(line,256, fp) == NULL) break;
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

        *(unsigned int *)lpBuffer=mainWnd->CalculateCRC32((unsigned char *)(lpBuffer+4),0x10000-4);
        *len=file_len=0x10000;
        if(mainWnd->envbuf!=NULL) free(mainWnd->envbuf);
        mainWnd->envbuf=(unsigned char *)malloc(0x10000);
        memcpy(mainWnd->envbuf,lpBuffer,0x10000);
        break;
    case UBOOT:
        memcpy(lpBuffer,ddrbuf,ddrlen);
        //------AES--------------------------------------------
        if(m_mmc_enc_check.GetCheck()!=TRUE)
            fread(lpBuffer+ddrlen,m_fhead->filelen,1,fp);
        else {
            char *encbuf;
            unsigned int len;
            CString szDirForKEY,KeyName;
            mainWnd->GetExeDir(szDirForKEY);
            szDirForKEY=szDirForKEY+_T("key_cfg\\");
            fclose(fp);
            m_combo_encrypt.GetLBText(m_combo_encrypt.GetCurSel(),KeyName);
            szDirForKEY=szDirForKEY+KeyName;
            encbuf=mainWnd->AES_Encrpt(szDirForKEY,m_pathName,&len);
            if(encbuf==NULL) {
                if(lpBuffer!=NULL) delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("AES encrpt failed or AES value equal to zero"));
                return FALSE;
            }
            memcpy(lpBuffer+ddrlen,encbuf,len);
        }
        break;
    }
#else
    if(m_type==UBOOT) {
        memcpy(lpBuffer,ddrbuf,ddrlen);
        //------AES--------------------------------------------
        if(m_mmc_enc_check.GetCheck()!=TRUE)
            fread(lpBuffer+ddrlen,m_fhead->filelen,1,fp);
        else {
            char *encbuf;
            unsigned int len;
            CString szDirForKEY,KeyName;
            mainWnd->GetExeDir(szDirForKEY);
            szDirForKEY=szDirForKEY+_T("key_cfg\\");
            fclose(fp);
            m_combo_encrypt.GetLBText(m_combo_encrypt.GetCurSel(),KeyName);
            szDirForKEY=szDirForKEY+KeyName;
            encbuf=mainWnd->AES_Encrpt(szDirForKEY,m_pathName,&len);
            memcpy(lpBuffer+ddrlen,encbuf,len);
        }
        //-----------------------------------------------------
    } else {
        fread(lpBuffer,m_fhead->filelen,1,fp);
    }
#endif
    scnt=file_len/BUF_SIZE;
    rcnt=file_len%BUF_SIZE;

#if !defined(BATCH_BURN)
    total=0;
    char *pbuf = lpBuffer;
    while(scnt>0) {
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf, BUF_SIZE);
        pbuf+=BUF_SIZE;
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            fclose(fp);
            return FALSE;
        }

        if(bResult==TRUE) {
            total+=BUF_SIZE;

            pos=(int)(((float)(((float)total/(float)file_len))*100));
            posstr.Format(_T("%d%%"),pos);

            DbgOut("eMMC wait ack");
            bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
            DbgOut("eMMC wait ack end");

            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("ACK error !"));
                return FALSE;
            }

        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            fclose(fp);
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }

        scnt--;

        if(pos%5==0) {
            PostMessage(WM_MMC_PROGRESS,(LPARAM)pos,0);
        }

    }

    if(rcnt>0) {
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf,rcnt);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            fclose(fp);
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }

        //printf("upload %%.2f\r",((float)total/file_len)*100);


        if(bResult==TRUE) {
            total+=rcnt;
            DbgOut("eMMC wait ack");
            bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
            DbgOut("eMMC wait ack end");

            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("ACK error !"));
                return FALSE;

            }

        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            fclose(fp);
            return FALSE;
        }

        pos=(int)(((float)(((float)total/(float)file_len))*100));

        if(pos>=100) {
            pos=100;
        }
        posstr.Format(_T("%d%%"),pos);

        if(pos%5==0) {

            PostMessage(WM_MMC_PROGRESS,(LPARAM)pos,0);
        }

    }

    delete []lpBuffer;
    fclose(fp);
//burn progress...
    burn_pos=0;
    PostMessage(WM_MMC_PROGRESS,(LPARAM)0,0);
    m_progress.SetBkColor(COLOR_BURN);

    while(burn_pos!=100) {
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            return FALSE;
        }

        DbgOut("eMMC wait burn ack");
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
        if(bResult==FALSE) {
            NucUsb.NUC_CloseHandle();
            AfxMessageBox(_T("ACK error !"));
            return FALSE;
        }
        DbgOut("eMMC wait burn ack end");
        if(!((ack>>16)&0xffff)) {
            burn_pos=(UCHAR)(ack&0xffff);
            PostMessage(WM_MMC_PROGRESS,(LPARAM)burn_pos,0);
        } else {
            NucUsb.NUC_CloseHandle();
            AfxMessageBox(_T("Burn error"));
            return FALSE;
        }

    }
#else
    total=0;
    char *pbuf = lpBuffer;
    int prepos=0;
    while(scnt>0) {
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf, BUF_SIZE);
        pbuf+=BUF_SIZE;
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            //fclose(fp);
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }

        if(bResult==TRUE) {
            total+=BUF_SIZE;

            pos=(int)(((float)(((float)total/(float)file_len))*100));
            posstr.Format(_T("%d%%"),pos);

            DbgOut("eMMC wait ack");
            bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
            DbgOut("eMMC wait ack end");

            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("ACK error !"));
                return FALSE;
            }

        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            //fclose(fp);
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
        }

        scnt--;

        if((pos!=prepos) || pos==100) {
            prepos=pos;
            PostMessage(WM_MMC_PROGRESS,(LPARAM)pos,0);
        }

    }

    if(rcnt>0) {
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf,rcnt);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            //fclose(fp);
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }

        //printf("upload %%.2f\r",((float)total/file_len)*100);


        if(bResult==TRUE) {
            total+=rcnt;
            DbgOut("eMMC wait ack");
            bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
            DbgOut("eMMC wait ack end");

            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                //fclose(fp);
                AfxMessageBox(_T("ACK error !"));
                return FALSE;

            }

        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            //fclose(fp);
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }

        pos=(int)(((float)(((float)total/(float)file_len))*100));

        if(pos>=100) {
            pos=100;
        }
        posstr.Format(_T("%d%%"),pos);

        if(pos%5==0) {

            PostMessage(WM_MMC_PROGRESS,(LPARAM)pos,0);
        }

    }

    delete []lpBuffer;
    fclose(fp);

    unsigned int blockNum;
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&blockNum,4);
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("read block error !"));
        return FALSE;
    }
#endif
    NucUsb.NUC_CloseHandle();
    return TRUE;
}

BOOL CMMCDlg::XUSB_Verify(CString& portName,CString& m_pathName)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    FILE* fp;
    int pos=0;
    unsigned int total=0,file_len,scnt,rcnt,ack;
    char* lpBuffer;
    char temp[BUF_SIZE];
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    PostMessage(WM_MMC_PROGRESS,(LPARAM)0,0);
    m_progress.SetBkColor(COLOR_VERIFY);

    /***********************************************/
    bResult = NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack;
    bResult=NucUsb.NUC_SetType(0,MMC,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }

    lpBuffer = new char[BUF_SIZE];
    fp=_wfopen(m_pathName,_T("rb"));

    if(!fp) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }
    if(m_type!=ENV) {
        if(m_mmc_enc_check.GetCheck()!=TRUE) {
            fseek(fp,0,SEEK_END);
            file_len=ftell(fp);
            fseek(fp,0,SEEK_SET);
        } else
            file_len=enclen;
    } else
        file_len=0x10000;

    if(!file_len) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("File length is zero\n"));
        return FALSE;
    }

    m_fhead->flag=VERIFY_ACTION;

    memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_MMC_HEAD));
    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_MMC_HEAD));
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("Write MMC head error\n"));
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("ACK error !"));
        return FALSE;
    }

    scnt=(file_len)/BUF_SIZE;
    rcnt=(file_len)%BUF_SIZE;

    total=0;
    int propos=0;
    while(scnt>0) {
        if(m_type!=ENV) {
            if(m_mmc_enc_check.GetCheck()!=TRUE)
                fread(temp,BUF_SIZE,1,fp);
            else {
                memcpy(temp,encbuf+total,BUF_SIZE);
            }
        } else {
            memcpy(temp,mainWnd->envbuf+total,BUF_SIZE);
        }
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)lpBuffer, BUF_SIZE);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        if(bResult==TRUE) {

            total+=BUF_SIZE;

            pos=(int)(((float)(((float)total/(float)file_len))*100));
            posstr.Format(_T("%d%%"),pos);

            if(DataCompare(temp,lpBuffer,BUF_SIZE))
                ack=BUF_SIZE;
            else
                ack=0;//compare error
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)&ack,4);
            if((bResult==FALSE)||(!ack)) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                return FALSE;
            }

        } else {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            fclose(fp);
            return FALSE;
        }

        scnt--;

        if(pos!=propos || pos==100) {
            propos=pos;
            PostMessage(WM_MMC_PROGRESS,(LPARAM)pos,0);
        }


    }

    if(rcnt>0) {
        if(m_type!=ENV) {
            if(m_mmc_enc_check.GetCheck()!=TRUE)
                fread(temp,rcnt,1,fp);
            else {
                memcpy(temp,encbuf+total,rcnt);
            }
        } else {
            memcpy(temp,mainWnd->envbuf+total,rcnt);
        }
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)lpBuffer,BUF_SIZE);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        if(bResult==TRUE) {
            total+=rcnt;
            if(DataCompare(temp,lpBuffer,rcnt))
                ack=BUF_SIZE;
            else
                ack=0;//compare error
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)&ack, 4);
            if((bResult==FALSE)||(!ack)) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                return FALSE;

            }

        } else {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        pos=(int)(((float)(((float)total/(float)file_len))*100));

        if(pos>=100) {
            pos=100;
        }
        posstr.Format(_T("%d%%"),pos);

        if(pos%5==0) {
            PostMessage(WM_MMC_PROGRESS,(LPARAM)pos,0);
        }

    }

    delete []lpBuffer;
    fclose(fp);
    NucUsb.NUC_CloseHandle();
    return TRUE;

}
BOOL CMMCDlg::XUSB_Read(CString& portName,CString& m_pathName,unsigned int addr,unsigned int len)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    int pos=0;
    //NORBOOT_NAND_HEAD fhead;
    unsigned int total=0,scnt,rcnt,ack;
    char* lpBuffer;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    PostMessage(WM_MMC_PROGRESS,(LPARAM)0,0);
    m_progress.SetBkColor(COLOR_VERIFY);

    /***********************************************/
    bResult = NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack;
    bResult=NucUsb.NUC_SetType(0,MMC,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }

    lpBuffer = new char[BUF_SIZE];
    FILE* tempfp;
    //-----------------------------------
    tempfp=_wfopen(m_pathName,_T("w+b"));
    //-----------------------------------
    if(!tempfp) {
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }

    m_fhead->flag=READ_ACTION;
    m_fhead->flashoffset=addr;
    m_fhead->filelen=len;
    memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_MMC_HEAD));
    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_MMC_HEAD));

    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("Write SPI head error\n"));
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("ACK error !"));
        return FALSE;
    }

    scnt=len/BUF_SIZE;
    rcnt=len%BUF_SIZE;
    total=0;
    int prepos=0;
    while(scnt>0) {
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)lpBuffer,BUF_SIZE);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            return FALSE;
        }
        if(bResult==TRUE) {
            total+=BUF_SIZE;
            pos=(int)(((float)(((float)total/(float)len))*100));
            posstr.Format(_T("%d%%"),pos);
            fwrite(lpBuffer,BUF_SIZE,1,tempfp);
            ack=BUF_SIZE;
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)&ack,4);
            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }
        } else {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            return FALSE;
        }

        scnt--;
        if((pos!=prepos) || pos==100 ) {
            prepos=pos;
            PostMessage(WM_MMC_PROGRESS,(LPARAM)pos,0);
        }
    }

    if(rcnt>0) {
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)lpBuffer,BUF_SIZE);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            return FALSE;
        }
        if(bResult==TRUE) {
            total+=rcnt;
            fwrite(lpBuffer,rcnt,1,tempfp);
            ack=BUF_SIZE;
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)&ack,4);
            if(bResult!=TRUE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }
        } else {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            return FALSE;
        }
        pos=(int)(((float)(((float)total/(float)len))*100));
        if(pos>=100) {
            pos=100;
        }
        posstr.Format(_T("%d%%"),pos);
        if((pos!=prepos) || (pos==100)) {
            PostMessage(WM_MMC_PROGRESS,(LPARAM)pos,0);
        }
    }
    delete []lpBuffer;
    NucUsb.NUC_CloseHandle();
    fclose(tempfp);
    /********************************************/
    return TRUE;
}
BOOL CMMCDlg::XUSB_Format(CString& portName)
{
    BOOL bResult;
    CString tempstr;
    int count=0;
    unsigned int ack,format_pos=0;
    NORBOOT_MMC_HEAD *fhead;
    char* lpBuffer;
    fhead=(NORBOOT_MMC_HEAD *)malloc(sizeof(NORBOOT_MMC_HEAD));

    /***********************************************/
    bResult = NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        free(fhead);
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack;
    bResult=NucUsb.NUC_SetType(0,MMC,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        free(fhead);
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }

    lpBuffer = new char[BUF_SIZE];
    memset((unsigned char*)fhead,0,sizeof(NORBOOT_MMC_HEAD));
    fhead->flag=FORMAT_ACTION;
    swscanf_s(m_space,_T("%d"),&fhead->ReserveSize);

    memcpy(lpBuffer,(unsigned char*)fhead,sizeof(NORBOOT_MMC_HEAD));
    free(fhead);

    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_MMC_HEAD));
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("Write eMMC head error\n"));
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("ACK error !"));
        return FALSE;
    }

    format_pos=0;
    int wait_pos=0;
    while(format_pos!=100) {
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }

        DbgOut("eMMC wait erase ack");
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
        if(bResult=FALSE) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            AfxMessageBox(_T("ACK error !"));
            return FALSE;
        }
        DbgOut("eMMC wait erase ack end");
        if(!((ack>>16)&0xffff)) {
            format_pos=ack&0xffff;
            PostMessage(WM_MMC_PROGRESS,(LPARAM)format_pos,0);
        } else {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            CString msg;
            msg.Format(_T("Format error ack=0x%08x\n"),ack);
            AfxMessageBox(msg);
            return FALSE;
        }

        if(format_pos==95) {
            wait_pos++;
            if(wait_pos>100) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                AfxMessageBox(_T("Format error"));
                return FALSE;
            }

        }

    }

    delete []lpBuffer;
    NucUsb.NUC_CloseHandle();
    return TRUE;
}

/************** MMC End ************/

/************** MMC End ************/

/************** NAND Begin ************/
BOOL CNANDDlg::XUSB_Pack(CString& portName,CString& m_pathName,int *len)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    FILE* fp;
    int pos=0;
    UCHAR burn_pos=0;
    unsigned int total,file_len,scnt,rcnt;
    ULONG ack;
    //NORBOOT_NAND_HEAD fhead;
    char* lpBuffer;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    bResult = NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack=0x0;
    bResult=NucUsb.NUC_SetType(0,NAND,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }


    fp=_wfopen(m_pathName,_T("rb"));

    if(!fp) {
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }

    fseek(fp,0,SEEK_END);
    file_len=ftell(fp);
    fseek(fp,0,SEEK_SET);

    if(!file_len) {
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("File length is zero\n"));
        return FALSE;
    }

    unsigned int magic;
    fread((unsigned char *)&magic,4,1,fp);
    if(magic!=0x5) {
        //NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("Pack Image Format Error\n"));
        return FALSE;
    }
    fseek(fp,0,SEEK_SET);

    lpBuffer = new char[file_len]; //read file to buffer
    memset(lpBuffer,0x00,file_len);

    memset((unsigned char*)m_fhead,0,sizeof(NORBOOT_NAND_HEAD));
    total=0;
    m_fhead->flag=PACK_ACTION;
    m_fhead->type=m_type;
    m_fhead->filelen=file_len;
    memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_NAND_HEAD));
    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));

    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("Write NAND head error\n"));
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("ACK error !"));
        return FALSE;
    }

    fread(lpBuffer,m_fhead->filelen,1,fp);
    if(lpBuffer[0]!=0x5) {
	    delete []lpBuffer;
		fclose(fp);
        AfxMessageBox(_T("This file is not pack image"));
    }
    fclose(fp);

	// Check DDR *ini
	bResult=CheckDDRiniData(lpBuffer, m_fhead->filelen);
	if(bResult==FALSE) {
		AfxMessageBox(_T("DDR Init select error\n"));
		delete []lpBuffer;
		//NucUsb.NUC_CloseHandle();
		return FALSE;
    }	

    char *pbuf = lpBuffer;
    PACK_HEAD *ppackhead=(PACK_HEAD *)lpBuffer;
    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf, sizeof(PACK_HEAD));
    Sleep(5);
    if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT)	return FALSE;
    if(bResult!=TRUE) return FALSE;
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }
    total+= sizeof(PACK_HEAD);
    pbuf+= sizeof(PACK_HEAD);

    PACK_CHILD_HEAD child;
    m_progress.SetRange(0,short(ppackhead->num*100));
    int posnum=0;
    int prepos=0;
#if !defined(BATCH_BURN)
    for(int i=0; i<(int)(ppackhead->num); i++) {
        memcpy(&child,(char *)pbuf,sizeof(PACK_CHILD_HEAD));
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf, sizeof(PACK_CHILD_HEAD));

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT)	return FALSE;
        if(bResult!=TRUE) return FALSE;
        total+= sizeof(PACK_CHILD_HEAD);
        pbuf+= sizeof(PACK_CHILD_HEAD);
        //Sleep(1);
        scnt=child.filelen/BUF_SIZE;
        rcnt=child.filelen%BUF_SIZE;

        while(scnt>0) {
            //Sleep(20);
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf, BUF_SIZE);
            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }

            if(bResult==TRUE) {
                pbuf+=BUF_SIZE;
                total+=BUF_SIZE;

                pos=(int)(((float)(((float)total/(float)file_len))*100));
                posstr.Format(_T("%d%%"),pos);

                DbgOut("SPI wait ack");
                bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
                DbgOut("SPI wait ack end");

                if(bResult==FALSE) {
                    delete []lpBuffer;
                    NucUsb.NUC_CloseHandle();
                    //fclose(fp);
                    AfxMessageBox(_T("ACK error !"));
                    return FALSE;
                }

            }

            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }

            scnt--;

            if(pos%5==0) {
                PostMessage(WM_NAND_PROGRESS,(LPARAM)(posnum+pos),0);
            }

        }

        if(rcnt>0) {
            //Sleep(20);
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf,rcnt);
            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }

            //printf("upload %%.2f\r",((float)total/file_len)*100);
            if(bResult==TRUE) {
                pbuf+=rcnt;
                total+=rcnt;
                DbgOut("NAND wait ack");
                bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
                DbgOut("NAND wait ack end");

                if(bResult==FALSE) {
                    delete []lpBuffer;
                    NucUsb.NUC_CloseHandle();
                    //fclose(fp);
                    AfxMessageBox(_T("ACK error !"));
                    return FALSE;

                }

            }

            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                //fclose(fp);
                return FALSE;
            }

            pos=(int)(((float)(((float)total/(float)file_len))*100));

            if(pos>=100) {
                pos=100;
            }
            posstr.Format(_T("%d%%"),pos);

            if(pos%5==0) {

                PostMessage(WM_NAND_PROGRESS,(LPARAM)(posnum+pos),0);
            }

        }


        posnum+=100;

        //fclose(fp);
        //burn progress...
        burn_pos=0;
        //PostMessage(WM_NAND_PROGRESS,(LPARAM)0,0);
        m_progress.SetBkColor(COLOR_BURN);

        while(burn_pos!=100) {
            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }

            DbgOut("NAND wait burn ack");
            bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                AfxMessageBox(_T("ACK error !"));
                return FALSE;
            }
            DbgOut("NAND wait burn ack end");
            if(!((ack>>16)&0xffff)) {
                burn_pos=(UCHAR)(ack&0xffff);
                PostMessage(WM_NAND_PROGRESS,(LPARAM)(posnum+burn_pos),0);
            } else {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                AfxMessageBox(_T("Burn error"));
                return FALSE;
            }
        }
        posnum+=100;
    }
#else
    unsigned int blockNum;
    for(int i=0; i<(int)(ppackhead->num); i++) {
        total=0;
        memcpy(&child,(char *)pbuf,sizeof(PACK_CHILD_HEAD));
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf, sizeof(PACK_CHILD_HEAD));
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT)	return FALSE;
        if(bResult!=TRUE) return FALSE;

        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
        if(bResult==FALSE) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }
        //total+= sizeof(PACK_CHILD_HEAD);
        pbuf+= sizeof(PACK_CHILD_HEAD);
        //Sleep(1);
        scnt=child.filelen/BUF_SIZE;
        rcnt=child.filelen%BUF_SIZE;
        total=0;
        while(scnt>0) {
            //Sleep(20);
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf, BUF_SIZE);
            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }

            if(bResult==TRUE) {
                pbuf+=BUF_SIZE;
                total+=BUF_SIZE;

                //pos=(int)(((float)(((float)total/(float)file_len))*100));
                pos=(int)(((float)(((float)total/(float)child.filelen))*100));
                posstr.Format(_T("%d%%"),pos);

                DbgOut("NAND wait ack");
                bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
                DbgOut("NAND wait ack end");

                if(bResult==FALSE) {
                    delete []lpBuffer;
                    NucUsb.NUC_CloseHandle();
                    CString tmp;
                    tmp.Format(_T("ACK error %d!"),i);
                    AfxMessageBox(tmp);
                    return FALSE;
                }

            }

            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }

            scnt--;

            if((pos!=prepos) || (pos==100)) {
                prepos=pos;
                PostMessage(WM_NAND_PROGRESS,(LPARAM)(posnum+pos),0);
            }

        }

        if(rcnt>0) {
            //Sleep(20);
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf,rcnt);
            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }

            //printf("upload %%.2f\r",((float)total/file_len)*100);
            if(bResult==TRUE) {
                pbuf+=rcnt;
                total+=rcnt;
                DbgOut("NAND wait ack");
                bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
                DbgOut("NAND wait ack end");

                if(bResult==FALSE) {
                    delete []lpBuffer;
                    NucUsb.NUC_CloseHandle();
                    //fclose(fp);
                    CString tmp;
                    tmp.Format(_T("ACK error(rcnt>0) %d!"),i);
                    AfxMessageBox(tmp);
                    return FALSE;

                }

            }

            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                return FALSE;
            }

            //pos=(int)(((float)(((float)total/(float)file_len))*100));
            pos=(int)(((float)(((float)total/(float)child.filelen))*100));

            if(pos>=100) {
                pos=100;
            }
            posstr.Format(_T("%d%%"),pos);

            if((pos!=prepos) || (pos==100)) {
                prepos=pos;
                PostMessage(WM_NAND_PROGRESS,(LPARAM)(posnum+pos),0);
            }

        }
        posnum+=100;

        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&blockNum,4);
        if(bResult==FALSE) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            AfxMessageBox(_T("read block error !"));
            return FALSE;
        }
    }//for(int i=0;i<(int)(ppackhead->num);i++)
#endif

    delete []lpBuffer;
    NucUsb.NUC_CloseHandle();
    return TRUE;

}

BOOL CNANDDlg::XUSB_Burn(CString& portName,CString& m_pathName,int *len,int *blockNum)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    FILE* fp;
    int pos=0;
    UCHAR burn_pos=0;
    unsigned int total,file_len,scnt,rcnt;
    ULONG ack;
    //NORBOOT_NAND_HEAD fhead;
    char* lpBuffer;
    unsigned char *ddrbuf;
    int ddrlen;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    m_progress.SetRange(0,100);
    bResult = NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack=0x0;
    bResult=NucUsb.NUC_SetType(0,NAND,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }

    fp=_wfopen(m_pathName,_T("rb"));

    if(!fp) {
        fclose(fp);
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }

    fseek(fp,0,SEEK_END);
    file_len=ftell(fp);
    fseek(fp,0,SEEK_SET);

    if( (file_len>(mainWnd->m_info.Nand_uPagePerBlock*mainWnd->m_info.Nand_uPageSize))&&m_type==UBOOT) {
#if 1
        //delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("Uboot File length cannot greater than block size\n"));
        return FALSE;
#else
        CString tmp;
        tmp.Format(_T("Uboot File length cannot greater than block size. Do you confirm this operation ?"),4*val-1);

        if(::MessageBox(this->m_hWnd,tmp,_T("Nu Writer"),MB_OKCANCEL|MB_ICONWARNING)==IDCANCEL) {
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }
#endif
    }

    if(!file_len) {
        //delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("File length is zero\n"));
        return FALSE;
    }

    memset((unsigned char*)m_fhead,0,sizeof(NORBOOT_NAND_HEAD));
    m_fhead->flag=WRITE_ACTION;
    ((NORBOOT_NAND_HEAD *)m_fhead)->initSize=0;

    //check AES------------------------------
    if(m_nand_enc_check.GetCheck()==TRUE)
        file_len=((file_len+15)/16)*16;
    m_fhead->filelen=file_len;
#if 1
    switch(m_type) {
    case DATA:
    case IMAGE:
    case PACK:
        swscanf_s(_T("0"),_T("%d"),&m_fhead->no);
        swscanf_s(m_execaddr,_T("%x"),&m_fhead->execaddr);
        swscanf_s(m_startblock,_T("%x"),&m_fhead->flashoffset);
        *len=file_len;
        lpBuffer = new char[file_len+sizeof(NORBOOT_NAND_HEAD)]; //read file to buffer
        memset(lpBuffer,0xff,file_len+sizeof(NORBOOT_NAND_HEAD));
        ((NORBOOT_NAND_HEAD *)m_fhead)->macaddr[7]=0;
        wcstombs( m_fhead->name, (wchar_t *)m_imagename.GetBuffer(), 16);
        m_fhead->type=m_type;
        memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_NAND_HEAD));
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));
        break;
    case ENV:
        swscanf_s(_T("0"),_T("%d"),&m_fhead->no);
        swscanf_s(m_execaddr,_T("%x"),&m_fhead->execaddr);
        swscanf_s(m_startblock,_T("%x"),&m_fhead->flashoffset);
        if(file_len>(0x10000-4)) {
            //delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            AfxMessageBox(_T("The environment file size is less then 64KB\n"));
            return FALSE;
        }
        lpBuffer = new char[0x10000]; //read file to buffer
        memset(lpBuffer,0x00,0x10000);

        ((NORBOOT_NAND_HEAD *)m_fhead)->macaddr[7]=0;

        wcstombs( m_fhead->name, (wchar_t *)m_imagename.GetBuffer(), 16);
        m_fhead->filelen=0x10000;
        m_fhead->type=m_type;
        memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_NAND_HEAD));
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));
        break;
    case UBOOT:
        swscanf_s(_T("1"),_T("%d"),&m_fhead->no);
        swscanf_s(m_execaddr,_T("%x"),&m_fhead->execaddr);
        swscanf_s(m_startblock,_T("%x"),&m_fhead->flashoffset);
        //-------------------DDR---------------------
        //ddrbuf=DDR2Buf(mainWnd->DDRBuf,mainWnd->DDRLen,&ddrlen);
        ddrbuf=DDR2Buf(mainWnd->ShareDDRBuf,mainWnd->DDRLen,&ddrlen);
        file_len=file_len+ddrlen;
        ((NORBOOT_NAND_HEAD *)m_fhead)->initSize=ddrlen;
        //-------------------------------------------
        *len=file_len;
        lpBuffer = new char[file_len]; //read file to buffer
        memset(lpBuffer,0xff,file_len);
        ((NORBOOT_NAND_HEAD *)m_fhead)->macaddr[7]=0;


        wcstombs( m_fhead->name, (wchar_t *)m_imagename.GetBuffer(), 16);

        m_fhead->type=m_type;
        memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_NAND_HEAD));
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));
        break;
    }
#else
    if(m_type!=UBOOT) {
        swscanf_s(_T("0"),_T("%d"),&m_fhead->no);
        swscanf_s(m_execaddr,_T("%x"),&m_fhead->execaddr);
        swscanf_s(m_startblock,_T("%x"),&m_fhead->flashoffset);
        *len=file_len;
    } else {
        swscanf_s(_T("1"),_T("%d"),&m_fhead->no);
        swscanf_s(m_execaddr,_T("%x"),&m_fhead->execaddr);
        swscanf_s(m_startblock,_T("%x"),&m_fhead->flashoffset);
        //-------------------DDR---------------------
        ddrbuf=DDR2Buf(mainWnd->DDRBuf,mainWnd->DDRLen,&ddrlen);
        file_len=file_len+ddrlen;
        ((NORBOOT_NAND_HEAD *)m_fhead)->initSize=ddrlen;
        //-------------------------------------------
        *len=file_len;
    }
#endif
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("Write NOR NAND head error\n"));
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("ACK error !"));
        return FALSE;
    }

#if 1
    switch(m_type) {
    case DATA:
    case IMAGE:
        if(m_nand_enc_check.GetCheck()!=TRUE)
            fread(lpBuffer,m_fhead->filelen,1,fp);
        else {
            CString szDirForKEY,KeyName;
            mainWnd->GetExeDir(szDirForKEY);
            szDirForKEY=szDirForKEY+_T("key_cfg\\");
            fclose(fp);
            m_combo_encrypt.GetLBText(m_combo_encrypt.GetCurSel(),KeyName);
            szDirForKEY=szDirForKEY+KeyName;
            if(encbuf!=NULL) {
                free(encbuf);
                encbuf=NULL;
            }
            encbuf=mainWnd->AES_Encrpt(szDirForKEY,m_pathName,&enclen);
            if(encbuf==NULL) {
                if(lpBuffer!=NULL) delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("AES encrpt failed or AES value equal to zero"));
                return FALSE;
            }
            memcpy(lpBuffer,encbuf,enclen);
        }
        break;
    case PACK:
        fread(lpBuffer,m_fhead->filelen,1,fp);
        break;
    case ENV:
#if 0
        fread(lpBuffer+4,file_len,1,fp);
#else
        {
            char line[256];
            char* ptr=(char *)(lpBuffer+4);
            while (1) {
                if (fgets(line,256, fp) == NULL) break;
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

        *(unsigned int *)lpBuffer=mainWnd->CalculateCRC32((unsigned char *)(lpBuffer+4),0x10000-4);
        *len=file_len=0x10000;
        if(mainWnd->envbuf!=NULL) free(mainWnd->envbuf);
        mainWnd->envbuf=(unsigned char *)malloc(0x10000);
        memcpy(mainWnd->envbuf,lpBuffer,0x10000);
        break;
    case UBOOT:
        memcpy(lpBuffer,ddrbuf,ddrlen);
        //------AES--------------------------------------------
        if(m_nand_enc_check.GetCheck()!=TRUE)
            fread(lpBuffer+ddrlen,m_fhead->filelen,1,fp);
        else {
            char *encbuf;
            unsigned int len;
            CString szDirForKEY,KeyName;
            mainWnd->GetExeDir(szDirForKEY);
            szDirForKEY=szDirForKEY+_T("key_cfg\\");
            fclose(fp);
            m_combo_encrypt.GetLBText(m_combo_encrypt.GetCurSel(),KeyName);
            szDirForKEY=szDirForKEY+KeyName;
            encbuf=mainWnd->AES_Encrpt(szDirForKEY,m_pathName,&len);
            if(encbuf==NULL) {
                if(lpBuffer!=NULL) delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("AES encrpt failed or AES value equal to zero"));
                return FALSE;
            }
            memcpy(lpBuffer+ddrlen,encbuf,len);
        }
        break;
    }
#else
    if(m_type==UBOOT) {
        memcpy(lpBuffer,ddrbuf,ddrlen);
        //------AES--------------------------------------------
        if(m_nand_enc_check.GetCheck()!=TRUE)
            fread(lpBuffer+ddrlen,m_fhead->filelen,1,fp);
        else {
            char *encbuf;
            unsigned int len;
            CString szDirForKEY,KeyName;
            mainWnd->GetExeDir(szDirForKEY);
            szDirForKEY=szDirForKEY+_T("key_cfg\\");
            fclose(fp);
            m_combo_encrypt.GetLBText(m_combo_encrypt.GetCurSel(),KeyName);
            szDirForKEY=szDirForKEY+KeyName;
            encbuf=mainWnd->AES_Encrpt(szDirForKEY,m_pathName,&len);
            memcpy(lpBuffer+ddrlen,encbuf,len);
        }
        //-----------------------------------------------------
    } else {
        fread(lpBuffer,m_fhead->filelen,1,fp);
    }
#endif
    scnt=file_len/BUF_SIZE;
    rcnt=file_len%BUF_SIZE;

#if !defined(BATCH_BURN)
    total=0;
    char *pbuf = lpBuffer;
    while(scnt>0) {
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf, BUF_SIZE);
        pbuf+=BUF_SIZE;
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        if(bResult==TRUE) {
            total+=BUF_SIZE;

            pos=(int)(((float)(((float)total/(float)file_len))*100));
            posstr.Format(_T("%d%%"),pos);

            DbgOut("NAND wait ack");
            bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
            DbgOut("NAND wait ack end");

            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("ACK error !"));
                return FALSE;
            }

        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        scnt--;

        if(pos%5==0) {
            PostMessage(WM_NAND_PROGRESS,(LPARAM)pos,0);
        }

    }

    if(rcnt>0) {
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf,rcnt);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            fclose(fp);
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }

        //printf("upload %%.2f\r",((float)total/file_len)*100);


        if(bResult==TRUE) {
            total+=rcnt;
            DbgOut("NAND wait ack");
            bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
            DbgOut("NAND wait ack end");

            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("ACK error !"));
                return FALSE;

            }

        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        pos=(int)(((float)(((float)total/(float)file_len))*100));

        if(pos>=100) {
            pos=100;
        }
        posstr.Format(_T("%d%%"),pos);

        if(pos%5==0) {

            PostMessage(WM_NAND_PROGRESS,(LPARAM)pos,0);
        }

    }

    delete []lpBuffer;
    fclose(fp);

//burn progress...
    burn_pos=0;
    PostMessage(WM_NAND_PROGRESS,(LPARAM)0,0);
    m_progress.SetBkColor(COLOR_BURN);

    while(burn_pos!=100) {
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            return FALSE;
        }

        DbgOut("NAND wait burn ack");
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
        if(bResult==FALSE) {
            NucUsb.NUC_CloseHandle();
            AfxMessageBox(_T("ACK error !"));
            return FALSE;
        }
        DbgOut("NAND wait burn ack end");
        if(!((ack>>16)&0xffff)) {
            burn_pos=(UCHAR)(ack&0xffff);
            PostMessage(WM_NAND_PROGRESS,(LPARAM)burn_pos,0);
        } else {
            NucUsb.NUC_CloseHandle();
            AfxMessageBox(_T("Burn error"));
            return FALSE;
        }

    }

    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)blockNum,4);
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("read block error !"));
        return FALSE;
    }
#else
    total=0;
    char *pbuf = lpBuffer;
    while(scnt>0) {
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf, BUF_SIZE);
        pbuf+=BUF_SIZE;
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            fclose(fp);
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }

        if(bResult==TRUE) {
            total+=BUF_SIZE;

            pos=(int)(((float)(((float)total/(float)file_len))*100));
            posstr.Format(_T("%d%%"),pos);

            DbgOut("NAND wait ack");
            bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
            DbgOut("NAND wait ack end");

            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("ACK error !"));
                return FALSE;
            }

        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            fclose(fp);
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }

        scnt--;

        if(pos%5==0) {
            PostMessage(WM_NAND_PROGRESS,(LPARAM)pos,0);
        }

    }

    if(rcnt>0) {
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf,rcnt);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            fclose(fp);
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }

        //printf("upload %%.2f\r",((float)total/file_len)*100);


        if(bResult==TRUE) {
            total+=rcnt;
            DbgOut("NAND wait ack");
            bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
            DbgOut("NAND wait ack end");

            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("ACK error !"));
                return FALSE;

            }

        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            fclose(fp);
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }

        pos=(int)(((float)(((float)total/(float)file_len))*100));

        if(pos>=100) {
            pos=100;
        }
        posstr.Format(_T("%d%%"),pos);

        if(pos%5==0) {
            PostMessage(WM_NAND_PROGRESS,(LPARAM)pos,0);
        }

    }
    delete []lpBuffer;
    fclose(fp);

    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)blockNum,4);
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("read block error !"));
        return FALSE;
    }
#endif
    NucUsb.NUC_CloseHandle();
    return TRUE;
}

BOOL CNANDDlg::XUSB_BurnWithOOB(CString& portName,CString& m_pathName,int *len,int *blockNum)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    FILE* fp;
    int pos=0;
    UCHAR burn_pos=0;
    unsigned int total,file_len,scnt,rcnt;
    ULONG ack;
    //NORBOOT_NAND_HEAD fhead;
    char* lpBuffer;
    unsigned char *ddrbuf;
    int ddrlen;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    m_progress.SetRange(0,100);
    bResult = NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack=0x0;
    bResult=NucUsb.NUC_SetType(0,NAND,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }


    fp=_wfopen(m_pathName,_T("rb"));

    if(!fp) {
        //delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }



    fseek(fp,0,SEEK_END);
    file_len=ftell(fp);
    fseek(fp,0,SEEK_SET);

    if(file_len%(mainWnd->m_info.Nand_uPageSize+mainWnd->m_info.Nand_uSpareSize)!=0) {
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("File length is not Multiple of page+spare size \n"));
        return FALSE;
    }

    if(!file_len) {
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("File length is zero\n"));
        return FALSE;
    }

    memset((unsigned char*)m_fhead,0,sizeof(NORBOOT_NAND_HEAD));
    m_fhead->flag=WRITE_ACTION;
    ((NORBOOT_NAND_HEAD *)m_fhead)->initSize=0;

    //check AES------------------------------
    if(m_nand_enc_check.GetCheck()==TRUE)
        file_len=((file_len+15)/16)*16;
    m_fhead->filelen=file_len;

    switch(m_type) {
    case DATA:
    case IMAGE:
    case PACK:
        swscanf_s(_T("0"),_T("%d"),&m_fhead->no);
        swscanf_s(m_execaddr,_T("%x"),&m_fhead->execaddr);
        swscanf_s(m_startblock,_T("%x"),&m_fhead->flashoffset);
        *len=file_len;
        lpBuffer = new char[file_len+sizeof(NORBOOT_NAND_HEAD)]; //read file to buffer
        memset(lpBuffer,0xff,file_len+sizeof(NORBOOT_NAND_HEAD));
        ((NORBOOT_NAND_HEAD *)m_fhead)->macaddr[7]=0;
        wcstombs( m_fhead->name, (wchar_t *)m_imagename.GetBuffer(), 16);
        m_fhead->type=DATA_OOB;
        memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_NAND_HEAD));
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));
        break;
    case ENV:
        swscanf_s(_T("0"),_T("%d"),&m_fhead->no);
        swscanf_s(m_execaddr,_T("%x"),&m_fhead->execaddr);
        swscanf_s(m_startblock,_T("%x"),&m_fhead->flashoffset);
        if(file_len>(0x10000-4)) {
            //delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            AfxMessageBox(_T("The environment file size is less then 64KB\n"));
            return FALSE;
        }
        lpBuffer = new char[0x10000]; //read file to buffer
        memset(lpBuffer,0x00,0x10000);

        ((NORBOOT_NAND_HEAD *)m_fhead)->macaddr[7]=0;

        wcstombs( m_fhead->name, (wchar_t *)m_imagename.GetBuffer(), 16);
        m_fhead->filelen=0x10000;
        m_fhead->type=m_type;
        memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_NAND_HEAD));
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));
        break;
    case UBOOT:
        swscanf_s(_T("1"),_T("%d"),&m_fhead->no);
        swscanf_s(m_execaddr,_T("%x"),&m_fhead->execaddr);
        swscanf_s(m_startblock,_T("%x"),&m_fhead->flashoffset);
        //-------------------DDR---------------------
        //ddrbuf=DDR2Buf(mainWnd->DDRBuf,mainWnd->DDRLen,&ddrlen);
        ddrbuf=DDR2Buf(mainWnd->ShareDDRBuf,mainWnd->DDRLen,&ddrlen);
        file_len=file_len+ddrlen;
        ((NORBOOT_NAND_HEAD *)m_fhead)->initSize=ddrlen;
        //-------------------------------------------
        *len=file_len;
        lpBuffer = new char[file_len]; //read file to buffer
        memset(lpBuffer,0xff,file_len);
        ((NORBOOT_NAND_HEAD *)m_fhead)->macaddr[7]=0;


        wcstombs( m_fhead->name, (wchar_t *)m_imagename.GetBuffer(), 16);

        m_fhead->type=m_type;
        memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_NAND_HEAD));
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));
        break;
    }

    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("Write NOR NAND head error\n"));
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("ACK error !"));
        return FALSE;
    }


    switch(m_type) {
    case DATA:
    case IMAGE:
        if(m_nand_enc_check.GetCheck()!=TRUE)
            fread(lpBuffer,m_fhead->filelen,1,fp);
        else {
            CString szDirForKEY,KeyName;
            mainWnd->GetExeDir(szDirForKEY);
            szDirForKEY=szDirForKEY+_T("key_cfg\\");
            fclose(fp);
            m_combo_encrypt.GetLBText(m_combo_encrypt.GetCurSel(),KeyName);
            szDirForKEY=szDirForKEY+KeyName;
            if(encbuf!=NULL) {
                free(encbuf);
                encbuf=NULL;
            }
            encbuf=mainWnd->AES_Encrpt(szDirForKEY,m_pathName,&enclen);
            if(encbuf==NULL) {
                if(lpBuffer!=NULL) delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("AES encrpt failed or AES value equal to zero"));
                return FALSE;
            }
            memcpy(lpBuffer,encbuf,enclen);
        }
        break;
    case PACK:
        fread(lpBuffer,m_fhead->filelen,1,fp);
        break;
    case ENV:
#if 0
        fread(lpBuffer+4,file_len,1,fp);
#else
        {
            char line[256];
            char* ptr=(char *)(lpBuffer+4);
            while (1) {
                if (fgets(line,256, fp) == NULL) break;
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

        *(unsigned int *)lpBuffer=mainWnd->CalculateCRC32((unsigned char *)(lpBuffer+4),0x10000-4);
        *len=file_len=0x10000;
        if(mainWnd->envbuf!=NULL) free(mainWnd->envbuf);
        mainWnd->envbuf=(unsigned char *)malloc(0x10000);
        memcpy(mainWnd->envbuf,lpBuffer,0x10000);
        break;
    case UBOOT:
        memcpy(lpBuffer,ddrbuf,ddrlen);
        //------AES--------------------------------------------
        if(m_nand_enc_check.GetCheck()!=TRUE)
            fread(lpBuffer+ddrlen,m_fhead->filelen,1,fp);
        else {
            char *encbuf;
            unsigned int len;
            CString szDirForKEY,KeyName;
            mainWnd->GetExeDir(szDirForKEY);
            szDirForKEY=szDirForKEY+_T("key_cfg\\");
            fclose(fp);
            m_combo_encrypt.GetLBText(m_combo_encrypt.GetCurSel(),KeyName);
            szDirForKEY=szDirForKEY+KeyName;
            encbuf=mainWnd->AES_Encrpt(szDirForKEY,m_pathName,&len);
            if(encbuf==NULL) {
                if(lpBuffer!=NULL) delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("AES encrpt failed or AES value equal to zero"));
                return FALSE;
            }
            memcpy(lpBuffer+ddrlen,encbuf,len);
        }
        break;
    }

    int BurnBufLen=(mainWnd->m_info.Nand_uPageSize+mainWnd->m_info.Nand_uSpareSize);
    scnt=file_len/BurnBufLen;
    rcnt=file_len%BurnBufLen;

    if(rcnt!=0) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("File length error"));
        return 0;
    }

    total=0;
    char *pbuf = lpBuffer;
    while(scnt>0) {
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf, BurnBufLen);
        pbuf+=BurnBufLen;
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        if(bResult==TRUE) {
            total+=BurnBufLen;

            pos=(int)(((float)(((float)total/(float)file_len))*100));
            posstr.Format(_T("%d%%"),pos);

            DbgOut("NAND wait ack");
            bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
            DbgOut("NAND wait ack end");

            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("ACK error !"));
                return FALSE;
            }

        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        scnt--;

        if(pos%5==0) {
            PostMessage(WM_NAND_PROGRESS,(LPARAM)pos,0);
        }

    }

    if(rcnt>0) {
        bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)pbuf,rcnt);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            fclose(fp);
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }

        //printf("upload %%.2f\r",((float)total/file_len)*100);


        if(bResult==TRUE) {
            total+=rcnt;
            DbgOut("NAND wait ack");
            bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
            DbgOut("NAND wait ack end");

            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                AfxMessageBox(_T("ACK error !"));
                return FALSE;

            }

        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        pos=(int)(((float)(((float)total/(float)file_len))*100));

        if(pos>=100) {
            pos=100;
        }
        posstr.Format(_T("%d%%"),pos);

        if(pos%5==0) {
            PostMessage(WM_NAND_PROGRESS,(LPARAM)pos,0);
        }

    }
    delete []lpBuffer;
    fclose(fp);

    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)blockNum,4);
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("read block error !"));
        return FALSE;
    }

    NucUsb.NUC_CloseHandle();
    return TRUE;
}

int CNANDDlg::XUSB_Verify(CString& portName,CString& m_pathName)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    FILE* fp;
    int pos=0;
    //NORBOOT_NAND_HEAD fhead;
    unsigned int total=0,file_len,scnt,rcnt,ack;
    char* lpBuffer;
    char temp[BUF_SIZE];
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    PostMessage(WM_NAND_PROGRESS,(LPARAM)0,0);
    m_progress.SetBkColor(COLOR_VERIFY);

    /***********************************************/
    bResult = NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack;
    bResult=NucUsb.NUC_SetType(0,NAND,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }

    lpBuffer = new char[BUF_SIZE];
    fp=_wfopen(m_pathName,_T("rb"));

    if(!fp) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }



    if(m_type!=ENV) {
        if(m_nand_enc_check.GetCheck()!=TRUE) {
            fseek(fp,0,SEEK_END);
            file_len=ftell(fp);
            fseek(fp,0,SEEK_SET);
        } else
            file_len=enclen;
    } else {
        file_len=0x10000;
    }

    if(!file_len) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("File length is zero\n"));
        return FALSE;
    }

    if(m_type==PACK) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        return NAND_VERIFY_PACK_ERROR;
    }

    if(m_type==IMAGE && (file_len%512)!=0) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        return NAND_VERIFY_FILESYSTEM_ERROR;
    }

    m_fhead->flag=VERIFY_ACTION;

    memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_NAND_HEAD));
    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));

    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("Write NAND head error\n"));
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(fp);
        AfxMessageBox(_T("ACK error !"));
        return FALSE;
    }

    scnt=file_len/BUF_SIZE;
    rcnt=file_len%BUF_SIZE;

    total=0;
    int prepos=0;
    while(scnt>0) {
        if(m_type!=ENV) {
            if(m_nand_enc_check.GetCheck()!=TRUE)
                fread(temp,BUF_SIZE,1,fp);
            else {
                memcpy(temp,encbuf+total,BUF_SIZE);
            }
        } else {
            memcpy(temp,mainWnd->envbuf+total,BUF_SIZE);
        }
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)lpBuffer, BUF_SIZE);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        if(bResult==TRUE) {
            total+=BUF_SIZE;

            pos=(int)(((float)(((float)total/(float)file_len))*100));
            posstr.Format(_T("%d%%"),pos);

            if(DataCompare(temp,lpBuffer,BUF_SIZE))
                ack=BUF_SIZE;
            else
                ack=0;//compare error
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)&ack,4);
            if((bResult==FALSE)||(!ack)) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                return FALSE;
            }

        } else {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        scnt--;

        if(pos-prepos>=5 || pos==100) {
            prepos=pos;
            PostMessage(WM_NAND_PROGRESS,(LPARAM)pos,0);
        }


    }

    if(rcnt>0) {
        if(m_type!=ENV) {
            if(m_nand_enc_check.GetCheck()!=TRUE)
                fread(temp,rcnt,1,fp);
            else {
                memcpy(temp,encbuf+total,rcnt);
            }
        } else {
            memcpy(temp,mainWnd->envbuf+total,rcnt);
        }

        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)lpBuffer,BUF_SIZE);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        if(bResult==TRUE) {
            total+=rcnt;
            if(DataCompare(temp,lpBuffer,rcnt))
                ack=BUF_SIZE;
            else
                ack=0;//compare error
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)&ack, 4);
            if((bResult==FALSE)||(!ack)) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(fp);
                return FALSE;

            }

        } else {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(fp);
            return FALSE;
        }

        pos=(int)(((float)(((float)total/(float)file_len))*100));

        if(pos>=100) {
            pos=100;
        }
        posstr.Format(_T("%d%%"),pos);

        if(pos-prepos>=5 || pos==100) {
            prepos=pos;
            PostMessage(WM_NAND_PROGRESS,(LPARAM)pos,0);
        }

    }

    delete []lpBuffer;
    fclose(fp);
    NucUsb.NUC_CloseHandle();
    return TRUE;

}

BOOL CNANDDlg::XUSB_Read_Redunancy(CString& portName,CString& m_pathName,unsigned int addr,unsigned int len)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    int pos=0;
    //NORBOOT_NAND_HEAD fhead;
    unsigned int total=0,scnt,ack;
    char* lpBuffer;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    PostMessage(WM_NAND_PROGRESS,(LPARAM)0,0);
    m_progress.SetBkColor(COLOR_VERIFY);

    /***********************************************/
    bResult = NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack;
    bResult=NucUsb.NUC_SetType(0,NAND,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }

    lpBuffer = new char[BUF_SIZE];
    FILE* tempfp;
    //-----------------------------------
    tempfp=_wfopen(m_pathName,_T("w+b"));
    //-----------------------------------
    if(!tempfp) {
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }

    unsigned int sparesize,totalsize;
    sparesize=mainWnd->m_info.Nand_uSpareSize;
    totalsize=mainWnd->m_info.Nand_uPagePerBlock*(mainWnd->m_info.Nand_uPageSize+mainWnd->m_info.Nand_uSpareSize);
    m_fhead->flag=READ_ACTION;
    m_fhead->flashoffset=addr;
    m_fhead->filelen=len*(totalsize);
    m_fhead->initSize=1;  //read redunancy data, good block and bad block

    memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_NAND_HEAD));
    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));

    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(tempfp);
        AfxMessageBox(_T("Write NAND head error\n"));
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(tempfp);
        AfxMessageBox(_T("ACK error !"));
        return FALSE;
    }
#if 1
    unsigned int alen=len*(totalsize);
    total=0;
    for(unsigned int i=0; i<len; i++) {
        scnt=totalsize/(mainWnd->m_info.Nand_uPageSize+mainWnd->m_info.Nand_uSpareSize);
        while(scnt>0) {
            bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)lpBuffer,mainWnd->m_info.Nand_uPageSize);
            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(tempfp);
                return FALSE;
            }
            if(bResult==TRUE) {
                total+=mainWnd->m_info.Nand_uPageSize;
                pos=(int)(((float)(((float)total/(float)alen))*100));
                posstr.Format(_T("%d%%"),pos);
                fwrite(lpBuffer,mainWnd->m_info.Nand_uPageSize,1,tempfp);
                ack=mainWnd->m_info.Nand_uPageSize;
                bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)&ack,4);
                if(bResult==FALSE) {
                    delete []lpBuffer;
                    NucUsb.NUC_CloseHandle();
                    fclose(tempfp);
                    return FALSE;
                }
            } else {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(tempfp);
                return FALSE;
            }

            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(tempfp);
                return FALSE;
            }

            scnt--;
            if(pos%5==0) {
                PostMessage(WM_NAND_PROGRESS,(LPARAM)pos,0);
            }
            //sparesize---------------------------------------------------------
            bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)lpBuffer,sparesize);
            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(tempfp);
                return FALSE;
            }
            if(bResult==TRUE) {
                total+=sparesize;
                pos=(int)(((float)(((float)total/(float)alen))*100));
                posstr.Format(_T("%d%%"),pos);
                fwrite(lpBuffer,sparesize,1,tempfp);
                ack=sparesize;
                bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)&ack,4);
                if(bResult==FALSE) {
                    delete []lpBuffer;
                    NucUsb.NUC_CloseHandle();
                    fclose(tempfp);
                    return FALSE;
                }
            } else {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(tempfp);
                return FALSE;
            }
            if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(tempfp);
                return FALSE;
            }
            if(pos%5==0) {
                PostMessage(WM_NAND_PROGRESS,(LPARAM)pos,0);
            }
        }//while(scnt>0) end

    }
#else
    scnt=len/BUF_SIZE;
    rcnt=len%BUF_SIZE;
    total=0;
    while(scnt>0) {
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)lpBuffer,BUF_SIZE);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(tempfp);
            return FALSE;
        }
        if(bResult==TRUE) {
            total+=BUF_SIZE;
            pos=(int)(((float)(((float)total/(float)len))*100));
            posstr.Format(_T("%d%%"),pos);
            fwrite(lpBuffer,BUF_SIZE,1,tempfp);
            ack=BUF_SIZE;
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)&ack,4);
            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(tempfp);
                return FALSE;
            }
        } else {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(tempfp);
            return FALSE;
        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(tempfp);
            return FALSE;
        }

        scnt--;
        if(pos%5==0) {
            PostMessage(WM_NAND_PROGRESS,(LPARAM)pos,0);
        }
    }

    if(rcnt>0) {
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)lpBuffer,BUF_SIZE);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(tempfp);
            return FALSE;
        }
        if(bResult==TRUE) {
            total+=rcnt;
            fwrite(lpBuffer,rcnt,1,tempfp);
            ack=BUF_SIZE;
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)&ack,4);
            if(bResult!=TRUE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(tempfp);
                return FALSE;
            }
        } else {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(tempfp);
            return FALSE;
        }
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(tempfp);
            return FALSE;
        }
        pos=(int)(((float)(((float)total/(float)len))*100));
        if(pos>=100) {
            pos=100;
        }
        posstr.Format(_T("%d%%"),pos);
        if(pos%5==0) {
            PostMessage(WM_NAND_PROGRESS,(LPARAM)pos,0);
        }
    }
#endif
    delete []lpBuffer;
    NucUsb.NUC_CloseHandle();
    fclose(tempfp);
    /********************************************/
    return TRUE;
}

BOOL CNANDDlg::XUSB_Read(CString& portName,CString& m_pathName,unsigned int addr,unsigned int len)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    int pos=0;
    //NORBOOT_NAND_HEAD fhead;
    unsigned int total=0,scnt,rcnt,ack;
    char* lpBuffer;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    PostMessage(WM_NAND_PROGRESS,(LPARAM)0,0);
    m_progress.SetBkColor(COLOR_VERIFY);

    /***********************************************/
    bResult = NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack;
    bResult=NucUsb.NUC_SetType(0,NAND,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }

    lpBuffer = new char[BUF_SIZE];
    FILE* tempfp;
    //-----------------------------------
    tempfp=_wfopen(m_pathName,_T("w+b"));
    //-----------------------------------
    if(!tempfp) {
	    delete []lpBuffer;
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }

    m_fhead->flag=READ_ACTION;
    m_fhead->flashoffset=addr;
    m_fhead->filelen=len;
    m_fhead->initSize=0; //read good block
    memcpy(lpBuffer,(unsigned char*)m_fhead,sizeof(NORBOOT_NAND_HEAD));
    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));

    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(tempfp);
        AfxMessageBox(_T("Write NAND head error\n"));
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        fclose(tempfp);
        AfxMessageBox(_T("ACK error !"));
        return FALSE;
    }

    scnt=len/BUF_SIZE;
    rcnt=len%BUF_SIZE;
    total=0;
    while(scnt>0) {
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)lpBuffer,BUF_SIZE);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(tempfp);
            return FALSE;
        }
        if(bResult==TRUE) {
            total+=BUF_SIZE;
            pos=(int)(((float)(((float)total/(float)len))*100));
            posstr.Format(_T("%d%%"),pos);
            fwrite(lpBuffer,BUF_SIZE,1,tempfp);
            ack=BUF_SIZE;
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)&ack,4);
            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(tempfp);
                return FALSE;
            }
        } else {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(tempfp);
            return FALSE;
        }

        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(tempfp);
            return FALSE;
        }

        scnt--;
        if(pos%5==0) {
            PostMessage(WM_NAND_PROGRESS,(LPARAM)pos,0);
        }
    }

    if(rcnt>0) {
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)lpBuffer,BUF_SIZE);
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(tempfp);
            return FALSE;
        }
        if(bResult==TRUE) {
            total+=rcnt;
            fwrite(lpBuffer,rcnt,1,tempfp);
            ack=BUF_SIZE;
            bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)&ack,4);
            if(bResult!=TRUE) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                fclose(tempfp);
                return FALSE;
            }
        } else {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(tempfp);
            return FALSE;
        }
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            fclose(tempfp);
            return FALSE;
        }
        pos=(int)(((float)(((float)total/(float)len))*100));
        if(pos>=100) {
            pos=100;
        }
        posstr.Format(_T("%d%%"),pos);
        if(pos%5==0) {
            PostMessage(WM_NAND_PROGRESS,(LPARAM)pos,0);
        }
    }
    delete []lpBuffer;
    NucUsb.NUC_CloseHandle();
    fclose(tempfp);
    /********************************************/
    return TRUE;
}
BOOL CNANDDlg::XUSB_Erase(CString& portName)
{
    BOOL bResult;
    CString tempstr;
    int count=0;
    unsigned int ack,erase_pos=0;
    NORBOOT_NAND_HEAD *fhead;
    char* lpBuffer;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    fhead=(NORBOOT_NAND_HEAD *)malloc(sizeof(NORBOOT_NAND_HEAD));

    m_progress.SetRange(0,100);
    /***********************************************/
    bResult = NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        free(fhead);
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        free(fhead);
        return FALSE;
    }
#endif
    USHORT typeack;
    bResult=NucUsb.NUC_SetType(0,NAND,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        free(fhead);
        return FALSE;
    }

    lpBuffer = new char[BUF_SIZE];


    memset((unsigned char*)fhead,0,sizeof(NORBOOT_NAND_HEAD));
    fhead->flag=ERASE_ACTION;

    fhead->flashoffset = _wtoi(m_sblocks); //start erase block
    fhead->execaddr=_wtoi(m_blocks);  //erase block length
    fhead->type=m_erase_flag; // Decide chip erase mode or erase mode

    if(mainWnd->ChipEraseWithBad==0)
        fhead->no=0xFFFFFFFF;//erase good block
    else
        fhead->no=0xFFFFFFFE;//erase good block and bad block
    memcpy(lpBuffer,(unsigned char*)fhead,sizeof(NORBOOT_NAND_HEAD));
    bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));
    free(fhead);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("Write NAND head error\n"));
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("ACK error !"));
        return FALSE;
    }

    erase_pos=0;
    int wait_pos=0;
    while(erase_pos!=100) {
        if(WaitForSingleObject(m_ExitEvent, 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            return FALSE;
        }

        DbgOut("NAND wait erase ack");
        Sleep(2);
        bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
        if(bResult=FALSE) {
            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            AfxMessageBox(_T("ACK error !"));
            return FALSE;
        }
        DbgOut("NAND wait erase ack end");
        if(!((ack>>16)&0xffff)) {
            erase_pos=ack&0xffff;
            PostMessage(WM_NAND_PROGRESS,(LPARAM)erase_pos,0);
        } else {

            delete []lpBuffer;
            NucUsb.NUC_CloseHandle();
            CString msg;
            msg.Format(_T("Erase error ack=0x%08x\n"),ack);
            AfxMessageBox(msg);
            return FALSE;
        }

        if(erase_pos==95) {
            wait_pos++;
            if(wait_pos>100) {
                delete []lpBuffer;
                NucUsb.NUC_CloseHandle();
                AfxMessageBox(_T("Erase error"));
                return FALSE;
            }

        }

    }


    delete []lpBuffer;
    NucUsb.NUC_CloseHandle();
    return TRUE;

}

/************** NAND End ************/

/************** MTP Begin ************/
#define MTP_KEY_SUCCESS			0x80
#define MTP_NO_KEY 				0x81
#define MTP_LOCK_FAILED 		0x82
#define MTP_LOCK_KEY    		0x83
#define MTP_LOCK_SUCCESS        0x84
#define MTP_ENABLE_SUCCESS 	    0x85

//#define TEST
BOOL CMTPDlg::XUSB_Burn(CString& portName,CString& m_pathName)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;

    int count=0;
    int pos=0;
    NORBOOT_MTP_HEAD fhead;
    unsigned int ack;
    char* lpBuffer;

    bResult = NucUsb.EnableWinUsbDevice();
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(0);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif

#ifndef TEST
    USHORT typeack=0xff;
    bResult=NucUsb.NUC_SetType(0,MTP,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.NUC_CloseHandle();
        return FALSE;
    }
#endif
    char *buf;
    int len=0;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    memset((char *)&fhead,0x0,sizeof(NORBOOT_MTP_HEAD));

    memset(mainWnd->_FileLineBuff,0x00,LINE_BUFF_LEN);

    if(m_mtp_option==0) { // AES
        buf=mainWnd->Get_OTP_KEY(m_pathName,&len);
        memcpy((char *)&fhead.KeyOTP[0],buf,len);
        fhead.KeyLen = len;
    } else { // SHA
        unsigned char data[32];
        //buf=(char *)mainWnd->CalculateSHA(m_aesfilename);
        buf=(char *)mainWnd->CalculateSHA(m_pathName);
        for(int i=0; i<8; i++) {
            for(int j=0; j<4; j++)
                data[i*4+j]=buf[i*4+(3-j)];
        }

        memcpy((char *)&fhead.KeyOTP[0],data,32);
        fhead.KeyLen = 32;
        CString showsha;

        showsha.Format(_T("Key:0x%08x;0x%08x;0x%08x;0x%08x;0x%08x;0x%08x;0x%08x;0x%08x"),
                       fhead.KeyOTP[0],
                       fhead.KeyOTP[1],
                       fhead.KeyOTP[2],
                       fhead.KeyOTP[3],
                       fhead.KeyOTP[4],
                       fhead.KeyOTP[5],
                       fhead.KeyOTP[6],
                       fhead.KeyOTP[7]);
        m_mtp_status.SetWindowText(showsha);
        m_mtp_status.ShowWindow(1);
    }
    fhead.Mode = m_mtp_mode;

    if(m_mtp_option==0)  //AES
        fhead.Option = (1<<m_mtp_option) | ((1-m_mtp_encrypt)<<2)| 0x8;
    else { //SHA
        if(m_mtp_mode==1)
            fhead.Option = (1<<m_mtp_option) | ((1-m_mtp_encrypt)<<2)| 0x8; //USB without SHA
        else
            fhead.Option = (1<<m_mtp_option) | ((1-m_mtp_encrypt)<<2); //USB with SHA
    }


    if( ((CButton*)GetDlgItem(IDC_CHECK_LOCK_ENABLE))->GetCheck() )
        fhead.Lock = 1;
    else
        fhead.Lock = 0;

	TRACE(_T("m_mtp_option= %d, m_mtp_mode=%d,m_mtp_encrypt=%d, fhead.Mode = %d, fhead.Option = %d,  fhead.Lock = %d\n"), m_mtp_option, m_mtp_mode, m_mtp_encrypt, fhead.Mode, fhead.Option, fhead.Lock);

	lpBuffer = new char[BUF_SIZE];
	memcpy(lpBuffer,(unsigned char*)&fhead,sizeof(NORBOOT_MTP_HEAD));
	bResult=NucUsb.NUC_WritePipe(0,(UCHAR *)lpBuffer, sizeof(NORBOOT_MTP_HEAD));
	if(bResult==FALSE)
	{
		delete []lpBuffer;
		NucUsb.NUC_CloseHandle();
		AfxMessageBox(_T("Write MTP head error\n"));
		return FALSE;
	}

    Sleep(50);
    bResult=NucUsb.NUC_ReadPipe(0,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.NUC_CloseHandle();
        AfxMessageBox(_T("MTP head ack error\n"));
        return FALSE;
    }

    pos=100;
    PostMessage(WM_MTP_PROGRESS,(LPARAM)pos,0);

    delete []lpBuffer;
    NucUsb.NUC_CloseHandle();

    switch(ack) {
    case MTP_KEY_SUCCESS:
        AfxMessageBox(_T("Burn successfully"));
        break;
    case MTP_NO_KEY:
        AfxMessageBox(_T("No key in MTP"));
        break;
    case MTP_LOCK_FAILED:
        AfxMessageBox(_T("Lock failed in MTP"));
        break;
    case MTP_LOCK_KEY:
        AfxMessageBox(_T("Failure. The key was locked in MTP"));
        break;
    case MTP_LOCK_SUCCESS:
        AfxMessageBox(_T("Lock successfully"));
        break;
    }

    return TRUE;
}
/************** MTP End ************/

/************** Fast Begin ************/

BOOL FastDlg::XUSB_FastNANDErase(int id, CString& portName)
{
    BOOL bResult;
    CString tempstr;
    int count=0;
    unsigned int ack,erase_pos=0;
    NORBOOT_NAND_HEAD *fhead;
    char* lpBuffer;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    fhead=(NORBOOT_NAND_HEAD *)malloc(sizeof(NORBOOT_NAND_HEAD));

    /***********************************************/
    bResult=NucUsb.EnableOneWinUsbDevice(id);
    if(bResult==FALSE) {
        free(fhead);
        //AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(id);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack;
    bResult=NucUsb.NUC_SetType(id,NAND,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        free(fhead);
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("XXX (%d) NAND Erase error\n"),id);
        return FALSE;
    }

    lpBuffer = new char[BUF_SIZE];
    memset((unsigned char*)fhead,0,sizeof(NORBOOT_NAND_HEAD));
    fhead->flag=ERASE_ACTION;

    fhead->flashoffset = 0; //start erase block
    fhead->execaddr= 20;//erase block length
    fhead->type= 0;// chip erase mode

    if(mainWnd->ChipEraseWithBad==0)
        fhead->no=0xFFFFFFFF;//erase good block
    else
        fhead->no=0xFFFFFFFE;//erase good block and bad block

    memcpy(lpBuffer,(unsigned char*)fhead,sizeof(NORBOOT_NAND_HEAD));
    bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));
    //TRACE("debug xusb.cpp  44   id= %d, bResult = %d \n", id, bResult);
    free(fhead);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        //TRACE(_T("XXX (%d) NAND Erase head error\n"),id);
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
    //TRACE("debug xusb.cpp  55   id= %d, bResult = %d, ack=0x%x\n", id, bResult, ack);
    if(bResult==FALSE || ack != 0x83) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        //TRACE(_T("XXX (%d) NAND Erase error\n"),id);
        return FALSE;
    }

    erase_pos=0;
    ((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetRange(0,100);
    ((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetBkColor(COLOR_ERASE);
    int wait_pos=0;
    while(erase_pos!=100) {
        if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            //TRACE(_T("XXX (%d) NAND Erase error\n"),id);
            return FALSE;
        }

        //TRACE(_T("(%d) NAND wait erase ack\n"), id);
        Sleep(2);
        bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
        //TRACE("debug xusb.cpp  66   id= %d, bResult = %d, ack=0x%x\n", id, bResult, ack);
        if(ack == 0xffff) {  // Storage error
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            TRACE(_T("XXX (%d) NAND Erase error\n"),id);
            return FALSE;
        }

        if(bResult==FALSE) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            //TRACE(_T("(%d) #5872 NAND Erase error\n"),id);
            return FALSE;
        }

        //TRACE(_T("(%d) NAND wait erase ack end\n"), id);
        //TRACE("debug xusb.cpp  77   ack=0x%x,  ((ack>>16)&0xffff)=0x%x\n", ack, ((ack>>16)&0xffff));
        if(!((ack>>16)&0xffff)) {
            erase_pos=ack&0xffff;
            PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)erase_pos,0);
        } else {

            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            TRACE(_T("XXX (%d) NAND Erase error. ack = 0x%x\n"),id, ack);
            return FALSE;
        }

        if(erase_pos==95) {
            wait_pos++;
            if(wait_pos>100) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                //TRACE(_T("XXX (%d) NAND Erase error\n"),id);
                return FALSE;
            }

        }
    }

    delete []lpBuffer;
    NucUsb.CloseWinUsbDevice(id);
    return TRUE;
}


BOOL FastDlg::XUSB_FastNANDBurn(int id, CString& portName,CString& m_pathName,int *len)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    FILE* fp;
    int pos=0;
    UCHAR burn_pos=0;
    unsigned int total,file_len,scnt,rcnt, totalsize;
    ULONG ack;
    char* lpBuffer;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    bResult=NucUsb.EnableOneWinUsbDevice(id);
    //TRACE("debug xusb.cpp  99   id= %d, bResult = %d \n", id, bResult);
    if(!bResult) {
        //AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }

#if(0)
    bResult=NucUsb.NUC_CheckFw(id);

    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif

    USHORT typeack=0x0;
    bResult=NucUsb.NUC_SetType(id,NAND,(UCHAR *)&typeack,sizeof(typeack));
    //TRACE("debug xusb.cpp  aa   id= %d, bResult = %d \n", id, bResult);
    if(bResult==FALSE) {
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("XXX (%d) NAND Burn NUC_SetType error\n"),id);
        return FALSE;
    }


    fp=_wfopen(m_pathName,_T("rb"));

    if(!fp) {
        NucUsb.CloseWinUsbDevice(id);
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }

    fseek(fp,0,SEEK_END);
    file_len=ftell(fp);
    fseek(fp,0,SEEK_SET);

    if(!file_len) {
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        AfxMessageBox(_T("File length is zero\n"));
        return FALSE;
    }

    unsigned int magic;
    fread((unsigned char *)&magic,4,1,fp);
    if(magic!=0x5) {
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        //AfxMessageBox(_T("Pack Image Format Error\n"));
        return ERR_PACK_FORMAT;
    }
    fseek(fp,0,SEEK_SET);

    lpBuffer = new char[file_len]; //read file to buffer
    memset(lpBuffer,0x00,file_len);

    memset((unsigned char*)m_fhead_nand,0,sizeof(NORBOOT_NAND_HEAD));
    total=0;
    m_fhead_nand->flag=PACK_ACTION;
    m_fhead_nand->type=m_type;
    m_fhead_nand->filelen=file_len;
    memcpy(lpBuffer,(unsigned char*)m_fhead_nand,sizeof(NORBOOT_NAND_HEAD));
    bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));

    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        TRACE(_T("XXX (%d) NAND Burn error\n"),id);
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        TRACE(_T("XXX (%d) NAND Burn error\n"),id);
        return FALSE;
    }

    fread(lpBuffer,m_fhead_nand->filelen,1,fp);
    if(lpBuffer[0]!=0x5) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        //AfxMessageBox(_T("This file is not pack image"));
        return ERR_PACK_FORMAT;
    }
    fclose(fp);

	// Check DDR *ini
	bResult=CheckDDRiniData(lpBuffer, m_fhead_nand->filelen);
	if(bResult==FALSE) {
		delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
		//AfxMessageBox(_T("DDR Init select error\n"));
		return ERR_DDRINI_DATACOM;
    }

    char *pbuf = lpBuffer;
    PACK_HEAD *ppackhead=(PACK_HEAD *)lpBuffer;
    bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)pbuf, sizeof(PACK_HEAD));
    //Sleep(5);
    if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("XXX (%d) NAND Burn error\n"),id);
        return FALSE;
    }

    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("XXX (%d) NAND Burn error\n"),id);
        return FALSE;
    }

    bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("XXX (%d) NAND Burn error\n"),id);
        return FALSE;
    }

    total+= sizeof(PACK_HEAD);
    pbuf+= sizeof(PACK_HEAD);
    PACK_CHILD_HEAD child;
    ((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetRange(0,short(ppackhead->num*100));
    int posnum=0;
    int prepos=0;

    unsigned int blockNum;
    totalsize = 0;
    for(int i=0; i<(int)(ppackhead->num); i++) {
        total=0;
        memcpy(&child,(char *)pbuf,sizeof(PACK_CHILD_HEAD));
        bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)pbuf, sizeof(PACK_CHILD_HEAD));
        //Sleep(1);
        if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            TRACE(_T("XXX (%d) NAND Burn error\n"),id);
            return FALSE;
        }
        if(bResult==FALSE) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            TRACE(_T("XXX (%d) NAND Burn error\n"),id);
            return FALSE;
        }

        bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
        if(bResult==FALSE) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            TRACE(_T("XXX (%d) NAND Burn error\n"),id);
            return FALSE;
        }

        pbuf+= sizeof(PACK_CHILD_HEAD);
        //Sleep(1);
        scnt=child.filelen/BUF_SIZE;
        rcnt=child.filelen%BUF_SIZE;
        total=0;
        while(scnt>0) {
            //Sleep(1);
            bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)pbuf, BUF_SIZE);
            if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
                //TRACE("event 01\n");
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) NAND Burn error! scnt = %d\n"),id, scnt);
                return FALSE;
            }
            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) NAND Burn error! scnt = %d\n"),id, scnt);
                return FALSE;
            }

            if(bResult==TRUE) {
                pbuf+=BUF_SIZE;
                total+=BUF_SIZE;
                totalsize += BUF_SIZE;

                pos=(int)(((float)(((float)totalsize/(float)file_len))*100));

                bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
                if(bResult==FALSE || ack==0xffff) {  // Storage error
                    delete []lpBuffer;
                    NucUsb.CloseWinUsbDevice(id);
                    TRACE(_T("XXX (%d) NAND Burn error! scnt = %d, ack= 0x%x\n"),id, scnt, ack);
                    return FALSE;
                }
            }

            if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) WaitForSingleObject error! scnt = %d, ack= 0x%x\n"),id, scnt, ack);
                return FALSE;
            }

            scnt--;

            if((pos!=prepos) || (pos==100)) {
                prepos=pos;
                if((scnt % 4 == 0) || pos == 100) {
                    PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)(pos),0);
                }
            }

        }
        if(rcnt>0) {
            //Sleep(20);
            bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)pbuf,rcnt);
            if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) WaitForSingleObject error! rcnt = %d, ack= 0x%x\n"),id, rcnt, ack);
                return FALSE;
            }

            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) NAND Burn error! rcnt = %d\n"),id, rcnt);
                return FALSE;
            }

            if(bResult==TRUE) {
                pbuf+=rcnt;
                total+=rcnt;
                totalsize +=rcnt;

                bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
                if(bResult==FALSE) {
                    delete []lpBuffer;
                    NucUsb.CloseWinUsbDevice(id);
                    TRACE(_T("XXX (%d) NAND Burn error! rcnt = %d\n"),id, rcnt);
                    return FALSE;
                }
            }
            //TRACE("OneDeviceXUSB_Pack:device %d,08\n",id);
            if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) WaitForSingleObject error! rcnt = %d\n"),id, rcnt);
                return FALSE;
            }

            //pos=(int)(((float)(((float)total/(float)child.filelen))*100));
            pos=(int)(((float)(((float)totalsize/(float)file_len))*100));
            if(pos>=100) {
                pos=100;
            }
            posstr.Format(_T("%d%%"),pos);
            if((pos!=prepos) || (pos==100)) {
                prepos=pos;
                if(pos%5==0) {
                    PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)pos,0);
                }
            }

        }
        //posnum+=100;
        bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&blockNum,4);
        if(bResult==FALSE) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            TRACE(_T("XXX (%d) XUSB_FastNANDBurn failed!\n"), id);
            return FALSE;
        }
    }//for(int i=0;i<(int)(ppackhead->num);i++)

    delete []lpBuffer;
    NucUsb.CloseWinUsbDevice(id);
    return TRUE;
}

int FastDlg::XUSB_FastNANDVerify(int id, CString& portName, CString& m_pathName)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    FILE* fp;
    int pos=0;
    unsigned int total=0,file_len,scnt,rcnt,ack;
    char* lpBuffer;
    char temp[BUF_SIZE];
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)0,0);

    /***********************************************/
    bResult=NucUsb.EnableOneWinUsbDevice(id);
    if(bResult==FALSE) {
        //AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(id);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack;
    bResult=NucUsb.NUC_SetType(id,NAND,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.CloseWinUsbDevice(id);
        return FALSE;
    }

    fp=_wfopen(m_pathName,_T("rb"));
    if(!fp) {
        NucUsb.CloseWinUsbDevice(id);
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }

    fseek(fp,0,SEEK_END);
    file_len=ftell(fp);
    fseek(fp,0,SEEK_SET);


    if(!file_len) {
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        AfxMessageBox(_T("File length is zero\n"));
        return FALSE;
    }


#if !defined(BATCH_BURN)

#else
    lpBuffer = new char[file_len]; //read file to buffer
    memset(lpBuffer,0x00,file_len);

    fread(lpBuffer,file_len,1,fp);
    PACK_HEAD *ppackhead =(PACK_HEAD *)lpBuffer;

    int imagenum = 0;
    ppackhead->fileLength = ((lpBuffer[7]&0xff) << 24 | (lpBuffer[6]&0xff) << 16 | (lpBuffer[5]&0xff) << 8 | (lpBuffer[4]&0xff));

    ppackhead->num = ((lpBuffer[11]&0xff) << 24 | (lpBuffer[10]&0xff) << 16 | (lpBuffer[9]&0xff) << 8 | (lpBuffer[8]&0xff));
    imagenum = ppackhead->num;


    char *pbuf = lpBuffer;
    total+= sizeof(PACK_HEAD);
    pbuf+= sizeof(PACK_HEAD);

    PACK_CHILD_HEAD child;
    ((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetRange(0,100);
    ((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetBkColor(COLOR_VERIFY);
    int posnum=0;
    int prepos=0;
    int uBootHeadLen = 16;

    memset(m_fhead_nand, 0x00, sizeof(NORBOOT_NAND_HEAD));
    m_fhead_nand->flag=PACK_VERIFY_ACTION;
    //TRACE(_T("%x  %x  %x %x\n"),lpBuffer[16], lpBuffer[17], lpBuffer[18], lpBuffer[19]);
    m_fhead_nand->filelen= ((lpBuffer[19]&0xff) << 24 | (lpBuffer[18]&0xff) << 16 | (lpBuffer[17]&0xff) << 8 | (lpBuffer[16]&0xff)); // child1 file len
    m_fhead_nand->flashoffset = ((lpBuffer[23]&0xff) << 24 | (lpBuffer[22]&0xff) << 16 | (lpBuffer[21]&0xff) << 8 | (lpBuffer[20]&0xff)); // child1 start address
    m_fhead_nand->type = ((lpBuffer[27]&0xff) << 24 | (lpBuffer[26]&0xff) << 16 | (lpBuffer[25]&0xff) << 8 | (lpBuffer[24]&0xff)); // child1 image type
    m_fhead_nand->no = imagenum;
    memcpy(temp,(unsigned char*)m_fhead_nand,sizeof(NORBOOT_NAND_HEAD));
    bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)temp, sizeof(NORBOOT_NAND_HEAD));

    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        TRACE(_T("XXX (%d) NAND Verify Write head error\n"), id);
        return FALSE;
    }
    //Sleep(5);
    bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        TRACE(_T("XXX (%d) NAND Verify Read head error !\n"), id);
        return FALSE;
    }

    for(int i=0; i<(int)imagenum; i++) {
        memcpy(&child,(char *)pbuf,sizeof(PACK_CHILD_HEAD));
        memcpy(temp,(char *)pbuf,sizeof(PACK_CHILD_HEAD));
        bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)temp, sizeof(PACK_CHILD_HEAD));
        if(bResult==FALSE) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            fclose(fp);
            TRACE(_T("XXX (%d) NAND Verify error\n"), id, i);
            return FALSE;
        }
        //Sleep(5);
        bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
        if(bResult==FALSE) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            fclose(fp);
            TRACE(_T("XXX (%d) NAND Verify error\n"), id);
            return FALSE;
        }
        total+= sizeof(PACK_CHILD_HEAD);
        pbuf+= sizeof(PACK_CHILD_HEAD);

        int ddrlen;
        //UCHAR * ddrbuf;
        if(child.imagetype == UBOOT) {
            //ddrbuf=DDR2Buf(mainWnd->DDRBuf,mainWnd->DDRLen,&ddrlen);
            ddrlen = (((mainWnd->DDRLen+8+15)/16)*16);
            pbuf += 16 + mainWnd->DDRLen + uBootHeadLen;
            total+= 16 + mainWnd->DDRLen + uBootHeadLen;

            // send DDR parameter Length
            bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)&ddrlen,4);
            //Sleep(5);
            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                fclose(fp);
                TRACE(_T("XXX (%d) NAND Verify error\n"), id);
                return FALSE;
            }

            bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                fclose(fp);
                TRACE(_T("XXX (%d) NAND Verify error\n"), id);
                return FALSE;
            }
        }

        if(child.imagetype == UBOOT) {
            scnt=(child.filelen-ddrlen-16)/BUF_SIZE;
            rcnt=(child.filelen-ddrlen-16)%BUF_SIZE;
        } else {
            scnt=child.filelen/BUF_SIZE;
            rcnt=child.filelen%BUF_SIZE;
        }
        //TRACE("child.filelen = %d, scnt=%d, rcnt =%d\n", child.filelen, scnt, rcnt);
        int prepos=0;
        while(scnt>0) {
            //TRACE("scnt=0x%x(%d)\n", scnt, scnt);
            if(child.imagetype !=ENV) {
                fseek(fp,total,SEEK_SET);
                fread(temp,BUF_SIZE,1,fp);
            } else {
                fseek(fp,total,SEEK_SET);
                fread(temp,BUF_SIZE,1,fp);
            }
            bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)lpBuffer, BUF_SIZE);
            if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                fclose(fp);
                TRACE(_T("XXX (%d) NAND Verify error. scnt=%d\n"), id, scnt);
                return FALSE;
            }

            if(bResult==TRUE) {
                total+=BUF_SIZE;
                pbuf+=BUF_SIZE;

                pos=(int)(((float)(((float)total/(float)file_len))*100));
                if(DataCompare(temp,lpBuffer,BUF_SIZE))
                    ack=BUF_SIZE;
                else
                    ack=0;//compare error

                bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)&ack,4);
                if((bResult==FALSE)||(!ack)) {
                    delete []lpBuffer;
                    NucUsb.CloseWinUsbDevice(id);
                    fclose(fp);
                    TRACE(_T("XXX (%d) NAND Verify error. scnt=%d\n"), id, scnt);
                    return ERR_VERIFY_DATACOM;
                }

            } else {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                fclose(fp);
                TRACE(_T("XXX (%d) NAND Verify error. scnt=%d\n"), id, scnt);
                return FALSE;
            }

            if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                fclose(fp);
                TRACE(_T("XXX (%d) WaitForSingleObject error. scnt=%d\n"), id, scnt);
                return FALSE;
            }

            scnt--;

            if((pos!=prepos) || (pos==100)) {
                prepos=pos;
                PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)pos,0);
            }

        }

        int temp_len = 0;
        if(rcnt>0) {
            memset((char *)&temp, 0xff, BUF_SIZE);

            if(child.imagetype != ENV) {
                fseek(fp,total,SEEK_SET);
                fread(temp,rcnt,1,fp);
                temp_len = rcnt;
            } else {
                fseek(fp,total,SEEK_SET);
                fread(temp,BUF_SIZE,1,fp);
                temp_len = BUF_SIZE;
            }

            bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)lpBuffer,BUF_SIZE);
            if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                fclose(fp);
                TRACE(_T("XXX (%d) WaitForSingleObject error. rcnt=%d\n"), id, rcnt);
                return FALSE;
            }

            if(bResult==TRUE) {
                total+=temp_len;
                pbuf+=temp_len;
                if(DataCompare(temp,lpBuffer,temp_len))
                    ack=BUF_SIZE;
                else
                    ack=0;//compare error

                bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)&ack, 4);
                if((bResult==FALSE)||(!ack)) {
                    delete []lpBuffer;
                    NucUsb.CloseWinUsbDevice(id);
                    fclose(fp);
                    TRACE(_T("XXX (%d) NAND Verify error. rcnt=%d  ack= 0x%x\n"), id, rcnt, ack);
                    return ERR_VERIFY_DATACOM;

                }

            } else {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                fclose(fp);
                TRACE(_T("XXX (%d) NAND Verify error. rcnt=%d\n"), id, rcnt);
                return FALSE;
            }

            if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                fclose(fp);
                TRACE(_T("XXX (%d) NAND Verify error. rcnt=%d\n"), id, rcnt);
                return FALSE;
            }

            pos=(int)(((float)(((float)total/(float)file_len))*100));
            if(pos>=100) {
                pos=100;
            }
            if((pos!=prepos) || (pos==100)) {
                prepos=pos;
                PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)pos,0);
            }
        }
    }

    delete []lpBuffer;
    fclose(fp);
    NucUsb.CloseWinUsbDevice(id);
    return TRUE;
#endif
}

BOOL FastDlg::XUSB_FastSPIErase(int id, CString& portName)
{
    BOOL bResult;
    CString tempstr;
    int count=0;
    unsigned int ack,erase_pos=0;
    NORBOOT_NAND_HEAD *fhead;
    char* lpBuffer;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    fhead=(NORBOOT_NAND_HEAD *)malloc(sizeof(NORBOOT_NAND_HEAD));

    /***********************************************/
    bResult=NucUsb.EnableOneWinUsbDevice(id);
    if(bResult==FALSE) {
        //AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        free(fhead);
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(id);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        free(fhead);
        return FALSE;
    }
#endif
    USHORT typeack;
    bResult=NucUsb.NUC_SetType(id,SPI,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.CloseWinUsbDevice(id);
        free(fhead);
        TRACE(_T("XXX (%d) SPI NUC_SetType error.\n"), id);
        return FALSE;
    }

    lpBuffer = new char[BUF_SIZE];
    memset((unsigned char*)fhead,0,sizeof(NORBOOT_NAND_HEAD));
    fhead->flag=ERASE_ACTION;
    fhead->flashoffset = 0; //start erase block
    fhead->execaddr=1; //erase block length
    fhead->type=0; // Decide chip erase mode
    fhead->no=0xffffffff;//erase all

    memcpy(lpBuffer,(unsigned char*)fhead,sizeof(NORBOOT_NAND_HEAD));
    bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));
    free(fhead);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("XXX (%d) SPI Erase head error.\n"), id);
        return FALSE;
    }

    bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
    if(bResult==FALSE || ack!=0x85) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("XXX (%d) SPI Erase head error. ack= 0x%x\n"), id, ack);
        return FALSE;
    }

    erase_pos=0;
    int wait_pos=0;
    ((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetRange(0,100);

    while(erase_pos!=100) {
        if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            TRACE(_T("XXX (%d) SPI Erase WaitForSingleObject error"), id);
            return FALSE;
        }

        //TRACE(_T("SPI Erase (%d) wait erase ack\n"), id);
        //Sleep(10);
        bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
        if(bResult=FALSE || ack==0xFFFFFFFF) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            TRACE(_T("XXX (%d) SPI Erase error. ack= 0x%x\n"), id, ack);
            return FALSE;
        }

        //TRACE(_T("SPI Erase (%d) wait erase ack end\n"), id);
        if(!((ack>>16)&0xffff)) {
            erase_pos=ack&0xffff;
            PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)erase_pos,0);
        } else {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            TRACE(_T("XXX (%d) SPI Erase error. ack= 0x%x\n"), id, ack);
            return FALSE;
        }

        if(erase_pos==95) {
            wait_pos++;
            if(wait_pos>100) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) SPI Erase error.\n"), id);
                return FALSE;
            }

        }
    }

    delete []lpBuffer;
    NucUsb.CloseWinUsbDevice(id);
    return TRUE;
}

BOOL FastDlg::XUSB_FastSPIBurn(int id, CString& portName,CString& m_pathName,int *len)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    FILE* fp;
    int pos=0;
    UCHAR burn_pos=0;
    unsigned int total,file_len,scnt,rcnt,totalsize;
    ULONG ack;
    char* lpBuffer;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    bResult=NucUsb.EnableOneWinUsbDevice(id);
    if(!bResult) {
        //AfxMessageBox(_T("Device Enable error\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(id);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack=0x0;
    bResult=NucUsb.NUC_SetType(id,SPI,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("XXX (%d) SPI Burn NUC_SetType error.\n"), id);
        return FALSE;
    }

    fp=_wfopen(m_pathName,_T("rb"));
    if(!fp) {
        NucUsb.CloseWinUsbDevice(id);
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }

    fseek(fp,0,SEEK_END);
    file_len=ftell(fp);
    fseek(fp,0,SEEK_SET);

    if(!file_len) {
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        AfxMessageBox(_T("File length is zero\n"));
        return FALSE;
    }

    unsigned int magic;
    fread((unsigned char *)&magic,4,1,fp);
    if(magic!=0x5) {
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        AfxMessageBox(_T("Pack Image Format Error\n"));
        return ERR_PACK_FORMAT;
    }

    fseek(fp,0,SEEK_SET);
    lpBuffer = new char[file_len]; //read file to buffer
    memset(lpBuffer,0x00,file_len);
    memset((unsigned char*)m_fhead_spi,0,sizeof(NORBOOT_NAND_HEAD));
    total=0;
    m_fhead_spi->flag=PACK_ACTION;
    m_fhead_spi->type=m_type;
    m_fhead_spi->filelen=file_len;
    memcpy(lpBuffer,(unsigned char*)m_fhead_spi,sizeof(NORBOOT_NAND_HEAD));
    bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)lpBuffer, sizeof(NORBOOT_NAND_HEAD));
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        TRACE(_T("XXX (%d) SPI Burn error. 0x%x\n"), id, GetLastError());
        return FALSE;
    }
    bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        TRACE(_T("XXX (%d) SPI Burn error. 0x%x\n"), id, GetLastError());
        return FALSE;
    }

    fread(lpBuffer,m_fhead_spi->filelen,1,fp);
    if(lpBuffer[0]!=0x5) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        //AfxMessageBox(_T("This file is not pack image"));
        return ERR_PACK_FORMAT;
    }
    fclose(fp);

	// Check DDR *ini
	bResult=CheckDDRiniData(lpBuffer, m_fhead_spi->filelen);
	if(bResult==FALSE) {
		delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
		//AfxMessageBox(_T("DDR Init select error\n"));
		return ERR_DDRINI_DATACOM;
    }

    char *pbuf = lpBuffer;
    PACK_HEAD *ppackhead=(PACK_HEAD *)lpBuffer;
    bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)pbuf, sizeof(PACK_HEAD));
    //Sleep(5);
    if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("XXX (%d) SPI Burn WaitForSingleObject error. 0x%x\n"), id, GetLastError());
        return FALSE;
    }
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("XXX (%d) SPI Burn error. 0x%x\n"), id, GetLastError());
        return FALSE;
    }

    bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("XXX (%d) SPI Burn error. 0x%x\n"), id, GetLastError());
        return FALSE;
    }

    total+= sizeof(PACK_HEAD);
    pbuf+= sizeof(PACK_HEAD);
    PACK_CHILD_HEAD child;
    ((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetRange(0,short(ppackhead->num*200));
    //((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetRange(0,100);

    int posnum=0;
    int prepos=0;
    totalsize = 0;
    for(int i=0; i<(int)(ppackhead->num); i++) {
        memcpy(&child,(char *)pbuf,sizeof(PACK_CHILD_HEAD));
        //Sleep(20);
        bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)pbuf, sizeof(PACK_CHILD_HEAD));
        if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            TRACE(_T("XXX (%d) SPI Burn WaitForSingleObject error. 0x%x\n"), id, GetLastError());
            return FALSE;
        }
        if(bResult==FALSE) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            TRACE(_T("XXX (%d) SPI Burn error. 0x%x\n"), id, GetLastError());
            return FALSE;
        }

        total+= sizeof(PACK_CHILD_HEAD);
        pbuf+= sizeof(PACK_CHILD_HEAD);
        scnt=child.filelen/BUF_SIZE;
        rcnt=child.filelen%BUF_SIZE;
        //TRACE(_T("(%d) #6768 SPI Burn scnt = %d  rcnt %d \n"), id, scnt, rcnt);
        while(scnt>0) {
            //Sleep(20);
            bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)pbuf, BUF_SIZE);
            if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) WaitForSingleObject SPI Burn error. scnt = %d  0x%x \n"), id, scnt, GetLastError());
                return FALSE;
            }
            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) SPI Burn error. scnt = %d  0x%x \n"), id, scnt, GetLastError());
                return FALSE;
            }

            if(bResult==TRUE) {
                pbuf+=BUF_SIZE;
                total+=BUF_SIZE;
                totalsize+=BUF_SIZE;

                pos=(int)(((float)(((float)total/(float)child.filelen))*100));
                //pos=(int)(((float)(((float)totalsize/(float)file_len))*100));
                posstr.Format(_T("%d%%"),pos);

                bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
                if(bResult==FALSE || ack==0xffff) {  // Storage error
                    delete []lpBuffer;
                    NucUsb.CloseWinUsbDevice(id);
                    TRACE(_T("XXX (%d) SPI Burn error. scnt = %d, ack= 0x%x, 0x%x\n"), id, scnt, ack, GetLastError());
                    return FALSE;
                }
            }

            if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) SPI Burn WaitForSingleObject error. scnt = %d, 0x%x\n"), id, scnt, GetLastError());
                return FALSE;
            }

            scnt--;

            if(pos%5==0) {
                //prepos=pos;
                PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)(posnum+pos),0);
            }

#if(0)
            if((pos!=prepos) || (pos==100)) {
                prepos=pos;
                if((scnt % 4 == 0) || pos == 100) {
                    PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)(pos),0);
                }
            }
#endif
        }

        if(rcnt>0) {
            //Sleep(20);
            bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)pbuf,rcnt);
            if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) SPI Burn WaitForSingleObject error. rcnt = %d, 0x%x\n"), id, rcnt, GetLastError());
                return FALSE;
            }
            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) SPI Burn error. rcnt = %d, 0x%x\n"), id, rcnt, GetLastError());
                return FALSE;
            }

            if(bResult==TRUE) {
                pbuf+=rcnt;
                total+=rcnt;
                totalsize+=rcnt;
                //TRACE(_T("SPI (%d) wait Burn rcnt(%d) ack\n"), id, rcnt);
                bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
                //TRACE(_T("SPI (%d) wait Burn rcnt ack\n"), id);

                if(bResult==FALSE) {
                    delete []lpBuffer;
                    NucUsb.CloseWinUsbDevice(id);
                    TRACE(_T("XXX (%d) SPI Burn error. rcnt = %d, 0x%x\n"), id, rcnt, GetLastError());
                    return FALSE;
                }
            }

            if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) SPI Burn WaitForSingleObject error. rcnt = %d, 0x%x\n"), id, rcnt, GetLastError());
                return FALSE;
            }
#if(0)
            //pos=(int)(((float)(((float)total/(float)child.filelen))*100));
            pos=(int)(((float)(((float)totalsize/(float)file_len))*100));
            if(pos>=100) {
                pos=100;
            }
            posstr.Format(_T("%d%%"),pos);

            if(pos%5==0) {
               PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)(posnum+pos),0);
            	//PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)pos,0);
            }
            // if((pos!=prepos) || (pos==100)) {
                // prepos=pos;
                // if(pos%5==0) {
                    // PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)pos,0);
                // }
            // }
#else
            pos=(int)(((float)(((float)total/(float)file_len))*100));

            if(pos>=100) {
                pos=100;
            }
            posstr.Format(_T("%d%%"),pos);

            if(pos%5==0) {

                PostMessage(WM_SPI_PROGRESS,(LPARAM)(posnum+pos),0);
            }			
#endif
        }

#if(1)
        posnum+=100;
        burn_pos=0;

        while(burn_pos!=100) {
            //while(burn_pos!=posnum) {
            if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                return FALSE;
            }

            DbgOut("SPI wait burn ack");
            bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                //AfxMessageBox(_T("ACK error !"));
                return FALSE;
            }
            DbgOut("SPI wait burn ack end");
            if(!((ack>>16)&0xffff)) {
                burn_pos=(UCHAR)(ack&0xffff);
                //TRACE("posstr  %d %d \n", burn_pos, posnum);
                PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)(posnum+burn_pos),0);
            } else {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                //AfxMessageBox(_T("Burn error"));
                return FALSE;
            }
        }
        posnum+=100;
#endif
    }

    delete []lpBuffer;
    NucUsb.CloseWinUsbDevice(id);
    return TRUE;
}


//SPI Verify
int FastDlg::XUSB_FastSPIVerify(int id, CString& portName, CString& m_pathName)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    FILE* fp;
    int pos=0;
    unsigned int total=0,file_len,scnt,rcnt,ack,totalsize;
    char* lpBuffer;
    char temp[BUF_SIZE];
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)0,0);

    /***********************************************/
    bResult=NucUsb.EnableOneWinUsbDevice(id);
    if(bResult==FALSE) {
        //AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(id);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack;
    bResult=NucUsb.NUC_SetType(id,SPI,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.CloseWinUsbDevice(id);
        return FALSE;
    }

    fp=_wfopen(m_pathName,_T("rb"));
    if(!fp) {
        NucUsb.CloseWinUsbDevice(id);
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }

    fseek(fp,0,SEEK_END);
    file_len=ftell(fp);
    fseek(fp,0,SEEK_SET);

    if(!file_len) {
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        AfxMessageBox(_T("File length is zero\n"));
        return FALSE;
    }

    lpBuffer = new char[file_len]; //read file to buffer
    memset(lpBuffer,0x00,file_len);

    fread(lpBuffer,file_len,1,fp);
    PACK_HEAD *ppackhead =(PACK_HEAD *)lpBuffer;

    int imagenum = 0;
    ppackhead->fileLength = (lpBuffer[7] << 24 | lpBuffer[6] << 16 | lpBuffer[5] << 8 | lpBuffer[4]);
    ppackhead->num = (lpBuffer[11] << 24 | lpBuffer[10] << 16 | lpBuffer[9] << 8 | lpBuffer[8]);
    imagenum = ppackhead->num;

    char *pbuf = lpBuffer;
    total+= sizeof(PACK_HEAD);
    pbuf+= sizeof(PACK_HEAD);

    PACK_CHILD_HEAD child;
    ((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetRange(0,100);
    //((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetBkColor(COLOR_VERIFY);
    int posnum=0;
    int prepos=0;
    int uBootHeadLen = 16;

    memset(m_fhead_spi, 0x00, sizeof(NORBOOT_NAND_HEAD));
    m_fhead_spi->flag=PACK_VERIFY_ACTION;
    //TRACE("lpBuffer[16~19] = 0x%x, 0x%x, 0x%x, 0x%x\n", (lpBuffer[16] & 0xff), lpBuffer[17], lpBuffer[18], lpBuffer[19]);
    m_fhead_spi->filelen= ((lpBuffer[19] & 0xff) << 24 | (lpBuffer[18] & 0xff) << 16 | (lpBuffer[17] & 0xff) << 8 | (lpBuffer[16] & 0xff));//216452;//(lpBuffer[19] << 24 | lpBuffer[18] << 16 | lpBuffer[17] << 8 | lpBuffer[16]); // child1 file len
    m_fhead_spi->flashoffset = ((lpBuffer[23] & 0xff) << 24 | (lpBuffer[22] & 0xff) << 16 | (lpBuffer[21] & 0xff) << 8 | (lpBuffer[20] & 0xff)); // child1 start address
    //m_fhead_spi->execaddr = 0;// Struct without execaddr
    m_fhead_spi->type = (lpBuffer[27] << 24 | lpBuffer[26] << 16 | lpBuffer[25] << 8 | lpBuffer[24]); // child1 image type
    m_fhead_spi->no = imagenum;
    memcpy(temp,(unsigned char*)m_fhead_spi,sizeof(NORBOOT_NAND_HEAD));
    bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)temp, sizeof(NORBOOT_NAND_HEAD));

    if(bResult==FALSE) {
        fclose(fp);
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        //AfxMessageBox(_T("XXX (%d) Write SPI head error\n"), id);
        return FALSE;
    }
    Sleep(5);
    bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        fclose(fp);
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        //AfxMessageBox(_T("ACK error !"));
        return FALSE;
    }

    totalsize = 0;
    for(int i=0; i<(int)imagenum; i++) {
        memcpy(&child,(char *)pbuf,sizeof(PACK_CHILD_HEAD));

        memcpy(temp,(char *)pbuf,sizeof(PACK_CHILD_HEAD));
        bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)temp, sizeof(PACK_CHILD_HEAD));

        if(bResult==FALSE) {
            fclose(fp);
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            //AfxMessageBox(_T("Write SPI head error\n"));
            return FALSE;
        }

        //Sleep(5);
        bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
        if(bResult==FALSE) {
            fclose(fp);
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            //AfxMessageBox(_T("ACK error !"));
            return FALSE;
    }
    total+= sizeof(PACK_CHILD_HEAD);
    pbuf+= sizeof(PACK_CHILD_HEAD);

    int ddrlen;
    //UCHAR * ddrbuf;
    if(child.imagetype == UBOOT) {
        //ddrbuf=DDR2Buf(mainWnd->DDRBuf,mainWnd->DDRLen,&ddrlen);
        ddrlen = (((mainWnd->DDRLen+8+15)/16)*16);
        pbuf += 16 + mainWnd->DDRLen + uBootHeadLen;
        total+= 16 + mainWnd->DDRLen + uBootHeadLen;
    }


    if(child.imagetype == UBOOT) {
        // send DDR parameter Length
        bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)&ddrlen,4);
        Sleep(5);
        if(bResult==FALSE) {
            fclose(fp);
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            //AfxMessageBox(_T("Write SPI head error\n"));
            return FALSE;
        }

        bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
        if(bResult==FALSE || ack!=0x85) {
            fclose(fp);
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            //AfxMessageBox(_T("ACK error !"));
            return FALSE;
        }

        scnt=(child.filelen-ddrlen-16)/BUF_SIZE;
        rcnt=(child.filelen-ddrlen-16)%BUF_SIZE;
    } else {
        scnt=child.filelen/BUF_SIZE;
        rcnt=child.filelen%BUF_SIZE;
    }

    int prepos=0;
    while(scnt>0) {
    
        if(child.imagetype !=ENV) {
            fseek(fp,total,SEEK_SET);
            fread(temp,BUF_SIZE,1,fp);
        } else {
            fseek(fp,total,SEEK_SET);
            fread(temp,BUF_SIZE,1,fp);
        }
        bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)lpBuffer, BUF_SIZE);
        if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            fclose(fp);
            NucUsb.CloseWinUsbDevice(id);
            return FALSE;
        }
    
        if(bResult==TRUE) {
            total+=BUF_SIZE;
            pbuf+=BUF_SIZE;
            totalsize += BUF_SIZE;
    
            pos=(int)(((float)(((float)total/(float)file_len))*100));
            posstr.Format(_T("%d%%"),pos);
    
            if(DataCompare(temp,lpBuffer,BUF_SIZE))
                ack=BUF_SIZE;
            else
                ack=0;//compare error
            bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)&ack,4);
            if((bResult==FALSE)||(!ack)) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                fclose(fp);
                //return FALSE;
				return ERR_VERIFY_DATACOM;
            }
    
        } else {
            fclose(fp);
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            return FALSE;
        }
    
        if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
            fclose(fp);
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            return FALSE;
        }

        scnt--;
#if(0)
        if((pos!=prepos) || (pos==100)) {
            prepos=pos;
            if((scnt % 4 == 0) || pos == 100) {
                PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)(pos),0);
            }
#endif
            if(pos%5==0) {
                prepos=pos;
                PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)pos,0);
            }
        }

        if(rcnt>0) {
            memset((char *)&temp, 0xff, BUF_SIZE);
            if(m_fhead_spi->type != ENV) {
                fseek(fp,total,SEEK_SET);
                fread(temp,rcnt,1,fp);
            } else {
                fseek(fp,total,SEEK_SET);
                fread(temp,BUF_SIZE,1,fp);
            }
            bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)lpBuffer,BUF_SIZE);
            if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
                fclose(fp);
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                return FALSE;
            }

            if(bResult==TRUE) {
                total+=rcnt;
                pbuf+=rcnt;
                totalsize += rcnt;
                if(DataCompare(temp,lpBuffer,rcnt))
                    ack=BUF_SIZE;
                else
                    ack=0;//compare error

                bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)&ack, 4);
                if((bResult==FALSE)||(!ack)) {
                    fclose(fp);
                    delete []lpBuffer;
                    NucUsb.CloseWinUsbDevice(id);
                    //return FALSE;
					return ERR_VERIFY_DATACOM;

                }

            } else {
                fclose(fp);
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                return FALSE;
            }

            if(WaitForSingleObject(m_ExitEventBurn[id], 0) != WAIT_TIMEOUT) {
                fclose(fp);
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                return FALSE;
            }

            pos=(int)(((float)(((float)total/(float)file_len))*100));
            //pos=(int)(((float)(((float)totalsize/(float)file_len))*100));

            if(pos>=100) {
                pos=100;
            }
            posstr.Format(_T("%d%%"),pos);

            //if((pos!=prepos) || (pos==100)) {
            if(pos%5==0) {
                prepos=pos;
                PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)pos,0);
            }
        }
    }

    fclose(fp);
    delete []lpBuffer;
    NucUsb.CloseWinUsbDevice(id);
    return TRUE;

}


BOOL FastDlg::XUSB_FasteMMCErase(int id, CString& portName)
{
    BOOL bResult;
    CString tempstr;
    int count=0;
    DWORD  iRet = 0x00;
    unsigned int ack,erase_pos=0;
    NORBOOT_MMC_HEAD *fhead;
    char* lpBuffer;

    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    fhead=(NORBOOT_MMC_HEAD *)malloc(sizeof(NORBOOT_MMC_HEAD));

    //TRACE(_T("XUSB_FasteMMCErase start (%d)\n"), id);
    /***********************************************/
    bResult=NucUsb.EnableOneWinUsbDevice(id);
    if(bResult==FALSE) {
        free(fhead);
        //AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(id);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        free(fhead);
        return FALSE;
    }
#endif
    USHORT typeack=0x0;
    bResult=NucUsb.NUC_SetType(id,MMC,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        free(fhead);
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("XXX Erase NUC_SetType error !!!\n"));
        return FALSE;
    }

    lpBuffer = new char[BUF_SIZE];
    memset((unsigned char*)fhead,0,sizeof(NORBOOT_MMC_HEAD));
    fhead->flag=FORMAT_ACTION;
    fhead->ReserveSize = 1;
    memcpy(lpBuffer,(unsigned char*)fhead,sizeof(NORBOOT_MMC_HEAD));
    free(fhead);

    bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)lpBuffer, sizeof(NORBOOT_MMC_HEAD));
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("(%d) XXX Erase eMMC head error\n"), id);
        return FALSE;
    }

    bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
    if(bResult==FALSE || ack!=0x85) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("(%d) XXX Erase Read ACK error ack= 0x%x   0x%x\n"), id, ack, GetLastError());
        return FALSE;
    }

    erase_pos=0;
    int wait_pos=0;
    ((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetRange(0,100);
    ((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetBkColor(COLOR_ERASE);

    while(erase_pos!=100) {
        if(WaitForSingleObject(m_ExitEventBurn[id], EMMC_RW_PIPE_TIMEOUT) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            TRACE(_T("XXX (%d) WaitForSingleObject error. 0x%x \n"),id, GetLastError());
            return FALSE;
        }
        Sleep(10);
        bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
        if(bResult=FALSE) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            TRACE(_T("XXX (%d) Erase Read ACK error ack=0x%x,  0x%x\n"), id, ack, GetLastError());
            return FALSE;
        }

        if(!((ack>>16)&0xffff)) {
            erase_pos=ack&0xffff;
            PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)erase_pos,0);
        } else {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            TRACE(_T("XXX (%d) Erase error ack=0x%x,  0x%x\n"), id, ack, GetLastError());
            return FALSE;
        }

        if(erase_pos==95) {
            wait_pos++;
            if(wait_pos>100) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) Erase error wait_pos=%d\n"), id, wait_pos);
                return FALSE;
            }
        }
    }

    delete []lpBuffer;
    NucUsb.CloseWinUsbDevice(id);
    return TRUE;
}

BOOL FastDlg::XUSB_FasteMMCBurn(int id, CString& portName,CString& m_pathName,int *len)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    FILE* fp;
    int pos=0;
    UCHAR burn_pos=0;
    unsigned int total,file_len,scnt,rcnt;
    unsigned int totalsize;
    ULONG ack;
    char* lpBuffer;
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);

    //TRACE(_T("XUSB_FasteMMCBurn start (%d)  ,%s\n"), id, m_pathName);
    bResult=NucUsb.EnableOneWinUsbDevice(id);
    if(!bResult) {
        //AfxMessageBox(_T("XXX Device Enable error\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(id);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack=0x0;
    bResult=NucUsb.NUC_SetType(id,MMC,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("XXX (%d) Burn  NUC_SetType error \n"), id);
        return FALSE;
    }

    fp=_wfopen(m_pathName,_T("rb"));
    if(!fp) {
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        AfxMessageBox(_T("File Open error\n"));
        return FALSE;
    }

    fseek(fp,0,SEEK_END);
    file_len=ftell(fp);
    fseek(fp,0,SEEK_SET);
    if(!file_len) {
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        AfxMessageBox(_T("File length is zero\n"));
        return FALSE;
    }

    unsigned int magic;
    fread((unsigned char *)&magic,4,1,fp);
    if(magic!=0x5) {
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        AfxMessageBox(_T("Pack Image Format Error\n"));
        return ERR_PACK_FORMAT;
    }

    fseek(fp,0,SEEK_SET);
    lpBuffer = new char[file_len]; //read file to buffer
    memset(lpBuffer,0x00,file_len);
    memset((unsigned char*)m_fhead_emmc,0,sizeof(NORBOOT_MMC_HEAD));
    total=0;
    m_fhead_emmc->flag=PACK_ACTION;
    m_fhead_emmc->type=m_type;
    m_fhead_emmc->filelen=file_len;
    memcpy(lpBuffer,(unsigned char*)m_fhead_emmc,sizeof(NORBOOT_MMC_HEAD));
    bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)lpBuffer, sizeof(NORBOOT_MMC_HEAD));
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        TRACE(_T("XXX (%d) eMMC Burn file format error! 0x%x\n"), id, GetLastError());
        return FALSE;
    }

    bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        TRACE(_T("XXX (%d) eMMC Burn Read ACK error! 0x%x\n"), id, GetLastError());
        return FALSE;
    }

    fread(lpBuffer,m_fhead_emmc->filelen,1,fp);
    if(lpBuffer[0]!=0x5) {
        delete []lpBuffer;
        fclose(fp);
        //AfxMessageBox(_T("This file is not pack image"));
        return ERR_PACK_FORMAT;
    }
    fclose(fp);

	// Check DDR *ini
	bResult=CheckDDRiniData(lpBuffer, m_fhead_emmc->filelen);
	if(bResult==FALSE) {
		delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
		//AfxMessageBox(_T("DDR Init select error\n"));
		return ERR_DDRINI_DATACOM;
    }

    char *pbuf = lpBuffer;
    PACK_HEAD *ppackhead=(PACK_HEAD *)lpBuffer;
    bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)pbuf, sizeof(PACK_HEAD));
    Sleep(5);
    if(WaitForSingleObject(m_ExitEventBurn[id], EMMC_RW_PIPE_TIMEOUT) != WAIT_TIMEOUT) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("XXX (%d) WaitForSingleObject error 1\n"),id);
        return FALSE;
    }
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("XXX (%d) eMMC Burn Write error! 0x%x\n"), id, GetLastError());
        return FALSE;
    }

    bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("XXX (%d) eMMC Burn Read error! 0x%x\n"), id, GetLastError());
        return FALSE;
    }

    total+= sizeof(PACK_HEAD);
    pbuf+= sizeof(PACK_HEAD);
    PACK_CHILD_HEAD child;
    ((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetRange(0,100);
    ((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetBkColor(COLOR_BURN);
    int posnum=0;
    int prepos=0;

#if !defined(BATCH_BURN)

#else
    unsigned int blockNum;
    totalsize = 0;
    for(int i=0; i<(int)(ppackhead->num); i++) {
        total=0;
        memcpy(&child,(char *)pbuf,sizeof(PACK_CHILD_HEAD));
        bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)pbuf, sizeof(PACK_CHILD_HEAD));
        if(WaitForSingleObject(m_ExitEventBurn[id], EMMC_RW_PIPE_TIMEOUT) != WAIT_TIMEOUT) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            TRACE(_T("XXX (%d) WaitForSingleObject error 2. 0x%x\n"),id, bResult);
            return FALSE;
        }
        if(bResult==FALSE) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            TRACE(_T("XXX (%d) eMMC Burn Write error! 0x%x\n"), id, GetLastError());
            return FALSE;
        }

        bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
        if(bResult==FALSE) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            TRACE(_T("XXX (%d) eMMC Burn Read error! 0x%x\n"), id, GetLastError());
            return FALSE;
        }

        pbuf+= sizeof(PACK_CHILD_HEAD);
        //Sleep(1);
        scnt=child.filelen/BUF_SIZE;
        rcnt=child.filelen%BUF_SIZE;
        total=0;

        while(scnt>0) {
            //Sleep(10);
            //TRACE(_T("#7459 (%d) eMMC Burn 44444444 scnt %d \n"), id, scnt);
            bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)pbuf, BUF_SIZE);
            if(WaitForSingleObject(m_ExitEventBurn[id], EMMC_RW_PIPE_TIMEOUT) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) WaitForSingleObject error 3, scnt= %d\n"),id, scnt);
                return FALSE;
            }
            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) eMMC Burn Write error. scnt= %d ,0x%x\n"),id, scnt, GetLastError());
                return FALSE;
            }

            if(bResult==TRUE) {
                pbuf+=BUF_SIZE;
                total+=BUF_SIZE;
                totalsize += BUF_SIZE;

                pos=(int)(((float)(((float)totalsize/(float)file_len))*100));

                bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
                if(bResult==FALSE) {
                    delete []lpBuffer;
                    NucUsb.CloseWinUsbDevice(id);
                    CString tmp;
                    tmp.Format(_T("eMMC Burn ACK error imagenum=%d  scnt= %d  0x%x"),i, scnt, GetLastError());
                    TRACE(_T("XXX (%d) %s !\n"), id, tmp);
                    return FALSE;
                }
            }

            if(WaitForSingleObject(m_ExitEventBurn[id], EMMC_RW_PIPE_TIMEOUT) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) WaitForSingleObject error 4. scnt= %d  0x%x\n"),id, scnt, GetLastError());
                return FALSE;
            }
            scnt--;
            if((pos!=prepos) || (pos==100)) {
                prepos=pos;
                if((scnt % 4 == 0) || pos == 100) {
                    PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)(pos),0);
                }
            }
        }

        if(rcnt>0) {
            //Sleep(20);
            //TRACE(_T("#7587 (%d) eMMC Burn 66666666  rcnt = %d\n"), id, rcnt);
            bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)pbuf,rcnt);
            if(WaitForSingleObject(m_ExitEventBurn[id], EMMC_RW_PIPE_TIMEOUT) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) WaitForSingleObject error 5. rcnt = %d  0x%x\n"),id, scnt, GetLastError());
                return FALSE;
            }
            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) eMMC Burn Write error! rcnt = %d  0x%x\n"), id, rcnt, GetLastError());
                return FALSE;
            }

            if(bResult==TRUE) {
                pbuf+=rcnt;
                total+=rcnt;
                totalsize += rcnt;

                bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
                if(bResult==FALSE) {
                    delete []lpBuffer;
                    NucUsb.CloseWinUsbDevice(id);
                    TRACE(_T("XXX (%d) eMMC Burn Read error! rcnt = %d  0x%x\n"), id, rcnt, GetLastError());
                    return FALSE;

                }
            }

            if(WaitForSingleObject(m_ExitEventBurn[id], EMMC_RW_PIPE_TIMEOUT) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                TRACE(_T("XXX (%d) WaitForSingleObject error 6. rcnt = %d  0x%x\n"),id, rcnt, GetLastError());
                return FALSE;
            }

            pos=(int)(((float)(((float)totalsize/(float)file_len))*100));
            if(pos>=100) {
                pos=100;
            }

            //TRACE("(%d) #7629 rcnt %d, pos =%d, prepos =%d\n", id, rcnt, pos, prepos);
            if((pos!=prepos) || (pos==100)) {
                prepos=pos;
                PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)(pos),0);
            }

        }

        bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&blockNum,4);
        //Sleep(10);
        if(bResult==FALSE) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            TRACE(_T("XXX (%d) eMMC Burn Read error. rcnt = %d  0x%x\n"), id, rcnt, GetLastError());
            return FALSE;
        }
    }//for(int i=0;i<(int)(ppackhead->num);i++)
#endif

    delete []lpBuffer;
    NucUsb.CloseWinUsbDevice(id);
    return TRUE;

}

BOOL FastDlg::XUSB_FasteMMCVerify(int id, CString& portName, CString& m_VfypathName)
{
    BOOL bResult;
    CString posstr;
    CString tempstr;
    int count=0;
    FILE* fp;
    int pos=0;
    unsigned int total=0,file_len,scnt,rcnt,ack;
    char* lpBuffer;
    char temp[BUF_SIZE];
    CNuWriterDlg* mainWnd=(CNuWriterDlg*)(AfxGetApp()->m_pMainWnd);
    PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)0,0);

    //TRACE(_T("XUSB_FasteMMCVerify start (%d)  ,%s\n"), id, m_VfypathName);
    /***********************************************/
    bResult=NucUsb.EnableOneWinUsbDevice(id);
    if(bResult==FALSE) {
        //AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#if(0)
    bResult=NucUsb.NUC_CheckFw(id);
    if(bResult==FALSE) {
        AfxMessageBox(_T("Please reset device and Re-connect now !!!\n"));
        return FALSE;
    }
#endif
    USHORT typeack;
    bResult=NucUsb.NUC_SetType(id,MMC,(UCHAR *)&typeack,sizeof(typeack));
    if(bResult==FALSE) {
        NucUsb.CloseWinUsbDevice(id);
        TRACE(_T("XXX (%d) eMMC Verify NUC_SetType error\n"),id);
        return FALSE;
    }

    fp=_wfopen(m_VfypathName,_T("rb"));
    if(!fp) {
        NucUsb.CloseWinUsbDevice(id);
        //AfxMessageBox(_T("File Open error\n"));
        fclose(fp);
        return FALSE;
    }

    fseek(fp,0,SEEK_END);
    file_len=ftell(fp);
    fseek(fp,0,SEEK_SET);
    if(!file_len) {
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        //AfxMessageBox(_T("File length is zero\n"));
        return FALSE;
    }

    lpBuffer = new char[file_len]; //read file to buffer
    memset(lpBuffer,0x00,file_len);
    fread(lpBuffer,file_len,1,fp);
    PACK_HEAD *ppackhead =(PACK_HEAD *)lpBuffer;

    int imagenum = 0;
    ppackhead->fileLength = (lpBuffer[7] << 24 | lpBuffer[6] << 16 | lpBuffer[5] << 8 | lpBuffer[4]);
    ppackhead->num = (lpBuffer[11] << 24 | lpBuffer[10] << 16 | lpBuffer[9] << 8 | lpBuffer[8]);
    imagenum = ppackhead->num;

    char *pbuf = lpBuffer;
    total+= sizeof(PACK_HEAD);
    pbuf+= sizeof(PACK_HEAD);

    PACK_CHILD_HEAD child;
    ((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetRange(0,100);
    ((CProgressCtrl *)GetDlgItem(iId_Array[id]))->SetBkColor(COLOR_VERIFY);
    int posnum=0;
    int prepos=0;
    int uBootHeadLen = 16;

    memset(m_fhead_emmc, 0x00, sizeof(NORBOOT_MMC_HEAD));
    m_fhead_emmc->flag=PACK_VERIFY_ACTION;
    //TRACE("lpBuffer[16~19] = 0x%x, 0x%x, 0x%x, 0x%x\n", (lpBuffer[16] & 0xff), lpBuffer[17], lpBuffer[18], lpBuffer[19]);
    m_fhead_emmc->filelen= ((lpBuffer[19] & 0xff) << 24 | (lpBuffer[18] & 0xff) << 16 | (lpBuffer[17] & 0xff) << 8 | (lpBuffer[16] & 0xff));//216452;//(lpBuffer[19] << 24 | lpBuffer[18] << 16 | lpBuffer[17] << 8 | lpBuffer[16]); // child1 file len
    m_fhead_emmc->flashoffset = ((lpBuffer[23] & 0xff) << 24 | (lpBuffer[22] & 0xff) << 16 | (lpBuffer[21] & 0xff) << 8 | (lpBuffer[20] & 0xff)); // child1 start address
    //m_fhead_emmc->execaddr = 0;// Struct without execaddr
    m_fhead_emmc->type = (lpBuffer[27] << 24 | lpBuffer[26] << 16 | lpBuffer[25] << 8 | lpBuffer[24]); // child1 image type
    m_fhead_emmc->no = imagenum;
    memcpy(temp,(unsigned char*)m_fhead_emmc,sizeof(NORBOOT_MMC_HEAD));

    bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)temp, sizeof(NORBOOT_MMC_HEAD));
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        TRACE(_T("XXX (%d) eMMC Verify head error. 0x%x\n"),id, GetLastError());
        return FALSE;
    }
    Sleep(5);
    bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
    if(bResult==FALSE) {
        delete []lpBuffer;
        NucUsb.CloseWinUsbDevice(id);
        fclose(fp);
        TRACE(_T("XXX (%d) eMMC Verify Read ACK head error. 0x%x\n"),id, GetLastError());
        return FALSE;
    }

    for(int i=0; i<(int)imagenum; i++) {
        memcpy(&child,(char *)pbuf,sizeof(PACK_CHILD_HEAD));
        memcpy(temp,(char *)pbuf,sizeof(PACK_CHILD_HEAD));

        bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)temp, sizeof(PACK_CHILD_HEAD));
        if(bResult==FALSE) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            fclose(fp);
            TRACE(_T("XXX (%d) eMMC Verify Write error. 0x%x\n"),id, GetLastError());
            return FALSE;
        }

        //Sleep(5);
        bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
        if(bResult==FALSE) {
            delete []lpBuffer;
            NucUsb.CloseWinUsbDevice(id);
            fclose(fp);
            TRACE(_T("XXX (%d) eMMC Verify Read error. 0x%x\n"),id, GetLastError());
            return FALSE;
        }
        total+= sizeof(PACK_CHILD_HEAD);
        pbuf+= sizeof(PACK_CHILD_HEAD);

        int ddrlen;
        //UCHAR * ddrbuf;

        if(child.imagetype == UBOOT) {
            ddrlen = (((mainWnd->DDRLen+8+15)/16)*16);
            //ddrbuf=DDR2Buf(mainWnd->DDRBuf,mainWnd->DDRLen,&ddrlen);
            pbuf += 16 + mainWnd->DDRLen + uBootHeadLen;
            total+= 16 + mainWnd->DDRLen + uBootHeadLen;

            // send DDR parameter Length
            bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)&ddrlen,4);
            Sleep(5);
            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                fclose(fp);
                TRACE(_T("XXX (%d) eMMC Verify Write error. 0x%x\n"),id, GetLastError());
                return FALSE;
            }

            bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)&ack,4);
            if(bResult==FALSE) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                fclose(fp);
                TRACE(_T("XXX (%d) eMMC Verify Read error. 0x%x\n"),id, GetLastError());
                return FALSE;
            }
        }


        if(child.imagetype == UBOOT) {
            scnt=(child.filelen-ddrlen-16)/BUF_SIZE;
            rcnt=(child.filelen-ddrlen-16)%BUF_SIZE;
        } else {
            scnt=child.filelen/BUF_SIZE;
            rcnt=child.filelen%BUF_SIZE;
        }

        int prepos=0;
        while(scnt>0) {

            if(child.imagetype !=ENV) {
                fseek(fp,total,SEEK_SET);
                fread(temp,BUF_SIZE,1,fp);
            } else {
                fseek(fp,total,SEEK_SET);
                fread(temp,BUF_SIZE,1,fp);
            }
            bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)lpBuffer, BUF_SIZE);
            if(WaitForSingleObject(m_ExitEventBurn[id], EMMC_RW_PIPE_TIMEOUT) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                fclose(fp);
                TRACE(_T("XXX (%d) WaitForSingleObject error 7,  0x%x\n"),id, GetLastError());
                return FALSE;
            }

            if(bResult==TRUE) {
                total+=BUF_SIZE;
                pbuf+=BUF_SIZE;

                pos=(int)(((float)(((float)total/(float)file_len))*100));
                posstr.Format(_T("%d%%"),pos);

                if(DataCompare(temp,lpBuffer,BUF_SIZE))
                    ack=BUF_SIZE;
                else {
                    //TRACE("XXX (%d) i = %d, total = %d, scnt = %d\n", id, i, total, scnt);
                    ack=0;//compare error
                }

                bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)&ack,4);
                if((bResult==FALSE)||(!ack)) {
                    delete []lpBuffer;
                    NucUsb.CloseWinUsbDevice(id);
                    fclose(fp);
                    TRACE(_T("XXX (%d) eMMC Verify error. \n"),id, GetLastError());
                    return ERR_VERIFY_DATACOM;
                }

            } else {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                fclose(fp);
                TRACE(_T("XXX (%d) eMMC Verify error. 0x%x\n"),id, GetLastError());
                return FALSE;
            }

            if(WaitForSingleObject(m_ExitEventBurn[id], EMMC_RW_PIPE_TIMEOUT) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                fclose(fp);
                TRACE(_T("XXX (%d) WaitForSingleObject error 8,  0x%x\n"),id, GetLastError());
                return FALSE;
            }

            scnt--;

            if(pos%5==0) {
                prepos=pos;
                PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)pos,0);
            }
        }

        if(rcnt>0) {
            memset((char *)&temp, 0xff, BUF_SIZE);
            if(m_fhead_emmc->type != ENV) {
                fseek(fp,total,SEEK_SET);
                fread(temp,rcnt,1,fp);
            } else {
                fseek(fp,total,SEEK_SET);
                fread(temp,BUF_SIZE,1,fp);
            }
            bResult=NucUsb.NUC_ReadPipe(id,(UCHAR *)lpBuffer,BUF_SIZE);
            if(WaitForSingleObject(m_ExitEventBurn[id], EMMC_RW_PIPE_TIMEOUT) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                fclose(fp);
                TRACE(_T("XXX (%d) WaitForSingleObject error 9, 0x%x\n"),id, GetLastError());
                return FALSE;
            }

            if(bResult==TRUE) {
                total+=rcnt;
                pbuf+=rcnt;
                if(DataCompare(temp,lpBuffer,rcnt))
                    ack=BUF_SIZE;
                else {
                    //TRACE("XXX i = %d, total = %d, rcnt = %d\n", i, total, rcnt);
                    ack=0;//compare error
                }

                bResult=NucUsb.NUC_WritePipe(id,(UCHAR *)&ack, 4);
                if((bResult==FALSE)||(!ack)) {
                    delete []lpBuffer;
                    NucUsb.CloseWinUsbDevice(id);
                    fclose(fp);
                    TRACE(_T("XXX (%d) eMMC Verify error. 0x%x\n"),id, GetLastError());
                    return ERR_VERIFY_DATACOM;
                }
            } else {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                fclose(fp);
                TRACE(_T("XXX (%d) eMMC Verify error. 0x%x\n"),id, GetLastError());
                return FALSE;
            }

            if(WaitForSingleObject(m_ExitEventBurn[id], EMMC_RW_PIPE_TIMEOUT) != WAIT_TIMEOUT) {
                delete []lpBuffer;
                NucUsb.CloseWinUsbDevice(id);
                fclose(fp);
                TRACE(_T("XXX (%d) WaitForSingleObject error A,  0x%x\n"),id, GetLastError());
                return FALSE;
            }

            pos=(int)(((float)(((float)total/(float)file_len))*100));

            if(pos>=100) {
                pos=100;
            }
            posstr.Format(_T("%d%%"),pos);

            if((pos!=prepos) || (pos==100)) {
                prepos=pos;
                PostMessage((WM_FAST_PROGRESS1+id),(LPARAM)pos,0);
            }
        }
    }

    delete []lpBuffer;
    fclose(fp);
    NucUsb.CloseWinUsbDevice(id);
    return TRUE;
}
/************** Fast End ************/
