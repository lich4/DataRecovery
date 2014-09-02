// ReadTableDlg.h : header file
//

#if !defined(AFX_READTABLEDLG_H__E3B66B7D_2153_4F83_93EC_E6A2F1A07740__INCLUDED_)
#define AFX_READTABLEDLG_H__E3B66B7D_2153_4F83_93EC_E6A2F1A07740__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CReadTableDlg dialog

#pragma pack(push, curAlignment)
#pragma pack(1)

struct PartitionTable
{
	BYTE	BootIndicator;
	BYTE	StartingHead;
	WORD	StartData;
	BYTE	PartitionID;
	BYTE	EndingHead;
	WORD	EndData;
	DWORD	RelativeSectors;
	DWORD	TotalSectors;
};

#pragma pack(pop, curAlignment)

struct TableList
{
	PartitionTable	Table;
	TableList*		Next;
	BOOL			IsPartValid;
	TableList()
	{
		Next=NULL;
	}
};

class CReadTableDlg : public CDialog
{
// Construction
public:
	CReadTableDlg(CWnd* pParent = NULL);	// standard constructor
	HANDLE hdev;
	TableList* TL;
	DWORD TotalSec;
	int ActualNum;
// Dialog Data
	//{{AFX_DATA(CReadTableDlg)
	enum { IDD = IDD_READTABLE_DIALOG };
	CListCtrl	m_disklist;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReadTableDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	static DWORD WINAPI ReadTable(LPVOID lpVoid);
	void AddChild(DWORD FirstExtPos,DWORD CurExtPos,TableList* ItemToAdd);
	void DrawMain();
	BOOL CheckPartition(DWORD RelativeSectors,BYTE PartitionID);
	// Generated message map functions
	//{{AFX_MSG(CReadTableDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_READTABLEDLG_H__E3B66B7D_2153_4F83_93EC_E6A2F1A07740__INCLUDED_)
