// RecoverStatus.cpp : implementation file
//

#include "stdafx.h"
#include "DataRecovery.h"
#include "RecoverStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecoverStatus dialog


CRecoverStatus::CRecoverStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CRecoverStatus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRecoverStatus)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRecoverStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecoverStatus)
	DDX_Control(pDX, IDC_PROGRESS, m_Status);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRecoverStatus, CDialog)
	//{{AFX_MSG_MAP(CRecoverStatus)
	ON_BN_CLICKED(IDC_Stop, OnStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecoverStatus message handlers

BOOL CRecoverStatus::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_Status.SetRange(0,100);
	m_Status.SetPos(0);
	m_StopThread=FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRecoverStatus::OnStop() 
{
	// TODO: Add your control notification handler code here
	m_StopThread=TRUE;
}
