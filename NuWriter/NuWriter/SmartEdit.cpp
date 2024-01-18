//
// SmartEdit.cpp - the smart edit control module
//
///////////////////////////////////////////////////////////////////
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1997 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.
//
///////////////////////////////////////////////////////////////////
//
//	Copyright (C) 1998 Rick York - Feel free to use this as
//	you wish provided that these notices remain intact and
//	a small credit is given (like in an about box :).
//
//	This began life as part of the CTRLTEST sample from the
//	MSDN disks and was called CParsedEdit.  I have changed
//	its name to reflect its enhanced functionality which
//	includes support for more input types : floating point,
//	underscore, and negative sign; and linking with a slider
//	for coordinated updates.
//
//	NOTE :	I have attempted to make this Unicode compatable
//			but I have not tested on a MBCS so I can't make
//			any guarantees.  Please notify me of any problems
//			(or any successes) at ry@proaxis.com
//
///////////////////////////////////////////////////////////////////
//
//@tabs=4

#include "Stdafx.h"
#include "SmartEdit.h"
#include "Sliders.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


AfxApi CSmartEdit * GetSmartEdit( CWnd *pcwnd, int id )
{
    return (CSmartEdit *)pcwnd->GetDlgItem( id );
}


///////////////////////////////////////////////////////////
// CSmartEdit implementation

BEGIN_MESSAGE_MAP(CSmartEdit, CEdit)
//{{AFX_MSG_MAP(CSmartEdit)
ON_WM_CHAR()
ON_CONTROL_REFLECT(EN_UPDATE, OnUpdate)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CSmartEdit::CSmartEdit()
{
    m_wParseType = SES_ALL;
    m_pSlider = NULL;
    m_iValue = INT_MIN;
    //m_pFormat = NULL;
    m_bUseFp = FALSE;
}


CSmartEdit::~CSmartEdit()
{
    //if( m_pFormat )
    //    free( m_pFormat );
}


int CSmartEdit::SetParseType( WORD type )
{
    m_wParseType = type;
    return type;
}


// Aliasing on top of an existing Edit control

BOOL CSmartEdit::SubclassEdit( UINT nID, CWnd* pParent, WORD wParseStyle )
{
    SetParseType( wParseStyle );
    return SubclassDlgItem( nID, pParent );
}


// Input character filter

void CSmartEdit::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags )
{
    WORD type = SES_OTHERCHARS;		// default
    int	alpha = IsCharAlpha( (TCHAR)nChar );
    int	number = IsCharAlphaNumeric( (TCHAR)nChar ) && ! alpha;
    
    if( nChar < 0x20 )
        type = SES_ALL; 			// always allow control chars ???
    else if( number )
        type = SES_NUMBERS | SES_SIGNED | SES_FLOATINGPT | SES_HEXADECIMAL;
    else if( alpha )
    {
        type = SES_LETTERS;
        int uprval = toupper( (TCHAR)nChar );
        if( uprval == 'E' )
            type |= ( SES_FLOATINGPT | SES_HEXADECIMAL );
        else if( uprval == 'X' )
            type |= SES_HEXADECIMAL;
        else if( ( uprval >= 'A' ) && ( uprval <= 'F' ) )
            type |= SES_HEXADECIMAL;
    }
    else if( (TCHAR)nChar == '-' )		// dash
        type = SES_SIGNED | SES_FLOATINGPT;
    else if( (TCHAR)nChar == '.' )		// dot
        type = SES_FLOATINGPT;
    else if( (TCHAR)nChar == '_' )		// underscore
        type = SES_UNDERSCORE | SES_OTHERCHARS;
    
    if( m_wParseType  & type )
    {
        CEdit::OnChar(nChar, nRepCnt, nFlags);	// permitted
    }
    else
    {
        // illegal character - inform parent
        OnBadInput();
    }
}



// This is the default response to bad input.
// This is virtual so it may be overridden.

void CSmartEdit::OnBadInput()
{
    MessageBeep((UINT)-1);
}


/////////////////////////////////////////////////////////////
//	The following code is used when an edit box is linked
//	with a slider for coordinated updates.  If you do not
//	want this functionality remove this code from here and
//	in the header.

void CSmartEdit::OnUpdate() 
{
    if( ! m_pSlider )
        return;
    
    //TCHAR buf[512];
	CString buf;
    //GetWindowText( buf, sizeof( buf ) / sizeof(TCHAR) );
	GetWindowText(buf);
    
    int	error = TRUE;
    if( m_bUseFp )
    {
        m_dValue = _ttof(buf); //atof( buf );
        if( m_dValue < m_dMin )
        {
            m_dValue = m_dMin;
            OnBadInput();
        }
        else if( m_dValue > m_dMax )
        {
            m_dValue = m_dMax;
            OnBadInput();
        }
        else
            error = FALSE;
        
        // scale to integer
        
        double dpos = ( m_dValue - m_dMin ) * (double)m_iRange / m_dRange;
        m_iValue = (int)dpos;
    }
    else
    {
        m_iValue = _wtoi( buf );//atoi( buf );
        if( m_iValue < m_iMin )
        {
            m_iValue = m_iMin;
            OnBadInput();
        }
        else if( m_iValue > m_iMax )
        {
            m_iValue = m_iMax;
            OnBadInput();
        }
        else
            error = FALSE;
    }
    
    if( m_pSlider )
        m_pSlider->SetSlidePos( m_iValue );
    
    if( error )
    {
        int posn = m_iValue;
        m_iValue = INT_MIN;
        UpdateEdit( posn );
    }
}


int	CSmartEdit::UpdateEdit( int position )
{
    //TCHAR buffer[512];
	CString buffer;
    
    if( position == m_iValue )
        return FALSE;		// been here already
    
    m_iValue = position;
    if( m_bUseFp )
    {
        // scale to double value
        m_dValue = m_dMin + (double)m_iValue * m_dRange / 1000.0;
        //sprintf( buffer, m_pFormat, m_dValue );
		buffer.Format(_T("%f"), m_dValue);
    }
    else
        //sprintf( buffer, "%d", m_iValue );
		buffer.Format(_T("%d"), m_iValue);
    
    SetWindowText( buffer );
    return TRUE;
}


void CSmartEdit::SetSlideLink( CWnd *pParent, int sliderid )
{
    m_pSlider = (CLinkSlider *)pParent->GetDlgItem( sliderid );
    ASSERT( m_pSlider );
    m_pSlider->SetEditLink( this );
}


int	CSmartEdit::InitSlider()
{
    if( ! m_pSlider )
        return FALSE;
    m_pSlider->SetEditLink( this );
    m_iRange = m_iMax - m_iMin;
    m_pSlider->SetSlideRange( m_iMin, m_iMax );
    int freq = m_iRange / m_iBands;
    m_pSlider->SetTicFreq( freq );
    return TRUE;
}


//void CSmartEdit::SetFormatString( ccp fmtstring )
void CSmartEdit::SetFormatString(CString fmtstring )
{
    //if( m_pFormat )
    //    free( m_pFormat );
    
    // this method is used so we can track allocations
    
    //int len = strlen( fmtstring );
	int len = wcslen( fmtstring );
    m_pFormat = (TCHAR *)calloc( sizeof( TCHAR ), 1 + len );
    //strcpy( m_pFormat, fmtstring );
	m_pFormat.Format(_T("%s"),fmtstring);
}


int CSmartEdit::SetParams( cint imin, cint imax, cint ticbands )
{
    m_bUseFp = FALSE;
    m_iBands = ticbands;
    m_iMin = imin;
    m_iMax = imax;
    InitSlider();
    SetFormatString(_T("%d"));
    WORD type = SES_NUMBERS;
    if( imin < 0 )
        type |= SES_SIGNED;
    SetParseType( type );
    return TRUE;
}


//int CSmartEdit::SetParams( cdbl dmin, cdbl dmax,
//                          cint ticbands, ccp fmtstring )
int CSmartEdit::SetParams( cdbl dmin, cdbl dmax,
                          cint ticbands, CString fmtstring )
{
    m_bUseFp = TRUE;
    m_iBands = ticbands;
    m_dRange = dmax - dmin;
    m_dMin = dmin;
    m_dMax = dmax;
    m_iMin = 0;
    m_iMax = 1000;
    
    InitSlider();
    SetFormatString( fmtstring );
    SetParseType( SES_FLOATINGPT );
    return TRUE;
}


int CSmartEdit::SetValue( cint ivalue )
{
    if( m_bUseFp )
        return FALSE;
    
    if( ( ivalue < m_iMin ) || ( ivalue > m_iMax ) )
        return FALSE;
    
    //TCHAR buf[512];
	CString buf;
    //sprintf( buf, m_pFormat, ivalue );
	buf.Format(_T("%d"), ivalue);
    SetWindowText( buf );
    return TRUE;
}


int CSmartEdit::SetValue( cdbl dvalue )
{
    if( ! m_bUseFp )
        return FALSE;
    
    if( ( dvalue < m_dMin ) || ( dvalue > m_dMax ) )
        return FALSE;
    
    //TCHAR buf[512];
	CString buf;
    //sprintf( buf, m_pFormat, dvalue );
	buf.Format(_T("%d"), dvalue);
    SetWindowText( buf );
    return TRUE;
}

