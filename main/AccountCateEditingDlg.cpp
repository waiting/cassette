
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

AccountCateEditingDlg::AccountCateEditingDlg( CWnd * parent, bool isAdd, winplus::Mixed * cateFields ) :
    Dialog( AccountCateEditingDlg::IDD, GetDesktopWindow() ),
    m_pWndParent(parent),
    m_isAdd(isAdd),
    m_cate(cateFields)
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
    AccountCate cate;
    cate = *m_cate;

    DDX_Text(pDX, IDC_EDIT_CATENAME, cate.m_cateName);
    DDX_Text(pDX, IDC_EDIT_CATEDESC, cate.m_cateDesc);
    DDX_CBIndex(pDX, IDC_COMBO_TYPES, m_typeIndex);
    DDX_Text(pDX, IDC_COMBO_TYPES, cate.m_typeName);
    DDX_Text(pDX, IDC_EDIT_URL, cate.m_url);
    DDX_Text(pDX, IDC_EDIT_ICOPATH, cate.m_icoPath);
    DDX_CBIndex(pDX, IDC_COMBO_STARTUP, m_startupIndex);
    DDX_Text(pDX, IDC_COMBO_STARTUP, cate.m_startup);
    DDX_Text(pDX, IDC_EDIT_KEYWORDS, cate.m_keywords);

    cate.assignTo( m_cate, "name,desc,type,url,icon,startup,keywords" );
}

BEGIN_MESSAGE_MAP(AccountCateEditingDlg, Dialog)
    //{{AFX_MSG_MAP(AccountCateEditingDlg)
    //}}AFX_MSG_MAP
    ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AccountCateEditingDlg message handlers

BOOL AccountCateEditingDlg::OnInitDialog()
{
    Dialog::OnInitDialog();

    m_ToolTips.SetTipTextColor( RGB( 255, 96, 0 ) ); // ������ʾ�ı���ɫ
    // ���ñ���
    SetWindowText( m_isAdd ? _T("����˻�����...") : _T("�޸��˻�����...") );
    int i, n;
    AccountTypeArray types;
    n = LoadAccountTypes( g_theApp.GetDatabase(), &types );
    CComboBox * pCboTypes = (CComboBox *)GetDlgItem(IDC_COMBO_TYPES);
    for ( i = 0; i < n; ++i )
    {
        pCboTypes->AddString(types[i].m_typeName);
    }

    CComboBox * pCboStartups = (CComboBox *)GetDlgItem(IDC_COMBO_STARTUP);
    CString startups[] = { _T("��վ"), _T("���"), _T("����") };
    ComboBoxLoadDataFromCArray( pCboStartups, startups );

    m_typeIndex = 0;
    m_startupIndex = 0;

    for ( i = 0; i < n; ++i )
    {
        if ( m_cate->get<winplus::String>("type") == (LPCTSTR)types[i].m_typeName )
        {
            m_typeIndex = i;
            break;
        }
    }

    for ( i = 0; i < countof(startups); ++i )
    {
        if ( m_cate->get<winplus::String>("startup") == (LPCTSTR)startups[i] )
        {
            m_startupIndex = i;
            break;
        }
    }

    UpdateData(FALSE);

    // ����
    CRect rc1, rc2;
    CPoint pt;
    if ( Window_CalcCenter( *this, *m_pWndParent, false, &rc1, &rc2, &pt ) )
        Window_Center( *this, *m_pWndParent );
    else
        Window_Center( *this, NULL );

    this->SetForegroundWindow();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void AccountCateEditingDlg::OnOK() 
{
    UpdateData(TRUE);
    if ( (*m_cate)["name"].refAnsi().empty() )
    {
        WarningError( _T("���Ʋ���Ϊ��"), _T("����") );
        return;
    }
    if ( (*m_cate)["keywords"].refAnsi().empty() )
    {
        WarningError( _T("�ؼ��ֲ���Ϊ��"), _T("����") );
        return;
    }
    EndDialog(IDOK);
}

void AccountCateEditingDlg::OnShowWindow( BOOL bShow, UINT nStatus )
{
    // �ö�
    this->SetWindowPos( &wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
}
