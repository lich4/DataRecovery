// DeepScan.h : main header file for the DEEPSCAN application
//

#if !defined(AFX_DEEPSCAN_H__7C1EFB29_FD43_43C7_B702_2313B28614E5__INCLUDED_)
#define AFX_DEEPSCAN_H__7C1EFB29_FD43_43C7_B702_2313B28614E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDeepScanApp:
// See DeepScan.cpp for the implementation of this class
//

class CDeepScanApp : public CWinApp
{
public:
	CDeepScanApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeepScanApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDeepScanApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEEPSCAN_H__7C1EFB29_FD43_43C7_B702_2313B28614E5__INCLUDED_)
