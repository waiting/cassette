// BrowsersDlg.cpp: 实现文件
//

#include "Cassette.h"
#include "CassetteApp.h"
#include "BrowsersDlg.h"
#include "BrowserEditingDlg.h"
// BrowsersDlg 对话框

BrowsersDlg::BrowsersDlg( CWnd * pParent ) : Dialog( BrowsersDlg::IDD, pParent )
{

}

BrowsersDlg::~BrowsersDlg()
{
}

void BrowsersDlg::UpdateList( int flag, long itemIndex )
{
    int count = 0;
    CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_BROWSERS);

    if ( flag & UPDATE_LOAD_DATA )
    {
        if ( itemIndex == -1 )
        {
            // 加载浏览器识别列表
            count = LoadBrowsers( g_theApp.GetDatabase(), &m_browsers );
        }
        else
        {
            // 用ID获取记录
            GetBrowser( g_theApp.GetDatabase(), m_browsers[itemIndex].m_id, &m_browsers[itemIndex] );
        }
    }

    if ( flag & UPDATE_LIST_ITEMS )
    {
        if ( itemIndex == -1 )
        {
            if ( !( flag & UPDATE_LOAD_DATA ) ) // 如果没有载入操作，需要从数组获取数量信息
            {
                count = m_browsers.GetCount();
            }
            // Clear all items
            lst.DeleteAllItems();
            int i;
            for ( i = 0; i < count; ++i )
            {
                lst.InsertItem( i, winplus::Format( _T("%d"), m_browsers[i].m_id ).c_str() );
                lst.SetItem( i, 1, LVIF_TEXT, m_browsers[i].m_browserName, 0, 0, 0, 0 );
                lst.SetItem( i, 2, LVIF_TEXT, m_browsers[i].m_browserTitle, 0, 0, 0, 0 );
                lst.SetItem( i, 3, LVIF_TEXT, m_browsers[i].m_exeName, 0, 0, 0, 0 );
            }
        }
        else
        {
            lst.SetItem( itemIndex, 0, LVIF_TEXT, winplus::Format( _T("%d"), m_browsers[itemIndex].m_id ).c_str(), 0, 0, 0, 0 );
            lst.SetItem( itemIndex, 1, LVIF_TEXT, m_browsers[itemIndex].m_browserName, 0, 0, 0, 0 );
            lst.SetItem( itemIndex, 2, LVIF_TEXT, m_browsers[itemIndex].m_browserTitle, 0, 0, 0, 0 );
            lst.SetItem( itemIndex, 3, LVIF_TEXT, m_browsers[itemIndex].m_exeName, 0, 0, 0, 0 );
        }
    }
}

void BrowsersDlg::DoDataExchange(CDataExchange* pDX)
{
	Dialog::DoDataExchange(pDX);
}

BOOL BrowsersDlg::OnInitDialog()
{
    Dialog::OnInitDialog();

    CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_BROWSERS);

    // 设置ListView中ListCtrl的扩展风格
    lst.SetExtendedStyle(LVS_EX_FULLROWSELECT);

    CRect rectClient;
    GetClientRect(&rectClient);
    int width = rectClient.Width() - 1 - GetSystemMetrics(SM_CXVSCROLL);

    // 初始化列表头
    lst.InsertColumn( 0, _T("编号"), LVCFMT_LEFT, 40 );
    lst.InsertColumn( 1, _T("缩写"), LVCFMT_LEFT, 60 );
    lst.InsertColumn( 3, _T("EXE文件名"), LVCFMT_LEFT, 100 );
    lst.InsertColumn( 2, _T("标题"), LVCFMT_LEFT, width - 200 );


    this->UpdateList();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

BEGIN_MESSAGE_MAP(BrowsersDlg, Dialog)
    ON_NOTIFY( NM_DBLCLK, IDC_LIST_BROWSERS, OnListActivated )
    ON_NOTIFY( NM_RETURN, IDC_LIST_BROWSERS, OnListActivated )
    ON_NOTIFY( NM_RCLICK, IDC_LIST_BROWSERS, OnListRClick )
    ON_COMMAND( ID_BROWSER_ADD, OnBrowserAdd )
    ON_COMMAND( ID_BROWSER_MODIFY, OnBrowserModify )
    ON_COMMAND( ID_BROWSER_DELETE, OnBrowserDelete )
    ON_UPDATE_COMMAND_UI_RANGE( ID_BROWSER_MODIFY, ID_BROWSER_DELETE, OnUpdateBrowserMenu )
END_MESSAGE_MAP()

// BrowsersDlg 消息处理程序

void BrowsersDlg::OnListActivated( NMHDR *pNMHDR, LRESULT *pResult )
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>( pNMHDR );
    if ( pNMItemActivate->iItem == -1 ) goto RETURN;

    // 模拟点击菜单命令
    PostMessage( WM_COMMAND, MAKEWPARAM( ID_BROWSER_MODIFY, 0 ), 0 );

RETURN:
    *pResult = 0;
}

void BrowsersDlg::OnListRClick( NMHDR *pNMHDR, LRESULT *pResult )
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>( pNMHDR );

    CMenu menu;
    menu.LoadMenu(IDM_OPERATE_BROWSERS);
    POINT pt;
    GetCursorPos(&pt);
    menu.GetSubMenu(0)->TrackPopupMenu( 0, pt.x, pt.y, this );

    *pResult = 0;
}

void BrowsersDlg::OnBrowserAdd()
{
    VERIFY_ONCE_DIALOG(onceEditingDlg);

    Mixed browserFields;
    BrowserEditingDlg editingDlg( GetOwner(), true, &browserFields );
    SetNullScopeOut setNullScopeOut( onceEditingDlg = &editingDlg );
    if ( IDOK == editingDlg.DoModal() )
    {
        int id = AddBrowser( g_theApp.GetDatabase(), browserFields );
        if ( id != 0 )
        {
            CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_BROWSERS);
            // 向list加入一项
            int itemIndex = lst.GetItemCount();
            lst.InsertItem( itemIndex, winplus::Format( _T("%d"), id ).c_str() );

            // 向数组添加一项
            Browser tmp;
            tmp.m_id = id;
            m_browsers.Add(tmp);

            UpdateList( UPDATE_LOAD_DATA | UPDATE_LIST_ITEMS, itemIndex );
            lst.SetItemState( itemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
        }
    }
}

void BrowsersDlg::OnBrowserModify()
{
    VERIFY_ONCE_DIALOG(onceEditingDlg);

    CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_BROWSERS);
    int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );

    Mixed browserFields;
    int id = m_browsers[index].m_id;
    m_browsers[index].assignTo(&browserFields);

    BrowserEditingDlg editingDlg( GetOwner(), false, &browserFields );
    SetNullScopeOut setNullScopeOut( onceEditingDlg = &editingDlg );
    if ( IDOK == editingDlg.DoModal() )
    {
        if ( ModifyBrowser( g_theApp.GetDatabase(), id, browserFields ) )
        {
            m_browsers[index] = browserFields;

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

void BrowsersDlg::OnBrowserDelete()
{
    if ( IDYES == GetOwner()->MessageBox( _T("此操作不可恢复，确定要删除？"), _T("确认"), MB_YESNO ) )
    {
        CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_BROWSERS);
        int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
        CString strId;
        strId = lst.GetItemText( index, 0 );
        if ( DeleteBrowser( g_theApp.GetDatabase(), winplus::Mixed( (LPCTSTR)strId ) ) )
        {
            lst.DeleteItem(index);

            m_browsers.RemoveAt(index);

            index = index < lst.GetItemCount() ? index : lst.GetItemCount() - 1;
            lst.SetItemState( index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
        }
    }
}

void BrowsersDlg::OnUpdateBrowserMenu( CCmdUI * pCmdUI )
{
    CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_BROWSERS);
    int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
    pCmdUI->Enable( index != -1 );
}
