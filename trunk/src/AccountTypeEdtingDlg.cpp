// AccountTypeEdtingDlg.cpp : implementation file
//

#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountTypeEdtingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AccountTypeEdtingDlg dialog


AccountTypeEdtingDlg::AccountTypeEdtingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AccountTypeEdtingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AccountTypeEdtingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AccountTypeEdtingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AccountTypeEdtingDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AccountTypeEdtingDlg, CDialog)
	//{{AFX_MSG_MAP(AccountTypeEdtingDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AccountTypeEdtingDlg message handlers
