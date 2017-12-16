
#if !defined(__FUNCTIONAL_H__)
#define __FUNCTIONAL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
// ��֧��
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
// ģ��֧��
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
winplus::String Base64Encode( winplus::AnsiString const & data );
winplus::AnsiString Base64Decode( winplus::String const & base64Str );
//////////////////////////////////////////////////////////////////////////
// encrypt content functional
//////////////////////////////////////////////////////////////////////////
// ���ݼ��ܴ������
enum EncryptErrorCode
{
    ERR_SUCCEED,
    ERR_DATASIZE,
    ERR_BUFFSIZE,
};

winplus::AnsiString EncryptContent( winplus::AnsiString const & content );
winplus::AnsiString DecryptContent( winplus::AnsiString const & encryptContent );
#define EncryptCArray(data) EncryptContent( BufferToAnsiString( data, sizeof(data) ) )
#define EncryptBuffer( buf, size ) EncryptContent( BufferToAnsiString( buf, size ) )
#define DecryptCArray(data) DecryptContent( BufferToAnsiString( data, sizeof(data) ) )
#define DecryptBuffer( buf, size ) DecryptContent( BufferToAnsiString( buf, size ) )

// �����ı��е�$xxx$�����Զ�����
winplus::String ExplainCustomVars( winplus::String const & str );

// ��ȡ��ִ���ļ�����·��, ĩβ��Ŀ¼�ָ��
CString GetExecutablePath();

//////////////////////////////////////////////////////////////////////////
// ���ݿ���ز���
//////////////////////////////////////////////////////////////////////////

struct Fields
{
    // ��int����,������ʼΪ1
    static int _bindInt( sqlite3_stmt * stmt, int sqlParamIndex, int val )
    {
        return sqlite3_bind_int( stmt, sqlParamIndex, val );
    }
    // ��string����,������ʼΪ1
    static int _bindString( sqlite3_stmt * stmt, int sqlParamIndex, CString const & val )
    {
        // תΪutf8�ٰ�
        return sqlite3_bind_text( stmt, sqlParamIndex, winplus::StringToUtf8( (LPCTSTR)val ).c_str(), -1, SQLITE_TRANSIENT );
    }
    // ��blob����,������ʼΪ1
    static int _bindBlob( sqlite3_stmt * stmt, int sqlParamIndex, LPCVOID data, int size )
    {
        return sqlite3_bind_blob( stmt, sqlParamIndex, data, size, SQLITE_TRANSIENT );
    }

    // ��ȡ�ֶ�intֵ,�ֶ�������ʼΪ0
    static int _getInt( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        return sqlite3_column_int( stmt, sqlFieldIndex );
    }
    // ��ȡ�ֶ�stringֵ,�ֶ�������ʼΪ0
    static CString _getString( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        char const * sUTF8 = (char const *)sqlite3_column_text( stmt, sqlFieldIndex );
        return winplus::Utf8ToString( sUTF8 ? sUTF8 : "" ).c_str();
    }
    // ��ȡ�ֶ�blobֵ,�ֶ�������ʼΪ0
    static void _getBlob( sqlite3_stmt * stmt, int sqlFieldIndex, winplus::AnsiString * blob )
    {
        int size;
        size = sqlite3_column_bytes( stmt, sqlFieldIndex );
        blob->assign( (char const *)sqlite3_column_blob( stmt, sqlFieldIndex ), size );
    }
};

// Users -----------------------------------------------------------------
// ����һ���û����������

enum FieldBits__am_users
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
};

// ����ѯȫ���ֶ�ʱ�ſ����������ȷ���ֶ�����
inline int FieldIndex( UINT fieldBitFlag )
{
    return (int)( log((double)fieldBitFlag) / log(2.0) + 0.0000005 );
}

struct User : public Fields
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

    // �����ݵ�ָ����SQL����ռλ��������,����1

    int bindId( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindInt( stmt, sqlParamIndex, m_id );
    }
    int bindUsername( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindString( stmt, sqlParamIndex, m_username );
    }
    int bindPassword( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        winplus::AnsiString encrypt;
        encrypt = EncryptContent( winplus::StringToUtf8( (LPCTSTR)m_password ) );
        return _bindBlob( stmt, sqlParamIndex, encrypt.c_str(), encrypt.size() );
    }
    int bindProtectLevel( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindInt( stmt, sqlParamIndex, m_protectLevel );
    }
    int bindCondone( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindInt( stmt, sqlParamIndex, m_condone );
    }
    int bindCurCondone( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindInt( stmt, sqlParamIndex, m_curCondone );
    }
    int bindUnlockTime( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindInt( stmt, sqlParamIndex, m_unlockTime );
    }
    int bindHotkey( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindInt( stmt, sqlParamIndex, m_hotkey );
    }
    int bindRegTime( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindInt( stmt, sqlParamIndex, m_regTime );
    }

    // ��STMT��ȡ���ݵ�����,�ֶ�����ֵ����0

    void loadId( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_id = _getInt( stmt, sqlFieldIndex );
    }
    void loadUsername( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_username = _getString( stmt, sqlFieldIndex );
    }
    void loadPassword( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        winplus::AnsiString encrypt;
        _getBlob( stmt, sqlFieldIndex, &encrypt );
        m_password = winplus::Utf8ToString( DecryptContent(encrypt) ).c_str();
    }
    void loadProtectLevel( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_protectLevel = _getInt( stmt, sqlFieldIndex );
    }
    void loadCondone( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_condone = _getInt( stmt, sqlFieldIndex );
    }
    void loadCurCondone( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_curCondone = _getInt( stmt, sqlFieldIndex );
    }
    void loadUnlockTime( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_unlockTime = _getInt( stmt, sqlFieldIndex );
    }
    void loadHotkey( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_hotkey = _getInt( stmt, sqlFieldIndex );
    }
    void loadRegTime( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_regTime = _getInt( stmt, sqlFieldIndex );
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

// ע���û�, ע��ɹ�����true, ����false
// user����m_id,m_condone,m_curCondone,m_unlockTime,m_regTime
bool RegisterUser( sqlite3 * db, User const & newUser );

// ��¼�û�, ִ����֤�ɹ�����true,����ȡUser����, ���򷵻�false.
bool LoginUser( sqlite3 * db, CString const & username, CString const & password, User * userData );
// �����û���Ϣ
bool LoadUser( sqlite3 * db, CString const & username, User * userData );
// ɾ���û�
bool DeleteUser( sqlite3 * db, CString const & username );
// ��֤�û�����
bool VerifyUserPassword( sqlite3 * db, CString const & username, CString const & password );
// �޸��û���Ϣ
bool ModifyUserEx( sqlite3 * db, CString const & username, UINT modifiedFieldBits, User const & newUser );

// Types -----------------------------------------------------------------

//һ���˻������������
struct AccountType : public Fields
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

    int bindTypeName( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindString( stmt, sqlParamIndex, m_typeName );
    }
    int bindSafeRank( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindInt( stmt, sqlParamIndex, m_safeRank );
    }
    
    void loadTypeName( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_typeName = _getString( stmt, sqlFieldIndex );
    }
    void loadSafeRank( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_safeRank = _getInt( stmt, sqlFieldIndex );
    }

    AccountType()
    {
        m_safeRank = 0;
    }
};

typedef CArray<AccountType, AccountType const &> AccountTypeArray;

// �����˻������Ϣ,���ؼ�¼��
int LoadAccountTypes( sqlite3 * db, AccountTypeArray * types );

// ��ȡһ������
bool GetAccountType( sqlite3 * db, CString const & typeName, AccountType * type );

// ����˻������Ϣ, �ɹ�����true, ���򷵻�false.
bool AddAccountType( sqlite3 * db, AccountType const & newType );
// �޸��˻������Ϣ, �ɹ�����true, ���򷵻�false.
bool ModifyAccountType( sqlite3 * db, CString const & typeName, AccountType const & newType );
// ɾ���˻������Ϣ, �ɹ�����true, ���򷵻�false.
bool DeleteAccountType( sqlite3 * db, CString const & typeName );

// Cates -----------------------------------------------------------------
struct AccountCate : public Fields
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

    int bindId( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindInt( stmt, sqlParamIndex, m_id );
    }
    int bindCateName( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindString( stmt, sqlParamIndex, m_cateName );
    }
    int bindCateDesc( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindString( stmt, sqlParamIndex, m_cateDesc );
    }
    int bindTypeName( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindString( stmt, sqlParamIndex, m_typeName );
    }
    int bindUrl( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindString( stmt, sqlParamIndex, m_url );
    }
    int bindIcoPath( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindString( stmt, sqlParamIndex, m_icoPath );
    }
    int bindStartup( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindString( stmt, sqlParamIndex, m_startup );
    }
    int bindKeywords( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindString( stmt, sqlParamIndex, m_keywords );
    }
    int bindTimeWriten( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindInt( stmt, sqlParamIndex, m_timeWriten );
    }
    
    void loadId( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_id = _getInt( stmt, sqlFieldIndex );
    }
    void loadCateName( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_cateName = _getString( stmt, sqlFieldIndex );
    }
    void loadCateDesc( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_cateDesc = _getString( stmt, sqlFieldIndex );
    }
    void loadTypeName( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_typeName = _getString( stmt, sqlFieldIndex );
    }
    void loadUrl( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_url = _getString( stmt, sqlFieldIndex );
    }
    void loadIcoPath( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_icoPath = _getString( stmt, sqlFieldIndex );
    }
    void loadStartup( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_startup = _getString( stmt, sqlFieldIndex );
    }
    void loadKeywords( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_keywords = _getString( stmt, sqlFieldIndex );
    }
    void loadTimeWriten( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_timeWriten = _getInt( stmt, sqlFieldIndex );
    }

    AccountCate()
    {
        m_id = 0;
        m_timeWriten = 0;
    }

};

typedef CArray<AccountCate, AccountCate const &> AccountCateArray;

// �����˻�������Ϣ, ���ؼ�¼��
int LoadAccountCates( sqlite3 * db, AccountCateArray * cates );

// ����һ���˻�������Ϣ
bool GetAccountCate( sqlite3 * db, int id, AccountCate * cate );

// ����˻�������Ϣ���ɹ�����ID��ʧ�ܷ���0
// newCate����m_id,m_timeWriten
int AddAccountCate( sqlite3 * db, AccountCate const & newCate );

// �޸��˻�������Ϣ
// newCate����m_id,m_timeWriten
bool ModifyAccountCate( sqlite3 * db, int id, AccountCate const & newCate );

// ɾ���˻�������Ϣ
bool DeleteAccountCate( sqlite3 * db, int id );

// ���������˻������ID�Լ���Ӧ�İ�ȫֵ
int LoadAccountCatesSafeRank( sqlite3 * db, CUIntArray * cateIds, CUIntArray * typeSafeRanks );

// ��ȡָ��cateId�����
bool GetTypeByCateId( sqlite3 * db, int cateId, AccountType * type );

// Accounts --------------------------------------------------------------
struct Account : public Fields
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

    int bindMyName( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindString( stmt, sqlParamIndex, m_myName );
    }
    int bindAccountName( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        winplus::AnsiString encrypt;
        encrypt = EncryptContent( winplus::StringToUtf8( (LPCTSTR)m_accountName ) );
        return _bindBlob( stmt, sqlParamIndex, encrypt.c_str(), encrypt.size() );
    }
    int bindAccountPwd( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        winplus::AnsiString encrypt;
        encrypt = EncryptContent( winplus::StringToUtf8( (LPCTSTR)m_accountPwd ) );
        return _bindBlob( stmt, sqlParamIndex, encrypt.c_str(), encrypt.size() );
    }
    int bindCateId( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindInt( stmt, sqlParamIndex, m_cateId );
    }
    int bindUserId( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindInt( stmt, sqlParamIndex, m_userId );
    }
    int bindSafeRank( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindInt( stmt, sqlParamIndex, m_safeRank );
    }
    int bindComment( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindString( stmt, sqlParamIndex, m_comment );
    }
    int bindTime( sqlite3_stmt * stmt, int sqlParamIndex ) const
    {
        return _bindInt( stmt, sqlParamIndex, m_time );
    }

    void loadMyName( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_myName = _getString( stmt, sqlFieldIndex );
    }
    void loadAccountName( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        winplus::AnsiString encrypt;
        _getBlob( stmt, sqlFieldIndex, &encrypt );
        m_accountName = winplus::Utf8ToString( DecryptContent(encrypt) ).c_str();
    }
    void loadAccountPwd( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        winplus::AnsiString encrypt;
        _getBlob( stmt, sqlFieldIndex, &encrypt );
        m_accountPwd = winplus::Utf8ToString( DecryptContent(encrypt) ).c_str();
    }
    void loadCateId( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_cateId = _getInt( stmt, sqlFieldIndex );
    }
    void loadUserId( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_userId = _getInt( stmt, sqlFieldIndex );
    }
    void loadSafeRank( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_safeRank = _getInt( stmt, sqlFieldIndex );
    }
    void loadComment( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_comment = _getString( stmt, sqlFieldIndex );
    }
    void loadTime( sqlite3_stmt * stmt, int sqlFieldIndex )
    {
        m_time = _getInt( stmt, sqlFieldIndex );
    }

    Account()
    {
        m_cateId = 0;
        m_userId = 0;
        m_safeRank = 0;
        m_time = 0;
    }

};

typedef CArray<Account, Account const &> AccountArray;

// ����ָ���û����е��˻���Ϣ,��ָ������:-1Ϊȫ������
int LoadAccounts( sqlite3 * db, int userId, AccountArray * accounts, int cateId = -1 );

// ��ȡָ���û���һ���˻���Ϣ
// ����(user,myname)
bool GetAccount( sqlite3 * db, int userId, CString const & myName, Account * account );

// ���һ��ָ���û����˻����ɹ�true��ʧ��false��
// ����(user,myname)
// ��ζ��ÿ���û�(user)ӵ���˻���myname���벻ͬ������ʧ�ܡ��޸�ʱҲһ����
// newAccount����m_time
bool AddAccount( sqlite3 * db, Account const & newAccount );

// �޸�һ��ָ���û����˻�
// ����(user,myname)
// newAccount����m_time
bool ModifyAccount( sqlite3 * db, int userId, CString const & myName, Account const & newAccount );

// ɾ��һ��ָ���û����˻�
bool DeleteAccount( sqlite3 * db, int userId, CString const & myName );

// Others ----------------------------------------------------------------

// ȡ��һ����ȷ��Account MyName�Ա�����˻�
CString GetCorrectAccountMyName( sqlite3 * db, int userId, CString const & myName );

// ��ȡ���ݿ�ȫ������
int LoadTableNames( sqlite3 * db, winplus::StringArray * tableNames, winplus::String const & like = _T("am\\_%") );

// ��ȡ���ݿ��DDL,����SQL�������
int DumpDDL( sqlite3 * db, winplus::String * ddl, winplus::String const & like = _T("am\\_%") );

// �ж�ExeName�Ƿ�Ϊһ�������
bool IsBrowserExeName( sqlite3 * db, CString const & exeName, CString * browserTitle );


//////////////////////////////////////////////////////////////////////////
// �ӿ�
//////////////////////////////////////////////////////////////////////////

// ����ListView���ݺ��б���
interface IUpdateListView
{
    enum
    {
        UPDATE_LOAD_DATA = 1,
        UPDATE_LIST_ITEMS = 2,
    };
    // �������ݺ��б���
    // itemIndexΪ-1ʱ,�������ݿ���ȫ�������ݲ����������б�,��һ�ε���������
    // itemIndexΪ�б�������ʱ,��ȡ����ָʾ�������¼,ͨ��������ѯ��������������
    // ����flag��ָʾ���в���:
    // UPDATE_LOAD_DATA �����ݿ�������ݵ������д���
    // UPDATE_LIST_ITEMS �������е����ݸ��µ��б���
    virtual void UpdateList( int flag = UPDATE_LOAD_DATA | UPDATE_LIST_ITEMS, long itemIndex = -1 ) PURE;

};

//////////////////////////////////////////////////////////////////////////

// ������ʾ��

// �����ʹ���
#define WarningError( str, title )\
    MessageBox( str, title, MB_OK | MB_ICONEXCLAMATION );
// �����ʹ���
#define FatalError( str, title )\
    MessageBox( str, title, MB_OK | MB_ICONERROR );


//////////////////////////////////////////////////////////////////////////

/* ��һ������������Ͽ�ؼ� */
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
