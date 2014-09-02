// FindString.h : main header file for the FINDSTRING application
//

#if !defined(AFX_FINDSTRING_H__B437D220_CC46_485D_BF90_26AB8A239FD1__INCLUDED_)
#define AFX_FINDSTRING_H__B437D220_CC46_485D_BF90_26AB8A239FD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFindStringApp:
// See FindString.cpp for the implementation of this class
//

class CFindStringApp : public CWinApp
{
public:
	CFindStringApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindStringApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFindStringApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDSTRING_H__B437D220_CC46_485D_BF90_26AB8A239FD1__INCLUDED_)
