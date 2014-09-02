// FindStringDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FindString.h"
#include "FindStringDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindStringDlg dialog

CFindStringDlg::CFindStringDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindStringDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindStringDlg)
	m_data = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFindStringDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindStringDlg)
	DDX_Control(pDX, IDC_RESULT, m_result);
	DDX_Control(pDX, IDC_KEYWORD, m_keyword);
	DDX_Control(pDX, IDC_DRIVE, m_drive);
	DDX_Text(pDX, IDC_DATA, m_data);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFindStringDlg, CDialog)
	//{{AFX_MSG_MAP(CFindStringDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SCAN, OnScan)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_KEYWORD, OnEndlabeleditKeyword)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindStringDlg message handlers

BOOL CFindStringDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_keyword.ModifyStyle(LVS_ICON,LVS_EDITLABELS);
	m_result.ModifyStyle(LVS_ICON|LVS_SMALLICON|LVS_LIST,LVS_REPORT);
	m_keyword.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_drive.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_result.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_keyword.InsertItem(0,"请单击2次输入关键字");
	m_result.InsertColumn(0,"序号",LVCFMT_LEFT,50);
	m_result.InsertColumn(1,"关键字",LVCFMT_LEFT,50);
	m_result.InsertColumn(2,"物理位置",LVCFMT_LEFT,200);

	char str[]="\\\\.\\C: ";
	m_hDevice=CreateFile(str,GENERIC_READ,FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
	if(m_hDevice==INVALID_HANDLE_VALUE)
	{
		MessageBox("ERROR");
		SendMessage(WM_CLOSE,NULL,NULL);
	}
	NTFS ntfs;
	unsigned long NumOfRead;
	ReadFile(m_hDevice,&ntfs,SIZE,&NumOfRead,NULL);//读取硬盘
	if(ntfs.bpb.wBytesPerSec!=SIZE) SendMessage(WM_CLOSE,0,0);
	if(memcmp(ntfs.chOemID,"NTFS",4)==0)
	{
		m_TotalSec.QuadPart=ntfs.bpb.n64TotalSec;
		m_BytePerSec=ntfs.bpb.wBytesPerSec;
	}
	m_Find=NULL;
	m_StopScan=TRUE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFindStringDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFindStringDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFindStringDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFindStringDlg::OnScan() 
{
	// TODO: Add your control notification handler code here
	if(m_keyword.GetItemCount()==1) 
	{
		MessageBox("未设置关键字");
		return;
	}
	m_result.DeleteAllItems();
	m_StopScan=FALSE;
	m_Find=CreateThread(NULL,0,FindSomething,this,0,0);
}

void CFindStringDlg::OnStop() 
{
	// TODO: Add your control notification handler code here
	m_StopScan=TRUE;
}

void CFindStringDlg::OnEndlabeleditKeyword(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	int cur=pDispInfo->item.iItem;
	int Total=m_keyword.GetItemCount();
	CString input=pDispInfo->item.pszText;
	if(input=="")
	{
		if(Total!=cur+1)//如果删除的是中间的一个
		{//全体上移一格
			for(int i=cur;i<Total-1;i++)
			{
				m_keyword.SetItemText(i,0,m_keyword.GetItemText(i+1,0));
			}
			m_keyword.DeleteItem(Total-1);
		}
	}
	else
	{
		if(Total==cur+1)//如果修改的是最后一个
		{
			m_keyword.InsertItem(Total,"请单击2次输入关键字");
		}
		m_keyword.SetItemText(cur,0,input);
	}

	*pResult = 0;
}

DWORD WINAPI CFindStringDlg::FindSomething(LPVOID lpVoid)
{

	CFindStringDlg* CurDlg=(CFindStringDlg*)lpVoid;
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	DWORD dwGran=SysInfo.dwAllocationGranularity;
	DWORD dwBlockBytes=1000*dwGran;
	char* buf=new char[dwBlockBytes];
	CurDlg->m_result.DeleteAllItems();
	LARGE_INTEGER cum;
	cum.QuadPart=0;
	char sss[64];
	DWORD numofread;
	DWORD totaltime=CurDlg->m_TotalSec.QuadPart/(dwBlockBytes/CurDlg->m_BytePerSec)+1;//能否漏掉分区最后一部分?
	int numin=dwBlockBytes/CurDlg->m_BytePerSec;
	SetFilePointer(CurDlg->m_hDevice,0,NULL,FILE_BEGIN);

	int keynum=CurDlg->m_keyword.GetItemCount()-1;
	int* length=new int[keynum];
	int* length2=new int[keynum];
	CString* pos1=new CString[keynum];
	char** posascii=new char*[keynum];
	wchar_t** posunicode=new wchar_t*[keynum];

	for(int j=0;j<keynum;j++)
	{
		length[j]=CurDlg->m_keyword.GetItemText(j,0).GetLength();
		pos1[j]=CurDlg->m_keyword.GetItemText(j,0);
		posascii[j]=(LPTSTR)(LPCTSTR)pos1[j];
		length2[j]=MultiByteToWideChar(CP_ACP,0,posascii[j],length[j],NULL,0)*2;
		posunicode[j]=new wchar_t[length2[j]/2];
		MultiByteToWideChar(CP_ACP,0,posascii[j],length[j],posunicode[j],length2[j]/2);
	}

	for(DWORD count=0;count<totaltime;count++)
	{
		if(CurDlg->m_StopScan) break;
		sprintf(sss,"H:%08X,L:%08X  进度:%d%%",cum.HighPart,cum.LowPart,int(100*(float)count/totaltime));
		CurDlg->GetDlgItem(IDC_SHOW)->SetWindowText(sss);		
		
		ReadFile(CurDlg->m_hDevice,buf,dwBlockBytes,&numofread,NULL);
		
		for(int j=0;j<keynum;j++)//大循环位于中心
		{		
			CurDlg->Sunday(length[j],dwBlockBytes,buf,posascii[j],cum);
			CurDlg->Sunday(length2[j],dwBlockBytes,buf,(char*)posunicode[j],cum);
		}
		cum.QuadPart+=dwBlockBytes;
	}

	delete []buf;
	delete []pos1;
	delete []posascii;
	for(j=0;j<keynum;j++)
	{
		delete []posunicode[j];
	}
	delete []length;
	delete []length2;
	return TRUE;
}


void CFindStringDlg::Sunday(size_t patt_size,size_t text_size,char *text,char *patt,LARGE_INTEGER cum)
{//sunday搜索算法
	size_t shift[256];
	for(size_t i=0;i<256;i++)
	{
		*(shift+i)=patt_size+1;
	}
	for(i=0;i<patt_size;i++)
	{
		*(shift+unsigned char(*(patt+i)))=patt_size-i;
	}
	size_t limit=text_size-patt_size+1;
	for(i=0;i<limit;i+=shift[unsigned char(text[i+patt_size])])
	{
		if(text[i]==*patt)
		{
			char* match_text=text+i+1;
			size_t match_size=1;
			do 
			{
				if(match_size==patt_size)
				{
					char sss[64];
					LARGE_INTEGER LI;
					LI.QuadPart=cum.QuadPart+i;
					int curnum=m_result.GetItemCount();
					sprintf(sss,"%d",curnum);
					m_result.InsertItem(curnum,sss);
					m_result.SetItemText(curnum,1,patt);
					sprintf(sss,"位置:%08X:%08X",LI.HighPart,LI.LowPart);
					m_result.SetItemText(curnum,2,sss);
				}
			} 
			while((*match_text++)==patt[match_size++]);
		}
	}
}
