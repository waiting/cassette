// UserRegisterDlg.cpp : implementation file
//

#include "Cassette.h"
#include "CassetteApp.h"
#include "UserRegisterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UserRegisterDlg dialog


UserRegisterDlg::UserRegisterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(UserRegisterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(UserRegisterDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void UserRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UserRegisterDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UserRegisterDlg, CDialog)
	//{{AFX_MSG_MAP(UserRegisterDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UserRegisterDlg message handlers
