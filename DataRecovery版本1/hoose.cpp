// hoose.cpp : implementation file
//

#include "stdafx.h"
#include "DataRecovery.h"
#include "hoose.h"
#include "NTFSReader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Choose dialog


Choose::Choose(CWnd* pParent /*=NULL*/)
	: CDialog(Choose::IDD, pParent)
{
	//{{AFX_DATA_INIT(Choose)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	hDevice=INVALID_HANDLE_VALUE;
}


void Choose::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Choose)
	DDX_Control(pDX, IDC_TAB, m_choose);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Choose, CDialog)
	//{{AFX_MSG_MAP(Choose)
	ON_NOTIFY(NM_CLICK, IDC_TAB, OnClickTab)
	ON_BN_CLICKED(IDREFRESH, OnRefresh)
	ON_NOTIFY(NM_CLICK,IDC_MyListCtrl,OnClickList)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Choose message handlers


BOOL Choose::OnInitDialog() 
{
	CDialog::OnInitDialog();
	hDevice=INVALID_HANDLE_VALUE;
	m_list=NULL;
	pImageList=NULL;
	TC_ITEM* m_pItem=new TC_ITEM[2];
	memset(m_pItem,0,sizeof(TC_ITEM[2]));
	m_pItem[0].mask=TCIF_TEXT|TCIF_IMAGE|TCIF_PARAM;
	m_pItem[0].iImage=0;
	m_pItem[0].pszText="逻辑驱动器";
	m_pItem[1].mask=TCIF_TEXT|TCIF_IMAGE|TCIF_PARAM;
	m_pItem[1].iImage=0;
	m_pItem[1].pszText="物理驱动器";
	
	HICON hIcon=ExtractIcon(NULL,"%SystemRoot%\\System32\\Shell32.dll",7);
	
	pImageList=new CImageList;
	ASSERT(pImageList!=NULL);
	pImageList->Create(32,32,TRUE,4,4);
	pImageList->Add(hIcon);
	DeleteObject(hIcon);
	
	m_choose.SetImageList(pImageList);
	m_choose.InsertItem(0,m_pItem);
	m_choose.InsertItem(1,m_pItem+1);
	ShowWindow(SW_SHOWNORMAL);
	// TODO: Add extra initialization here
	CRect r1,r2,r;
	m_choose.GetItemRect(0,&r1);
	m_choose.GetClientRect(&r2);
	r.left=r2.left;
	r.right=r2.right;
	r.top=r1.bottom;
	r.bottom=r2.bottom;
	
	m_list=new CListCtrl;
	m_list->Create(LVS_ALIGNLEFT|LVS_LIST|LVS_SINGLESEL|WS_BORDER,r,GetDlgItem(IDC_TAB),IDC_MyListCtrl);
	m_list->SetImageList(pImageList,LVSIL_SMALL);

	DWORD dwDrives;
	char i=0;
	CString cDriver;//最多100个盘
	dwDrives=GetLogicalDrives();
	while(dwDrives)
	{
		if(dwDrives&01)
		{	
			cDriver="本地磁盘 (A:)";
			char temp[]="A:";
			cDriver.SetAt(10,char(cDriver.GetAt(10)+i));
			temp[0]+=i;
			DWORD dwType=GetDriveType(temp);
			if(dwType!=DRIVE_FIXED) 
			{
				dwDrives>>=1;
				i++;
				continue;
			}
			m_list->InsertItem(m_list->GetItemCount(),cDriver);
		}
		i++;
		dwDrives>>=1;
	}
	m_list->ShowWindow(SW_SHOWNORMAL);
	delete []m_pItem;
	m_pItem=NULL;
	GetDlgItem(IDREFRESH)->EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void Choose::OnClickTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDREFRESH)->EnableWindow(FALSE);
	pNMHDR=pNMHDR;
	m_list->DeleteAllItems();
	CRect r1,r2,r;
	m_choose.GetItemRect(0,&r1);
	m_choose.GetClientRect(&r2);
	r.left=r2.left;
	r.right=r2.right;
	r.top=r1.bottom;
	r.bottom=r2.bottom;

	if(0==m_choose.GetCurSel())
	{
		GetDlgItem(IDREFRESH)->EnableWindow(FALSE);
		GetDlgItem(IDC_PIC)->EnableWindow(FALSE);
		m_list->ModifyStyle(LVS_REPORT|LVS_SINGLESEL,LVS_ICON|LVS_SMALLICON|LVS_LIST);
		DWORD dwDrives;
		char i=0;
		CString cDriver;//最多100个盘
		dwDrives=GetLogicalDrives();
		while(dwDrives)
		{
			if(dwDrives&01)
			{	
				cDriver="本地磁盘 (A:)";
				char temp[]="A:";
				cDriver.SetAt(10,char(cDriver.GetAt(10)+i));
				temp[0]+=i;
				DWORD dwType=GetDriveType(temp);
				if(dwType!=DRIVE_FIXED) 
				{
					dwDrives>>=1;
					i++;
					continue;
				}
				m_list->InsertItem(m_list->GetItemCount(),cDriver);
			}
			i++;
			dwDrives>>=1;
		}
		m_list->ShowWindow(SW_SHOWNORMAL);
	}	
	else
	{
		GetDlgItem(IDREFRESH)->EnableWindow(TRUE);
		GetDlgItem(IDC_PIC)->EnableWindow(TRUE);
		m_list->ModifyStyle(LVS_ICON|LVS_SMALLICON|LVS_LIST,LVS_REPORT|LVS_SINGLESEL);
		m_list->SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);	
		m_list->InsertColumn(0,"序号",LVCFMT_LEFT,80);
		m_list->InsertColumn(1,"起始磁头号",LVCFMT_LEFT,80);
		m_list->InsertColumn(2,"起始扇区号",LVCFMT_LEFT,80);
		m_list->InsertColumn(3,"起始柱面号",LVCFMT_LEFT,80);
		m_list->InsertColumn(4,"分区类型",LVCFMT_LEFT,60);
		m_list->InsertColumn(5,"结束磁头号",LVCFMT_LEFT,80);
		m_list->InsertColumn(6,"结束扇区号",LVCFMT_LEFT,80);
		m_list->InsertColumn(7,"结束柱面号",LVCFMT_LEFT,80);
		m_list->InsertColumn(8,"扇区绝对位置",LVCFMT_LEFT,150);
		m_list->InsertColumn(9,"分区大小(Byte)",LVCFMT_LEFT,150);
		m_list->InsertColumn(10,"是否有效",LVCFMT_LEFT,50);
		
		m_list->DeleteAllItems();
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
		TableList* cur=TL->Next;
		int num=ActualNum;
		while(cur)
		{
			sprintf(temp,"%d",findnum);
			m_list->InsertItem(findnum,temp);
			sprintf(temp,"%d",cur->Table.StartingHead);
			m_list->SetItemText(findnum,1,temp);
			sprintf(temp,"%d",cur->Table.StartData&63);
			m_list->SetItemText(findnum,2,temp);
			sprintf(temp,"%d",cur->Table.StartData>>6);
			m_list->SetItemText(findnum,3,temp);
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
			m_list->SetItemText(findnum,4,temp);
			sprintf(temp,"%d",cur->Table.EndingHead);
			m_list->SetItemText(findnum,5,temp);
			sprintf(temp,"%d",cur->Table.EndData&63);
			m_list->SetItemText(findnum,6,temp);
			sprintf(temp,"%d",cur->Table.EndData>>6);
			m_list->SetItemText(findnum,7,temp);
			LARGE_INTEGER Cur;
			Cur.QuadPart=cur->Table.RelativeSectors;
			Cur.QuadPart*=512;
			sprintf(temp,"H:%u,L:%10u",Cur.HighPart,Cur.LowPart);
			m_list->SetItemText(findnum,8,temp);
			sprintf(temp,"%dGB",cur->Table.TotalSectors/1024/1024*512/1024);
			m_list->SetItemText(findnum,9,temp);
			if(cur->IsPartValid) m_list->SetItemText(findnum,10,"有效");
			else m_list->SetItemText(findnum,10,"无效");

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
	*pResult = 0;
}

void Choose::DrawMain()
{
	//画分区表
	TableList* cur=TL->Next;
	int num=ActualNum;
	CDC* pDC=GetDlgItem(IDC_PIC)->GetDC();
	RECT rt;
	CPen pen;
	
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

void Choose::OnOK() 
{
	POSITION i=m_list->GetFirstSelectedItemPosition();//i==NULL
	if(i==NULL) 
	{
		MessageBox("请选择!");
		return;
	}
	int count=m_list->GetNextSelectedItem(i);
	OFFSET=0;
	if(0==m_choose.GetCurSel())
	{
		CString str="\\\\.\\A: ";
		str.SetAt(4,(m_list->GetItemText(count,0)).GetAt(10));
		hDevice=CreateFile(str,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
		if(hDevice==INVALID_HANDLE_VALUE) 
		{
			CString str;
			str.Format("%d",GetLastError());
			str="CreateFile错误号:"+str;
			MessageBox(str);
			return;
		}
		unsigned char buf[512];
		DWORD NumofRead;
		ReadFile(hDevice,buf,512,&NumofRead,NULL);
		if(memcmp(buf+0x03,"NTFS",4)==0) PARTID=0x07;
		else if(memcmp(buf+0x52,"FAT32",5)==0) PARTID=0x0B;
		else
		{
			MessageBox("暂不支持此分区!");
			CloseHandle(hDevice);
			hDevice=NULL;
		}
	}
	else
	{
		TableList* curList=TL->Next;
		for(int i=0;i<count;i++)
		{
			curList=curList->Next;
		}
		switch(curList->Table.PartitionID) 
		{
			case 0x01:
			case 0x11:
			case 0x04:
			case 0x06:
			case 0x0E:
			case 0x14:
			case 0x1E:
			case 0x05:
			case 0x0F:
				MessageBox("暂不支持此类分区!");
				return;
			default:
				break;	
		}
		OFFSET=curList->Table.RelativeSectors;
		PARTID=curList->Table.PartitionID;
		hDevice=CreateFile("\\\\.\\PhysicalDrive0",GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
	}
	CDialog::OnOK();
}

void Choose::OnRefresh() 
{
	// TODO: Add your control notification handler code here
	m_list->DeleteAllItems();
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
	TableList* cur=TL->Next;
	int num=ActualNum;
	while(cur)
	{
		sprintf(temp,"%d",findnum);
		m_list->InsertItem(findnum,temp);
		sprintf(temp,"%d",cur->Table.StartingHead);
		m_list->SetItemText(findnum,1,temp);
		sprintf(temp,"%d",cur->Table.StartData&63);
		m_list->SetItemText(findnum,2,temp);
		sprintf(temp,"%d",cur->Table.StartData>>6);
		m_list->SetItemText(findnum,3,temp);
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
		m_list->SetItemText(findnum,4,temp);
		sprintf(temp,"%d",cur->Table.EndingHead);
		m_list->SetItemText(findnum,5,temp);
		sprintf(temp,"%d",cur->Table.EndData&63);
		m_list->SetItemText(findnum,6,temp);
		sprintf(temp,"%d",cur->Table.EndData>>6);
		m_list->SetItemText(findnum,7,temp);
		LARGE_INTEGER Cur;
		Cur.QuadPart=cur->Table.RelativeSectors;
		Cur.QuadPart*=512;
		sprintf(temp,"H:%u,L:%10u",Cur.HighPart,Cur.LowPart);
		m_list->SetItemText(findnum,8,temp);
		sprintf(temp,"%dGB",cur->Table.TotalSectors/1024/1024*512/1024);
		m_list->SetItemText(findnum,9,temp);
		if(cur->IsPartValid) m_list->SetItemText(findnum,10,"有效");
		else m_list->SetItemText(findnum,10,"无效");

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

void Choose::OnClickList(NMHDR* pNMHDR, LRESULT* pResult)
{
	if(0==m_choose.GetCurSel()) return;
	DrawMain();
	CDC* pDC=GetDlgItem(IDC_PIC)->GetDC();
	POSITION pos=m_list->GetFirstSelectedItemPosition();
	if(!pos) return;
	int cur=m_list->GetNextSelectedItem(pos);
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

void Choose::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	DrawMain();
	// Do not call CDialog::OnPaint() for painting messages
}

