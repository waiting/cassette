
#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountTypeEditingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AccountTypeEditingDlg dialog
AccountTypeEditingDlg::AccountTypeEditingDlg( CWnd * parent, bool isAdd, AccountType * type )
: Dialog(AccountTypeEditingDlg::IDD, parent), m_isAdd(isAdd), m_type(type)
{
	//{{AFX_DATA_INIT(AccountTypeEditingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void AccountTypeEditingDlg::DoDataExchange(CDataExchange* pDX)
{
	Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AccountTypeEditingDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_TYPENAME, m_type->m_typeName);
	DDX_Text(pDX, IDC_EDIT_SAFERANK, m_type->m_safeRank);
}

BEGIN_MESSAGE_MAP(AccountTypeEditingDlg, Dialog)
	//{{AFX_MSG_MAP(AccountTypeEditingDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AccountTypeEditingDlg message handlers

BOOL AccountTypeEditingDlg::OnInitDialog() 
{
	Dialog::OnInitDialog();
	m_ToolTips.SetTipTextColor( RGB( 255, 96, 0 ) ); // 设置提示文本颜色
	// 设置标题
	SetWindowText( m_isAdd ? _T("添加账户类别...") : _T("修改账户类别...") );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AccountTypeEditingDlg::OnOK() 
{
	UpdateData(TRUE);

	EndDialog(IDOK);
}
