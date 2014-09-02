// DeepScan2.h : main header file for the DEEPSCAN2 application
//

#if !defined(AFX_DEEPSCAN2_H__B35A0A67_2AFB_45E0_944C_23E485AE06BB__INCLUDED_)
#define AFX_DEEPSCAN2_H__B35A0A67_2AFB_45E0_944C_23E485AE06BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDeepScan2App:
// See DeepScan2.cpp for the implementation of this class
//

class CDeepScan2App : public CWinApp
{
public:
	CDeepScan2App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeepScan2App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDeepScan2App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEEPSCAN2_H__B35A0A67_2AFB_45E0_944C_23E485AE06BB__INCLUDED_)
