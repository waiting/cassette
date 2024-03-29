
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

#ifdef _AFXDLL
    Enable3dControls();         // Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();   // Call this when linking to MFC statically
#endif

    winplus::StringArray argArr;
    winplus::CommandArgumentArray(&argArr);
    auto args = winplus::CommandArgs(&argArr);

    // 命令行解释
    winplus::CommandLineVars cmd( (int)argArr.size(), &args[0], "-url", "", "--verbose" );
    // 解释URL
    if ( cmd.hasParam("-url") )
    {
        winplus::Win32GUI_ShowConsole();

        std::cout << cmd.getParam("-url") << std::endl;

        system("pause");
        return FALSE;
    }
    // 开启命令行窗口输出详细信息
    if ( cmd. hasFlag("--verbose") )
    {
        winplus::Win32GUI_ShowConsole();
    }

    // 单例运行
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
        MainFrame * pFrame = new MainFrame();
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
            if ( winplus::StrTrim(sqls[i]).empty() ) continue;
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
        winplus::AnsiString keyJson;
        keyJson.assign( objResDbKey.getData<char>(), objResDbKey.getSize() );
        encryptKey = winplus::Mixed().json(keyJson).get("key");
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
    winplus::Ini::Section sec = settingsIni( sname.c_str() );

    if ( flag & Setting_EnabledAutoRun ) winplus::Mixed::ParseBool( sec.get( _T("EnabledAutoRun"), _T("false") ), &m_settings.isEnabledAutoRun );
    if ( flag & Setting_EnabledHotkey ) winplus::Mixed::ParseBool( sec.get( _T("EnabledHotkey"), _T("true") ), &m_settings.isEnabledHotkey );
    if ( flag & Setting_EnabledHttpSrv ) winplus::Mixed::ParseBool( sec.get( _T("EnabledHttpSrv"), _T("false") ), &m_settings.isEnabledHttpSrv );
    if ( flag & Setting_EnabledScheme ) winplus::Mixed::ParseBool( sec.get( _T("EnabledScheme"), _T("false") ), &m_settings.isEnabledScheme );
    if ( flag & Setting_DatabasePath ) m_settings.databasePath = sec.get( _T("DatabasePath"), winplus::Format( _T("$ROOT$\\%s.db"), winplus::LoadString(AFX_IDS_APP_TITLE).c_str() ) );
    if ( flag & Setting_BackupPath ) m_settings.backupPath = sec.get( _T("BackupPath"), _T("$ROOT$") );
    if ( flag & Setting_WordslibPath ) m_settings.wordslibPath = sec.get( _T("WordslibPath"), _T("$ROOT$\\words.wl") );

    if ( flag & Setting_AutoLogin ) winplus::Mixed::ParseBool( sec.get( _T("AutoLogin"), _T("false") ), &m_settings.isAutoLogin );
    if ( flag & Setting_SavePassword ) winplus::Mixed::ParseBool( sec.get( _T("SavePassword"), _T("false") ), &m_settings.isSavePassword );

    if ( flag & Setting_Username ) m_settings.username = sec.get( _T("Username"), _T("") );
    if ( flag & Setting_Password ) m_settings.password = sec.get( _T("Password"), _T("") );
}

void CassetteApp::SaveSettings( UINT flag )
{
    winplus::Ini settingsIni( winplus::ModulePath() + winplus::dirSep + winplus::LoadString(AFX_IDS_APP_TITLE) + TEXT(".ini") );
    winplus::String sname = winplus::LoadString(AFX_IDS_APP_TITLE);
    winplus::Ini::Section sec = settingsIni( sname.c_str() );

    if ( flag & Setting_EnabledAutoRun ) sec.set( _T("EnabledAutoRun"), (winplus::String)winplus::Mixed(m_settings.isEnabledAutoRun) );
    if ( flag & Setting_EnabledHotkey ) sec.set( _T("EnabledHotkey"), (winplus::String)winplus::Mixed(m_settings.isEnabledHotkey) );
    if ( flag & Setting_EnabledHttpSrv ) sec.set( _T("EnabledHttpSrv"), (winplus::String)winplus::Mixed(m_settings.isEnabledHttpSrv) );
    if ( flag & Setting_EnabledScheme ) sec.set( _T("EnabledScheme"), (winplus::String)winplus::Mixed(m_settings.isEnabledScheme) );
    if ( flag & Setting_DatabasePath ) sec.set( _T("DatabasePath"), m_settings.databasePath );
    if ( flag & Setting_BackupPath ) sec.set( _T("BackupPath"), m_settings.backupPath );
    if ( flag & Setting_WordslibPath ) sec.set( _T("WordslibPath"), m_settings.wordslibPath );

    if ( flag & Setting_AutoLogin ) sec.set( _T("AutoLogin"), (winplus::String)winplus::Mixed(m_settings.isAutoLogin) );
    if ( flag & Setting_SavePassword ) sec.set( _T("SavePassword"), (winplus::String)winplus::Mixed(m_settings.isSavePassword) );

    if ( flag & Setting_Username ) sec.set( _T("Username"), m_settings.username );
    if ( flag & Setting_Password ) sec.set( _T("Password"), m_settings.password );
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
    if ( flag & Setting_EnabledHotkey )
    {
        // 注册热键的功能移到了主窗口
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
    winplus::String keyNameAutoRun = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
    winplus::String appName = winplus::LoadString(AFX_IDS_APP_TITLE);

    Registry regAutoRun(keyNameAutoRun);
    if ( isEnabled )
    {
        if ( isForce || !regAutoRun.hasValue(appName) )
        {
            winplus::String appFullPath;
            appFullPath = winplus::ModulePath( NULL, &appFullPath ) + winplus::dirSep + appFullPath;
            if ( appFullPath.find(' ') != winplus::String::npos )
                appFullPath = TEXT("\"") + appFullPath + TEXT("\"");

            regAutoRun.setValue( appName, appFullPath );
        }
    }
    else
    {
        if ( isForce || regAutoRun.hasValue(appName) )
        {
            regAutoRun.delValue(appName);
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
        if ( !Registry::Exists(keyNameScheme) )
        {
            Registry regNameScheme( keyNameScheme, true );
            regNameScheme.setValue( "", "Account URL Protocol" );
            regNameScheme.setValue( "URL Protocol", "" );

            winplus::String appFullPath;
            appFullPath = winplus::ModulePath( NULL, &appFullPath ) + winplus::dirSep + appFullPath;
            Registry( regNameScheme.key(), "DefaultIcon", true ).setValue( "", appFullPath + ",0" );

            if ( appFullPath.find(' ') != winplus::String::npos )
                appFullPath = TEXT("\"") + appFullPath + TEXT("\"");

            Registry( regNameScheme.key(), "shell\\open\\command", true ).setValue( "", appFullPath + " -url \"%1\"" );
        }
    }
    else
    {
        if ( Registry::Exists(keyNameScheme) )
        {
            Registry::ForceDelete(keyNameScheme);
        }
    }
}

BOOL CassetteApp::DoSingletonRunning()
{
    //winplus::String sharedMemName = winplus::LoadString(IDS_SHAREDMEM_NAME);
    //if ( m_sharedMem.open(sharedMemName) )
    //{
    //    HWND hwndMain = m_sharedMem->hMainWnd;

    //    if ( winplus::Window_IsShow(hwndMain) )
    //        SetForegroundWindow(hwndMain);
    //    else
    //    {
    //        winplus::Window_Show(hwndMain);
    //        SetForegroundWindow(hwndMain);
    //    }
    //    return FALSE;
    //}
    //else
    //{
    //    m_sharedMem.create(sharedMemName);
    //}

    if ( !m_sharedMem.create(IDS_SHAREDMEM_NAME) ) return FALSE;

    HWND hwndMain = m_sharedMem->hMainWnd;
    if ( hwndMain != NULL )
    {
        if ( winplus::Window_IsShow(hwndMain) )
        {
            SetForegroundWindow(hwndMain);
        }
        else
        {
            winplus::Window_Show(hwndMain);
            SetForegroundWindow(hwndMain);
        }
        return FALSE;
    }

    return TRUE;
}
