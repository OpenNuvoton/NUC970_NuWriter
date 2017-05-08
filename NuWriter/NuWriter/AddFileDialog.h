#if !defined(AFX_ADDFILEDIALOG_H__3BCEF6C1_C5FC_4E04_9729_6DDAB25DDDD9__INCLUDED_)
#define AFX_ADDFILEDIALOG_H__3BCEF6C1_C5FC_4E04_9729_6DDAB25DDDD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddFileDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddFileDialog dialog




class CAddFileDialog : public CFileDialog
{
//	DECLARE_DYNAMIC(CAddFileDialog)

public:
	CAddFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);  // standard constructor

//	~CSelectFolderDialog();

	virtual void OnInitDone();

	

protected:
	//{{AFX_MSG(CSelectFolderDialog)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDFILEDIALOG_H__3BCEF6C1_C5FC_4E04_9729_6DDAB25DDDD9__INCLUDED_)
