// ReadTable.h : main header file for the READTABLE application
//

#if !defined(AFX_READTABLE_H__443FB8E4_3068_40F4_8307_D96DEE208072__INCLUDED_)
#define AFX_READTABLE_H__443FB8E4_3068_40F4_8307_D96DEE208072__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CReadTableApp:
// See ReadTable.cpp for the implementation of this class
//

class CReadTableApp : public CWinApp
{
public:
	CReadTableApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReadTableApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CReadTableApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_READTABLE_H__443FB8E4_3068_40F4_8307_D96DEE208072__INCLUDED_)
