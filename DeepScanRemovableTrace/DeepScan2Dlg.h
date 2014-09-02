// DeepScan2Dlg.h : header file
//

#if !defined(AFX_DEEPSCAN2DLG_H__D9E1F412_DC61_4CB1_9B11_F48D422A921A__INCLUDED_)
#define AFX_DEEPSCAN2DLG_H__D9E1F412_DC61_4CB1_9B11_F48D422A921A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDeepScan2Dlg dialog

#include "RegStruct.h"

class CDeepScan2Dlg : public CDialog
{
// Construction
public:
	CDeepScan2Dlg(CWnd* pParent = NULL);	// standard constructor
	HANDLE hDevice;
	HANDLE hDeepScan;
	int m_complete;
	CButton check;
// Dialog Data
	//{{AFX_DATA(CDeepScan2Dlg)
	enum { IDD = IDD_DEEPSCAN2_DIALOG };
	CListCtrl	m_list;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeepScan2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	static DWORD WINAPI DeepScan(LPVOID lpVoid);
	// Generated message map functions
	//{{AFX_MSG(CDeepScan2Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEEPSCAN2DLG_H__D9E1F412_DC61_4CB1_9B11_F48D422A921A__INCLUDED_)
