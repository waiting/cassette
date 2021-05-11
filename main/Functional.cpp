///////////////////////////////////////////////////////////////////////////
// Name:         Functional.cpp
// Purpos:       程序所有的功能操作
///////////////////////////////////////////////////////////////////////////
#include "Cassette.h"
#include "CassetteApp.h"
#include "Functional.h"

static winplus::AnsiString const _base64_encode_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char const _base64_decode_chars[] = 
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
};

winplus::String Base64Encode( winplus::AnsiString const & data )
{
    winplus::String outStr;
    unsigned char c1, c2, c3;
    int i = 0;
    int size = data.size();

    while ( i < size )
    {
        // read the first unsigned char
        c1 = data[i++];
        if ( i==size )       // pad with "="
        {
            outStr += _base64_encode_chars[ c1>>2 ];
            outStr += _base64_encode_chars[ (c1&0x3)<<4 ];
            outStr += "==";
            break;
        }

        // read the second unsigned char
        c2 = data[i++];
        if ( i==size )       // pad with "="
        {
            outStr += _base64_encode_chars[ c1>>2 ];
            outStr += _base64_encode_chars[ ((c1&0x3)<<4) | ((c2&0xF0)>>4) ];
            outStr += _base64_encode_chars[ (c2&0xF)<<2 ];
            outStr += "=";
            break;
        }

        // read the third unsigned char
        c3 = data[i++];
        // convert into four bytes String
        outStr += _base64_encode_chars[ c1>>2 ];
        outStr += _base64_encode_chars[ ((c1&0x3)<<4) | ((c2&0xF0)>>4) ];
        outStr += _base64_encode_chars[ ((c2&0xF)<<2) | ((c3&0xC0)>>6) ];
        outStr += _base64_encode_chars[ c3&0x3F ];
    }

    return outStr;
}

winplus::AnsiString Base64Decode( winplus::String const & base64Str )
{
    winplus::AnsiString outData;
    char c1, c2, c3, c4;
    int i = 0;
    int len = base64Str.length();

    while ( i<len )
    {
        // read the first unsigned char
        do {
            c1 = _base64_decode_chars[ base64Str[i++] ];
        } while ( i<len && c1==-1);

        if ( c1==-1)
            break;

        // read the second unsigned char
        do {
            c2 = _base64_decode_chars[ base64Str[i++] ];
        } while ( i<len && c2==-1);

        if ( c2==-1 )
            break;

        // assamble the first unsigned char
        outData += char( (c1<<2) | ((c2&0x30)>>4) );

        // read the third unsigned char
        do {
            c3 = base64Str[i++];
            if ( c3==61 )       // meet with "=", break
                return outData;
            c3 = _base64_decode_chars[ c3 ];
        } while ( i<len && c3==-1);

        if ( c3==-1 )
            break;

        // assamble the second unsigned char
        outData += char( ((c2&0XF)<<4) | ((c3&0x3C)>>2) );

        // read the fourth unsigned char
        do {
            c4 = base64Str[i++];
            if ( c4==61 )       // meet with "=", break
                return outData;
            c4 = _base64_decode_chars[ c4 ];
        } while ( i<len && c4==-1 );

        if ( c4==-1 )
            break;

        // assamble the third unsigned char
        outData += char( ((c3&0x03)<<6) | c4 );
    }

    return outData;
}

int encrypt_internal( unsigned char const key_if_only_one, unsigned char const * data, int data_size, unsigned char * buf, int buf_size, int * out_size )
{
    if ( data_size < 1 )
    {
        *out_size = 0;
        return ERR_DATASIZE;
    }
    else if ( data_size == 1 )
    {
        buf[0] = data[0] ^ key_if_only_one;
        *out_size = 1;
    }
    else
    {
        int i, j;
        i = j = 0;
        while ( i < data_size && j < buf_size )
        {
            buf[j] = data[i];
            ++i;
            ++j;
        }
        *out_size = j;
        for ( i = 0; i < *out_size - 1; ++i )
        {
            buf[i] = buf[i] ^ buf[i + 1];
        }
        buf[i] = buf[i] ^ buf[0];
    }
    return ERR_SUCCEED;
}

int decrypt_internal( unsigned char const key_if_only_one, unsigned char const * encrypt_data, int data_size, unsigned char * buf, int buf_size, int * out_size )
{
    if ( data_size < 1 )
    {
        *out_size = 0;
        return ERR_DATASIZE;
    }
    else if ( data_size == 1 )
    {
        buf[0] = encrypt_data[0] ^ key_if_only_one;
        *out_size = 1;
    }
    else
    {
        int i, j;
        i = j = 0;
        while ( i < data_size && j < buf_size )
        {
            buf[j] = encrypt_data[i];
            ++i;
            ++j;
        }
        *out_size = j;
        buf[*out_size - 1] = buf[*out_size - 1] ^ buf[0];
        for ( i = *out_size - 1 - 1; i >= 0; --i )
        {
            buf[i] = buf[i] ^ buf[i + 1];
        }
    }
    return ERR_SUCCEED;
}

int raw_encrypt( unsigned char const * data, int data_size, unsigned char * buf, int buf_size, int * out_size )
{
    int ret;
    ret = encrypt_internal( 0x25, data, data_size, buf, buf_size, out_size );
    //if ( ERR_SUCCEED != ret ) return ret;
    //ret = encrypt_internal( 0x56, buf, *out_size, buf, buf_size, out_size );
    //if ( ERR_SUCCEED != ret ) return ret;
    //ret = encrypt_internal( 0xd1, buf, *out_size, buf, buf_size, out_size );
    return ret;
}
int raw_decrypt( unsigned char const * encrypt_data, int data_size, unsigned char * buf, int buf_size, int * out_size )
{
    int ret;
    ret = decrypt_internal( 0x25, encrypt_data, data_size, buf, buf_size, out_size );
    //if ( ERR_SUCCEED != ret ) return ret;
    //ret = decrypt_internal( 0x56, buf, *out_size, buf, buf_size, out_size );
    //if ( ERR_SUCCEED != ret ) return ret;
    //ret = decrypt_internal( 0xd1, buf, *out_size, buf, buf_size, out_size );
    return ret;
    
}

winux::Buffer EncryptContent( winplus::AnsiString const & content )
{
    winux::Buffer res( (void*)content.c_str(), content.size() );
    int outSize = 0;
    if ( res.getSize() > 0 )
        raw_encrypt( (unsigned char *)content.c_str(), content.size(), (unsigned char *)res.getBuf(), res.getSize(), &outSize );
    return res;
}

winplus::AnsiString DecryptContent( winux::Buffer const & encryptContent )
{
    winplus::AnsiString res( (char const*)encryptContent.getBuf(), encryptContent.getSize() );
    int outSize = 0;
    if ( res.size() > 0 )
        raw_decrypt( (unsigned char *)encryptContent.getBuf(), encryptContent.getSize(), (unsigned char *)&res[0], res.size(), &outSize );
    return res;
}

winplus::String ExplainCustomVars( winplus::String const & str )
{
    winplus::String vars[] = {
        TEXT("$ROOT$"),
        TEXT("$CURRENT$")
    };
    winplus::String vals[] = {
        winplus::ModulePath(),
        winplus::GetCurrentDir()
    };
    winplus::MultiMatch mm( vars, countof(vars), vals, countof(vals) );
    return mm.replace(str);
}

CString GetExecutablePath()
{
    return ( winplus::ModulePath() + winplus::dirSep ).c_str();
}

//Database//////////////////////////////////////////////////////////////////////////

bool RegisterUser( eiendb::Database & db, User const & newUser )
{
    winux::Mixed encPassword = EncryptContent( (LPCTSTR)newUser.m_password );

    winux::Mixed fields;
    fields.addPair()
        ( "name", (LPCTSTR)newUser.m_username )
        ( "pwd", encPassword )
        ( "protect", newUser.m_protectLevel )
        ( "condone", 3 )
        ( "cur_condone", 3 )
        ( "unlock_time", winux::GetUtcTime() )
        ( "hotkey", newUser.m_hotkey )
        ( "time", winux::GetUtcTime() )
        ;

    try
    {
        auto mdf = db.mdf("am_users");
        return mdf->addNew(fields);
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
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
        winux::StringMixedMap userFields;
        if( resUser->fetchRow(&userFields) ) // 找到这个用户
        {
            int nowTime = (int)winux::GetUtcTime();

            tmpUserInfo.m_unlockTime = userFields["unlock_time"];
            tmpUserInfo.m_condone = userFields["condone"];
            tmpUserInfo.m_curCondone = userFields["cur_condone"];

            if ( nowTime < tmpUserInfo.m_unlockTime ) // 当前时刻小于锁定时刻说明处于锁定中
            {
                int hours = ( tmpUserInfo.m_unlockTime - nowTime ) / 3600;
                int minutes = ( ( tmpUserInfo.m_unlockTime - nowTime ) - hours * 3600 ) / 60;
                int seconds = ( tmpUserInfo.m_unlockTime - nowTime ) - hours * 3600 - minutes * 60;
                AfxGetMainWnd()->WarningError( winplus::Format( _T("用户锁定中，解锁还需要%d小时%d分%d秒"), hours, minutes, seconds ).c_str(), _T("错误") );
                ret = false;
                goto ExitProc;
            }
            else
            {
                if ( tmpUserInfo.m_curCondone < 1 ) // 表示这是刚从锁定状态恢复,重置cur_condone=condone
                {
                    tmpUserInfo.m_curCondone = tmpUserInfo.m_condone;
                    ModifyUserEx( db, username, winux::Mixed().addPair()( "cur_condone", tmpUserInfo.m_curCondone ) );
                }
            }

            // 验证密码
            tmpUserInfo.m_password = DecryptContent( userFields["pwd"].toBuffer() ).c_str();
            if ( tmpUserInfo.m_password != password )
            {
                // 减少当前容错次数
                tmpUserInfo.m_curCondone--;
                ModifyUserEx( db, username, winux::Mixed().addPair()( "cur_condone", tmpUserInfo.m_curCondone ) );

                AfxGetMainWnd()->WarningError( winplus::Format( _T("密码不正确，你还剩%d次机会"), tmpUserInfo.m_curCondone ).c_str(), _T("错误") );

                if ( tmpUserInfo.m_curCondone < 1 ) // 没有容错数,锁定用户
                {
                    int lockTime = 3600 * 3;
                    tmpUserInfo.m_unlockTime = winux::GetUtcTime() + lockTime;
                    ModifyUserEx( db, username, winux::Mixed().addPair()( "unlock_time", tmpUserInfo.m_unlockTime ) );
                }
                ret = false;
                goto ExitProc;
            }
            // 登录验证成功,重置cur_condone=condone
            tmpUserInfo.m_curCondone = tmpUserInfo.m_condone;
            ModifyUserEx( db, username, winux::Mixed().addPair()( "cur_condone", tmpUserInfo.m_curCondone ) );

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
        else //没有指定名称的用户
        {
            AfxGetMainWnd()->WarningError( _T("没有此用户"), _T("错误") );
            ret = false;
            goto ExitProc;
        }
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
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
        winux::StringMixedMap userFields;
        if( resUser->fetchRow(&userFields) ) // 找到这个用户
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
            //AfxGetMainWnd()->WarningError( _T("没有此用户"), _T("错误") );
            ret = false;
        }
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") ); //*/
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
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") ); //*/
    }
    return false;
}

bool VerifyUserPassword( eiendb::Database & db, CString const & username, CString const & password )
{
    try
    {
        winux::Buffer encPassword = EncryptContent( (LPCTSTR)password );
        auto res = db->query( db->buildStmt("SELECT pwd = ? FROM am_users WHERE name = ?;", winux::Mixed().add()(encPassword)((LPCTSTR)username) ) );
        winux::MixedArray f;
        if ( res->fetchRow(&f) )
        {
            return f[0].toBool();
        }
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") ); //*/
    }
    return false;
}

bool ModifyUserEx( eiendb::Database & db, CString const & username, winux::Mixed const & userFields )
{
    try
    {
        auto mdf = db.mdf("am_users");
        return mdf->modifyEx( userFields, "name=" + db->escape( (LPCTSTR)username ) );
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
    }
    return false;
}

// struct AccountType ---------------------------------------------------------------------
void AccountType::assignTo( winux::Mixed * accountTypeMixed, CString const & fieldNames )
{
    winux::StringArray fnames;
    winux::StrSplit( (LPCTSTR)fieldNames, ",", &fnames );
    if ( !accountTypeMixed->isCollection() )
        accountTypeMixed->createCollection();
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

void AccountType::assign( winux::Mixed const & accountTypeMixed )
{
    int n = accountTypeMixed.getCount();
    for ( int i = 0; i < n; ++i )
    {
        auto & pr = accountTypeMixed.getPair(i);
        winux::String const & keyname = pr.first.refAnsi();
        if ( keyname == "name" )
        {
            m_typeName = pr.second.refAnsi().c_str();
        }
        else if ( keyname == "rank" )
        {
            m_safeRank = pr.second;
        }
    }
}

int LoadAccountTypes( eiendb::Database & db, AccountTypeArray * types )
{
    int count = 0;
    IfPTR(types)->RemoveAll();
    try
    {
        auto resAccountTypes = db->query("SELECT * FROM am_account_types ORDER BY rank;");
        winux::MixedArray f;
        while ( resAccountTypes->fetchRow(&f) )
        {
            AccountType t;
            t.m_typeName = f[0].toAnsi().c_str();
            t.m_safeRank = f[1];
            IfPTR(types)->Add(t);
            count++;
        }
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
    }
    return count;
}

bool GetAccountType( eiendb::Database & db, CString const & typeName, AccountType * type )
{
    bool ret = false;
    try
    {
        auto resAccountType = db->query( db->buildStmt( "SELECT * FROM am_account_types WHERE name = ?;", (LPCTSTR)typeName ) );
        winux::MixedArray f;
        if ( resAccountType->fetchRow(&f) )
        {
            IfPTR(type)->m_typeName = f[0].toAnsi().c_str();
            IfPTR(type)->m_safeRank = f[1];
            ret = true;
        }
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
    }
    return ret;
}

bool AddAccountType( eiendb::Database & db, winux::Mixed const & newType )
{
    winux::Mixed newAccountType = newType;
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
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
    }
    return false;
}

bool ModifyAccountType( eiendb::Database & db, CString const & typeName, winux::Mixed const & newTypeFields )
{
    try
    {
        auto mdf = db.mdf("am_account_types");
        return mdf->modifyEx( newTypeFields, "name=" + db->escape( (LPCTSTR)typeName ) );
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
    }
    return false;
}

bool DeleteAccountType( eiendb::Database & db, CString const & typeName )
{
    try
    {
        auto resCountAccountCates = db->query( db->buildStmt( "SELECT COUNT(*) > 0 FROM am_account_cates WHERE type = ?;", (LPCTSTR)typeName ) );
        winux::MixedArray f;
        if ( resCountAccountCates->fetchRow(&f) && f[0].toInt() > 0 )
        {
            // 不能删除此类型
            AfxGetMainWnd()->WarningError( _T("该类型下关联有账户种类，因此不能删除"), _T("错误") );
            return false;
        }

        auto mdf = db.mdf("am_account_types");
        return mdf->deleteEx( "name=" + db->escape( (LPCTSTR)typeName ) );
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") ); //*/
    }
    return false;
}

// struct AccountCate ---------------------------------------------------------------------
void AccountCate::assignTo( winux::Mixed * accountCateMixed, CString const & fieldNames )
{
    winux::StringArray fnames;
    winux::StrSplit( (LPCTSTR)fieldNames, ",", &fnames );
    if ( !accountCateMixed->isCollection() )
        accountCateMixed->createCollection();
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

void AccountCate::assign( winux::Mixed const & accountCateMixed )
{
    int n = accountCateMixed.getCount();
    for ( int i = 0; i < n; ++i )
    {
        auto & pr = accountCateMixed.getPair(i);
        winux::String const & keyname = pr.first.refAnsi();
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
}

int LoadAccountCates( eiendb::Database & db, AccountCateArray * cates )
{
    int count = 0;
    IfPTR(cates)->RemoveAll();
    try
    {
        auto resAccountCates = db->query("SELECT * FROM am_account_cates ORDER BY id;");
        winux::StringMixedMap fields;
        while ( resAccountCates->fetchRow(&fields) )
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
            IfPTR(cates)->Add(cate);

            count++;
        }
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") ); //*/
    }
    return count;
}

bool GetAccountCate( eiendb::Database & db, int id, AccountCate * cate )
{
    bool ret = false;
    try
    {
        auto resAccountCate = db->query( db->buildStmt( "SELECT * FROM am_account_cates WHERE id = ?;", id ) );
        winux::StringMixedMap f;
        if ( resAccountCate->fetchRow(&f) )
        {
            IfPTR(cate)->m_id = f["id"];
            IfPTR(cate)->m_cateName = f["name"].toAnsi().c_str();
            IfPTR(cate)->m_cateDesc = f["desc"].toAnsi().c_str();
            IfPTR(cate)->m_typeName = f["type"].toAnsi().c_str();
            IfPTR(cate)->m_url = f["url"].toAnsi().c_str();
            IfPTR(cate)->m_icoPath = f["icon"].toAnsi().c_str();
            IfPTR(cate)->m_startup = f["startup"].toAnsi().c_str();
            IfPTR(cate)->m_keywords = f["keywords"].toAnsi().c_str();
            IfPTR(cate)->m_timeWriten = f["time"];
            ret = true;
        }
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
    }
    return ret;
}

int AddAccountCate( eiendb::Database & db, winux::Mixed const & newCate )
{
    winux::Mixed newAccountCate = newCate;
    newAccountCate.addPair()
        ( "time", (int)winplus::GetUtcTime() )
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
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
    }
    return 0;
}

bool ModifyAccountCate( eiendb::Database & db, int id, winux::Mixed const & newCateFields )
{
    try
    {
        auto mdf = db.mdf("am_account_cates");
        return mdf->modify( newCateFields, id );
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
    }
    return false;
}

bool DeleteAccountCate( eiendb::Database & db, int id )
{
    try
    {
        auto resCountAccounts = db->query( db->buildStmt( "SELECT COUNT(*) > 0 FROM am_accounts WHERE cate = ?;", id ) );
        winux::MixedArray f;
        if ( resCountAccounts->fetchRow(&f) && f[0].toInt() > 0 )
        {
            // 不能删除此类型
            AfxGetMainWnd()->WarningError( _T("该种类下关联有账户，因此不能删除"), _T("错误") );
            return false;
        }
        auto mdf = db.mdf("am_account_cates");
        return mdf->deleteOne(id);
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
    }
    return false;
}

int LoadAccountCatesSafeRank( eiendb::Database & db, CUIntArray * cateIds, CUIntArray * typeSafeRanks )
{
    int count = 0;
    IfPTR(cateIds)->RemoveAll();
    IfPTR(typeSafeRanks)->RemoveAll();
    try
    {
        auto resAccountCatesSafeRank = db->query("SELECT c.id, t.rank from am_account_cates AS c Left JOIN am_account_types AS t ON t.name = c.type;");
        winux::MixedArray f;
        while ( resAccountCatesSafeRank->fetchRow(&f) )
        {
            IfPTR(cateIds)->Add( f[0] );
            IfPTR(typeSafeRanks)->Add( f[1] );

            count++;
        }
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
    }

    return count;

}

bool GetTypeByCateId( eiendb::Database & db, int cateId, AccountType * type )
{
    bool ret = false;
    try
    {
        auto resAccountType = db->query( db->buildStmt( "SELECT c.id, t.name, t.rank FROM am_account_cates AS c Left JOIN am_account_types AS t ON t.name = c.type WHERE c.id = ?;", cateId ) );
        winux::MixedArray f;
        if ( resAccountType->fetchRow(&f) )
        {
            ASSERT( cateId == f[0].toInt() );
            IfPTR(type)->m_typeName = f[1].toAnsi().c_str();
            IfPTR(type)->m_safeRank = f[2];
            ret = true;
        }
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
    }
    return ret;
}

// struct Account -------------------------------------------------------------------------
void Account::assignTo( winux::Mixed * accountMixed, CString const & fieldNames )
{
    winux::StringArray fnames;
    winux::StrSplit( (LPCTSTR)fieldNames, ",", &fnames );

    if ( !accountMixed->isCollection() )
        accountMixed->createCollection();
    for ( auto it = fnames.begin(); it != fnames.end(); ++it )
    {
        if ( *it == "myname" )
        {
            (*accountMixed)[*it] = (LPCTSTR)m_myName;
        }
        else if ( *it == "account_name" )
        {
            (*accountMixed)[*it] = EncryptContent( winux::LocalToUtf8( (LPCTSTR)m_accountName ) );
        }
        else if ( *it == "account_pwd" )
        {
            (*accountMixed)[*it] = EncryptContent( winux::LocalToUtf8( (LPCTSTR)m_accountPwd ) );
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

void Account::assign( winux::Mixed const & accountMixed )
{
    int n = accountMixed.getCount();
    for ( int i = 0; i < n; ++i )
    {
        auto & pr = accountMixed.getPair(i);
        winux::String const & keyname = pr.first.refAnsi();
        if ( keyname == "myname" )
        {
            m_myName = pr.second.refAnsi().c_str();
        }
        else if ( keyname == "account_name" )
        {
            m_accountName = winux::LocalFromUtf8( DecryptContent( pr.second.toBuffer() ) ).c_str();
        }
        else if ( keyname == "account_pwd" )
        {
            m_accountPwd = winux::LocalFromUtf8( DecryptContent( pr.second.toBuffer() ) ).c_str();
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

}

int LoadAccounts( eiendb::Database & db, int userId, AccountArray * accounts, int cateId )
{
    int count = 0;
    IfPTR(accounts)->RemoveAll();
    try
    {
        winux::Mixed params;
        params.add()(userId);
        if ( cateId != -1 )
            params.add(cateId);

        winux::String sql = "SELECT * FROM am_accounts WHERE user = ?";
        sql += ( cateId != -1 ? " AND cate = ?" : "" );
        sql += " ORDER BY cate;";

        auto resAccounts = db->query( db->buildStmt( sql, params ) );
        winux::StringMixedMap f;
        while ( resAccounts->fetchRow(&f) )
        {
            Account account;
            account.assign(f);
/*
            account.m_myName = f["myname"].toAnsi().c_str();
            account.m_accountName = winux::LocalFromUtf8( DecryptContent( f["account_name"].toBuffer() ) ).c_str();
            account.m_accountPwd = winux::LocalFromUtf8( DecryptContent( f["account_pwd"].toBuffer() ) ).c_str();
            account.m_cateId = f["cate"];
            account.m_userId = f["user"];
            account.m_safeRank = f["safe_rank"];
            account.m_comment = f["comment"].toAnsi().c_str();
            account.m_time = f["time"];
*/
            IfPTR(accounts)->Add(account);
            count++;
        }
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
    }
    return count;
}

bool GetAccount( eiendb::Database & db, int userId, CString const & myName, Account * account )
{
    bool ret = false;
    try
    {
        auto resAccount = db->query( db->buildStmt( "SELECT * FROM am_accounts WHERE user = ? AND myname = ?;", winux::Mixed().add()(userId)((LPCTSTR)myName) ) );
        winux::StringMixedMap f;
        if ( resAccount->fetchRow(&f) )
        {
            IfPTR(account)->m_myName = f["myname"].toAnsi().c_str();
            IfPTR(account)->m_accountName = winux::LocalFromUtf8( DecryptContent( f["account_name"].toBuffer() ) ).c_str();
            IfPTR(account)->m_accountPwd = winux::LocalFromUtf8( DecryptContent( f["account_pwd"].toBuffer() ) ).c_str();
            IfPTR(account)->m_cateId = f["cate"];
            IfPTR(account)->m_userId = f["user"];
            IfPTR(account)->m_safeRank = f["safe_rank"];
            IfPTR(account)->m_comment = f["comment"].toAnsi().c_str();
            IfPTR(account)->m_time = f["time"];
            ret = true;
        }
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
    }
    return ret;
}

bool AddAccount( eiendb::Database & db, winux::Mixed const & newAccount )
{
    winux::Mixed newAccountMixed = newAccount;
    newAccountMixed.addPair()
        //( "myname", (LPCTSTR)newAccount.m_myName )
        //( "account_name", EncryptContent( winux::LocalToUtf8((LPCTSTR)newAccount.m_accountName) ) )
        //( "account_pwd", EncryptContent( winux::LocalToUtf8((LPCTSTR)newAccount.m_accountPwd) ) )
        //( "cate", newAccount.m_cateId )
        //( "user", newAccount.m_userId )
        //( "safe_rank", newAccount.m_safeRank )
        //( "comment", (LPCTSTR)newAccount.m_comment )
        ( "time", (int)winux::GetUtcTime() )
        ;
    try
    {
        auto mdf = db.mdf("am_accounts");
        return mdf->addNew(newAccountMixed);
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
    }
    return false;
}

bool ModifyAccount( eiendb::Database & db, int userId, CString const & myName, winux::Mixed const & newAccountFields )
{
    try
    {
        auto mdf = db.mdf("am_accounts");
        return mdf->modifyEx( newAccountFields, "user=" + winux::Mixed(userId).toAnsi() + " AND myname=" + db->escape( (LPCTSTR)myName ) );
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
    }
    return false;
}

bool DeleteAccount( eiendb::Database & db, int userId, CString const & myName )
{
    try
    {
        auto mdf = db.mdf("am_accounts");
        return mdf->deleteEx( "user=" + winux::Mixed(userId).toAnsi() + " AND myname=" + db->escape( (LPCTSTR)myName ) );
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
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
        auto stmt = db->buildStmt( "SELECT myname FROM am_accounts WHERE user = ? AND myname = ?;", winux::Mixed().add()(userId)((LPCTSTR)result) );
        auto resAccount = db->query(stmt);
        while ( resAccount->rowsCount() > 0 )
        {
            number++;
            result.Format( "%s%d", (LPCTSTR)myName, number );

            stmt = db->buildStmt( "SELECT myname FROM am_accounts WHERE user = ? AND myname = ?;", winux::Mixed().add()(userId)((LPCTSTR)result) );
            resAccount = db->query(stmt);
        }
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
    }
    return result;
}

int LoadTableNames( eiendb::Database & db, winplus::StringArray * tableNames, winplus::String const & like )
{
    int count = 0;
    IfPTR(tableNames)->clear();
    try
    {
        auto resTableNames = db->query( db->buildStmt( "SELECT tbl_name FROM sqlite_master WHERE type = \'table\' AND tbl_name LIKE ? ESCAPE \'\\\';", like ) );
        winux::MixedArray f;
        while ( resTableNames->fetchRow(&f) )
        {
            IfPTR(tableNames)->push_back(f[0]);
            count++;
        }
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
    }
    return count;
}

int DumpDDL( eiendb::Database & db, winplus::String * ddl, winplus::String const & like )
{
    int count = 0;
    try
    {
        auto resDdlSqls = db->query( db->buildStmt( "SELECT sql FROM sqlite_master WHERE tbl_name LIKE ? ESCAPE \'\\\' AND NOT sql IS Null ORDER BY tbl_name;", like ) );
        winux::MixedArray f;
        while ( resDdlSqls->fetchRow(&f) )
        {
            AssignPTR(ddl) += f[0].toAnsi() + _T(";") + winux::LineSep;
            count++;
        }
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
    }
    return count;
}

bool IsBrowserExeName( eiendb::Database & db, CString const & exeName, CString * browserTitle )
{
    bool ret = false;
    try
    {
        auto resIsBrowser = db->query( db->buildStmt( "SELECT title FROM am_browsers WHERE exe_name = ?;", (LPCTSTR)exeName ) );
        winux::MixedArray f;
        if ( resIsBrowser->fetchRow(&f) )
        {
            AssignPTR(browserTitle) = f[0].toAnsi().c_str();
            ret = true;
        }
    }
    catch ( winux::Error const & e )
    {
        AfxGetMainWnd()->FatalError( e.what(), _T("数据库错误") );
    }
    return ret;
}
