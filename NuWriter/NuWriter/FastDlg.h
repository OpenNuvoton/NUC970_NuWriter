#pragma once
#include "afxwin.h"


// FastDlg 對話方塊

class FastDlg : public CDialog {
    DECLARE_DYNAMIC(FastDlg)

public:
    FastDlg(CWnd* pParent = NULL);   // 標準建構函式
    virtual ~FastDlg();

// 對話方塊資料
    enum { IDD = IDD_FAST };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
    virtual BOOL OnInitDialog();
    LRESULT ShowStatusD1(WPARAM  pos, LPARAM message);
    LRESULT ShowStatusD2(WPARAM  pos, LPARAM message);
    LRESULT ShowStatusD3(WPARAM  pos, LPARAM message);
    LRESULT ShowStatusD4(WPARAM  pos, LPARAM message);
    LRESULT ShowStatusD5(WPARAM  pos, LPARAM message);
    LRESULT ShowStatusD6(WPARAM  pos, LPARAM message);
    LRESULT ShowStatusD7(WPARAM  pos, LPARAM message);
    LRESULT ShowStatusD8(WPARAM  pos, LPARAM message);
    LRESULT ThreadClose(WPARAM  id, LPARAM message);
    LRESULT TimeoutCtl( WPARAM  id, LPARAM message);

    DECLARE_MESSAGE_MAP()
public:
    CString m_imagename;
    CString m_filename;

    HANDLE m_ExitEventBurn[8];
    unsigned ThreadArray[8];
    HANDLE hThread[8];

    int m_type;
    int iId_Array[8];
    int m_DeviceId;
    int m_Downloadcnt;
    int m_Successcnt;
    int InitFlag;
	int flashType;
    NORBOOT_NAND_HEAD *m_fhead_nand;
    NORBOOT_NAND_HEAD *m_fhead_spi;
    NORBOOT_MMC_HEAD *m_fhead_emmc;

    BOOL InitFile(int flag);
    void Start(int id);
    static unsigned WINAPI Start_proc(void* args);
    afx_msg void OnBnClickedBtnFastStart();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnBnClickedFastBrowse();

    BOOL FastErase(int id, int storagetype);
    BOOL FastBurn(int id, int storagetype);
    int  FastVerify(int id, int storagetype);

    BOOL XUSB_FastNANDErase(int id, CString& portName);
    BOOL XUSB_FastNANDBurn(int id, CString& portName,CString& m_pathName,int *len);
    BOOL XUSB_FastNANDVerify(int id, CString& portName, CString& m_pathName);

    BOOL XUSB_FastSPIErase(int id, CString& portName);
    BOOL XUSB_FastSPIBurn(int id, CString& portName,CString& m_pathName,int *len);
    BOOL XUSB_FastSPIVerify(int id, CString& portName, CString& m_pathName);

    BOOL XUSB_FasteMMCErase(int id, CString& portName);
    BOOL XUSB_FasteMMCBurn(int id, CString& portName,CString& m_pathName,int *len);
    BOOL XUSB_FasteMMCVerify(int id, CString& portName, CString& m_pathName);

    BOOL CheckImageSouce(CString& m_pathName);
    BOOL FWDownload(int id);
    BOOL DDRtoDevice(int id, char *buf,unsigned int len);
    BOOL XUSB(int id, CString& m_pathName);

    CString block;
    CString sblock;
    int m_erase_type;
    CString	EraseInfo;
    CStatic m_erase_info;
    BOOL timerctl[8];
    BOOL timeoutflag[8];
    int retrycnt[8];
    int timercnt;
    int timeoutsec;


    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();
    afx_msg void OnCbnSelchangeComboFastId();
    CComboBox m_FastDeviceID;
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedRstbtn();
    afx_msg void OnBnClickedAllrstbtn();
    afx_msg void OnBnClickedRadioFastNand();
    afx_msg void OnBnClickedRadioFastSpi();
    afx_msg void OnBnClickedRadioFastemmc();
	CButton m_nandflashInfo_check;
	afx_msg void OnBnClickedFastNandUsrconfig();
};
