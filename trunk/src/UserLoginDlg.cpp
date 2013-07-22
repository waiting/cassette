
#include "Cassette.h"
#include "CassetteApp.h"
#include "UserLoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UserLoginDlg dialog

UserLoginDlg::UserLoginDlg( CWnd * pParent, CString * username, CString * password, BOOL * autoLogin, BOOL * savePwd )
: Dialog(UserLoginDlg::IDD, pParent), m_username(username), m_password(password), m_autoLogin(autoLogin), m_savePwd(savePwd)
{
	//{{AFX_DATA_INIT(UserLoginDlg)
	//}}AFX_DATA_INIT
}


void UserLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UserLoginDlg)
	DDX_Control( pDX, IDC_LABEL_LOGIN, m_lblLogin );
	//}}AFX_DATA_MAP
	DDX_Text( pDX, IDC_EDIT_USERNAME, *m_username );
	DDX_Text( pDX, IDC_EDIT_PASSWORD, *m_password );
	DDX_Check( pDX, IDC_CHECK_AUTOLOGIN, *m_autoLogin );
	DDX_Check( pDX, IDC_CHECK_SAVEPWD, *m_savePwd );
}

BEGIN_MESSAGE_MAP(UserLoginDlg, Dialog)
	//{{AFX_MSG_MAP(UserLoginDlg)
		ON_NOTIFY( NM_RETURN, IDC_SYSLINK_REGISTER, OnSysLinkRegister )
		ON_NOTIFY( NM_RETURN, IDC_SYSLINK_FORGETPWD, OnSysLinkForgetPwd )
		ON_NOTIFY( NM_CLICK, IDC_SYSLINK_REGISTER, OnSysLinkRegister )
		ON_NOTIFY( NM_CLICK, IDC_SYSLINK_FORGETPWD, OnSysLinkForgetPwd )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UserLoginDlg message handlers

void UserLoginDlg::OnSysLinkRegister( NMHDR * pNmHdr, LRESULT * pResult )
{

	*pResult = 0;
}

void UserLoginDlg::OnSysLinkForgetPwd( NMHDR * pNmHdr, LRESULT * pResult )
{

	*pResult = 0;
}

BOOL UserLoginDlg::OnInitDialog() 
{
	Dialog::OnInitDialog();

	m_ToolTips.SetTipTextColor( RGB( 255, 96, 0 ) ); // 设置提示文本颜色
	// 把窗口句柄提交给共享内存,以便激活
	g_theApp.GetSharedMemory()->hMainWnd = this->GetSafeHwnd();
	// 修改logo文本样式
	m_lblLogin.SetColor( RGB( 0, 128, 255 ) );
	m_lblLogin.SetFontSize(240);
	m_lblLogin.ApplyChange();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void UserLoginDlg::OnOK() 
{
	this->UpdateData(TRUE);

	if ( m_username->IsEmpty() )
	{
		WarningError( _T("用户名不能为空"), _T("错误") );
		return;
	}

	if ( !VerifyUserPassword( *m_username, *m_password ) )
	{
		WarningError( _T("密码不正确"), _T("错误") );
		return;
	}
	Dialog::OnOK();
}
