#if !defined(AFX_RECOVERSTATUS_H__01F130E9_F9C0_46A7_9712_36E2F149AF32__INCLUDED_)
#define AFX_RECOVERSTATUS_H__01F130E9_F9C0_46A7_9712_36E2F149AF32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RecoverStatus.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRecoverStatus dialog

class CRecoverStatus : public CDialog
{
// Construction
public:
	CRecoverStatus(CWnd* pParent = NULL);   // standard constructor
	BOOL m_StopThread;
// Dialog Data
	//{{AFX_DATA(CRecoverStatus)
	enum { IDD = IDD_RecoverStatu };
	CProgressCtrl	m_Status;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecoverStatus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRecoverStatus)
	virtual BOOL OnInitDialog();
	afx_msg void OnStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECOVERSTATUS_H__01F130E9_F9C0_46A7_9712_36E2F149AF32__INCLUDED_)
