#if !defined(AFX_HOOSE_H__C60C81B6_19DA_47A5_8D0F_75CB27F9EEF9__INCLUDED_)
#define AFX_HOOSE_H__C60C81B6_19DA_47A5_8D0F_75CB27F9EEF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// hoose.h : header file
//
#include "NTFSReader.h"
/////////////////////////////////////////////////////////////////////////////
// Choose dialog
#define IDC_MyListCtrl 10000

class Choose : public CDialog
{
// Construction
public:
	Choose(CWnd* pParent = NULL);   // standard constructor
	CImageList*		pImageList;
	CListCtrl*		m_list;
	HANDLE			hDevice;
	HANDLE			m_SearchDisk;
	unsigned int	m_TotalSec;
	TableList*		TL;
	DWORD			TotalSec;
	int				ActualNum;
	DWORD			OFFSET;
	int				PARTID;
// Dialog Data
	//{{AFX_DATA(Choose)
	enum { IDD = IDD_CHOOSE };
	CTabCtrl	m_choose;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Choose)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DrawMain();
	// Generated message map functions
	//{{AFX_MSG(Choose)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickTab(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnRefresh();
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOOSE_H__C60C81B6_19DA_47A5_8D0F_75CB27F9EEF9__INCLUDED_)
