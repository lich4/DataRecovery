// ReadRegDlg.h : header file
//

#if !defined(AFX_READREGDLG_H__9D7AE72D_67CB_46D7_8B72_A3DECA971E2A__INCLUDED_)
#define AFX_READREGDLG_H__9D7AE72D_67CB_46D7_8B72_A3DECA971E2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RegStruct.h"

/////////////////////////////////////////////////////////////////////////////
// CReadRegDlg dialog

class CReadRegDlg : public CDialog
{
// Construction
public:
	CReadRegDlg(CWnd* pParent = NULL);	// standard constructor
	char*	Buf;
	char*	relative;//µÚÒ»¸öhbin
	DWORD	m_size;
// Dialog Data
	//{{AFX_DATA(CReadRegDlg)
	enum { IDD = IDD_READREG_DIALOG };
	CTreeCtrl	m_RegTree;
	CListCtrl	m_RegList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReadRegDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	void AnalysisRegFile();
	void AddToTree(HTREEITEM curtree,DWORD curkey);
	// Generated message map functions
	//{{AFX_MSG(CReadRegDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnOpen();
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_READREGDLG_H__9D7AE72D_67CB_46D7_8B72_A3DECA971E2A__INCLUDED_)
