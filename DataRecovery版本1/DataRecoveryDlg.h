// DataRecoveryDlg.h : header file
//

#if !defined(AFX_DATARECOVERYDLG_H__8FB36C3E_C9B9_4622_A0C2_070C9BB4FEAB__INCLUDED_)
#define AFX_DATARECOVERYDLG_H__8FB36C3E_C9B9_4622_A0C2_070C9BB4FEAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "hoose.h"
#include "NTFSReader.h"
#include "Splitter.h"
#include "ListCtrlEx.h"
/////////////////////////////////////////////////////////////////////////////
// CDataRecoveryDlg dialog

class CDataRecoveryDlg : public CDialog
{
// Construction
public:
	CDataRecoveryDlg(CWnd* pParent = NULL);	// standard constructor
	Choose*			m_thisChoose;
	FILESTRUCT*		m_files;
	DirTree*		m_root;
	CString			m_FindStr;
	HTREEITEM		m_last;
	CImageList*		m_pImageList;
	DWORD			m_dwBlockBytes;
	DWORD			m_fileNum;
	BOOL			m_IsFindFirst;
	BOOL			m_StopThread;//搜索线程是否停止
	BOOL			m_isDirok;
	BOOL			m_isNTFS;
	DWORD			TotalSec;
	int				ActualNum;
	TableList*		TL;
	DWORD			GlobalOffset;
// Dialog Data
	//{{AFX_DATA(CDataRecoveryDlg)
	enum { IDD = IDD_DATARECOVERY_DIALOG };
	CListCtrlEx		m_filelist;
	CSplitter		m_Splitter;
	CProgressCtrl	m_Progress;
	CTreeCtrl		m_directory;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataRecoveryDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static DWORD WINAPI SearchNTFSFile(LPVOID lpVoid);
	static DWORD WINAPI SearchFAT32File(LPVOID lpVoid);
	static DWORD WINAPI RecoverFile(LPVOID lpVoid);
	static DWORD WINAPI ReadTable(LPVOID lpVoid);
	void RecoverFolder(CString path,HTREEITEM HI);
	void GetALLDirectory();//生成目录树
	void RecoverRuns(HTREEITEM hi,CString path);
	void SearchFAT32ChildDir(DWORD ClustNum,DWORD& MaxUsedNum,DWORD* FATTABLE);
	BOOL CheckPartition(DWORD RelativeSectors,BYTE PartitionID);
	void AddChild(DWORD FirstExtPos,DWORD CurExtPos,TableList* TL);
	HICON	m_hIcon;
	HANDLE	m_SearchFile;
	HANDLE	m_RecoverFile;
	HANDLE	m_SearchDisk;
	NTFS	m_ntfs;
	FAT32	m_fat32;
	// Generated message map functions
	//{{AFX_MSG(CDataRecoveryDlg)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOpen();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonEnd();
	afx_msg void OnDestroy();
	afx_msg void OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRecovery();
	afx_msg void OnFind();
	afx_msg void OnQuit();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATARECOVERYDLG_H__8FB36C3E_C9B9_4622_A0C2_070C9BB4FEAB__INCLUDED_)

