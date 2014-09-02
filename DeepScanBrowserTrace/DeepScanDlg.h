// DeepScanDlg.h : header file
//

#if !defined(AFX_DEEPSCANDLG_H__781BB526_77E4_4558_9F04_D498270A9A8F__INCLUDED_)
#define AFX_DEEPSCANDLG_H__781BB526_77E4_4558_9F04_D498270A9A8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDeepScanDlg dialog

class CDeepScanDlg : public CDialog
{
// Construction
public:

	CDeepScanDlg(CWnd* pParent = NULL);	// standard constructor
	HANDLE m_hDevice;
	LARGE_INTEGER m_TotalSec;
	DWORD m_BytePerSec;
	BOOL m_StopScan;
// Dialog Data
	//{{AFX_DATA(CDeepScanDlg)
	enum { IDD = IDD_DEEPSCAN_DIALOG };
	CListCtrl	m_ScanList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeepScanDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	HANDLE m_Find;
	static DWORD WINAPI FindSomething(LPVOID lpVoid);
	// Generated message map functions
	//{{AFX_MSG(CDeepScanDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnDeepScan();
	afx_msg void OnStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEEPSCANDLG_H__781BB526_77E4_4558_9F04_D498270A9A8F__INCLUDED_)
