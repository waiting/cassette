// AccountCatesDlg.cpp : implementation file
//

#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountCatesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AccountCatesDlg dialog


AccountCatesDlg::AccountCatesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AccountCatesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AccountCatesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AccountCatesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AccountCatesDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AccountCatesDlg, CDialog)
	//{{AFX_MSG_MAP(AccountCatesDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AccountCatesDlg message handlers
