﻿
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

// bind blob data
#define BlobBind( i, v ) Bind( i, (unsigned char const *)v.c_str(), v.size() )

// verify unique dialog
#define VERIFY_ONCE_DIALOG(dlg) static CDialog * dlg = NULL;\
if ( dlg != NULL )\
{\
    dlg->SetWindowPos( &wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );\
    dlg->SetForegroundWindow();\
    dlg->SetFocus();\
    return;\
}
#define VERIFY_RUNONLY_OTHER_HPROCESS( parent ) {\
DWORD dwProcessId;\
GetWindowThreadProcessId( *parent, &dwProcessId );\
if ( GetCurrentProcessId() == dwProcessId && parent != AfxGetMainWnd() )\
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

// 解释文本中的$xxx$程序自定变量
winplus::String ExplainCustomVars( winplus::String const & str );

// 获取可执行文件所在路径, 末尾含目录分割符
CString GetExecutablePath();

//////////////////////////////////////////////////////////////////////////
// 数据库相关操作
//////////////////////////////////////////////////////////////////////////

/*
struct Fields
{
    // 绑定int参数,索引起始为1
    static int _bindInt( sqlite3_stmt * stmt, int sqlParamIndex, int val )
    {
        return sqlite3_bind_int( stmt, sqlParamIndex, val );
    }
    // 绑定string参数,索引起始为1
    static int _bindString( sqlite3_stmt * stmt, int sqlParamIndex, CString const & val )
    {
        // 转为utf8再绑定
        return sqlite3_bind_text( stmt, sqlParamIndex, winplus::StringToUtf8( (LPCTSTR)val ).c_str(), -1, SQLITE_TRANSIENT );
    }
    // 绑定blob参数,索引起始为1
    static int _bindBlob( sqlite3_stmt * stmt, int sqlParamIndex, LPCVOID data, int size )
    {
        return sqlite3_bind_blob( stmt, sqlParamIndex, data, size, SQLITE_TRANSIENT );
    }

    // 获取字段int值,字段索引起始为0
    static int _getInt( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        return sqlite3_column_int( stmt, sqlFieldIndex );
    }
    // 获取字段string值,字段索引起始为0
    static CString _getString( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        char const * sUTF8 = (char const *)sqlite3_column_text( stmt, sqlFieldIndex );
        return winplus::Utf8ToString( sUTF8 ? sUTF8 : "" ).c_str();
    }
    // 获取字段blob值,字段索引起始为0
    static void _getBlob( sqlite3_stmt * stmt, int sqlFieldIndex, winplus::AnsiString * blob )
    {
        int size;
        size = sqlite3_column_bytes( stmt, sqlFieldIndex );
        blob->assign( (char const *)sqlite3_column_blob( stmt, sqlFieldIndex ), size );
    }
};*/

// Users -----------------------------------------------------------------
// 代表一个用户的相关数据

/*enum FieldBits__am_users
{
    am_users__id          = winplus::Bin0<0000000001>::val,
    am_users__name        = winplus::Bin0<0000000010>::val,
    am_users__pwd         = winplus::Bin0<0000000100>::val,
    am_users__protect     = winplus::Bin0<0000001000>::val,
    am_users__condone     = winplus::Bin0<0000010000>::val,
    am_users__cur_condone = winplus::Bin0<0000100000>::val,
    am_users__unlock_time = winplus::Bin0<0001000000>::val,
    am_users__hotkey      = winplus::Bin0<0010000000>::val,
    am_users__time        = winplus::Bin0<0100000000>::val,
};*/

// 当查询全部字段时才可用这个函数确定字段索引
inline int FieldIndex( UINT fieldBitFlag )
{
    return (int)( log((double)fieldBitFlag) / log(2.0) + 0.0000005 );
}

struct User/* : Fields*/
{
    int m_id;
    CString m_username;
    CString m_password;
    int m_protectLevel;
    int m_condone;
    int m_curCondone;
    int m_unlockTime;
    int m_hotkey;
    int m_regTime;

    User & operator = ( User const & other )
    {
        if ( this != &other )
        {
            m_id = other.m_id;
            m_username = other.m_username;
            m_password = other.m_password;
            m_protectLevel = other.m_protectLevel;
            m_condone = other.m_condone;
            m_curCondone = other.m_curCondone;
            m_unlockTime = other.m_unlockTime;
            m_hotkey = other.m_hotkey;
            m_regTime = other.m_regTime;
        }
        return *this;
    }

    User()
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

// 注册用户, 注册成功返回true, 否则false
// user忽略m_id,m_condone,m_curCondone,m_unlockTime,m_regTime
bool RegisterUser( eiendb::Database & db, User const & newUser );

// 登录用户, 执行验证成功返回true,并获取User数据, 否则返回false.
bool LoginUser( eiendb::Database & db, CString const & username, CString const & password, User * userData );
// 载入用户信息
bool LoadUser( eiendb::Database & db, CString const & username, User * userData );
// 删除用户
bool DeleteUser( eiendb::Database & db, CString const & username );
// 验证用户密码
bool VerifyUserPassword( eiendb::Database & db, CString const & username, CString const & password );
// 修改用户信息
bool ModifyUserEx( eiendb::Database & db, CString const & username, winplus::Mixed const & userFields );

// Types -----------------------------------------------------------------

//一条账户类型相关数据
struct AccountType/* : public Fields*/
{
    CString m_typeName;
    int m_safeRank;

    AccountType & operator = ( AccountType const & other )
    {
        if ( this != &other )
        {
            m_typeName = other.m_typeName;
            m_safeRank = other.m_safeRank;
        }
        return *this;
    }

    AccountType()
    {
        m_safeRank = 0;
    }

    void assignTo( winplus::Mixed * accountTypeMixed, CString const & fieldNames = "name,rank" );
    void assign( winplus::Mixed const & accountTypeMixed );
};

typedef CArray<AccountType, AccountType const &> AccountTypeArray;

// 加载账户类别信息,返回记录数
int LoadAccountTypes( eiendb::Database & db, AccountTypeArray * types );

// 获取一个类型
bool GetAccountType( eiendb::Database & db, CString const & typeName, AccountType * type );

// 添加账户类别信息, 成功返回true, 否则返回false.
bool AddAccountType( eiendb::Database & db, winplus::Mixed const & newType );

// 修改账户类别信息, 成功返回true, 否则返回false.
// name,rank
bool ModifyAccountType( eiendb::Database & db, CString const & typeName, winplus::Mixed const & newTypeFields );

// 删除账户类别信息, 成功返回true, 否则返回false.
bool DeleteAccountType( eiendb::Database & db, CString const & typeName );

// Cates -----------------------------------------------------------------
struct AccountCate
{
    int m_id;
    CString m_cateName;
    CString m_cateDesc;
    CString m_typeName;
    CString m_url;
    CString m_icoPath;
    CString m_startup;
    CString m_keywords;
    int m_timeWriten;

    AccountCate & operator = ( AccountCate const & other )
    {
        if ( this != &other )
        {
            m_id = other.m_id;
            m_cateName = other.m_cateName;
            m_cateDesc = other.m_cateDesc;
            m_typeName = other.m_typeName;
            m_url = other.m_url;
            m_icoPath = other.m_icoPath;
            m_startup = other.m_startup;
            m_keywords = other.m_keywords;
            m_timeWriten = other.m_timeWriten;
        }
        return *this;
    }

    AccountCate()
    {
        m_id = 0;
        m_timeWriten = 0;
    }

    void assignTo( winplus::Mixed * accountCateMixed, CString const & fieldNames = "id,name,desc,type,url,icon,startup,keywords,time" );
    void assign( winplus::Mixed const & accountCateMixed );
};

typedef CArray<AccountCate, AccountCate const &> AccountCateArray;

// 载入账户种类信息, 返回记录数
int LoadAccountCates( eiendb::Database & db, AccountCateArray * cates );

// 载入一条账户种类信息
bool GetAccountCate( eiendb::Database & db, int id, AccountCate * cate );

// 添加账户种类信息，成功返回ID，失败返回0
// newCate忽略m_id,m_timeWriten
int AddAccountCate( eiendb::Database & db, winplus::Mixed const & newCate );

// 修改账户种类信息
// id,name,desc,type,url,icon,startup,keywords,time
bool ModifyAccountCate( eiendb::Database & db, int id, winplus::Mixed const & newCateFields );

// 删除账户种类信息
bool DeleteAccountCate( eiendb::Database & db, int id );

// 载入所有账户种类的ID以及相应的安全值
int LoadAccountCatesSafeRank( eiendb::Database & db, CUIntArray * cateIds, CUIntArray * typeSafeRanks );

// 获取指定cateId的类别
bool GetTypeByCateId( eiendb::Database & db, int cateId, AccountType * type );

// Accounts --------------------------------------------------------------
struct Account/* : public Fields*/
{
    CString m_myName;
    CString m_accountName;
    CString m_accountPwd;
    int m_cateId;
    int m_userId;
    int m_safeRank;
    CString m_comment;
    int m_time;

    Account & operator = ( Account const & other )
    {
        if ( this != &other )
        {
            m_myName = other.m_myName;
            m_accountName = other.m_accountName;
            m_accountPwd = other.m_accountPwd;
            m_cateId = other.m_cateId;
            m_userId = other.m_userId;
            m_safeRank = other.m_safeRank;
            m_comment = other.m_comment;
            m_time = other.m_time;
        }
        return *this;
    }

    Account()
    {
        m_cateId = 0;
        m_userId = 0;
        m_safeRank = 0;
        m_time = 0;
    }

    void assignTo( winplus::Mixed * accountMixed, CString const & fieldNames = "myname,account_name,account_pwd,cate,user,safe_rank,comment,time" );
    void assign( winplus::Mixed const & accountMixed );
};

typedef CArray<Account, Account const &> AccountArray;

// 载入指定用户所有的账户信息,可指定种类:-1为全部种类
int LoadAccounts( eiendb::Database & db, int userId, AccountArray * accounts, int cateId = -1 );

// 获取指定用户的一个账户信息
// 主键(user,myname)
bool GetAccount( eiendb::Database & db, int userId, CString const & myName, Account * account );

// 添加一个指定用户的账户，成功true，失败false。
// 主键(user,myname)
// 意味着每个用户(user)拥有账户的myname必须不同，否则失败。修改时也一样。
// newAccount忽略m_time
bool AddAccount( eiendb::Database & db, winplus::Mixed const & newAccount );

// 修改一个指定用户的账户
// 主键(user,myname)
// myname,account_name,account_pwd,cate,user,safe_rank,comment,time
bool ModifyAccount( eiendb::Database & db, int userId, CString const & myName, winplus::Mixed const & newAccountFields );

// 删除一个指定用户的账户
bool DeleteAccount( eiendb::Database & db, int userId, CString const & myName );

// Others ----------------------------------------------------------------

// 取得一个正确的Account MyName以便添加账户
CString GetCorrectAccountMyName( eiendb::Database & db, int userId, CString const & myName );

// 获取数据库全部表名
int LoadTableNames( eiendb::Database & db, winplus::StringArray * tableNames, winplus::String const & like = _T("am\\_%") );

// 获取数据库的DDL,返回SQL语句条数
int DumpDDL( eiendb::Database & db, winplus::String * ddl, winplus::String const & like = _T("am\\_%") );

// 判断ExeName是否为一个浏览器
bool IsBrowserExeName( eiendb::Database & db, CString const & exeName, CString * browserTitle );


//////////////////////////////////////////////////////////////////////////
// 接口
//////////////////////////////////////////////////////////////////////////

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
