#if !defined(AFX_SUBFORMCOLLECTION_H__EEF10080_9028_11D3_8594_00105A744766__INCLUDED_)
#define AFX_SUBFORMCOLLECTION_H__EEF10080_9028_11D3_8594_00105A744766__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

class CSubFormCollection  
{
public:
	CSubFormCollection();
	virtual ~CSubFormCollection();

    bool SetActive(int ix);   // set and display an other sub form
    
    bool CreateSubForm(UINT id,CWnd* pParentWnd);
        // create a subform with a given resource template
    
    bool ShowSubForm(int ix = -1);
        // display the requested subform (by default the active form is displayed)
    
    void SetRelPos(const CRect& rect) {m_Rect = rect;m_bCentered = false;};
        // set the relative position of the subforms
    
    void SetCenterPos(const CRect& rect) {m_Rect = rect;m_bCentered = true;};
        // set the position of the subforms and switch the center mode on


	CDialog* GetSubForm(int ix);

protected:
    CArray<CDialog*, CDialog*> m_Forms; // array with all forms
    int m_nActiveForm;  // index to the currently active sub form
    CRect m_Rect;       // position of the sub form
    bool m_bCentered;   // true if the form is centred within the rect, else false

    int AddForm(CDialog* dlg) { return(m_Forms.Add(dlg));}; // internal method for adding a newly created form
};

#endif // !defined(AFX_SUBFORMCOLLECTION_H__EEF10080_9028_11D3_8594_00105A744766__INCLUDED_)
