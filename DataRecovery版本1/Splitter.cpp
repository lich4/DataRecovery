// Splitter.cpp : implementation file
//

#include "stdafx.h"
#include "datarecovery.h"
#include "Splitter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitter

CSplitter::CSplitter()
{
	m_IsLeftDown=FALSE;
}

CSplitter::~CSplitter()
{
}


BEGIN_MESSAGE_MAP(CSplitter, CButton)
	//{{AFX_MSG_MAP(CSplitter)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplitter message handlers

void CSplitter::OnMouseMove(UINT nFlags, CPoint point) 
{//point是相对于控件的坐标
	// TODO: Add your message handler code here and/or call default
	::SetCursor(::LoadCursor(NULL,IDC_SIZEWE));
	if(m_IsLeftDown)
	{
		POINT pt;
		GetCursorPos(&pt);
		RECT rtTree,rtList,rtSplit;
		m_Father->GetDlgItem(IDC_TREE)->GetWindowRect(&rtTree);
		m_Father->GetDlgItem(IDC_LIST)->GetWindowRect(&rtList);
		m_Father->GetDlgItem(IDC_SPLITTER)->GetWindowRect(&rtSplit);
		rtSplit.left=pt.x-10;
		rtSplit.right=pt.x+10;
		m_Father->ScreenToClient(&rtSplit);
		m_Father->GetDlgItem(IDC_SPLITTER)->MoveWindow(&rtSplit);
		
		m_Father->ScreenToClient(&rtTree);
		rtTree.right=rtSplit.left-5;
		m_Father->GetDlgItem(IDC_TREE)->MoveWindow(&rtTree);
		m_Father->GetDlgItem(IDC_TREE)->UpdateWindow();

		m_Father->ScreenToClient(&rtList);
		rtList.left=rtSplit.right+5;
		m_Father->GetDlgItem(IDC_LIST)->MoveWindow(&rtList);
		m_Father->GetDlgItem(IDC_LIST)->Invalidate();
	}

	CButton::OnMouseMove(nFlags, point);
}

void CSplitter::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_IsLeftDown=TRUE;
	CButton::OnLButtonDown(nFlags, point);
}

void CSplitter::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_IsLeftDown=FALSE;
	CButton::OnLButtonUp(nFlags, point);
}
