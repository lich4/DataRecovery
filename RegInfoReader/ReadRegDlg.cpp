// ReadRegDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ReadReg.h"
#include "ReadRegDlg.h"

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
// CReadRegDlg dialog

CReadRegDlg::CReadRegDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReadRegDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReadRegDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CReadRegDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReadRegDlg)
	DDX_Control(pDX, IDC_TREE, m_RegTree);
	DDX_Control(pDX, IDC_LIST, m_RegList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CReadRegDlg, CDialog)
	//{{AFX_MSG_MAP(CReadRegDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_COMMAND(ID_OPEN, OnOpen)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReadRegDlg message handlers

BOOL CReadRegDlg::OnInitDialog()
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
	m_RegList.ModifyStyle(LVS_ICON|LVS_SMALLICON|LVS_LIST,LVS_REPORT|LVS_SINGLESEL);
	m_RegList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_RegList.InsertColumn(0,"名称",LVCFMT_LEFT,200);
	m_RegList.InsertColumn(1,"类型",LVCFMT_LEFT,50);
	m_RegList.InsertColumn(2,"数据",LVCFMT_LEFT,200);
	
	Buf=NULL;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CReadRegDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CReadRegDlg::OnPaint() 
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
HCURSOR CReadRegDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CReadRegDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	if(Buf) delete []Buf;
	CDialog::OnClose();
}

void CReadRegDlg::AnalysisRegFile()
{
	REGF* HEAD=(REGF*)Buf;
	if(memcmp(Buf,"regf",4)) 
	{
		MessageBox("非注册表文件!","ERROR",MB_OK);
		return;
	}
	//找到第一个hbin作为相对位置
	relative=Buf;
	do
	{
		relative+=0x1000;
	}
	while(memcmp(relative,"hbin",4));
	Key_Cell* FirstKey=(Key_Cell*)(relative+HEAD->RootCell);
	ASSERT(memcmp(FirstKey->Signature,"nk",2)==0);

	HTREEITEM root=m_RegTree.InsertItem("root");
	m_RegTree.SetItemData(root,(DWORD)&FirstKey->Count);//记录值
	//枚举子节点
	DWORD childnum;
	KeyChildList* childlist=(KeyChildList*)(FirstKey->SubKeyList[0]+relative);
	if(FirstKey->SubKeyList[0]>m_size) return;

	if(memcmp(childlist->Signature,"lf",2)==0 || memcmp(childlist->Signature,"lh",2)==0)
	{
		KeyChildList* nextchild=childlist+1;
		childnum=(0x100000000-childlist->Size)/sizeof(KeyChildList)-1;
		for(DWORD i=0;i<childnum;i++)
		{
			if(((InsideChild*)nextchild)->offset<m_size)
				AddToTree(root,((InsideChild*)nextchild)->offset);
			nextchild+=1;
		}
	}
	else if(memcmp(childlist->Signature,"ri",2)==0 || memcmp(childlist->Signature,"li",2)==0)
	{
		DWORD* nextchild=(DWORD*)(childlist+2);
		childnum=(0x100000000-childlist->Size)/sizeof(DWORD)-2;
		for(DWORD i=0;i<childnum;i++)
		{
			if(((InsideChild*)nextchild)->offset<m_size)
				AddToTree(root,*nextchild);
			nextchild+=1;
		}
	}
	else 
		return;	
}

void CReadRegDlg::AddToTree(HTREEITEM curtree,DWORD curkey)
{//curtree:父树		curkey:键巢室指针
	Key_Cell* FirstKey=(Key_Cell*)(relative+curkey);

	if(memcmp(FirstKey->Signature,"nk",2)==0)
	{
		char* name=new char[FirstKey->NameLength+1];
		memcpy(name,FirstKey+1,FirstKey->NameLength);
		name[FirstKey->NameLength]='\0';
		HTREEITEM child=m_RegTree.InsertItem(name,curtree,TVI_LAST);
		TRACE2("pos=%08x,name=%s\n",curkey,name);

		delete []name;
		m_RegTree.SetItemData(child,(DWORD)&FirstKey->Count);//记录值
		
		DWORD childnum;
		KeyChildList* childlist=(KeyChildList*)(FirstKey->SubKeyList[0]+relative);
		if(FirstKey->SubKeyList[0]>m_size) return;

		if(memcmp(childlist->Signature,"lh",2)==0 || memcmp(childlist->Signature,"lf",2)==0)
		{
			childnum=(0x100000000-childlist->Size)/sizeof(KeyChildList)-1;
			KeyChildList* nextchild=childlist+1;
			for(DWORD i=0;i<childnum;i++)
			{
				if(((InsideChild*)nextchild)->offset<m_size)
				{
					if(((InsideChild*)nextchild)->offset==0x20) continue;
					if(((InsideChild*)nextchild)->offset==curkey) continue;
					AddToTree(child,((InsideChild*)nextchild)->offset);
				}
				nextchild+=1;
			}
		}
		else if(memcmp(childlist->Signature,"ri",2)==0 || memcmp(childlist->Signature,"li",2)==0)
		{
			childnum=(0x100000000-childlist->Size)/sizeof(DWORD)-2;
			DWORD* nextchild=(DWORD*)(childlist+1);
			for(DWORD i=0;i<childnum;i++)
			{
				if(*nextchild<m_size)
				{
					if(*nextchild==0x20) continue;
					if(*nextchild==curkey) continue;
					AddToTree(child,*nextchild);
				}
				nextchild+=1;
			}
		}
		else 
			return;
	}
	else
	{
		KeyChildList* childlist=(KeyChildList*)FirstKey;
		if(memcmp(FirstKey->Signature,"lh",2)==0 || memcmp(FirstKey->Signature,"lf",2)==0)
		{
			KeyChildList* nextchild=childlist+1;
			DWORD othersize=(0x100000000-childlist->Size)/sizeof(KeyChildList)-1;
			while(othersize--)
			{
				if(((InsideChild*)nextchild)->offset<m_size)
				{
					if(((InsideChild*)nextchild)->offset==0x20) continue;
					if(((InsideChild*)nextchild)->offset==curkey) continue;
					AddToTree(curtree,((InsideChild*)nextchild)->offset);
				}
				nextchild+=1;
			}
		}
		else if(memcmp(FirstKey->Signature,"ri",2)==0 || memcmp(FirstKey->Signature,"li",2)==0)
		{
			DWORD* nextchild=(DWORD*)(childlist+1);
			DWORD othersize=(0x100000000-childlist->Size)/sizeof(DWORD)-2;
			while(othersize--)
			{
				if(((InsideChild*)nextchild)->offset<m_size)
				{
					if(*nextchild==0x20) continue;
					if(*nextchild==curkey) continue;
					AddToTree(curtree,*nextchild);
				}
				nextchild+=1;
			}
		}
		else
			return;
	}
}

void CReadRegDlg::OnOpen() 
{
	// TODO: Add your command handler code here
	CFileDialog cur(TRUE);
	cur.DoModal();
	CFile file(cur.GetPathName(),CFile::modeRead);
	DWORD size=file.GetLength();
	m_size=size;
	if(Buf) delete []Buf;
	m_RegTree.DeleteAllItems();
	m_RegList.DeleteAllItems();
	Buf=new char[size];
	file.Read(Buf,size);
	file.Close();
	AnalysisRegFile();
}

void CReadRegDlg::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_RegList.DeleteAllItems();
	DWORD* curlist=(DWORD*)m_RegTree.GetItemData(m_RegTree.GetSelectedItem());
	if(curlist==NULL) return;
	int num=curlist[0];
	DWORD* address=(DWORD*)(curlist[1]+relative);
	address++;
	for(int i=0;i<num;i++)
	{
		KeyNode* KN=(KeyNode*)(*address+relative);
		char* buf=new char[KN->NameLength+1];
		memcpy(buf,KN+1,KN->NameLength);
		buf[KN->NameLength]='\0';
		m_RegList.InsertItem(i,buf);
		delete []buf;
		CString temp="";
		switch(KN->Type)
		{
			case REG_NONE:
				temp="REG_NONE";
				break;
			case REG_SZ:
				temp="REG_SZ";
				break;
			case REG_EXPAND_SZ:
				temp="REG_EXPAND_SZ";
				break;
			case REG_BINARY:
				temp="REG_BINARY";
				break;
			case REG_DWORD:
				temp="REG_DWORD";
				break;
			case REG_DWORD_BIG_ENDIAN:
				temp="REG_DWORD_BIG_ENDIAN";
				break;
			case REG_LINK:
				temp="REG_LINK";
				break;
			case REG_MULTI_SZ:
				temp="REG_MULTI_SZ";
				break;
			case REG_RESOURCE_LIST:
				temp="REG_RESOURCE_LIST";
				break;
			case REG_FULL_RESOURCE_DESCRIPTOR:
				temp="REG_FULL_RESOURCE_DESCRIPTOR";
				break;
			case REG_RESOURCE_REQUIREMENTS_LIST:
				temp="REG_RESOURCE_REQUIREMENTS_LIST";
				break;
			default:
				break;
		}
		m_RegList.SetItemText(i,1,temp);
		if(KN->DataType&128)//为数据
		{
			char unicode[4];
			memcpy(unicode,&KN->Data,4);
			if(unicode[0]>=0x30)
			{
				buf=new char[2];
				WideCharToMultiByte(CP_ACP,0,(wchar_t*)unicode,-1,buf,2,NULL,NULL);
				m_RegList.SetItemText(i,2,buf);
				delete []buf;		
			}
			else
			{
				CString str;
				str.Format("%08x",KN->Data);
				m_RegList.SetItemText(i,2,str);
			}

		}
		else//为指针
		{
			WORD length=KN->DataLength;
			switch(KN->Type)
			{
				case REG_NONE:
				case REG_BINARY:
					{
						char* temp=new char[3*length+1];
						strcpy(temp,"");
						for(WORD j=0;j<length;j++)
						{
							sprintf(temp,"%s%02X ",temp,*(BYTE*)(relative+KN->Data+4+j));
						}
						m_RegList.SetItemText(i,2,temp);
						delete []temp;
						break;
					}

				case REG_SZ:
				case REG_MULTI_SZ:
				case REG_RESOURCE_LIST:
				case REG_FULL_RESOURCE_DESCRIPTOR:
				case REG_RESOURCE_REQUIREMENTS_LIST:
					{
						char* unicode=new char[length];
						memcpy(unicode,relative+KN->Data+4,length);
						int nByte=WideCharToMultiByte(CP_ACP,0,(wchar_t*)unicode,-1,NULL,0,NULL,NULL);
						buf=new char[nByte];
						WideCharToMultiByte(CP_ACP,0,(wchar_t*)unicode,-1,buf,nByte,NULL,NULL);
						m_RegList.SetItemText(i,2,buf);
						delete []unicode;
						delete []buf;
						break;
					}
				default:
					break;
			}
		}
		address++;
	}
	*pResult = 0;
}
