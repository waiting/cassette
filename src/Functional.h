
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

// ������ת��Ϊansi_string�����ƴ�
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
string Base64Encode( ansi_string const & data );
ansi_string Base64Decode( string const & base64Str );
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

ansi_string EncryptContent( ansi_string const & content );
ansi_string DecryptContent( ansi_string const & encryptContent );
#define EncryptCArray(data) EncryptContent( BufferToAnsiString( data, sizeof(data) ) )
#define EncryptBuffer( buf, size ) EncryptContent( BufferToAnsiString( buf, size ) )
#define DecryptCArray(data) DecryptContent( BufferToAnsiString( data, sizeof(data) ) )
#define DecryptBuffer( buf, size ) DecryptContent( BufferToAnsiString( buf, size ) )

// �����ı��е�$xxx$�����Զ�����
string ExplainCustomVars( string const & str );

// ��ȡ��ִ���ļ�����·��, ĩβ��Ŀ¼�ָ��
CString GetExecutablePath();

//////////////////////////////////////////////////////////////////////////
// ���ݿ���ز���
//////////////////////////////////////////////////////////////////////////

// ע���û�, ע��ɹ�����true, ����false.
bool RegisterUser( CString const & username, CString const & password, int protectLevel, UINT32 hotkey );
// ����һ���û�
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

// ��¼�û�, ִ����֤�ɹ�����true,����ȡUser����, ���򷵻�false.
bool LoginUser( CString const & username, CString const & password, UserInfo * userInfo );
// �����û���Ϣ
bool LoadUser( CString const & username, UserInfo * userInfo );
// ɾ���û�
bool DeleteUser( CString const & username );
// ��֤�û�����
bool VerifyUserPassword( CString const & username, CString const & password );
// �޸��û���Ϣ
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

// �����˻������Ϣ, ���ؼ�¼��
int LoadAccountTypes( CStringArray * typeNames, CUIntArray * safeRanks );

// ��ȡһ������
bool GetAccountType( CString const & typeName, int * safeRank );

// ����˻������Ϣ, �ɹ�����true, ���򷵻�false.
bool AddAccountType( CString const & typeName, int safeRank );
// �޸��˻������Ϣ, �ɹ�����true, ���򷵻�false.
bool ModifyAccountType( CString const & typeName, CString const & newTypeName, int newSafeRank );
// ɾ���˻������Ϣ, �ɹ�����true, ���򷵻�false.
bool DeleteAccountType( CString const & typeName );

// �����˻�������Ϣ, ���ؼ�¼��
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

// ����һ���˻�������Ϣ
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

// ����˻�������Ϣ���ɹ�����ID��ʧ�ܷ���0
int AddAccountCate(
	CString const & cateName,
	CString const & cateDesc,
	CString const & typeName,
	CString const & url,
	CString const & icoPath,
	CString const & startup,
	CString const & keywords
);
// �޸��˻�������Ϣ
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
// ɾ���˻�������Ϣ
bool DeleteAccountCate( int id );

// ���������˻������ID�Լ���Ӧ�İ�ȫֵ
int LoadAccountCatesSafeRank( CUIntArray * cateIds, CUIntArray * typeSafeRanks );

// ��ȡָ��cateId�����
bool GetTypeByCateId( int cateId, CString * typeName, int * safeRank );

// ���뵱ǰ��¼�û����е��˻���Ϣ
int LoadAccounts(
	CStringArray * myNames,
	CStringArray * accountNames,
	CStringArray * accountPwds,
	CUIntArray * cateIds,
	CUIntArray * safeRanks,
	CStringArray * comments,
	CUIntArray * times
);

// ��ȡ��ǰ��¼�û���һ���˻���Ϣ
bool GetAccount(
	CString const & myName,
	CString * accountName,
	CString * accountPwd,
	int * cateId,
	int * safeRank,
	CString * comment,
	int * time
);

// ���һ����ǰ��¼�û����˻����ɹ�true��ʧ��false��
// ����(user,myname)
// ��ζ��ÿ���û�(user)ӵ���˻���myname���벻ͬ������ʧ�ܡ��޸�ʱҲһ����
bool AddAccount(
	CString const & myName,
	CString const & accountName,
	CString const & accountPwd,
	int cateId,
	int safeRank,
	CString const & comment
);
// �޸�һ����ǰ��¼�û����˻�
// ����(user,myname)
bool ModifyAccount(
	CString const & myName,
	CString const & newMyName,
	CString const & newAccountName,
	CString const & newAccountPwd,
	int newCateId,
	int newSafeRank,
	CString const & newComment
);
// ɾ��һ����ǰ��¼�û����˻�
bool DeleteAccount( CString const & myName );

// ��������
bool BackupData( CString const & filename );
// �ָ�����
bool ResumeData( CString const & filename );
// ȡ��һ����ȷ��Account MyName�Ա�����˻�
CString GetCorrectAccountMyName( CString const & myName );

//////////////////////////////////////////////////////////////////////////
// �ӿ�

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
