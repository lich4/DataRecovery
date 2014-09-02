// DataRecovery.h : main header file for the DATARECOVERY application
//

#if !defined(AFX_DATARECOVERY_H__FBA01B76_41A7_4771_93C7_5F2022147D29__INCLUDED_)
#define AFX_DATARECOVERY_H__FBA01B76_41A7_4771_93C7_5F2022147D29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDataRecoveryApp:
// See DataRecovery.cpp for the implementation of this class
//

class CDataRecoveryApp : public CWinApp
{
public:
	CDataRecoveryApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataRecoveryApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDataRecoveryApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATARECOVERY_H__FBA01B76_41A7_4771_93C7_5F2022147D29__INCLUDED_)
