// ShowInfo.cpp : implementation file
//

#include "stdafx.h"
#include "FormatRecovery.h"
#include "ShowInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowInfo dialog


CShowInfo::CShowInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(CShowInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShowInfo)
	m_showinfo = _T("");
	//}}AFX_DATA_INIT
}


void CShowInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShowInfo)
	DDX_Text(pDX, IDC_SHOWINFO, m_showinfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShowInfo, CDialogEx)
	//{{AFX_MSG_MAP(CShowInfo)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowInfo message handlers
