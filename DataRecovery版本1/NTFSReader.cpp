// NTFSReader.cpp: implementation of the NTFSReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataRecovery.h"
#include "NTFSReader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NTFSReader::NTFSReader()
{

}

NTFSReader::~NTFSReader()
{

}

DirTree::DirTree(int num)
{
	root=new DirTreeNode[2*num];//留一个位置给lostfile
	stillnotfind=new LostFile;
	m_TreeCtrl=NULL;
	m_ListCtrl=NULL;
	root[5].selfMFT=5;
	root[5].name="root";
	TotalNum=num;
}

DirTree::~DirTree()
{//析构
	for(int cur=0;cur<2*TotalNum;cur++)
	{
		while(root[cur].nextchild)
		{
			DirTreeNode* temp=root[cur].nextchild->nextchild;
			delete root[cur].nextchild;
			root[cur].nextchild=temp;
		}
	}
	delete []root;
	while(stillnotfind->next)
	{
		LostFile* temp=stillnotfind->next;
		stillnotfind->next=stillnotfind->next->next;
		delete temp;
	}
	delete stillnotfind;
};

void DirTree::AddFileToTree(int self,int father,int serial,CString& name)
{
	if(father>2*TotalNum-1 || self>2*TotalNum-1) 
	{//如果不在散列表中则加入到未知树中
		LostFile* temp=stillnotfind->next;
		stillnotfind->next=new LostFile;
		stillnotfind->next->FatherMFT=father;
		stillnotfind->next->name=name;
		stillnotfind->next->next=temp;
		stillnotfind->next->serial=serial;
		stillnotfind->next->SelfMFT=self;
		return;
	}
	
	DirTreeNode* temp=root+father;
	while(temp->nextchild) 
		temp=temp->nextchild;
	temp->nextchild=new DirTreeNode;
	temp->nextchild->name=name;
	temp->nextchild->selfMFT=self;
	temp->nextchild->serial=serial;
}

void DirTree::AddToCtrl(CTreeCtrl* Tree,CListCtrl* List)
{
	m_TreeCtrl=Tree;
	m_ListCtrl=List;
	root->m_TreeItem=Tree->InsertItem("根目录");
	AddToCtrlEx(root->m_TreeItem,5);
}

void DirTree::AddToCtrlEx(HTREEITEM cur,int serial)
{//serial为父目录MFT
	DirTreeNode* temp=root[serial].nextchild;
	while(temp)
	{
		HTREEITEM child=m_TreeCtrl->InsertItem(temp->name,cur,TVI_LAST);
		FILESTRUCT* PFS=(FILESTRUCT*)m_ListCtrl->GetItemData(temp->serial);
		PFS->treeitem=child;
		m_TreeCtrl->SetItemData(child,temp->serial);
		AddToCtrlEx(child,temp->selfMFT);
		temp=temp->nextchild;
	}
}
