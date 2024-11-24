///////////////////////////////////////////////////////////////////////////
// Name:         Functional.cpp
// Purpos:       �������еĹ��ܲ���
///////////////////////////////////////////////////////////////////////////
#include "Cassette.h"
#include "CassetteApp.h"
#include "Functional.h"

String ExplainCustomVars( String const & str )
{
    String vars[] = {
        TEXT("$ROOT$"),
        TEXT("$CURRENT$")
    };
    String vals[] = {
        ModulePath(),
        GetCurrentDir()
    };
    MultiMatch mm( vars, vals );
    return mm.replace(str);
}

// Database -------------------------------------------------------------------------------
bool RegisterUser( eiendb::Database & db, User const & newUser )
{
    Buffer encPassword = EncryptContent( Buffer( (LPCTSTR)newUser.m_password, newUser.m_password.GetLength(), true ) );

    Mixed fields;
    fields.addPair()
        ( "name", (LPCTSTR)newUser.m_username )
        ( "pwd", encPassword )
        ( "protect", newUser.m_protectLevel )
        ( "condone", 3 )
        ( "cur_condone", 3 )
        ( "unlock_time", GetUtcTime() )
        ( "hotkey", newUser.m_hotkey )
        ( "time", GetUtcTime() )
        ;

    try
    {
        auto mdf = db.mdf("am_users");
        return mdf->addNew(fields);
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return false;
}

bool LoginUser( eiendb::Database & db, CString const & username, CString const & password, User * userData )
{
    bool ret = false;

    try
    {
        User tmpUserInfo;

        auto resUser = db->query( db->buildStmt("SELECT * FROM am_users WHERE name = ?;", (LPCTSTR)username ) );
        StringMixedMap userFields;
        if ( resUser->fetchMap(&userFields) ) // �ҵ�����û�
        {
            int nowTime = (int)GetUtcTime();

            tmpUserInfo.m_unlockTime = userFields["unlock_time"];
            tmpUserInfo.m_condone = userFields["condone"];
            tmpUserInfo.m_curCondone = userFields["cur_condone"];

            if ( nowTime < tmpUserInfo.m_unlockTime ) // ��ǰʱ��С������ʱ��˵������������
            {
                int hours = ( tmpUserInfo.m_unlockTime - nowTime ) / 3600;
                int minutes = ( ( tmpUserInfo.m_unlockTime - nowTime ) - hours * 3600 ) / 60;
                int seconds = ( tmpUserInfo.m_unlockTime - nowTime ) - hours * 3600 - minutes * 60;
                AfxGetMainWnd()->WarningError( Format( _T("�û������У���������Ҫ%dСʱ%d��%d��"), hours, minutes, seconds ).c_str(), _T("����") );
                ret = false;
                goto ExitProc;
            }
            else
            {
                if ( tmpUserInfo.m_curCondone < 1 ) // ��ʾ���Ǹմ�����״̬�ָ�,����cur_condone=condone
                {
                    tmpUserInfo.m_curCondone = tmpUserInfo.m_condone;
                    ModifyUserEx( db, username, Mixed().addPair()( "cur_condone", tmpUserInfo.m_curCondone ) );
                }
            }

            // ��֤����
            tmpUserInfo.m_password = DecryptContent( userFields["pwd"].toAnsi() ).c_str();
            if ( tmpUserInfo.m_password != password )
            {
                // ���ٵ�ǰ�ݴ����
                tmpUserInfo.m_curCondone--;
                ModifyUserEx( db, username, Mixed().addPair()( "cur_condone", tmpUserInfo.m_curCondone ) );

                AfxGetMainWnd()->WarningError( Format( _T("���벻��ȷ���㻹ʣ%d�λ���"), tmpUserInfo.m_curCondone ).c_str(), _T("����") );

                if ( tmpUserInfo.m_curCondone < 1 ) // û���ݴ���,�����û�
                {
                    int lockTime = 3600 * 3;
                    tmpUserInfo.m_unlockTime = GetUtcTime() + lockTime;
                    ModifyUserEx( db, username, Mixed().addPair()( "unlock_time", tmpUserInfo.m_unlockTime ) );
                }
                ret = false;
                goto ExitProc;
            }
            // ��¼��֤�ɹ�,����cur_condone=condone
            tmpUserInfo.m_curCondone = tmpUserInfo.m_condone;
            ModifyUserEx( db, username, Mixed().addPair()( "cur_condone", tmpUserInfo.m_curCondone ) );

            if ( userData != NULL )
            {
                userData->m_id = userFields["id"];
                userData->m_username = userFields["name"].toAnsi().c_str();
                userData->m_protectLevel = userFields["protect"];
                userData->m_condone = userFields["condone"];
                userData->m_curCondone = userFields["cur_condone"];
                userData->m_unlockTime = userFields["unlock_time"];
                userData->m_hotkey = userFields["hotkey"];
                userData->m_regTime = userFields["time"];
            }
            ret = true;
            goto ExitProc;
        }
        else //û��ָ�����Ƶ��û�
        {
            AfxGetMainWnd()->WarningError( _T("û�д��û�"), _T("����") );
            ret = false;
            goto ExitProc;
        }
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }

ExitProc:
    return ret;
}

bool LoadUser( eiendb::Database & db, CString const & username, User * userData )
{
    bool ret = false;
    try
    {
        auto resUser = db->query( db->buildStmt("SELECT * FROM am_users WHERE name = ?;", (LPCTSTR)username ) );
        StringMixedMap userFields;
        if( resUser->fetchMap(&userFields) ) // �ҵ�����û�
        {
            if ( userData != NULL )
            {
                userData->m_id = userFields["id"];
                userData->m_username = userFields["name"].toAnsi().c_str();
                userData->m_protectLevel = userFields["protect"];
                userData->m_condone = userFields["condone"];
                userData->m_curCondone = userFields["cur_condone"];
                userData->m_unlockTime = userFields["unlock_time"];
                userData->m_hotkey = userFields["hotkey"];
                userData->m_regTime = userFields["time"];
            }
            ret = true;
        }
        else
        {
            //AfxGetMainWnd()->WarningError( _T("û�д��û�"), _T("����") );
            ret = false;
        }
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") ); //*/
    }

    return ret;
}

bool DeleteUser( eiendb::Database & db, CString const & username )
{
    try
    {
        auto mdf = db.mdf("am_users");
        return mdf->deleteEx( "name=" + db->escape( (LPCTSTR)username ) );
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") ); //*/
    }
    return false;
}

bool VerifyUserPassword( eiendb::Database & db, CString const & username, CString const & password )
{
    try
    {
        Buffer encPassword = EncryptContent( Buffer( (LPCTSTR)password, password.GetLength(), true ) );
        auto res = db->query( db->buildStmt("SELECT pwd = ? FROM am_users WHERE name = ?;", Mixed().add()(encPassword)((LPCTSTR)username) ) );
        MixedArray f;
        if ( res->fetchArray(&f) )
        {
            return f[0].toBool();
        }
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") ); //*/
    }
    return false;
}

bool ModifyUserEx( eiendb::Database & db, CString const & username, Mixed const & userFields )
{
    try
    {
        auto mdf = db.mdf("am_users");
        return mdf->modifyEx( userFields, "name=" + db->escape( (LPCTSTR)username ) );
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return false;
}

int LoadAccountTypes( eiendb::Database & db, AccountTypeArray * types )
{
    int count = 0;
    IF_PTR(types)->RemoveAll();
    try
    {
        auto resAccountTypes = db->query("SELECT * FROM am_account_types ORDER BY rank;");
        MixedArray f;
        while ( resAccountTypes->fetchArray(&f) )
        {
            AccountType t;
            t.m_typeName = f[0].toAnsi().c_str();
            t.m_safeRank = f[1];
            IF_PTR(types)->Add(t);
            count++;
        }
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return count;
}

bool GetAccountType( eiendb::Database & db, CString const & typeName, AccountType * type )
{
    bool ret = false;
    try
    {
        auto resAccountType = db->query( db->buildStmt( "SELECT * FROM am_account_types WHERE name = ?;", (LPCTSTR)typeName ) );
        MixedArray f;
        if ( resAccountType->fetchArray(&f) )
        {
            IF_PTR(type)->m_typeName = f[0].toAnsi().c_str();
            IF_PTR(type)->m_safeRank = f[1];
            ret = true;
        }
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return ret;
}

bool AddAccountType( eiendb::Database & db, Mixed const & newType )
{
    Mixed newAccountType = newType;
/*
    newAccountType.addPair()
        ( "name", (LPCTSTR)newType.m_typeName )
        ( "rank", newType.m_safeRank )
        ;
*/

    try
    {
        auto mdf = db.mdf("am_account_types");
        return mdf->addNew(newAccountType);
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return false;
}

bool ModifyAccountType( eiendb::Database & db, CString const & typeName, Mixed const & newTypeFields )
{
    try
    {
        auto mdf = db.mdf("am_account_types");
        return mdf->modifyEx( newTypeFields, "name=" + db->escape( (LPCTSTR)typeName ) );
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return false;
}

bool DeleteAccountType( eiendb::Database & db, CString const & typeName )
{
    try
    {
        auto resCountAccountCates = db->query( db->buildStmt( "SELECT COUNT(*) > 0 FROM am_account_cates WHERE type = ?;", (LPCTSTR)typeName ) );
        MixedArray f;
        if ( resCountAccountCates->fetchArray(&f) && f[0].toInt() > 0 )
        {
            // ����ɾ��������
            AfxGetMainWnd()->WarningError( _T("�������¹������˻����࣬��˲���ɾ��"), _T("����") );
            return false;
        }

        auto mdf = db.mdf("am_account_types");
        return mdf->deleteEx( "name=" + db->escape( (LPCTSTR)typeName ) );
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") ); //*/
    }
    return false;
}

int LoadAccountCates( eiendb::Database & db, AccountCateArray * cates )
{
    int count = 0;
    IF_PTR(cates)->RemoveAll();
    try
    {
        auto resAccountCates = db->query("SELECT * FROM am_account_cates ORDER BY id;");
        StringMixedMap fields;
        while ( resAccountCates->fetchMap(&fields) )
        {
            AccountCate cate;
            cate.m_id = fields["id"];
            cate.m_cateName = fields["name"].toAnsi().c_str();
            cate.m_cateDesc = fields["desc"].toAnsi().c_str();
            cate.m_typeName = fields["type"].toAnsi().c_str();
            cate.m_url = fields["url"].toAnsi().c_str();
            cate.m_icoPath = fields["icon"].toAnsi().c_str();
            cate.m_startup = fields["startup"].toAnsi().c_str();
            cate.m_keywords = fields["keywords"].toAnsi().c_str();
            cate.m_timeWriten = fields["time"];
            IF_PTR(cates)->Add(cate);

            count++;
        }
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return count;
}

bool GetAccountCate( eiendb::Database & db, int id, AccountCate * cate )
{
    bool ret = false;
    try
    {
        auto resAccountCate = db->query( db->buildStmt( "SELECT * FROM am_account_cates WHERE id = ?;", id ) );
        StringMixedMap f;
        if ( resAccountCate->fetchMap(&f) )
        {
            IF_PTR(cate)->m_id = f["id"];
            IF_PTR(cate)->m_cateName = f["name"].toAnsi().c_str();
            IF_PTR(cate)->m_cateDesc = f["desc"].toAnsi().c_str();
            IF_PTR(cate)->m_typeName = f["type"].toAnsi().c_str();
            IF_PTR(cate)->m_url = f["url"].toAnsi().c_str();
            IF_PTR(cate)->m_icoPath = f["icon"].toAnsi().c_str();
            IF_PTR(cate)->m_startup = f["startup"].toAnsi().c_str();
            IF_PTR(cate)->m_keywords = f["keywords"].toAnsi().c_str();
            IF_PTR(cate)->m_timeWriten = f["time"];
            ret = true;
        }
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return ret;
}

int AddAccountCate( eiendb::Database & db, Mixed const & newCate )
{
    Mixed newAccountCate = newCate;
    newAccountCate.del("id");
    newAccountCate.addPair()
        ( "time", (int)GetUtcTime() )
    ;

    try
    {
        auto mdf = db.mdf("am_account_cates");
        if ( mdf->addNew(newAccountCate) )
        {
            return (int)db->insertId();
        }
        else
        {
            AfxGetMainWnd()->FatalError( newAccountCate.myJson().c_str(), db->error().c_str() );
            return 0;
        }
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return 0;
}

bool ModifyAccountCate( eiendb::Database & db, int id, Mixed const & newCateFields )
{
    try
    {
        auto mdf = db.mdf("am_account_cates");
        return mdf->modify( newCateFields, id );
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return false;
}

bool DeleteAccountCate( eiendb::Database & db, int id )
{
    try
    {
        auto resCountAccounts = db->query( db->buildStmt( "SELECT COUNT(*) > 0 FROM am_accounts WHERE cate = ?;", id ) );
        MixedArray f;
        if ( resCountAccounts->fetchArray(&f) && f[0].toInt() > 0 )
        {
            // ����ɾ��������
            AfxGetMainWnd()->WarningError( _T("�������¹������˻�����˲���ɾ��"), _T("����") );
            return false;
        }
        auto mdf = db.mdf("am_account_cates");
        return mdf->deleteOne(id);
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return false;
}

int LoadAccountCatesSafeRank( eiendb::Database & db, CUIntArray * cateIds, CUIntArray * typeSafeRanks )
{
    int count = 0;
    IF_PTR(cateIds)->RemoveAll();
    IF_PTR(typeSafeRanks)->RemoveAll();
    try
    {
        auto resAccountCatesSafeRank = db->query("SELECT c.id, t.rank from am_account_cates AS c Left JOIN am_account_types AS t ON t.name = c.type;");
        MixedArray f;
        while ( resAccountCatesSafeRank->fetchArray(&f) )
        {
            IF_PTR(cateIds)->Add( f[0] );
            IF_PTR(typeSafeRanks)->Add( f[1] );

            count++;
        }
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }

    return count;

}

bool GetTypeByCateId( eiendb::Database & db, int cateId, AccountType * type )
{
    bool ret = false;
    try
    {
        auto resAccountType = db->query( db->buildStmt( "SELECT c.id, t.name, t.rank FROM am_account_cates AS c Left JOIN am_account_types AS t ON t.name = c.type WHERE c.id = ?;", cateId ) );
        MixedArray f;
        if ( resAccountType->fetchArray(&f) )
        {
            ASSERT( cateId == f[0].toInt() );
            IF_PTR(type)->m_typeName = f[1].toAnsi().c_str();
            IF_PTR(type)->m_safeRank = f[2];
            ret = true;
        }
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return ret;
}

int LoadAccounts( eiendb::Database & db, int userId, AccountArray * accounts, int cateId )
{
    int count = 0;
    IF_PTR(accounts)->RemoveAll();
    try
    {
        Mixed params;
        params.add()(userId);
        if ( cateId != -1 )
            params.add(cateId);

        String sql = "SELECT * FROM am_accounts WHERE user = ?";
        sql += ( cateId != -1 ? " AND cate = ?" : "" );
        sql += " ORDER BY cate;";

        auto resAccounts = db->query( db->buildStmt( sql, params ) );
        StringMixedMap f;
        while ( resAccounts->fetchMap(&f) )
        {
            Account account;
            account = f;
            IF_PTR(accounts)->Add(account);
            count++;
        }
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return count;
}

bool GetAccount( eiendb::Database & db, int userId, CString const & myName, Account * account )
{
    bool ret = false;
    try
    {
        auto resAccount = db->query( db->buildStmt( "SELECT * FROM am_accounts WHERE user = ? AND myname = ?;", Mixed().add()(userId)((LPCTSTR)myName) ) );
        StringMixedMap f;
        if ( resAccount->fetchMap(&f) )
        {
            IF_PTR(account)->m_myName = f["myname"].toAnsi().c_str();
            IF_PTR(account)->m_accountName = Utf8ToString( DecryptContent( f["account_name"].toAnsi() ) ).c_str();
            IF_PTR(account)->m_accountPwd = Utf8ToString( DecryptContent( f["account_pwd"].toAnsi() ) ).c_str();
            IF_PTR(account)->m_cateId = f["cate"];
            IF_PTR(account)->m_userId = f["user"];
            IF_PTR(account)->m_safeRank = f["safe_rank"];
            IF_PTR(account)->m_comment = f["comment"].toAnsi().c_str();
            IF_PTR(account)->m_time = f["time"];
            ret = true;
        }
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return ret;
}

bool AddAccount( eiendb::Database & db, Mixed const & newAccount )
{
    Mixed newAccountMixed = newAccount;
    newAccountMixed.addPair()
        //( "myname", (LPCTSTR)newAccount.m_myName )
        //( "account_name", EncryptContent( LocalToUtf8((LPCTSTR)newAccount.m_accountName) ) )
        //( "account_pwd", EncryptContent( LocalToUtf8((LPCTSTR)newAccount.m_accountPwd) ) )
        //( "cate", newAccount.m_cateId )
        //( "user", newAccount.m_userId )
        //( "safe_rank", newAccount.m_safeRank )
        //( "comment", (LPCTSTR)newAccount.m_comment )
        ( "time", (int)GetUtcTime() )
        ;
    try
    {
        auto mdf = db.mdf("am_accounts");
        return mdf->addNew(newAccountMixed);
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return false;
}

bool ModifyAccount( eiendb::Database & db, int userId, CString const & myName, Mixed const & newAccountFields )
{
    try
    {
        auto mdf = db.mdf("am_accounts");
        return mdf->modifyEx( newAccountFields, "user=" + Mixed(userId).toAnsi() + " AND myname=" + db->escape( (LPCTSTR)myName ) );
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return false;
}

bool DeleteAccount( eiendb::Database & db, int userId, CString const & myName )
{
    try
    {
        auto mdf = db.mdf("am_accounts");
        return mdf->deleteEx( "user=" + Mixed(userId).toAnsi() + " AND myname=" + db->escape( (LPCTSTR)myName ) );
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return false;
}

CString GetCorrectAccountMyName( eiendb::Database & db, int userId, CString const & myName )
{
    CString result;
    int number = 0;

    result = myName;
    try
    {
        auto stmt = db->buildStmt( "SELECT myname FROM am_accounts WHERE user = ? AND myname = ?;", Mixed().add()(userId)((LPCTSTR)result) );
        auto resAccount = db->query(stmt);
        while ( resAccount->rowsCount() > 0 )
        {
            number++;
            result.Format( "%s%d", (LPCTSTR)myName, number );

            stmt = db->buildStmt( "SELECT myname FROM am_accounts WHERE user = ? AND myname = ?;", Mixed().add()(userId)((LPCTSTR)result) );
            resAccount = db->query(stmt);
        }
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return result;
}

int LoadTableNames( eiendb::Database & db, StringArray * tableNames, String const & like )
{
    int count = 0;
    IF_PTR(tableNames)->clear();
    try
    {
        auto resTableNames = db->query( db->buildStmt( "SELECT tbl_name FROM sqlite_master WHERE type = \'table\' AND tbl_name LIKE ? ESCAPE \'\\\';", like ) );
        MixedArray f;
        while ( resTableNames->fetchArray(&f) )
        {
            IF_PTR(tableNames)->push_back(f[0]);
            count++;
        }
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return count;
}

int DumpDDL( eiendb::Database & db, String * ddl, String const & like )
{
    int count = 0;
    try
    {
        auto resDdlSqls = db->query( db->buildStmt( "SELECT sql FROM sqlite_master WHERE tbl_name LIKE ? ESCAPE \'\\\' AND NOT sql IS Null ORDER BY tbl_name;", like ) );
        MixedArray f;
        while ( resDdlSqls->fetchArray(&f) )
        {
            ASSIGN_PTR(ddl) += f[0].toAnsi() + _T(";") + LineSep;
            count++;
        }
    }
    catch ( Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("���ݿ����") );
    }
    return count;
}

bool IsBrowserExeName( eiendb::Database & db, String const & exeName, String * browserTitle )
{
    bool ret = false;
    try
    {
        auto resIsBrowser = db->query( db->buildStmt( "SELECT title FROM am_browsers WHERE exe_name = ?;", exeName ) );
        MixedArray f;
        if ( resIsBrowser->fetchArray(&f) )
        {
            ASSIGN_PTR(browserTitle) = f[0].toAnsi();
            ret = true;
        }
    }
    catch ( Error const & e )
    {
        ErrBox( e.what(), _T("���ݿ����"), *AfxGetMainWnd() );
    }
    return ret;
}

int LoadBrowsers( eiendb::Database & db, BrowserArray * browsers )
{
    int count = 0;
    IF_PTR(browsers)->RemoveAll();
    try
    {
        auto rs = db->query("SELECT * FROM am_browsers ORDER BY id;");
        Mixed fields;
        while ( rs->fetchRow(&fields) )
        {
            Browser browser;
            browser = fields;
            IF_PTR(browsers)->Add(browser);
            count++;
        }
    }
    catch ( winux::Error const & e )
    {
        ErrBox( e.what(), _T("���ݿ����"), *AfxGetMainWnd() );
    }
    return count;
}

bool GetBrowser( eiendb::Database & db, int id, Browser * browser )
{
    bool ret = false;
    try
    {
        auto res = db->query( db->buildStmt( "SELECT * FROM am_browsers WHERE id = ?;", id ) );
        Mixed f;
        if ( res->fetchRow(&f) )
        {
            ASSIGN_PTR(browser) = f;

            ret = true;
        }
    }
    catch ( Error const & e )
    {
        ErrBox( e.what(), _T("���ݿ����"), *AfxGetMainWnd() );
    }
    return ret;
}

int AddBrowser( eiendb::Database & db, Mixed const & newBrowser )
{
    Mixed browserMixed = newBrowser;
    browserMixed.del("id");
    try
    {
        auto mdf = db.mdf("am_browsers");
        if ( mdf->addNew(browserMixed) )
        {
            return (int)db->insertId();
        }
        else
        {
            ErrBox( browserMixed.myJson(), db->error(), *AfxGetMainWnd() );
            return 0;
        }
    }
    catch ( Error const & e )
    {
        ErrBox( e.what(), _T("���ݿ����"), *AfxGetMainWnd() );
    }
    return 0;
}

bool ModifyBrowser( eiendb::Database & db, int id, Mixed const & modifiedFields )
{
    try
    {
        auto mdf = db.mdf("am_browsers");
        return mdf->modify( modifiedFields, id );
    }
    catch ( Error const & e )
    {
        ErrBox( e.what(), _T("���ݿ����"), *AfxGetMainWnd() );
    }
    return false;
}

bool DeleteBrowser( eiendb::Database & db, int id )
{
    try
    {
        auto mdf = db.mdf("am_browsers");
        return mdf->deleteOne(id);
    }
    catch ( Error const & e )
    {
        ErrBox( e.what(), _T("���ݿ����"), *AfxGetMainWnd() );
    }
    return false;
}
