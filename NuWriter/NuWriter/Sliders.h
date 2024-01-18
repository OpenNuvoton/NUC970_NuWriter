//
// Sliders.h - header for slider subclassing module
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

#ifdef _SLIDERS_H
#error repeated include of this file
#else // _SLIDERS_H
#define _SLIDERS_H
#endif // _SLIDERS_H


#ifndef __cplusplus
#error this is file may be used in C++ only
#endif


#ifndef AfxExt

#ifdef EXTENSION_DLL	// define this if controls are in a DLL
#define	AfxExt	AFX_EXT_CLASS
#define	AfxApi	AFX_EXT_API
#else	// EXTENSION_DLL
#define	AfxExt			// controls are NOT in a DLL
#define	AfxApi
#endif	// EXTENSION_DLL

#endif // AfxExt


class AfxExt CSmartEdit;


class AfxExt CLinkSlider : public CSliderCtrl
{
public:
	// Construction
	CLinkSlider();

	CSmartEdit *	m_pEdit;

	int		m_iMin;
	int		m_iMax;
	int		m_iPosition;
	BOOL	m_bVertical;

	void	SetSlidePos( const int pos );
	void	SetSlideRange( const int min, const int max );

	void	SetEditLink( CSmartEdit *pedit );

	// Implementation
protected:
	//{{AFX_MSG(CLinkSlider)
	afx_msg void HScroll( UINT ncode, UINT pos );
	afx_msg void VScroll( UINT ncode, UINT pos );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


AfxApi CLinkSlider * GetLinkSlider( CWnd *pcwnd, int id );
