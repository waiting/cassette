// AccountCateEditingDlg.cpp : implementation file
//

#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountCateEditingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AccountCateEditingDlg dialog


AccountCateEditingDlg::AccountCateEditingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AccountCateEditingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AccountCateEditingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AccountCateEditingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AccountCateEditingDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AccountCateEditingDlg, CDialog)
	//{{AFX_MSG_MAP(AccountCateEditingDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AccountCateEditingDlg message handlers
