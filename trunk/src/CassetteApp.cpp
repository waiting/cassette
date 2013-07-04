
#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountsView.h"
#include "MainFrame.h"
#include "UserLoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CassetteApp

BEGIN_MESSAGE_MAP(CassetteApp, CWinApp)
	//{{AFX_MSG_MAP(CassetteApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CassetteApp construction

CassetteApp::CassetteApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_db = NULL;
	m_viaAutoLogin = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CassetteApp initialization
BOOL CassetteApp::InitInstance()
{
	AfxEnableControlContainer();
	InitCommonControls();
	//AfxOleInit();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	
	// 命令行解释
	command_line cmd;
	if ( cmd.include("url") )
	{
		win32gui_show_console();
		
		std::cout << cmd.get_param("url") << std::endl;
		
		system("pause");
		return FALSE;
	}
	
	if ( !DoSingletonRunning() ) return FALSE;
	
	// 以下为启动GUI的代码 ------------------------------------------------------
	this->LoadSettings(); // 加载ini设置
	this->DoSettings();   // 处理设置选项
	this->InitDatabase(); // 初始化数据库

	BOOL isLogined = FALSE; // 是否已经登录
	m_viaAutoLogin = FALSE;

	CString username, password;
	BOOL isAutoLogin = m_settings.isAutoLogin, isSavePassword = m_settings.isSavePassword;

	if ( isSavePassword ) // 载入保存的用户和密码
	{
		username = DecryptContent( Base64Decode(m_settings.username) ).c_str();
		password = DecryptContent( Base64Decode(m_settings.password) ).c_str();
	}

	if ( isAutoLogin && !username.IsEmpty() && LoginUser( username, password, &m_loginedUser ) ) // 自动登录
	{
		m_viaAutoLogin = TRUE;
		isLogined = TRUE;
	}
	else
	{
		// 登录对话框
		UserLoginDlg loginDlg( NULL, &username, &password, &isAutoLogin, &isSavePassword );
		
		while ( loginDlg.DoModal() == IDOK )
		{
			// 保存相关设置
			m_settings.isAutoLogin = isAutoLogin != FALSE;
			m_settings.isSavePassword = isSavePassword != FALSE;
			if ( isSavePassword ) // 保存用户和密码
			{
				m_settings.username = Base64Encode( EncryptContent( (LPCSTR)username ) );
				m_settings.password = Base64Encode( EncryptContent( (LPCSTR)password ) );
			}
			else
			{
				m_settings.username = _T("");
				m_settings.password = _T("");
			}
			this->SaveSettings( Setting_AutoLogin | Setting_SavePassword | Setting_Username | Setting_Password );
			
			if ( LoginUser( username, password, &m_loginedUser ) )
			{
				isLogined = TRUE;
				break;
			}
		}
	}

	if ( isLogined ) // 若已经登录,则显示主界面
	{
		MainFrame * pFrame = new MainFrame;
		m_pMainWnd = pFrame;
		// create and load the frame with its resources
		pFrame->LoadFrame( IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL );
		// The one and only window has been initialized, so show and update it.
		pFrame->UpdateWindow();
		pFrame->ShowWindow(this->m_nCmdShow);
		return TRUE;
	}
	return FALSE;

}

int CassetteApp::ExitInstance()
{
	this->FreeDatabase();
	
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CassetteApp object
CassetteApp g_theApp;

//////////////////////////////////////////////////////////////////////////
// some operates
void CassetteApp::InitDatabase()
{
	int rc;
	string databasePath = ExplainCustomVars(m_settings.databasePath);
	ansi_string encryptKey;

	sqlite3 * db;
	const char * localError = "Out of memory";
	rc = sqlite3_open_v2( string_to_utf8(databasePath).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
	if ( rc != SQLITE_OK )
	{
		goto OccurDbError;
	}
	rc = sqlite3_extended_result_codes( db, 1 );
	if ( rc != SQLITE_OK )
	{
		goto OccurDbError;
	}
	// 如果有密码
	if ( encryptKey.size() > 0 )
	{
		rc = sqlite3_key( db, encryptKey.c_str(), (int)encryptKey.size() );
		if ( rc != SQLITE_OK )
		{
			goto OccurDbError;
		}
	}
	m_db = db;
	return;

OccurDbError:
	if ( db )
	{
		localError = sqlite3_errmsg(db);
		sqlite3_close(db);
	}
	AfxMessageBox( utf8_to_string(localError).c_str() );
}

void CassetteApp::FreeDatabase()
{
	if ( m_db != NULL )
	{
		sqlite3_close(m_db);
		m_db = NULL;
	}
}

void CassetteApp::LoadSettings( UINT flag )
{
	ini settingsIni( module_path() + dir_sep + load_string(AFX_IDS_APP_TITLE) + TEXT(".ini") );
	string sname = load_string(AFX_IDS_APP_TITLE);
	ini::section & s = settingsIni( sname.c_str() );

	if ( flag & Setting_AutoRun ) m_settings.isAutoRun = mixed( s.get( _T("AutoRun"), _T("false") ) );
	if ( flag & Setting_EnabledHotkey ) m_settings.isEnabledHotkey = mixed( s.get( _T("EnabledHotkey"), _T("true") ) );
	if ( flag & Setting_EnabledHttpSrv ) m_settings.isEnabledHttpSrv = mixed( s.get( _T("EnabledHttpSrv"), _T("false") ) );
	if ( flag & Setting_EnabledScheme ) m_settings.isEnabledScheme = mixed( s.get( _T("EnabledScheme"), _T("true") ) );
	if ( flag & Setting_DatabasePath ) m_settings.databasePath = s.get( _T("DatabasePath"), format( _T("$ROOT$\\%s.db"), load_string(AFX_IDS_APP_TITLE).c_str() ) );
	if ( flag & Setting_BackupPath ) m_settings.backupPath = s.get( _T("BackupPath"), _T("$ROOT$") );
	if ( flag & Setting_AutoLogin ) m_settings.isAutoLogin = mixed( s.get( _T("AutoLogin"), _T("false") ) );
	if ( flag & Setting_SavePassword ) m_settings.isSavePassword = mixed( s.get( _T("SavePassword"), _T("false") ) );

	if ( flag & Setting_Username ) m_settings.username = s.get( _T("Username"), _T("") );
	if ( flag & Setting_Password ) m_settings.password = s.get( _T("Password"), _T("") );

}

void CassetteApp::SaveSettings( UINT flag )
{
	ini settingsIni( module_path() + dir_sep + load_string(AFX_IDS_APP_TITLE) + TEXT(".ini") );
	string sname = load_string(AFX_IDS_APP_TITLE);
	ini::section & s = settingsIni( sname.c_str() );

	if ( flag & Setting_AutoRun ) s.set( _T("AutoRun"), (string)mixed(m_settings.isAutoRun) );
	if ( flag & Setting_EnabledHotkey ) s.set( _T("EnabledHotkey"), (string)mixed(m_settings.isEnabledHotkey) );
	if ( flag & Setting_EnabledHttpSrv ) s.set( _T("EnabledHttpSrv"), (string)mixed(m_settings.isEnabledHttpSrv) );
	if ( flag & Setting_EnabledScheme ) s.set( _T("EnabledScheme"), (string)mixed(m_settings.isEnabledScheme) );
	if ( flag & Setting_DatabasePath ) s.set( _T("DatabasePath"), m_settings.databasePath );
	if ( flag & Setting_BackupPath ) s.set( _T("BackupPath"), m_settings.backupPath );
	if ( flag & Setting_AutoLogin ) s.set( _T("AutoLogin"), (string)mixed(m_settings.isAutoLogin) );
	if ( flag & Setting_SavePassword ) s.set( _T("SavePassword"), (string)mixed(m_settings.isSavePassword) );

	if ( flag & Setting_Username ) s.set( _T("Username"), m_settings.username );
	if ( flag & Setting_Password ) s.set( _T("Password"), m_settings.password );
}

void CassetteApp::DoSettings( UINT flag )
{
	if ( flag & Setting_EnabledScheme )
	{
		// 是否开启scheme服务
		EnableScheme(m_settings.isEnabledScheme);
	}
	if ( flag & Setting_EnabledHttpSrv )
	{
		// 是否开启http服务
		EnableHttpService(m_settings.isEnabledHttpSrv);
	}
	if ( flag & Setting_AutoRun )
	{
		// 是否开机自启动
		EnableAutoRun(m_settings.isAutoRun);
	}
}

void CassetteApp::EnableAutoRun( bool isEnabled, bool isForce /*= false */ )
{
	LPCSTR keyNameAutoRun = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	ansi_string appName = string_to_local( load_string(AFX_IDS_APP_TITLE) );
	if ( isEnabled )
	{
		if ( isForce || !reg_exists( keyNameAutoRun, appName.c_str() ) )
		{
			string appFullPath;
			appFullPath = module_path( NULL, &appFullPath ) + dir_sep + appFullPath;
			if ( appFullPath.find(' ') != string::npos )
				appFullPath = TEXT("\"") + appFullPath + TEXT("\"");

			reg_set_string( keyNameAutoRun, appName.c_str(), appFullPath.c_str() );
		}
	}
	else
	{
		if ( isForce || reg_exists( keyNameAutoRun, appName.c_str() ) )
		{
			reg_delete( keyNameAutoRun, appName.c_str() );
		}
	}
}

void CassetteApp::EnableHttpService( bool isEnabled )
{
	
}

void CassetteApp::EnableScheme( bool isEnabled )
{
	ansi_string keyNameScheme = "HKEY_CLASSES_ROOT\\account";
	if ( isEnabled )
	{
		if ( !reg_exists( keyNameScheme.c_str(), NULL ) )
		{
			reg_set_string( keyNameScheme.c_str(), NULL, "Account URL Protocol" );
			reg_set_string( keyNameScheme.c_str(), "URL Protocol", "" );
			string appFullPath;
			appFullPath = module_path( NULL, &appFullPath ) + dir_sep + appFullPath;
			reg_set_string( ( keyNameScheme + "\\DefaultIcon" ).c_str(), NULL, ( appFullPath + ",0" ).c_str() );

			if ( appFullPath.find(' ') != string::npos )
				appFullPath = TEXT("\"") + appFullPath + TEXT("\"");

			reg_set_string( ( keyNameScheme + "\\shell\\open\\command" ).c_str(), NULL, ( appFullPath + " -url \"%1\"" ).c_str() );
		}
	}
	else
	{
		if ( reg_exists( keyNameScheme.c_str(), NULL ) )
		{
			reg_force_delete( keyNameScheme.c_str() );
		}
	}
}

BOOL CassetteApp::DoSingletonRunning()
{
	string sharedMemName = format( _T("%s - "), load_string(AFX_IDS_APP_TITLE).c_str() ) + get_self_module_version();
	if ( m_sharedMem.open( sharedMemName.c_str() ) )
	{
		HWND hwndMain = m_sharedMem->hMainWnd;
		
		if ( window_is_show(hwndMain) )
			SetForegroundWindow(hwndMain);
		else
		{
			window_show(hwndMain);
			SetForegroundWindow(hwndMain);
		}
		return FALSE;
	}
	else
	{
		m_sharedMem.create( sharedMemName.c_str(), sizeof(SharedData) );
	}
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CassetteApp message handlers
