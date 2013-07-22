
#if !defined(__FUNCTIONAL_H__)
#define __FUNCTIONAL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
// 宏支持
//////////////////////////////////////////////////////////////////////////
// calc array's count
#ifndef countof
#define countof(arr) ( sizeof(arr) / sizeof(arr[0]) )
#endif

// 缓冲区转换为ansi_string二进制串
#define BufferToAnsiString( buf, size ) ansi_string( (char const *)(buf), (size_t)(size) )

// if pointer
#define IfPTR(ptr) if( (ptr) != NULL ) (ptr)
#define AssignPTR(ptr) if( (ptr) != NULL ) *(ptr)

// bind blob data
#define BlobBind( i, v ) Bind( i, (unsigned char const *)v.c_str(), v.size() )

// verify unique dialog
#define VERIFY_ONCE_DIALOG(dlg) static CDialog * dlg = NULL;\
if ( dlg != NULL )\
{\
	dlg->SetFocus();\
	return;\
}

// get `bool` value
#define Bool(v) ((v) != FALSE)

//////////////////////////////////////////////////////////////////////////
// 模板支持
//////////////////////////////////////////////////////////////////////////
// Set pointer as null when the scope out.
class SetNullScopeOut
{
	void * & _p;
public:
	template < typename _Ty >
	SetNullScopeOut( _Ty * & p ) : _p((void*&)p) {}
	~SetNullScopeOut() { if ( _p ) _p = NULL; }
private:
	SetNullScopeOut( SetNullScopeOut const & );
	SetNullScopeOut & operator = ( SetNullScopeOut const & );
};

//////////////////////////////////////////////////////////////////////////
// base64
//////////////////////////////////////////////////////////////////////////
string Base64Encode( ansi_string const & data );
ansi_string Base64Decode( string const & base64Str );
//////////////////////////////////////////////////////////////////////////
// encrypt content functional
//////////////////////////////////////////////////////////////////////////
// 内容加密错误代码
enum EncryptErrorCode
{
	ERR_SUCCEED,
	ERR_DATASIZE,
	ERR_BUFFSIZE,
};

ansi_string EncryptContent( ansi_string const & content );
ansi_string DecryptContent( ansi_string const & encryptContent );
#define EncryptCArray(data) EncryptContent( BufferToAnsiString( data, sizeof(data) ) )
#define EncryptBuffer( buf, size ) EncryptContent( BufferToAnsiString( buf, size ) )
#define DecryptCArray(data) DecryptContent( BufferToAnsiString( data, sizeof(data) ) )
#define DecryptBuffer( buf, size ) DecryptContent( BufferToAnsiString( buf, size ) )

// 解释文本中的$xxx$程序自定变量
string ExplainCustomVars( string const & str );

// 获取可执行文件所在路径, 末尾含目录分割符
CString GetExecutablePath();

//////////////////////////////////////////////////////////////////////////
// 数据库相关操作
//////////////////////////////////////////////////////////////////////////

// 注册用户, 注册成功返回true, 否则false.
bool RegisterUser( CString const & username, CString const & password, int protectLevel, UINT32 hotkey );
// 代表一个用户
class UserInfo
{
public:
	int m_id;
	CString m_username;
	CString m_password;
	int m_protectLevel;
	int m_condone;
	int m_curCondone;
	int m_unlockTime;
	int m_hotkey;
	int m_regTime;

	UserInfo()
	{
		m_id = 0;
		m_protectLevel = 0;
		m_condone = 0;
		m_curCondone = 0;
		m_unlockTime = 0;
		m_hotkey = 0;
		m_regTime = 0;
	}
};

// 登录用户, 执行验证成功返回true,并获取User数据, 否则返回false.
bool LoginUser( CString const & username, CString const & password, UserInfo * userInfo );
// 载入用户信息
bool LoadUser( CString const & username, UserInfo * userInfo );
// 删除用户
bool DeleteUser( CString const & username );
// 验证用户密码
bool VerifyUserPassword( CString const & username, CString const & password );
// 修改用户信息
bool ModifyUser(
	CString const & username,
	bool isModifyPassword,
	CString const & newUsername,
	CString const & newPassword,
	int newProtectLevel,
	int newCondone,
	int newCurCondone,
	int newHotkey
);

// 加载账户类别信息, 返回记录数
int LoadAccountTypes( CStringArray * typeNames, CUIntArray * safeRanks );

// 获取一个类型
bool GetAccountType( CString const & typeName, int * safeRank );

// 添加账户类别信息, 成功返回true, 否则返回false.
bool AddAccountType( CString const & typeName, int safeRank );
// 修改账户类别信息, 成功返回true, 否则返回false.
bool ModifyAccountType( CString const & typeName, CString const & newTypeName, int newSafeRank );
// 删除账户类别信息, 成功返回true, 否则返回false.
bool DeleteAccountType( CString const & typeName );

// 载入账户种类信息, 返回记录数
int LoadAccountCates(
	CUIntArray * ids,
	CStringArray * cateNames,
	CStringArray * cateDescs,
	CStringArray * typeNames,
	CStringArray * urls,
	CStringArray * icoPaths,
	CStringArray * startups,
	CStringArray * keywordss,
	CUIntArray * timeWritens
);

// 载入一条账户种类信息
bool GetAccountCate(
	int id,
	CString * cateName,
	CString * cateDesc,
	CString * typeName,
	CString * url,
	CString * icoPath,
	CString * startup,
	CString * keywords,
	int * timeWriten
);

// 添加账户种类信息，成功返回ID，失败返回0
int AddAccountCate(
	CString const & cateName,
	CString const & cateDesc,
	CString const & typeName,
	CString const & url,
	CString const & icoPath,
	CString const & startup,
	CString const & keywords
);
// 修改账户种类信息
bool ModifyAccountCate(
	int id,
	CString const & newCateName,
	CString const & newCateDesc,
	CString const & newTypeName,
	CString const & newUrl,
	CString const & newIcoPath,
	CString const & newStartup,
	CString const & newKeywords
);
// 删除账户种类信息
bool DeleteAccountCate( int id );

// 载入所有账户种类的ID以及相应的安全值
int LoadAccountCatesSafeRank( CUIntArray * cateIds, CUIntArray * typeSafeRanks );

// 获取指定cateId的类别
bool GetTypeByCateId( int cateId, CString * typeName, int * safeRank );

// 载入当前登录用户所有的账户信息
int LoadAccounts(
	CStringArray * myNames,
	CStringArray * accountNames,
	CStringArray * accountPwds,
	CUIntArray * cateIds,
	CUIntArray * safeRanks,
	CStringArray * comments,
	CUIntArray * times
);

// 获取当前登录用户的一个账户信息
bool GetAccount(
	CString const & myName,
	CString * accountName,
	CString * accountPwd,
	int * cateId,
	int * safeRank,
	CString * comment,
	int * time
);

// 添加一个当前登录用户的账户，成功true，失败false。
// 主键(user,myname)
// 意味着每个用户(user)拥有账户的myname必须不同，否则失败。修改时也一样。
bool AddAccount(
	CString const & myName,
	CString const & accountName,
	CString const & accountPwd,
	int cateId,
	int safeRank,
	CString const & comment
);
// 修改一个当前登录用户的账户
// 主键(user,myname)
bool ModifyAccount(
	CString const & myName,
	CString const & newMyName,
	CString const & newAccountName,
	CString const & newAccountPwd,
	int newCateId,
	int newSafeRank,
	CString const & newComment
);
// 删除一个当前登录用户的账户
bool DeleteAccount( CString const & myName );

// 备份数据
bool BackupData( CString const & filename );
// 恢复数据
bool ResumeData( CString const & filename );
// 取得一个正确的Account MyName以便添加账户
CString GetCorrectAccountMyName( CString const & myName );

//////////////////////////////////////////////////////////////////////////
// 接口

// 更新ListView数据和列表项
interface IUpdateListView
{
	enum
	{
		UPDATE_LOAD_DATA = 1,
		UPDATE_LIST_ITEMS = 2,
	};
	// 更新数据和列表项
	// itemIndex为-1时,加载数据库内全部的数据并更新整个列表,第一次调用请用它
	// itemIndex为列表项索引时,读取索引指示的数组记录,通过主键查询数该条数据内容
	// 依据flag的指示进行操作:
	// UPDATE_LOAD_DATA 从数据库加载数据到数组中储存
	// UPDATE_LIST_ITEMS 把数组中的数据更新到列表项
	virtual void UpdateList( int flag = UPDATE_LOAD_DATA | UPDATE_LIST_ITEMS, long itemIndex = -1 ) PURE;

};

//////////////////////////////////////////////////////////////////////////

// 错误提示框

// 警告型错误
#define WarningError( str, title )\
	MessageBox( str, title, MB_OK | MB_ICONEXCLAMATION );
// 致命型错误
#define FatalError( str, title )\
	MessageBox( str, title, MB_OK | MB_ICONERROR );


//////////////////////////////////////////////////////////////////////////

/* 把一个数组载入组合框控件 */
inline int ComboBoxLoadDataFromArray( CComboBox * pCbo, CStringArray const & arr )
{
	int i;
	int n = arr.GetSize();
	for ( i = 0; i < n; ++i )
	{
		pCbo->AddString(arr[i]);
	}
	return n;
}

inline int ComboBoxLoadDataFromArray( CComboBox * pCbo, CString * arr, int n )
{
	int i;
	for ( i = 0; i < n; ++i )
	{
		pCbo->AddString(arr[i]);
	}
	return n;
}

#define ComboBoxLoadDataFromCArray(cbo,arr) ComboBoxLoadDataFromArray(cbo, arr, countof(arr))


#endif // !defined(__FUNCTIONAL_H__)
