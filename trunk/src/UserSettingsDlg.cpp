// UserSettingsDlg.cpp : implementation file
//

#include "Cassette.h"
#include "CassetteApp.h"
#include "UserSettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UserSettingsDlg dialog


UserSettingsDlg::UserSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(UserSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(UserSettingsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void UserSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UserSettingsDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UserSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(UserSettingsDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UserSettingsDlg message handlers
