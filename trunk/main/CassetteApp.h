//////////////////////////////////////////////////////////////////////////
// Cassette application
//////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CASSETTE_H__A6A4CE47_157C_4047_B55A_BA78A709DEB0__INCLUDED_)
#define AFX_CASSETTE_H__A6A4CE47_157C_4047_B55A_BA78A709DEB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Functional.h"

/////////////////////////////////////////////////////////////////////////////
// CassetteApp:

class CassetteApp : public CWinApp
{
public:
	CassetteApp();

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CassetteApp)
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL
	enum SettingsParametersFlag
	{
		Setting_EnabledAutoRun = 0x0001,
		Setting_EnabledHotkey = 0x0002,
		Setting_EnabledHttpSrv = 0x0004,
		Setting_EnabledScheme = 0x0008,
		Setting_DatabasePath = 0x0010,
		Setting_BackupPath = 0x0020,
		Setting_WordslibPath = 0x0040,

		Setting_AutoLogin = 0x0080,
		Setting_SavePassword = 0x0100,

		Setting_Username = 0x0200,
		Setting_Password = 0x0400,
	};

	struct SettingsParameters
	{
		bool isEnabledAutoRun;   // 是否自启动运行
		bool isEnabledHotkey;    // 是否开启用户热键
		bool isEnabledHttpSrv;   // 是否开启HTTP服务
		bool isEnabledScheme;    // 是否注册scheme
		string databasePath;     // 数据库路径
		string backupPath;       // 备份路径
		string wordslibPath;     // 词库路径

		bool isAutoLogin;        // 是否自动登录
		bool isSavePassword;     // 是否保存密码

		string username;         // 保存的用户名(ec&base64加密)
		string password;         // 保存的密码(ec&base64加密)
	};

	struct CassetteSharedData
	{
		HWND hMainWnd; // 主窗口句柄
	};
protected:
	shared_memory<CassetteSharedData> m_sharedMem;//共享内存
	sqlite3 * m_db;
	wordslib * m_wordslib;//词库

	// 开启/关闭开机自动启动
	void EnableAutoRun( bool isEnabled, bool isForce = false );
	// 开启/关闭http服务
	void EnableHttpService( bool isEnabled );
	// 开启/关闭Scheme
	void EnableScheme( bool isEnabled );

	// 使程序只能单例运行
	// 如果程序第一次运行,创建共享内存,返回TRUE
	// 否则将从共享内存中获取句柄激活已有程序,返回FALSE
	BOOL DoSingletonRunning();
public:
	// 代表已登录用户
	User m_loginedUser;
	BOOL m_viaAutoLogin; // 是否通过自动登录方式登录
	SettingsParameters m_settings; // 程序设置的参数

	// 获取共享内存引用
	shared_memory<CassetteSharedData> & GetSharedMemory() { return m_sharedMem; }
	// 初始化数据表和数据
	void InitDatabaseSchema();
	// 打开数据库资源
	void OpenDatabase();
	// 关闭数据库资源
	void CloseDatabase();
	// open wordslib
	void OpenWordslib();
	void CloseWordslib();
	// close wordslib
	// 获取数据库
	sqlite3 * GetDatabase() const { return m_db; }
	// get wordslib
	wordslib * GetWordslib() const { return m_wordslib; }
	// 备份数据
	bool BackupData( CString const & filename );
	// 恢复数据
	bool ResumeData( CString const & filename );
	// 处理选项设置
	// 从ini读取设置到变量中,flag指示是什么设置
	void LoadSettings( UINT flag = (UINT)-1 );
	// 从变量写入设置到ini中,flag指示是什么设置
	void SaveSettings( UINT flag = (UINT)-1 );
	// 使设置生效，一些选项需要特别的功能调用才能生效,flag指示是什么设置
	void DoSettings( UINT flag = (UINT)-1 );

	//{{AFX_MSG(CassetteApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CassetteApp g_theApp;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CASSETTE_H__A6A4CE47_157C_4047_B55A_BA78A709DEB0__INCLUDED_)
