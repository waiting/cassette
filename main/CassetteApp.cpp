
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

#ifdef StrTrim
#undef StrTrim
#endif

// The one and only CassetteApp object
CassetteApp g_theApp;
// GDI+ initialize
winplus::GdiplusInit g_initGdiplus;
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
    m_wordslib = NULL;
    m_viaAutoLogin = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CassetteApp initialization
BOOL CassetteApp::InitInstance()
{
    AfxEnableControlContainer();
    InitCommonControls();
    AfxOleInit();
    //AfxOleInit();
    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.

#ifdef _AFXDLL
    Enable3dControls();         // Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();   // Call this when linking to MFC statically
#endif

    winplus::StringArray argArr;
    winplus::CommandArguments(&argArr);
    std::vector<const char *> args;
    for ( auto && arg : argArr )
    {
        args.push_back( arg.c_str() );
    }
    args.push_back(nullptr);

    // 命令行解释
    winplus::CommandLineVars cmd( (int)argArr.size(), &args[0], "-url", "", "--verbose" );
    if ( cmd.hasParam("-url") )
    {
        winplus::Win32GUI_ShowConsole();

        std::cout << cmd.getParam("-url") << std::endl;

        system("pause");
        return FALSE;
    }
    if ( cmd. hasFlag("--verbose") )
    {
        winplus::Win32GUI_ShowConsole();
    }

    if ( !DoSingletonRunning() ) return FALSE;

    // ----------------------------------------------------------------------------------
    LoadSettings(); // 从ini加载设置
    DoSettings();   // 处理设置选项

    BOOL isLogined = FALSE; // 是否已经登录
    m_viaAutoLogin = FALSE;

    CString username, password;
    BOOL isAutoLogin = m_settings.isAutoLogin, isSavePassword = m_settings.isSavePassword;

    if ( isSavePassword ) // 载入保存的用户和密码
    {
        username = winplus::DecryptContent( winplus::Base64Decode(m_settings.username) ).c_str();
        password = winplus::DecryptContent( winplus::Base64Decode(m_settings.password) ).c_str();
    }

    if ( isAutoLogin && !username.IsEmpty() && LoginUser( g_theApp.GetDatabase(), username, password, &m_loginedUser ) ) // 自动登录
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
            if ( LoginUser( g_theApp.GetDatabase(), username, password, &m_loginedUser ) )
            {
                // 保存登录相关设置
                m_settings.isAutoLogin = isAutoLogin != FALSE;
                m_settings.isSavePassword = isSavePassword != FALSE;
                if ( isSavePassword ) // 保存用户和密码
                {
                    m_settings.username = winplus::Base64Encode( winplus::EncryptContent( (LPCSTR)username ) );
                    m_settings.password = winplus::Base64Encode( winplus::EncryptContent( (LPCSTR)password ) );
                }
                else
                {
                    m_settings.username = _T("");
                    m_settings.password = _T("");
                }
                this->SaveSettings( Setting_AutoLogin | Setting_SavePassword | Setting_Username | Setting_Password );

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
    CloseDatabase();
    CloseWordslib();

    return CWinApp::ExitInstance();
}

//////////////////////////////////////////////////////////////////////////

void CassetteApp::InitDatabaseSchema()
{
    winplus::AnsiString sql;
    winplus::StringArray sqls;
    int nSQLs, i;
    winplus::Resource ResSQL( IDR_SQL_DBSCHEMA, _T("SQL") );
    if ( !ResSQL ) goto OccurDbError;
    sql.resize( ResSQL.getSize() );
    if ( sql.size() ) ResSQL.copyTo( &sql[0], (DWORD)sql.size() );
    nSQLs = (int)winplus::StrSplit( sql, _T(";"), &sqls );
    try
    {
        for ( i = 0; i < nSQLs; ++i )
        {
            if ( winplus::StrTrimA(sqls[i]).empty() ) continue;
            m_db->cnn()->exec( sqls[i].c_str() );
        }
    }
    catch ( winplus::Error const & e )
    {
        AfxMessageBox( e.what(), MB_ICONERROR );
    }

    return;

OccurDbError:
    AfxMessageBox( _T("未能成功初始化数据库"), MB_ICONERROR );
}

void CassetteApp::OpenDatabase()
{
    winplus::String databasePath = ExplainCustomVars(m_settings.databasePath);
    winplus::AnsiString encryptKey;
    winplus::Resource objResDbKey;
    // 从资源加载KEY
    if ( objResDbKey.load( IDR_KEY_DB, _T("KEY") ) )
    {
        encryptKey.resize( objResDbKey.getSize() );
        CopyMemory( &encryptKey[0], objResDbKey.getData(), encryptKey.size() );
    }
    // 判断数据库文件是否存在，不存在则需要初始化
    bool isNeedInit = !winplus::DetectPath(databasePath);

    try
    {
        m_db = new eiendb::Database( winplus::$c{
            { "driver", "sqlite" },
            { "path", databasePath },
            { "dbkey", encryptKey },
            { "charset", "" }
        } );
    }
    catch ( winplus::Error const & e )
    {
        AfxMessageBox( e.what(), MB_ICONERROR );
    }

    if ( isNeedInit )
    {
        InitDatabaseSchema();
    }
}

void CassetteApp::CloseDatabase()
{
    if ( m_db != NULL )
    {
        delete m_db;
        m_db = NULL;
    }
}

void CassetteApp::OpenWordslib()
{
    winplus::String wordslibPath = ExplainCustomVars(m_settings.wordslibPath);
    try
    {
        m_wordslib = new winplus::WordsLib(wordslibPath);
    }
    catch ( winplus::WordsLibException const & e )
    {
        AfxMessageBox( e.what() );
    }
}

void CassetteApp::CloseWordslib()
{
    if ( m_wordslib != NULL )
    {
        delete m_wordslib;
        m_wordslib = NULL;
    }
}

bool CassetteApp::BackupData( CString const & filename )
{
    bool ret = false;
    CloseDatabase();
    ret = FALSE != CopyFile( ExplainCustomVars(g_theApp.m_settings.databasePath).c_str(), filename, FALSE );
    OpenDatabase();
    return ret;
}

bool CassetteApp::ResumeData( CString const & filename )
{
    bool ret = false;
    CloseDatabase();
    ret = FALSE != CopyFile( filename, ExplainCustomVars(g_theApp.m_settings.databasePath).c_str(), FALSE );
    OpenDatabase();
    return ret;
}

void CassetteApp::LoadSettings( UINT flag )
{
    winplus::Ini settingsIni( winplus::ModulePath() + winplus::dirSep + winplus::LoadString(AFX_IDS_APP_TITLE) + TEXT(".ini") );
    winplus::String sname = winplus::LoadString(AFX_IDS_APP_TITLE);
    winplus::Ini::Section s = settingsIni( sname.c_str() );

    if ( flag & Setting_EnabledAutoRun ) m_settings.isEnabledAutoRun = winplus::Mixed( s.get( _T("EnabledAutoRun"), _T("false") ) );
    if ( flag & Setting_EnabledHotkey ) m_settings.isEnabledHotkey = winplus::Mixed( s.get( _T("EnabledHotkey"), _T("true") ) );
    if ( flag & Setting_EnabledHttpSrv ) m_settings.isEnabledHttpSrv = winplus::Mixed( s.get( _T("EnabledHttpSrv"), _T("false") ) );
    if ( flag & Setting_EnabledScheme ) m_settings.isEnabledScheme = winplus::Mixed( s.get( _T("EnabledScheme"), _T("false") ) );
    if ( flag & Setting_DatabasePath ) m_settings.databasePath = s.get( _T("DatabasePath"), winplus::Format( _T("$ROOT$\\%s.db"), winplus::LoadString(AFX_IDS_APP_TITLE).c_str() ) );
    if ( flag & Setting_BackupPath ) m_settings.backupPath = s.get( _T("BackupPath"), _T("$ROOT$") );
    if ( flag & Setting_WordslibPath ) m_settings.wordslibPath = s.get( _T("WordslibPath"), _T("$ROOT$\\words.wl") );

    if ( flag & Setting_AutoLogin ) m_settings.isAutoLogin = winplus::Mixed( s.get( _T("AutoLogin"), _T("false") ) );
    if ( flag & Setting_SavePassword ) m_settings.isSavePassword = winplus::Mixed( s.get( _T("SavePassword"), _T("false") ) );

    if ( flag & Setting_Username ) m_settings.username = s.get( _T("Username"), _T("") );
    if ( flag & Setting_Password ) m_settings.password = s.get( _T("Password"), _T("") );

}

void CassetteApp::SaveSettings( UINT flag )
{
    winplus::Ini settingsIni( winplus::ModulePath() + winplus::dirSep + winplus::LoadString(AFX_IDS_APP_TITLE) + TEXT(".ini") );
    winplus::String sname = winplus::LoadString(AFX_IDS_APP_TITLE);
    winplus::Ini::Section s = settingsIni( sname.c_str() );

    if ( flag & Setting_EnabledAutoRun ) s.set( _T("EnabledAutoRun"), (winplus::String)winplus::Mixed(m_settings.isEnabledAutoRun) );
    if ( flag & Setting_EnabledHotkey ) s.set( _T("EnabledHotkey"), (winplus::String)winplus::Mixed(m_settings.isEnabledHotkey) );
    if ( flag & Setting_EnabledHttpSrv ) s.set( _T("EnabledHttpSrv"), (winplus::String)winplus::Mixed(m_settings.isEnabledHttpSrv) );
    if ( flag & Setting_EnabledScheme ) s.set( _T("EnabledScheme"), (winplus::String)winplus::Mixed(m_settings.isEnabledScheme) );
    if ( flag & Setting_DatabasePath ) s.set( _T("DatabasePath"), m_settings.databasePath );
    if ( flag & Setting_BackupPath ) s.set( _T("BackupPath"), m_settings.backupPath );
    if ( flag & Setting_WordslibPath ) s.set( _T("WordslibPath"), m_settings.wordslibPath );

    if ( flag & Setting_AutoLogin ) s.set( _T("AutoLogin"), (winplus::String)winplus::Mixed(m_settings.isAutoLogin) );
    if ( flag & Setting_SavePassword ) s.set( _T("SavePassword"), (winplus::String)winplus::Mixed(m_settings.isSavePassword) );

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
    if ( flag & Setting_EnabledAutoRun )
    {
        // 是否开机自启动
        EnableAutoRun(m_settings.isEnabledAutoRun);
    }
    if ( flag & Setting_DatabasePath )
    {
        CloseDatabase();
        OpenDatabase();
    }
    if ( flag & Setting_WordslibPath )
    {
        CloseWordslib();
        OpenWordslib();
    }
}

void CassetteApp::EnableAutoRun( bool isEnabled, bool isForce /*= false */ )
{
    LPCSTR keyNameAutoRun = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
    winplus::AnsiString appName = winplus::StringToLocal( winplus::LoadString(AFX_IDS_APP_TITLE) );
    if ( isEnabled )
    {
        if ( isForce || !reg_exists( keyNameAutoRun, appName.c_str() ) )
        {
            winplus::String appFullPath;
            appFullPath = winplus::ModulePath( NULL, &appFullPath ) + winplus::dirSep + appFullPath;
            if ( appFullPath.find(' ') != winplus::String::npos )
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
    winplus::AnsiString keyNameScheme = "HKEY_CLASSES_ROOT\\account";
    if ( isEnabled )
    {
        if ( !reg_exists( keyNameScheme.c_str(), NULL ) )
        {
            reg_set_string( keyNameScheme.c_str(), NULL, "Account URL Protocol" );
            reg_set_string( keyNameScheme.c_str(), "URL Protocol", "" );
            winplus::String appFullPath;
            appFullPath = winplus::ModulePath( NULL, &appFullPath ) + winplus::dirSep + appFullPath;
            reg_set_string( ( keyNameScheme + "\\DefaultIcon" ).c_str(), NULL, ( appFullPath + ",0" ).c_str() );

            if ( appFullPath.find(' ') != winplus::String::npos )
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
    winplus::String sharedMemName = winplus::LoadString(IDS_SHAREDMEM_NAME);
    if ( m_sharedMem.open( sharedMemName.c_str() ) )
    {
        HWND hwndMain = m_sharedMem->hMainWnd;

        if ( winplus::Window_IsShow(hwndMain) )
            SetForegroundWindow(hwndMain);
        else
        {
            winplus::Window_Show(hwndMain);
            SetForegroundWindow(hwndMain);
        }
        return FALSE;
    }
    else
    {
        m_sharedMem.create( sharedMemName.c_str(), sizeof(CassetteSharedData) );
    }
    return TRUE;
}
