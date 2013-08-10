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
UserRegisterDlg::UserRegisterDlg( CWnd * parent, User * user )
: Dialog(UserRegisterDlg::IDD, parent), m_user(user)
{
	//{{AFX_DATA_INIT(UserRegisterDlg)
	m_cfmPassword = _T("");
	//}}AFX_DATA_INIT
}

void UserRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UserRegisterDlg)
	DDX_Text(pDX, IDC_EDIT_CFMPWD, m_cfmPassword);
	//}}AFX_DATA_MAP
	DDX_Text( pDX, IDC_EDIT_USERNAME, m_user->m_username );
	DDX_Text( pDX, IDC_EDIT_PASSWORD, m_user->m_password );
	DDX_CBIndex( pDX, IDC_COMBO_PROTECTLEVEL, m_user->m_protectLevel );
}

BEGIN_MESSAGE_MAP(UserRegisterDlg, Dialog)
	//{{AFX_MSG_MAP(UserRegisterDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UserRegisterDlg message handlers

BOOL UserRegisterDlg::OnInitDialog() 
{
	Dialog::OnInitDialog();
	m_ToolTips.SetTipTextColor( RGB( 255, 96, 0 ) ); // ������ʾ�ı���ɫ
	//��ʼ������
	CComboBox * pCboProtectLevel = (CComboBox *)this->GetDlgItem(IDC_COMBO_PROTECTLEVEL);
	CString protectLevels[] = { _T("һ��"), _T("ǿ��"), _T("����") };
	ComboBoxLoadDataFromCArray( pCboProtectLevel, protectLevels );

	UpdateData(FALSE);
	// �����ȼ�ֵ
	CHotKeyCtrl * pHkCtrl = (CHotKeyCtrl *)this->GetDlgItem(IDC_HOTKEY_CUSTOM);
	WORD m, vk;
	m = HIWORD(m_user->m_hotkey);
	m = MOD_to_HOTKEYF(m);
	vk = LOWORD(m_user->m_hotkey);
	pHkCtrl->SetHotKey( vk, m );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void UserRegisterDlg::OnOK() 
{
	UpdateData(TRUE);
	// ��ȡ�ȼ�ֵ
	CHotKeyCtrl * pHkCtrl = (CHotKeyCtrl *)this->GetDlgItem(IDC_HOTKEY_CUSTOM);
	WORD m, vk;
	pHkCtrl->GetHotKey( vk, m );
	m = HOTKEYF_to_MOD(m);
	m_user->m_hotkey = MAKELONG( vk, m );

	if ( m_user->m_username.IsEmpty() )
	{
		WarningError( _T("�û�������Ϊ��"), _T("����") );
		return;
	}

	if ( m_user->m_password != m_cfmPassword )
	{
		WarningError( _T("ȷ�����벻һ��"), _T("����") );
		return;
	}

	EndDialog(IDOK);
}
