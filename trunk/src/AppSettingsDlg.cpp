// AppSettingsDlg.cpp : implementation file
//

#include "Cassette.h"
#include "CassetteApp.h"
#include "AppSettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AppSettingsDlg dialog


AppSettingsDlg::AppSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AppSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AppSettingsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AppSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AppSettingsDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AppSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(AppSettingsDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AppSettingsDlg message handlers
