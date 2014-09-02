#if !defined(AFX_SHOWINFO_H__0FE9D82F_2223_4E2A_A12E_E84E0F94D141__INCLUDED_)
#define AFX_SHOWINFO_H__0FE9D82F_2223_4E2A_A12E_E84E0F94D141__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShowInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShowInfo dialog

class CShowInfo : public CDialogEx
{
// Construction
public:
	CShowInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShowInfo)
	enum { IDD = IDD_SHOWINFO };
	CString	m_showinfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShowInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShowInfo)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOWINFO_H__0FE9D82F_2223_4E2A_A12E_E84E0F94D141__INCLUDED_)
