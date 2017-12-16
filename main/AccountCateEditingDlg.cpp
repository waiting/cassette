
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

AccountCateEditingDlg::AccountCateEditingDlg( CWnd * parent, bool isAdd, AccountCate * cate )
: Dialog(AccountCateEditingDlg::IDD, parent), m_isAdd(isAdd), m_cate(cate)
{
    //{{AFX_DATA_INIT(AccountCateEditingDlg)
    //}}AFX_DATA_INIT
    m_typeIndex = -1;
    m_startupIndex = -1;
}

void AccountCateEditingDlg::DoDataExchange(CDataExchange* pDX)
{
    Dialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(AccountCateEditingDlg)
    //}}AFX_DATA_MAP
    DDX_Text(pDX, IDC_EDIT_CATENAME, m_cate->m_cateName);
    DDX_Text(pDX, IDC_EDIT_CATEDESC, m_cate->m_cateDesc);
    DDX_CBIndex(pDX, IDC_COMBO_TYPES, m_typeIndex);
    DDX_Text(pDX, IDC_COMBO_TYPES, m_cate->m_typeName);
    DDX_Text(pDX, IDC_EDIT_URL, m_cate->m_url);
    DDX_Text(pDX, IDC_EDIT_ICOPATH, m_cate->m_icoPath);
    DDX_CBIndex(pDX, IDC_COMBO_STARTUP, m_startupIndex);
    DDX_Text(pDX, IDC_COMBO_STARTUP, m_cate->m_startup);
    DDX_Text(pDX, IDC_EDIT_KEYWORDS, m_cate->m_keywords);
}

BEGIN_MESSAGE_MAP(AccountCateEditingDlg, Dialog)
    //{{AFX_MSG_MAP(AccountCateEditingDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AccountCateEditingDlg message handlers

BOOL AccountCateEditingDlg::OnInitDialog() 
{
    Dialog::OnInitDialog();

    m_ToolTips.SetTipTextColor( RGB( 255, 96, 0 ) ); // 设置提示文本颜色
    // 设置标题
    SetWindowText( m_isAdd ? _T("添加账户种类...") : _T("修改账户种类...") );
    int i, n;
    AccountTypeArray types;
    n = LoadAccountTypes( g_theApp.GetDatabase(), &types );
    CComboBox * pCboTypes = (CComboBox *)GetDlgItem(IDC_COMBO_TYPES);
    for ( i = 0; i < n; ++i )
    {
        pCboTypes->AddString(types[i].m_typeName);
    }

    CComboBox * pCboStartups = (CComboBox *)GetDlgItem(IDC_COMBO_STARTUP);
    CString startups[] = { _T("网站"), _T("软件"), _T("其他") };
    ComboBoxLoadDataFromCArray( pCboStartups, startups );

    m_typeIndex = 0;
    m_startupIndex = 0;

    for ( i = 0; i < n; ++i )
    {
        if ( m_cate->m_typeName == types[i].m_typeName )
        {
            m_typeIndex = i;
            break;
        }
    }

    for ( i = 0; i < countof(startups); ++i )
    {
        if ( m_cate->m_startup == startups[i] )
        {
            m_startupIndex = i;
            break;
        }
    }

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void AccountCateEditingDlg::OnOK() 
{
    UpdateData(TRUE);
    if ( m_cate->m_cateName.IsEmpty() )
    {
        WarningError( _T("名称不能为空"), _T("错误") );
        return;
    }
    if ( m_cate->m_keywords.IsEmpty() )
    {
        WarningError( _T("关键字不能为空"), _T("错误") );
        return;
    }
    EndDialog(IDOK);
}
