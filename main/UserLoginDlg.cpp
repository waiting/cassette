
#include "Cassette.h"
#include "CassetteApp.h"
#include "UserLoginDlg.h"
#include "UserRegisterDlg.h"
#include "ForgetPwdStep1Dlg.h"

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
	CString username, password;
	int protectLevel = 0;
	int hotkey = VK_F11;
	UserRegisterDlg registerDlg( this, &username, &password, &protectLevel, &hotkey );

	while ( IDOK == registerDlg.DoModal() )
	{
		if ( RegisterUser( username, password, protectLevel, hotkey ) )
		{
			MessageBox( _T("�û�ע��ɹ�"), _T("��ʾ"), MB_ICONINFORMATION );
			break;
		}
	}

	*pResult = 0;
}

void UserLoginDlg::OnSysLinkForgetPwd( NMHDR * pNmHdr, LRESULT * pResult )
{
	ForgetPwdStep1Dlg dlg(this);
	dlg.DoModal();

	*pResult = 0;
}

BOOL UserLoginDlg::OnInitDialog() 
{
	Dialog::OnInitDialog();

	m_ToolTips.SetTipTextColor( RGB( 255, 96, 0 ) ); // ������ʾ�ı���ɫ
	// �Ѵ��ھ���ύ�������ڴ�,�Ա㼤��
	g_theApp.GetSharedMemory()->hMainWnd = this->GetSafeHwnd();
	// �޸�logo�ı���ʽ
	m_lblLogin.SetColor( RGB( 0, 128, 255 ) );
	m_lblLogin.SetFontSize(220);
	m_lblLogin.ApplyChange();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void UserLoginDlg::OnOK() 
{
	UpdateData(TRUE);

	if ( m_username->IsEmpty() )
	{
		WarningError( _T("�û�������Ϊ��"), _T("����") );
		return;
	}

	/*if ( !VerifyUserPassword( *m_username, *m_password ) )
	{
		WarningError( _T("���벻��ȷ"), _T("����") );
		return;
	}*/

	EndDialog(IDOK);
}
