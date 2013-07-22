
#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountCatesDlg.h"
#include "AccountCateEditingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AccountCatesDlg dialog

AccountCatesDlg::AccountCatesDlg(CWnd* pParent /*=NULL*/)
: Dialog(AccountCatesDlg::IDD, pParent)
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

BEGIN_MESSAGE_MAP(AccountCatesDlg, Dialog)
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
			count = LoadAccountCates(
				&m_ids,
				&m_cateNames,
				&m_cateDescs,
				&m_typeNames,
				&m_urls,
				&m_icoPaths,
				&m_startups,
				&m_keywordss,
				&m_timeWritens
			);
		}
		else
		{
			// 用ID获取记录
			GetAccountCate(
				m_ids[itemIndex],
				&m_cateNames[itemIndex],
				&m_cateDescs[itemIndex],
				&m_typeNames[itemIndex],
				&m_urls[itemIndex],
				&m_icoPaths[itemIndex],
				&m_startups[itemIndex],
				&m_keywordss[itemIndex],
				(int *)&m_timeWritens[itemIndex]
			);
		}
	}
	
	if ( flag & UPDATE_LIST_ITEMS )
	{
		if ( itemIndex == -1 )
		{
			if ( !( flag & UPDATE_LOAD_DATA ) ) // 如果没有载入操作，需要从数组获取数量信息
			{
				count = m_ids.GetSize();
			}
			// Clear all items
			lst.DeleteAllItems();
			int i;
			for ( i = 0; i < count; ++i )
			{
				lst.InsertItem( i, format( _T("%d"), m_ids[i] ).c_str() );
				lst.SetItem( i, 1, LVIF_TEXT, m_cateNames[i], 0, 0, 0, 0 );
				lst.SetItem( i, 2, LVIF_TEXT, m_typeNames[i], 0, 0, 0, 0 );
				lst.SetItem( i, 3, LVIF_TEXT, m_urls[i], 0, 0, 0, 0 );
				lst.SetItem( i, 4, LVIF_TEXT, m_startups[i], 0, 0, 0, 0 );
				lst.SetItem( i, 5, LVIF_TEXT, m_keywordss[i], 0, 0, 0, 0 );
			}
		}
		else
		{
			lst.SetItem( itemIndex, 0, LVIF_TEXT, format( _T("%d"), m_ids[itemIndex] ).c_str(), 0, 0, 0, 0 );
			lst.SetItem( itemIndex, 1, LVIF_TEXT, m_cateNames[itemIndex], 0, 0, 0, 0 );
			lst.SetItem( itemIndex, 2, LVIF_TEXT, m_typeNames[itemIndex], 0, 0, 0, 0 );
			lst.SetItem( itemIndex, 3, LVIF_TEXT, m_urls[itemIndex], 0, 0, 0, 0 );
			lst.SetItem( itemIndex, 4, LVIF_TEXT, m_startups[itemIndex], 0, 0, 0, 0 );
			lst.SetItem( itemIndex, 5, LVIF_TEXT, m_keywordss[itemIndex], 0, 0, 0, 0 );
		}
	}

}

/////////////////////////////////////////////////////////////////////////////
// AccountCatesDlg message handlers

BOOL AccountCatesDlg::OnInitDialog() 
{
	Dialog::OnInitDialog();

	CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_CATES);

	// 设置ListView中ListCtrl的扩展风格
	lst.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	
	// 初始化列表头
	lst.InsertColumn( 0, _T("编号"), LVCFMT_LEFT, 48 );
	lst.InsertColumn( 1, _T("名称"), LVCFMT_LEFT, 96 );
	lst.InsertColumn( 2, _T("类别"), LVCFMT_LEFT, 48 );
	lst.InsertColumn( 3, _T("URL"), LVCFMT_LEFT, 130 );
	lst.InsertColumn( 4, _T("启动"), LVCFMT_LEFT, 48 );
	lst.InsertColumn( 5, _T("关键字"), LVCFMT_LEFT, 120 );

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
	// 模拟点击菜单命令
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
	CString cateName, cateDesc, typeName, url, icoPath, startup, keywords;
	AccountCateEditingDlg editingDlg(
		this,
		true,
		&cateName,
		&cateDesc,
		&typeName,
		&url,
		&icoPath,
		&startup,
		&keywords
	);
	if ( IDOK == editingDlg.DoModal() )
	{
		int id;
		id = AddAccountCate( cateName, cateDesc, typeName, url, icoPath, startup, keywords );
		if ( id != 0 )
		{
			CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_CATES);
			// 向list加入一项
			int itemIndex;
			itemIndex = lst.GetItemCount();
			lst.InsertItem( itemIndex, format( _T("%d"), id ).c_str() );

			// 向数组添加一项
			m_ids.Add(id);
			m_cateNames.Add(_T(""));
			m_cateDescs.Add(_T(""));
			m_typeNames.Add(_T(""));
			m_urls.Add(_T(""));
			m_icoPaths.Add(_T(""));
			m_startups.Add(_T(""));
			m_keywordss.Add(_T(""));
			m_timeWritens.Add(0);

			UpdateList( UPDATE_LOAD_DATA | UPDATE_LIST_ITEMS, itemIndex );
			lst.SetItemState( itemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		}
	}
}

void AccountCatesDlg::OnModify()
{
	CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_CATES);
	int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
	int id = m_ids[index];
	CString newCateName = m_cateNames[index];
	CString newCateDesc = m_cateDescs[index];
	CString newTypeName = m_typeNames[index];
	CString newUrl = m_urls[index];
	CString newIcoPath = m_icoPaths[index];
	CString newStartup = m_startups[index];
	CString newKeywords = m_keywordss[index];

	AccountCateEditingDlg editingDlg(
		this,
		false,
		&newCateName,
		&newCateDesc,
		&newTypeName,
		&newUrl,
		&newIcoPath,
		&newStartup,
		&newKeywords
	);
	if ( IDOK == editingDlg.DoModal() )
	{
		if ( ModifyAccountCate( id, newCateName, newCateDesc, newTypeName, newUrl, newIcoPath, newStartup, newKeywords ) )
		{
			m_cateNames[index] = newCateName;
			m_cateDescs[index] = newCateDesc;
			m_typeNames[index] = newTypeName;
			m_urls[index] = newUrl;
			m_icoPaths[index] = newIcoPath;
			m_startups[index] = newStartup;
			m_keywordss[index] = newKeywords;

			UpdateList( UPDATE_LIST_ITEMS, index );
			CString strId;
			strId.Format( _T("%d"), id );
			LVFINDINFO fi;
			fi.flags = LVFI_PARTIAL | LVFI_STRING;
			fi.psz = strId;
			int itemIndex = lst.FindItem(&fi);
			lst.SetItemState( itemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		}
	}
}

void AccountCatesDlg::OnDelete()
{
	if ( IDYES == MessageBox( _T("此操作不可恢复，确定要删除？"), _T("确认"), MB_YESNO ) )
	{
		CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_CATES);
		int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
		CString strId;
		strId = lst.GetItemText( index, 0 );
		if ( DeleteAccountCate( mixed((LPCTSTR)strId) ) )
		{
			lst.DeleteItem(index);

			m_ids.RemoveAt(index);
			m_cateNames.RemoveAt(index);
			m_cateDescs.RemoveAt(index);
			m_typeNames.RemoveAt(index);
			m_urls.RemoveAt(index);
			m_icoPaths.RemoveAt(index);
			m_startups.RemoveAt(index);
			m_keywordss.RemoveAt(index);
			m_timeWritens.RemoveAt(index);

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