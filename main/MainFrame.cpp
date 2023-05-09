
#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountsView.h"
#include "MainFrame.h"
#include "AboutView.h"
#include "AboutWnd.h"
#include "UserSettingsDlg.h"
#include "AppSettingsDlg.h"
#include "AccountEditingDlg.h"
#include "AccountComprehensiveWnd.h"
#include "BrowsersDlg.h"

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

MainFrame::MainFrame() : m_pAccountsView(new AccountsView), m_noti(0)
{
}

MainFrame::~MainFrame()
{
}

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
        WarningError( _T("�û��ȼ�ע��ʧ��"), _T("����") );
    }
}

void MainFrame::DoAddAccount( CWnd * parent, winplus::Mixed & newAccount )
{
    VERIFY_ONCE_DIALOG(onceEditingDlg);
    VERIFY_RUNONLY_OTHER_HPROCESS(parent);

    AccountEditingDlg editingDlg( parent, true, &newAccount );
    SetNullScopeOut setNullScopeOut( onceEditingDlg = &editingDlg );
    if ( IDOK == editingDlg.DoModal() )
    {
        if ( AddAccount( g_theApp.GetDatabase(), newAccount ) )
        {
            Account account;
            account = newAccount;

            CListCtrl & lst = m_pAccountsView->GetListCtrl();
            // ��list����һ��
            int itemIndex;
            itemIndex = lst.GetItemCount();
            lst.InsertItem( itemIndex, account.m_myName );

            // ���������һ��
            m_pAccountsView->m_accounts.Add(account);

            this->UpdateList( UPDATE_LOAD_DATA | UPDATE_LIST_ITEMS, itemIndex );
            lst.SetItemState( itemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
        }
    }
}

void MainFrame::DoIntelligentHotkey()
{
    // ȡ�õ�ǰ���ǰ̨����
    CWnd * pCurWnd = GetForegroundWindow();
    if ( pCurWnd == nullptr )
    {
        ColorOutputLine( fgRed, "Is nullptr" );
        return;
    }
    else
    {
        std::cout << *pCurWnd << ", " << "top window:" << std::boolalpha << winplus::Window_IsTopLevel(*pCurWnd) << ", "
            << winplus::Window_GetRect(*pCurWnd) << ", "
            << "title:" << winplus::Window_GetText(*pCurWnd) << std::endl;

        // ���Ƕ��㴰������Ҹ����㴰��
        if ( !Window_IsTopLevel(*pCurWnd) )
        {
            pCurWnd = pCurWnd->GetTopLevelParent();
            std::cout << *pCurWnd << ", " << "top window:" << std::boolalpha << winplus::Window_IsTopLevel(*pCurWnd) << ", "
                << winplus::Window_GetRect(*pCurWnd) << ", "
                << "title:" << winplus::Window_GetText(*pCurWnd) << std::endl;
        }
        // ����Ǳ����򴰿����˳�
        if ( pCurWnd == this )
        {
            ColorOutputLine( fgRed, "Is Mainframe" );
            return;
        }
    }

    // ͨ��HWND��ȡ���̾��,������ȡ����·��
    // �ж���������������
    winplus::String exeName, exePath;
    exePath = winplus::GetAppPathByHwnd(*pCurWnd);
    winplus::FilePath( exePath, &exeName );
    winplus::StrMakeLower(&exeName);
    CString browserTitle;
    bool isBrowser = IsBrowserExeName( g_theApp.GetDatabase(), exeName.c_str(), &browserTitle );
    winplus::String curWndTitle = winplus::Window_GetText( pCurWnd->GetSafeHwnd() ); // ��ǰ���ڱ���
    
    AccountCate newCate;
    if ( isBrowser ) // �����������˵������վ
    {
        int pos = (int)curWndTitle.rfind( _T(" - ") );
        if ( pos != -1 )
        {
            curWndTitle = curWndTitle.substr( 0, pos );
        }
        newCate.m_cateName = StringToCString(curWndTitle);
        newCate.m_startup = _T("��վ");
    }
    else // ����Ϊ���
    {
        newCate.m_cateName = StringToCString(curWndTitle);
        newCate.m_startup = _T("���");
        newCate.m_url = StringToCString(exePath);
        newCate.m_icoPath = StringToCString( exePath + _T(",0") );
    }

    // ���ݴ��ڱ���͹ؼ���ƥ���Լ��Ƿ�Ϊ������ж����ĸ�����
    // ���û���������,���������;������ʾ�������������˻�
    // ���û���˻�,������˻�
    int cateIndex = m_catesDlg.GetCateIndexMatchWndTitle( curWndTitle.c_str(), isBrowser );
    if ( cateIndex == -1 )
    {
        winplus::StringArray autoKeywords;
        if ( g_theApp.GetWordslib() )
            g_theApp.GetWordslib()->splitWords2( curWndTitle, &autoKeywords );
        newCate.m_keywords = winplus::StrJoin( _T(","), autoKeywords ).c_str();

        winplus::Mixed cateFields;
        newCate.assignTo( &cateFields, "name,desc,type,url,icon,startup,keywords" );

        m_catesDlg.DoAdd( pCurWnd, &cateFields );
    }
    else // �и�����
    {
        AccountArray accounts;
        int accountsCount = LoadAccounts( g_theApp.GetDatabase(), g_theApp.m_loginedUser.m_id, &accounts, m_catesDlg.m_cates[cateIndex].m_id );
        if ( accountsCount < 1 ) // û���˻�����
        {
            Account newAccount;
            newAccount.m_cateId = m_catesDlg.m_cates[cateIndex].m_id;
            newAccount.m_userId = g_theApp.m_loginedUser.m_id;

            winplus::Mixed accountFields;
            accountFields.addPair()
                ( "cate", m_catesDlg.m_cates[cateIndex].m_id )
                ( "user", g_theApp.m_loginedUser.m_id )
                ;

            DoAddAccount( pCurWnd, accountFields );
        }
        else // ���˻�����
        {
            CRect rcCurWnd;
            pCurWnd->GetClientRect(&rcCurWnd);
            // �����˻���Ϣ���ڵ�λ��
            CSize siAccountsWnd( 300, 412 );
            CPoint ptAccountsWnd( rcCurWnd.Width() - siAccountsWnd.cx, rcCurWnd.Height() - siAccountsWnd.cy );
            pCurWnd->ClientToScreen(&ptAccountsWnd);

            // ��ʾ�˻���Ϣ����
            auto pAccountsWnd = AccountComprehensiveWnd::GetWnd<AccountComprehensiveWnd>(*pCurWnd);
            if ( pAccountsWnd )
            {
                //pIntegratedWnd->SetAccountsInfo( m_catesDlg.m_cates[cateIndex], accounts );
                pAccountsWnd->SetWindowPos( NULL, ptAccountsWnd.x, ptAccountsWnd.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
                pCurWnd->SetWindowPos( &wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
            }
            else
            {
                pAccountsWnd = AccountComprehensiveWnd::Create<AccountComprehensiveWnd>(
                    *pCurWnd,
                    pCurWnd,
                    CRect( 0, 0, siAccountsWnd.cx, siAccountsWnd.cy ),
                    m_catesDlg.m_cates[cateIndex],
                    accounts
                );
                pAccountsWnd->AutoDelete(TRUE);
                pAccountsWnd->UpdateWindow();
                pAccountsWnd->ShowWindow(SW_NORMAL);

                pAccountsWnd->SetWindowPos( NULL, ptAccountsWnd.x, ptAccountsWnd.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
                pCurWnd->SetWindowPos( &wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
            }
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

void MainFrame::UpdateList( int flag, long itemIndex )
{
    IF_PTR(m_pAccountsView)->UpdateList( flag, itemIndex );
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

BOOL MainFrame::OnCmdMsg( UINT nID, int nCode, void * pExtra, AFX_CMDHANDLERINFO * pHandlerInfo )
{
    // let the view have first crack at the command
    if ( m_pAccountsView->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) )
        return TRUE;

    // otherwise, do default handling
    return CFrameWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

BEGIN_MESSAGE_MAP(MainFrame, CFrameWnd)
    //{{AFX_MSG_MAP(MainFrame)
    ON_WM_CREATE()
    ON_WM_SETFOCUS()
    ON_COMMAND(ID_APP_EXIT, OnAppExit)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    //ON_COMMAND(ID_APP_HELP, OnAppHelp)
    //ON_COMMAND(ID_HELP, OnHelp)
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
    ON_COMMAND(ID_BROWSERS, OnBrowsers)
    ON_COMMAND(ID_BACKUP_DATA, OnBackupData)
    ON_COMMAND(ID_RESUME_DATA, OnResumeData)
    ON_COMMAND(ID_MAINWND_SHOWHIDE, OnMainWndShowHide )
    ON_UPDATE_COMMAND_UI(ID_MAINWND_SHOWHIDE, OnUpdateMainWndShowHide)
    ON_UPDATE_COMMAND_UI(ID_ACCOUNT_CATES, OnUpdateAccountCates)
    ON_UPDATE_COMMAND_UI(ID_ACCOUNT_TYPES, OnUpdateAccountTypes)
    //}}AFX_MSG_MAP
    ON_MESSAGE( WM_HOTKEY, OnHotkey )
    ON_UPDATE_COMMAND_UI_RANGE(ID_MODIFY_ACCOUNT, ID_DEL_ACCOUNT, OnUpdateOperateAccount)
    ON_MESSAGE( WM_UPDATELIST_ALL, OnUpdateListAll )
    ON_MESSAGE(WM_TRAY_NOTIFICATION, OnTrayNotification)
    ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MainFrame message handlers

int MainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    if ( CFrameWnd::OnCreate(lpCreateStruct) == -1 )
        return -1;

    // ����һ���˻���ͼռ��������ܴ��ڵĿͻ�����
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

    // �����б�
    m_pAccountsView->UpdateList();

    // ����һ��������
    if ( !m_toolBar.CreateEx(
            this,
            TBSTYLE_FLAT | TBSTYLE_LIST | TBSTYLE_TRANSPARENT | TBSTYLE_TOOLTIPS,
            WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC
        ) || !m_toolBar.LoadToolBar(IDR_MAINFRAME)
    )
    {
        TRACE0("Failed to create toolbar\n");
        return -1;      // fail to create
    }

    // ���빤����ͼƬ
    // ��Դ�ļ��пɸı乤������ť��С,�ȽϷ���
    // ͼƬ�͹�������ť��СӦ��ƥ��
    CImageList imgList;
    imgList.Create( 48, 48, ILC_COLOR32 | ILC_MASK, 0, 1 );
    //winplus::ImageList_Add32bpp( imgList.GetSafeHandle(), IDB_TOOLBAR32, IMAGE_BITMAP );
    winplus::ImageList_AddAlphaImage( imgList.GetSafeHandle(), IDR_PNG_TOOLBAR32, _T("PNG") );
    m_toolBar.GetToolBarCtrl().SetImageList(&imgList);
    imgList.Detach();//*/

    // ����㲻��Ҫ��������ͣ��,��ɾ��������3��
    m_toolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_toolBar);

    // ����״̬��
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
    

    // ����ͼ��
    SetIcon( winplus::Icon( IDR_MAINFRAME, 16, 16 ), TRUE );

    // �Ѵ��ھ���ύ�������ڴ�,�Ա㼤��
    g_theApp.GetSharedMemory()->hMainWnd = GetSafeHwnd();

    this->UpdateTitle();
    // ע���ȼ�
    this->RefreshHotkey(g_theApp.m_loginedUser.m_hotkey);

    // ������ģ̬�Ի���, CatesDlg, TypesDlg, BrowsersDlg
    m_catesDlg.Create(this);
    m_catesDlg.UpdateWindow();
    m_typesDlg.Create(this);
    m_typesDlg.UpdateWindow();

    // ��������֪ͨ
    //static HICON hTrayIcon;
    //LoadIconMetric( AfxGetApp()->m_hInstance, MAKEINTRESOURCEW(IDR_MAINFRAME), LIM_SMALL, &hTrayIcon );
    static winplus::Icon trayIcon(IDR_MAINFRAME);

    m_noti.add( GetSafeHwnd(), 1, trayIcon, winplus::LoadString(IDR_MAINFRAME) );
    m_noti.setMessage(WM_TRAY_NOTIFICATION);
    //m_noti.setBalloonInfo( winplus::LoadString(IDR_MAINFRAME), "Cassette ��������" );
    m_noti.modify();

    return 0;
}

void MainFrame::OnDestroy() 
{
    this->RefreshHotkey(0);

    CFrameWnd::OnDestroy();
}

void MainFrame::OnSetFocus( CWnd * pOldWnd )
{
    // forward focus to the view window
    m_pAccountsView->SetFocus();
}

void MainFrame::OnAppExit()
{
    __super::OnClose();
}

void MainFrame::OnAppAbout()
{
    // ֻ�ܴ�һ������
    AboutWnd * pAbout = AboutWnd::GetSingleton(this);
    pAbout->ShowWindow(SW_NORMAL);
    pAbout->SetForegroundWindow();
    pAbout->SetFocus();
}

void MainFrame::OnHelp()
{
    cout << "OnHelp()\n";
}

void MainFrame::OnAppHelp()
{

}

void MainFrame::OnUpdateViaAutoLogin( CCmdUI * pCmdUI ) 
{
    pCmdUI->Enable(g_theApp.m_viaAutoLogin);
    //pCmdUI->SetCheck( g_theApp.m_viaAutoLogin );
    pCmdUI->SetText( _T("���Զ���¼") );
}

void MainFrame::OnViaAutoLogin() 
{
    g_theApp.m_viaAutoLogin = FALSE;
    g_theApp.m_settings.isAutoLogin = false;
    g_theApp.SaveSettings(CassetteApp::Setting_AutoLogin);
}

// �����²����˻�����ز˵�״̬ʱ����
void MainFrame::OnUpdateOperateAccount( CCmdUI * pCmdUI ) 
{
    CListCtrl & lst = this->m_pAccountsView->GetListCtrl();
    int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
    pCmdUI->Enable( index != -1 );
}

// ������OpenURL�˵�״̬ʱ����
void MainFrame::OnUpdateOpenUrl( CCmdUI * pCmdUI ) 
{
    CListCtrl & lst = this->m_pAccountsView->GetListCtrl();
    int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
    if ( index != -1 )
    {
        AccountCate cate;
        GetAccountCate( g_theApp.GetDatabase(), this->m_pAccountsView->m_accounts[index].m_cateId, &cate );
        if ( !cate.m_url.IsEmpty() && cate.m_startup != _T("����") )
        {
            pCmdUI->SetText( _T("��") + cate.m_cateName + _T("(&Go)\tCtrl+G") );
            pCmdUI->Enable(TRUE);
        }
        else
        {
            pCmdUI->Enable(FALSE);
        }
    }
    else
    {
        pCmdUI->SetText( _T("��URL(&Go)\tCtrl+G") );
        pCmdUI->Enable(FALSE);
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
        if ( cate.m_startup == _T("��վ") )
        {
            ShellExecute( NULL, _T("open"), cate.m_url, NULL, NULL, SW_NORMAL );
        }
        else if ( cate.m_startup == _T("���") )
        {
            ShellExecute( NULL, NULL, cate.m_url, NULL, NULL, SW_NORMAL );
        }
    }
}

void MainFrame::OnUserSettings() 
{
    VERIFY_ONCE_DIALOG(onceSettingsDlg);

    winplus::Mixed userFields;
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

    winplus::Mixed accountFields;
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

    winplus::Mixed accountFields;
    m_pAccountsView->m_accounts[index].assignTo( &accountFields, "myname,account_name,account_pwd,cate,user,safe_rank,comment,time" );

    AccountEditingDlg editingDlg( this, false, &accountFields );
    SetNullScopeOut setNullScopeOut( onceEditingDlg = &editingDlg );
    if ( IDOK == editingDlg.DoModal() )
    {
        if ( ModifyAccount( g_theApp.GetDatabase(), g_theApp.m_loginedUser.m_id, myName, accountFields ) )
        {
            m_pAccountsView->m_accounts[index] = accountFields;

            this->UpdateList( UPDATE_LIST_ITEMS, index );
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
    if ( IDYES == MessageBox( _T("�˲������ɻָ���ȷ��Ҫɾ����"), _T("ȷ��"), MB_YESNO ) )
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

void MainFrame::OnBrowsers()
{
    BrowsersDlg browsersDlg(this);
    switch ( browsersDlg.DoModal() )
    {
    case IDOK:
        break;
    default:
        break;
    }
}

void MainFrame::OnBackupData()
{
    static CString backupPath = ExplainCustomVars(g_theApp.m_settings.backupPath).c_str();
    winplus::FileDialog dlg( GetSafeHwnd(), FALSE, _T("�����뱸�ݺ�Ҫ�洢�ļ�������"), _T("bak") );
    if ( dlg.doModal( backupPath, _T("�����ļ�(*.bak)\0*.bak\0�����ļ�(*.*)\0*.*\0") ) )
    {
        backupPath = dlg.getDirPath().c_str();

        if ( g_theApp.BackupData( dlg.getFilePath().c_str() ) )
        {
            MessageBox( _T("���ݳɹ�"), _T("��ʾ"), MB_ICONINFORMATION );
        }
    }
}

void MainFrame::OnResumeData()
{
    static CString backupPath = ExplainCustomVars(g_theApp.m_settings.backupPath).c_str();
    winplus::FileDialog dlg( GetSafeHwnd(), TRUE, _T("������Ҫ�ָ����ļ�����") );
    if ( dlg.doModal( backupPath, _T("�����ļ�(*.bak)\0*.bak\0�����ļ�(*.*)\0*.*\0") ) )
    {
        backupPath = dlg.getDirPath().c_str();

        if ( g_theApp.ResumeData( dlg.getFilePath().c_str() ) )
        {
            MessageBox( _T("�ָ��ɹ�"), _T("��ʾ"), MB_ICONINFORMATION );
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

void MainFrame::OnClose()
{
    ShowWindow(SW_HIDE);
}

void MainFrame::OnUpdateMainWndShowHide(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck( winplus::Window_IsShow( GetSafeHwnd() ) );
}

void MainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
    __super::OnShowWindow(bShow, nStatus);
    if ( bShow )
    {
        BringWindowToTop();
        SetWindowPos( &CWnd::wndTop, 0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
        SetForegroundWindow();
    }
}

LRESULT MainFrame::OnUpdateListAll( WPARAM wParam, LPARAM lParam )
{
    if ( wParam & UPDATELIST_MAIN )
        this->UpdateList();
    if ( wParam & UPDATELIST_TYPESDLG )
        m_typesDlg.UpdateList();
    if ( wParam & UPDATELIST_CATESDLG )
        m_catesDlg.UpdateList();
    return 0L;
}

LRESULT MainFrame::OnTrayNotification( WPARAM wParam, LPARAM lParam )
{
    //USHORT x = GET_X_LPARAM(wParam), y = GET_Y_LPARAM(wParam);
    //winplus::String tips = winplus::FormatA("lw:%u, hw:%u, ll:%u, hl:%u\n", LOWORD(wParam), HIWORD(wParam), LOWORD(lParam), HIWORD(lParam) );
    //WriteFile( GetStdHandle(STD_OUTPUT_HANDLE), tips.c_str(), tips.length(), 0, 0 );

    switch ( LOWORD(lParam) )
    {
    case NIN_BALLOONUSERCLICK:
        //AfxMessageBox("NIN_BALLOONUSERCLICK");
        //m_noti.setBalloonInfo( "���������", winplus::Format("%u,%u",x,y) );
        //m_noti.modify();
        break;
    case NIN_SELECT:
    case NIN_KEYSELECT:
    case WM_LBUTTONUP:
        this->OnMainWndShowHide();
        break;
    case WM_RBUTTONUP:
    case WM_CONTEXTMENU:
        {
            SetForegroundWindow();
            CPoint pt;
            GetCursorPos(&pt);
            CMenu menu;
            menu.LoadMenu(IDM_TRAYNOTI_MENU);
            menu.GetSubMenu(0)->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_HORPOSANIMATION, pt.x, pt.y, this );

            /* menu.CreatePopupMenu();
            menu.AppendMenu( MF_STRING, ID_MAINWND_SHOWHIDE, "��ʾ/����" );
            menu.AppendMenu( MF_SEPARATOR );
            menu.AppendMenu( MF_STRING, ID_APP_EXIT, "�˳�����" );
            menu.TrackPopupMenu( TPM_RIGHTBUTTON | TPM_HORPOSANIMATION, x, y, this );*/
        }
        break;

    }
    return 0;
}
