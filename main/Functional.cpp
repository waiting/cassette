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

winplus::AnsiString EncryptContent( winplus::AnsiString const & content )
{
    winplus::AnsiString res(content);
    int outSize = 0;
    if ( res.size() > 0 )
        raw_encrypt( (unsigned char *)content.c_str(), content.size(), (unsigned char *)&res[0], res.size(), &outSize );
    return res;
}

winplus::AnsiString DecryptContent( winplus::AnsiString const & encryptContent )
{
    winplus::AnsiString res(encryptContent);
    int outSize = 0;
    if ( res.size() > 0 )
        raw_decrypt( (unsigned char *)encryptContent.c_str(), encryptContent.size(), (unsigned char *)&res[0], res.size(), &outSize );
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
    int rowsChanged = 0;

    auto mdf_am_users = db.mdf("am_users");

    winux::Mixed fields;
    fields.createCollection();

    winux::AnsiString encPassword = EncryptContent( (LPCSTR)newUser.m_password );

    fields.addPair()
        ( "name", (LPCSTR)newUser.m_username )
        ( "pwd", (LPCSTR)newUser.m_password )
        ;

    mdf_am_users->addNew(fields);

    winplus::AnsiString sql = winplus::StringToUtf8("INSERT INTO am_users( name, pwd, protect, condone, cur_condone, unlock_time, hotkey, time ) VALUES( ?, ?, ?, ?, ?, ?, ?, ? );");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;

    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 绑定参数
    // 用户名
    rc = newUser.bindUsername( stmt, 1 );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 密码
    rc = newUser.bindPassword( stmt, 2 );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 保护级别
    rc = newUser.bindProtectLevel( stmt, 3 );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 容错数
    rc = newUser._bindInt( stmt, 4, 3 );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 当前剩余容错数
    rc = newUser._bindInt( stmt, 5, 3 );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 解锁时刻
    rc = newUser._bindInt( stmt, 6, (int)winplus::GetUtcTime() );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 热键
    rc = newUser.bindHotkey( stmt, 7 );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 注册时刻
    rc = newUser._bindInt( stmt, 8, (int)winplus::GetUtcTime() );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    rc = sqlite3_step(stmt);
    if ( rc == SQLITE_DONE )
    {
        rowsChanged = sqlite3_changes(db);
        rc = sqlite3_reset(stmt);
        if ( rc != SQLITE_OK )
        {
            goto OccurDbError;
        }
    }
    else
    {
        rc = sqlite3_reset(stmt);
        goto OccurDbError;
    }

    // 正常无错误结束
    if ( stmt )
        sqlite3_finalize(stmt);
    return rowsChanged == 1;

OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return false;
}

bool LoginUser( sqlite3 * db, CString const & username, CString const & password, User * userData )
{
    User tmpUserInfo;

    bool ret = false;

    winplus::AnsiString sql = winplus::StringToUtf8("SELECT * FROM am_users WHERE name = ?;");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;

    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 绑定参数
    // 用户名
    rc = Fields::_bindString( stmt, 1, username );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    rc = sqlite3_step(stmt);
    if ( rc == SQLITE_ROW )
    {
        // 判断用户是否处于锁定时间中
        tmpUserInfo.loadUnlockTime( stmt, FieldIndex(am_users__unlock_time) );
        int nowTime = (int)winplus::GetUtcTime();

        tmpUserInfo.loadCondone( stmt, FieldIndex(am_users__condone) );
        tmpUserInfo.loadCurCondone( stmt, FieldIndex(am_users__cur_condone) );

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
                ModifyUserEx( db, username, am_users__cur_condone, tmpUserInfo );
            }
        }

        // 验证密码
        tmpUserInfo.loadPassword( stmt, FieldIndex(am_users__pwd) );
        if ( tmpUserInfo.m_password != password )
        {
            // 减少当前容错次数
            tmpUserInfo.m_curCondone--;
            ModifyUserEx( db, username, am_users__cur_condone, tmpUserInfo );

            AfxGetMainWnd()->WarningError( winplus::Format( _T("密码不正确，你还剩%d次机会"), tmpUserInfo.m_curCondone ).c_str(), _T("错误") );

            if ( tmpUserInfo.m_curCondone < 1 ) // 没有容错数,锁定用户
            {
                int lockTime = 3600 * 3;
                tmpUserInfo.m_unlockTime = winplus::GetUtcTime() + lockTime;
                ModifyUserEx( db, username, am_users__unlock_time, tmpUserInfo );
            }
            ret = false;
            goto ExitProc;
        }
        // 登录验证成功,重置cur_condone=condone
        tmpUserInfo.m_curCondone = tmpUserInfo.m_condone;
        ModifyUserEx( db, username, am_users__cur_condone, tmpUserInfo );

        if ( userData != NULL )
        {
            userData->loadId( stmt, 0 );
            userData->loadUsername( stmt, 1 );
            userData->loadProtectLevel( stmt, 3 );
            userData->loadCondone( stmt, 4 );
            userData->loadCurCondone( stmt, 5 );
            userData->loadUnlockTime( stmt, 6 );
            userData->loadHotkey( stmt, 7 );
            userData->loadRegTime( stmt, 8 );
        }
        ret = true;
        goto ExitProc;
    }
    else if ( rc == SQLITE_DONE ) // 没有找到用户
    {
        AfxGetMainWnd()->WarningError( _T("没有此用户"), _T("错误") );
        ret = false;
        goto ExitProc;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }

ExitProc:
    // 无硬性错误的结束
    sqlite3_reset(stmt);
    if ( stmt )
        sqlite3_finalize(stmt);
    return ret;

OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return false;
}

bool LoadUser( sqlite3 * db, CString const & username, User * userData )
{
    bool ret = false;

    winplus::AnsiString sql = winplus::StringToUtf8("SELECT * FROM am_users WHERE name = ?;");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;
    
    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 绑定参数
    // 用户名
    rc = Fields::_bindString( stmt, 1, username );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    rc = sqlite3_step(stmt);
    if ( rc == SQLITE_ROW )
    {
        if ( userData != NULL )
        {
            userData->loadId( stmt, 0 );
            userData->loadUsername( stmt, 1 );
            userData->loadProtectLevel( stmt, 3 );
            userData->loadCondone( stmt, 4 );
            userData->loadCurCondone( stmt, 5 );
            userData->loadUnlockTime( stmt, 6 );
            userData->loadHotkey( stmt, 7 );
            userData->loadRegTime( stmt, 8 );
        }
        ret = true;
        goto ExitProc;
    }
    else if ( rc == SQLITE_DONE ) // 没有找到用户
    {
        ret = false;
        goto ExitProc;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }
    
ExitProc:
    // 正常无错误结束
    sqlite3_reset(stmt);
    if ( stmt )
        sqlite3_finalize(stmt);
    return ret;

OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return false;
}

bool DeleteUser( sqlite3 * db, CString const & username )
{
    int rowsChanged = 0;

    winplus::AnsiString sql = winplus::StringToUtf8("DELETE FROM am_users WHERE name = ?;");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;

    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 绑定参数
    // 用户名
    rc = Fields::_bindString( stmt, 1, username );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    rc = sqlite3_step(stmt);
    if ( rc == SQLITE_DONE )
    {
        rowsChanged = sqlite3_changes(db);
        rc = sqlite3_reset(stmt);
        if ( rc != SQLITE_OK )
        {
            goto OccurDbError;
        }
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }
    
    // 正常无错误结束
    if ( stmt )
        sqlite3_finalize(stmt);
    return rowsChanged == 1;
    
OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return false;
}

bool VerifyUserPassword( sqlite3 * db, CString const & username, CString const & password )
{
    winplus::AnsiString encryptPassword;
    bool ret = false;

    winplus::AnsiString sql = winplus::StringToUtf8("SELECT pwd = ? FROM am_users WHERE name = ?;");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;

    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 绑定参数
    // 用户名
    rc = Fields::_bindString( stmt, 2, username );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 密码
    encryptPassword = EncryptContent( winplus::StringToUtf8( (LPCTSTR)password ) );
    rc = Fields::_bindBlob( stmt, 1, encryptPassword.c_str(), encryptPassword.size() );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    rc = sqlite3_step(stmt);
    if ( rc == SQLITE_ROW )
    {
        ret = Fields::_getInt( stmt, 0 ) != 0;
        goto ExitProc;
    }
    else if ( rc == SQLITE_DONE ) // 没有找到合适的用户
    {
        ret = false;
        goto ExitProc;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }
    
ExitProc:
    // 正常无错误结束
    sqlite3_reset(stmt);
    if ( stmt )
        sqlite3_finalize(stmt);
    return ret;
    
OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return false;
}

bool ModifyUserEx( sqlite3 * db, CString const & username, UINT modifiedFieldBits, User const & newUser )
{
    int rowsChanged = 0;
    int paramIndex = 0;
    // 生成SQL
    winplus::AnsiStringArray modifiedFields;
    if ( modifiedFieldBits & am_users__id )
        modifiedFields.push_back("id = ?");
    if ( modifiedFieldBits & am_users__name )
        modifiedFields.push_back("name = ?");
    if ( modifiedFieldBits & am_users__pwd )
        modifiedFields.push_back("pwd = ?");
    if ( modifiedFieldBits & am_users__protect )
        modifiedFields.push_back("protect = ?");
    if ( modifiedFieldBits & am_users__condone )
        modifiedFields.push_back("condone = ?");
    if ( modifiedFieldBits & am_users__cur_condone )
        modifiedFields.push_back("cur_condone = ?");
    if ( modifiedFieldBits & am_users__unlock_time )
        modifiedFields.push_back("unlock_time = ?");
    if ( modifiedFieldBits & am_users__hotkey )
        modifiedFields.push_back("hotkey = ?");
    if ( modifiedFieldBits & am_users__time )
        modifiedFields.push_back("time = ?");

    winplus::AnsiString sql = winplus::StringToUtf8(
        "UPDATE am_users SET " + winplus::StrJoin( ", ", modifiedFields ) + " WHERE name = ?;"
    );
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;

    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 绑定参数

    // 新ID
    if ( modifiedFieldBits & am_users__id )
    {
        rc = newUser.bindId( stmt, ++paramIndex );
        if ( rc != SQLITE_OK ) goto OccurDbError;
    }

    // 新用户名
    if ( modifiedFieldBits & am_users__name )
    {
        rc = newUser.bindUsername( stmt, ++paramIndex );
        if ( rc != SQLITE_OK ) goto OccurDbError;
    }
    
    // 新密码
    if ( modifiedFieldBits & am_users__pwd )
    {
        rc = newUser.bindPassword( stmt, ++paramIndex );
        if ( rc != SQLITE_OK ) goto OccurDbError;
    }

    // 新保护级别
    if ( modifiedFieldBits & am_users__protect )
    {
        rc = newUser.bindProtectLevel( stmt, ++paramIndex );
        if ( rc != SQLITE_OK ) goto OccurDbError;
    }
    
    // 新容错数
    if ( modifiedFieldBits & am_users__condone )
    {
        rc = newUser.bindCondone( stmt, ++paramIndex );
        if ( rc != SQLITE_OK ) goto OccurDbError;
    }
    
    // 新当前容错数
    if ( modifiedFieldBits & am_users__cur_condone )
    {
        rc = newUser.bindCurCondone( stmt, ++paramIndex );
        if ( rc != SQLITE_OK ) goto OccurDbError;
    }

    // 新解锁时刻
    if ( modifiedFieldBits & am_users__unlock_time )
    {
        rc = newUser.bindUnlockTime( stmt, ++paramIndex );
        if ( rc != SQLITE_OK ) goto OccurDbError;
    }

    // 新热键
    if ( modifiedFieldBits & am_users__hotkey )
    {
        rc = newUser.bindHotkey( stmt, ++paramIndex );
        if ( rc != SQLITE_OK ) goto OccurDbError;
    }

    // 新录入时刻
    if ( modifiedFieldBits & am_users__time )
    {
        rc = newUser.bindRegTime( stmt, ++paramIndex );
        if ( rc != SQLITE_OK ) goto OccurDbError;
    }

    // where子句
    // 用户名
    rc = Fields::_bindString( stmt, ++paramIndex, username );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    rc = sqlite3_step(stmt);
    if ( rc == SQLITE_DONE )
    {
        rowsChanged = sqlite3_changes(db);
        rc = sqlite3_reset(stmt);
        if ( rc != SQLITE_OK )
        {
            goto OccurDbError;
        }
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }

    // 无硬性错误的结束
    if ( stmt )
        sqlite3_finalize(stmt);
    return rowsChanged == 1;
    
OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return false;
}

int LoadAccountTypes( sqlite3 * db, AccountTypeArray * types )
{
    int count = 0;
    IfPTR(types)->RemoveAll();

    winplus::AnsiString sql = winplus::StringToUtf8("SELECT * FROM am_account_types ORDER BY rank;");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;
    
    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    while ( ( rc = sqlite3_step(stmt) ) == SQLITE_ROW )
    {
        AccountType t;
        t.loadTypeName( stmt, 0 );
        t.loadSafeRank( stmt, 1 );
        IfPTR(types)->Add(t);
        count++;
    }
    
    if ( rc == SQLITE_DONE ) // 数据已完或没有数据
    {
        goto ExitProc;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }
    
ExitProc:
    // 正常无错误结束
    sqlite3_reset(stmt);
    if ( stmt )
        sqlite3_finalize(stmt);
    return count;

OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return 0;
}

bool GetAccountType( sqlite3 * db, CString const & typeName, AccountType * type )
{
    bool ret = false;

    winplus::AnsiString sql = winplus::StringToUtf8("SELECT * FROM am_account_types WHERE name = ?;");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;
    
    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 绑定参数
    // 类型名
    rc = Fields::_bindString( stmt, 1, typeName );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    rc = sqlite3_step(stmt);
    if ( rc == SQLITE_ROW )
    {
        IfPTR(type)->loadTypeName( stmt, 0 );
        IfPTR(type)->loadSafeRank( stmt, 1 );
        ret = true;
        goto ExitProc;
    }
    else if ( rc == SQLITE_DONE )
    {
        ret = false;
        goto ExitProc;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }
    
ExitProc:
    // 正常无错误结束
    sqlite3_reset(stmt);
    if ( stmt )
        sqlite3_finalize(stmt);
    return ret;
    
OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return false;
}

bool AddAccountType( sqlite3 * db, AccountType const & newType )
{
    int rowsChanged = 0;

    winplus::AnsiString sql = winplus::StringToUtf8("INSERT INTO am_account_types( name, rank ) VALUES( ?, ? );");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;
    
    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 绑定参数
    // 类型名
    rc = newType.bindTypeName( stmt, 1 );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    
    // 安全值
    rc = newType.bindSafeRank( stmt, 2 );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    
    // 执行语句
    rc = sqlite3_step(stmt);
    if ( rc == SQLITE_DONE )
    {
        rowsChanged = sqlite3_changes(db);
        rc = sqlite3_reset(stmt);
        if ( rc != SQLITE_OK )
        {
            goto OccurDbError;
        }
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }
    
    // 正常无错误结束
    if ( stmt )
        sqlite3_finalize(stmt);
    return rowsChanged == 1;
    
OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return false;
}

bool ModifyAccountType( sqlite3 * db, CString const & typeName, AccountType const & newType )
{
    int rowsChanged = 0;

    winplus::AnsiString sql = winplus::StringToUtf8("UPDATE am_account_types SET name = ?, rank = ? WHERE name = ?;");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;
    
    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    
    // 绑定参数
    // 新类型名
    rc = newType.bindTypeName( stmt, 1 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 新安全值
    rc = newType.bindSafeRank( stmt, 2 );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    
    // 类型名
    rc = Fields::_bindString( stmt, 3, typeName );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    rc = sqlite3_step(stmt);
    if ( rc == SQLITE_DONE )
    {
        rowsChanged = sqlite3_changes(db);
        rc = sqlite3_reset(stmt);
        if ( rc != SQLITE_OK )
        {
            goto OccurDbError;
        }
    }
    else
    {
        rc = sqlite3_reset(stmt);
        goto OccurDbError;
    }
    
    // 正常无错误结束
    if ( stmt )
        sqlite3_finalize(stmt);
    return rowsChanged == 1;
    
OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return false;
}

bool DeleteAccountType( sqlite3 * db, CString const & typeName )
{
    int rowsChanged = 0;

    winplus::AnsiString sql1, sql2;
    sqlite3_stmt * stmt1 = NULL;
    sqlite3_stmt * stmt2 = NULL;
    int rc;
    const char * localError;

    // 准备语句
    sql1 = winplus::StringToUtf8("SELECT COUNT(*) > 0 FROM am_account_cates WHERE type = ?;");
    rc = sqlite3_prepare_v2( db, sql1.c_str(), sql1.size(), &stmt1, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 绑定参数
    // 类型名
    rc = Fields::_bindString( stmt1, 1, typeName );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句,判断是否执行删除类型操作
    rc = sqlite3_step(stmt1);
    if ( rc != SQLITE_ROW ) goto OccurDbError;

    if ( Fields::_getInt( stmt1, 0 ) )
    {
        // 不能删除此类型
        AfxGetMainWnd()->WarningError( _T("该类型下关联有账户种类，因此不能删除"), _T("错误") );
        goto ExitProc;
    }

    //////////////////////////////////////////////////////////////////////////
    // 准备语句
    sql2 = winplus::StringToUtf8("DELETE FROM am_account_types WHERE name = ?;");
    rc = sqlite3_prepare_v2( db, sql2.c_str(), sql2.size(), &stmt2, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 绑定参数
    // 类型名
    rc = Fields::_bindString( stmt2, 1, typeName );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    rc = sqlite3_step(stmt2);
    if ( rc == SQLITE_DONE )
    {
        rowsChanged = sqlite3_changes(db);
        rc = sqlite3_reset(stmt2);
        if ( rc != SQLITE_OK ) goto OccurDbError;
    }
    else
    {
        sqlite3_reset(stmt2);
        goto OccurDbError;
    }

ExitProc:
    // 无硬性错误的结束
    if ( stmt1 )
        sqlite3_finalize(stmt1);
    if ( stmt2 )
        sqlite3_finalize(stmt2);
    return rowsChanged == 1;

OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt1 )
        sqlite3_finalize(stmt1);
    if ( stmt2 )
        sqlite3_finalize(stmt2);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return false;
}

int LoadAccountCates( sqlite3 * db, AccountCateArray * cates )
{
    IfPTR(cates)->RemoveAll();

    int count = 0;

    winplus::AnsiString sql = winplus::StringToUtf8("SELECT * FROM am_account_cates ORDER BY id;");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;
    
    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    
    // 执行语句
    while ( ( rc = sqlite3_step(stmt) ) == SQLITE_ROW )
    {
        AccountCate cate;
        cate.loadId( stmt, 0 );//Add( sqlite3_column_int( stmt, 0 ) );
        cate.loadCateName( stmt, 1 );//Add( winplus::Utf8ToString( (char const *)sqlite3_column_text( stmt, 1 ) ).c_str() );
        cate.loadCateDesc( stmt, 2 );//Add( winplus::Utf8ToString( (char const *)sqlite3_column_text( stmt, 2 ) ).c_str() );
        cate.loadTypeName( stmt, 3 );//Add( winplus::Utf8ToString( (char const *)sqlite3_column_text( stmt, 3 ) ).c_str() );
        cate.loadUrl( stmt, 4 );//Add( winplus::Utf8ToString( (char const *)sqlite3_column_text( stmt, 4 ) ).c_str() );
        cate.loadIcoPath( stmt, 5 );//Add( winplus::Utf8ToString( (char const *)sqlite3_column_text( stmt, 5 ) ).c_str() );
        cate.loadStartup( stmt, 6 );//Add( winplus::Utf8ToString( (char const *)sqlite3_column_text( stmt, 6 ) ).c_str() );
        cate.loadKeywords( stmt, 7 );//Add( winplus::Utf8ToString( (char const *)sqlite3_column_text( stmt, 7 ) ).c_str() );
        cate.loadTimeWriten( stmt, 8 );//Add( sqlite3_column_int( stmt, 8 ) );
        IfPTR(cates)->Add(cate);

        count++;
    }
    
    if ( rc == SQLITE_DONE ) // 数据已完或没有数据
    {
        goto ExitProc;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }
    
ExitProc:
    // 正常无错误结束
    sqlite3_reset(stmt);
    if ( stmt )
        sqlite3_finalize(stmt);
    return count;
    
OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return 0;
}

bool GetAccountCate( sqlite3 * db, int id, AccountCate * cate )
{
    bool ret = false;

    winplus::AnsiString sql = winplus::StringToUtf8("SELECT * FROM am_account_cates WHERE id = ?;");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;
    
    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 绑定参数
    // cateId
    rc = Fields::_bindInt( stmt, 1, id );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    
    // 执行语句
    rc = sqlite3_step(stmt);
    if ( rc == SQLITE_ROW )
    {
        IfPTR(cate)->loadId( stmt, 0 );
        IfPTR(cate)->loadCateName( stmt, 1 );
        IfPTR(cate)->loadCateDesc( stmt, 2 );
        IfPTR(cate)->loadTypeName( stmt, 3 );
        IfPTR(cate)->loadUrl( stmt, 4 );
        IfPTR(cate)->loadIcoPath( stmt, 5 );
        IfPTR(cate)->loadStartup( stmt, 6 );
        IfPTR(cate)->loadKeywords( stmt, 7 );
        IfPTR(cate)->loadTimeWriten( stmt, 8 );
        ret = true;
        goto ExitProc;
    }
    else if ( rc == SQLITE_DONE )
    {
        ret = false;
        goto ExitProc;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }
    
ExitProc:
    // 正常无错误结束
    sqlite3_reset(stmt);
    if ( stmt )
        sqlite3_finalize(stmt);
    return ret;
    
OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return false;
}

int AddAccountCate( sqlite3 * db, AccountCate const & newCate )
{
    int rowsChanged = 0;

    winplus::AnsiString sql = winplus::StringToUtf8(
        "INSERT INTO am_account_cates( "
        "name, desc, type, url, icon, startup, keywords, time"
        " ) VALUES( ?, ?, ?, ?, ?, ?, ?, ? );"
    );
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;
    
    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 绑定参数
    // 种类名
    rc = newCate.bindCateName( stmt, 1 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 种类描述
    rc = newCate.bindCateDesc( stmt, 2 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 类型名
    rc = newCate.bindTypeName( stmt, 3 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // URL
    rc = newCate.bindUrl( stmt, 4 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 图标路径
    rc = newCate.bindIcoPath( stmt, 5 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 启动方式
    rc = newCate.bindStartup( stmt, 6 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 关键字
    rc = newCate.bindKeywords( stmt, 7 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 写入时间
    rc = Fields::_bindInt( stmt, 8, (int)winplus::GetUtcTime() );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    
    // 执行语句
    rc = sqlite3_step(stmt);
    if ( rc == SQLITE_DONE )
    {
        rowsChanged = sqlite3_changes(db);
        rc = sqlite3_reset(stmt);
        if ( rc != SQLITE_OK ) goto OccurDbError;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }
    
    // 正常无错误结束
    if ( stmt )
        sqlite3_finalize(stmt);
    if ( rowsChanged == 1 )
    {
        return (int)sqlite3_last_insert_rowid(db);
    }
    else
    {
        return 0;
    }
OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return 0;
}

bool ModifyAccountCate( sqlite3 * db, int id, AccountCate const & newCate )
{
    int rowsChanged = 0;

    winplus::AnsiString sql = winplus::StringToUtf8("UPDATE am_account_cates SET name = ?, desc = ?, type = ?, url = ?, icon = ?, startup = ?, keywords = ? WHERE id = ?;");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;

    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 绑定参数
    // 新种类名
    rc = newCate.bindCateName( stmt, 1 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 新种类描述
    rc = newCate.bindCateDesc( stmt, 2 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 新类型名
    rc = newCate.bindTypeName( stmt, 3 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 新URL
    rc = newCate.bindUrl( stmt, 4 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 新图标路径
    rc = newCate.bindIcoPath( stmt, 5 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 新启动方式
    rc = newCate.bindStartup( stmt, 6 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 新关键字
    rc = newCate.bindKeywords( stmt, 7 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // CateId
    rc = Fields::_bindInt( stmt, 8, id );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    rc = sqlite3_step(stmt);
    if ( rc == SQLITE_DONE )
    {
        rowsChanged = sqlite3_changes(db);
        rc = sqlite3_reset(stmt);
        if ( rc != SQLITE_OK ) goto OccurDbError;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }

    // 正常无错误结束
    if ( stmt )
        sqlite3_finalize(stmt);
    return rowsChanged == 1;
    
OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return false;
}

bool DeleteAccountCate( sqlite3 * db, int id )
{
    int rowsChanged = 0;

    winplus::AnsiString sql1, sql2;
    sqlite3_stmt * stmt1 = NULL;
    sqlite3_stmt * stmt2 = NULL;
    int rc;
    const char * localError;

    // 准备语句
    sql1 = winplus::StringToUtf8("SELECT COUNT(*) > 0 FROM am_accounts WHERE cate = ?;");
    rc = sqlite3_prepare_v2( db, sql1.c_str(), sql1.size(), &stmt1, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 绑定参数
    // Cate ID
    rc = Fields::_bindInt( stmt1, 1, id );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句,判断是否执行删除类型操作
    rc = sqlite3_step(stmt1);
    if ( rc != SQLITE_ROW ) goto OccurDbError;

    if ( Fields::_getInt( stmt1, 0 ) )
    {
        // 不能删除此类型
        AfxGetMainWnd()->WarningError( _T("该种类下关联有账户，因此不能删除"), _T("错误") );
        goto ExitProc;
    }

    //////////////////////////////////////////////////////////////////////////
    // 准备语句
    sql2 = winplus::StringToUtf8("DELETE FROM am_account_cates WHERE id = ?;");
    rc = sqlite3_prepare_v2( db, sql2.c_str(), sql2.size(), &stmt2, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    
    // 绑定参数
    // CateId
    rc = Fields::_bindInt( stmt2, 1, id );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    rc = sqlite3_step(stmt2);
    if ( rc == SQLITE_DONE )
    {
        rowsChanged = sqlite3_changes(db);
        rc = sqlite3_reset(stmt2);
        if ( rc != SQLITE_OK ) goto OccurDbError;
    }
    else
    {
        sqlite3_reset(stmt2);
        goto OccurDbError;
    }

ExitProc:
    // 无硬性错误的结束
    if ( stmt1 )
        sqlite3_finalize(stmt1);
    if ( stmt2 )
        sqlite3_finalize(stmt2);
    return rowsChanged == 1;
    
OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt1 )
        sqlite3_finalize(stmt1);
    if ( stmt2 )
        sqlite3_finalize(stmt2);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return false;
}

int LoadAccountCatesSafeRank( sqlite3 * db, CUIntArray * cateIds, CUIntArray * typeSafeRanks )
{
    IfPTR(cateIds)->RemoveAll();
    IfPTR(typeSafeRanks)->RemoveAll();

    int count = 0;

    winplus::AnsiString sql = winplus::StringToUtf8("SELECT c.id, t.rank from am_account_cates AS c Left JOIN am_account_types AS t ON t.name = c.type;");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;
    
    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    
    // 执行语句
    while ( ( rc = sqlite3_step(stmt) ) == SQLITE_ROW )
    {
        IfPTR(cateIds)->Add( sqlite3_column_int( stmt, 0 ) );
        IfPTR(typeSafeRanks)->Add( sqlite3_column_int( stmt, 1 ) );

        count++;
    }
    
    if ( rc == SQLITE_DONE ) // 数据已完或没有数据
    {
        goto ExitProc;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }
    
ExitProc:
    // 正常无错误结束
    sqlite3_reset(stmt);
    if ( stmt )
        sqlite3_finalize(stmt);
    return count;

OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return 0;
}

bool GetTypeByCateId( sqlite3 * db, int cateId, AccountType * type )
{
    bool ret = false;

    winplus::AnsiString sql = winplus::StringToUtf8("SELECT c.id, t.name, t.rank FROM am_account_cates AS c Left JOIN am_account_types AS t ON t.name = c.type WHERE c.id = ?;");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;
    
    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 绑定参数
    // cateId
    rc = Fields::_bindInt( stmt, 1, cateId );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    rc = sqlite3_step(stmt);
    if ( rc == SQLITE_ROW )
    {
        ASSERT( cateId == Fields::_getInt( stmt, 0 ) );
        IfPTR(type)->loadTypeName( stmt, 1 );
        IfPTR(type)->loadSafeRank( stmt, 2 );
        ret = true;
        goto ExitProc;
    }
    else if ( rc == SQLITE_DONE )
    {
        ret = false;
        goto ExitProc;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }
    
ExitProc:
    // 正常无错误结束
    sqlite3_reset(stmt);
    if ( stmt )
        sqlite3_finalize(stmt);
    return ret;
    
OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return false;
}

int LoadAccounts( sqlite3 * db, int userId, AccountArray * accounts, int cateId )
{
    IfPTR(accounts)->RemoveAll();

    int count = 0;
    int paramIndex = 0;

    winplus::AnsiString sql = winplus::StringToUtf8( winplus::String("SELECT * FROM am_accounts WHERE user = ?") + ( cateId != -1 ? " AND cate = ?" : "" ) + " ORDER BY cate;" );
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;

    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 参数绑定
    // 所属用户
    rc = Fields::_bindInt( stmt, ++paramIndex, userId );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 所属种类
    if ( cateId != -1 )
    {
        rc = Fields::_bindInt( stmt, ++paramIndex, cateId );
        if ( rc != SQLITE_OK ) goto OccurDbError;
    }

    // 执行语句
    while ( ( rc = sqlite3_step(stmt) ) == SQLITE_ROW )
    {
        Account account;
        account.loadMyName( stmt, 0 );
        account.loadAccountName( stmt, 1 );
        account.loadAccountPwd( stmt, 2 );
        account.loadCateId( stmt, 3 );
        account.loadUserId( stmt, 4 );
        account.loadSafeRank( stmt, 5 );
        account.loadComment( stmt, 6 );
        account.loadTime( stmt, 7 );//*/
        IfPTR(accounts)->Add(account);
        count++;
    }
    
    if ( rc == SQLITE_DONE ) // 数据已完或没有数据
    {
        goto ExitProc;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }
    
ExitProc:
    // 正常无错误结束
    sqlite3_reset(stmt);
    if ( stmt )
        sqlite3_finalize(stmt);
    return count;
    
OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return 0;
}

bool GetAccount( sqlite3 * db, int userId, CString const & myName, Account * account )
{
    bool ret = false;

    winplus::AnsiString sql = winplus::StringToUtf8("SELECT * FROM am_accounts WHERE user = ? AND myname = ?;");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;

    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 绑定参数
    // 所属用户
    rc = Fields::_bindInt( stmt, 1, userId );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 自定种类名
    rc = Fields::_bindString( stmt, 2, myName );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    rc = sqlite3_step(stmt);
    if ( rc == SQLITE_ROW )
    {
        IfPTR(account)->loadMyName( stmt, 0 );
        IfPTR(account)->loadAccountName( stmt, 1 );
        IfPTR(account)->loadAccountPwd( stmt, 2 );
        IfPTR(account)->loadCateId( stmt, 3 );
        IfPTR(account)->loadUserId( stmt, 4 );
        IfPTR(account)->loadSafeRank( stmt, 5 );
        IfPTR(account)->loadComment( stmt, 6 );
        IfPTR(account)->loadTime( stmt, 7 );
        ret = true;
        goto ExitProc;
    }
    else if ( rc == SQLITE_DONE )
    {
        ret = false;
        goto ExitProc;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }
    
ExitProc:
    // 正常无错误结束
    sqlite3_reset(stmt);
    if ( stmt )
        sqlite3_finalize(stmt);
    return ret;
    
OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return false;
}

bool AddAccount( sqlite3 * db, Account const & newAccount )
{
    int rowsChanged = 0;

    winplus::AnsiString sql = winplus::StringToUtf8("INSERT INTO am_accounts( myname, account_name, account_pwd, cate, user, safe_rank, comment, time ) VALUES( ?, ?, ?, ?, ?, ?, ?, ? );");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;

    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    
    // 绑定参数
    // 账户自定种类名
    rc = newAccount.bindMyName( stmt, 1 );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    
    // 账户名
    rc = newAccount.bindAccountName( stmt, 2 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 账户密码
    rc = newAccount.bindAccountPwd( stmt, 3 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // CateId
    rc = newAccount.bindCateId( stmt, 4 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 所属用户
    rc = newAccount.bindUserId( stmt, 5 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 安全权值
    rc = newAccount.bindSafeRank( stmt, 6 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 备注
    rc = newAccount.bindComment( stmt, 7 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 写入时间
    rc = Fields::_bindInt( stmt, 8, (int)winplus::GetUtcTime() );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    rc = sqlite3_step(stmt);
    if ( rc == SQLITE_DONE )
    {
        rowsChanged = sqlite3_changes(db);
        rc = sqlite3_reset(stmt);
        if ( rc != SQLITE_OK ) goto OccurDbError;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }

    // 正常无错误结束
    if ( stmt )
        sqlite3_finalize(stmt);
    return rowsChanged == 1;

OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return false;
}

bool ModifyAccount( sqlite3 * db, int userId, CString const & myName, Account const & newAccount )
{
    int rowsChanged = 0;

    winplus::AnsiString sql = winplus::StringToUtf8("UPDATE am_accounts SET myname = ?, account_name = ?, account_pwd = ?, cate = ?, safe_rank = ?, comment = ? WHERE user = ? AND myname = ?;");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;

    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 绑定参数
    // 新账户自定种类名
    rc = newAccount.bindMyName( stmt, 1 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 新账户名
    rc = newAccount.bindAccountName( stmt, 2 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 新账户密码
    rc = newAccount.bindAccountPwd( stmt, 3 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 新CateId
    rc = newAccount.bindCateId( stmt, 4 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 新安全权值
    rc = newAccount.bindSafeRank( stmt, 5 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 新备注
    rc = newAccount.bindComment( stmt, 6 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 所属用户
    rc = Fields::_bindInt( stmt, 7, userId );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 账户自定名
    rc = Fields::_bindString( stmt, 8, myName );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    rc = sqlite3_step(stmt);
    if ( rc == SQLITE_DONE )
    {
        rowsChanged = sqlite3_changes(db);
        rc = sqlite3_reset(stmt);
        if ( rc != SQLITE_OK ) goto OccurDbError;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }
    
    // 正常无错误结束
    if ( stmt )
        sqlite3_finalize(stmt);
    return rowsChanged == 1;
    
OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return false;
}

bool DeleteAccount( sqlite3 * db, int userId, CString const & myName )
{
    int rowsChanged = 0;

    winplus::AnsiString sql = winplus::StringToUtf8("DELETE FROM am_accounts WHERE user = ? AND myname = ?;");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;

    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 绑定参数
    // 所属用户
    rc = Fields::_bindInt( stmt, 1, userId );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 自定名
    rc = Fields::_bindString( stmt, 2, myName );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    rc = sqlite3_step(stmt);
    if ( rc == SQLITE_DONE )
    {
        rowsChanged = sqlite3_changes(db);
        rc = sqlite3_reset(stmt);
        if ( rc != SQLITE_OK ) goto OccurDbError;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }

    // 正常无错误结束
    if ( stmt )
        sqlite3_finalize(stmt);
    return rowsChanged == 1;
    
OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return false;
}

CString GetCorrectAccountMyName( sqlite3 * db, int userId, CString const & myName )
{
    CString result;
    int number = 0;

    winplus::AnsiString sql = winplus::StringToUtf8("SELECT myname FROM am_accounts WHERE user = ? AND myname = ?;");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;

    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    result = myName;
    // 参数绑定
    // 所属用户
    rc = Fields::_bindInt( stmt, 1, userId );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 自定种类名
    rc = Fields::_bindString( stmt, 2, result );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    while ( ( rc = sqlite3_step(stmt) ) == SQLITE_ROW )
    {
        rc = sqlite3_reset(stmt);
        if ( rc != SQLITE_OK ) goto OccurDbError;
        ++number;
        result.Format( _T("%s%d"), (LPCTSTR)myName, number );
        // 参数绑定
        // 所属用户
        rc = Fields::_bindInt( stmt, 1, userId );
        if ( rc != SQLITE_OK ) goto OccurDbError;
        
        // 自定种类名
        rc = Fields::_bindString( stmt, 2, result );
        if ( rc != SQLITE_OK ) goto OccurDbError;
    }

    if ( rc == SQLITE_DONE ) // 数据已完或没有数据
    {
        goto ExitProc;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }

ExitProc:
    // 正常无错误结束
    sqlite3_reset(stmt);
    if ( stmt )
        sqlite3_finalize(stmt);
    return result;

OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return _T("");
}

int LoadTableNames( sqlite3 * db, winplus::StringArray * tableNames, winplus::String const & like )
{
    int count = 0;
    IfPTR(tableNames)->clear();

    winplus::AnsiString sql = winplus::StringToUtf8("SELECT tbl_name FROM sqlite_master WHERE type = \'table\' AND tbl_name LIKE ? ESCAPE \'\\\';");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;

    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    rc = sqlite3_bind_text( stmt, 1, winplus::StringToUtf8(like).c_str(), -1, SQLITE_TRANSIENT );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    while ( ( rc = sqlite3_step(stmt) ) == SQLITE_ROW )
    {
        IfPTR(tableNames)->push_back( winplus::Utf8ToString( (char const *)sqlite3_column_text( stmt, 0 ) ) );
        count++;
    }
    
    if ( rc == SQLITE_DONE ) // 数据已完或没有数据
    {
        goto ExitProc;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }
    
ExitProc:
    // 正常无错误结束
    sqlite3_reset(stmt);
    if ( stmt )
        sqlite3_finalize(stmt);
    return count;
    
OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return 0;
}

int DumpDDL( sqlite3 * db, winplus::String * ddl, winplus::String const & like )
{
    int count = 0;

    winplus::AnsiString sql = winplus::StringToUtf8("SELECT sql FROM sqlite_master WHERE tbl_name LIKE ? ESCAPE \'\\\' AND NOT sql IS Null ORDER BY tbl_name;");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;

    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    rc = sqlite3_bind_text( stmt, 1, winplus::StringToUtf8(like).c_str(), -1, SQLITE_TRANSIENT );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    while ( ( rc = sqlite3_step(stmt) ) == SQLITE_ROW )
    {
        AssignPTR(ddl) += winplus::Utf8ToString( (char const *)sqlite3_column_text( stmt, 0 ) ) + _T(";") + winplus::lineSep;
        count++;
    }

    if ( rc == SQLITE_DONE ) // 数据已完或没有数据
    {
        goto ExitProc;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }

ExitProc:
    // 正常无错误结束
    sqlite3_reset(stmt);
    if ( stmt )
        sqlite3_finalize(stmt);
    return count;

OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return 0;
}

bool IsBrowserExeName( sqlite3 * db, CString const & exeName, CString * browserTitle )
{
    bool ret = false;

    winplus::AnsiString sql = winplus::StringToUtf8("SELECT title FROM am_browsers WHERE exe_name = ?;");
    sqlite3_stmt * stmt = NULL;
    int rc;
    const char * localError;

    // 准备语句
    rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 绑定参数
    rc = sqlite3_bind_text( stmt, 1, winplus::StringToUtf8( (LPCTSTR)exeName ).c_str(), -1, SQLITE_TRANSIENT );
    if ( rc != SQLITE_OK ) goto OccurDbError;

    // 执行语句
    rc = sqlite3_step(stmt);
    if ( rc == SQLITE_ROW )
    {
        AssignPTR(browserTitle) = winplus::Utf8ToString( (char const *)sqlite3_column_text( stmt, 0 ) ).c_str();
        ret = true;
        goto ExitProc;
    }
    else if ( rc == SQLITE_DONE )
    {
        ret = false;
        goto ExitProc;
    }
    else
    {
        sqlite3_reset(stmt);
        goto OccurDbError;
    }

ExitProc:
    // 正常无错误结束
    sqlite3_reset(stmt);
    if ( stmt )
        sqlite3_finalize(stmt);
    return ret;

OccurDbError: // 出错处理
    localError = sqlite3_errmsg(db);
    if ( stmt )
        sqlite3_finalize(stmt);
    AfxGetMainWnd()->FatalError( winplus::Utf8ToString(localError).c_str(), _T("数据库错误") );
    return false;

}
