#include "stdafx.h"
#include "NuWriterDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSubFormCollection::CSubFormCollection()
{
    // initialize some members
    m_nActiveForm = -1;
    m_Rect.right  = 0;
    m_Rect.top    = 0;
    m_Rect.bottom = 0;
    m_Rect.left   = 0;
    m_bCentered = false;
}

CSubFormCollection::~CSubFormCollection()
{
    // Free all sub forms on destruction
    for (int i = 0; i <=  m_Forms.GetUpperBound(); i++) {
        CDialog* pDlg = m_Forms.GetAt(i);

        pDlg->DestroyWindow();
        delete pDlg;
    }
}

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

bool CSubFormCollection::SetActive(int ix)
{

    if (ix > m_Forms.GetUpperBound()) {
        // does not exist
        return(false);
    }
    m_nActiveForm = ix; // make it the active one
    return(true);
}


bool CSubFormCollection::CreateSubForm(UINT id,CWnd* pParentWnd)
{
    CDialog* pDlg;

    switch(id) {
    case IDD_SDRAM:
        pDlg = new CRAMDlg();
        break;
    case IDD_SPI:
        pDlg = new CSPIDlg();
        break;
    case IDD_NAND:
        pDlg = new CNANDDlg();
        break;
    case IDD_MMC:
        pDlg = new CMMCDlg();
        break;
    case IDD_MTP:
        pDlg = new CMTPDlg();
        break;
    case IDD_PACK:
        pDlg = new CPACKDlg();
        break;
    case IDD_FAST:
        pDlg = new FastDlg();
        break;
    default:
        pDlg = new CDialog();
        break;
    }

    ASSERT(pDlg != NULL);

    if (pDlg == NULL) return(false); // exit and tell the world about it
    AddForm(pDlg); // Add it to the internal table
    pDlg->Create(id,pParentWnd);
    CRect r;
    int x,y;
    pDlg->GetWindowRect(&r); // Get the size and position of the dialog in screen coordinates
    if (m_bCentered) {
        x = m_Rect.left - r.left + ((m_Rect.Width() - r.Width()) / 2);
        y = m_Rect.top - r.top + ((m_Rect.Height() - r.Height()) / 2);
    } else { // just use the top left corner
        x = m_Rect.left - r.left;
        y = m_Rect.top - r.top;
    }
    pDlg->SetWindowPos(pParentWnd,x,y, r.Width(), r.Height(), SWP_NOZORDER);
    return(true); // everything ok
}

bool CSubFormCollection::ShowSubForm(int ix)
{

    ASSERT(ix <= m_Forms.GetUpperBound());
    if (ix > m_Forms.GetUpperBound()) return(false); // does not exist

    CDialog* pDlg;
    if (ix != -1) {
        if (m_nActiveForm != -1) {
            pDlg = m_Forms.GetAt(m_nActiveForm);
            ASSERT(pDlg != NULL);
            ASSERT(pDlg->m_hWnd != NULL);
            if ((pDlg == NULL)
                    ||  (pDlg->m_hWnd == NULL)) return(false);
            pDlg->ShowWindow(SW_HIDE);
        }
        m_nActiveForm = ix; // this is the active one now;
    } else {
        if (m_nActiveForm == -1) {
            m_nActiveForm = 0;
        }
    }



    pDlg = m_Forms.GetAt(m_nActiveForm);
    ASSERT(pDlg != NULL);
    ASSERT(pDlg->m_hWnd != NULL);
    if ((pDlg == NULL)
            ||  (pDlg->m_hWnd == NULL)) return(false);
    pDlg->ShowWindow(SW_SHOW);
    pDlg->Invalidate();
    pDlg->RedrawWindow();
    return(true);
}

CDialog* CSubFormCollection::GetSubForm(int ix)
{

    ASSERT(ix <= m_Forms.GetUpperBound());
    if (ix > m_Forms.GetUpperBound()) return(false); // does not exist

    CDialog* pDlg;
    if (ix != -1) {
        if (m_nActiveForm != -1) {
            pDlg = m_Forms.GetAt(m_nActiveForm);
            ASSERT(pDlg != NULL);
            ASSERT(pDlg->m_hWnd != NULL);
            if ((pDlg == NULL)
                    ||  (pDlg->m_hWnd == NULL)) return(false);
            return pDlg;
        }
    }
    return false;
}