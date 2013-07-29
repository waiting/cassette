
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
AccountEditingDlg::AccountEditingDlg(
	CWnd * parent,
	bool isAdd,
	CString * myName,
	CString * accountName,
	CString * accountPwd,
	int * cateId,
	int * safeRank,
	CString * comment
)
:
Dialog( AccountEditingDlg::IDD, parent ),
m_isAdd(isAdd),
m_myName(myName),
m_accountName(accountName),
m_accountPwd(accountPwd),
m_cateId(cateId),
m_safeRank(safeRank),
m_comment(comment)
{
	//{{AFX_DATA_INIT(AccountEditingDlg)

	//}}AFX_DATA_INIT
}

void AccountEditingDlg::DoDataExchange(CDataExchange* pDX)
{
	Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AccountEditingDlg)

	//}}AFX_DATA_MAP
	DDX_CBIndex(pDX, IDC_COMBO_CATES, m_cateIndex);
	DDX_Text(pDX, IDC_EDIT_ACCOUNTNAME, *m_accountName);
	DDX_Text(pDX, IDC_EDIT_ACCOUNTPWD, *m_accountPwd);
	DDX_Text(pDX, IDC_EDIT_COMMENT, *m_comment);
	DDX_Text(pDX, IDC_EDIT_MYNAME, *m_myName);
	DDX_Text(pDX, IDC_EDIT_SAFERANK, *m_safeRank);
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
	CStringArray cateNames;
	int catesCount;
	catesCount = LoadAccountCates( &m_cateIds, &cateNames, NULL, NULL, NULL, NULL, NULL, NULL, NULL );
	LoadAccountCatesSafeRank( &m_cateIds2, &m_typeSafeRanks );
	// 内部的选择框是通过索引来确定选择项的，因此外部传递的CateID需要转换为索引
	// 这样才能选中相应的Cate
	m_cateIndex = -1;
	int i;
	// ID to Index
	for ( i = 0; i < catesCount; ++i )
	{
		if ( m_cateIds[i] == *m_cateId )
		{
			m_cateIndex = i;
			break;
		}
	}
	// Add data to combobox
	CComboBox * pCboCates = (CComboBox *)GetDlgItem(IDC_COMBO_CATES);
	for ( i = 0; i < catesCount; ++i )
	{
		pCboCates->AddString(cateNames[i]);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AccountEditingDlg::OnOK() 
{
	UpdateData(TRUE);

	if ( m_myName->IsEmpty() )
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
	*m_cateId = m_cateIds[m_cateIndex];

	this->EndDialog(IDOK);
}

void AccountEditingDlg::OnSelChangeComboCates() 
{
	if ( !m_isAdd ) return; // 只有添加账户时需要此功能
	UpdateData(TRUE);
	CComboBox * pCboCates = (CComboBox *)GetDlgItem(IDC_COMBO_CATES);
	*m_safeRank = GetSafeRankByCateId(m_cateIds[m_cateIndex]);
	CString tmpMyName;
	pCboCates->GetLBText( m_cateIndex, tmpMyName );
	tmpMyName = _T("我的") + tmpMyName + _T("账户");
	*m_myName = GetCorrectAccountMyName(tmpMyName);
	UpdateData(FALSE);
}
