//
// SmartEdit.h - CSmartEdit is a specialized CEdit control
//				that only allows characters of a given type.
//				Available types are defined below.
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
//	a small credit is given (like in an about box ;).
//
//	This began life as part of the CTRLTEST sample from the
//	MSDN disks and was called CParsedEdit.  I have changed
//	its name to reflect its enhanced functionality which
//	includes support for more input types : floating point,
//	underscore, and negative sign; and linking with a slider
//	for coordinated updates.
//
///////////////////////////////////////////////////////////////////
//
//@tabs=4


#ifdef _SMARTEDIT_H
#error repeated include of this file
#else // _SMARTEDIT_H
#define _SMARTEDIT_H
#endif // _SMARTEDIT_H


#ifndef __cplusplus
#error this is file may be used in C++ only
#endif


	// smart edit control sub-styles

#define SES_NUMBERS		0x0001	// no negative sign allowed
#define SES_LETTERS		0x0002	// also upper and lower case
#define SES_FLOATINGPT	0x0004	// allows '-', '.', and 'E'
#define SES_OTHERCHARS	0x0008	// allows punctuation
#define SES_SIGNED		0x0010	// allows negative sign also
#define SES_UNDERSCORE	0x0020	// allows the underscore
#define	SES_HEXADECIMAL 0x0040	// allows hex chars (0-9,A-F,X)
#define SES_ALL			0xFFFF	// anything goes

	// define some constant data types

typedef const char *	ccp;
typedef const int		cint;
typedef const double	cdbl;


#ifndef AfxExt

#ifdef EXTENSION_DLL	// define this if controls are in a DLL
#define	AfxExt	AFX_EXT_CLASS
#define	AfxApi	AFX_EXT_API
#else	// EXTENSION_DLL
#define	AfxExt			// controls are NOT in a DLL
#define	AfxApi
#endif	// EXTENSION_DLL

#endif // AfxExt


class AfxExt CLinkSlider;	// a specialized slider control


class AfxExt CSmartEdit : public CEdit
{
private:
	WORD	m_wParseType;

protected:
	// Overridables
	virtual void OnBadInput();

public:
	CSmartEdit();
	virtual ~CSmartEdit();

	int		SetParseType( WORD type );
	BOOL	SubclassEdit( UINT nID, CWnd* pParent,
						WORD wParseStyle );

/////////////////////////////////////////////////////////////
// code for linking with a slider

		// link the slider and edit control first

	void	SetSlideLink( CWnd *pParent, int sliderid );

	int		SetParams( cint imin, cint imax, cint ticbands );
	//int		SetParams( cdbl dmin, cdbl dmax,
	//				cint tbands, ccp fmtstring );
	int		SetParams( cdbl dmin, cdbl dmax,
					cint tbands, CString fmtstring );

	int		SetValue( cint ivalue );
	int		SetValue( cdbl dvalue );
	int		UpdateEdit( cint position );

	//void	SetFormatString( ccp fmtstring );
	void	SetFormatString( CString fmtstring );

private :
	int		InitSlider();

	// Implementation

private:
	CLinkSlider *	m_pSlider;

	//TCHAR *	m_pFormat;
	CString m_pFormat;
	int		m_iTextId;
	int		m_iSliderId;
	int		m_iBands;
	BOOL	m_bUseFp;
	int		m_iMin, m_iMax, m_iValue, m_iRange;
	double	m_dMin, m_dMax, m_dValue, m_dRange;

// end of code for linking with a slider
/////////////////////////////////////////////////////////////

protected:
	//{{AFX_MSG(CSmartEdit)
	afx_msg void OnChar(UINT, UINT, UINT);
	afx_msg void OnUpdate();	// used with a linked slider
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


AfxApi CSmartEdit * GetSmartEdit( CWnd *pcwnd, int id );
