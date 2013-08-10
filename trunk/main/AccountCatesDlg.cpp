
#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountCatesDlg.h"
#include "AccountCateEditingDlg.h"
#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AccountCatesDlg dialog

AccountCatesDlg::AccountCatesDlg( CWnd * parent /*=NULL*/)
: Dialog(AccountCatesDlg::IDD, parent)
{
	//{{AFX_DATA_INIT(AccountCatesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void AccountCatesDlg::DoDataExchange(CDataExchange* pDX)
{
	Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AccountCatesDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP( AccountCatesDlg, Dialog )
	//{{AFX_MSG_MAP(AccountCatesDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CATES, OnListActivated)
	ON_NOTIFY(NM_RETURN, IDC_LIST_CATES, OnListActivated)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_CATES, OnListRClick)
	ON_COMMAND(ID_CATE_ADD, OnAdd)
	ON_COMMAND(ID_CATE_MODIFY, OnModify)
	ON_COMMAND(ID_CATE_DELETE, OnDelete)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI_RANGE(ID_CATE_MODIFY, ID_CATE_DELETE, OnUpdateModifyDeleteMenu)
END_MESSAGE_MAP()

void AccountCatesDlg::UpdateList( int flag /*= UPDATE_LOAD_DATA | UPDATE_LIST_ITEMS*/, long itemIndex /*= -1 */ )
{
	int count = 0;
	CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_CATES);
	
	if ( flag & UPDATE_LOAD_DATA )
	{
		if ( itemIndex == -1 )
		{
			count = LoadAccountCates( g_theApp.GetDatabase(), &m_cates );
		}
		else
		{
			// ��ID��ȡ��¼
			GetAccountCate( g_theApp.GetDatabase(), m_cates[itemIndex].m_id, &m_cates[itemIndex] );
		}
	}

	if ( flag & UPDATE_LIST_ITEMS )
	{
		if ( itemIndex == -1 )
		{
			if ( !( flag & UPDATE_LOAD_DATA ) ) // ���û�������������Ҫ�������ȡ������Ϣ
			{
				count = m_cates.GetSize();
			}
			// Clear all items
			lst.DeleteAllItems();
			int i;
			for ( i = 0; i < count; ++i )
			{
				lst.InsertItem( i, format( _T("%d"), m_cates[i].m_id ).c_str() );
				lst.SetItem( i, 1, LVIF_TEXT, m_cates[i].m_cateName, 0, 0, 0, 0 );
				lst.SetItem( i, 2, LVIF_TEXT, m_cates[i].m_typeName, 0, 0, 0, 0 );
				lst.SetItem( i, 3, LVIF_TEXT, m_cates[i].m_url, 0, 0, 0, 0 );
				lst.SetItem( i, 4, LVIF_TEXT, m_cates[i].m_startup, 0, 0, 0, 0 );
				lst.SetItem( i, 5, LVIF_TEXT, m_cates[i].m_keywords, 0, 0, 0, 0 );
			}
		}
		else
		{
			lst.SetItem( itemIndex, 0, LVIF_TEXT, format( _T("%d"), m_cates[itemIndex].m_id ).c_str(), 0, 0, 0, 0 );
			lst.SetItem( itemIndex, 1, LVIF_TEXT, m_cates[itemIndex].m_cateName, 0, 0, 0, 0 );
			lst.SetItem( itemIndex, 2, LVIF_TEXT, m_cates[itemIndex].m_typeName, 0, 0, 0, 0 );
			lst.SetItem( itemIndex, 3, LVIF_TEXT, m_cates[itemIndex].m_url, 0, 0, 0, 0 );
			lst.SetItem( itemIndex, 4, LVIF_TEXT, m_cates[itemIndex].m_startup, 0, 0, 0, 0 );
			lst.SetItem( itemIndex, 5, LVIF_TEXT, m_cates[itemIndex].m_keywords, 0, 0, 0, 0 );
		}
	}

}

void AccountCatesDlg::DoAdd( CWnd * parent )
{
	VERIFY_ONCE_DIALOG(onceEditingDlg);

	AccountCate cate;
	AccountCateEditingDlg editingDlg( parent, true, &cate );

	SetNullScopeOut setNullScopeOut( onceEditingDlg = &editingDlg );
	
	if ( IDOK == editingDlg.DoModal() )
	{
		int id;
		id = AddAccountCate( g_theApp.GetDatabase(), cate );
		if ( id != 0 )
		{
			CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_CATES);
			// ��list����һ��
			int itemIndex;
			itemIndex = lst.GetItemCount();
			lst.InsertItem( itemIndex, format( _T("%d"), id ).c_str() );
			
			// ���������һ��
			AccountCate tmp;
			tmp.m_id = id;
			m_cates.Add(tmp);

			UpdateList( UPDATE_LOAD_DATA | UPDATE_LIST_ITEMS, itemIndex );
			lst.SetItemState( itemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// AccountCatesDlg message handlers

BOOL AccountCatesDlg::OnInitDialog() 
{
	Dialog::OnInitDialog();

	CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_CATES);

	// ����ListView��ListCtrl����չ���
	lst.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	
	// ��ʼ���б�ͷ
	lst.InsertColumn( 0, _T("���"), LVCFMT_LEFT, 48 );
	lst.InsertColumn( 1, _T("����"), LVCFMT_LEFT, 96 );
	lst.InsertColumn( 2, _T("���"), LVCFMT_LEFT, 48 );
	lst.InsertColumn( 3, _T("URL"), LVCFMT_LEFT, 130 );
	lst.InsertColumn( 4, _T("����"), LVCFMT_LEFT, 48 );
	lst.InsertColumn( 5, _T("�ؼ���"), LVCFMT_LEFT, 120 );

	UpdateList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AccountCatesDlg::OnListActivated( NMHDR* pNMHDR, LRESULT* pResult )
{
	CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_CATES);
	int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
	if ( index == -1 )
	{
		return;
	}
	// ģ�����˵�����
	PostMessage( WM_COMMAND, MAKEWPARAM( ID_CATE_MODIFY, 0 ), 0 );

	*pResult = 0;
}

void AccountCatesDlg::OnListRClick( NMHDR* pNMHDR, LRESULT* pResult )
{
	CMenu menu;
	menu.LoadMenu(IDM_OPERATE_CATES);
	POINT pt;
	GetCursorPos(&pt);
	menu.GetSubMenu(0)->TrackPopupMenu( 0, pt.x, pt.y, this );

	*pResult = 0;
}

void AccountCatesDlg::OnAdd()
{
	DoAdd( GetOwner() );
}

void AccountCatesDlg::OnModify()
{
	VERIFY_ONCE_DIALOG(onceEditingDlg);

	CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_CATES);
	int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
	AccountCate newCate;
	int id = m_cates[index].m_id;
	newCate = m_cates[index];

	AccountCateEditingDlg editingDlg( GetOwner(), false, &newCate );
	SetNullScopeOut setNullScopeOut( onceEditingDlg = &editingDlg );
	if ( IDOK == editingDlg.DoModal() )
	{
		if ( ModifyAccountCate( g_theApp.GetDatabase(), id, newCate ) )
		{
			m_cates[index] = newCate;

			UpdateList( UPDATE_LIST_ITEMS, index );
			CString strId;
			strId.Format( _T("%d"), id );
			LVFINDINFO fi;
			fi.flags = LVFI_PARTIAL | LVFI_STRING;
			fi.psz = strId;
			int itemIndex = lst.FindItem(&fi);
			lst.SetItemState( itemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
			GetOwner()->PostMessage( WM_UPDATELIST_ALL, MainFrame::UpdateList_Main );
		}
	}
}

void AccountCatesDlg::OnDelete()
{
	if ( IDYES == GetOwner()->MessageBox( _T("�˲������ɻָ���ȷ��Ҫɾ����"), _T("ȷ��"), MB_YESNO ) )
	{
		CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_CATES);
		int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
		CString strId;
		strId = lst.GetItemText( index, 0 );
		if ( DeleteAccountCate( g_theApp.GetDatabase(), mixed( (LPCTSTR)strId ) ) )
		{
			lst.DeleteItem(index);

			m_cates.RemoveAt(index);

			index = index < lst.GetItemCount() ? index : lst.GetItemCount() - 1;
			lst.SetItemState( index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		}
	}
}

void AccountCatesDlg::OnUpdateModifyDeleteMenu( CCmdUI * pCmdUI )
{
	CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_CATES);
	int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
	pCmdUI->Enable( index != -1 );
}
