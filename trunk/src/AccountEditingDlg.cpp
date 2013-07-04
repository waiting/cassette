// AccountEditingDlg.cpp : implementation file
//

#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountEditingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AccountEditingDlg dialog


AccountEditingDlg::AccountEditingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AccountEditingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AccountEditingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AccountEditingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AccountEditingDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AccountEditingDlg, CDialog)
	//{{AFX_MSG_MAP(AccountEditingDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AccountEditingDlg message handlers
