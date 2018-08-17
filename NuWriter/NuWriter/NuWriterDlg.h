// NuWriterDlg.h : header file
//
#ifndef __NUWRITERDLG_H__
#define __NUWRITERDLG_H__

#pragma once
#include "NuWriter.h"

#include "IniFile.h"
#include "InitDlg.h"
#include "SubFormCollection.h"
#include "SPIDlg.h"
#include "SPIReadDlg.h"
#include "EraseDlg.h"
#include "RAMDlg.h"
#include "NANDDlg.h"
#include "MMCDlg.h"
#include "MTPDlg.h"
#include "PACKDlg.h"
#include "FastDlg.h"
#include "FormatDlg.h"
#include "SelChipDlg.h"
#include "PackTab1.h"
#include "PackTab2.h"
#include "aes.h"
#include "Serial.h"
#include <vector>
#include <fstream>

extern "C" {
#include "winusb.h"
}

#include "NucWinUsb.h"
#include "afxwin.h"

#define PROJECT_NAME _T("Nuvoton NuWriter v1.1")
//for AES ------------------
#define LINE_BUFF_LEN            4096


// CNuWriterDlg dialog
class CNuWriterDlg : public CDialog {
// Construction
public:
    CNuWriterDlg(CWnd* pParent = NULL); // standard constructor

// Dialog Data
    enum { IDD = IDD_NUWRITER_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support


// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

    CSubFormCollection m_SubForms; //#### define an instance of the form collection
public:
    afx_msg void OnBnClickedReconnect();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    void FastMode_ProgressControl(int num, BOOL isHIDEType);
    CIniFile m_inifile;
    CComboBox m_gtype;
    CStatic m_static_ddr;
    CStatic m_static_ddrfile;
    CButton	m_type;
    CString m_portName;
    CStatic	m_burntext;
    int g_iDeviceNum;
    int g_iCurDevNum;
    //------------------------------------------------------
    void LoadDirINI(CString Dir,wchar_t *, vector<CString> &sList);
    void GetExeDir(CString & path);
    CString szDirForDDR;
    //char * LoadDDRInit(CString pathName,int *len);
    void CNuWriterDlg::LoadDDRInit(CString pathName,int *len);
    char *DDRBuf;
    char ShareDDRBuf[1024];
    unsigned char DDR2Buf[512];
    int	DDRLen;
    unsigned int DDRAddress;
    CString DDRFileFullPath;
    CString DDRFileName;
    void UpdateBufForDDR(void);
    //------------------------------------------------------
    CString szDirForKey;
    int TargetChip;
    int DDR_Idx;  //UI default display next time
    //AES---------------------------------------------------
    int  _nKenLen;
    uint8  _ucKeys[32];
    char _FileLineBuff[LINE_BUFF_LEN];
    int	Read_File_Line(HANDLE hFile);
    int Get_Key_Data(HANDLE hFile);
    char * AES_Encrpt(CString keypath,CString filepath,unsigned int *len);
    char * Get_OTP_KEY(CString& m_pathName,int * len);
    //------------------------------------------------------
    int page_idx;
    //Timer--------------------------------------------------
    int TimeEn;
    int Timeus;
    //-------------------------------------------------------
    afx_msg void OnDestroy();
    void INItoSaveOrLoad(int Flag);
    CSelChipDlg m_SelChipdlg;
    afx_msg void OnCbnSelchangeComboType();
    void OneSelchangeComboType(int id);

    FooButton m_reconnect;
    FooButton m_exit;
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    CInitDlg m_initdlg;
    CStatic	m_version;
    int iDevice;


    //--------------------------------------------------
    HANDLE m_ExitEvent[8];//m_ExitEvent;
    INFO_T m_info;
    BOOL XUSB_Info();
    static unsigned WINAPI Info_proc(void* args);
    BOOL Info();
    void GetInfo();
    //--------------------------------------------------
    unsigned char * envbuf;
    unsigned int CalculateCRC32(unsigned char * buf,unsigned int len);
    int ChipEraseWithBad;
    int ChipReadWithBad;
    int ChipWriteWithOOB;
    unsigned char* CalculateSHA(CString filename);

    int FirstDelay;

    CString Nand_size;

    int Nand_uBlockPerFlash;
    int Nand_uPagePerBlock;

    int DtbEn;
    BOOL OneDeviceInfo(int id);
    afx_msg void OnBnClickedCancel();
};

#endif