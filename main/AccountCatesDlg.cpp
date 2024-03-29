
#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountCatesDlg.h"
#include "AccountCateEditingDlg.h"
#include "MainFrame.h"
#include "AccountComprehensiveWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AccountCatesDlg dialog

AccountCatesDlg::AccountCatesDlg( CWnd * parent ) : Dialog(AccountCatesDlg::IDD, parent)
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
    ON_COMMAND(ID_CATE_ADD, OnCateAdd)
    ON_COMMAND(ID_CATE_MODIFY, OnCateModify)
    ON_COMMAND(ID_CATE_DELETE, OnCateDelete)
    ON_COMMAND(ID_CATE_SHOW_ACCOUNTS, OnCateShowAccounts)
    //}}AFX_MSG_MAP
    ON_UPDATE_COMMAND_UI_RANGE(ID_CATE_MODIFY, ID_CATE_SHOW_ACCOUNTS, OnUpdateCateMenu)
END_MESSAGE_MAP()

void AccountCatesDlg::UpdateList( int flag, long itemIndex )
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
            // 用ID获取记录
            GetAccountCate( g_theApp.GetDatabase(), m_cates[itemIndex].m_id, &m_cates[itemIndex] );
        }
    }

    if ( flag & UPDATE_LIST_ITEMS )
    {
        if ( itemIndex == -1 )
        {
            if ( !( flag & UPDATE_LOAD_DATA ) ) // 如果没有载入操作，需要从数组获取数量信息
            {
                count = m_cates.GetSize();
            }
            // Clear all items
            lst.DeleteAllItems();
            int i;
            for ( i = 0; i < count; ++i )
            {
                lst.InsertItem( i, winplus::Format( _T("%d"), m_cates[i].m_id ).c_str() );
                lst.SetItem( i, 1, LVIF_TEXT, m_cates[i].m_cateName, 0, 0, 0, 0 );
                lst.SetItem( i, 2, LVIF_TEXT, m_cates[i].m_typeName, 0, 0, 0, 0 );
                lst.SetItem( i, 3, LVIF_TEXT, m_cates[i].m_url, 0, 0, 0, 0 );
                lst.SetItem( i, 4, LVIF_TEXT, m_cates[i].m_startup, 0, 0, 0, 0 );
                lst.SetItem( i, 5, LVIF_TEXT, m_cates[i].m_keywords, 0, 0, 0, 0 );
            }
        }
        else
        {
            lst.SetItem( itemIndex, 0, LVIF_TEXT, winplus::Format( _T("%d"), m_cates[itemIndex].m_id ).c_str(), 0, 0, 0, 0 );
            lst.SetItem( itemIndex, 1, LVIF_TEXT, m_cates[itemIndex].m_cateName, 0, 0, 0, 0 );
            lst.SetItem( itemIndex, 2, LVIF_TEXT, m_cates[itemIndex].m_typeName, 0, 0, 0, 0 );
            lst.SetItem( itemIndex, 3, LVIF_TEXT, m_cates[itemIndex].m_url, 0, 0, 0, 0 );
            lst.SetItem( itemIndex, 4, LVIF_TEXT, m_cates[itemIndex].m_startup, 0, 0, 0, 0 );
            lst.SetItem( itemIndex, 5, LVIF_TEXT, m_cates[itemIndex].m_keywords, 0, 0, 0, 0 );
        }
    }
}

void AccountCatesDlg::DoAdd( CWnd * parent, winplus::Mixed * cate )
{
    VERIFY_ONCE_DIALOG(onceEditingDlg);
    VERIFY_RUNONLY_OTHER_HPROCESS(parent);

    AccountCateEditingDlg editingDlg( parent, true, cate );
    SetNullScopeOut setNullScopeOut( onceEditingDlg = &editingDlg );
    if ( IDOK == editingDlg.DoModal() )
    {
        int id = AddAccountCate( g_theApp.GetDatabase(), *cate );
        if ( id != 0 )
        {
            CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_CATES);
            // 向list加入一项
            int itemIndex = lst.GetItemCount();
            lst.InsertItem( itemIndex, winplus::Format( _T("%d"), id ).c_str() );

            // 向数组添加一项
            AccountCate tmp;
            tmp.m_id = id;
            m_cates.Add(tmp);

            UpdateList( UPDATE_LOAD_DATA | UPDATE_LIST_ITEMS, itemIndex );
            lst.SetItemState( itemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
        }
    }
}

// 搜索关键字，输出匹配的关键字，返回个数
size_t __SearchKeywords( CString const & text, winplus::StringArray * pArrKeywords )
{
    if ( pArrKeywords->size() < 1 ) return 0;
    winplus::StringArray matchedKeywords;
    winplus::ssize_t offset = 0;
    winplus::MultiMatch::MatchResult r;
    do
    {
        winplus::MultiMatch mm( *pArrKeywords, NULL );
        r = mm.search( CStringToString(text), offset );
        if ( r.pos != winplus::npos )
        {
            matchedKeywords.push_back(mm.getMatchItem(r.item));
            offset = r.pos + mm.getMatchItem(r.item).length();
            pArrKeywords->erase( pArrKeywords->begin() + r.item );
        }
    }
    while ( r.pos != winplus::npos );

    *pArrKeywords = matchedKeywords;
    return pArrKeywords->size();
}

int AccountCatesDlg::GetCateIndexMatchWndTitle( CString const & wndTitle, bool isBrowser ) const
{
    // 载入关键字
    std::vector<winplus::StringArray> keywordsArr;
    int i, j;
    int catesCount = m_cates.GetSize(); // 种类数

    for ( i = 0; i < catesCount; ++i )
    {
        winplus::StringArray keywords;
        winplus::StrSplit( CStringToString(m_cates[i].m_keywords), _T(","), &keywords );
        keywordsArr.push_back(keywords);
    }

    // 待检测的CateIndex
    std::vector<int> checkCateIndexs;
    for ( i = 0; i < catesCount; ++i )
    {
        if ( isBrowser )
        {
            if ( m_cates[i].m_startup == _T("网站") )
            {
                checkCateIndexs.push_back(i);
            }
        }
        else
        {
            if ( m_cates[i].m_startup == _T("软件") )
            {
                checkCateIndexs.push_back(i);
            }
        }
    }

    // 应该按每个种类的关键字搜寻标题，找到关键字匹配最多的种类
    for ( j = 0; j < checkCateIndexs.size(); )
    {
        winplus::StringArray & keywords = keywordsArr[checkCateIndexs[j]];
        if ( __SearchKeywords( wndTitle, &keywords ) == 0 )
        {
            checkCateIndexs.erase( checkCateIndexs.begin() + j );
            continue;
        }
        ++j;
    }

    // 判断所属种类索引
    if ( checkCateIndexs.size() == 0 )
    {
        return -1;
    }
    else
    {
        int m = 0, mIndex = -1;
        // 选出匹配关键字最多的那个
        for ( i = 0; i < checkCateIndexs.size(); ++i )
        {
            if ( keywordsArr[checkCateIndexs[i]].size() > m )
            {
                m = (int)keywordsArr[checkCateIndexs[i]].size();
                mIndex = checkCateIndexs[i];
            }
        }

        return mIndex;
    }
}

/////////////////////////////////////////////////////////////////////////////
// AccountCatesDlg message handlers

BOOL AccountCatesDlg::OnInitDialog() 
{
    Dialog::OnInitDialog();

    CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_CATES);

    // 设置ListView中ListCtrl的扩展风格
    lst.SetExtendedStyle(LVS_EX_FULLROWSELECT);

    // 初始化列表头
    lst.InsertColumn( 0, _T("编号"), LVCFMT_LEFT, 48 );
    lst.InsertColumn( 1, _T("名称"), LVCFMT_LEFT, 96 );
    lst.InsertColumn( 2, _T("类别"), LVCFMT_LEFT, 48 );
    lst.InsertColumn( 3, _T("URL"), LVCFMT_LEFT, 130 );
    lst.InsertColumn( 4, _T("启动"), LVCFMT_LEFT, 48 );
    lst.InsertColumn( 5, _T("关键字"), LVCFMT_LEFT, 120 );

    this->UpdateList();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void AccountCatesDlg::OnListActivated( NMHDR* pNMHDR, LRESULT* pResult )
{
    CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_CATES);
    int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
    if ( index == -1 ) return;

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

void AccountCatesDlg::OnCateAdd()
{
    winplus::Mixed cate;
    DoAdd( GetOwner(), &cate );
}

void AccountCatesDlg::OnCateModify()
{
    VERIFY_ONCE_DIALOG(onceEditingDlg);

    CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_CATES);
    int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );

    winplus::Mixed curCate;
    int id = m_cates[index].m_id;
    m_cates[index].assignTo(&curCate);

    AccountCateEditingDlg editingDlg( GetOwner(), false, &curCate );
    SetNullScopeOut setNullScopeOut( onceEditingDlg = &editingDlg );
    if ( IDOK == editingDlg.DoModal() )
    {
        if ( ModifyAccountCate( g_theApp.GetDatabase(), id, curCate ) )
        {
            m_cates[index] = curCate;

            UpdateList( UPDATE_LIST_ITEMS, index );
            CString strId;
            strId.Format( _T("%d"), id );
            LVFINDINFO fi;
            fi.flags = LVFI_PARTIAL | LVFI_STRING;
            fi.psz = strId;
            int itemIndex = lst.FindItem(&fi);
            lst.SetItemState( itemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
            GetOwner()->PostMessage( WM_UPDATELIST_ALL, UPDATELIST_MAIN );
        }
    }
}

void AccountCatesDlg::OnCateDelete()
{
    if ( IDYES == GetOwner()->MessageBox( _T("此操作不可恢复，确定要删除？"), _T("确认"), MB_YESNO ) )
    {
        CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_CATES);
        int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
        CString strId;
        strId = lst.GetItemText( index, 0 );
        if ( DeleteAccountCate( g_theApp.GetDatabase(), winplus::Mixed( (LPCTSTR)strId ) ) )
        {
            lst.DeleteItem(index);

            m_cates.RemoveAt(index);

            index = index < lst.GetItemCount() ? index : lst.GetItemCount() - 1;
            lst.SetItemState( index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
        }
    }
}

void AccountCatesDlg::OnCateShowAccounts()
{
    CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_CATES);
    int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );

    AccountArray accounts;
    int accountsCount = LoadAccounts( g_theApp.GetDatabase(), g_theApp.m_loginedUser.m_id, &accounts, m_cates[index].m_id );
    // 测试新账户综合窗口
    AccountComprehensiveWnd * pComprehensiveWnd = AccountComprehensiveWnd::Create<AccountComprehensiveWnd>(
        AfxGetMainWnd()->GetSafeHwnd(),
        AfxGetMainWnd(),
        CRect( 0, 0, 300, 412 ),
        m_cates[index],
        accounts
    );
    pComprehensiveWnd->AutoDelete(TRUE);
    pComprehensiveWnd->UpdateWindow();
    winplus::Window_Center( *pComprehensiveWnd, AfxGetMainWnd()->GetSafeHwnd() );
    pComprehensiveWnd->ShowWindow(SW_NORMAL);
}

void AccountCatesDlg::OnUpdateCateMenu( CCmdUI * pCmdUI )
{
    CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_CATES);
    int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
    pCmdUI->Enable( index != -1 );
}
