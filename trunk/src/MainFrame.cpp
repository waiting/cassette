
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
	ON_WM_DESTROY()
	ON_COMMAND(ID_ADD_ACCOUNT, OnAddAccount)
	ON_COMMAND(ID_MODIFY_ACCOUNT, OnModifyAccount)
	ON_COMMAND(ID_DEL_ACCOUNT, OnDelAccount)
	ON_COMMAND(ID_ACCOUNT_CATES, OnAccountCates)
	ON_COMMAND(ID_ACCOUNT_TYPES, OnAccountTypes)
	ON_COMMAND(ID_APP_SETTINGS, OnAppSettings)
	ON_COMMAND(ID_BACKUP_DATA, OnBackupData)
	ON_COMMAND(ID_RESUME_DATA, OnResumeData)
	//}}AFX_MSG_MAP
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
	// create a view to occupy the client area of the frame
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
	//msgbox( format( "%x", m_pAccountsView->GetDlgCtrlID() ) );

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
	
	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
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
	AboutWnd * pAbout = AboutWnd::GetAboutWnd(this);
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
		CString startup, cateName, url;
		GetAccountCate( this->m_pAccountsView->m_cateIds[index], &cateName, NULL, NULL, &url, NULL, &startup, NULL, NULL );
		if ( !url.IsEmpty() && startup != _T("其他") )
		{
			pCmdUI->SetText( _T("打开") + cateName + _T("(&Go)\tCtrl+G") );
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
	int cateId = this->m_pAccountsView->m_cateIds[index];
	CString url, startup;
	if ( GetAccountCate( cateId, NULL, NULL, NULL, &url, NULL, &startup, NULL, NULL ) )
	{
		if ( startup == _T("网站") )
		{
			ShellExecute( NULL, _T("open"), url, NULL, NULL, SW_NORMAL );
		}
		else if ( startup == _T("软件") )
		{
			ShellExecute( NULL, NULL, url, NULL, NULL, SW_NORMAL );
		}
	}
}

void MainFrame::OnUserSettings() 
{
	CString username = g_theApp.m_loginedUser.m_username;
	CString password;
	int protectLevel = g_theApp.m_loginedUser.m_protectLevel;
	int condone = g_theApp.m_loginedUser.m_condone;
	int hotkey = g_theApp.m_loginedUser.m_hotkey;

	UserSettingsDlg settingsDlg( this, &username, &password, &protectLevel, &condone, &hotkey );

	if ( IDOK == settingsDlg.DoModal() )
	{
		if ( ModifyUser( g_theApp.m_loginedUser.m_username, settingsDlg.IsModifyPassword(), username, password, protectLevel, condone, g_theApp.m_loginedUser.m_curCondone, hotkey ) )
		{
			g_theApp.m_loginedUser.m_username = username;
			this->UpdateTitle();
			g_theApp.m_loginedUser.m_protectLevel = protectLevel;
			g_theApp.m_loginedUser.m_condone = condone;
			g_theApp.m_loginedUser.m_hotkey = hotkey;
			this->RefreshHotkey(hotkey);
		}
	}

}

void MainFrame::OnAppSettings() 
{
	AppSettingsDlg settingDlg( this, &g_theApp.m_settings );
	if ( IDOK == settingDlg.DoModal() )
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
		RefreshHotkey(g_theApp.m_loginedUser.m_hotkey);
	}
}

void MainFrame::OnAddAccount() 
{
	CString myName;
	CString accountName;
	CString accountPwd;
	int cateId = 0;
	int safeRank = 20;
	CString comment;

	AccountEditingDlg editingDlg( this, true, &myName, &accountName, &accountPwd, &cateId, &safeRank, &comment );
	if ( IDOK == editingDlg.DoModal() )
	{
		if ( AddAccount( myName, accountName, accountPwd, cateId, safeRank, comment ) )
		{
			CListCtrl & lst = m_pAccountsView->GetListCtrl();
			// 向list加入一项
			int itemIndex;
			itemIndex = lst.GetItemCount();
			lst.InsertItem( itemIndex, myName );

			// 向数组添加一项
			m_pAccountsView->m_myNames.Add(myName);
			m_pAccountsView->m_accountNames.Add("");
			m_pAccountsView->m_accountPwds.Add("");
			m_pAccountsView->m_cateIds.Add(0);
			m_pAccountsView->m_safeRanks.Add(0);
			m_pAccountsView->m_times.Add(0);
			m_pAccountsView->m_comments.Add("");

			UpdateList( UPDATE_LOAD_DATA | UPDATE_LIST_ITEMS, itemIndex );
			lst.SetItemState( itemIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		}
	}
}

void MainFrame::OnModifyAccount() 
{
	CListCtrl & lst = this->m_pAccountsView->GetListCtrl();
	int index = lst.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );
	CString myName = m_pAccountsView->m_myNames[index];
	CString newMyName = m_pAccountsView->m_myNames[index];
	CString newAccountName = m_pAccountsView->m_accountNames[index];
	CString newAccountPwd = m_pAccountsView->m_accountPwds[index];
	int newCateId = m_pAccountsView->m_cateIds[index];
	int newSafeRank = m_pAccountsView->m_safeRanks[index];
	CString newComment = m_pAccountsView->m_comments[index];

	AccountEditingDlg editingDlg( this, false, &newMyName, &newAccountName, &newAccountPwd, &newCateId, &newSafeRank, &newComment );
	if ( IDOK == editingDlg.DoModal() )
	{
		if ( ModifyAccount( myName, newMyName, newAccountName, newAccountPwd, newCateId, newSafeRank, newComment ) )
		{
			m_pAccountsView->m_myNames[index] = newMyName;
			m_pAccountsView->m_accountNames[index] = newAccountName;
			m_pAccountsView->m_accountPwds[index] = newAccountPwd;
			m_pAccountsView->m_cateIds[index] = newCateId;
			m_pAccountsView->m_safeRanks[index] = newSafeRank;
			m_pAccountsView->m_comments[index] = newComment;

			UpdateList( UPDATE_LIST_ITEMS, index );
			LVFINDINFO fi;
			fi.flags = LVFI_PARTIAL | LVFI_STRING;
			fi.psz = newMyName;
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
		if ( DeleteAccount(myName) )
		{
			lst.DeleteItem(index);

			m_pAccountsView->m_myNames.RemoveAt(index);
			m_pAccountsView->m_accountNames.RemoveAt(index);
			m_pAccountsView->m_accountPwds.RemoveAt(index);
			m_pAccountsView->m_cateIds.RemoveAt(index);
			m_pAccountsView->m_safeRanks.RemoveAt(index);
			m_pAccountsView->m_comments.RemoveAt(index);
			m_pAccountsView->m_times.RemoveAt(index);

			index = index < lst.GetItemCount() ? index : lst.GetItemCount() - 1;
			lst.SetItemState( index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		}
	}
}

void MainFrame::OnAccountCates() 
{
	//AccountCatesDlg catesDlg(this);
	m_catesDlg.DoModal();
}

void MainFrame::OnAccountTypes() 
{
	//AccountTypesDlg typesDlg(this);
	m_typesDlg.DoModal();
}

void MainFrame::OnBackupData() 
{
	static CString backupPath = ExplainCustomVars(g_theApp.m_settings.backupPath).c_str();
	file_dialog dlg( GetSafeHwnd(), FALSE, _T("请输入备份后要存储文件的名称"), _T("bak") );
	if ( dlg.do_modal( backupPath, _T("备份文件(*.bak)\0*.bak\0所有文件(*.*)\0*.*\0") ) )
	{
		backupPath = dlg.get_dirpath().c_str();

		if ( BackupData( dlg.get_filepath().c_str() ) )
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

		if ( ResumeData( dlg.get_filepath().c_str() ) )
		{
			MessageBox( _T("恢复成功"), _T("提示"), MB_ICONINFORMATION );
		}
	}
}

LRESULT MainFrame::OnHotkey( WPARAM wParam, LPARAM lParam )
{
	switch ( wParam )
	{
	case IDC_REGISTERED_HOTKEY:
		PostMessage( WM_COMMAND, MAKEWPARAM( ID_ACCOUNT_CATES, 0 ), 0 );
		//MessageBox("user pressed hotkey");
		
		//PostMessage( WM_COMMAND, MAKEWPARAM( ID_ACCOUNT_TYPES, 0 ), 0 );
		m_TestFlag = !m_TestFlag;
		break;
	}
	return 0L;
}
