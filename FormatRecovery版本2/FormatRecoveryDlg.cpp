// FormatRecoveryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FormatRecovery.h"
#include "FormatRecoveryDlg.h"
#include "FormatDefs.h"
#include "ShowInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CFormatRecoveryDlg dialog

CFormatRecoveryDlg::CFormatRecoveryDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFormatRecoveryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFormatRecoveryDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFormatRecoveryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormatRecoveryDlg)
	DDX_Control(pDX, IDC_SEARCHAREA, m_searcharea);
	DDX_Control(pDX, IDC_SEARCHEXT, m_searchext);
	DDX_Control(pDX, IDC_ALLEXT, m_allext);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_RESULT, m_result);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFormatRecoveryDlg, CDialogEx)
	//{{AFX_MSG_MAP(CFormatRecoveryDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ADDEXT,&CFormatRecoveryDlg::OnAddext)
	ON_BN_CLICKED(IDC_ADDALL,&CFormatRecoveryDlg::OnAddall)
	ON_BN_CLICKED(IDC_DELEXT,&CFormatRecoveryDlg::OnDelext)
	ON_BN_CLICKED(IDC_SEARCH, &CFormatRecoveryDlg::OnSearch)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_RECOVERPOS,&CFormatRecoveryDlg::OnRecoverpos)
	ON_COMMAND(ID_RECOVERFILE,&CFormatRecoveryDlg::OnMenuRecoverFile)
	ON_COMMAND(ID_GETINFO,&CFormatRecoveryDlg::OnMenuGetInfo)
	ON_COMMAND(ID_FIRSTCLUSTER,&CFormatRecoveryDlg::OnGetFirstCluster)
	ON_NOTIFY(LVN_COLUMNCLICK,IDC_RESULT,&CFormatRecoveryDlg::OnColumnclickResult)
	ON_NOTIFY(NM_RCLICK,IDC_RESULT,&CFormatRecoveryDlg::OnNMRClickResult)
	ON_BN_CLICKED(IDC_STOP,&CFormatRecoveryDlg::OnBnClickedStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormatRecoveryDlg message handlers

BOOL CFormatRecoveryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
// 	if(__argc != 2 || 0 != strcmp(__argv[1],"NJU"))
// 		SendMessage(WM_CLOSE,0,0);
	SetErrorMode( SEM_NOGPFAULTERRORBOX | SEM_FAILCRITICALERRORS);

	// TODO: Add extra initialization here
	m_allext.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_searchext.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_result.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_result.InsertColumn(0,"序号",LVCFMT_LEFT,100);
	m_result.InsertColumn(1,"文件类型",LVCFMT_LEFT,200);
	m_result.InsertColumn(2,"文件大小",LVCFMT_LEFT,100);
	m_result.InsertColumn(3,"是否有效",LVCFMT_LEFT,100);

	pthread=NULL;
	recover=NULL;
	CFormatRecovery null(NULL,m_result,m_progress);//利用空类，仅仅获取dll支持类型，其他什么都不做
	CList<CString,CString&> mylist;
	null.GetSupportFileExtension(mylist);
	POSITION pos=mylist.GetHeadPosition();
	for(int i=0;i<mylist.GetCount();i++)
	{
		m_allext.InsertItem(i,mylist.GetNext(pos));
	}

	//枚举分区
	int bufsize=GetLogicalDriveStrings(0,NULL);
	char* buf=new char[bufsize+1];
	GetLogicalDriveStrings(bufsize,buf);
	char* ptr=buf;
	int index=0;
	while(*ptr)
	{
		UINT type=GetDriveType(ptr);
		if(type == DRIVE_REMOVABLE || type == DRIVE_FIXED || type == DRIVE_REMOTE)
		{
			char drive[16]="\\\\.\\";
			strcat_s(drive,ptr);//得到设备符号
			drive[strlen(drive)-1]='\0';
			m_searcharea.AddString(drive);
		}
		ptr+=strlen(ptr)+1;
	}
	delete []buf;

	//枚举硬盘
	char drive[32];
	int disknum=0;
	while(TRUE)
	{
		sprintf_s(drive,"\\\\.\\PHYSICALDRIVE%d",disknum++);//对于硬盘以0x1000为单位搜不到文件，以后的版本需要改善，改为0x08?
		HANDLE hDisk=CreateFile(drive,0,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hDisk == INVALID_HANDLE_VALUE)//如果句柄无效说明已经枚举完毕
			break;
		CloseHandle(hDisk);
		m_searcharea.AddString(drive);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFormatRecoveryDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFormatRecoveryDlg::OnPaint() 
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
		CDialogEx::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFormatRecoveryDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFormatRecoveryDlg::OnAddext() 
{//把所有扩展名列表项的已选择项加入待选扩展名列表
	// TODO: Add your control notification handler code here
	int count2=m_searchext.GetItemCount();
	POSITION itor=m_allext.GetFirstSelectedItemPosition();
	if(!itor)
		return;
	while(itor)
	{
		itor=m_allext.GetFirstSelectedItemPosition();
		int index=m_allext.GetNextSelectedItem(itor);
		m_searchext.InsertItem(count2++,m_allext.GetItemText(index,0));
		m_allext.DeleteItem(index);
	}
}

void CFormatRecoveryDlg::OnAddall() 
{//把所有扩展名列表项的全部项加入待选扩展名列表
	// TODO: Add your control notification handler code here
	int count1=m_allext.GetItemCount(),count2=m_searchext.GetItemCount();
	for(int i=0;i<m_allext.GetItemCount();i++)
	{
		m_searchext.InsertItem(count2++,m_allext.GetItemText(i,0));
	}
	m_allext.DeleteAllItems();
}

void CFormatRecoveryDlg::OnDelext() 
{//把待选扩展名列表的已选择项放回所有扩展名列表
	// TODO: Add your control notification handler code here
	int count2=m_allext.GetItemCount();
	POSITION itor=m_searchext.GetFirstSelectedItemPosition();
	if(!itor)
		return;
	while(itor)
	{
		itor=m_searchext.GetFirstSelectedItemPosition();
		int index=m_searchext.GetNextSelectedItem(itor);
		m_allext.InsertItem(count2++,m_searchext.GetItemText(index,0));
		m_searchext.DeleteItem(index);
	}
}

void CFormatRecoveryDlg::OnRecoverfile() 
{
	// TODO: Add your command handler code here
	if(!recover)
		return;
	CString recpos;
	GetDlgItemText(IDC_SAVEPOS,recpos);
	recover->DoRecovery(recpos);
}

UINT SearchThread(PVOID param)
{//开始搜索          用线程不会让界面卡死
	CFormatRecoveryDlg* dlg=(CFormatRecoveryDlg*)param;
	CString pos;
	CList<CString,CString&> tofind;
	int num,i;
	dlg->GetDlgItem(IDC_SEARCH)->EnableWindow(FALSE);
	int sel=dlg->m_searcharea.GetCurSel();
	if(sel == LB_ERR)
		goto END1;

	dlg->m_searcharea.GetText(sel,pos);
	dlg->m_diskhandle=CreateFile(pos,GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
	if(INVALID_HANDLE_VALUE == dlg->m_diskhandle)
		goto END1;
	
	num=dlg->m_searchext.GetItemCount();
	for(i=0;i<num;i++)
	{
		tofind.AddTail(dlg->m_searchext.GetItemText(i,0));
	}
	
	dlg->recover=new CFormatRecovery(dlg->m_diskhandle,dlg->m_result,dlg->m_progress);
	dlg->recover->Initialize(tofind);
	dlg->recover->AnalysisDisk();

END1:
	dlg->GetDlgItem(IDC_SEARCH)->EnableWindow(TRUE);
	dlg->pthread=NULL;
	return 0;
}

void CFormatRecoveryDlg::OnSearch() 
{
	// TODO: Add your control notification handler code here
	if(recover)
	{
		CloseHandle(m_diskhandle);
		delete recover;
	}

	if(!pthread)
		pthread=AfxBeginThread(SearchThread,(LPVOID)this);
}

void CFormatRecoveryDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
// 	if(recover)
// 	{
// 		CloseHandle(m_diskhandle);
// 		delete recover;
// 	}
// 	if(pthread)
// 		TerminateThread(pthread->m_hThread,0);
	CDialogEx::OnClose();
}

void CFormatRecoveryDlg::OnRecoverpos() 
{
	// TODO: Add your control notification handler code here
	BROWSEINFO stInfo = {NULL}; 
	LPCITEMIDLIST pIdlst; 
	CHAR szPath[MAX_PATH]; 
	
#define BIF_NEWDIALOGSTYLE      0x00000040 
#define BIF_USENEWUI            (BIF_NEWDIALOGSTYLE | BIF_EDITBOX)
	
	stInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI; 
	stInfo.lpszTitle= "请选择路径:"; 
	pIdlst = SHBrowseForFolder(&stInfo); 
	if(!pIdlst || !SHGetPathFromIDList(pIdlst, szPath)) 
		return ;
	int len=_tcslen(szPath);
	if(szPath[len-1] == '\\')
		szPath[len-1]='\0';

	SetDlgItemText(IDC_SAVEPOS,szPath);
}

void CFormatRecoveryDlg::OnMenuRecoverFile()
{
	if(!recover || recover->IsSearching())
		return;
	CString path;
	GetDlgItemText(IDC_SAVEPOS,path);
	if(path == "")
	{
		AfxMessageBox("请选择保存位置");
		return;
	}
	recover->DoRecovery(path);
}

void CFormatRecoveryDlg::OnMenuGetInfo()
{
	if(!recover || recover->IsSearching())
		return;
	CString result;
	recover->DoGetInfo(result);
	CShowInfo dlg(this);
	dlg.m_showinfo=result;
	dlg.DoModal();
}

void CFormatRecoveryDlg::OnGetFirstCluster()
{
	if(!recover || recover->IsSearching())
		return;
	POSITION itor=m_result.GetFirstSelectedItemPosition();
	if(!itor)
		return;

	CString result="";
	char format[8];
	int index=m_result.GetNextSelectedItem(itor);
	StorageData* pData=(StorageData*)m_result.GetItemData(index);
	
	CDiskMappingPointer pointer(m_diskhandle,pData->Base+pData->Offset);
	BYTE* ptr=(BYTE*)pointer;
	int i,j;
	for(i=0;i<0x100;i++)
	{
		for(j=0;j<8;j++)
		{
			memset(format,0,8);
			sprintf_s(format,"%02X ",ptr[0x10*i+j]);
			result += format;
		}
		result += "-- ";
		for(j=0;j<8;j++)
		{
			memset(format,0,8);
			sprintf_s(format,"%02X ",ptr[0x10*i+j+8]);
			result += format;
		}
		result += "\r\n";
	}
	CShowInfo dlg(this);
	dlg.m_showinfo=result;
	dlg.DoModal();
}

MakeCompareFunc(CFormatRecoveryDlg,m_result);

void CFormatRecoveryDlg::OnColumnclickResult(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if(!recover || recover->IsSearching())
		return;
	int num=m_result.GetItemCount();
	for(int i=0;i<num;i++)
	{
		m_result.SetItemData(i,i);
	}
	
	col=pNMLV->iSubItem;
	
	if(direct[col])
		m_result.SortItems(CompareFuncUp,(LPARAM)this);
	else
		m_result.SortItems(CompareFuncDown,(LPARAM)this);
	direct[col]=!direct[col];

	*pResult = 0;
}

void CFormatRecoveryDlg::OnBnClickedStop()
{
	// TODO: 在此添加控件通知处理程序代码
	if(recover)
	{
		recover->Stop();
	}
}

void CFormatRecoveryDlg::OnNMRClickResult(NMHDR* pNMHDR, LRESULT* pResult)
{
	CMenu menu,*pmenu;
	menu.LoadMenu(IDR_RECOVER);
	pmenu=menu.GetSubMenu(0);
	if(pmenu)
	{
		POINT point;
		GetCursorPos(&point);
		pmenu->TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);
	}

	*pResult=0;
}