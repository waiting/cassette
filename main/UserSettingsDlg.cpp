
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

UserSettingsDlg::UserSettingsDlg( CWnd * parent, winux::Mixed * userFields ) :
    Dialog( UserSettingsDlg::IDD, parent ),
    m_userFields(*userFields)
{
    //{{AFX_DATA_INIT(UserSettingsDlg)
    m_oldPassword = _T("");
    m_newPassword = _T("");
    m_cfmPassword = _T("");
    //}}AFX_DATA_INIT

}

void UserSettingsDlg::DoDataExchange( CDataExchange * pDX )
{
    Dialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(UserSettingsDlg)
    DDX_Text(pDX, IDC_EDIT_OLDPWD, m_oldPassword);
    DDX_Text(pDX, IDC_EDIT_NEWPWD, m_newPassword);
    DDX_Text(pDX, IDC_EDIT_CFMPWD, m_cfmPassword);
    //}}AFX_DATA_MAP

    CString username;
    int protectLevel, condone;

    username = m_userFields["name"].refAnsi().c_str();
    protectLevel = m_userFields["protect"];
    condone = m_userFields["condone"];

    DDX_Text( pDX, IDC_EDIT_USERNAME, username );
    DDX_CBIndex( pDX, IDC_COMBO_PROTECTLEVEL, protectLevel );
    DDX_Text( pDX, IDC_EDIT_CONDONE, condone );

    m_userFields["name"] = (LPCTSTR)username;
    m_userFields["protect"] = protectLevel;
    m_userFields["condone"] = condone;
}

BEGIN_MESSAGE_MAP(UserSettingsDlg, Dialog)
    //{{AFX_MSG_MAP(UserSettingsDlg)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

bool UserSettingsDlg::IsModifyPassword() const
{
    return !( m_oldPassword.IsEmpty() && m_newPassword.IsEmpty() && m_cfmPassword.IsEmpty() );
}

/////////////////////////////////////////////////////////////////////////////
// UserSettingsDlg message handlers

BOOL UserSettingsDlg::OnInitDialog()
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
    m = HIWORD(m_userFields["hotkey"]);
    m = winplus::MOD_to_HOTKEYF(m);
    vk = LOWORD(m_userFields["hotkey"]);
    pHkCtrl->SetHotKey( vk, m );

    return TRUE;
}

void UserSettingsDlg::OnOK()
{
    UpdateData(TRUE); // �����TRUE,�ӿؼ���ȡ���ݵ�����;FALSE,�ӱ����������ݵ��ؼ���ʾ
    // ��ȡ�ȼ�ֵ
    CHotKeyCtrl * pHkCtrl = (CHotKeyCtrl *)this->GetDlgItem(IDC_HOTKEY_CUSTOM);
    WORD m, vk;
    pHkCtrl->GetHotKey( vk, m );
    m = winplus::HOTKEYF_to_MOD(m);
    m_userFields["hotkey"] = MAKELONG( vk, m );
    // ���봦��
    if ( this->IsModifyPassword() ) // ȷʵ�޸�����
    {
        // ��֤������
        if ( !VerifyUserPassword( g_theApp.GetDatabase(), g_theApp.m_loginedUser.m_username, m_oldPassword ) )
        {
            WarningError( _T("�����벻��!"), _T("����") );
            return;
        }
        if ( m_newPassword != m_cfmPassword )
        {
            WarningError( _T("ȷ�����벻һ��!"), _T("����") );
            return;
        }
        m_userFields["pwd"] = EncryptContent( (LPCTSTR)m_newPassword );
    }

    this->EndDialog(IDOK);
}
