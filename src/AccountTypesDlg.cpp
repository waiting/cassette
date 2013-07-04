// AccountTypesDlg.cpp : implementation file
//

#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountTypesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AccountTypesDlg dialog


AccountTypesDlg::AccountTypesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AccountTypesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AccountTypesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AccountTypesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AccountTypesDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AccountTypesDlg, CDialog)
	//{{AFX_MSG_MAP(AccountTypesDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AccountTypesDlg message handlers
