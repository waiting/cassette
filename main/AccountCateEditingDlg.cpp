
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

AccountCateEditingDlg::AccountCateEditingDlg(
	CWnd * parent,
	bool isAdd,
	CString * cateName,
	CString * cateDesc,
	CString * typeName,
	CString * url,
	CString * icoPath,
	CString * startup,
	CString * keywords
) : Dialog(AccountCateEditingDlg::IDD, parent),
m_isAdd(isAdd),
m_cateName(cateName),
m_cateDesc(cateDesc),
m_typeName(typeName),
m_url(url),
m_icoPath(icoPath),
m_startup(startup),
m_keywords(keywords)
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
	DDX_Text(pDX, IDC_EDIT_CATENAME, *m_cateName);
	DDX_Text(pDX, IDC_EDIT_CATEDESC, *m_cateDesc);
	DDX_CBIndex(pDX, IDC_COMBO_TYPES, m_typeIndex);
	DDX_Text(pDX, IDC_COMBO_TYPES, *m_typeName);
	DDX_Text(pDX, IDC_EDIT_URL, *m_url);
	DDX_Text(pDX, IDC_EDIT_ICOPATH, *m_icoPath);
	DDX_CBIndex(pDX, IDC_COMBO_STARTUP, m_startupIndex);
	DDX_Text(pDX, IDC_COMBO_STARTUP, *m_startup);
	DDX_Text(pDX, IDC_EDIT_KEYWORDS, *m_keywords);
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

	m_ToolTips.SetTipTextColor( RGB( 255, 96, 0 ) ); // ������ʾ�ı���ɫ
	// ���ñ���
	SetWindowText( m_isAdd ? _T("����˻�����...") : _T("�޸��˻�����...") );

	CStringArray typeNames;
	int n = LoadAccountTypes( &typeNames, NULL );
	CComboBox * pCboTypes = (CComboBox *)GetDlgItem(IDC_COMBO_TYPES);
	ComboBoxLoadDataFromArray( pCboTypes, typeNames );

	CComboBox * pCboStartups = (CComboBox *)GetDlgItem(IDC_COMBO_STARTUP);
	CString startups[] = { _T("��վ"), _T("���"), _T("����") };
	ComboBoxLoadDataFromCArray( pCboStartups, startups );

	m_typeIndex = 0;
	m_startupIndex = 0;

	int i;
	for ( i = 0; i < typeNames.GetSize(); ++i )
	{
		if ( *m_typeName == typeNames[i] )
		{
			m_typeIndex = i;
			break;
		}
	}

	for ( i = 0; i < countof(startups); ++i )
	{
		if ( *m_startup == startups[i] )
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
	if ( m_cateName->IsEmpty() )
	{
		WarningError( _T("���Ʋ���Ϊ��"), _T("����") );
		return;
	}
	if ( m_keywords->IsEmpty() )
	{
		WarningError( _T("�ؼ��ֲ���Ϊ��"), _T("����") );
		return;
	}
	EndDialog(IDOK);
}
