// DeepScanDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeepScan.h"
#include "DeepScanDlg.h"
#include "Index_Dat.h"

#define SIZE 512

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
// CDeepScanDlg dialog

CDeepScanDlg::CDeepScanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeepScanDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeepScanDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDeepScanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeepScanDlg)
	DDX_Control(pDX, IDC_LIST, m_ScanList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDeepScanDlg, CDialog)
	//{{AFX_MSG_MAP(CDeepScanDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_DEEP_SCAN, OnDeepScan)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeepScanDlg message handlers

BOOL CDeepScanDlg::OnInitDialog()
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

	m_ScanList.ModifyStyle(LVS_ICON|LVS_SMALLICON|LVS_LIST,LVS_REPORT);
	m_ScanList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);	
	m_ScanList.InsertColumn(0,"序号",LVCFMT_LEFT,50);
	m_ScanList.InsertColumn(1,"上网记录",LVCFMT_LEFT,200);
	m_ScanList.InsertColumn(2,"标识",LVCFMT_LEFT,50);
	m_ScanList.InsertColumn(3,"缓存文件名",LVCFMT_LEFT,100);
	m_ScanList.InsertColumn(4,"最后修改时间",LVCFMT_LEFT,100);
	m_ScanList.InsertColumn(5,"最后访问时间",LVCFMT_LEFT,100);
	m_ScanList.InsertColumn(6,"存取(点击)次数",LVCFMT_LEFT,80);
	m_ScanList.InsertColumn(7,"引用次数",LVCFMT_LEFT,80);
	m_ScanList.InsertColumn(8,"HTTP头部信息",LVCFMT_LEFT,200);
	m_ScanList.InsertColumn(9,"位置",LVCFMT_LEFT,200);

	// TODO: Add extra initialization here
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
	ASSERT(ntfs.bpb.wBytesPerSec==SIZE);
	if(memcmp(ntfs.chOemID,"NTFS",4)==0)
	{
		m_TotalSec.QuadPart=ntfs.bpb.n64TotalSec;
		m_BytePerSec=ntfs.bpb.wBytesPerSec;
	}
	m_Find=NULL;
	m_StopScan=TRUE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDeepScanDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDeepScanDlg::OnPaint() 
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
HCURSOR CDeepScanDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDeepScanDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	CloseHandle(m_hDevice);
	if(m_Find) CloseHandle(m_Find);
	// TODO: Add your message handler code here
	
}

void CDeepScanDlg::OnDeepScan() 
{
	// TODO: Add your control notification handler code here
	m_StopScan=FALSE;
	m_Find=CreateThread(NULL,0,FindSomething,this,0,0);
}
	
DWORD WINAPI CDeepScanDlg::FindSomething(LPVOID lpVoid)
{
	CDeepScanDlg* CurDlg=(CDeepScanDlg*)lpVoid;
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	DWORD dwGran=SysInfo.dwAllocationGranularity;
	DWORD dwBlockBytes=1023*dwGran;//ReadFile一次最多读取
	char* buf=new char[dwBlockBytes];
	DWORD totaltime=CurDlg->m_TotalSec.QuadPart/(dwBlockBytes/CurDlg->m_BytePerSec)+1;
	SetFilePointer(CurDlg->m_hDevice,0,NULL,FILE_BEGIN);
	
	URL_LEAK UL;
	LARGE_INTEGER cum;
	cum.QuadPart=0;
	char str[64];
	DWORD numofread;

	for(DWORD count=0;count<totaltime;count++)
	{
		sprintf(str,"位置:%08X:%08X  进度:%d%%",cum.HighPart,cum.LowPart,int(100*(float)count/totaltime));
		CurDlg->GetDlgItem(IDC_SHOW)->SetWindowText(str);		
		if(CurDlg->m_StopScan) return TRUE;
		ReadFile(CurDlg->m_hDevice,buf,dwBlockBytes,&numofread,NULL);
		
		for(DWORD offset=0;offset<dwBlockBytes;offset+=0x80)//0x80
		{
			char *p=buf+offset;
			if((p[0]=='U'||p[0]=='L')&&(p[1]=='R'||p[1]=='E')&&(p[2]=='L'||p[2]=='A')&&(p[3]==' '||p[3]=='K'))
			{
				memcpy(&UL,buf+offset,sizeof(URL_LEAK));
				if(UL.nBlocks>0x20 || UL.UrlNameOffset>0x1000) continue;				
				memset(str,0,64);
				int num=CurDlg->m_ScanList.GetItemCount();

				sprintf(str,"%d",num);
				CurDlg->m_ScanList.InsertItem(num,str);
				CurDlg->m_ScanList.SetItemText(num,1,buf+offset+UL.UrlNameOffset);
				memcpy(str,buf+offset,4);
				str[4]='\0';	
				
				CurDlg->m_ScanList.SetItemText(num,2,str);
				if(UL.InternalFileNameOffset)
					CurDlg->m_ScanList.SetItemText(num,3,buf+offset+UL.InternalFileNameOffset);
				SYSTEMTIME st;
				FileTimeToSystemTime((FILETIME*)&UL.LastModifiedTime,&st);
				sprintf(str,"%d年%d月%d日 %d:%d:%d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
				CurDlg->m_ScanList.SetItemText(num,4,str);
				FileTimeToSystemTime((FILETIME*)&UL.LastAccessedTime,&st);
				sprintf(str,"%d年%d月%d日 %d:%d:%d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
				CurDlg->m_ScanList.SetItemText(num,5,str);
				sprintf(str,"%d",UL.NumAccessed);
				CurDlg->m_ScanList.SetItemText(num,6,str);
				sprintf(str,"%d",UL.NumReferences);
				CurDlg->m_ScanList.SetItemText(num,7,str);
				if(UL.HeaderInfoSize)
				{
					char* HTTP=new char[UL.HeaderInfoSize];
					memcpy(HTTP,buf+offset+UL.HeaderInfoOffset,UL.HeaderInfoSize);
					CurDlg->m_ScanList.SetItemText(num,8,HTTP);
					delete []HTTP;
				}
				LARGE_INTEGER temp;
				temp.QuadPart=cum.QuadPart+offset;
				sprintf(str,"H:%08X,L:%08X",temp.HighPart,temp.LowPart);
				CurDlg->m_ScanList.SetItemText(num,9,str);
			}
			else if(p[0]=='R'&&p[1]=='E'&&p[2]=='D'&&p[3]=='R')
			{
				REDIR redir;
				memcpy(&redir,buf+offset,sizeof(REDIR));
				if(redir.nBlocks>0xff) continue;
				char str[64];
				memset(str,0,64);
				int num=CurDlg->m_ScanList.GetItemCount();
				sprintf(str,"%d",num);
				CurDlg->m_ScanList.InsertItem(num,str);
				CurDlg->m_ScanList.SetItemText(num,1,buf+offset+sizeof(REDIR));
				sprintf(str,"%s",buf+offset);	
				str[4]='\0';
				CurDlg->m_ScanList.SetItemText(num,2,str);
				LARGE_INTEGER temp;
				temp.QuadPart=cum.QuadPart+offset;
				sprintf(str,"H:%08X,L:%08X",temp.HighPart,temp.LowPart);
				CurDlg->m_ScanList.SetItemText(num,9,str);
			}
		}
		cum.QuadPart+=dwBlockBytes;
	}
	delete []buf;
	CurDlg->GetDlgItem(IDC_SHOW)->SetWindowText("ok");
	return TRUE;
}

void CDeepScanDlg::OnStop() 
{
	// TODO: Add your control notification handler code here
	m_StopScan=TRUE;
}
