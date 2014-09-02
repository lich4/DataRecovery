// FormatRecoveryDlg.h : header file
//

#if !defined(AFX_FORMATRECOVERYDLG_H__BB53F458_F228_4824_A565_1C4956458D1C__INCLUDED_)
#define AFX_FORMATRECOVERYDLG_H__BB53F458_F228_4824_A565_1C4956458D1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "FormatDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CFormatRecoveryDlg dialog

class CFormatRecoveryDlg : public CDialogEx
{
// Construction
public:
	CFormatRecoveryDlg(CWnd* pParent = NULL);	// standard constructor
	CFormatRecovery* recover;
	int col;//用于列表控件排序
	BOOL direct[20];//用于列表控件排序
	HANDLE m_diskhandle;//保存已打开的设备句柄，此句柄共用于CDiskMappingPointer，因此谨慎使用
// Dialog Data
	//{{AFX_DATA(CFormatRecoveryDlg)
	enum { IDD = IDD_FORMATRECOVERY_DIALOG };
	CListBox	m_searcharea;
	CListCtrl	m_searchext;
	CListCtrl	m_allext;
	CProgressCtrl	m_progress;
	CListCtrl	m_result;
	CWinThread* pthread;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormatRecoveryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFormatRecoveryDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAddext();
	afx_msg void OnAddall();
	afx_msg void OnDelext();
	afx_msg void OnRecoverfile();
	afx_msg void OnSearch();
	afx_msg void OnClose();
	afx_msg void OnRecoverpos();
	afx_msg void OnMenuRecoverFile();
	afx_msg void OnMenuGetInfo();
	afx_msg void OnGetFirstCluster();
	afx_msg void OnColumnclickResult(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClickResult(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMATRECOVERYDLG_H__BB53F458_F228_4824_A565_1C4956458D1C__INCLUDED_)
