//
//	Sliders.cpp - the slider subclassing module
//
///////////////////////////////////////////////////////////////////
//
//	Copyright (C) 1998 Rick York - Feel free to use this as
//	you wish provided that this notice remains intact and
//	a small credit is given (like in an about box :).
//
//	This class is used to link a slider with an edit box
//	for coordinated updates.
//
///////////////////////////////////////////////////////////////////
//
//@tabs=4

#include "Stdafx.h"
#include "Sliders.h"
#include "SmartEdit.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


AfxApi CLinkSlider * GetLinkSlider( CWnd *pcwnd, int id )
{
	return (CLinkSlider *)pcwnd->GetDlgItem( id );
}


///////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CLinkSlider, CSliderCtrl)
	//{{AFX_MSG_MAP(CLinkSlider)
	ON_WM_HSCROLL_REFLECT()
	ON_WM_VSCROLL_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CLinkSlider::CLinkSlider() : CSliderCtrl()
{
	m_pEdit = NULL;
	m_iPosition = INT_MAX;
	m_bVertical = FALSE;
}


void CLinkSlider::SetEditLink( CSmartEdit *pedit )
{
	m_pEdit = pedit;
	DWORD style = GetStyle();
	if( style & TBS_VERT )
		m_bVertical = TRUE;
}


void CLinkSlider::SetSlidePos( const int pos )
{
	if( m_bVertical )
		m_iPosition = m_iMax - ( pos - m_iMin );
	else
		m_iPosition = pos;

	SetPos( m_iPosition );
}


void CLinkSlider::SetSlideRange( const int imin, const int imax )
{
	m_iMin = imin;
	m_iMax = imax;
	SetRange( m_iMin, m_iMax );
}


void CLinkSlider::HScroll( UINT ncode, UINT pos )
{
	CSliderCtrl::OnHScroll( ncode, pos, NULL );
	if( ! m_pEdit )
		return;

	switch( ncode )
		{
		case TB_PAGEUP :
		case TB_PAGEDOWN :
			m_iPosition = GetPos();
			m_pEdit->UpdateEdit( m_iPosition );
			break;
	
		case TB_THUMBTRACK :
			m_iPosition = pos;
			m_pEdit->UpdateEdit( pos );
			break;
		}
}


void CLinkSlider::VScroll( UINT ncode, UINT pos )
{
	CSliderCtrl::OnVScroll( ncode, pos, NULL );
	if( ! m_pEdit )
		return;

	switch( ncode )
		{
		// we have to play some games with vertical sliders -
		// max position is on the bottom and min is on the top

		case TB_PAGEUP :
		case TB_PAGEDOWN :
			m_iPosition = m_iMax - ( GetPos() - m_iMin );
			m_pEdit->UpdateEdit( m_iPosition );
			break;

		case TB_THUMBTRACK :
			m_iPosition = m_iMax - ( pos - m_iMin );
			m_pEdit->UpdateEdit( m_iPosition );
		}
}
