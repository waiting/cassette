
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

// verify unique dialog
#define VERIFY_ONCE_DIALOG(dlg) static CDialog * dlg = NULL;\
if ( dlg != NULL )\
{\
    dlg->SetWindowPos( &wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );\
    dlg->ShowWindow(SW_NORMAL);\
    dlg->SetForegroundWindow();\
    dlg->SetFocus();\
    return;\
}

// Verify runable only other process Or parent window is MainFrame
#define VERIFY_RUNONLY_OTHER_HPROCESS( parent ) {\
DWORD dwProcessId;\
GetWindowThreadProcessId( *parent, &dwProcessId );\
if ( GetCurrentProcessId() == dwProcessId && parent != AfxGetMainWnd() )\
    return;\
}

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
    DISABLE_OBJECT_COPY(SetNullScopeOut)
};

// winplus::String和CString字符串互相转换
inline winplus::String CStringToString( CString const & str )
{
    return winplus::String( str.GetString(), str.GetLength() );
}

inline CString StringToCString( winplus::String const & str )
{
    return CString( str.c_str(), (int)str.length() );
}

// 解释文本中的$xxx$程序自定变量
winplus::String ExplainCustomVars( winplus::String const & str );

//////////////////////////////////////////////////////////////////////////
// 数据库相关操作
//////////////////////////////////////////////////////////////////////////

// Users -----------------------------------------------------------------
// 代表一个用户的相关数据
struct User
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

    //User & operator = ( User const & other )
    //{
    //    if ( this != &other )
    //    {
    //        m_id = other.m_id;
    //        m_username = other.m_username;
    //        m_password = other.m_password;
    //        m_protectLevel = other.m_protectLevel;
    //        m_condone = other.m_condone;
    //        m_curCondone = other.m_curCondone;
    //        m_unlockTime = other.m_unlockTime;
    //        m_hotkey = other.m_hotkey;
    //        m_regTime = other.m_regTime;
    //    }
    //    return *this;
    //}
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
struct AccountType
{
    CString m_typeName;
    int m_safeRank;

    AccountType()
    {
        m_safeRank = 0;
    }

    //AccountType & operator = ( AccountType const & other )
    //{
    //    if ( this != &other )
    //    {
    //        m_typeName = other.m_typeName;
    //        m_safeRank = other.m_safeRank;
    //    }
    //    return *this;
    //}

    AccountType & operator = ( winplus::Mixed const & accountTypeMixed )
    {
        size_t n = accountTypeMixed.getCount();
        for ( size_t i = 0; i < n; ++i )
        {
            auto && pr = accountTypeMixed.getPair(i);
            winplus::String const & keyname = pr.first.refAnsi();
            if ( keyname == "name" )
            {
                m_typeName = pr.second.refAnsi().c_str();
            }
            else if ( keyname == "rank" )
            {
                m_safeRank = pr.second;
            }
        }
        return *this;
    }

    void assignTo( winplus::Mixed * accountTypeMixed, String const & fieldNames = "name,rank" )
    {
        winplus::StringArray fnames;
        winplus::StrSplit( fieldNames, ",", &fnames );
        if ( !accountTypeMixed->isCollection() ) accountTypeMixed->createCollection();
        for ( auto it = fnames.begin(); it != fnames.end(); ++it )
        {
            if ( *it == "name" )
            {
                (*accountTypeMixed)[*it] = (LPCTSTR)m_typeName;
            }
            else if ( *it == "rank" )
            {
                (*accountTypeMixed)[*it] = m_safeRank;
            }
        }
    }
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

    AccountCate()
    {
        m_id = 0;
        m_timeWriten = 0;
    }

    //AccountCate & operator = ( AccountCate const & other )
    //{
    //    if ( this != &other )
    //    {
    //        m_id = other.m_id;
    //        m_cateName = other.m_cateName;
    //        m_cateDesc = other.m_cateDesc;
    //        m_typeName = other.m_typeName;
    //        m_url = other.m_url;
    //        m_icoPath = other.m_icoPath;
    //        m_startup = other.m_startup;
    //        m_keywords = other.m_keywords;
    //        m_timeWriten = other.m_timeWriten;
    //    }
    //    return *this;
    //}

    AccountCate & operator = ( winplus::Mixed const & accountCateMixed )
    {
        size_t n = accountCateMixed.getCount();
        for ( size_t i = 0; i < n; ++i )
        {
            auto && pr = accountCateMixed.getPair(i);
            winplus::String const & keyname = pr.first.refAnsi();
            if ( keyname == "id" )
            {
                m_id = pr.second;
            }
            else if ( keyname == "name" )
            {
                m_cateName = pr.second.refAnsi().c_str();
            }
            else if ( keyname == "desc" )
            {
                m_cateDesc = pr.second.refAnsi().c_str();
            }
            else if ( keyname == "type" )
            {
                m_typeName = pr.second.refAnsi().c_str();
            }
            else if ( keyname == "url" )
            {
                m_url = pr.second.refAnsi().c_str();
            }
            else if ( keyname == "icon" )
            {
                m_icoPath = pr.second.refAnsi().c_str();
            }
            else if ( keyname == "startup" )
            {
                m_startup = pr.second.refAnsi().c_str();
            }
            else if ( keyname == "keywords" )
            {
                m_keywords = pr.second.refAnsi().c_str();
            }
            else if ( keyname == "time" )
            {
                m_timeWriten = pr.second;
            }
        }
        return *this;
    }

    void assignTo( winplus::Mixed * accountCateMixed, String const & fieldNames = "id,name,desc,type,url,icon,startup,keywords,time" )
    {
        winplus::StringArray fnames;
        winplus::StrSplit( fieldNames, ",", &fnames );
        if ( !accountCateMixed->isCollection() ) accountCateMixed->createCollection();
        for ( auto it = fnames.begin(); it != fnames.end(); ++it )
        {
            if ( *it == "id" )
            {
                (*accountCateMixed)[*it] = m_id;
            }
            else if ( *it == "name" )
            {
                (*accountCateMixed)[*it] = (LPCTSTR)m_cateName;
            }
            else if ( *it == "desc" )
            {
                (*accountCateMixed)[*it] = (LPCTSTR)m_cateDesc;
            }
            else if ( *it == "type" )
            {
                (*accountCateMixed)[*it] = (LPCTSTR)m_typeName;
            }
            else if ( *it == "url" )
            {
                (*accountCateMixed)[*it] = (LPCTSTR)m_url;
            }
            else if ( *it == "icon" )
            {
                (*accountCateMixed)[*it] = (LPCTSTR)m_icoPath;
            }
            else if ( *it == "startup" )
            {
                (*accountCateMixed)[*it] = (LPCTSTR)m_startup;
            }
            else if ( *it == "keywords" )
            {
                (*accountCateMixed)[*it] = (LPCTSTR)m_keywords;
            }
            else if ( *it == "time" )
            {
                (*accountCateMixed)[*it] = m_timeWriten;
            }
        }
    }
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
struct Account
{
    CString m_myName;
    CString m_accountName;
    CString m_accountPwd;
    int m_cateId;
    int m_userId;
    int m_safeRank;
    CString m_comment;
    int m_time;

    Account()
    {
        m_cateId = 0;
        m_userId = 0;
        m_safeRank = 0;
        m_time = 0;
    }

    //Account & operator = ( Account const & other )
    //{
    //    if ( this != &other )
    //    {
    //        m_myName = other.m_myName;
    //        m_accountName = other.m_accountName;
    //        m_accountPwd = other.m_accountPwd;
    //        m_cateId = other.m_cateId;
    //        m_userId = other.m_userId;
    //        m_safeRank = other.m_safeRank;
    //        m_comment = other.m_comment;
    //        m_time = other.m_time;
    //    }
    //    return *this;
    //}

    Account & operator = ( winplus::Mixed const & accountMixed )
    {
        size_t n = accountMixed.getCount();
        for ( size_t i = 0; i < n; ++i )
        {
            auto && pr = accountMixed.getPair(i);
            winplus::String const & keyname = pr.first.refAnsi();
            if ( keyname == "myname" )
            {
                m_myName = pr.second.refAnsi().c_str();
            }
            else if ( keyname == "account_name" )
            {
                m_accountName = winplus::Utf8ToString( winplus::DecryptContent( pr.second.toAnsi() ) ).c_str();
            }
            else if ( keyname == "account_pwd" )
            {
                m_accountPwd = winplus::Utf8ToString( winplus::DecryptContent( pr.second.toAnsi() ) ).c_str();
            }
            else if ( keyname == "cate" )
            {
                m_cateId = pr.second;
            }
            else if ( keyname == "user" )
            {
                m_userId = pr.second;
            }
            else if ( keyname == "safe_rank" )
            {
                m_safeRank = pr.second;
            }
            else if ( keyname == "comment" )
            {
                m_comment = pr.second.refAnsi().c_str();
            }
            else if ( keyname == "time" )
            {
                m_time = pr.second;
            }
        }
        return *this;
    }

    void assignTo( winplus::Mixed * accountMixed, String const & fieldNames = "myname,account_name,account_pwd,cate,user,safe_rank,comment,time" )
    {
        winplus::StringArray fnames;
        winplus::StrSplit( fieldNames, ",", &fnames );
        if ( !accountMixed->isCollection() ) accountMixed->createCollection();
        for ( auto it = fnames.begin(); it != fnames.end(); ++it )
        {
            if ( *it == "myname" )
            {
                (*accountMixed)[*it] = (LPCTSTR)m_myName;
            }
            else if ( *it == "account_name" )
            {
                (*accountMixed)[*it] = winplus::EncryptContent( winplus::Buffer( winplus::StringToUtf8( (LPCTSTR)m_accountName ) ) );
            }
            else if ( *it == "account_pwd" )
            {
                (*accountMixed)[*it] = winplus::EncryptContent( winplus::Buffer( winplus::StringToUtf8( (LPCTSTR)m_accountPwd ) ) );
            }
            else if ( *it == "cate" )
            {
                (*accountMixed)[*it] = m_cateId;
            }
            else if ( *it == "user" )
            {
                (*accountMixed)[*it] = m_userId;
            }
            else if ( *it == "safe_rank" )
            {
                (*accountMixed)[*it] = m_safeRank;
            }
            else if ( *it == "comment" )
            {
                (*accountMixed)[*it] = (LPCTSTR)m_comment;
            }
            else if ( *it == "time" )
            {
                (*accountMixed)[*it] = m_time;
            }
        }
    }
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

// 浏览器识别记录 --------------------------------------------------------------------------------
struct Browser
{
    int m_id;
    CString m_browserName;
    CString m_browserTitle;
    CString m_exeName;

    Browser()
    {
        m_id = 0;
    }

    Browser & operator = ( Mixed const & browserMixed )
    {
        size_t n = browserMixed.getCount();
        for ( size_t i = 0; i < n; ++i )
        {
            auto && pr = browserMixed.getPair(i);
            winplus::String const & keyname = pr.first.refAnsi();
            if ( keyname == "id" )
            {
                m_id = pr.second;
            }
            else if ( keyname == "name" )
            {
                m_browserName = pr.second.refAnsi().c_str();
            }
            else if ( keyname == "title" )
            {
                m_browserTitle = pr.second.refAnsi().c_str();
            }
            else if ( keyname == "exe_name" )
            {
                m_exeName = pr.second.refAnsi().c_str();
            }
        }
        return *this;
    }

    void assignTo( winplus::Mixed * browserMixed, String const & fieldNames = "id,name,title,exe_name" )
    {
        winplus::StringArray fnames;
        winplus::StrSplit( fieldNames, ",", &fnames );
        if ( !browserMixed->isCollection() ) browserMixed->createCollection();
        for ( auto && fname : fnames )
        {
            if ( fname == "id" )
            {
                (*browserMixed)[fname] = m_id;
            }
            else if ( fname == "name" )
            {
                (*browserMixed)[fname] = (LPCTSTR)m_browserName;
            }
            else if ( fname == "title" )
            {
                (*browserMixed)[fname] = (LPCTSTR)m_browserTitle;
            }
            else if ( fname == "exe_name" )
            {
                (*browserMixed)[fname] = (LPCTSTR)m_exeName;
            }
        }
    }
};

typedef CArray< Browser, Browser const & > BrowserArray;

// 判断ExeName是否为一个浏览器
bool IsBrowserExeName( eiendb::Database & db, CString const & exeName, CString * browserTitle );

// 加载可识别的浏览器记录
int LoadBrowsers( eiendb::Database & db, BrowserArray * browsers );


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
#define WarningError( str, title ) MessageBox( str, title, MB_OK | MB_ICONEXCLAMATION );
// 致命型错误
#define FatalError( str, title ) MessageBox( str, title, MB_OK | MB_ICONERROR );


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
