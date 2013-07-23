
#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountTypesDlg.h"
#include "AccountTypeEditingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AccountTypesDlg dialog

AccountTypesDlg::AccountTypesDlg(CWnd* pParent /*=NULL*/)
: Dialog(AccountTypesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AccountTypesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void AccountTypesDlg::DoDataExchange(CDataExchange* pDX)
{
	Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AccountTypesDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(AccountTypesDlg, Dialog)
	//{{AFX_MSG_MAP(AccountTypesDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TYPES, OnListActivated)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_TYPES, OnListRClick)
	ON_COMMAND(ID_TYPE_ADD, OnAdd)
	ON_COMMAND(ID_TYPE_MODIFY, OnModify)
	ON_COMMAND(ID_TYPE_DELETE, OnDelete)
	ON_NOTIFY(NM_RETURN, IDC_LIST_TYPES, OnListActivated)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI_RANGE(ID_TYPE_MODIFY, ID_TYPE_DELETE, OnUpdateModifyDeleteMenu)
END_MESSAGE_MAP()

void AccountTypesDlg::UpdateList( int flag, long itemIndex )
{
	int count = 0;
	CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_TYPES);
	
	if ( flag & UPDATE_LOAD_DATA )
	{
		if ( itemIndex == -1 )
		{
			count = LoadAccountTypes(
				&m_typeNames,
				&m_safeRanks
			);
		}
		else
		{
			GetAccountType(
				m_typeNames[itemIndex],
				(int *)&m_safeRanks[itemIndex]
			);
		}
	}
	
	if ( flag & UPDATE_LIST_ITEMS )
	{
		if ( itemIndex == -1 )
		{
			if ( !( flag & UPDATE_LOAD_DATA ) ) // ���û�������������Ҫ�������ȡ������Ϣ
			{
				count = m_typeNames.GetSize();
			}
			// Clear all items
			lst.DeleteAllItems();
			int i;
			for ( i = 0; i < count; ++i )
			{
				lst.InsertItem( i, m_typeNames[i] );
				lst.SetItem( i, 1, LVIF_TEXT, format( _T("%d"), m_safeRanks[i] ).c_str(), 0, 0, 0, 0 );
			}
		}
		else
		{
			lst.SetItem( itemIndex, 0, LVIF_TEXT, m_typeNames[itemIndex], 0, 0, 0, 0 );
			lst.SetItem( itemIndex, 1, LVIF_TEXT, format( _T("%d"), m_safeRanks[itemIndex] ).c_str(), 0, 0, 0, 0 );
		}
	}

}
/////////////////////////////////////////////////////////////////////////////
// AccountTypesDlg message handlers

BOOL AccountTypesDlg::OnInitDialog()
{
	Dialog::OnInitDialog();
	m_ToolTips.SetTipTextColor( RGB( 255, 96, 0 ) ); // ������ʾ�ı���ɫ

	CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_TYPES);

	// ����ListView��ListCtrl����չ���
	lst.SetExtendedStyle( LVS_EX_FULLROWSELECT );

	// ��ʼ���б�ͷ
	lst.InsertColumn( 0, _T("�������"), LVCFMT_LEFT, 100 );
	lst.InsertColumn( 1, _T("��ȫȨֵ"), LVCFMT_LEFT, 100 );

	UpdateList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AccountTypesDlg::OnListActivated( NMHDR * pNMHDR, LRESULT * pResult ) 
{
	CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_TYPES);
	int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
	if ( index == -1 )
	{
		return;
	}
	// ģ�����˵�����
	PostMessage( WM_COMMAND, MAKEWPARAM( ID_TYPE_MODIFY, 0 ), 0 );

	*pResult = 0;
}

void AccountTypesDlg::OnListRClick( NMHDR * pNMHDR, LRESULT * pResult ) 
{
	CMenu menu;
	menu.LoadMenu(IDM_OPERATE_TYPES);
	POINT pt;
	GetCursorPos(&pt);
	menu.GetSubMenu(0)->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this );

	*pResult = 0;
}

void AccountTypesDlg::OnAdd() 
{
	VERIFY_ONCE_DIALOG(onceEditingDlg);

	CString typeName;
	int safeRank = 20;

	AccountTypeEditingDlg editingDlg( this, true, &typeName, &safeRank );
	SetNullScopeOut setNullScopeOut( onceEditingDlg = &editingDlg );
	if ( IDOK == editingDlg.DoModal() )
	{
		if ( AddAccountType( typeName, safeRank ) )
		{
			CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_TYPES);
			// ��list����һ��
			int itemIndex;
			itemIndex = lst.GetItemCount();
			lst.InsertItem( itemIndex, typeName );

			// ���������һ��
			m_typeNames.Add(typeName);
			m_safeRanks.Add(0);

			UpdateList( UPDATE_LOAD_DATA | UPDATE_LIST_ITEMS, itemIndex );
			lst.SetItemState( itemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		}
	}
}

void AccountTypesDlg::OnModify() 
{
	VERIFY_ONCE_DIALOG(onceEditingDlg);

	CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_TYPES);
	int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
	CString typeName = m_typeNames[index];
	CString newTypeName = m_typeNames[index];
	int newSafeRank = m_safeRanks[index];

	AccountTypeEditingDlg editingDlg( this, false, &newTypeName, &newSafeRank );
	SetNullScopeOut setNullScopeOut( onceEditingDlg = &editingDlg );
	if ( IDOK == editingDlg.DoModal() )
	{
		if ( ModifyAccountType( typeName, newTypeName, newSafeRank ) )
		{
			m_typeNames[index] = newTypeName;
			m_safeRanks[index] = newSafeRank;

			UpdateList( UPDATE_LIST_ITEMS, index );
			LVFINDINFO fi;
			fi.flags = LVFI_PARTIAL | LVFI_STRING;
			fi.psz = newTypeName;
			int itemIndex = lst.FindItem(&fi);
			lst.SetItemState( itemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		}
	}
}

void AccountTypesDlg::OnDelete() 
{
	if ( IDYES == MessageBox( _T("�˲������ɻָ���ȷ��Ҫɾ����"), _T("ȷ��"), MB_YESNO ) )
	{
		CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_TYPES);
		int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
		CString typeName;
		typeName = lst.GetItemText( index, 0 );
		if ( DeleteAccountType(typeName) )
		{
			lst.DeleteItem(index);

			m_typeNames.RemoveAt(index);
			m_safeRanks.RemoveAt(index);

			index = index < lst.GetItemCount() ? index : lst.GetItemCount() - 1;
			lst.SetItemState( index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		}
	}
}

void AccountTypesDlg::OnUpdateModifyDeleteMenu( CCmdUI * pCmdUI )
{
	CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_TYPES);
	int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
	pCmdUI->Enable( index != -1 );
}
