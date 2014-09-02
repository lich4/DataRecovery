// DataRecoveryDlg.cpp : implementation file
//
#include "stdafx.h"
#include "DataRecovery.h"
#include "DataRecoveryDlg.h"
#include "NTFSReader.h"
#include "FindDlg1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SIZE 512//若扇区大小为512

#ifndef BIF_NEWDIALOGSTYLE
#define BIF_NEWDIALOGSTYLE 0x40
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
BOOL initialed=FALSE;

struct CompareData
{
	CListCtrlEx*	listctrl;
	int				isub;
};

struct RecoverData
{
	CDataRecoveryDlg*	CurDlg;
	CFile*				curfile;
	LARGE_INTEGER		FileBeginPos;
	LARGE_INTEGER		FileSize;
	LARGE_INTEGER		FileAllocSize;
	CString				path;
};

int CALLBACK MyCompareLtoH(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort)
{
	CompareData* cur=(CompareData*)lParamSort;
	return strcmp(((FILESTRUCT*)lParam1)->DATA[cur->isub],((FILESTRUCT*)lParam2)->DATA[cur->isub]);
}

int CALLBACK MyCompareHtoL(LPARAM lParam1,LPARAM lParam2,LPARAM lParamSort)
{
	CompareData* cur=(CompareData*)lParamSort;
	return strcmp(((FILESTRUCT*)lParam2)->DATA[cur->isub],((FILESTRUCT*)lParam1)->DATA[cur->isub]);
}

int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData) 
{
	switch(uMsg) 
	{
	case BFFM_INITIALIZED: 
		SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)pData);
		break;
	default:
		break;
	}
	return 0;
	lp=lp;
}

BOOL GetDirectory(HWND hWnd, LPTSTR szDir, LPTSTR szDefaultPath)
{ 
	BOOL			fRet; 
	TCHAR			szPath[SIZE]; 
	LPITEMIDLIST	pidl; 
	LPMALLOC		lpMalloc;  
	BROWSEINFO		bi={hWnd,NULL,szPath,"Please select a folder to extract",
		BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE,NULL,0L,0};
	if(szDefaultPath)
	{
		bi.lpfn = BrowseCallbackProc;
		bi.lParam = (LPARAM)szDefaultPath;
	}
	pidl = SHBrowseForFolder(&bi);  
	if (NULL != pidl) 
		fRet = SHGetPathFromIDList(pidl, szDir); 
	else 
		fRet = FALSE; 
	// Get the shell's allocator to free PIDLs 
	if(!SHGetMalloc(&lpMalloc) && (NULL != lpMalloc)) 
	{ 
		if(NULL != pidl) 
		{ 
			lpMalloc->Free(pidl); 
		}  
		lpMalloc->Release(); 
	}  
	return fRet; 
}

/////////////////////////////////////////////////////////////////////////////
// CDataRecoveryDlg dialog

CDataRecoveryDlg::CDataRecoveryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDataRecoveryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDataRecoveryDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDataRecoveryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataRecoveryDlg)
	DDX_Control(pDX, IDC_LIST, m_filelist);
	DDX_Control(pDX, IDC_SPLITTER, m_Splitter);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_TREE, m_directory);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDataRecoveryDlg, CDialog)
	//{{AFX_MSG_MAP(CDataRecoveryDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_BUTTON_END, OnButtonEnd)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST, OnColumnclickList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST, OnRclickList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_COMMAND(ID_RECOVERY, OnRecovery)
	ON_COMMAND(ID_FIND, OnFind)
	ON_COMMAND(ID_QUIT, OnQuit)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST, OnKeydownList)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataRecoveryDlg message handlers

void CDataRecoveryDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDataRecoveryDlg::OnPaint() 
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
HCURSOR CDataRecoveryDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDataRecoveryDlg::OnOpen() 
{
	// TODO: Add your command handler code here
	m_isDirok=FALSE;
	if(m_thisChoose) 
	{		
		if(m_thisChoose->hDevice)
			CloseHandle(m_thisChoose->hDevice);
		delete m_thisChoose;
	}
	if(m_files)
	{
		delete []m_files;
		m_files=NULL;
	}
	m_fileNum=0;
	m_thisChoose=new Choose;
	m_thisChoose->m_SearchDisk=m_SearchDisk;
	m_thisChoose->TL=TL;
	m_thisChoose->TotalSec=TotalSec;
	m_thisChoose->ActualNum=ActualNum;
	m_filelist.DeleteAllItems();
	m_directory.DeleteAllItems();
	if(IDOK==m_thisChoose->DoModal())
	{
		GlobalOffset=m_thisChoose->OFFSET;
		unsigned long NumOfRead;
		LARGE_INTEGER LI;
		LI.QuadPart=GlobalOffset;
		LI.QuadPart*=512;
		SetFilePointer(m_thisChoose->hDevice,LI.LowPart,&LI.HighPart,FILE_BEGIN);
		switch(m_thisChoose->PARTID)
		{
			case 0x0B:
			case 0x0C:
			case 0x16:
			case 0x1B:
			case 0x1C://FAT32
				FAT32 fat32;
				ReadFile(m_thisChoose->hDevice,&fat32,SIZE,&NumOfRead,NULL);
				ASSERT(fat32.bpb.wBytesPerSec==SIZE);
				m_fat32=fat32;
				m_SearchFile=CreateThread(NULL,0,SearchFAT32File,this,0,0);
				ASSERT(m_SearchFile!=0);
				SetDlgItemText(IDC_STATE,"正在扫描");
				SetDlgItemText(IDC_BUTTON_END,"停止扫描");
				m_isNTFS=FALSE;
				break;

			case 0x07:
			case 0x17://NTFS
				NTFS ntfs;
				ReadFile(m_thisChoose->hDevice,&ntfs,SIZE,&NumOfRead,NULL);//读取硬盘
				ASSERT(ntfs.bpb.wBytesPerSec==SIZE);
				m_ntfs=ntfs;
				m_SearchFile=CreateThread(NULL,0,SearchNTFSFile,this,0,0);
				ASSERT(m_SearchFile!=0);
				SetDlgItemText(IDC_STATE,"正在扫描"); 
				SetDlgItemText(IDC_BUTTON_END,"停止扫描");
				m_isNTFS=TRUE;
				break;

			default:
				MessageBox("ERROR!");
				return;
		}
	}
	delete m_thisChoose->m_choose;
	delete m_thisChoose->m_list;
	delete m_thisChoose->pImageList;
}

BOOL CDataRecoveryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	 
	// Add "About..." menu item to system menu.
	
	// IDM_ABOUTBOX must be in the system command range.
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
		}
	}
	
	initialed=TRUE;
	m_Splitter.m_Father=this;
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_thisChoose=NULL;
	m_SearchFile=NULL;
	m_RecoverFile=NULL;
	m_SearchDisk=NULL;
	m_files=NULL;
	m_root=NULL;
	m_isDirok=FALSE;
	m_fileNum=0;
	m_filelist.ModifyStyle(LVS_ICON|LVS_SMALLICON|LVS_LIST,LVS_REPORT|LVS_EDITLABELS|LVS_SINGLESEL);
	m_directory.ModifyStyle(LVS_EDITLABELS,LVS_SINGLESEL|TVS_SINGLEEXPAND);
	m_filelist.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);	
	m_filelist.InsertColumn(0,"文件名",LVCFMT_LEFT,100);
	m_filelist.InsertColumn(1,"序号",LVCFMT_LEFT,70);
	m_filelist.InsertColumn(2,"分配的文件大小(Byte)",LVCFMT_LEFT,120);
	m_filelist.InsertColumn(3,"真实的文件大小(Byte)",LVCFMT_LEFT,120);
	m_filelist.InsertColumn(4,"文件属性",LVCFMT_LEFT,100);
	m_filelist.InsertColumn(5,"已删除?",LVCFMT_LEFT,60);
	m_filelist.InsertColumn(6,"创建时间",LVCFMT_LEFT,150);
	m_filelist.InsertColumn(7,"最后修改时间",LVCFMT_LEFT,150);
	m_filelist.InsertColumn(8,"MFT修改时间",LVCFMT_LEFT,150);
	m_filelist.InsertColumn(9,"最后访问时间",LVCFMT_LEFT,150);
	m_filelist.InsertColumn(10,"文件MFT参考号",LVCFMT_LEFT,100);
	m_filelist.InsertColumn(11,"父目录MFT参考号",LVCFMT_LEFT,100);
	m_filelist.InsertColumn(12,"文件参考位置(Byte)",LVCFMT_LEFT,120);
	m_filelist.InsertColumn(13,"多运行附加信息",LVCFMT_LEFT,400);
	int array[]={1,0,2,3,4,5,6,7,8,9,10,11,12,13};//总共14个不变
	m_filelist.SetColumnOrderArray(14,array);
	m_StopThread=TRUE;
	m_FindStr="";
	m_IsFindFirst=TRUE;
	m_last=NULL;

	HICON hIcon=ExtractIcon(NULL,"%SystemRoot%\\System32\\Shell32.dll",0);
	m_pImageList=NULL;
 	m_pImageList=new CImageList;
 	ASSERT(m_pImageList!=NULL);
 	m_pImageList->Create(24,24,ILC_COLOR32|ILC_MASK,2,0);
 	m_pImageList->Add(hIcon);
	hIcon=ExtractIcon(NULL,"%SystemRoot%\\System32\\Shell32.dll",4);
	m_pImageList->Add(hIcon);
 	DeleteObject(hIcon);	
 	m_filelist.SetImageList(m_pImageList,LVSIL_SMALL);

	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	DWORD dwGran=SysInfo.dwAllocationGranularity;
	m_dwBlockBytes=1000*dwGran;
	m_SearchDisk=CreateFile("\\\\.\\PhysicalDrive0",GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
	if(m_SearchDisk==INVALID_HANDLE_VALUE) MessageBox("无法打开硬盘");
	else
	{
		char Buf[512];
		DWORD readnum;
		TotalSec=0;
		//计算总的硬盘扇区大小
		ReadFile(m_SearchDisk,Buf,512,&readnum,NULL);
		char* p=Buf+446;
		PartitionTable PT;
		TL=new TableList;
		for(int i=0;i<4;i++)
		{		
			memcpy(&PT,p,sizeof(PartitionTable));
			if(PT.PartitionID==0) break;
			TableList* cur=TL;
			while(cur->Next) cur=cur->Next;
			cur->Next=new TableList;
			
			PartitionTable* curtable=&cur->Next->Table;
			curtable->BootIndicator=PT.BootIndicator;
			curtable->EndData=PT.EndData;
			curtable->EndingHead=PT.EndingHead;
			curtable->PartitionID=PT.PartitionID;
			curtable->RelativeSectors=PT.RelativeSectors;
			curtable->StartData=PT.StartData;
			curtable->StartingHead=PT.StartingHead;
			curtable->TotalSectors=PT.TotalSectors;
			cur->Next->IsPartValid=CheckPartition(curtable->RelativeSectors,curtable->PartitionID);
			if(PT.PartitionID==0xF) 
				AddChild(PT.RelativeSectors,PT.RelativeSectors,TL);
			p+=sizeof(PartitionTable);
			TotalSec+=PT.TotalSectors;
		}
		int num=0;
		TableList* cur=TL->Next;
		while(cur) 
		{
			num++;
			cur=cur->Next;
		}
		ActualNum=num;
//		HANDLE Table=CreateThread(NULL,0,ReadTable,this,0,0);
//		SetThreadPriority(Table,THREAD_PRIORITY_LOWEST);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

DWORD WINAPI CDataRecoveryDlg::SearchNTFSFile(LPVOID lpVoid)
{
	char Buf[2*SIZE];
	char dest[SIZE];
	DWORD NumOfRead;
	CDataRecoveryDlg* CurDlg=(CDataRecoveryDlg*)lpVoid;
	Choose* choosevolume=CurDlg->m_thisChoose;
	LARGE_INTEGER offsetH;

	offsetH.QuadPart=(CurDlg->m_ntfs.bpb.n64MFTLogicalClustNum*CurDlg->m_ntfs.bpb.uchSecPerClust+
		CurDlg->GlobalOffset)*CurDlg->m_ntfs.bpb.wBytesPerSec;
	SetFilePointer(choosevolume->hDevice,offsetH.LowPart,&offsetH.HighPart,FILE_BEGIN);
	unsigned char* run=NULL,*ori=NULL;
	{
		char $MFT[SIZE];
		ReadFile(choosevolume->hDevice,$MFT,SIZE,&NumOfRead,NULL);//读取$MFT以获取文件记录开始及大小
		int offset=0x38;//跳过MFT头
		NTFS_ATTRIBUTE NA;
		while(offset<SIZE)
		{
			memcpy(&NA,$MFT+offset,sizeof(NTFS_ATTRIBUTE));
			if(NA.dwType==0x80)
			{
				ASSERT(NA.dwFullLength-NA.Attr.NonResident.wDatarunOffset>0);
				run=new unsigned char[NA.dwFullLength-NA.Attr.NonResident.wDatarunOffset];
				memcpy(run,$MFT+offset+NA.Attr.NonResident.wDatarunOffset,NA.dwFullLength-NA.Attr.NonResident.wDatarunOffset);
				break;
			}
			offset+=NA.dwFullLength;
		}
	}
	ori=run;
	unsigned char p=*run;

	LARGE_INTEGER originPos;
	originPos.QuadPart=CurDlg->GlobalOffset;
	originPos.QuadPart*=CurDlg->m_ntfs.bpb.wBytesPerSec;
	SetFilePointer(choosevolume->hDevice,originPos.LowPart,&originPos.HighPart,FILE_BEGIN);

	int deleted=0;
	do
	{
		choosevolume->m_TotalSec=0;
		LARGE_INTEGER LI;
		LI.QuadPart=0;
		memcpy(&choosevolume->m_TotalSec,run+1,p&0xf);
		choosevolume->m_TotalSec*=CurDlg->m_ntfs.bpb.uchSecPerClust/2;
		memcpy(&LI.QuadPart,run+1+(p&15),p>>4);
		LONGLONG LL=1;
		LL<<=(p>>4)*8;
		if((LI.QuadPart&(LL>>1))&&(ori!=run))
			LI.QuadPart-=LL<<1;//负偏移位置
		LI.QuadPart*=CurDlg->m_ntfs.bpb.uchSecPerClust*CurDlg->m_ntfs.bpb.wBytesPerSec;
		originPos.QuadPart+=LI.QuadPart;
		SetFilePointer(choosevolume->hDevice,originPos.LowPart,&originPos.HighPart,FILE_BEGIN);
		{
			unsigned int total=choosevolume->m_TotalSec;
			int curpos;
			MFT_HEAD mfthead;
			NTFS_ATTRIBUTE attribute;
			LARGE_INTEGER result;

			for(unsigned int countl=0;countl<total;countl++)
			{
				if(!CurDlg->m_StopThread) goto ExitThread;
				if(countl&1023)
				{
					CurDlg->m_Progress.SetPos(int((float)countl/total*100));
				}
				ReadFile(choosevolume->hDevice,Buf,2*SIZE,&NumOfRead,NULL);
				{
					curpos=0;
					if(memcmp(Buf+curpos,"FILE",4)) 
						goto start1;
						
					memcpy(&mfthead,Buf+curpos,sizeof(MFT_HEAD));
					if((mfthead.wFlags&1)==0) 
					{
						deleted++;
						char str[20];
						sprintf(str,"已删除的文件数:%d",deleted);
						CurDlg->GetDlgItem(IDC_DELETED)->SetWindowText(str);
					}
					curpos+=mfthead.wAttribOffset;
					
					int count=CurDlg->m_fileNum/*CurDlg->m_filelist.GetItemCount()*/;
					
					for(int offset=0;;curpos+=attribute.dwFullLength)
					{
						//if(curpos>=SIZE) goto start1;//所有属性已结束
						memcpy(&attribute,Buf+curpos,sizeof(NTFS_ATTRIBUTE));
						attribute.dwFullLength&=0x1ff;//防止无效
						switch(attribute.dwType)
						{
							case 0x10://$STANDART_INFORMATION
							case 0x20:
							case 0x40:
							case 0x50:
							case 0x60:
							case 0x70:
								break;
							case 0x30://$FILE_NAME   
								if(attribute.uchNonResFlag) //30H必为常驻属性
									goto start1;//属性错误
								FILE_NAME filename;
								offset=attribute.Attr.Resident.wAttrOffset;//18+2*N
								
								memcpy(&filename,Buf+curpos+offset,sizeof(FILE_NAME));
								if((filename.chFileNameType&1)==0)
									continue;
								if(filename.chFileNameLength==0) 
									goto start1;
								offset+=sizeof(FILE_NAME);

								{
									CurDlg->m_fileNum++;
									wchar_t unicode[256];
									memset(unicode,0,256);
									char* p=NULL;
									memcpy(unicode,Buf+curpos+offset,filename.chFileNameLength*2);
									int nByte=WideCharToMultiByte(CP_ACP,0,unicode,-1,NULL,0,NULL,NULL);
									p=new char[nByte];
									WideCharToMultiByte(CP_ACP,0,unicode,-1,p,nByte,NULL,NULL);
									p[nByte-1]='\0';
									sprintf(dest,"%08d",count);//最大文件数int
									if(filename.dwFlags&0x10000000)
										CurDlg->m_filelist.InsertItem(count,p,1);
									else
										CurDlg->m_filelist.InsertItem(count,p);
	//								CurDlg->m_filelist.EnsureVisible(count,TRUE);//调试用   滚动至最后一行
									CurDlg->m_filelist.SetItemText(count,1,dest);
									DWORD flag=filename.dwFlags;
									strcpy(dest,"");
									while(flag)
									{
										if(flag&FILE_ATTRIBUTE_READONLY)
										{
											strcat(dest,"只读 ");
											flag^=FILE_ATTRIBUTE_READONLY;
											continue;
										}
										if(flag&FILE_ATTRIBUTE_HIDDEN)
										{
											strcat(dest,"隐藏 ");
											flag^=FILE_ATTRIBUTE_HIDDEN;
											continue;
										}
										if(flag&FILE_ATTRIBUTE_SYSTEM)
										{
											strcat(dest,"系统 ");
											flag^=FILE_ATTRIBUTE_SYSTEM;
											continue;
										}
										if(flag&FILE_ATTRIBUTE_DIRECTORY)
										{
											strcat(dest,"目录 ");
											flag^=FILE_ATTRIBUTE_DIRECTORY;
											continue;
										}
										if(flag&FILE_ATTRIBUTE_ARCHIVE)
										{
											strcat(dest,"存档 ");
											flag^=FILE_ATTRIBUTE_ARCHIVE;
											continue;
										}
										if(flag&FILE_ATTRIBUTE_ENCRYPTED)
										{
											strcat(dest,"设备 ");
											flag^=FILE_ATTRIBUTE_ENCRYPTED;
											continue;
										}
										if(flag&FILE_ATTRIBUTE_NORMAL)
										{
											strcat(dest,"常规 ");
											flag^=FILE_ATTRIBUTE_NORMAL;
											continue;
										}
										if(flag&FILE_ATTRIBUTE_TEMPORARY)
										{
											strcat(dest,"临时 ");
											flag^=FILE_ATTRIBUTE_TEMPORARY;
											continue;
										}
										if(flag&FILE_ATTRIBUTE_SPARSE_FILE)
										{
											strcat(dest,"稀疏 ");
											flag^=FILE_ATTRIBUTE_SPARSE_FILE;
											continue;
										}
										if(flag&FILE_ATTRIBUTE_REPARSE_POINT)
										{
											strcat(dest,"重解析 ");
											flag^=FILE_ATTRIBUTE_REPARSE_POINT;
											continue;
										}
										if(flag&FILE_ATTRIBUTE_COMPRESSED)
										{
											strcat(dest,"压缩 ");
											flag^=FILE_ATTRIBUTE_COMPRESSED;
											continue;
										}
										if(flag&FILE_ATTRIBUTE_OFFLINE)
										{
											strcat(dest,"脱机 ");
											flag^=FILE_ATTRIBUTE_OFFLINE;
											continue;
										}
										if(flag&FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)
										{
											strcat(dest,"未编入索引 ");
											flag^=FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;
											continue;
										}
										if(flag&0x4000)
										{
											strcat(dest,"加密 ");
											flag^=0x4000;
											continue;
										}
										if(flag&0x10000000)
										{
											strcat(dest,"目录 ");
											flag^=0x10000000;
											continue;
										}
										if(flag&0x20000000)
										{
											strcat(dest,"索引视图 ");
											flag^=0x20000000;
											continue;
										}
									}
									CurDlg->m_filelist.SetItemText(count,4,dest);
									if((mfthead.wFlags&1)==0) sprintf(dest,"%s","已删除");
									else sprintf(dest,"%s","未删除");
									SYSTEMTIME st;
									CurDlg->m_filelist.SetItemText(count,5,dest);
									FileTimeToSystemTime((FILETIME*)&filename.n64Create,&st);
									sprintf(dest,"%d年%d月%d日 %d:%d:%d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
									CurDlg->m_filelist.SetItemText(count,6,dest);
									FileTimeToSystemTime((FILETIME*)&filename.n64Modify,&st);
									sprintf(dest,"%d年%d月%d日 %d:%d:%d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
									CurDlg->m_filelist.SetItemText(count,7,dest);
									FileTimeToSystemTime((FILETIME*)&filename.n64Modfil,&st);
									sprintf(dest,"%d年%d月%d日 %d:%d:%d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
									CurDlg->m_filelist.SetItemText(count,8,dest);
									FileTimeToSystemTime((FILETIME*)&filename.n64Access,&st);
									sprintf(dest,"%d年%d月%d日 %d:%d:%d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
									CurDlg->m_filelist.SetItemText(count,9,dest);
									sprintf(dest,"%08d",mfthead.dwMFTRecNumber);
									CurDlg->m_filelist.SetItemText(count,10,dest);
									sprintf(dest,"%08d",filename.dwMftParentDir);//20位数  2^64
									CurDlg->m_filelist.SetItemText(count,11,dest);
									delete []p;
									p=NULL;
								}
								break;
							case 0x80:
								{
									if(attribute.dwFullLength==attribute.wNameOffset) 
										goto start1;//数据有误
									if(attribute.uchNonResFlag==0)//若为常驻属性
									{
										sprintf(dest,"H:%u,L:%10u",0,CurDlg->m_ntfs.bpb.uchSecPerClust*CurDlg->m_ntfs.bpb.wBytesPerSec);
										CurDlg->m_filelist.SetItemText(count,2,dest);
										sprintf(dest,"H:%u,L:%10u",0,attribute.Attr.Resident.dwLength);
										CurDlg->m_filelist.SetItemText(count,3,dest);
										result.QuadPart=offsetH.QuadPart+countl*2*SIZE+curpos+attribute.Attr.Resident.wAttrOffset;
										sprintf(dest,"H:%u,L:%10u",result.HighPart,result.LowPart);
										 CurDlg->m_filelist.SetItemText(count,12,dest);
									}
  									else//非常驻属性
  									{
 										result.QuadPart=attribute.Attr.NonResident.n64AllocSize;
 										sprintf(dest,"H:%u,L:%10u",result.HighPart,result.LowPart);
 										CurDlg->m_filelist.SetItemText(count,2,dest);
 										result.QuadPart=attribute.Attr.NonResident.n64RealSize;
 										sprintf(dest,"H:%u,L:%10u",result.HighPart,result.LowPart);
 										CurDlg->m_filelist.SetItemText(count,3,dest);
 										unsigned char* run1=NULL;
										if(attribute.dwFullLength<=sizeof(NTFS_ATTRIBUTE)) 
											goto start1;//无效数据
										int lLength=attribute.dwFullLength-attribute.Attr.NonResident.wDatarunOffset;
 										run1=new unsigned char[lLength];
  										memcpy(run1,Buf+curpos+attribute.Attr.NonResident.wDatarunOffset,lLength);
  										unsigned char p=*run1;
 										LARGE_INTEGER startLCN;
										startLCN.QuadPart=0;
 										memcpy(&startLCN.QuadPart,run1+1+(p&15),p>>4);
										startLCN.QuadPart*=CurDlg->m_ntfs.bpb.uchSecPerClust;
										startLCN.QuadPart+=CurDlg->GlobalOffset;
										startLCN.QuadPart*=CurDlg->m_ntfs.bpb.wBytesPerSec;
 										sprintf(dest,"H:%u,L:%10u",startLCN.HighPart,startLCN.LowPart);
 										CurDlg->m_filelist.SetItemText(count,12,dest);
 										if(*(run1+1+(p&15)+(p>>4))==0) 
										{
											delete []run1;
											goto start1;//只有一个运行
										}
										int offset=0,num=0;
										CString* runs=new CString("");
										LARGE_INTEGER sizelast;
										sizelast.QuadPart=attribute.Attr.NonResident.n64RealSize;
										startLCN.QuadPart=0;
										do
										{
											LARGE_INTEGER size,curLCN;
											strcpy(dest,"");
											size.QuadPart=0;
											curLCN.QuadPart=0;
											if(offset>lLength || (p&15)>5 || (p>>4)>4) 
											{//防止数据有误
												delete []run1;
												goto start1;
											}
											memcpy(&size.QuadPart,run1+1+offset,(p&15));
											size.QuadPart*=CurDlg->m_ntfs.bpb.uchSecPerClust*CurDlg->m_ntfs.bpb.wBytesPerSec;
											
											if(*(run1+offset+1+(p&15)+(p>>4))==0)//如果是最后一个运行
											{
												size.QuadPart=sizelast.QuadPart;
											}
											sizelast.QuadPart-=size.QuadPart;
											if((p>>4)>4)
											{
												CString str;
												str.Format("count=%d,%d",CurDlg->m_filelist.GetItemCount(),p>>4);
												CurDlg->MessageBox(str);
											}
											memcpy(&curLCN.QuadPart,run1+1+(p&15)+offset,p>>4);

											LONGLONG LL=1;
											LL<<=(p>>4)*8;
											if((curLCN.QuadPart&(LL>>1))&&num)
												curLCN.QuadPart-=LL;//负偏移位置
 											curLCN.QuadPart*=CurDlg->m_ntfs.bpb.uchSecPerClust*CurDlg->m_ntfs.bpb.wBytesPerSec;
 											startLCN.QuadPart+=curLCN.QuadPart;
											
 											sprintf(dest,"POS:(H:%u,L:%u) SIZE:(H:%u,L:%u) NUM:%d;",startLCN.HighPart,startLCN.LowPart,
 												size.HighPart,size.LowPart,num+1);//容易溢出，用cstring
											*runs+=dest;

											offset+=1+(p&15)+(p>>4);
											p=*(run1+offset);	
											num++;
										}
										while((p&15)&&(p>>4));
										sprintf(dest,"%d",runs);
										CurDlg->m_filelist.SetItemText(count,13,dest);
										delete []run1;	
									}
									goto start1;
								}
							default:
								goto start1;//本文件查询结束，跳出循环，查询下一个文件
						}
					}
				}
				start1:;
			}
		}
		run+=(p>>4)+(p&15)+1;
		p=*run;
	}
	while((p&15)&&(p>>4));
	
ExitThread:
	delete []ori;
	ori=NULL;
	CurDlg->GetDlgItem(IDC_STATE)->SetWindowText("空闲");
	CurDlg->GetDlgItem(IDC_BUTTON_END)->SetWindowText("开始扫描");
	CurDlg->m_Progress.SetPos(100);
	CurDlg->m_StopThread=TRUE;
	CurDlg->GetALLDirectory();
	return TRUE;
}

DWORD WINAPI CDataRecoveryDlg::SearchFAT32File(LPVOID lpVoid)
{
	//定位根目录位置
	CDataRecoveryDlg* CurDlg=(CDataRecoveryDlg*)lpVoid;
	Choose* choosevolume=CurDlg->m_thisChoose;
	DWORD RootSecNum=CurDlg->m_fat32.bpb.wReservedSec+CurDlg->m_fat32.bpb.dwSecPerFat*CurDlg->m_fat32.bpb.wFATNum+
		(CurDlg->m_fat32.bpb.dwRootClustNum-2)*CurDlg->m_fat32.bpb.uchSecPerClust;
	LARGE_INTEGER RootPos;
	RootPos.QuadPart=RootSecNum+CurDlg->GlobalOffset;
	RootPos.QuadPart*=CurDlg->m_fat32.bpb.wBytesPerSec;
	LARGE_INTEGER FAT_TABLE_POS;
	FAT_TABLE_POS.QuadPart=CurDlg->m_fat32.bpb.wReservedSec+CurDlg->GlobalOffset;
	FAT_TABLE_POS.QuadPart*=CurDlg->m_fat32.bpb.wBytesPerSec;
	int FAT_TABLE_SIZE=CurDlg->m_fat32.bpb.dwSecPerFat*CurDlg->m_fat32.bpb.wBytesPerSec/4;
	DWORD* FAT_TABLE=new DWORD[FAT_TABLE_SIZE];
	SetFilePointer(choosevolume->hDevice,FAT_TABLE_POS.LowPart,&FAT_TABLE_POS.HighPart,FILE_BEGIN);
	DWORD NumOfRead;
	ReadFile(choosevolume->hDevice,FAT_TABLE,FAT_TABLE_SIZE,&NumOfRead,NULL);
	SetFilePointer(choosevolume->hDevice,RootPos.LowPart,&RootPos.HighPart,FILE_BEGIN);

	//计算该目录簇数
	int ClusterNum=0;
	DWORD curcluster=CurDlg->m_fat32.bpb.dwRootClustNum;
	do
	{
		ClusterNum++;
		curcluster=FAT_TABLE[curcluster];
	}
	while(curcluster!=0xFFFFFFF && curcluster!=0xFFFFFF8 && curcluster!=0);
	DWORD size=CurDlg->m_fat32.bpb.uchSecPerClust*CurDlg->m_fat32.bpb.wBytesPerSec;
	char* Buf=new char[ClusterNum*size];
	char* Ptr=Buf;
	curcluster=CurDlg->m_fat32.bpb.dwRootClustNum;
	LARGE_INTEGER curpos;
	curpos.QuadPart=curcluster;
	do
	{
		curpos.QuadPart=CurDlg->GlobalOffset;
		curpos.QuadPart+=CurDlg->m_fat32.bpb.wReservedSec+CurDlg->m_fat32.bpb.dwSecPerFat*CurDlg->m_fat32.bpb.wFATNum+
			(curcluster-2)*CurDlg->m_fat32.bpb.uchSecPerClust;
		curpos.QuadPart*=CurDlg->m_fat32.bpb.wBytesPerSec;
		SetFilePointer(choosevolume->hDevice,curpos.LowPart,&curpos.HighPart,FILE_BEGIN);
		ReadFile(choosevolume->hDevice,Ptr,size,&NumOfRead,NULL);
		curcluster=FAT_TABLE[curcluster];
		Ptr+=size;
	}
	while(curcluster!=0xFFFFFFF && curcluster!=0xFFFFFF8 && curcluster!=0);
	Ptr=Buf;
	FAT32_DIR fat32dir;
	DWORD Maxnum=6;
	while(*Ptr)
	{
		int count=CurDlg->m_fileNum/*CurDlg->m_filelist.GetItemCount()*/;
		CurDlg->m_fileNum++;
		memcpy(&fat32dir,Ptr,sizeof(FAT32_DIR));
		char temp[SIZE];

		if(fat32dir.shortdir.dwFileAttrib==0xF)//长文件名
		{
			int namenum=0;
			if(fat32dir.longdir.dwFlag!=0xE5) namenum=fat32dir.longdir.dwFlag&0x3F;
			else
			{
				int signal=fat32dir.longdir.Serial;
				while(fat32dir.longdir.Serial==signal)
				{
					namenum++;
					memcpy(&fat32dir,Ptr+0x20*namenum,sizeof(FAT32_DIR));
				}
			}
			unsigned short unicode[256];
			memset(unicode,0,512);
			for(int i=0;i<namenum;i++)
			{
				memcpy(&fat32dir,Ptr+0x20*(namenum-i-1),sizeof(FAT32_DIR));
				memcpy(unicode+13*i,fat32dir.longdir.namepart1,10);
				memcpy(unicode+13*i+5,fat32dir.longdir.namepart2,12);
				memcpy(unicode+13*i+11,fat32dir.longdir.namepart3,4);
			}
			int nByte=WideCharToMultiByte(CP_ACP,0,(LPCWSTR)unicode,-1,NULL,0,NULL,NULL);
			WideCharToMultiByte(CP_ACP,0,(LPCWSTR)unicode,-1,temp,nByte,NULL,NULL);
			temp[nByte-1]='\0';
			Ptr+=0x20*namenum;
			memcpy(&fat32dir,Ptr,sizeof(FAT32_DIR));
		}
		else//短文件名
		{
			//拼接文件名
			strcpy(temp,"");
			strncat(temp,fat32dir.shortdir.dwFileName,8);
			//格式化字符串
			temp[8]='\0';
			int i;
			for(i=7;i>=0 && temp[i]==0x20;i--)
			{
				temp[i]=0;
			}
			if(fat32dir.shortdir.dwFileExt[0]!=0x20)
			{
				strcat(temp,".");
				int n=strlen(temp);
				strcat(temp,fat32dir.shortdir.dwFileExt);
				temp[n+3]='\0';
				for(i=n+2;i>=n && temp[i]==0x20;i--)
				{
					temp[i]=0;
				}
			}
		}
		

		BYTE attrib=fat32dir.shortdir.dwFileAttrib;
		if(attrib & 0x10)
			CurDlg->m_filelist.InsertItem(count,temp,1);
		else
			CurDlg->m_filelist.InsertItem(count,temp);
		sprintf(temp,"%08d",count);//最大文件数int
		CurDlg->m_filelist.SetItemText(count,1,temp);
		sprintf(temp,"H:%u,L:%10u",0,fat32dir.shortdir.dwFileSize);
		CurDlg->m_filelist.SetItemText(count,3,temp);
		strcpy(temp,"");
		
		if(attrib & 0x01)
		{
			strcat(temp,"只读 ");
		}
		if(attrib & 0x02)
		{
			strcat(temp,"隐藏 ");
		}
		if(attrib & 0x04)
		{
			strcat(temp,"系统 ");
		}
		if(attrib & 0x08)
		{
			strcat(temp,"卷标 ");
		}
		if(attrib & 0x10)
		{
			strcat(temp,"目录 ");
		}
		if(attrib & 0x20)
		{
			strcat(temp,"归档 ");
		}
		CurDlg->m_filelist.SetItemText(count,4,temp);
		
		if(fat32dir.longdir.dwFlag==0xE5) sprintf(temp,"%s","已删除");
		else sprintf(temp,"%s","未删除");
		CurDlg->m_filelist.SetItemText(count,5,temp);
		BYTE HL[4];
		memcpy(HL,&fat32dir.shortdir.dwCreateDate,2);
		memcpy(HL+2,&fat32dir.shortdir.dwCreateTime,2);
		sprintf(temp,"%d年%d月%d日 %d:%d:%d",1980+(HL[1]>>1),((HL[1]&1)<<3)+(HL[0]>>5),HL[0]&31,HL[3]>>3,
			((HL[3]&7)<<3)+(HL[2]>>5),HL[2]&31);
		CurDlg->m_filelist.SetItemText(count,6,temp);
		memcpy(HL,&fat32dir.shortdir.dwSysLastModifyDate,2);
		memcpy(HL+2,&fat32dir.shortdir.dwSysLastModifyTime,2);
		sprintf(temp,"%d年%d月%d日 %d:%d:%d",1980+(HL[1]>>1),((HL[1]&1)<<3)+(HL[0]>>5),HL[0]&31,HL[3]>>3,
			((HL[3]&7)<<3)+(HL[2]>>5),HL[2]&31);
		CurDlg->m_filelist.SetItemText(count,7,temp);
		CurDlg->m_filelist.SetItemText(count,8,temp);
		memcpy(HL,&fat32dir.shortdir.dwLastAccessDate,2);
		sprintf(temp,"%d年%d月%d日",1980+(HL[1]>>1),((HL[1]&1)<<3)+(HL[0]>>5),HL[0]&31);
		CurDlg->m_filelist.SetItemText(count,9,temp);
		sprintf(temp,"%08d",Maxnum);
		CurDlg->m_filelist.SetItemText(count,10,temp);
		sprintf(temp,"%08d",5);
		CurDlg->m_filelist.SetItemText(count,11,temp);
		//判断是否为单运行

		DWORD filepos=fat32dir.shortdir.dwHigh16;
		filepos=(filepos<<16)+fat32dir.shortdir.dwLow16;
		LARGE_INTEGER filepos1;

		filepos1.QuadPart=CurDlg->m_fat32.bpb.wReservedSec+CurDlg->m_fat32.bpb.dwSecPerFat*CurDlg->m_fat32.bpb.wFATNum+
			(filepos-2)*CurDlg->m_fat32.bpb.uchSecPerClust;
		filepos1.QuadPart+=CurDlg->GlobalOffset;
		filepos1.QuadPart*=CurDlg->m_fat32.bpb.wBytesPerSec;
		sprintf(temp,"H:%u,L:%10u",filepos1.HighPart,filepos1.LowPart);
		CurDlg->m_filelist.SetItemText(count,12,temp);
		if(FAT_TABLE[filepos]!=0xFFFFFFF && FAT_TABLE[filepos]!=0xFFFFFF8 && FAT_TABLE[filepos]!=0)
		{		
			DWORD filesize=fat32dir.shortdir.dwFileSize;	
			filesize=fat32dir.shortdir.dwFileSize/(CurDlg->m_fat32.bpb.wBytesPerSec*CurDlg->m_fat32.bpb.uchSecPerClust);
			filesize=(filesize+1)*CurDlg->m_fat32.bpb.wBytesPerSec*CurDlg->m_fat32.bpb.uchSecPerClust;
			sprintf(temp,"H:%u,L:%10u",0,filesize);
			CurDlg->m_filelist.SetItemText(count,2,temp);
		}
		else
		{
			sprintf(temp,"H:%u,L:%10u",0,CurDlg->m_fat32.bpb.uchSecPerClust*CurDlg->m_fat32.bpb.wBytesPerSec);
			CurDlg->m_filelist.SetItemText(count,2,temp);
		}
		Ptr+=0x20;

		//若有子目录。。。。。
		if(attrib & 0x10)
		{
			CurDlg->SearchFAT32ChildDir(filepos,Maxnum,FAT_TABLE);
		}
		if(!CurDlg->m_StopThread) break;
		Maxnum++;
	}
	delete []Buf;
	delete []FAT_TABLE;
	CurDlg->GetDlgItem(IDC_STATE)->SetWindowText("空闲");
	CurDlg->GetDlgItem(IDC_BUTTON_END)->SetWindowText("开始扫描");
	CurDlg->m_Progress.SetPos(100);
	CurDlg->m_StopThread=TRUE;
	CurDlg->GetALLDirectory();
	return TRUE;
}

void CDataRecoveryDlg::SearchFAT32ChildDir(DWORD ClustNum,DWORD& MaxUsedNum,DWORD* FATTABLE)
{//ClustNum:目录的第一个簇
	DWORD father=MaxUsedNum;
	DWORD DirSecNum=m_fat32.bpb.wReservedSec+m_fat32.bpb.dwSecPerFat*m_fat32.bpb.wFATNum+(ClustNum-2)*m_fat32.bpb.uchSecPerClust;
	LARGE_INTEGER DirPos;
	DirPos.QuadPart=DirSecNum+GlobalOffset;
	DirPos.QuadPart*=m_fat32.bpb.wBytesPerSec;
	SetFilePointer(m_thisChoose->hDevice,DirPos.LowPart,&DirPos.HighPart,FILE_BEGIN);
	//计算该目录簇数
	int ClusterNum=0;
	DWORD curcluster=ClustNum;
	do
	{
		ClusterNum++;
		curcluster=FATTABLE[curcluster];
	}
	while(curcluster!=0xFFFFFFF && curcluster!=0xFFFFFF8 && curcluster!=0);
	DWORD size=m_fat32.bpb.uchSecPerClust*m_fat32.bpb.wBytesPerSec;
	char* Buf=new char[ClusterNum*size];
	char* Ptr=Buf;
	curcluster=ClustNum;
	LARGE_INTEGER curpos;
	curpos.QuadPart=curcluster;
	DWORD NumOfRead;
	do
	{
		curpos.QuadPart=GlobalOffset;
		curpos.QuadPart+=m_fat32.bpb.wReservedSec+m_fat32.bpb.dwSecPerFat*m_fat32.bpb.wFATNum+
			(curcluster-2)*m_fat32.bpb.uchSecPerClust;
		curpos.QuadPart*=m_fat32.bpb.wBytesPerSec;
		SetFilePointer(m_thisChoose->hDevice,curpos.LowPart,&curpos.HighPart,FILE_BEGIN);
		ReadFile(m_thisChoose->hDevice,Ptr,size,&NumOfRead,NULL);
		curcluster=FATTABLE[curcluster];
		Ptr+=size;
	}
	while(curcluster!=0xFFFFFFF && curcluster!=0xFFFFFF8 && curcluster!=0);
	Ptr=Buf+2*0x20;//越过子目录的.和..
	FAT32_DIR fat32dir;

	while(*Ptr)
	{
		int count=m_fileNum/*CurDlg->m_filelist.GetItemCount()*/;
		m_fileNum++;
		memcpy(&fat32dir,Ptr,sizeof(FAT32_DIR));
		char temp[SIZE];
		if(fat32dir.shortdir.dwFileAttrib==0xF)//长文件名
		{
			int namenum=0;
			if(fat32dir.longdir.dwFlag!=0xE5) namenum=fat32dir.longdir.dwFlag&0x3F;
			else
			{
				int signal=fat32dir.longdir.Serial;
				while(fat32dir.longdir.Serial==signal)
				{
					namenum++;
					memcpy(&fat32dir,Ptr+0x20*namenum,sizeof(FAT32_DIR));
				}
			}
			unsigned short unicode[256];
			memset(unicode,0,512);
			for(int i=0;i<namenum;i++)
			{
				memcpy(&fat32dir,Ptr+0x20*(namenum-i-1),sizeof(FAT32_DIR));
				memcpy(unicode+13*i,fat32dir.longdir.namepart1,10);
				memcpy(unicode+13*i+5,fat32dir.longdir.namepart2,12);
				memcpy(unicode+13*i+11,fat32dir.longdir.namepart3,4);
			}
			int nByte=WideCharToMultiByte(CP_ACP,0,(LPCWSTR)unicode,-1,NULL,0,NULL,NULL);
			WideCharToMultiByte(CP_ACP,0,(LPCWSTR)unicode,-1,temp,nByte,NULL,NULL);
			temp[nByte-1]='\0';
			Ptr+=0x20*namenum;
			memcpy(&fat32dir,Ptr,sizeof(FAT32_DIR));
		}
		else//短文件名
		{
			//拼接文件名
			strcpy(temp,"");
			strncat(temp,fat32dir.shortdir.dwFileName,8);
			//格式化字符串
			temp[8]='\0';
			int i;
			for(i=7;i>=0 && temp[i]==0x20;i--)
			{
				temp[i]=0;
			}
			if(fat32dir.shortdir.dwFileExt[0]!=0x20)
			{
				strcat(temp,".");
				int n=strlen(temp);
				strcat(temp,fat32dir.shortdir.dwFileExt);
				temp[n+3]='\0';
				for(i=n+3;i>=n && temp[i]==0x20;i--)
				{
					temp[i]=0;
				}
			}
		}

		BYTE attrib=fat32dir.shortdir.dwFileAttrib;
		if(attrib & 0x10)
			m_filelist.InsertItem(count,temp,1);
		else
			m_filelist.InsertItem(count,temp);
		sprintf(temp,"%08d",count);//最大文件数int
		m_filelist.SetItemText(count,1,temp);

		sprintf(temp,"H:%u,L:%10u",0,fat32dir.shortdir.dwFileSize);
		m_filelist.SetItemText(count,3,temp);
		strcpy(temp,"");

		if(attrib & 0x01)
		{
			strcat(temp,"只读 ");
		}
		if(attrib & 0x02)
		{
			strcat(temp,"隐藏 ");
		}
		if(attrib & 0x04)
		{
			strcat(temp,"系统 ");
		}
		if(attrib & 0x08)
		{
			strcat(temp,"卷标 ");
		}
		if(attrib & 0x10)
		{
			strcat(temp,"子目录 ");
		}
		if(attrib & 0x20)
		{
			strcat(temp,"归档 ");
		}
		m_filelist.SetItemText(count,4,temp);
		
		if(fat32dir.longdir.dwFlag==0xE5) sprintf(temp,"%s","已删除");
		else sprintf(temp,"%s","未删除");
		m_filelist.SetItemText(count,5,temp);
		BYTE HL[4];
		memcpy(HL,&fat32dir.shortdir.dwCreateDate,2);
		memcpy(HL+2,&fat32dir.shortdir.dwCreateTime,2);
		sprintf(temp,"%d年%d月%d日 %d:%d:%d",1980+(HL[1]>>1),((HL[1]&1)<<3)+(HL[0]>>5),HL[0]&31,HL[3]>>3,
			((HL[3]&7)<<3)+(HL[2]>>5),HL[2]&31);
		m_filelist.SetItemText(count,6,temp);
		memcpy(HL,&fat32dir.shortdir.dwSysLastModifyDate,2);
		memcpy(HL+2,&fat32dir.shortdir.dwSysLastModifyTime,2);
		sprintf(temp,"%d年%d月%d日 %d:%d:%d",1980+(HL[1]>>1),((HL[1]&1)<<3)+(HL[0]>>5),HL[0]&31,HL[3]>>3,
			((HL[3]&7)<<3)+(HL[2]>>5),HL[2]&31);
		m_filelist.SetItemText(count,7,temp);
		m_filelist.SetItemText(count,8,temp);
		memcpy(HL,&fat32dir.shortdir.dwLastAccessDate,2);
		sprintf(temp,"%d年%d月%d日",1980+(HL[1]>>1),((HL[1]&1)<<3)+(HL[0]>>5),HL[0]&31);
		m_filelist.SetItemText(count,9,temp);
		sprintf(temp,"%08d",++MaxUsedNum);
		m_filelist.SetItemText(count,10,temp);
		sprintf(temp,"%08d",father);
		m_filelist.SetItemText(count,11,temp);
		//判断是否为单运行
		DWORD filepos=fat32dir.shortdir.dwHigh16;
		filepos=(filepos<<16)+fat32dir.shortdir.dwLow16;
		
		LARGE_INTEGER filepos1;
		filepos1.QuadPart=m_fat32.bpb.wReservedSec+m_fat32.bpb.dwSecPerFat*m_fat32.bpb.wFATNum+(filepos-2)*m_fat32.bpb.uchSecPerClust;
		filepos1.QuadPart+=GlobalOffset;
		filepos1.QuadPart*=m_fat32.bpb.wBytesPerSec;
		sprintf(temp,"H:%u,L:%10u",filepos1.HighPart,filepos1.LowPart);
		m_filelist.SetItemText(count,12,temp);
		if(FATTABLE[filepos]!=0xFFFFFFF && FATTABLE[filepos]!=0xFFFFFF8 && FATTABLE[filepos]!=0)
		{		
			DWORD filesize=fat32dir.shortdir.dwFileSize;
			filesize=fat32dir.shortdir.dwFileSize/(m_fat32.bpb.wBytesPerSec*m_fat32.bpb.uchSecPerClust);
			filesize=(filesize+1)*m_fat32.bpb.wBytesPerSec*m_fat32.bpb.uchSecPerClust;
			sprintf(temp,"H:%u,L:%10u",0,filesize);
			m_filelist.SetItemText(count,2,temp);
		}
		else
		{
			sprintf(temp,"H:%u,L:%10u",0,m_fat32.bpb.uchSecPerClust*m_fat32.bpb.wBytesPerSec);
			m_filelist.SetItemText(count,2,temp);
		}
		Ptr+=0x20;

		//若有子目录。。。。。
		if(attrib & 0x10)
		{
			SearchFAT32ChildDir(filepos,MaxUsedNum,FATTABLE);
		}
		if(!m_StopThread) break;
	}
	delete []Buf;

}

void CDataRecoveryDlg::OnButtonEnd() 
{
	// TODO: Add your control notification handler code here
	DWORD dwCode;
	CString str;
	GetDlgItemText(IDC_BUTTON_END,str);
	if(str=="开始扫描") OnOpen();
	else if(str=="停止扫描")
	{
		if(GetExitCodeThread(m_SearchFile,&dwCode))
		{
			if(dwCode==STILL_ACTIVE)
			{
				SetDlgItemText(IDC_BUTTON_END,"正在停止...");
				GetDlgItem(IDC_BUTTON_END)->EnableWindow(false);
				m_StopThread=FALSE;
				return;
			}
		}
	}
}

void CDataRecoveryDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	if(m_thisChoose) 
	{
		if(m_thisChoose->hDevice!=INVALID_HANDLE_VALUE) 
			CloseHandle(m_thisChoose->hDevice);
		delete m_thisChoose;
	}
	if(m_files)
	{
		delete []m_files;
	}
	if(m_root) 
	{
		delete m_root;
	}
	if(m_SearchFile) CloseHandle(m_SearchFile);
	if(m_pImageList) delete m_pImageList;
	if(m_SearchDisk && m_SearchDisk!=INVALID_HANDLE_VALUE) CloseHandle(m_SearchDisk);
	TableList* cur=TL;
	while(TL)
	{
		cur=TL->Next;
		delete TL;
		TL=cur;
	}
	// TODO: Add your message handler code here
	
}

void CDataRecoveryDlg::OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(m_isDirok==FALSE) return;
	// TODO: Add your control notification handler code here
	if(MB_OKCANCEL!=MessageBox("此操作需要很长时间，继续?",NULL,MB_OKCANCEL)) return;
	CompareData CD;
	CD.isub=pNMListView->iSubItem;
	CD.listctrl=&m_filelist;
	if(m_filelist.GetItemText(0,pNMListView->iSubItem)>m_filelist.GetItemText(1,pNMListView->iSubItem))
		m_filelist.SortItems(MyCompareLtoH,(DWORD)&CD);
	else m_filelist.SortItems(MyCompareHtoL,(DWORD)&CD);
	
	*pResult = 0;
}

void CDataRecoveryDlg::OnRclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CMenu curMenu;
	curMenu.LoadMenu(IDR_MAINMENU);	
	CMenu* pPopup=curMenu.GetSubMenu(0);
	DWORD dwPos=GetMessagePos();
	CPoint cPt;
	cPt.x=(int)LOWORD(dwPos);
	cPt.y=(int)HIWORD(dwPos);
	pPopup->TrackPopupMenu(TPM_LEFTBUTTON,cPt.x,cPt.y,this,NULL);
	pNMHDR=pNMHDR;
	*pResult = 0;
}

void CDataRecoveryDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	OnRecovery();
	*pResult = 0;
	pNMHDR=pNMHDR;
}

void CDataRecoveryDlg::OnRecovery() 
{
	// TODO: Add your command handler code here
	POSITION pos=m_filelist.GetFirstSelectedItemPosition();
	if(!pos) 
	{
		MessageBox("未选中任何文件!");
		return;
	}

	ASSERT(m_thisChoose->hDevice!=INVALID_HANDLE_VALUE);
	EnableWindow(FALSE);

	char szPath[SIZE];
	GetCurrentDirectory(SIZE,szPath);
	if(!GetDirectory(m_hWnd, szPath,szPath)) 
	{
		EnableWindow(TRUE);
		return;
	}
	int count=m_filelist.GetNextSelectedItem(pos);

	HTREEITEM HI=((FILESTRUCT*)m_filelist.GetItemData(count))->treeitem;
	if(HI==NULL) 
	{
		MessageBox("暂时无法恢复此文件，请待扫描完再试");
		return;
	}
	if(MB_OKCANCEL!=MessageBox("此操作会花费一定时间，继续?",NULL,MB_OKCANCEL)) return;
	//判断是否为目录
	CString attributes=m_filelist.GetItemText(count,4);
	if(-1!=attributes.Find("目录")) 
	{
		RecoverFolder(szPath,HI);
		EnableWindow(TRUE);
		MessageBox("恢复成功!");

		return;
	}
	else RecoverRuns(HI,szPath);
	EnableWindow(TRUE);
	MessageBox("恢复成功!");
}

void CDataRecoveryDlg::RecoverRuns(HTREEITEM hi,CString path)
{
	RecoverData RD;
	DWORD count=m_directory.GetItemData(hi);
	CString attributes=m_filelist.GetItemText(count,4);
	RD.path=path;
	if(-1!=attributes.Find("目录"))//子节点为目录
	{
		HTREEITEM sub=((FILESTRUCT*)m_filelist.GetItemData(count))->treeitem;
		RecoverFolder(RD.path,sub);
		return;
	}
	//子节点为单个文件
	CFile* curfile;
	RD.path=path+"\\"+m_filelist.GetItemText(count,0);
	TRY 
	{
		curfile=new CFile(RD.path,CFile::modeCreate | CFile::modeReadWrite);
		ASSERT(curfile->m_hFile!=CFile::hFileNull);
		//modeWrite必须有   read用来filemap
	}
	CATCH (CFileException, e)
	{
		switch(e->m_cause)
		{
			case CFileException::badPath:
				MessageBox("路径错误!");
				break;
			case CFileException::accessDenied:
				MessageBox("拒绝访问!");
				break;
			case CFileException::directoryFull:
				MessageBox("目录已满!");
				break;
			case CFileException::hardIO:
				MessageBox("硬件IO错误!");
				break;
			case CFileException::sharingViolation:
				MessageBox("共享违例!");
				break;
			case CFileException::diskFull:
				MessageBox("磁盘已满!");
				break;
			default:
				MessageBox("未知错误!");
				break;
		}
		return;
	}
	END_CATCH
		
	if(m_filelist.GetItemText(count,13)=="")
	{//单运行文件
		RD.FileBeginPos.QuadPart=0;
		RD.FileSize.QuadPart=0;
		RD.FileAllocSize.QuadPart=0;
		sscanf(m_filelist.GetItemText(count,12),"H:%u,L:%u",&RD.FileBeginPos.HighPart,&RD.FileBeginPos.LowPart);//获取文件位置
		sscanf(m_filelist.GetItemText(count,3),"H:%u,L:%u",&RD.FileSize.HighPart,&RD.FileSize.LowPart);//获取文件大小
		sscanf(m_filelist.GetItemText(count,2),"H:%u,L:%u",&RD.FileAllocSize.HighPart,&RD.FileAllocSize.LowPart);//获取文件分配大小
		RD.CurDlg=this;
		RD.curfile=curfile;
		HANDLE m_RecoverFile=CreateThread(NULL,0,RecoverFile,&RD,0,0);
		WaitForSingleObject(m_RecoverFile,INFINITE);
		CloseHandle(m_RecoverFile);
	}
	else
	{//多运行文件
		int Runnum,RunNum;
		sscanf(m_filelist.GetItemText(count,13),"%d",&Runnum);
		CString* RunInfo=(CString*)Runnum;
		sscanf(RunInfo->Right(RunInfo->GetLength()-RunInfo->ReverseFind(':')),":%d;",&RunNum);
		WORD BytesPerSec=m_isNTFS?m_ntfs.bpb.wBytesPerSec:m_fat32.bpb.wBytesPerSec;
		for(int i=0;i<RunNum;i++)
		{
			RD.FileBeginPos.QuadPart=0;
			RD.FileSize.QuadPart=0;
			RD.FileAllocSize.QuadPart=0;
			sscanf(*RunInfo,"POS:(H:%u,L:%u) SIZE:(H:%u,L:%u) %*s;",&RD.FileBeginPos.HighPart,&RD.FileBeginPos.LowPart,
				&RD.FileSize.HighPart,&RD.FileSize.LowPart);//获取文件位置和大小
			RD.FileAllocSize.HighPart=RD.FileSize.HighPart;
			int delta=RD.FileSize.LowPart%BytesPerSec;
			if(delta)
				RD.FileAllocSize.LowPart=RD.FileSize.LowPart-delta+BytesPerSec;
			else RD.FileAllocSize.LowPart=RD.FileSize.LowPart;
			RD.CurDlg=this;
			RD.curfile=curfile;
			HANDLE m_RecoverFile=CreateThread(NULL,0,RecoverFile,&RD,0,0);
			WaitForSingleObject(m_RecoverFile,INFINITE);
			CloseHandle(m_RecoverFile);
			*RunInfo=RunInfo->Right(RunInfo->GetLength()-RunInfo->Find(';')-1);
		}
	}
	if(RD.curfile) RD.curfile->Close();
}

void CDataRecoveryDlg::RecoverFolder(CString path,HTREEITEM HI)
{
	DWORD count=m_directory.GetItemData(HI);
	path=path+"\\"+m_filelist.GetItemText(count,0);
	int result=CreateDirectory(path+"\\",NULL);
	int debug=GetLastError();

	if(result==0 && debug!=183) 
	{
		MessageBox("恢复失败：未知错误");
		return;
	}
	if(m_directory.ItemHasChildren(HI)==FALSE) return;
	//有子节点
	HTREEITEM hNextItem=m_directory.GetChildItem(HI);
	RecoverRuns(hNextItem,path);
	
	hNextItem=m_directory.GetNextItem(hNextItem,TVGN_NEXT);
	while(hNextItem)
	{
		RecoverRuns(hNextItem,path);
		hNextItem=m_directory.GetNextItem(hNextItem,TVGN_NEXT);
	}
}

void CDataRecoveryDlg::OnFind() 
{
	// TODO: Add your command handler code here
	if(m_IsFindFirst)
	{
		CFindDlg find;
		if(IDOK!=find.DoModal() || find.m_Str=="") return;
		m_FindStr=find.m_Str;
		m_FindStr.MakeLower();
	}
	int num=m_filelist.GetItemCount();
	int cursel=0;
	POSITION pos=m_filelist.GetFirstSelectedItemPosition();
	if(pos)
	{
		cursel=m_filelist.GetNextSelectedItem(pos)+1;
	}
	for(int i=cursel;i<num;i++)
	{
		CString str=m_filelist.GetItemText(i,0);
		str.MakeLower();
		if(-1!=str.Find(m_FindStr))
		{
			m_filelist.EnsureVisible(i,TRUE);
			m_filelist.SetItemState(i,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
			if(cursel) m_filelist.SetItemState(cursel-1,0,LVIS_SELECTED|LVIS_FOCUSED);
			break;
		}
	}
}

void CDataRecoveryDlg::OnQuit() 
{
	// TODO: Add your command handler code here
	SendMessage(WM_CLOSE,0,0);
}

DWORD WINAPI CDataRecoveryDlg::RecoverFile(LPVOID lpVoid)
{//处理单运行单文件
	CDataRecoveryDlg* CurDlg=((RecoverData*)lpVoid)->CurDlg;
	LARGE_INTEGER FileAllocSize=((RecoverData*)lpVoid)->FileAllocSize;
	LARGE_INTEGER FileBeginPos=((RecoverData*)lpVoid)->FileBeginPos;
	LARGE_INTEGER FileSize=((RecoverData*)lpVoid)->FileSize;
	CFile* curfile=((RecoverData*)lpVoid)->curfile;
	CString path=((RecoverData*)lpVoid)->path;

	DWORD dwBlockBytes=CurDlg->m_dwBlockBytes;
	//处理文件夹
	
	if(FileAllocSize.HighPart || FileAllocSize.LowPart>dwBlockBytes) //60M以上的文件
	{
		SetFilePointer(CurDlg->m_thisChoose->hDevice,FileBeginPos.LowPart,&FileBeginPos.HighPart,FILE_BEGIN);
		LONGLONG qwFileOffset=0;//偏移地址
		HANDLE hFileMap=CreateFileMapping((HANDLE)curfile->m_hFile,NULL,PAGE_READWRITE,FileSize.HighPart,FileSize.LowPart,NULL);
		if(hFileMap==NULL && (GetLastError())==ERROR_DISK_FULL)
		{
			CurDlg->MessageBox("磁盘已满!");
			CloseHandle(hFileMap);
			return FALSE;
		}
		ASSERT(hFileMap!=NULL);
		DWORD times=unsigned long(FileSize.QuadPart/dwBlockBytes);
		LPBYTE lpbMapAddress;
		
		for(DWORD i=0;i<times;i++)
		{
			lpbMapAddress=(LPBYTE)MapViewOfFile(hFileMap,FILE_MAP_ALL_ACCESS,(DWORD)(qwFileOffset>>32),
				(DWORD)(qwFileOffset&0xFFFFFFFF),dwBlockBytes);
			ASSERT(lpbMapAddress!=NULL);
			{
				char* buf=new char[dwBlockBytes];
				memset(buf,0,dwBlockBytes);
				DWORD num;
				ReadFile(CurDlg->m_thisChoose->hDevice,buf,dwBlockBytes,&num,NULL);
				curfile->Write(buf,dwBlockBytes);
				delete []buf;
			}
			UnmapViewOfFile(lpbMapAddress);
			qwFileOffset+=dwBlockBytes;
		}
		lpbMapAddress=(LPBYTE)MapViewOfFile(hFileMap,FILE_MAP_ALL_ACCESS,(DWORD)(qwFileOffset>>32),
			(DWORD)(qwFileOffset&0xFFFFFFFF),unsigned long(FileSize.QuadPart-times*dwBlockBytes));
		ASSERT(lpbMapAddress!=NULL);
		{
			char* buf=new char[dwBlockBytes];
			memset(buf,0,dwBlockBytes);
			DWORD num;
			ReadFile(CurDlg->m_thisChoose->hDevice,buf,dwBlockBytes,&num,NULL);
			curfile->Write(buf,unsigned int(FileSize.QuadPart-times*dwBlockBytes));
			delete []buf;
		}
		UnmapViewOfFile(lpbMapAddress);
		CloseHandle(hFileMap);
	}
	else 
	{//处理小文件
		WORD BytesPerSec=CurDlg->m_isNTFS?CurDlg->m_ntfs.bpb.wBytesPerSec:CurDlg->m_fat32.bpb.wBytesPerSec;
		DWORD offsetY=FileBeginPos.LowPart%BytesPerSec;
		char* buf;
		if(offsetY==0)
		{
			buf=new char[FileAllocSize.LowPart];
			memset(buf,0,FileAllocSize.LowPart);
			DWORD readnum;
			
			SetFilePointer(CurDlg->m_thisChoose->hDevice,FileBeginPos.LowPart,&FileBeginPos.HighPart,FILE_BEGIN);
			ReadFile(CurDlg->m_thisChoose->hDevice,buf,FileAllocSize.LowPart,&readnum,NULL);
			//读取buf大小必须是扇区大小的倍数!!!!
			curfile->Write(buf,FileSize.LowPart);
		}
		else
		{
			buf=new char[FileAllocSize.LowPart];
			memset(buf,0,FileAllocSize.LowPart);
			DWORD readnum;
			
			int ii=SetFilePointer(CurDlg->m_thisChoose->hDevice,FileBeginPos.LowPart-offsetY,&FileBeginPos.HighPart,FILE_BEGIN);
			//SetFilePointer参数必须设置为扇区倍数
			ii=GetLastError();
			ReadFile(CurDlg->m_thisChoose->hDevice,buf,FileAllocSize.LowPart,&readnum,NULL);
			//读取buf大小必须是扇区大小的倍数!!!!
			curfile->Write(buf+offsetY,FileSize.LowPart);
		}
		delete []buf;
	}

	return TRUE;
}

void CDataRecoveryDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	if(!initialed) return;
	RECT rtfile,rttree,rt,rtwin,rtsplit;
	GetWindowRect(&rtwin);
	m_directory.GetWindowRect(&rttree);
	GetDlgItem(IDC_BUTTON_END)->GetWindowRect(&rt);
	int height=rt.bottom-rt.top;
	rttree.bottom=rtwin.bottom-height-10;
	rttree.right=rtwin.left+int(0.25*cx);
	ScreenToClient(&rttree);
	m_directory.MoveWindow(&rttree,TRUE);
	
	m_Splitter.GetWindowRect(&rtsplit);
	rtsplit.left=rtwin.left+int(0.25*cx)+5;
	rtsplit.right=rtsplit.left+20;
	rtsplit.bottom=rtwin.bottom-height-10;
	ScreenToClient(&rtsplit);
	m_Splitter.MoveWindow(&rtsplit);

	m_filelist.GetWindowRect(&rtfile);
	rtfile.left=rtwin.left+int(0.25*cx)+30;
	rtfile.right=rtwin.right-10;
	rtfile.bottom=rtwin.bottom-height-10;
	ScreenToClient(&rtfile);
	m_filelist.MoveWindow(&rtfile);

	rt.top=rtwin.bottom-height-10;
	rt.bottom=rtwin.bottom-10;
	ScreenToClient(&rt);
	GetDlgItem(IDC_BUTTON_END)->MoveWindow(&rt);

	GetDlgItem(IDC_STATE)->GetWindowRect(&rt);
	rt.top=rtwin.bottom-height-10;
	rt.bottom=rtwin.bottom-10;
	ScreenToClient(&rt);
	GetDlgItem(IDC_STATE)->MoveWindow(&rt);

	GetDlgItem(IDC_PROGRESS)->GetWindowRect(&rt);
	rt.top=rtwin.bottom-height-10;
	rt.bottom=rtwin.bottom-10;
	rt.right=rtwin.right-80;
	ScreenToClient(&rt);
	GetDlgItem(IDC_PROGRESS)->MoveWindow(&rt);

	GetDlgItem(IDC_DELETED)->GetWindowRect(&rt);
	rt.left=rtwin.right-80;
	rt.right=rtwin.right-10;
	rt.top=rtwin.bottom-height-10;
	rt.bottom=rtwin.bottom-10;
	ScreenToClient(&rt);
	GetDlgItem(IDC_DELETED)->MoveWindow(&rt);

	GetDlgItem(IDC_LIST)->Invalidate();
	// TODO: Add your message handler code here
}

void CDataRecoveryDlg::GetALLDirectory()
{
	int TotalNum=m_filelist.GetItemCount();
	m_files=new FILESTRUCT[TotalNum];
	if(m_root) delete m_root;
	m_root=new DirTree(TotalNum);
	GetDlgItem(IDC_BUTTON_END)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATE)->SetWindowText("正在重建目录树...");
	m_Progress.SetPos(0);
	for(int i=0;i<TotalNum;i++)
	{
		m_files[i].name=m_filelist.GetItemText(i,0);
		sscanf(m_filelist.GetItemText(i,10),"%d",&m_files[i].MFT);
		sscanf(m_filelist.GetItemText(i,11),"%d",&m_files[i].FATHERMFT);
		CString name=m_filelist.GetItemText(i,0);		
		m_filelist.SetItemData(i,(LPARAM)(m_files+i));
		if(m_files[i].MFT==5) continue;
		m_root->AddFileToTree(m_files[i].MFT,m_files[i].FATHERMFT,i,name);
		for(int j=0;j<13;j++)
		{
			m_files[i].DATA[j]=m_filelist.GetItemText(i,j);
		}
		m_Progress.SetPos(int(100*(float)i/TotalNum));
	}
	//目录树数据结构加入到CTreeCtrl
	m_root->AddToCtrl(&m_directory,&m_filelist);
	m_Progress.SetPos(100);
	GetDlgItem(IDC_BUTTON_END)->EnableWindow(TRUE);
	GetDlgItem(IDC_STATE)->SetWindowText("空闲");
	m_isDirok=TRUE;
}


void CDataRecoveryDlg::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	DWORD serialNum=m_directory.GetItemData(m_directory.GetSelectedItem());
	CString str="";
	str.Format("%08d",serialNum);
	int count=m_filelist.GetItemCount();
	for(int i=0;i<count;i++)
	{
		if(str==m_filelist.GetItemText(i,1))
		{
			m_filelist.EnsureVisible(i,TRUE);
			m_filelist.SetItemState(i,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
			break;
		}
	}
	*pResult = 0;
	return;
	pNMTreeView=pNMTreeView;
}

void CDataRecoveryDlg::OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	if(pLVKeyDow->wVKey!=VK_F3) return;
	BOOL is=FALSE;
	if(m_FindStr!="") is=TRUE;
	if(is) m_IsFindFirst=FALSE;
	OnFind();
	if(is) m_IsFindFirst=TRUE;
	*pResult = 0;
}

void CDataRecoveryDlg::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(m_isDirok==FALSE) return;
	POSITION pos=m_filelist.GetFirstSelectedItemPosition();
	if(!pos) return;
	int cur=m_filelist.GetNextSelectedItem(pos);
	FILESTRUCT* PFS=(FILESTRUCT*)m_filelist.GetItemData(cur);
	if(m_last)
	{
		m_directory.SetItemState(m_last,0,LVIS_SELECTED|LVIS_FOCUSED);
		m_directory.SetCheck(m_last,FALSE);
	}
	m_directory.EnsureVisible(PFS->treeitem);
	m_directory.SetItemState(PFS->treeitem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
	m_directory.SetCheck(PFS->treeitem);
	m_last=PFS->treeitem;
	*pResult = 0;
	return;
	pNMHDR=pNMHDR;
}

BOOL CDataRecoveryDlg::CheckPartition(DWORD RelativeSectors,BYTE PartitionID)
{
	HANDLE hdevice=CreateFile("\\\\.\\PhysicalDrive0",GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
	LARGE_INTEGER LI;
	LI.QuadPart=RelativeSectors;
	LI.QuadPart*=512;
	SetFilePointer(hdevice,LI.LowPart,&LI.HighPart,FILE_BEGIN);
	char Buf[512];
	DWORD NumofRead;
	ReadFile(hdevice,Buf,512,&NumofRead,NULL);
	BOOL isValid=TRUE;
	switch(PartitionID)
	{
		case 0x01:
		case 0x11://FAT12
			if(memcmp(Buf+0x36,"FAT",3) && memcmp(Buf+0x36,"FAT12",5)) isValid=FALSE;
			break;
		case 0x04:
		case 0x06:
		case 0x0E:
		case 0x14:
		case 0x1E://FAT16
			if(memcmp(Buf+0x36,"FAT16",5)) isValid=FALSE;
			break;
		case 0x07:
		case 0x17://NTFS
			if(memcmp(Buf+0x03,"NTFS",4)) isValid=FALSE;
			break;
		case 0x0B:
		case 0x0C:
		case 0x16:
		case 0x1B:
		case 0x1C://FAT32
			if(memcmp(Buf+0x52,"FAT32",5)) isValid=FALSE;
			break;
		default:
			break;
	}
	CloseHandle(hdevice);
	return isValid;
}

void CDataRecoveryDlg::AddChild(DWORD FirstExtPos,DWORD CurExtPos,TableList* TL)
{
	LARGE_INTEGER CUR;
	CUR.QuadPart=CurExtPos;
	CUR.QuadPart*=512;
	DWORD Numofread;
	unsigned char Buf[512];
	SetFilePointer(m_SearchDisk,CUR.LowPart,&CUR.HighPart,FILE_BEGIN);
	ReadFile(m_SearchDisk,Buf,512,&Numofread,NULL);
	PartitionTable PT;
	unsigned char* p=Buf+446;
	TableList* cur=TL;
	for(int i=0;i<4;i++)
	{		
		memcpy(&PT,p,sizeof(PartitionTable));
		if(PT.PartitionID==0) break;
		cur=TL;
		while(cur->Next) cur=cur->Next;
		cur->Next=new TableList;
		PartitionTable* curtable=&cur->Next->Table;
		curtable->BootIndicator=PT.BootIndicator;
		curtable->EndData=PT.EndData;
		curtable->EndingHead=PT.EndingHead;
		curtable->PartitionID=PT.PartitionID;
		curtable->StartData=PT.StartData;
		curtable->StartingHead=PT.StartingHead;
		curtable->TotalSectors=PT.TotalSectors;
		
		if(PT.PartitionID!=0x05 && PT.PartitionID!=0x0F) 
		{
			curtable->RelativeSectors=PT.RelativeSectors+CurExtPos;			
			cur->Next->IsPartValid=CheckPartition(curtable->RelativeSectors,curtable->PartitionID);
		}
		else 
		{
			delete cur->Next;
			cur->Next=NULL;
			AddChild(FirstExtPos,PT.RelativeSectors+FirstExtPos,TL);
		}
		p+=sizeof(PartitionTable);
	}
}

DWORD WINAPI CDataRecoveryDlg::ReadTable(LPVOID lpVoid)
{
	CDataRecoveryDlg* CurDlg=(CDataRecoveryDlg*)lpVoid;
	DWORD readnum;
	PartitionTable PT;
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	DWORD dwGran=SysInfo.dwAllocationGranularity;
	DWORD m_dwBlockBytes=1000*dwGran;
	int numonetime=m_dwBlockBytes/512;//(扇区字节数)
	unsigned char* Buf=new unsigned char[m_dwBlockBytes];
	SetFilePointer(CurDlg->m_SearchDisk,0,NULL,FILE_BEGIN);
	if(GetLastError()) return FALSE;
	LARGE_INTEGER curpos;
	curpos.QuadPart=0;
	do 
	{
		ReadFile(CurDlg->m_SearchDisk,Buf,m_dwBlockBytes,&readnum,NULL);
		unsigned char* p=Buf;
		for(int i=0;i<numonetime;i++)
		{
			if(p[510]==0x55 && p[511]==0xAA && (p[446]==0x80 || p[446]==0x00))
			{
				unsigned char* ptr=p+446;
				for(int j=0;j<4;j++)
				{
					memcpy(&PT,ptr,sizeof(PartitionTable));
					if(PT.PartitionID==0) break;//循环结束条件
					switch(PT.PartitionID)
					{
						case 0x01:
						case 0x11:
						case 0x04:
						case 0x06:
						case 0x0E:
						case 0x14:
						case 0x1E:
						case 0x07:
						case 0x17:
						case 0x0B:
						case 0x0C:
						case 0x16:
						case 0x1B:
						case 0x1C:
							break;
						default:
							continue;
					}
					LARGE_INTEGER cur;
					cur.QuadPart=PT.RelativeSectors;
					cur.QuadPart*=512;
					cur.QuadPart+=curpos.QuadPart+i*512;
					TableList* curTL=CurDlg->TL;
					BOOL isRepeat=FALSE;
					while(curTL->Next)
					{
						if(curTL->Next->Table.RelativeSectors==cur.QuadPart/512)
						{
							isRepeat=TRUE;
							break;
						}
						curTL=curTL->Next;
					}
					if(isRepeat) continue;
					curTL->Next=new TableList;
					PartitionTable* curtable=&curTL->Next->Table;
					curtable->BootIndicator=PT.BootIndicator;
					curtable->EndData=PT.EndData;
					curtable->EndingHead=PT.EndingHead;
					curtable->PartitionID=PT.PartitionID;
					curtable->StartData=PT.StartData;
					curtable->StartingHead=PT.StartingHead;
					curtable->TotalSectors=PT.TotalSectors;
					curtable->RelativeSectors=DWORD(cur.QuadPart/512);
					curTL->Next->IsPartValid=CurDlg->CheckPartition(curtable->RelativeSectors,curtable->PartitionID);
					//检查数据合法性
					ptr+=sizeof(PartitionTable);
				}
			}
			p+=512;
		}
		curpos.QuadPart+=m_dwBlockBytes;
	}
	while(readnum);
	delete []Buf;
	CurDlg->MessageBox("当前硬盘分区已扫描完成!");
	return TRUE;
}

