// ReadTableDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ReadTable.h"
#include "ReadTableDlg.h"

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
// CReadTableDlg dialog

CReadTableDlg::CReadTableDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReadTableDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReadTableDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CReadTableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReadTableDlg)
	DDX_Control(pDX, IDC_LIST1, m_disklist);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CReadTableDlg, CDialog)
	//{{AFX_MSG_MAP(CReadTableDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReadTableDlg message handlers

BOOL CReadTableDlg::OnInitDialog()
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
	m_disklist.ModifyStyle(LVS_ICON|LVS_SMALLICON|LVS_LIST,LVS_REPORT|LVS_SINGLESEL);
	m_disklist.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);	
	m_disklist.InsertColumn(0,"序号",LVCFMT_LEFT,80);
	m_disklist.InsertColumn(1,"起始磁头号",LVCFMT_LEFT,80);
	m_disklist.InsertColumn(2,"起始扇区号",LVCFMT_LEFT,80);
	m_disklist.InsertColumn(3,"起始柱面号",LVCFMT_LEFT,80);
	m_disklist.InsertColumn(4,"分区类型",LVCFMT_LEFT,60);
	m_disklist.InsertColumn(5,"结束磁头号",LVCFMT_LEFT,80);
	m_disklist.InsertColumn(6,"结束扇区号",LVCFMT_LEFT,80);
	m_disklist.InsertColumn(7,"结束柱面号",LVCFMT_LEFT,80);
	m_disklist.InsertColumn(8,"扇区绝对位置",LVCFMT_LEFT,150);
	m_disklist.InsertColumn(9,"分区大小(Byte)",LVCFMT_LEFT,150);
	m_disklist.InsertColumn(10,"是否有效",LVCFMT_LEFT,50);

	hdev=CreateFile("\\\\.\\PhysicalDrive0",GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
	char Buf[512];
	DWORD readnum;
	TotalSec=0;
	//计算总的硬盘扇区大小
	ReadFile(hdev,Buf,512,&readnum,NULL);
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
	CreateThread(NULL,0,ReadTable,this,0,0);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CReadTableDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CReadTableDlg::OnPaint() 
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
		DrawMain();
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CReadTableDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

DWORD WINAPI CReadTableDlg::ReadTable(LPVOID lpVoid)
{
	CReadTableDlg* CurDlg=(CReadTableDlg*)lpVoid;
	DWORD readnum;
	char temp[32];
	PartitionTable PT;
	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	DWORD dwGran=SysInfo.dwAllocationGranularity;
	DWORD m_dwBlockBytes=1000*dwGran;
	int numonetime=m_dwBlockBytes/512;//(扇区字节数)
	unsigned char* Buf=new unsigned char[m_dwBlockBytes];
	CurDlg->m_disklist.DeleteAllItems();
	SetFilePointer(CurDlg->hdev,0,NULL,FILE_BEGIN);
	if(GetLastError()) return FALSE;
	LARGE_INTEGER curpos;
	curpos.QuadPart=0;
	do 
	{
		ReadFile(CurDlg->hdev,Buf,m_dwBlockBytes,&readnum,NULL);
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
		sprintf(temp,"当前位置:H:%X,L:%10X",curpos.HighPart,curpos.LowPart);
		CurDlg->GetDlgItem(IDC_STATIC)->SetWindowText(temp);
	}
	while(readnum);
	delete []Buf;

	return TRUE;
}

void CReadTableDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CloseHandle(hdev);
	TableList* cur=TL;
	while(TL)
	{
		cur=TL->Next;
		delete TL;
		TL=cur;
	}
	CDialog::OnClose();
}


void CReadTableDlg::OnOK() 
{
	// TODO: Add extra validation here
	m_disklist.DeleteAllItems();
	int findnum=0;
	CDC* pDC=GetDlgItem(IDC_PIC)->GetDC();
	RECT rt;
	CPen pen;
	pen.CreatePen(PS_SOLID,6,RGB(0,0,255));
	CPen* OldPen=(CPen*)pDC->SelectObject(&pen);//主硬盘用红色
	GetDlgItem(IDC_PIC)->GetClientRect(&rt);
	pDC->Rectangle(rt.left,rt.top,rt.right,rt.bottom);
	pDC->SelectObject(OldPen);
	pen.DeleteObject();
	
	char temp[64];
	int length=rt.right-rt.left;
	int delta=5;
	TableList* cur=TL->Next;
	int num=ActualNum;
	while(cur)
	{
		sprintf(temp,"%d",findnum);
		m_disklist.InsertItem(findnum,temp);
		sprintf(temp,"%d",cur->Table.StartingHead);
		m_disklist.SetItemText(findnum,1,temp);
		sprintf(temp,"%d",cur->Table.StartData&63);
		m_disklist.SetItemText(findnum,2,temp);
		sprintf(temp,"%d",cur->Table.StartData>>6);
		m_disklist.SetItemText(findnum,3,temp);
		switch(cur->Table.PartitionID)
		{
			case 0x01:
			case 0x11:
				strcpy(temp,"FAT12");
				break;
			case 0x04:
			case 0x06:
			case 0x0E:
			case 0x14:
			case 0x1E:
				strcpy(temp,"FAT16");
				break;
			case 0x07:
			case 0x17:
				strcpy(temp,"NTFS");
				break;
			case 0x0B:
			case 0x0C:
			case 0x16:
			case 0x1B:
			case 0x1C:
				strcpy(temp,"FAT32");
				break;
			case 0x05:
			case 0x0F:
				strcpy(temp,"扩展分区");
				break;
			default:
				continue;
		}
		m_disklist.SetItemText(findnum,4,temp);
		sprintf(temp,"%d",cur->Table.EndingHead);
		m_disklist.SetItemText(findnum,5,temp);
		sprintf(temp,"%d",cur->Table.EndData&63);
		m_disklist.SetItemText(findnum,6,temp);
		sprintf(temp,"%d",cur->Table.EndData>>6);
		m_disklist.SetItemText(findnum,7,temp);
		LARGE_INTEGER Cur;
		Cur.QuadPart=cur->Table.RelativeSectors;
		Cur.QuadPart*=512;
		sprintf(temp,"H:%u,L:%10u",Cur.HighPart,Cur.LowPart);
		m_disklist.SetItemText(findnum,8,temp);
		sprintf(temp,"%dGB",cur->Table.TotalSectors/1024/1024*512/1024);
		m_disklist.SetItemText(findnum,9,temp);
		if(cur->IsPartValid) m_disklist.SetItemText(findnum,10,"有效");
		else m_disklist.SetItemText(findnum,10,"无效");

		if(findnum<=num)
		{
			//画分区表
			RECT rtchild;
			rtchild.left=int((double)cur->Table.RelativeSectors/TotalSec*length)+10;
			rtchild.right=rtchild.left+int((double)cur->Table.TotalSectors/TotalSec*length)-10;
			rtchild.top=rt.top+10;
			rtchild.bottom=rt.bottom-10;
			CString str="";
			switch(cur->Table.PartitionID)
			{
				case 0x01:
				case 0x11:
					pen.CreatePen(PS_SOLID,3,RGB(153,51,0));
					str="FAT12";
					break;
				case 0x04:
				case 0x06:
				case 0x0E:
				case 0x14:
				case 0x1E:
					pen.CreatePen(PS_SOLID,3,0x00BFFF);
					str="FAT16";
					break;
				case 0x07:
				case 0x17:
					pen.CreatePen(PS_SOLID,3,0x0000FF);
					str="NTFS";
					break;
				case 0x0B:
				case 0x0C:
				case 0x16:
				case 0x1B:
				case 0x1C:
					pen.CreatePen(PS_SOLID,3,0x808000);
					str="FAT32";
					break;
				case 0x05:
				case 0x0F:
					pen.CreatePen(PS_SOLID,3,0xFFD700);
					rtchild.left-=5;
					rtchild.right+=5;
					rtchild.top-=5;
					rtchild.bottom+=5;
					break;
				default:
					pen.CreatePen(PS_SOLID,3,RGB(0,0,0));
					break;
			}
			OldPen=(CPen*)pDC->SelectObject(&pen);
			pDC->Rectangle(&rtchild);
			pDC->DrawText(str,&rtchild,DT_CENTER);
			pDC->SelectObject(OldPen);
			pen.DeleteObject();
		}

		cur=cur->Next;
		findnum++;
	}
	GetDlgItem(IDC_PIC)->ReleaseDC(pDC);
}

void CReadTableDlg::AddChild(DWORD FirstExtPos,DWORD CurExtPos,TableList* TL)
{
	LARGE_INTEGER CUR;
	CUR.QuadPart=CurExtPos;
	CUR.QuadPart*=512;
	DWORD Numofread;
	unsigned char Buf[512];
	SetFilePointer(hdev,CUR.LowPart,&CUR.HighPart,FILE_BEGIN);
	ReadFile(hdev,Buf,512,&Numofread,NULL);
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

void CReadTableDlg::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	DrawMain();
	CDC* pDC=GetDlgItem(IDC_PIC)->GetDC();
	POSITION pos=m_disklist.GetFirstSelectedItemPosition();
	if(!pos) return;
	int cur=m_disklist.GetNextSelectedItem(pos);
	TableList* curList=TL->Next;
	for(int i=0;i<cur;i++)
	{
		curList=curList->Next;
	}
	RECT rt;
	GetDlgItem(IDC_PIC)->GetClientRect(&rt);
	int length=rt.right-rt.left;
	RECT rtchild;
	rtchild.left=int((double)curList->Table.RelativeSectors/TotalSec*length)+10;
	rtchild.right=rtchild.left+int((double)curList->Table.TotalSectors/TotalSec*length)-10;
	rtchild.top=rt.top+10;
	rtchild.bottom=rt.bottom-10;
	pDC->InvertRect(&rtchild);
	GetDlgItem(IDC_PIC)->ReleaseDC(pDC);
	*pResult = 0;
}

void CReadTableDlg::DrawMain()
{
	//画分区表
	TableList* cur=TL->Next;
	int num=ActualNum;
	CDC* pDC=GetDlgItem(IDC_PIC)->GetDC();
	RECT rt;
	CPen pen;
	
	int delta=5;
	pen.CreatePen(PS_SOLID,6,RGB(0,0,255));
	CPen* OldPen=(CPen*)pDC->SelectObject(&pen);//主硬盘用红色
	GetDlgItem(IDC_PIC)->GetClientRect(&rt);
	int length=rt.right-rt.left;
	pDC->Rectangle(rt.left,rt.top,rt.right,rt.bottom);
	pDC->SelectObject(OldPen);
	pen.DeleteObject();
	while(cur&&num)
	{
		RECT rtchild;
		rtchild.left=int((double)cur->Table.RelativeSectors/TotalSec*length)+10;
		rtchild.right=rtchild.left+int((double)cur->Table.TotalSectors/TotalSec*length)-10;
		rtchild.top=rt.top+10;
		rtchild.bottom=rt.bottom-10;
		CString str="";
		switch(cur->Table.PartitionID)
		{
			case 0x01:
			case 0x11:
				pen.CreatePen(PS_SOLID,3,RGB(153,51,0));
				str="FAT12";
				break;
			case 0x04:
			case 0x06:
			case 0x0E:
			case 0x14:
			case 0x1E:
				pen.CreatePen(PS_SOLID,3,0x00BFFF);
				str="FAT16";
				break;
			case 0x07:
			case 0x17:
				pen.CreatePen(PS_SOLID,3,0x0000FF);
				str="NTFS";
				break;
			case 0x0B:
			case 0x0C:
			case 0x16:
			case 0x1B:
			case 0x1C:
				pen.CreatePen(PS_SOLID,3,0x808000);
				str="FAT32";
				break;
			case 0x05:
			case 0x0F:
				pen.CreatePen(PS_SOLID,3,0xFFD700);
				rtchild.left-=5;
				rtchild.right+=5;
				rtchild.top-=5;
				rtchild.bottom+=5;
				break;
			default:
				pen.CreatePen(PS_SOLID,3,RGB(0,0,0));
				break;
		}
		OldPen=(CPen*)pDC->SelectObject(&pen);
		pDC->Rectangle(&rtchild);
		pDC->DrawText(str,&rtchild,DT_CENTER);
		pDC->SelectObject(OldPen);
		pen.DeleteObject();
		cur=cur->Next;
		num--;
	}
	GetDlgItem(IDC_PIC)->ReleaseDC(pDC);
}

BOOL CReadTableDlg::CheckPartition(DWORD RelativeSectors,BYTE PartitionID)
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
			if(memcmp(Buf+0x36,"FAT",3) && memcpy(Buf+0x36,"FAT12",5)) isValid=FALSE;
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
