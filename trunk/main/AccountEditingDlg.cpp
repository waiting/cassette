
#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountEditingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AccountEditingDlg dialog
AccountEditingDlg::AccountEditingDlg( CWnd * parent, bool isAdd, Account * account )
: Dialog( AccountEditingDlg::IDD, parent ), m_isAdd(isAdd), m_account(account)
{
	//{{AFX_DATA_INIT(AccountEditingDlg)
	//}}AFX_DATA_INIT
}

void AccountEditingDlg::DoDataExchange( CDataExchange * pDX )
{
	Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AccountEditingDlg)

	//}}AFX_DATA_MAP
	DDX_CBIndex(pDX, IDC_COMBO_CATES, m_cateIndex);
	DDX_Text(pDX, IDC_EDIT_MYNAME, m_account->m_myName);
	DDX_Text(pDX, IDC_EDIT_ACCOUNTNAME, m_account->m_accountName);
	DDX_Text(pDX, IDC_EDIT_ACCOUNTPWD, m_account->m_accountPwd);
	DDX_Text(pDX, IDC_EDIT_SAFERANK, m_account->m_safeRank);
	DDX_Text(pDX, IDC_EDIT_COMMENT, m_account->m_comment);
}

BEGIN_MESSAGE_MAP(AccountEditingDlg, Dialog)
	//{{AFX_MSG_MAP(AccountEditingDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_CATES, OnSelChangeComboCates)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int AccountEditingDlg::GetSafeRankByCateId( int cateId ) const
{
	int count = (int)m_cateIds2.GetSize();
	int i;
	for ( i = 0; i < count; ++i )
	{
		if ( cateId == m_cateIds2[i] )
		{
			return m_typeSafeRanks[i];
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// AccountEditingDlg message handlers

BOOL AccountEditingDlg::OnInitDialog() 
{
	Dialog::OnInitDialog();
	m_ToolTips.SetTipTextColor( RGB( 255, 96, 0 ) ); // 设置提示文本颜色

	// 设置标题
	SetWindowText( m_isAdd ? _T("添加账户...") : _T("修改账户...") );

	// 载入种类信息
	int catesCount;
	catesCount = LoadAccountCates( g_theApp.GetDatabase(), &m_cates );
	LoadAccountCatesSafeRank( g_theApp.GetDatabase(), &m_cateIds2, &m_typeSafeRanks );
	// 内部的选择框是通过索引来确定选择项的，因此外部传递的CateID需要转换为索引
	// 这样才能选中相应的Cate
	m_cateIndex = -1;
	int i;
	// ID to Index
	for ( i = 0; i < catesCount; ++i )
	{
		if ( m_cates[i].m_id == m_account->m_cateId )
		{
			m_cateIndex = i;
			break;
		}
	}
	// Add data to combobox
	CComboBox * pCboCates = (CComboBox *)GetDlgItem(IDC_COMBO_CATES);
	for ( i = 0; i < catesCount; ++i )
	{
		pCboCates->AddString(m_cates[i].m_cateName);
	}

	UpdateData(FALSE);

	if ( m_isAdd && m_cateIndex != -1 ) // 如果选择了种类,触发组合框选择改变事件
		SendMessage( WM_COMMAND, MAKEWPARAM( IDC_COMBO_CATES, CBN_SELCHANGE ), (LPARAM)pCboCates->GetSafeHwnd() );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AccountEditingDlg::OnOK() 
{
	UpdateData(TRUE);

	if ( m_account->m_myName.IsEmpty() )
	{
		WarningError( _T("名称不能为空"), _T("错误") );
		return;
	}

	if ( m_cateIndex == -1 )
	{
		WarningError( _T("必须选择一个账户种类"), _T("错误") );
		return;
	}
	// Index to Id
	m_account->m_cateId = m_cates[m_cateIndex].m_id;

	EndDialog(IDOK);
}

void AccountEditingDlg::OnSelChangeComboCates() 
{
	if ( !m_isAdd ) return; // 只有添加账户时需要此功能
	UpdateData(TRUE);
	CComboBox * pCboCates = (CComboBox *)GetDlgItem(IDC_COMBO_CATES);
	m_account->m_safeRank = GetSafeRankByCateId(m_cates[m_cateIndex].m_id);
	m_account->m_comment = m_cates[m_cateIndex].m_cateDesc;
	CString tmpMyName;
	pCboCates->GetLBText( m_cateIndex, tmpMyName );
	tmpMyName = _T("我的") + tmpMyName + _T("账户");
	m_account->m_myName = GetCorrectAccountMyName( g_theApp.GetDatabase(), g_theApp.m_loginedUser.m_id, tmpMyName );
	UpdateData(FALSE);
}
