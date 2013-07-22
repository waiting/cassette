
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
AppSettingsDlg::AppSettingsDlg( CWnd * pParent, CassetteApp::SettingsParameters * pSettings )
: Dialog( AppSettingsDlg::IDD, pParent ), m_pSettings(pSettings)
{
	//{{AFX_DATA_INIT(AppSettingsDlg)
	m_IsEnabledAutoRun = pSettings->isEnabledAutoRun;
	m_IsEnabledHotkey = pSettings->isEnabledHotkey;
	m_IsEnabledHttpSrv = pSettings->isEnabledHttpSrv;
	m_IsEnabledScheme = pSettings->isEnabledScheme;
	m_BackupPath = pSettings->backupPath.c_str();
	m_DatabasePath = pSettings->databasePath.c_str();
	//}}AFX_DATA_INIT
}

void AppSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AppSettingsDlg)
	DDX_Check(pDX, IDC_CHECK_ENABLEAUTORUN, m_IsEnabledAutoRun);
	DDX_Check(pDX, IDC_CHECK_ENABLEHOTKEY, m_IsEnabledHotkey);
	DDX_Check(pDX, IDC_CHECK_ENABLEHTTPSRV, m_IsEnabledHttpSrv);
	DDX_Check(pDX, IDC_CHECK_ENABLESCHEME, m_IsEnabledScheme);
	DDX_Text(pDX, IDC_EDIT_BACKUPPATH, m_BackupPath);
	DDX_Text(pDX, IDC_EDIT_DATABASEPATH, m_DatabasePath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(AppSettingsDlg, Dialog)
	//{{AFX_MSG_MAP(AppSettingsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AppSettingsDlg message handlers

BOOL AppSettingsDlg::OnInitDialog() 
{
	Dialog::OnInitDialog();
	m_ToolTips.SetTipTextColor( RGB( 255, 96, 0 ) ); // 设置提示文本颜色
	m_ToolTips.SetMaxTipWidth(240);




	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AppSettingsDlg::OnOK() 
{
	UpdateData(TRUE);
	m_pSettings->isEnabledAutoRun = m_IsEnabledAutoRun != FALSE;
	m_pSettings->isEnabledHotkey = m_IsEnabledHotkey != FALSE;
	m_pSettings->isEnabledHttpSrv = m_IsEnabledHttpSrv != FALSE;
	m_pSettings->isEnabledScheme = m_IsEnabledScheme != FALSE;
	m_pSettings->backupPath = m_BackupPath;
	m_pSettings->databasePath = m_DatabasePath;

	EndDialog(IDOK);
}
