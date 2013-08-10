
#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountsView.h"
#include "MainFrame.h"
#include "AboutView.h"
#include "AboutWnd.h"
#include "UserSettingsDlg.h"
#include "AppSettingsDlg.h"
#include "AccountEditingDlg.h"

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
	cs.lpszClass = AfxRegisterWndClass( 0, 0, (HBRUSH)GetStockObject(WHITE_BRUSH) );
	return TRUE;
}

BEGIN_MESSAGE_MAP(MainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(MainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_UPDATE_COMMAND_UI(ID_VIA_AUTOLOGIN, OnUpdateViaAutoLogin)
	ON_COMMAND(ID_VIA_AUTOLOGIN, OnViaAutoLogin)
	ON_UPDATE_COMMAND_UI(ID_OPEN_URL, OnUpdateOpenUrl)
	ON_COMMAND(ID_OPEN_URL, OnOpenUrl)
	ON_COMMAND(ID_USER_SETTINGS, OnUserSettings)
	ON_COMMAND(ID_APP_SETTINGS, OnAppSettings)
	ON_WM_DESTROY()
	ON_COMMAND(ID_ADD_ACCOUNT, OnAddAccount)
	ON_COMMAND(ID_MODIFY_ACCOUNT, OnModifyAccount)
	ON_COMMAND(ID_DEL_ACCOUNT, OnDelAccount)
	ON_COMMAND(ID_ACCOUNT_CATES, OnAccountCates)
	ON_COMMAND(ID_ACCOUNT_TYPES, OnAccountTypes)
	ON_COMMAND(ID_BACKUP_DATA, OnBackupData)
	ON_COMMAND(ID_RESUME_DATA, OnResumeData)
	ON_UPDATE_COMMAND_UI(ID_ACCOUNT_CATES, OnUpdateAccountCates)
	ON_UPDATE_COMMAND_UI(ID_ACCOUNT_TYPES, OnUpdateAccountTypes)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_UPDATELIST_ALL, OnUpdateListAll )
	ON_MESSAGE( WM_HOTKEY, OnHotkey )
	ON_UPDATE_COMMAND_UI_RANGE(ID_MODIFY_ACCOUNT, ID_DEL_ACCOUNT, OnUpdateOperateAccount)
END_MESSAGE_MAP()

void MainFrame::UpdateTitle()
{
	CString newTitle;
	newTitle.Format( _T("%s [%s]"), load_string(AFX_IDS_APP_TITLE).c_str(), (LPCTSTR)g_theApp.m_loginedUser.m_username );
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

void MainFrame::DoAddAccount( CWnd * parent )
{
	VERIFY_ONCE_DIALOG(onceEditingDlg);
	Account newAccount;
	newAccount.m_userId = g_theApp.m_loginedUser.m_id;
	newAccount.m_cateId = 0;
	newAccount.m_safeRank = 20;

	AccountEditingDlg editingDlg( parent, true, &newAccount );

	SetNullScopeOut setNullScopeOut( onceEditingDlg = &editingDlg );

	if ( IDOK == editingDlg.DoModal() )
	{
		if ( AddAccount( g_theApp.GetDatabase(), newAccount ) )
		{
			CListCtrl & lst = m_pAccountsView->GetListCtrl();
			// 向list加入一项
			int itemIndex;
			itemIndex = lst.GetItemCount();
			lst.InsertItem( itemIndex, newAccount.m_myName );
			
			// 向数组添加一项
			m_pAccountsView->m_accounts.Add(newAccount);

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
			/*TBSTYLE_FLAT*/0,
			WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC
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
	ImageList_Add32bpp( imgList.GetSafeHandle(), IDR_MAINFRAME, IMAGE_BITMAP );
	//ImageList_AddAlphaImage( imgList.GetSafeHandle(), IDR_PNG_TOOLBAR, _T("PNG") );
	m_toolBar.GetToolBarCtrl().SetImageList(&imgList);
	imgList.Detach();//*/


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
	
	// 如果你不想要工具条可停靠,可删除下面这3行
	m_toolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_toolBar);

	// 设置图标
	SetIcon( icon( IDR_MAINFRAME, 16, 16 ), TRUE );
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

	User user;
	user = g_theApp.m_loginedUser;

	UserSettingsDlg settingsDlg( this, &user );

	SetNullScopeOut setNullScopeOut( onceSettingsDlg = &settingsDlg );

	if ( IDOK == settingsDlg.DoModal() )
	{
		if ( ModifyUserEx(
			g_theApp.GetDatabase(),
			g_theApp.m_loginedUser.m_username,
			am_users__name |
			( settingsDlg.IsModifyPassword() ? am_users__pwd : 0 ) |
			am_users__protect |
			am_users__condone |
			am_users__cur_condone |
			am_users__hotkey,
			user
		) )
		{
			g_theApp.m_loginedUser = user;
			this->UpdateTitle();
			this->RefreshHotkey(user.m_hotkey);
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
			CassetteApp::Setting_BackupPath
		);
		g_theApp.DoSettings(
			CassetteApp::Setting_EnabledAutoRun |
			CassetteApp::Setting_EnabledHotkey |
			CassetteApp::Setting_EnabledHttpSrv |
			CassetteApp::Setting_EnabledScheme |
			CassetteApp::Setting_DatabasePath |
			CassetteApp::Setting_BackupPath
		);
		g_theApp.CloseDatabase();
		g_theApp.OpenDatabase();
		PostMessage( WM_UPDATELIST_ALL, -1, 0 );
		RefreshHotkey(g_theApp.m_loginedUser.m_hotkey);
	}
}

void MainFrame::OnAddAccount() 
{
	DoAddAccount(this);
}

void MainFrame::OnModifyAccount() 
{
	VERIFY_ONCE_DIALOG(onceEditingDlg);

	CListCtrl & lst = this->m_pAccountsView->GetListCtrl();
	int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
	CString myName = m_pAccountsView->m_accounts[index].m_myName;
	Account newAccount = m_pAccountsView->m_accounts[index];

	AccountEditingDlg editingDlg( this, false, &newAccount );

	SetNullScopeOut setNullScopeOut( onceEditingDlg = &editingDlg );

	if ( IDOK == editingDlg.DoModal() )
	{
		if ( ModifyAccount( g_theApp.GetDatabase(), g_theApp.m_loginedUser.m_id, myName, newAccount ) )
		{
			m_pAccountsView->m_accounts[index] = newAccount;

			UpdateList( UPDATE_LIST_ITEMS, index );
			LVFINDINFO fi;
			fi.flags = LVFI_PARTIAL | LVFI_STRING;
			fi.psz = newAccount.m_myName;
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
	pCmdUI->SetCheck( window_is_show( m_catesDlg.GetSafeHwnd() ) );
}

void MainFrame::OnUpdateAccountTypes( CCmdUI * pCmdUI ) 
{
	pCmdUI->SetCheck( window_is_show( m_typesDlg.GetSafeHwnd() ) );
}

void MainFrame::OnAccountCates() 
{
	if ( window_is_show( m_catesDlg.GetSafeHwnd() ) )
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
	if ( window_is_show( m_typesDlg.GetSafeHwnd() ) )
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
	file_dialog dlg( GetSafeHwnd(), FALSE, _T("请输入备份后要存储文件的名称"), _T("bak") );
	if ( dlg.do_modal( backupPath, _T("备份文件(*.bak)\0*.bak\0所有文件(*.*)\0*.*\0") ) )
	{
		backupPath = dlg.get_dirpath().c_str();

		if ( g_theApp.BackupData( dlg.get_filepath().c_str() ) )
		{
			MessageBox( _T("备份成功"), _T("提示"), MB_ICONINFORMATION );
		}
	}
}

void MainFrame::OnResumeData() 
{
	static CString backupPath = ExplainCustomVars(g_theApp.m_settings.backupPath).c_str();
	file_dialog dlg( GetSafeHwnd(), TRUE, _T("请输入要恢复的文件名称") );
	if ( dlg.do_modal( backupPath, _T("备份文件(*.bak)\0*.bak\0所有文件(*.*)\0*.*\0") ) )
	{
		backupPath = dlg.get_dirpath().c_str();

		if ( g_theApp.ResumeData( dlg.get_filepath().c_str() ) )
		{
			MessageBox( _T("恢复成功"), _T("提示"), MB_ICONINFORMATION );
		}
	}
}

LRESULT MainFrame::OnHotkey( WPARAM wHotkeyId, LPARAM lParam )
{
	switch ( wHotkeyId )
	{
	case IDC_REGISTERED_HOTKEY:
		{
			// 取得当前活动的前台窗口
			CWnd * pWnd = GetForegroundWindow();
			if ( pWnd )
			{
				// 通过HWND获取进程句柄,进而获取程序路径
				// 判断是浏览器还是软件
				string exeName;
				file_path( GetAppPathFromHWND( pWnd->GetSafeHwnd() ), &exeName );
				strlwr(&exeName[0]);
				bool isBrowser = IsBrowserExeName( g_theApp.GetDatabase(), exeName.c_str(), NULL );
				string caption = window_get_text( pWnd->GetSafeHwnd() ); // 当前窗口标题
				if ( isBrowser ) // 若是浏览器，说明是网站
				{

				}
				else // 否则为软件
				{

				}
				m_catesDlg.DoAdd(pWnd);

			}
		}

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
