// DeepScan2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeepScan2.h"
#include "DeepScan2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
DWORD ExitCode=0;
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
// CDeepScan2Dlg dialog

CDeepScan2Dlg::CDeepScan2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeepScan2Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeepScan2Dlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDeepScan2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeepScan2Dlg)
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDeepScan2Dlg, CDialog)
	//{{AFX_MSG_MAP(CDeepScan2Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CDeepScan2Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDeepScan2Dlg::OnBnClickedButton2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeepScan2Dlg message handlers

BOOL CDeepScan2Dlg::OnInitDialog()
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
	m_list.ModifyStyle(LVS_ICON|LVS_SMALLICON|LVS_LIST,LVS_REPORT|LVS_EDITLABELS|LVS_SINGLESEL);
	m_list.InsertColumn(0,"序号",LVCFMT_LEFT,50);	
	m_list.InsertColumn(1,"介质名称",LVCFMT_LEFT,100);
	m_list.InsertColumn(2,"型号",LVCFMT_LEFT,120);
	m_list.InsertColumn(3,"版本",LVCFMT_LEFT,50);
	m_list.InsertColumn(4,"序列号",LVCFMT_LEFT,200);
	m_list.InsertColumn(5,"上次访问时间",LVCFMT_LEFT,145);
	hDevice=CreateFile("\\\\.\\C: ",GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
	if(hDevice==INVALID_HANDLE_VALUE) 
	{
		MessageBox("ERROR");
		SendMessage(WM_CLOSE,NULL,NULL);
	}
	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDeepScan2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDeepScan2Dlg::OnPaint() 
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
HCURSOR CDeepScan2Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

DWORD WINAPI CDeepScan2Dlg::DeepScan(LPVOID lpVoid)
{
	CDeepScan2Dlg* CurDlg=(CDeepScan2Dlg*)lpVoid;
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	DWORD dwGran=SysInfo.dwAllocationGranularity;
	DWORD dwBlockBytes=1000*dwGran;
	int numonetime=dwBlockBytes/8;
	char* Buf=new char[dwBlockBytes];
	DWORD	NumOfRead;
	int findnum=0;
	LARGE_INTEGER LI,BytesOfVolume;
	DWORD dwSecPerCluster,dwBytesPerSec,dwNumOfFreeClu,dwTotalClu;
	LI.QuadPart=0;
	char temp[256];
	CString str;

	GetDiskFreeSpace("C:\\",&dwSecPerCluster,&dwBytesPerSec,&dwNumOfFreeClu,&dwTotalClu);
	BytesOfVolume.QuadPart=((LONGLONG)dwBytesPerSec )* dwSecPerCluster * dwTotalClu;

	do 
	{
		ReadFile(CurDlg->hDevice,Buf,dwBlockBytes,&NumOfRead,NULL);
		char* p=Buf;
		for(int i=0;i<numonetime;i++,p+=8)
		{
			if(memcmp(p,"Disk&Ven_",9))	continue;
			if(*(p+9)<'A'||*(p+9)>'z') continue;
			Key_Cell* CurCell=(Key_Cell*)(p-sizeof(Key_Cell));
			sprintf(temp,"%5d",findnum);
			CurDlg->m_list.InsertItem(findnum,temp);
			SYSTEMTIME st;
			FileTimeToSystemTime((FILETIME*)&CurCell->LastWriteTime,&st);
			sprintf(temp,"%d-%d-%d,%d:%d:%d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			CurDlg->m_list.SetItemText(findnum,5,temp);

			CString c_str=p,buffer;
			int flag;
			c_str.TrimLeft("Disk&Ven_");
			flag=c_str.FindOneOf("&");
			buffer=c_str.Left(flag);
			CurDlg->m_list.SetItemText(findnum,1,buffer);
			c_str.TrimLeft(buffer+"&Prod_");
			flag=c_str.FindOneOf("&");
			buffer=c_str.Left(flag);
			c_str.Left(flag);
			CurDlg->m_list.SetItemText(findnum,2,buffer);
			c_str.TrimLeft(buffer+"&Rev_");
			CurDlg->m_list.SetItemText(findnum,3,c_str);

			for(int j=0;j<0x100;j+=8)
			{
				if(memcmp(p+j+4,"nk",2)==0)
				{
					CurDlg->m_list.SetItemText(findnum,4,(char*)(p+j+sizeof(Key_Cell)));
					break;
				}
			}
			findnum++;
		}
		LI.QuadPart+=dwBlockBytes;
		int e =(int)((double) LI.QuadPart/BytesOfVolume.QuadPart*100);
		str.Format("正在搜索，请稍候！已进行%d%%",e);
		CurDlg->SetDlgItemText(IDC_STATIC,str);
	}
	while (NumOfRead);

	str.Format("完成搜索！");
	CurDlg->SetDlgItemText(IDC_STATIC,str);

	delete []Buf;
	return true;
}


void CDeepScan2Dlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	GetExitCodeThread(hDeepScan,&ExitCode);
	if(ExitCode!=STILL_ACTIVE)
	{
		hDeepScan=CreateThread(NULL,0,DeepScan,this,0,0);				
		SetDlgItemText(IDC_BUTTON1,"检查中……");
	}
}


void CDeepScan2Dlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	TerminateThread(hDeepScan,0);
	CloseHandle(hDeepScan);
	SetDlgItemText(IDC_BUTTON1,"开始检查");
	SetDlgItemText(IDC_STATIC,"");
}
