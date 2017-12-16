
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

UserSettingsDlg::UserSettingsDlg( CWnd * parent, User * user )
: Dialog( UserSettingsDlg::IDD, parent ), m_user(user)
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
    DDX_Text( pDX, IDC_EDIT_USERNAME, m_user->m_username );
    DDX_CBIndex( pDX, IDC_COMBO_PROTECTLEVEL, m_user->m_protectLevel );
    DDX_Text( pDX, IDC_EDIT_CONDONE, m_user->m_condone );
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
    m_ToolTips.SetTipTextColor( RGB( 255, 96, 0 ) ); // 设置提示文本颜色

    //初始化数据
    CComboBox * pCboProtectLevel = (CComboBox *)this->GetDlgItem(IDC_COMBO_PROTECTLEVEL);
    CString protectLevels[] = { _T("一般"), _T("强力"), _T("绝对") };
    ComboBoxLoadDataFromCArray( pCboProtectLevel, protectLevels );

    UpdateData(FALSE);
    // 设置热键值
    CHotKeyCtrl * pHkCtrl = (CHotKeyCtrl *)this->GetDlgItem(IDC_HOTKEY_CUSTOM);
    WORD m, vk;
    m = HIWORD(m_user->m_hotkey);
    m = winplus::MOD_to_HOTKEYF(m);
    vk = LOWORD(m_user->m_hotkey);
    pHkCtrl->SetHotKey( vk, m );

    return TRUE;
}

void UserSettingsDlg::OnOK()
{
    UpdateData(TRUE); // 如果是TRUE,从控件获取数据到变量;FALSE,从变量传递数据到控件显示
    // 获取热键值
    CHotKeyCtrl * pHkCtrl = (CHotKeyCtrl *)this->GetDlgItem(IDC_HOTKEY_CUSTOM);
    WORD m, vk;
    pHkCtrl->GetHotKey( vk, m );
    m = winplus::HOTKEYF_to_MOD(m);
    m_user->m_hotkey = MAKELONG( vk, m );
    // 密码处理
    if ( this->IsModifyPassword() ) // 确实修改密码
    {
        // 验证旧密码
        if ( !VerifyUserPassword( g_theApp.GetDatabase(), g_theApp.m_loginedUser.m_username, m_oldPassword ) )
        {
            WarningError( _T("旧密码不对!"), _T("错误") );
            return;
        }
        if ( m_newPassword != m_cfmPassword )
        {
            WarningError( _T("确认密码不一样!"), _T("错误") );
            return;
        }
        m_user->m_password = m_newPassword;
    }

    this->EndDialog(IDOK);
}
