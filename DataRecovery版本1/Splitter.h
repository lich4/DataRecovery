#if !defined(AFX_SPLITTER_H__8745D902_B3A6_452C_BF07_351EBA0DEF7D__INCLUDED_)
#define AFX_SPLITTER_H__8745D902_B3A6_452C_BF07_351EBA0DEF7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Splitter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSplitter window

class CSplitter : public CButton
{
// Construction
public:
	CSplitter();

// Attributes
public:
	CWnd* m_Father;
// Operations
public:
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitter)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSplitter();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSplitter)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
private:
	BOOL m_IsLeftDown;
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLITTER_H__8745D902_B3A6_452C_BF07_351EBA0DEF7D__INCLUDED_)

