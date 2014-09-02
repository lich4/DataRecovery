// FormatRecovery.h : main header file for the FORMATRECOVERY application
//

#if !defined(AFX_FORMATRECOVERY_H__952346B8_686E_4697_8252_CAC6C0B35C33__INCLUDED_)
#define AFX_FORMATRECOVERY_H__952346B8_686E_4697_8252_CAC6C0B35C33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFormatRecoveryApp:
// See FormatRecovery.cpp for the implementation of this class
//

class CFormatRecoveryApp : public CWinApp
{
public:
	CFormatRecoveryApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormatRecoveryApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFormatRecoveryApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMATRECOVERY_H__952346B8_686E_4697_8252_CAC6C0B35C33__INCLUDED_)
