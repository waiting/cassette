
#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountsView.h"
#include "MainFrame.h"
#include "AboutView.h"
#include "AboutWnd.h"
#include "UserSettingsDlg.h"
#include "AppSettingsDlg.h"
#include "AccountEditingDlg.h"
#include "AccountIntegratedWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MainFrame

IMPLEMENT_DYNAMIC(MainFrame, CFrameWnd)

/////////////////////////////////////////////////////////////////////////////
// MainFrame construction/destruction

MainFrame::MainFrame() : m_pAccountsView(new AccountsView)
{
}

MainFrame::~MainFrame()
{
}

BOOL MainFrame::PreCreateWindow( CREATESTRUCT & cs )
{
    if ( !CFrameWnd::PreCreateWindow(cs) )
        return FALSE;

    cs.style &= ~WS_BORDER;
    cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
    cs.lpszClass = AfxRegisterWndClass( 0, 0, (HBRUSH)/*GetStockObject*/(WHITE_BRUSH+1) );
    return TRUE;
}

BEGIN_MESSAGE_MAP(MainFrame, CFrameWnd)
    //{{AFX_MSG_MAP(MainFrame)
    ON_WM_CREATE()
    ON_WM_SETFOCUS()
    ON_COMMAND(ID_APP_EXIT, OnAppExit)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_UPDATE_COMMAND_UI(ID_VIA_AUTOLOGIN, OnUpdateViaAutoLogin)
    ON_COMMAND(ID_VIA_AUTOLOGIN, OnViaAutoLogin)
    ON_UPDATE_COMMAND_UI(ID_OPEN_URL, OnUpdateOpenUrl)
    ON_COMMAND(ID_OPEN_URL, OnOpenUrl)
    ON_COMMAND(ID_USER_SETTINGS, OnUserSettings)
    ON_COMMAND(ID_APP_SETTINGS, OnAppSettings)
    ON_WM_CLOSE()
    ON_WM_DESTROY()
    ON_COMMAND(ID_ADD_ACCOUNT, OnAddAccount)
    ON_COMMAND(ID_MODIFY_ACCOUNT, OnModifyAccount)
    ON_COMMAND(ID_DEL_ACCOUNT, OnDelAccount)
    ON_COMMAND(ID_ACCOUNT_CATES, OnAccountCates)
    ON_COMMAND(ID_ACCOUNT_TYPES, OnAccountTypes)
    ON_COMMAND(ID_BACKUP_DATA, OnBackupData)
    ON_COMMAND(ID_RESUME_DATA, OnResumeData)
    ON_COMMAND(ID_MAINWND_SHOWHIDE, OnMainWndShowHide )
    ON_UPDATE_COMMAND_UI(ID_MAINWND_SHOWHIDE, OnUpdateMainWndShowHide)
    ON_UPDATE_COMMAND_UI(ID_ACCOUNT_CATES, OnUpdateAccountCates)
    ON_UPDATE_COMMAND_UI(ID_ACCOUNT_TYPES, OnUpdateAccountTypes)
    //}}AFX_MSG_MAP
    ON_MESSAGE( WM_UPDATELIST_ALL, OnUpdateListAll )
    ON_MESSAGE( WM_HOTKEY, OnHotkey )
    ON_UPDATE_COMMAND_UI_RANGE(ID_MODIFY_ACCOUNT, ID_DEL_ACCOUNT, OnUpdateOperateAccount)
    ON_MESSAGE(WM_TRAY_NOTIFICATION, OnTrayNotification)
    ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

void MainFrame::UpdateTitle()
{
    CString newTitle;
    newTitle.Format( _T("%s [%s]"), winplus::LoadString(AFX_IDS_APP_TITLE).c_str(), (LPCTSTR)g_theApp.m_loginedUser.m_username );
    this->SetWindowText(newTitle);
}

void MainFrame::RefreshHotkey( int hotkey )
{
    if ( !g_theApp.m_settings.isEnabledHotkey )
    {
        UnregisterHotKey( GetSafeHwnd(), IDC_REGISTERED_HOTKEY );
        return;
    }
    BOOL ret;
    ret = UnregisterHotKey( GetSafeHwnd(), IDC_REGISTERED_HOTKEY );
    if ( !hotkey ) return;
    ret = RegisterHotKey( GetSafeHwnd(), IDC_REGISTERED_HOTKEY, HIWORD(hotkey), LOWORD(hotkey) );
    if ( !ret )
    {
        WarningError( _T("用户热键注册失败"), _T("错误") );
    }
}

void MainFrame::UpdateList( int flag, long itemIndex )
{
    IfPTR(m_pAccountsView)->UpdateList( flag, itemIndex );
}

void MainFrame::DoAddAccount( CWnd * parent, winux::Mixed & newAccount )
{
    VERIFY_RUNONLY_OTHER_HPROCESS(parent);
    VERIFY_ONCE_DIALOG(onceEditingDlg);

    AccountEditingDlg editingDlg( parent, true, &newAccount );

    SetNullScopeOut setNullScopeOut( onceEditingDlg = &editingDlg );

    if ( IDOK == editingDlg.DoModal() )
    {
        if ( AddAccount( g_theApp.GetDatabase(), newAccount ) )
        {
            Account account;
            account.assign(newAccount);

            CListCtrl & lst = m_pAccountsView->GetListCtrl();
            // 向list加入一项
            int itemIndex;
            itemIndex = lst.GetItemCount();
            lst.InsertItem( itemIndex, account.m_myName );

            // 向数组添加一项
            m_pAccountsView->m_accounts.Add(account);

            UpdateList( UPDATE_LOAD_DATA | UPDATE_LIST_ITEMS, itemIndex );
            lst.SetItemState( itemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// MainFrame diagnostics

#ifdef _DEBUG
void MainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

void MainFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// MainFrame message handlers

int MainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if ( CFrameWnd::OnCreate(lpCreateStruct) == -1 )
        return -1;
    // 创建一个账户视图占据整个框架窗口的客户区域
    if ( !m_pAccountsView->Create(
            NULL,
            NULL,
            AFX_WS_DEFAULT_VIEW,
            CRect( 0, 0, 0, 0 ),
            this,
            AFX_IDW_PANE_FIRST
        )
    )
    {
        TRACE0("Failed to create view window\n");
        return -1;
    }
    // 更新列表
    m_pAccountsView->UpdateList();

    // 创建一个工具条
    if ( !m_toolBar.CreateEx(
            this,
            TBSTYLE_FLAT | TBSTYLE_LIST /*| TBSTYLE_TRANSPARENT | TBSTYLE_TOOLTIPS*/,
            WS_CHILD | WS_VISIBLE | CBRS_TOP /*| CBRS_GRIPPER*/ | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC
        ) || !m_toolBar.LoadToolBar(IDR_MAINFRAME)
    )
    {
        TRACE0("Failed to create toolbar\n");
        return -1;      // fail to create
    }
    // 载入工具条图片
    // 资源文件中可改变工具条按钮大小,比较方便
    // 图片和工具条按钮大小应该匹配
    CImageList imgList;
    imgList.Create( 48, 48, ILC_COLOR32, 0, 1 );
    winplus::ImageList_Add32bpp( imgList.GetSafeHandle(), IDR_MAINFRAME, IMAGE_BITMAP );
    //ImageList_AddAlphaImage( imgList.GetSafeHandle(), IDR_PNG_TOOLBAR, _T("PNG") );
    m_toolBar.GetToolBarCtrl().SetImageList(&imgList);
    imgList.Detach();//*/

    // 如果你不想要工具条可停靠,可删除下面这3行
    m_toolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_toolBar);

    // 创建状态条
    static UINT indicators[] =
    {
        ID_SEPARATOR,           // status line indicator
        //ID_INDICATOR_CAPS,
        //ID_INDICATOR_NUM,
        //ID_INDICATOR_SCRL,
    };
    
    if ( !m_statusBar.Create(this) || !m_statusBar.SetIndicators( indicators, countof(indicators) ) )
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }
    

    // 设置图标
    SetIcon( winplus::Icon( IDR_MAINFRAME, 16, 16 ), TRUE );
    // 把窗口句柄提交给共享内存,以便激活
    g_theApp.GetSharedMemory()->hMainWnd = GetSafeHwnd();

    UpdateTitle();
    // 注册热键
    RefreshHotkey(g_theApp.m_loginedUser.m_hotkey);

    // 创建非模态对话框, CatesDlg, TypesDlg
    m_catesDlg.Create(this);
    m_catesDlg.UpdateWindow();
    m_typesDlg.Create(this);
    m_typesDlg.UpdateWindow();

    // 创建托盘通知
    //static HICON hTrayIcon;
    //LoadIconMetric( AfxGetApp()->m_hInstance, MAKEINTRESOURCEW(IDR_MAINFRAME), LIM_SMALL, &hTrayIcon );
    static winplus::Icon trayIcon(IDR_MAINFRAME);

    m_noti.add( GetSafeHwnd(), 1, trayIcon, winplus::LoadString(IDR_MAINFRAME) );
    m_noti.setMessage(WM_TRAY_NOTIFICATION);
    //m_noti.setBalloonInfo( winplus::LoadString(IDR_MAINFRAME), "已经启动" );
    m_noti.modify();

    return 0;
}

void MainFrame::OnDestroy() 
{
    this->RefreshHotkey(0);

    CFrameWnd::OnDestroy();
}

void MainFrame::OnSetFocus(CWnd* pOldWnd)
{
    // forward focus to the view window
    m_pAccountsView->SetFocus();
}

BOOL MainFrame::OnCmdMsg( UINT nID, int nCode, void * pExtra, AFX_CMDHANDLERINFO * pHandlerInfo )
{
    // let the view have first crack at the command
    if ( m_pAccountsView->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) )
        return TRUE;

    // otherwise, do default handling
    return CFrameWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

void MainFrame::OnAppAbout()
{
    // 只能打开一个窗口
    AboutWnd * pAbout = AboutWnd::GetSingleton(this);
    pAbout->ShowWindow(SW_NORMAL);
    pAbout->SetForegroundWindow();
    pAbout->SetFocus();
    
}

void MainFrame::OnUpdateViaAutoLogin( CCmdUI * pCmdUI ) 
{
    pCmdUI->Enable(g_theApp.m_viaAutoLogin);
    //pCmdUI->SetCheck( g_theApp.m_viaAutoLogin );
    pCmdUI->SetText( _T("不自动登录") );
}

void MainFrame::OnViaAutoLogin() 
{
    g_theApp.m_viaAutoLogin = FALSE;
    g_theApp.m_settings.isAutoLogin = false;
    g_theApp.SaveSettings(CassetteApp::Setting_AutoLogin);
}

// 当更新操作账户的相关菜单状态时调用
void MainFrame::OnUpdateOperateAccount( CCmdUI * pCmdUI ) 
{
    CListCtrl & lst = this->m_pAccountsView->GetListCtrl();
    int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
    pCmdUI->Enable( index != -1 );
}

// 当更新OpenURL菜单状态时调用
void MainFrame::OnUpdateOpenUrl( CCmdUI * pCmdUI ) 
{
    CListCtrl & lst = this->m_pAccountsView->GetListCtrl();
    int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
    pCmdUI->Enable(FALSE);
    if ( index != -1 )
    {
        AccountCate cate;
        GetAccountCate( g_theApp.GetDatabase(), this->m_pAccountsView->m_accounts[index].m_cateId, &cate );
        if ( !cate.m_url.IsEmpty() && cate.m_startup != _T("其他") )
        {
            pCmdUI->SetText( _T("打开") + cate.m_cateName + _T("(&Go)\tCtrl+G") );
            pCmdUI->Enable(TRUE);
        }
    }
    else
    {
        pCmdUI->SetText( _T("打开URL(&Go)\tCtrl+G") );
    }
}

void MainFrame::OnOpenUrl() 
{
    CListCtrl & lst = this->m_pAccountsView->GetListCtrl();
    int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
    int cateId = this->m_pAccountsView->m_accounts[index].m_cateId;
    AccountCate cate;
    if ( GetAccountCate( g_theApp.GetDatabase(), this->m_pAccountsView->m_accounts[index].m_cateId, &cate ) )
    {
        if ( cate.m_startup == _T("网站") )
        {
            ShellExecute( NULL, _T("open"), cate.m_url, NULL, NULL, SW_NORMAL );
        }
        else if ( cate.m_startup == _T("软件") )
        {
            ShellExecute( NULL, NULL, cate.m_url, NULL, NULL, SW_NORMAL );
        }
    }
}

void MainFrame::OnUserSettings() 
{
    VERIFY_ONCE_DIALOG(onceSettingsDlg);

    winux::Mixed userFields;
    userFields.addPair()
        ( "name", (LPCTSTR)g_theApp.m_loginedUser.m_username )
        ( "protect", g_theApp.m_loginedUser.m_protectLevel )
        ( "condone", g_theApp.m_loginedUser.m_condone )
        ( "hotkey", g_theApp.m_loginedUser.m_hotkey )
        ;

    UserSettingsDlg settingsDlg( this, &userFields );

    SetNullScopeOut setNullScopeOut( onceSettingsDlg = &settingsDlg );

    if ( IDOK == settingsDlg.DoModal() )
    {
        if ( ModifyUserEx(
            g_theApp.GetDatabase(),
            g_theApp.m_loginedUser.m_username,
            userFields
        ) )
        {
            g_theApp.m_loginedUser.m_username = userFields["name"].toAnsi().c_str();
            g_theApp.m_loginedUser.m_protectLevel = userFields["protect"];
            g_theApp.m_loginedUser.m_condone = userFields["condone"];
            g_theApp.m_loginedUser.m_hotkey = userFields["hotkey"];

            this->UpdateTitle();
            this->RefreshHotkey(userFields["hotkey"]);
        }
    }

}

void MainFrame::OnAppSettings()
{
    VERIFY_ONCE_DIALOG(onceSettingsDlg);

    AppSettingsDlg settingsDlg( this, &g_theApp.m_settings );

    SetNullScopeOut setNullScopeOut( onceSettingsDlg = &settingsDlg );

    if ( IDOK == settingsDlg.DoModal() )
    {
        g_theApp.SaveSettings(
            CassetteApp::Setting_EnabledAutoRun |
            CassetteApp::Setting_EnabledHotkey |
            CassetteApp::Setting_EnabledHttpSrv |
            CassetteApp::Setting_EnabledScheme |
            CassetteApp::Setting_DatabasePath |
            CassetteApp::Setting_BackupPath |
            CassetteApp::Setting_WordslibPath
        );
        g_theApp.DoSettings(
            CassetteApp::Setting_EnabledAutoRun |
            CassetteApp::Setting_EnabledHotkey |
            CassetteApp::Setting_EnabledHttpSrv |
            CassetteApp::Setting_EnabledScheme |
            CassetteApp::Setting_DatabasePath |
            CassetteApp::Setting_BackupPath |
            CassetteApp::Setting_WordslibPath
        );
        PostMessage( WM_UPDATELIST_ALL, -1, 0 );
        RefreshHotkey(g_theApp.m_loginedUser.m_hotkey);
    }
}

void MainFrame::OnAddAccount()
{
    Account newAccount;
    newAccount.m_userId = g_theApp.m_loginedUser.m_id;
    newAccount.m_cateId = 0;
    newAccount.m_safeRank = 20;

    winux::Mixed accountFields;
    accountFields.addPair()
        ( "user", g_theApp.m_loginedUser.m_id )
        ( "cate", 0 )
        ( "safe_rank", 20 )
        ;

    DoAddAccount( this, accountFields );
}

void MainFrame::OnModifyAccount()
{
    VERIFY_ONCE_DIALOG(onceEditingDlg);

    CListCtrl & lst = this->m_pAccountsView->GetListCtrl();
    int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
    CString myName = m_pAccountsView->m_accounts[index].m_myName;

    winux::Mixed accountFields;
    m_pAccountsView->m_accounts[index].assignTo( &accountFields, "myname,account_name,account_pwd,cate,user,safe_rank,comment,time" );

    AccountEditingDlg editingDlg( this, false, &accountFields );

    SetNullScopeOut setNullScopeOut( onceEditingDlg = &editingDlg );

    if ( IDOK == editingDlg.DoModal() )
    {
        if ( ModifyAccount( g_theApp.GetDatabase(), g_theApp.m_loginedUser.m_id, myName, accountFields ) )
        {
            m_pAccountsView->m_accounts[index].assign(accountFields);

            UpdateList( UPDATE_LIST_ITEMS, index );
            LVFINDINFO fi;
            fi.flags = LVFI_PARTIAL | LVFI_STRING;
            myName = accountFields["myname"].refAnsi().c_str();
            fi.psz = myName;
            int itemIndex = lst.FindItem(&fi);
            lst.SetItemState( itemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
        }
    }
}

void MainFrame::OnDelAccount()
{
    if ( IDYES == MessageBox( _T("此操作不可恢复，确定要删除？"), _T("确认"), MB_YESNO ) )
    {
        CListCtrl & lst = this->m_pAccountsView->GetListCtrl();
        int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
        CString myName;
        myName = lst.GetItemText( index, 0 );
        if ( DeleteAccount( g_theApp.GetDatabase(), g_theApp.m_loginedUser.m_id, myName ) )
        {
            lst.DeleteItem(index);

            m_pAccountsView->m_accounts.RemoveAt(index);

            index = index < lst.GetItemCount() ? index : lst.GetItemCount() - 1;
            lst.SetItemState( index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
        }
    }
}

void MainFrame::OnUpdateAccountCates( CCmdUI * pCmdUI )
{
    pCmdUI->SetCheck( winplus::Window_IsShow( m_catesDlg.GetSafeHwnd() ) );
}

void MainFrame::OnUpdateAccountTypes( CCmdUI * pCmdUI )
{
    pCmdUI->SetCheck( winplus::Window_IsShow( m_typesDlg.GetSafeHwnd() ) );
}

void MainFrame::OnAccountCates()
{
    if ( winplus::Window_IsShow( m_catesDlg.GetSafeHwnd() ) )
    {
        m_catesDlg.ShowWindow(SW_HIDE);
    }
    else
    {
        CRect rcView, rcCatesDlg;
        m_pAccountsView->GetWindowRect(&rcView);
        m_catesDlg.GetWindowRect(&rcCatesDlg);
        m_catesDlg.SetWindowPos( NULL, rcView.right - rcCatesDlg.Width(), rcView.bottom - rcCatesDlg.Height(), 0, 0, SWP_NOSIZE | SWP_NOZORDER );
        m_catesDlg.ShowWindow(SW_NORMAL);
    }
}

void MainFrame::OnAccountTypes()
{
    if ( winplus::Window_IsShow( m_typesDlg.GetSafeHwnd() ) )
    {
        m_typesDlg.ShowWindow(SW_HIDE);
    }
    else
    {
        CRect rcView, rcTypesDlg;
        m_pAccountsView->GetWindowRect(&rcView);
        m_typesDlg.GetWindowRect(&rcTypesDlg);
        m_typesDlg.SetWindowPos( NULL, rcView.left, rcView.bottom - rcTypesDlg.Height(), 0, 0, SWP_NOSIZE | SWP_NOZORDER );
        m_typesDlg.ShowWindow(SW_NORMAL);
    }
}

void MainFrame::OnBackupData()
{
    static CString backupPath = ExplainCustomVars(g_theApp.m_settings.backupPath).c_str();
    winplus::FileDialog dlg( GetSafeHwnd(), FALSE, _T("请输入备份后要存储文件的名称"), _T("bak") );
    if ( dlg.doModal( backupPath, _T("备份文件(*.bak)\0*.bak\0所有文件(*.*)\0*.*\0") ) )
    {
        backupPath = dlg.getDirPath().c_str();

        if ( g_theApp.BackupData( dlg.getFilePath().c_str() ) )
        {
            MessageBox( _T("备份成功"), _T("提示"), MB_ICONINFORMATION );
        }
    }
}

void MainFrame::OnResumeData()
{
    static CString backupPath = ExplainCustomVars(g_theApp.m_settings.backupPath).c_str();
    winplus::FileDialog dlg( GetSafeHwnd(), TRUE, _T("请输入要恢复的文件名称") );
    if ( dlg.doModal( backupPath, _T("备份文件(*.bak)\0*.bak\0所有文件(*.*)\0*.*\0") ) )
    {
        backupPath = dlg.getDirPath().c_str();

        if ( g_theApp.ResumeData( dlg.getFilePath().c_str() ) )
        {
            MessageBox( _T("恢复成功"), _T("提示"), MB_ICONINFORMATION );
        }
    }
}

void MainFrame::OnMainWndShowHide()
{
    //this->ShowWindow();
    winplus::Window_Show( GetSafeHwnd(), !winplus::Window_IsShow( GetSafeHwnd() ) );
}

LRESULT MainFrame::OnHotkey( WPARAM wHotkeyId, LPARAM lParam )
{
    switch ( wHotkeyId )
    {
    case IDC_REGISTERED_HOTKEY:
        DoIntelligentHotkey();
        break;
    }
    return 0L;
}

LRESULT MainFrame::OnUpdateListAll( WPARAM wParam, LPARAM lParam )
{
    if ( wParam & UpdateList_Main )
        UpdateList();
    if ( wParam & UpdateList_TypesDlg )
        m_typesDlg.UpdateList();
    if ( wParam & UpdateList_CatesDlg )
        m_catesDlg.UpdateList();
    return 0L;
}

void MainFrame::DoIntelligentHotkey()
{
    // 取得当前活动的前台窗口
    CWnd * pCurWnd = GetForegroundWindow();
    if ( !pCurWnd ) return;

    // 通过HWND获取进程句柄,进而获取程序路径
    // 判断是浏览器还是软件
    winplus::String exeName, exePath;
    exePath = winplus::GetAppPathFromHWND(*pCurWnd);
    winplus::FilePath( exePath, &exeName );
    _strlwr(&exeName[0]);
    CString browserTitle;
    bool isBrowser = IsBrowserExeName( g_theApp.GetDatabase(), exeName.c_str(), &browserTitle );
    winplus::String curWndTitle = winplus::Window_GetText( pCurWnd->GetSafeHwnd() ); // 当前窗口标题
    
    AccountCate cate;
    if ( isBrowser ) // 若是浏览器，说明是网站
    {
        int pos = curWndTitle.rfind( _T(" - ") );
        if ( pos != -1 )
        {
            curWndTitle = curWndTitle.substr( 0, pos );
        }
        cate.m_cateName = curWndTitle.c_str();
        cate.m_startup = _T("网站");
    }
    else // 否则为软件
    {
        cate.m_cateName = curWndTitle.c_str();
        cate.m_startup = _T("软件");
        cate.m_url = exePath.c_str();
        cate.m_icoPath = (exePath + _T(",0")).c_str();
    }

    // 根据窗口标题和关键字匹配以及是否为浏览器判断是哪个种类
    // 如果没有种类符合,则添加种类;否则将显示相关种类的所有账户
    // 如果没有账户,则添加账户
    int cateIndex = m_catesDlg.GetCateIndexMatchWndTitle( curWndTitle.c_str(), isBrowser );
    if ( cateIndex == -1 )
    {
        winplus::StringArray autoKeywords;
        if ( g_theApp.GetWordslib() )
            g_theApp.GetWordslib()->splitWords( curWndTitle, &autoKeywords );
        cate.m_keywords = winplus::StrJoin( _T(","), autoKeywords ).c_str();

        winux::Mixed cateFields;
        cate.assignTo(&cateFields,"name,desc,type,url,icon,startup,keywords");

        m_catesDlg.DoAdd( pCurWnd, &cateFields );
    }
    else // 有该种类
    {
        AccountArray accounts;
        int accountsCount = LoadAccounts( g_theApp.GetDatabase(), g_theApp.m_loginedUser.m_id, &accounts, m_catesDlg.m_cates[cateIndex].m_id );
        if ( accountsCount < 1 )
        {
            Account newAccount;
            newAccount.m_cateId = m_catesDlg.m_cates[cateIndex].m_id;
            newAccount.m_userId = g_theApp.m_loginedUser.m_id;

            winux::Mixed accountFields;
            accountFields.addPair()
                ( "cate", m_catesDlg.m_cates[cateIndex].m_id )
                ( "user", g_theApp.m_loginedUser.m_id )
                ;

            DoAddAccount( pCurWnd, accountFields );
        }
        else // 有账户数据
        {
            CRect rcCurWnd = winplus::Window_GetClient(*pCurWnd);
            // 计算综合窗口的位置
            Gdiplus::Size siIntegratedWnd( 240, 300 );
            CRect rcIntegratedWnd = winplus::RectGdiplusToGdi<RECT>(
                    Gdiplus::Rect( Gdiplus::Point(
                    rcCurWnd.Width()-siIntegratedWnd.Width,
                    rcCurWnd.Height()-siIntegratedWnd.Height
                ),
                    siIntegratedWnd
                )
            );
            pCurWnd->ClientToScreen(&rcIntegratedWnd);

            AccountIntegratedWnd * pIntegratedWnd = NULL;
            if ( pIntegratedWnd = AccountIntegratedWnd::GetDisplayedWnd(*pCurWnd) )
            {
                //pIntegratedWnd->RefreshAccountsInfo( AccountCate const & cate, AccountArray const & accounts );
                pIntegratedWnd->SetWindowPos( NULL, rcIntegratedWnd.left, rcIntegratedWnd.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
            }
            else
            {
/*
                pIntegratedWnd = new AccountIntegratedWnd(
                    pCurWnd,
                    m_catesDlg.m_cates[cateIndex].m_cateName,
                    WS_SYSMENU,
                    WS_EX_LAYERED,
                    rcIntegratedWnd
                );
                //pIntegratedWnd->RefreshAccountsInfo( AccountCate const & cate, AccountArray const & accounts );
                //pIntegratedWnd->UpdateWindow();
                pIntegratedWnd->ShowWindow(SW_NORMAL);

                pCurWnd->SetWindowPos( &wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );*/

            }
        }
    }
}

LRESULT MainFrame::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
    USHORT x = GET_X_LPARAM(wParam), y = GET_Y_LPARAM(wParam);

    switch ( LOWORD(lParam) )
    {
    case NIN_BALLOONUSERCLICK:
        //AfxMessageBox("NIN_BALLOONUSERCLICK");
        //m_noti.setBalloonInfo( "点击了气泡", winux::Format("%u,%u",x,y) );
        //m_noti.modify();
        break;
    case NIN_SELECT:
        this->OnMainWndShowHide();
        break;
    case WM_CONTEXTMENU:
        {

            SetForegroundWindow();
            CMenu menu;
            menu.LoadMenu(IDM_TRAYNOTI_MENU);
            menu.GetSubMenu(0)->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_HORPOSANIMATION, x, y, this );
            /* menu.CreatePopupMenu();
            menu.AppendMenu( MF_STRING, ID_MAINWND_SHOWHIDE, "显示/隐藏" );
            menu.AppendMenu( MF_SEPARATOR );
            menu.AppendMenu( MF_STRING, ID_APP_EXIT, "退出程序" );
            menu.TrackPopupMenu( TPM_RIGHTBUTTON | TPM_HORPOSANIMATION, x, y, this );*/
        }
        break;

    }
    return 0;
}

void MainFrame::OnUpdateMainWndShowHide(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck( winplus::Window_IsShow( GetSafeHwnd() ) );
}

void MainFrame::OnClose()
{
    ShowWindow(SW_HIDE);
    //ShowWindow(SW_HIDE);
}

void MainFrame::OnAppExit()
{
    __super::OnClose();
}

void MainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
    __super::OnShowWindow(bShow, nStatus);
    if ( bShow )
    {
        //BringWindowToTop();
        SetWindowPos( &CWnd::wndTopMost, 0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
        //SetForegroundWindow();
    }
}
