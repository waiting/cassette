
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

// Verify runable only other process Or parent window is MainFrame
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
    DISABLE_OBJECT_COPY(SetNullScopeOut)
};

// winplus::String��CString�ַ�������ת��
inline winplus::String CStringToString( CString const & str )
{
    return winplus::String( str.GetString(), str.GetLength() );
}

inline CString StringToCString( winplus::String const & str )
{
    return CString( str.c_str(), (int)str.length() );
}

// �����ı��е�$xxx$�����Զ�����
winplus::String ExplainCustomVars( winplus::String const & str );

// ��ȡ��ִ���ļ�����·��, ĩβ��Ŀ¼�ָ��
CString GetExecutablePath();

//////////////////////////////////////////////////////////////////////////
// ���ݿ���ز���
//////////////////////////////////////////////////////////////////////////

// Users -----------------------------------------------------------------
// ����һ���û����������

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

// ����ѯȫ���ֶ�ʱ�ſ����������ȷ���ֶ�����
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

// ע���û�, ע��ɹ�����true, ����false
// user����m_id,m_condone,m_curCondone,m_unlockTime,m_regTime
bool RegisterUser( eiendb::Database & db, User const & newUser );

// ��¼�û�, ִ����֤�ɹ�����true,����ȡUser����, ���򷵻�false.
bool LoginUser( eiendb::Database & db, CString const & username, CString const & password, User * userData );
// �����û���Ϣ
bool LoadUser( eiendb::Database & db, CString const & username, User * userData );
// ɾ���û�
bool DeleteUser( eiendb::Database & db, CString const & username );
// ��֤�û�����
bool VerifyUserPassword( eiendb::Database & db, CString const & username, CString const & password );
// �޸��û���Ϣ
bool ModifyUserEx( eiendb::Database & db, CString const & username, winplus::Mixed const & userFields );

// Types -----------------------------------------------------------------

//һ���˻������������
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

// �����˻������Ϣ,���ؼ�¼��
int LoadAccountTypes( eiendb::Database & db, AccountTypeArray * types );

// ��ȡһ������
bool GetAccountType( eiendb::Database & db, CString const & typeName, AccountType * type );

// ����˻������Ϣ, �ɹ�����true, ���򷵻�false.
bool AddAccountType( eiendb::Database & db, winplus::Mixed const & newType );

// �޸��˻������Ϣ, �ɹ�����true, ���򷵻�false.
// name,rank
bool ModifyAccountType( eiendb::Database & db, CString const & typeName, winplus::Mixed const & newTypeFields );

// ɾ���˻������Ϣ, �ɹ�����true, ���򷵻�false.
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

// �����˻�������Ϣ, ���ؼ�¼��
int LoadAccountCates( eiendb::Database & db, AccountCateArray * cates );

// ����һ���˻�������Ϣ
bool GetAccountCate( eiendb::Database & db, int id, AccountCate * cate );

// ����˻�������Ϣ���ɹ�����ID��ʧ�ܷ���0
// newCate����m_id,m_timeWriten
int AddAccountCate( eiendb::Database & db, winplus::Mixed const & newCate );

// �޸��˻�������Ϣ
// id,name,desc,type,url,icon,startup,keywords,time
bool ModifyAccountCate( eiendb::Database & db, int id, winplus::Mixed const & newCateFields );

// ɾ���˻�������Ϣ
bool DeleteAccountCate( eiendb::Database & db, int id );

// ���������˻������ID�Լ���Ӧ�İ�ȫֵ
int LoadAccountCatesSafeRank( eiendb::Database & db, CUIntArray * cateIds, CUIntArray * typeSafeRanks );

// ��ȡָ��cateId�����
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

// ����ָ���û����е��˻���Ϣ,��ָ������:-1Ϊȫ������
int LoadAccounts( eiendb::Database & db, int userId, AccountArray * accounts, int cateId = -1 );

// ��ȡָ���û���һ���˻���Ϣ
// ����(user,myname)
bool GetAccount( eiendb::Database & db, int userId, CString const & myName, Account * account );

// ���һ��ָ���û����˻����ɹ�true��ʧ��false��
// ����(user,myname)
// ��ζ��ÿ���û�(user)ӵ���˻���myname���벻ͬ������ʧ�ܡ��޸�ʱҲһ����
// newAccount����m_time
bool AddAccount( eiendb::Database & db, winplus::Mixed const & newAccount );

// �޸�һ��ָ���û����˻�
// ����(user,myname)
// myname,account_name,account_pwd,cate,user,safe_rank,comment,time
bool ModifyAccount( eiendb::Database & db, int userId, CString const & myName, winplus::Mixed const & newAccountFields );

// ɾ��һ��ָ���û����˻�
bool DeleteAccount( eiendb::Database & db, int userId, CString const & myName );

// Others ----------------------------------------------------------------

// ȡ��һ����ȷ��Account MyName�Ա�����˻�
CString GetCorrectAccountMyName( eiendb::Database & db, int userId, CString const & myName );

// ��ȡ���ݿ�ȫ������
int LoadTableNames( eiendb::Database & db, winplus::StringArray * tableNames, winplus::String const & like = _T("am\\_%") );

// ��ȡ���ݿ��DDL,����SQL�������
int DumpDDL( eiendb::Database & db, winplus::String * ddl, winplus::String const & like = _T("am\\_%") );

// �ж�ExeName�Ƿ�Ϊһ�������
bool IsBrowserExeName( eiendb::Database & db, CString const & exeName, CString * browserTitle );


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
#define WarningError( str, title ) MessageBox( str, title, MB_OK | MB_ICONEXCLAMATION );
// �����ʹ���
#define FatalError( str, title ) MessageBox( str, title, MB_OK | MB_ICONERROR );


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
