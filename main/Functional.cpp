///////////////////////////////////////////////////////////////////////////
// Name:         Functional.cpp
// Purpos:       程序所有的功能操作
///////////////////////////////////////////////////////////////////////////
#include "Cassette.h"
#include "CassetteApp.h"
#include "Functional.h"

static ansi_string const _base64_encode_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

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

string Base64Encode( ansi_string const & data )
{
	string outStr;
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
		// convert into four bytes string
		outStr += _base64_encode_chars[ c1>>2 ];
		outStr += _base64_encode_chars[ ((c1&0x3)<<4) | ((c2&0xF0)>>4) ];
		outStr += _base64_encode_chars[ ((c2&0xF)<<2) | ((c3&0xC0)>>6) ];
		outStr += _base64_encode_chars[ c3&0x3F ];
	}

	return outStr;
}

ansi_string Base64Decode( string const & base64Str )
{
	ansi_string outData;
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

ansi_string EncryptContent( ansi_string const & content )
{
	ansi_string res(content);
	int outSize;
	raw_encrypt( (unsigned char *)content.c_str(), content.size(), (unsigned char *)&res[0], res.size(), &outSize );
	return res;
}

ansi_string DecryptContent( ansi_string const & encryptContent )
{
	ansi_string res(encryptContent);
	int outSize;
	raw_decrypt( (unsigned char *)encryptContent.c_str(), encryptContent.size(), (unsigned char *)&res[0], res.size(), &outSize );
	return res;
}

string ExplainCustomVars( string const & str )
{
	string vars[] = {
		TEXT("$ROOT$"),
			TEXT("$CURRENT$")
	};
	string vals[] = {
		module_path(),
			get_current_dir()
	};
	multi_match mm( vars, countof(vars), vals, countof(vals) );
	return mm.replace(str);
}

CString GetExecutablePath()
{
	return ( module_path() + dir_sep ).c_str();
}

bool RegisterUser( CString const & username, CString const & password, int protectLevel, UINT32 hotkey )
{
	ansi_string encryptPassword;
	int rowsChanged;

	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("INSERT INTO am_users( name, pwd, protect, condone, cur_condone, unlock_time, hotkey, time ) VALUES( ?, ?, ?, ?, ?, ?, ?, ? );");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;

	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	// 绑定参数
	// 用户名
	rc = sqlite3_bind_text( stmt, 1, string_to_utf8( (LPCTSTR)username ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	// 密码
	encryptPassword = EncryptContent( string_to_utf8( (LPCTSTR)password ) );
	rc = sqlite3_bind_blob( stmt, 2, encryptPassword.c_str(), encryptPassword.size(), SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	// 保护级别
	rc = sqlite3_bind_int( stmt, 3, protectLevel );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	// 容错数
	rc = sqlite3_bind_int( stmt, 4, 3 );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	// 当前剩余容错数
	rc = sqlite3_bind_int( stmt, 5, 3 );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	// 解锁时刻
	rc = sqlite3_bind_int( stmt, 6, (int)get_utc_time() );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	// 热键
	rc = sqlite3_bind_int( stmt, 7, (int)hotkey );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	// 注册时刻
	rc = sqlite3_bind_int( stmt, 8, (int)get_utc_time() );
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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return false;
}

bool DeleteUser( CString const & username )
{
	int rowsChanged;

	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("DELETE FROM am_users WHERE name = ?;");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;

	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	// 绑定参数
	// 用户名
	rc = sqlite3_bind_text( stmt, 1, string_to_utf8( (LPCTSTR)username ).c_str(), -1, SQLITE_TRANSIENT );
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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return false;
}


bool LoginUser( CString const & username, CString const & password, UserInfo * userInfo )
{
	bool ret = false;

	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("SELECT * FROM am_users WHERE name = ?;");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;

	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	// 绑定参数
	// 用户名
	rc = sqlite3_bind_text( stmt, 1, string_to_utf8( (LPCTSTR)username ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 执行语句
	rc = sqlite3_step(stmt);
	if ( rc == SQLITE_ROW )
	{
		// 判断用户是否处于锁定时间中
		int condone, curCondone;
		int unlockTime = sqlite3_column_int( stmt, FieldIndex(am_users__unlock_time) );
		int nowTime = get_utc_time();

		condone = sqlite3_column_int( stmt, FieldIndex(am_users__condone) );
		curCondone = sqlite3_column_int( stmt, FieldIndex(am_users__cur_condone) );

		if ( nowTime < unlockTime ) // 处于锁定中
		{
			int hours = ( unlockTime - nowTime ) / 3600;
			int minutes = ( ( unlockTime - nowTime ) - hours * 3600 ) / 60;
			int seconds = ( unlockTime - nowTime ) - hours * 3600 - minutes * 60;
			AfxGetMainWnd()->WarningError( format( _T("用户锁定中,解锁还需要%d小时%d分%d秒"), hours, minutes, seconds ).c_str(), _T("错误") );
			ret = false;
			goto ExitProc;
		}
		else
		{
			if ( curCondone < 1 ) // 表示这是刚从锁定状态恢复,重置cur_condone=condone
			{
				ModifyUserEx( username, am_users__cur_condone, 0, "", "", 0, 0, condone, 0, 0, 0 );
				curCondone = condone;
			}

		}

		// 验证密码
		ansi_string pwdInDb;
		int size = sqlite3_column_bytes( stmt, 2 );
		pwdInDb.assign( (char const *)sqlite3_column_blob( stmt, 2 ), size );
		CString pwd = utf8_to_string( DecryptContent(pwdInDb) ).c_str();
		if ( pwd != password )
		{
			// 减少当前容错次数
			curCondone--;

			ModifyUserEx( username, am_users__cur_condone, 0, "", "", 0, 0, curCondone, 0, 0, 0 );

			AfxGetMainWnd()->WarningError( format( _T("密码不正确，你还剩%d次机会"), curCondone ).c_str(), _T("错误") );

			if ( curCondone < 1 ) // 没有容错数,锁定用户
			{
				int lockTime = 3600 * 3;
				ModifyUserEx( username, am_users__unlock_time, 0, "", "", 0, 0, 0, get_utc_time() + lockTime, 0, 0 );
			}
			ret = false;
			goto ExitProc;
		}
		// 登录验证成功,重置cur_condone=condone
		ModifyUserEx( username, am_users__cur_condone, 0, "", "", 0, 0, condone, 0, 0, 0 );
		curCondone = condone;

		if ( userInfo != NULL )
		{
			userInfo->m_id = sqlite3_column_int( stmt, 0 );
			userInfo->m_username = utf8_to_string( (char const *)sqlite3_column_text( stmt, 1 ) ).c_str();
			userInfo->m_protectLevel = sqlite3_column_int( stmt, 3 );
			userInfo->m_condone = sqlite3_column_int( stmt, 4 );
			userInfo->m_curCondone = sqlite3_column_int( stmt, 5 );
			userInfo->m_unlockTime = sqlite3_column_int( stmt, 6 );
			userInfo->m_hotkey = sqlite3_column_int( stmt, 7 );
			userInfo->m_regTime = sqlite3_column_int( stmt, 8 );
		}
		ret = true;
		goto ExitProc;
	}
	else if ( rc == SQLITE_DONE ) // 没有找到用户
	{
		MessageBox( *AfxGetMainWnd(), _T("没有此用户"), _T("错误"), MB_OK | MB_ICONEXCLAMATION );
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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return false;
}

bool LoadUser( CString const & username, UserInfo * userInfo )
{
	bool ret = false;

	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("SELECT * FROM am_users WHERE name = ?;");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;
	
	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	// 绑定参数
	// 用户名
	rc = sqlite3_bind_text( stmt, 1, string_to_utf8( (LPCTSTR)username ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 执行语句
	rc = sqlite3_step(stmt);
	if ( rc == SQLITE_ROW )
	{
		if ( userInfo != NULL )
		{
			userInfo->m_id = sqlite3_column_int( stmt, 0 );
			userInfo->m_username = utf8_to_string( (char const *)sqlite3_column_text( stmt, 1 ) ).c_str();
			userInfo->m_protectLevel = sqlite3_column_int( stmt, 3 );
			userInfo->m_condone = sqlite3_column_int( stmt, 4 );
			userInfo->m_curCondone = sqlite3_column_int( stmt, 5 );
			userInfo->m_unlockTime = sqlite3_column_int( stmt, 6 );
			userInfo->m_hotkey = sqlite3_column_int( stmt, 7 );
			userInfo->m_regTime = sqlite3_column_int( stmt, 8 );
		}
		ret = true;
		goto ExitProc;
	}
	else if ( rc == SQLITE_DONE ) // 没有找到用户
	{
		if ( userInfo )
		{
			userInfo->m_id = 0;
			userInfo->m_username = _T("Guest");
			userInfo->m_protectLevel = 0;
			userInfo->m_condone = 0;
			userInfo->m_curCondone = 0;
			userInfo->m_unlockTime = 0x7fffff;
			userInfo->m_hotkey = 0;
			userInfo->m_regTime = 0;
		}
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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return false;
}

bool VerifyUserPassword( CString const & username, CString const & password )
{
	ansi_string encryptPassword;
	bool ret = false;
	
	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("SELECT pwd = ? FROM am_users WHERE name = ?;");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;

	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	// 绑定参数
	// 用户名
	rc = sqlite3_bind_text( stmt, 2, string_to_utf8( (LPCTSTR)username ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	// 密码
	encryptPassword = EncryptContent( string_to_utf8( (LPCTSTR)password ) );
	rc = sqlite3_bind_blob( stmt, 1, encryptPassword.c_str(), encryptPassword.size(), SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 执行语句
	rc = sqlite3_step(stmt);
	if ( rc == SQLITE_ROW )
	{
		ret = sqlite3_column_int( stmt, 0 ) != 0;
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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return false;
}

bool ModifyUser( CString const & username, bool isModifyPassword, CString const & newUsername, CString const & newPassword, int newProtectLevel, int newCondone, int newCurCondone, int newHotkey )
{
	return ModifyUserEx(
		username,
		am_users__name |
		( isModifyPassword ? am_users__pwd : 0 ) |
		am_users__protect |
		am_users__condone |
		am_users__cur_condone |
		am_users__hotkey,
		0,
		newUsername,
		newPassword,
		newProtectLevel,
		newCondone,
		newCurCondone,
		0,
		newHotkey,
		0
	);
}

bool ModifyUserEx( CString const & username, UINT modifiedFieldBits, int newId, CString const & newUsername, CString const & newPassword, int newProtectLevel, int newCondone, int newCurCondone, int newUnlockTime, int newHotkey, int newTime )
{
	ansi_string encryptPassword;
	int rowsChanged;
	int paramIndex;

	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string_array modifiedFields;
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

	ansi_string sql = string_to_utf8(
		"UPDATE am_users SET " + str_join( ", ", modifiedFields ) + " WHERE name = ?;"
	);
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;

	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 绑定参数
	paramIndex = 0;

	// 新ID
	if ( modifiedFieldBits & am_users__id )
	{
		rc = sqlite3_bind_int( stmt, ++paramIndex, newId );
		if ( rc != SQLITE_OK ) goto OccurDbError;
	}

	// 新用户名
	if ( modifiedFieldBits & am_users__name )
	{
		rc = sqlite3_bind_text( stmt, ++paramIndex, string_to_utf8( (LPCTSTR)newUsername ).c_str(), -1, SQLITE_TRANSIENT );
		if ( rc != SQLITE_OK ) goto OccurDbError;
	}
	
	// 新密码
	if ( modifiedFieldBits & am_users__pwd )
	{
		encryptPassword = EncryptContent( string_to_utf8( (LPCTSTR)newPassword ) );
		rc = sqlite3_bind_blob( stmt, ++paramIndex, encryptPassword.c_str(), encryptPassword.size(), SQLITE_TRANSIENT );
		if ( rc != SQLITE_OK ) goto OccurDbError;
	}

	// 新保护级别
	if ( modifiedFieldBits & am_users__protect )
	{
		rc = sqlite3_bind_int( stmt, ++paramIndex, newProtectLevel );
		if ( rc != SQLITE_OK ) goto OccurDbError;
	}
	
	// 新容错数
	if ( modifiedFieldBits & am_users__condone )
	{
		rc = sqlite3_bind_int( stmt, ++paramIndex, newCondone );
		if ( rc != SQLITE_OK ) goto OccurDbError;
	}
	
	// 新当前容错数
	if ( modifiedFieldBits & am_users__cur_condone )
	{
		rc = sqlite3_bind_int( stmt, ++paramIndex, newCurCondone );
		if ( rc != SQLITE_OK ) goto OccurDbError;
	}

	// 新解锁时刻
	if ( modifiedFieldBits & am_users__unlock_time )
	{
		rc = sqlite3_bind_int( stmt, ++paramIndex, newUnlockTime );
		if ( rc != SQLITE_OK ) goto OccurDbError;
	}
	
	// 新热键
	if ( modifiedFieldBits & am_users__hotkey )
	{
		rc = sqlite3_bind_int( stmt, ++paramIndex, newHotkey );
		if ( rc != SQLITE_OK ) goto OccurDbError;
	}

	// 新录入时刻
	if ( modifiedFieldBits & am_users__time )
	{
		rc = sqlite3_bind_int( stmt, ++paramIndex, newTime );
		if ( rc != SQLITE_OK ) goto OccurDbError;
	}

	// where子句
	// 用户名
	rc = sqlite3_bind_text( stmt, ++paramIndex, string_to_utf8( (LPCTSTR)username ).c_str(), -1, SQLITE_TRANSIENT );
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

	// 无硬性错误的结束
	if ( stmt )
		sqlite3_finalize(stmt);
	return rowsChanged == 1;
	
OccurDbError: // 出错处理
	localError = sqlite3_errmsg(db);
	if ( stmt )
		sqlite3_finalize(stmt);
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return false;
}

int LoadAccountTypes( CStringArray * typeNames, CUIntArray * safeRanks )
{
	int count = 0;
	IfPTR(typeNames)->RemoveAll();
	IfPTR(safeRanks)->RemoveAll();

	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("SELECT * FROM am_account_types ORDER BY rank;");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;
	
	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 执行语句
	while ( ( rc = sqlite3_step(stmt) ) == SQLITE_ROW )
	{
		IfPTR(typeNames)->Add( utf8_to_string( (char const *)sqlite3_column_text( stmt, 0 ) ).c_str() );
		IfPTR(safeRanks)->Add( sqlite3_column_int( stmt, 1 ) );
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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return 0;
}

bool GetAccountType( CString const & typeName, int * safeRank )
{
	bool ret = false;
	
	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("SELECT * FROM am_account_types WHERE name = ?;");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;
	
	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	// 绑定参数
	// 类型名
	rc = sqlite3_bind_text( stmt, 1, string_to_utf8( (LPCTSTR)typeName ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 执行语句
	rc = sqlite3_step(stmt);
	if ( rc == SQLITE_ROW )
	{
		ASSERT( typeName == utf8_to_string( (char const *)sqlite3_column_text( stmt, 0 ) ).c_str() );
		AssignPTR(safeRank) = sqlite3_column_int( stmt, 1 );
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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return false;
}

bool AddAccountType( CString const & typeName, int safeRank )
{
	int rowsChanged;
	
	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("INSERT INTO am_account_types( name, rank ) VALUES( ?, ? );");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;
	
	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 绑定参数
	// 类型名
	rc = sqlite3_bind_text( stmt, 1, string_to_utf8( (LPCTSTR)typeName ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 安全值
	rc = sqlite3_bind_int( stmt, 2, safeRank );
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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return false;
}

bool ModifyAccountType( CString const & typeName, CString const & newTypeName, int newSafeRank )
{
	int rowsChanged;
	
	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("UPDATE am_account_types SET name = ?, rank = ? WHERE name = ?;");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;
	
	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 绑定参数
	// 新类型名
	rc = sqlite3_bind_text( stmt, 1, string_to_utf8( (LPCTSTR)newTypeName ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 新安全值
	rc = sqlite3_bind_int( stmt, 2, newSafeRank );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 类型名
	rc = sqlite3_bind_text( stmt, 3, string_to_utf8( (LPCTSTR)typeName ).c_str(), -1, SQLITE_TRANSIENT );
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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return false;
}

bool DeleteAccountType( CString const & typeName )
{
	int rowsChanged = 0;
	
	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql2, sql;
	sqlite3_stmt * stmt2 = NULL;
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;

	// 准备语句
	sql2 = string_to_utf8("SELECT COUNT(*) > 0 FROM am_account_cates WHERE type = ?;");
	rc = sqlite3_prepare_v2( db, sql2.c_str(), sql2.size(), &stmt2, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 绑定参数
	// 类型名
	rc = sqlite3_bind_text( stmt2, 1, string_to_utf8( (LPCTSTR)typeName ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 执行语句,判断是否执行删除类型操作
	rc = sqlite3_step(stmt2);
	if ( rc != SQLITE_ROW ) goto OccurDbError;

	if ( sqlite3_column_int( stmt2, 0 ) )
	{
		// 不能删除此类型
		AfxGetMainWnd()->WarningError( _T("该类型下关联有账户种类，因此不能删除"), _T("错误") );
		goto ExitProc;
	}

	//////////////////////////////////////////////////////////////////////////
	// 准备语句
	sql = string_to_utf8("DELETE FROM am_account_types WHERE name = ?;");
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 绑定参数
	// 类型名
	rc = sqlite3_bind_text( stmt, 1, string_to_utf8( (LPCTSTR)typeName ).c_str(), -1, SQLITE_TRANSIENT );
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

ExitProc:
	// 无硬性错误的结束
	if ( stmt2 )
		sqlite3_finalize(stmt2);
	if ( stmt )
		sqlite3_finalize(stmt);
	return rowsChanged == 1;

OccurDbError: // 出错处理
	localError = sqlite3_errmsg(db);
	if ( stmt2 )
		sqlite3_finalize(stmt2);
	if ( stmt )
		sqlite3_finalize(stmt);
	AfxGetMainWnd()->FatalError( utf8_to_string(localError).c_str(), _T("数据库错误") );
	return false;
}

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
)
{
	IfPTR(ids)->RemoveAll();
	IfPTR(cateNames)->RemoveAll();
	IfPTR(cateDescs)->RemoveAll();
	IfPTR(typeNames)->RemoveAll();
	IfPTR(urls)->RemoveAll();
	IfPTR(icoPaths)->RemoveAll();
	IfPTR(startups)->RemoveAll();
	IfPTR(keywordss)->RemoveAll();
	IfPTR(timeWritens)->RemoveAll();

	int count = 0;

	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("SELECT * FROM am_account_cates ORDER BY id;");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;
	
	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 执行语句
	while ( ( rc = sqlite3_step(stmt) ) == SQLITE_ROW )
	{
		IfPTR(ids)->Add( sqlite3_column_int( stmt, 0 ) );
		IfPTR(cateNames)->Add( utf8_to_string( (char const *)sqlite3_column_text( stmt, 1 ) ).c_str() );
		IfPTR(cateDescs)->Add( utf8_to_string( (char const *)sqlite3_column_text( stmt, 2 ) ).c_str() );
		IfPTR(typeNames)->Add( utf8_to_string( (char const *)sqlite3_column_text( stmt, 3 ) ).c_str() );
		IfPTR(urls)->Add( utf8_to_string( (char const *)sqlite3_column_text( stmt, 4 ) ).c_str() );
		IfPTR(icoPaths)->Add( utf8_to_string( (char const *)sqlite3_column_text( stmt, 5 ) ).c_str() );
		IfPTR(startups)->Add( utf8_to_string( (char const *)sqlite3_column_text( stmt, 6 ) ).c_str() );
		IfPTR(keywordss)->Add( utf8_to_string( (char const *)sqlite3_column_text( stmt, 7 ) ).c_str() );
		IfPTR(timeWritens)->Add( sqlite3_column_int( stmt, 8 ) );

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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return 0;
}

bool GetAccountCate( int id, CString * cateName, CString * cateDesc, CString * typeName, CString * url, CString * icoPath, CString * startup, CString * keywords, int * timeWriten )
{
	bool ret = false;

	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("SELECT * FROM am_account_cates WHERE id = ?;");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;
	
	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	// 绑定参数
	// cateId
	rc = sqlite3_bind_int( stmt, 1, id );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 执行语句
	rc = sqlite3_step(stmt);
	if ( rc == SQLITE_ROW )
	{
		ASSERT( id == sqlite3_column_int( stmt, 0 ) );
		AssignPTR(cateName) = utf8_to_string( (char const *)sqlite3_column_text( stmt, 1 ) ).c_str();
		AssignPTR(cateDesc) = utf8_to_string( (char const *)sqlite3_column_text( stmt, 2 ) ).c_str();
		AssignPTR(typeName) = utf8_to_string( (char const *)sqlite3_column_text( stmt, 3 ) ).c_str();
		AssignPTR(url) = utf8_to_string( (char const *)sqlite3_column_text( stmt, 4 ) ).c_str();
		AssignPTR(icoPath) = utf8_to_string( (char const *)sqlite3_column_text( stmt, 5 ) ).c_str();
		AssignPTR(startup) = utf8_to_string( (char const *)sqlite3_column_text( stmt, 6 ) ).c_str();
		AssignPTR(keywords) = utf8_to_string( (char const *)sqlite3_column_text( stmt, 7 ) ).c_str();
		AssignPTR(timeWriten) = sqlite3_column_int( stmt, 8 );
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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return false;
}

int AddAccountCate( CString const & cateName, CString const & cateDesc, CString const & typeName, CString const & url, CString const & icoPath, CString const & startup, CString const & keywords )
{
	int rowsChanged;
	
	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("INSERT INTO am_account_cates( name, desc, type, url, icon, startup, keywords, time ) VALUES( ?, ?, ?, ?, ?, ?, ?, ? );");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;
	
	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 绑定参数
	// 种类名
	rc = sqlite3_bind_text( stmt, 1, string_to_utf8( (LPCTSTR)cateName ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 种类描述
	rc = sqlite3_bind_text( stmt, 2, string_to_utf8( (LPCTSTR)cateDesc ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 类型名
	rc = sqlite3_bind_text( stmt, 3, string_to_utf8( (LPCTSTR)typeName ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// URL
	rc = sqlite3_bind_text( stmt, 4, string_to_utf8( (LPCTSTR)url ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 图标路径
	rc = sqlite3_bind_text( stmt, 5, string_to_utf8( (LPCTSTR)icoPath ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 启动方式
	rc = sqlite3_bind_text( stmt, 6, string_to_utf8( (LPCTSTR)startup ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 关键字
	rc = sqlite3_bind_text( stmt, 7, string_to_utf8( (LPCTSTR)keywords ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 写入时间
	rc = sqlite3_bind_int( stmt, 8, (int)get_utc_time() );
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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return 0;
}

bool ModifyAccountCate( int id, CString const & newCateName, CString const & newCateDesc, CString const & newTypeName, CString const & newUrl, CString const & newIcoPath, CString const & newStartup, CString const & newKeywords )
{
	int rowsChanged;
	
	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("UPDATE am_account_cates SET name = ?, desc = ?, type = ?, url = ?, icon = ?, startup = ?, keywords = ? WHERE id = ?;");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;
	
	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 绑定参数
	// 新种类名
	rc = sqlite3_bind_text( stmt, 1, string_to_utf8( (LPCTSTR)newCateName ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 新种类描述
	rc = sqlite3_bind_text( stmt, 2, string_to_utf8( (LPCTSTR)newCateDesc ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 新类型名
	rc = sqlite3_bind_text( stmt, 3, string_to_utf8( (LPCTSTR)newTypeName ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 新URL
	rc = sqlite3_bind_text( stmt, 4, string_to_utf8( (LPCTSTR)newUrl ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 新图标路径
	rc = sqlite3_bind_text( stmt, 5, string_to_utf8( (LPCTSTR)newIcoPath ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 新启动方式
	rc = sqlite3_bind_text( stmt, 6, string_to_utf8( (LPCTSTR)newStartup ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 新关键字
	rc = sqlite3_bind_text( stmt, 7, string_to_utf8( (LPCTSTR)newKeywords ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// CateId
	rc = sqlite3_bind_int( stmt, 8, id );
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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return false;
}

bool DeleteAccountCate( int id )
{
	int rowsChanged = 0;

	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql, sql2;
	sqlite3_stmt * stmt2 = NULL;
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;

	// 准备语句
	sql2 = string_to_utf8("SELECT COUNT(*) > 0 FROM am_accounts WHERE cate = ?;");
	rc = sqlite3_prepare_v2( db, sql2.c_str(), sql2.size(), &stmt2, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 绑定参数
	// Cate ID
	rc = sqlite3_bind_int( stmt2, 1, id );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 执行语句,判断是否执行删除类型操作
	rc = sqlite3_step(stmt2);
	if ( rc != SQLITE_ROW ) goto OccurDbError;
	
	if ( sqlite3_column_int( stmt2, 0 ) )
	{
		// 不能删除此类型
		AfxGetMainWnd()->WarningError( _T("该种类下关联有账户，因此不能删除"), _T("错误") );
		goto ExitProc;
	}

	//////////////////////////////////////////////////////////////////////////
	// 准备语句
	sql = string_to_utf8("DELETE FROM am_account_cates WHERE id = ?;");
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 绑定参数
	// CateId
	rc = sqlite3_bind_int( stmt, 1, id );
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

ExitProc:
	// 无硬性错误的结束
	if ( stmt2 )
		sqlite3_finalize(stmt2);
	if ( stmt )
		sqlite3_finalize(stmt);
	return rowsChanged == 1;
	
OccurDbError: // 出错处理
	localError = sqlite3_errmsg(db);
	if ( stmt2 )
		sqlite3_finalize(stmt2);
	if ( stmt )
		sqlite3_finalize(stmt);
	AfxGetMainWnd()->FatalError( utf8_to_string(localError).c_str(), _T("数据库错误") );
	return false;
}

int LoadAccountCatesSafeRank( CUIntArray * cateIds, CUIntArray * typeSafeRanks )
{
	IfPTR(cateIds)->RemoveAll();
	IfPTR(typeSafeRanks)->RemoveAll();

	int count = 0;
	
	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("SELECT c.id, t.rank from am_account_cates AS c Left JOIN am_account_types AS t ON t.name = c.type;");
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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return 0;
}

bool GetTypeByCateId( int cateId, CString * typeName, int * safeRank )
{
	bool ret = false;
	
	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("SELECT c.id, t.name, t.rank FROM am_account_cates AS c Left JOIN am_account_types AS t ON t.name = c.type WHERE c.id = ?;");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;
	
	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	// 绑定参数
	// cateId
	rc = sqlite3_bind_int( stmt, 1, cateId );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 执行语句
	rc = sqlite3_step(stmt);
	if ( rc == SQLITE_ROW )
	{
		char const * sUTF8 = (char const *)sqlite3_column_text( stmt, 1 );
		ASSERT( cateId == sqlite3_column_int( stmt, 0 ) );
		AssignPTR(typeName) = utf8_to_string( sUTF8 ? sUTF8 : "" ).c_str();
		AssignPTR(safeRank) = sqlite3_column_int( stmt, 2 );
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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return false;
}

bool AddAccount( CString const & myName, CString const & accountName, CString const & accountPwd, int cateId, int safeRank, CString const & comment )
{
	ansi_string encryptAccountName, encryptAccountPwd;

	int rowsChanged;
	
	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("INSERT INTO am_accounts( myname, account_name, account_pwd, cate, user, safe_rank, comment, time ) VALUES( ?, ?, ?, ?, ?, ?, ?, ? );");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;
	
	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 绑定参数
	// 账户自定种类名
	rc = sqlite3_bind_text( stmt, 1, string_to_utf8( (LPCTSTR)myName ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 账户名
	encryptAccountName = EncryptContent( string_to_utf8( (LPCTSTR)accountName ) );
	rc = sqlite3_bind_blob( stmt, 2, encryptAccountName.c_str(), encryptAccountName.size(), SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 账户密码
	encryptAccountPwd = EncryptContent( string_to_utf8( (LPCTSTR)accountPwd ) );
	rc = sqlite3_bind_blob( stmt, 3, encryptAccountPwd.c_str(), encryptAccountPwd.size(), SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// CateId
	rc = sqlite3_bind_int( stmt, 4, cateId );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 所属用户
	rc = sqlite3_bind_int( stmt, 5, g_theApp.m_loginedUser.m_id );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 安全权值
	rc = sqlite3_bind_int( stmt, 6, safeRank );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 备注
	rc = sqlite3_bind_text( stmt, 7, string_to_utf8( (LPCTSTR)comment ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 写入时间
	rc = sqlite3_bind_int( stmt, 8, (int)get_utc_time() );
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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return false;
}

bool ModifyAccount( CString const & myName, CString const & newMyName, CString const & newAccountName, CString const & newAccountPwd, int newCateId, int newSafeRank, CString const & newComment )
{
	ansi_string encryptAccountName, encryptAccountPwd;
	int rowsChanged;
	
	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("UPDATE am_accounts SET myname = ?, account_name = ?, account_pwd = ?, cate = ?, safe_rank = ?, comment = ? WHERE user = ? AND myname = ?;");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;
	
	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 绑定参数
	// 新账户自定种类名
	rc = sqlite3_bind_text( stmt, 1, string_to_utf8( (LPCTSTR)newMyName ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 新账户名
	encryptAccountName = EncryptContent( string_to_utf8( (LPCTSTR)newAccountName ) );
	rc = sqlite3_bind_blob( stmt, 2, encryptAccountName.c_str(), encryptAccountName.size(), SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 新账户密码
	encryptAccountPwd = EncryptContent( string_to_utf8( (LPCTSTR)newAccountPwd ) );
	rc = sqlite3_bind_blob( stmt, 3, encryptAccountPwd.c_str(), encryptAccountPwd.size(), SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 新CateId
	rc = sqlite3_bind_int( stmt, 4, newCateId );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 新安全权值
	rc = sqlite3_bind_int( stmt, 5, newSafeRank );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 新备注
	rc = sqlite3_bind_text( stmt, 6, string_to_utf8( (LPCTSTR)newComment ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 所属用户
	rc = sqlite3_bind_int( stmt, 7, g_theApp.m_loginedUser.m_id );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 账户自定种类名
	rc = sqlite3_bind_text( stmt, 8, string_to_utf8( (LPCTSTR)myName ).c_str(), -1, SQLITE_TRANSIENT );
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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return false;
}

bool DeleteAccount( CString const & myName )
{
	int rowsChanged;
	
	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("DELETE FROM am_accounts WHERE user = ? AND myname = ?;");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;
	
	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 绑定参数
	// 所属用户
	rc = sqlite3_bind_int( stmt, 1, g_theApp.m_loginedUser.m_id );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 自定种类名
	rc = sqlite3_bind_text( stmt, 2, string_to_utf8( (LPCTSTR)myName ).c_str(), -1, SQLITE_TRANSIENT );
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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return false;
}

int LoadAccounts(
	CStringArray * myNames,
	CStringArray * accountNames,
	CStringArray * accountPwds,
	CUIntArray * cateIds,
	CUIntArray * safeRanks,
	CStringArray * comments,
	CUIntArray * times
)
{
	IfPTR(myNames)->RemoveAll();
	IfPTR(accountNames)->RemoveAll();
	IfPTR(accountPwds)->RemoveAll();
	IfPTR(cateIds)->RemoveAll();
	IfPTR(safeRanks)->RemoveAll();
	IfPTR(comments)->RemoveAll();
	IfPTR(times)->RemoveAll();

	int count = 0;

	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("SELECT * FROM am_accounts WHERE user = ? ORDER BY cate;");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;

	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	// 参数绑定
	// 所属用户
	rc = sqlite3_bind_int( stmt, 1, g_theApp.m_loginedUser.m_id );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 执行语句
	while ( ( rc = sqlite3_step(stmt) ) == SQLITE_ROW )
	{
		int size;
		ansi_string encryptAccountName, encryptAccountPwd;

		IfPTR(myNames)->Add( utf8_to_string( (char const *)sqlite3_column_text( stmt, 0 ) ).c_str() );

		size = sqlite3_column_bytes( stmt, 1 );
		encryptAccountName.assign( (char const *)sqlite3_column_blob( stmt, 1 ), size );
		// 这里是个bug,数据库存的BLOB数据并未进行过UTF8加密,因此现在进行修复
		// 利用GUI重新设定数据库内的名字即可
		IfPTR(accountNames)->Add( utf8_to_string( DecryptContent(encryptAccountName) ).c_str() );

		size = sqlite3_column_bytes( stmt, 2 );
		encryptAccountPwd.assign( (char const *)sqlite3_column_blob( stmt, 2 ), size );
		IfPTR(accountPwds)->Add( utf8_to_string( DecryptContent(encryptAccountPwd) ).c_str() );

		IfPTR(cateIds)->Add( sqlite3_column_int( stmt, 3 ) );

		ASSERT( g_theApp.m_loginedUser.m_id == sqlite3_column_int( stmt, 4 ) );

		IfPTR(safeRanks)->Add( sqlite3_column_int( stmt, 5 ) );
		IfPTR(comments)->Add( utf8_to_string( (char const *)sqlite3_column_text( stmt, 6 ) ).c_str() );
		IfPTR(times)->Add( sqlite3_column_int( stmt, 7 ) );

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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return 0;
}

bool GetAccount(
	CString const & myName,
	CString * accountName,
	CString * accountPwd,
	int * cateId,
	int * safeRank,
	CString * comment,
	int * time
)
{
	bool ret = false;
	
	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("SELECT * FROM am_accounts WHERE user = ? AND myname = ?;");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;
	
	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	// 绑定参数
	// 所属用户
	rc = sqlite3_bind_int( stmt, 1, g_theApp.m_loginedUser.m_id );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 自定种类名
	rc = sqlite3_bind_text( stmt, 2, string_to_utf8( (LPCTSTR)myName ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 执行语句
	rc = sqlite3_step(stmt);
	if ( rc == SQLITE_ROW )
	{
		int size;
		ansi_string encryptAccountName, encryptAccountPwd;

		ASSERT( myName == utf8_to_string( (char const *)sqlite3_column_text( stmt, 0 ) ).c_str() );

		size = sqlite3_column_bytes( stmt, 1 );
		encryptAccountName.assign( (char const *)sqlite3_column_blob( stmt, 1 ), size );
		AssignPTR(accountName) = utf8_to_string( DecryptContent(encryptAccountName) ).c_str();

		size = sqlite3_column_bytes( stmt, 2 );
		encryptAccountPwd.assign( (char const *)sqlite3_column_blob( stmt, 2 ), size );
		AssignPTR(accountPwd) = utf8_to_string( DecryptContent(encryptAccountPwd) ).c_str();

		AssignPTR(cateId) = sqlite3_column_int( stmt, 3 );

		ASSERT( g_theApp.m_loginedUser.m_id == sqlite3_column_int( stmt, 4 ) );

		AssignPTR(safeRank) = sqlite3_column_int( stmt, 5 );
		AssignPTR(comment) = utf8_to_string( (char const *)sqlite3_column_text( stmt, 6 ) ).c_str();
		AssignPTR(time) = sqlite3_column_int( stmt, 7 );
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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return false;
}

bool BackupData( CString const & filename )
{
	bool ret = false;
	g_theApp.CloseDatabase();
	ret = FALSE != CopyFile( ExplainCustomVars(g_theApp.m_settings.databasePath).c_str(), filename, FALSE );
	g_theApp.OpenDatabase();
	return ret;
}

bool ResumeData( CString const & filename )
{
	bool ret = false;
	g_theApp.CloseDatabase();
	ret = FALSE != CopyFile( filename, ExplainCustomVars(g_theApp.m_settings.databasePath).c_str(), FALSE );
	g_theApp.OpenDatabase();
	return ret;
}

CString GetCorrectAccountMyName( CString const & myName )
{
	CString result;
	int number = 0;

	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("SELECT myname FROM am_accounts WHERE user = ? AND myname = ?;");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;
	
	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	result = myName;
	// 参数绑定
	// 所属用户
	rc = sqlite3_bind_int( stmt, 1, g_theApp.m_loginedUser.m_id );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 自定种类名
	rc = sqlite3_bind_text( stmt, 2, string_to_utf8( (LPCTSTR)result ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 执行语句
	while ( ( rc = sqlite3_step(stmt) ) == SQLITE_ROW )
	{
		rc = sqlite3_reset(stmt);
		if ( rc != SQLITE_OK ) goto OccurDbError;
		number++;
		result.Format( _T("%s%d"), (LPCTSTR)myName, number );
		// 参数绑定
		// 所属用户
		rc = sqlite3_bind_int( stmt, 1, g_theApp.m_loginedUser.m_id );
		if ( rc != SQLITE_OK ) goto OccurDbError;
		
		// 自定种类名
		rc = sqlite3_bind_text( stmt, 2, string_to_utf8( (LPCTSTR)result ).c_str(), -1, SQLITE_TRANSIENT );
		if ( rc != SQLITE_OK ) goto OccurDbError;

	}

	if ( rc == SQLITE_DONE ) // 数据已完或没有数据
	{
		goto ExitProc;
	}
	else
	{
		rc = sqlite3_reset(stmt);
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
	MessageBox( *AfxGetMainWnd(), utf8_to_string(localError).c_str(), _T("数据库错误"), MB_OK | MB_ICONERROR );
	return _T("");
}

int LoadTableNames( sqlite3 * db, string_array * tableNames, string const & like )
{
	int count = 0;
	IfPTR(tableNames)->clear();

	ansi_string sql = string_to_utf8("SELECT tbl_name FROM sqlite_master WHERE type = \'table\' AND tbl_name LIKE ? ESCAPE \'\\\';");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;

	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	rc = sqlite3_bind_text( stmt, 1, string_to_utf8(like).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 执行语句
	while ( ( rc = sqlite3_step(stmt) ) == SQLITE_ROW )
	{
		IfPTR(tableNames)->push_back( utf8_to_string( (char const *)sqlite3_column_text( stmt, 0 ) ) );
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
	AfxGetMainWnd()->FatalError( utf8_to_string(localError).c_str(), _T("数据库错误") );
	return 0;
}

int DumpDDL( sqlite3 * db, string * ddl, string const & like )
{
	int count = 0;
	
	ansi_string sql = string_to_utf8("SELECT sql FROM sqlite_master WHERE tbl_name LIKE ? ESCAPE \'\\\' AND NOT sql IS Null ORDER BY tbl_name;");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;
	
	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	rc = sqlite3_bind_text( stmt, 1, string_to_utf8(like).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	
	// 执行语句
	while ( ( rc = sqlite3_step(stmt) ) == SQLITE_ROW )
	{
		AssignPTR(ddl) += utf8_to_string( (char const *)sqlite3_column_text( stmt, 0 ) ) + _T(";") + line_sep;
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
	AfxGetMainWnd()->FatalError( utf8_to_string(localError).c_str(), _T("数据库错误") );
	return 0;
}

bool IsBrowserExeName( CString const & exeName, CString * browserTitle )
{
	bool ret = false;
	
	sqlite3 * db = g_theApp.GetDatabase();
	ansi_string sql = string_to_utf8("SELECT title FROM am_browsers WHERE exe_name = ?;");
	sqlite3_stmt * stmt = NULL;
	int rc;
	const char * localError;

	// 准备语句
	rc = sqlite3_prepare_v2( db, sql.c_str(), sql.size(), &stmt, NULL );
	if ( rc != SQLITE_OK ) goto OccurDbError;
	// 绑定参数
	rc = sqlite3_bind_text( stmt, 1, string_to_utf8( (LPCTSTR)exeName ).c_str(), -1, SQLITE_TRANSIENT );
	if ( rc != SQLITE_OK ) goto OccurDbError;

	// 执行语句
	rc = sqlite3_step(stmt);
	if ( rc == SQLITE_ROW )
	{
		AssignPTR(browserTitle) = utf8_to_string( (char const *)sqlite3_column_text( stmt, 0 ) ).c_str();
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
	AfxGetMainWnd()->FatalError( utf8_to_string(localError).c_str(), _T("数据库错误") );
	return false;

}