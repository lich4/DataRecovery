// FindStringDlg.h : header file
//

#if !defined(AFX_FINDSTRINGDLG_H__5BB4ADD9_A049_41D4_B1B2_BD5851683261__INCLUDED_)
#define AFX_FINDSTRINGDLG_H__5BB4ADD9_A049_41D4_B1B2_BD5851683261__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SIZE 512
#define MAX_CHAR_SIZE	256
/////////////////////////////////////////////////////////////////////////////
// CFindStringDlg dialog

class CFindStringDlg : public CDialog
{
// Construction
public:
	void Sunday(size_t patt_size,size_t text_size,char* text,char* patt,LARGE_INTEGER cum);
	CFindStringDlg(CWnd* pParent = NULL);	// standard constructor
	HANDLE m_hDevice;
	LARGE_INTEGER m_TotalSec;
	DWORD m_BytePerSec;
	BOOL m_StopScan;
// Dialog Data
	//{{AFX_DATA(CFindStringDlg)
	enum { IDD = IDD_FINDSTRING_DIALOG };
	CListCtrl	m_result;
	CListCtrl	m_keyword;
	CListCtrl	m_drive;
	CString	m_data;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindStringDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	HANDLE m_Find;
	static DWORD WINAPI FindSomething(LPVOID lpVoid);
	// Generated message map functions
	//{{AFX_MSG(CFindStringDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnScan();
	afx_msg void OnStop();
	afx_msg void OnEndlabeleditKeyword(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#pragma pack(push, curAlignment)
#pragma pack(1)

struct NTFS//NTFS引导扇区
{
	char		chJumpInstruction[3];//跳转指令
	char		chOemID[4];//改行和下行表示的是文件系统，内容是"NTFS"。
	char		chDummy[4];//四个空格
	
	struct NTFS_BPB
	{
		WORD		wBytesPerSec;//每扇区字节数
		BYTE		uchSecPerClust;//每簇扇区数
		WORD		wReservedSec;//保留扇区
		BYTE		uchReserved[3];//总是0
		WORD		wUnused1;//未使用 为0
		BYTE		uchMediaDescriptor;//介质描述
		WORD		wUnused2;//未使用 为0
		WORD		wSecPerTrack;//每磁道扇区数目
		WORD		wNumberOfHeads;//每柱面磁头数
		DWORD		dwHiddenSec;//隐含扇区
		DWORD		dwUnused3;//未使用 为0
		DWORD		dwUnused4;//未使用 为80008000
		LONGLONG	n64TotalSec;//扇区总数
		LONGLONG	n64MFTLogicalClustNum;//$MFT的起始逻辑簇号
		LONGLONG	n64MFTMirrLogicalClustNum;//$MFTMirr的起始逻辑簇号
		int			nClustPerMFTRecord;//文件记录的大小描述
		int			nClustPerIndexRecord;//索引缓冲的大小描述
		LONGLONG	n64VolumeSerialNum;//分区的逻辑序列号，此处为格式化时随机产生
		DWORD		dwChecksum;//校验和
	} bpb;
	
	char		chBootstrapCode[426];//启动代码
	WORD		wSecMark;//引导扇区结束标志55AA
};

#pragma pack(pop, curAlignment)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDSTRINGDLG_H__5BB4ADD9_A049_41D4_B1B2_BD5851683261__INCLUDED_)
