#include "eiendb_common.hpp"
#include "eiendb_sqlite.hpp"

#ifdef HAVE_DB_SQLITE
#include "sqlite3.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#if defined(_MSC_VER) || defined(WIN32)
#pragma warning( disable: 4018 )
#endif

#ifndef UNICODE
    #define _tremove remove
#else
    #define _tremove _wremove
#endif

namespace eiendb
{
#ifdef HAVE_DB_SQLITE

#include "is_x_funcs.inl"

#define TRY_LOAD_LINE( ifile, i, line ) ( i < line.length() || ( i = 0, ( line = ifile->getLine() ).length() ) )

static winux::String __SqliteLoadSqlFile_String( winux::IFile * ifile, winux::String::size_type & i, winux::String & line )
{
    winux::String::value_type quote = line[i];
    winux::String s;

    i++; // skip quote
    s += quote;

    while ( TRY_LOAD_LINE( ifile, i, line ) )
    {
        winux::String::value_type ch = line[i];
        if ( ch == quote )
        {
            s += ch;
            i++;
            // 判断后面还有字符没有
            if ( TRY_LOAD_LINE( ifile, i, line ) ) // 还有字符
            {
                // 判断引号之后是不是还连着一个引号
                ch = line[i];
                if ( ch == quote )
                {
                    s += ch;
                    i++;
                }
                else // 后面是其他字符
                {
                    // 结束字符串解析
                    break;
                }
            }
            else // 没有字符了
            {
                // 结束字符串解析
                break;
            }
        }
        else
        {
            s += ch;
            i++;
        }
    }

    return s;
}

static size_t __SqliteLoadSqlFile( winux::IFile * ifile, winux::StringArray * arrSql, IDbConnection * cnn )
{
    winux::String sql;
    size_t nSql = 0;

    winux::String::size_type i = 0;
    winux::String line;

    while ( TRY_LOAD_LINE( ifile, i, line ) )
    {
        winux::String::value_type ch = line[i];
        if ( i == 0 && ch == '-' ) // 一行开头
        {
            i++; // skip first '-'
            if ( TRY_LOAD_LINE( ifile, i, line ) )
            {
                ch = line[i];
                if ( ch == '-' ) // equal "--" start
                {
                    i = line.length(); // skip a line
                }
                else
                {
                    sql += '-';
                    sql += ch;
                    i++;
                }
            }
            else
            {
                break;
            }
        }
        else if ( ch == '\'' || ch == '\"' )
        {
            sql += __SqliteLoadSqlFile_String( ifile, i, line );
        }
        else if ( ch == ';' )
        {
            sql += ch;
            arrSql->push_back(sql);
            sql.clear();
            nSql++;
            i++;
        }
        else
        {
            // 当sql不为空时，空白字符才予加入
            if ( sql.empty() )
            {
                if ( !IsSpace(ch) )
                {
                    sql += ch;
                }
            }
            else
            {
                sql += ch;
            }
            i++;
        }
    }

    if ( winux::StrTrim(sql).length() > 0 )
    {
        arrSql->push_back(sql);
        sql.clear();
        nSql++;
    }
    return nSql;
}

// 通过PRAGMA table_info(tablename)获取表的字段类型说明，再根据辨别规则甄别。see TypeAffinityTable.txt
// 辨别类型亲缘性（affinity），以便类型转换和构造
static winux::String __GetTypeAffinity( winux::String typeDeclare )
{
    winux::StrMakeLower(&typeDeclare);
    if ( typeDeclare.find("int") != winux::String::npos )
    {
        return TEXT("integer");
    }
    else if ( typeDeclare.find("char") != winux::String::npos || typeDeclare.find("clob") != winux::String::npos || typeDeclare.find("text") != winux::String::npos )
    {
        return TEXT("text");
    }
    else if ( typeDeclare == "blob" || typeDeclare.empty() )
    {
        return TEXT("none");
    }
    else if ( typeDeclare.find("real") != winux::String::npos || typeDeclare.find("floa") != winux::String::npos || typeDeclare.find("doub") != winux::String::npos )
    {
        return TEXT("real");
    }
    return TEXT("numeric");
}

// class SqliteConnection -------------------------------------------------------------------
SqliteConnection::SqliteConnection( winux::String const & path, winux::AnsiString const & dbKey, winux::String linkCharset, bool doConnect )
: _db(nullptr), _path(path), _linkCharset(linkCharset), _dbKey(dbKey)
{
    winux::StrMakeLower(&_linkCharset);
    doConnect && this->connect();
}

SqliteConnection::~SqliteConnection()
{
    this->close();
}

bool SqliteConnection::setLinkCharset( winux::String const & charset )
{
    _linkCharset = charset;
    winux::StrMakeLower(&_linkCharset);
    return true;
}

bool SqliteConnection::_open( winux::String const & database, winux::uint flags )
{
    this->close();
    int rc;
    _database = database;
    // 打开数据库
    rc = sqlite3_open_v2( stringToSqliteStr( winux::CombinePath( _dirPath, database ) ).c_str(), &_db, flags, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    // 开启外键约束
    rc = sqlite3_exec( _db, "PRAGMA foreign_keys = ON", NULL, NULL, NULL );
    if ( rc != SQLITE_OK ) goto OccurDbError;
    rc = sqlite3_extended_result_codes( _db, 1 );
    if ( rc != SQLITE_OK ) goto OccurDbError;

//#ifdef SQLITE_HAS_CODEC
    // 如果有密码
    if ( _dbKey.size() > 0 )
    {
        rc = sqlite3_key( _db, _dbKey.c_str(), (int)_dbKey.size() );
        if ( rc != SQLITE_OK ) goto OccurDbError;
    }
//#endif
    return true;
OccurDbError:
    return false;
}

bool SqliteConnection::createDb( winux::String const & database )
{
    return this->_open( database, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE );
}

bool SqliteConnection::selectDb( winux::String const & database )
{
    return this->_open( database, SQLITE_OPEN_READWRITE );
}

bool SqliteConnection::close()
{
    if ( _db != nullptr )
    {
        int rc;
        //do
        //{
        rc = sqlite3_close_v2(_db);
        //} while ( rc == SQLITE_BUSY );
        //if ( rc != SQLITE_OK ) throw SqliteDbError( this->errNo(), this->error().c_str() );

        _db = nullptr;
        return true;
    }
    return false;
}

void SqliteConnection::alive()
{
    if ( _db == nullptr )
    {
        this->connect();
    }
}

bool SqliteConnection::connect()
{
    _path = winux::NormalizePath(_path); // 正规化路径
    if ( !_path.empty() )
    {
        if ( _path == TEXT(":memory:") )
        {
            _database = _path;
            return this->selectDb(_database);
        }
        else
        {
            // 判断数据库是否存在，把路径和库名分开
            bool isExists, isDir;
            isExists = winux::DetectPath( _path, &isDir );
            if ( isExists && !isDir ) // 存在数据库文件，则直接打开它
            {
                _dirPath = winux::FilePath( _path, &_database );
                return this->selectDb(_database);
            }
            else if ( isExists && isDir ) // path是一个目录
            {
                _dirPath = _path;
                return false;
            }
            else // 不存在数据库文件，创建它
            {
                _dirPath = winux::FilePath( _path, &_database );
                return this->createDb(_database);
            }
        }
    }
    return false;
}

bool SqliteConnection::dropDb( winux::String const & database )
{
    if ( _database == database )
        this->close();

    return 0 == _tremove( winux::CombinePath( _dirPath, database ).c_str() );
}

size_t SqliteConnection::affectedRows()
{
    if ( _db == nullptr ) throw SqliteDbError( 0, "SQLite no valid connection" );

    return (size_t)sqlite3_changes(_db);
}

size_t SqliteConnection::insertId()
{
    if ( _db == nullptr ) throw SqliteDbError( 0, "SQLite no valid connection" );

    return (size_t)sqlite3_last_insert_rowid(_db);
}

int SqliteConnection::errNo()
{
    if ( _db == nullptr ) throw SqliteDbError( 0, "SQLite no valid connection" );

    return sqlite3_errcode(_db);
}

winux::String SqliteConnection::error()
{
    if ( _db == nullptr ) throw SqliteDbError( 0, "SQLite no valid connection" );

    return sqliteStrToString( sqlite3_errmsg(_db) );
}

// 只是简单的引号double。内部不会进行utf8换码
winux::String SqliteConnection::escape( winux::String const & str, winux::String const & addQuote )
{
    winux::String r;
    //winux::String line, nl;
    //int i = 0;
    if ( str.length() > 0 )
    {
        if ( !addQuote.empty() )
        {
            r += addQuote + winux::AddQuotes( str, '\'' ) + addQuote;
        }
        else
        {
            r += winux::AddQuotes( str, '\'' );
        }
        /*while ( winux::StrGetLine( &line, str, &i, &nl ) )
        {
            if ( !line.empty() )
            {
                if ( !r.empty() ) r += " || ";

                if ( !addQuote.empty() )
                {
                    r += addQuote + winux::AddQuotes( line, '\'' ) + addQuote;
                }
                else
                {
                    r += winux::AddQuotes( line, '\'' );
                }
            }

            if ( !nl.empty() )
            {
                r += " || " + this->escape( (winux::byte *)nl.c_str(), nl.length() );
            }
        }*/
    }
    else
    {
        r = "";
        if ( !addQuote.empty() )
        {
            return addQuote + r + addQuote;
        }
    }
    return r;
}

// 将二进制数据转换成X'******'的形式
winux::String SqliteConnection::escape( void const * buf, size_t size, winux::String const & addQuote )
{
    winux::String r;
    size_t i;
    if ( size > 0 )
    {
        r += "X\'";
        for ( i = 0; i < size; ++i )
        {
            r += winux::FormatEx( 10, TEXT("%02X"), *( (winux::byte *)buf + i ) );
        }
        r += "\'";
    }
    else
    {
        r = "";
        if ( !addQuote.empty() )
        {
            return addQuote + r + addQuote;
        }
    }
    return r;
}

// 执行SQL语句查询并返回结果集
winux::SharedPointer<IDbResult> SqliteConnection::query( winux::String const & sql )
{
    return winux::SharedPointer<IDbResult>( new SqliteResult( winux::SharedPointer<SqliteStatement>( new SqliteStatement( this, sql ) ) ) );
}

winux::SharedPointer<IDbResult> SqliteConnection::query( winux::SharedPointer<IDbStatement> stmt )
{
    winux::SharedPointer<SqliteStatement> stmt2 = stmt.ensureCast<SqliteStatement>();
    return winux::SharedPointer<IDbResult>( new SqliteResult(stmt2) );
}

// 执行SQL语句不需要返回结果集，返回bool代表是否成功
bool SqliteConnection::exec( winux::String const & sql )
{
    if ( _db == nullptr ) throw SqliteDbError( 0, "SQLite no valid connection" );

    int rc;
    rc = sqlite3_exec( _db, stringToSqliteStr(sql).c_str(), nullptr, nullptr, nullptr );

    if ( rc != SQLITE_OK ) throw SqliteDbError( this->errNo(), this->error() );

    return true;
}

bool SqliteConnection::exec( winux::SharedPointer<IDbStatement> stmt )
{
    if ( _db == nullptr ) throw SqliteDbError( 0, "SQLite no valid connection" );

    sqlite3_stmt * sqStmt = *static_cast<SqliteStatement *>( stmt.get() );

    if ( sqStmt == nullptr ) throw SqliteDbError( 0, "Invalid statement object" );

    int rc;
    rc = sqlite3_step(sqStmt);
    switch ( rc )
    {
    case SQLITE_ROW:
    case SQLITE_DONE:
        return true;
        break;
    }

    return false;
}

winux::SharedPointer<IDbResult> SqliteConnection::listDbs()
{
    winux::String fields[] = { "Databases" };
    MemoryResult * p = new MemoryResult(fields);
    // 枚举_dirPath目录下所有的db文件
    winux::StringArray rows;
    winux::EnumFiles( _dirPath, "db", &rows );
    winux::StringArray::const_iterator it;
    for ( it = rows.begin(); it != rows.end(); ++it )
    {
        winux::String values[] = { *it };
        p->addRow(values);
    }

    return winux::SharedPointer<IDbResult>(p);
}

// 执行 PRAGMA table_info(tablename)
winux::SharedPointer<IDbResult> SqliteConnection::listFields( winux::String const & tableName )
{
    winux::String fields[] = { "Fields" };
    auto p = winux::MakeShared( new MemoryResult(fields) );
    winux::String sql = "PRAGMA table_info(" + this->symbolQuotes(tableName) + ");";
    auto rs = this->query(sql);
    winux::MixedArray f;
    while ( rs->fetchArray(&f) )
    {
        winux::String values[] = { f[1] };
        p->addRow(values);
    }
    return p;
}

// 执行 SELECT name FROM sqlite_master WHERE type = 'table' AND substr( name, 1, 7 ) <> 'sqlite_' ORDER BY name;
winux::SharedPointer<IDbResult> SqliteConnection::listTables()
{
    winux::String fields[] = { "Tables" };
    MemoryResult * p = new MemoryResult(fields);
    winux::String sql = TEXT("SELECT name FROM sqlite_master WHERE type = \'table\' AND substr( name, 1, 7 ) <> \'sqlite_\' ORDER BY name;");
    winux::SharedPointer<IDbResult> rs = this->query(sql);
    winux::MixedArray f;
    while ( rs->fetchArray(&f) )
    {
        winux::String values[] = { f[0] };
        p->addRow(values);
    }

    return winux::SharedPointer<IDbResult>(p);
}

inline static winux::String __ReplaceCrlfToLf( winux::String const & str )
{
    thread_local winux::MultiMatch mmReplaceCrlfToLf( { "\r\n" }, { "\n" } );
    return mmReplaceCrlfToLf.replace(str);
}

winux::String SqliteConnection::tableDdl( winux::String const & tableName )
{
    winux::MixedArray params;
    params.push_back(tableName);
    winux::SharedPointer<IDbStatement> stmt = this->buildStmt( "SELECT sql FROM sqlite_master WHERE tbl_name LIKE ? ESCAPE \'\\\' AND NOT sql IS Null ORDER BY tbl_name;", params );
    winux::SharedPointer<IDbResult> rs = this->query(stmt);
    winux::MixedArray f;
    winux::String ddl;
    while ( rs->fetchArray(&f) )
    {
        ddl += __ReplaceCrlfToLf(f[0]);
        ddl += ";\n";
    }
    return ddl;
}

winux::String SqliteConnection::symbolQuotes( winux::String const & str, bool periodAsSeparator )
{
    if ( periodAsSeparator )
    {
        winux::StringArray arr;
        winux::StrSplit( str, ".", &arr );
        return "[" + winux::StrJoin( "].[", arr ) + "]";
    }
    else
    {
        return "[" + str + "]";
    }
}

size_t SqliteConnection::loadSqlFile( winux::IFile * sqlFile, winux::StringArray * arrSql )
{
    return __SqliteLoadSqlFile( sqlFile, arrSql, this );
}

size_t SqliteConnection::getPrimaryKey( winux::String const & tableName, winux::StringArray * arrKeyColumn )
{
    winux::String sql = winux::Format( "PRAGMA table_info(%s);", this->symbolQuotes(tableName).c_str() );
    winux::SharedPointer<IDbStatement> stmt = this->buildStmt(sql);
    winux::SharedPointer<SqliteStatement> stmt2 = stmt.ensureCast<SqliteStatement>();

    SqliteResult res(stmt2);
    winux::StringMixedMap f;
    std::map< int, winux::String > keyColumn;
    while ( res.fetchMap(&f) )
    {
        winux::String fieldname = f["name"];
        int pk = f["pk"];
        if ( pk > 0 )
        {
            keyColumn[pk] = fieldname;
        }
    }
    arrKeyColumn->clear();
    for ( auto it = keyColumn.begin(); it != keyColumn.end(); ++it )
    {
        arrKeyColumn->push_back(it->second);
    }

    return arrKeyColumn->size();
}

winux::SharedPointer<IDbStatement> SqliteConnection::buildStmt( winux::String const & sql, winux::Mixed const & params )
{
    return winux::SharedPointer<IDbStatement>( new SqliteStatement( this, sql, params ) );
}

// SQLite的字符串转到winux::String，NULL也能正确处理
winux::String SqliteConnection::sqliteStrToString( char const * s )
{
    if ( s != nullptr )
    {
        if ( _linkCharset.empty() )
        {
            return winux::LocalFromUtf8(s);
        }
        else if ( _linkCharset == "utf-8" )
        {
            return s;
        }
        else
        {
            winux::Conv conv( "utf-8", _linkCharset );
            return conv.convert< winux::String, winux::AnsiString >(s);
        }
    }
    return TEXT("");
}

// winux::String转到SQLite字符串
winux::AnsiString SqliteConnection::stringToSqliteStr( winux::String const & s )
{
    if ( _linkCharset.empty() )
    {
        return winux::LocalToUtf8(s);
    }
    else if ( _linkCharset == "utf-8" )
    {
        return s;
    }
    else
    {
        winux::Conv conv( _linkCharset, "utf-8" );
        return conv.convert< winux::AnsiString, winux::String >(s);
    }
}

// class SqliteStatement ------------------------------------------------------------------------
SqliteStatement::SqliteStatement( SqliteConnection * cnn ) : _cnn(cnn), _stmt(NULL)
{
}

SqliteStatement::SqliteStatement( SqliteConnection * cnn, winux::String const & sql, winux::Mixed const & params ) : _cnn(cnn), _stmt(NULL)
{
    this->build( sql, params );
}

SqliteStatement::~SqliteStatement()
{
    this->_free();
}

void SqliteStatement::_free()
{
    if ( _stmt != nullptr )
    {
        sqlite3_finalize(_stmt);
        _stmt = nullptr;
    }
}

bool SqliteStatement::build( winux::String const & sql, winux::Mixed const & params )
{
    this->_free();

    if ( !*_cnn ) throw SqliteDbError( 0, "SQLite no valid connection" );

    this->_sql = sql;
    winux::AnsiString sqlUtf8 = _cnn->stringToSqliteStr(sql);
    int rc = sqlite3_prepare_v2( *_cnn, sqlUtf8.c_str(), (int)sqlUtf8.length(), &_stmt, nullptr );

    bool r = rc == SQLITE_OK;
    if ( !r )
    {
        throw SqliteDbError( _cnn->errNo(), _cnn->error() );
    }
    else
    {
        if ( params.isNull() )
        {
        }
        else if ( params.isArray() )
        {
            size_t i, count = params.getCount();
            for ( i = 0; i < count; ++i )
            {
                bool bindOk = this->bind( i + 1, params[i] );
                if ( r && !bindOk ) r = false;
            }
        }
        else if ( params.isCollection() )
        {
            size_t i, count = params.getCount();
            for ( i = 0; i < count; ++i )
            {
                auto & pr = params.getPair(i);
                bool bindOk = this->bind( pr.first.toString<winux::String::value_type>(), pr.second );
                if ( r && !bindOk ) r = false;
            }
        }
        else // 只是一个值，作为bind(1,val)处理
        {
            bool bindOk = this->bind( 1, params );
            if ( r && !bindOk ) r = false;
        }
    }
    return r;
}

bool SqliteStatement::bind( size_t paramIndex, winux::Mixed const & val )
{
    if ( _stmt == nullptr ) throw SqliteDbError( 0, "Invalid statement object" );

    int rc = SQLITE_ERROR;
    switch ( val._type )
    {
    case winux::Mixed::MT_NULL: // null
        rc = sqlite3_bind_null( _stmt, (int)paramIndex );
        break;
    case winux::Mixed::MT_BOOLEAN:
    case winux::Mixed::MT_BYTE:
    case winux::Mixed::MT_SHORT:
    case winux::Mixed::MT_USHORT:
    case winux::Mixed::MT_INT:
    case winux::Mixed::MT_UINT:
    case winux::Mixed::MT_LONG:
    case winux::Mixed::MT_ULONG:
        rc = sqlite3_bind_int( _stmt, (int)paramIndex, val );
        break;
    case winux::Mixed::MT_INT64:
    case winux::Mixed::MT_UINT64: // integer
        rc = sqlite3_bind_int64( _stmt, (int)paramIndex, val );
        break;
    case winux::Mixed::MT_FLOAT:
    case winux::Mixed::MT_DOUBLE: // real
        rc = sqlite3_bind_double( _stmt, (int)paramIndex, val );
        break;
    case winux::Mixed::MT_ANSI:
    case winux::Mixed::MT_UNICODE: // text
        {
            winux::AnsiString s = _cnn->stringToSqliteStr(val);
            rc = sqlite3_bind_text( _stmt, (int)paramIndex, s.c_str(), (int)s.length(), SQLITE_TRANSIENT );
        }
        break;
    case winux::Mixed::MT_ARRAY: // 由于没有类型对应，只好绑定null
        rc = sqlite3_bind_null( _stmt, (int)paramIndex );
        break;
    case winux::Mixed::MT_COLLECTION:
        rc = sqlite3_bind_null( _stmt, (int)paramIndex );
        break;
    case winux::Mixed::MT_BINARY: // blob
        rc = sqlite3_bind_blob( _stmt, (int)paramIndex, val._pBuf->getBuf(), (int)val._pBuf->getSize(), SQLITE_TRANSIENT );
        break;
    }

    // 把绑定的参数记下（参数索引，参数值），以便之后语句更改重绑定
    bool isFound = false;
    for ( auto it = _bindingParams.begin(); it != _bindingParams.end(); ++it )
    {
        if ( it->first == paramIndex )
        {
            isFound = true;
            it->second = val;
            break;
        }
    }
    if ( !isFound )
    {
        _bindingParams.push_back( std::make_pair( paramIndex, val ) );
    }
    return rc == SQLITE_OK;
}

bool SqliteStatement::bind( winux::String const & paramName, winux::Mixed const & val )
{
    if ( _stmt == nullptr ) throw SqliteDbError( 0, "Invalid statement object" );

    int paramIndex = sqlite3_bind_parameter_index( _stmt, _cnn->stringToSqliteStr(paramName).c_str() );
    return this->bind( paramIndex, val );
}

// class SqliteResult ---------------------------------------------------------------------------
SqliteResult::SqliteResult( winux::SharedPointer<SqliteStatement> stmt ) : _stmt(stmt)
{
    if ( !_stmt || !*_stmt.get() ) throw SqliteDbError( 0, "Invalid statement object" );

    // stmt prepare 后可以直接调用: column_name(), column_origin_name(), column_count(), column_decltype().

    // 取得字段数
    int n = sqlite3_column_count(*_stmt.get());
    // 在得到数据之前，可以获取字段名
    for ( int i = 0; i < n; ++i )
    {
        _fieldNames.push_back( this->_stmt->getCnn()->sqliteStrToString( sqlite3_column_name( *_stmt.get(), i ) ) );
    }

    // 预先执行一次
    _hasRow = ( ( _retCode = sqlite3_step(*_stmt.get()) ) == SQLITE_ROW );
    _first = true;
}

SqliteResult::~SqliteResult()
{
    this->free();
}

bool SqliteResult::free()
{
    return true;
}

// 把字段值赋给winux::Mixed，当前必须能获取数据行才能调用
static void __FieldValueAssignToMixed( winux::Mixed & m, SqliteStatement * stmt, int fieldIndex )
{
    // 根据类型自动获取值
    switch ( sqlite3_column_type( *stmt, fieldIndex ) )
    {
    case SQLITE_INTEGER:
        m = sqlite3_column_int64( *stmt, fieldIndex );
        break;
    case SQLITE_FLOAT:
        m = sqlite3_column_double( *stmt, fieldIndex );
        break;
    case SQLITE_TEXT:
        m = stmt->getCnn()->sqliteStrToString( (char const *)sqlite3_column_text( *stmt, fieldIndex ) );
        break;
    case SQLITE_BLOB:
        {
            winux::uint size = sqlite3_column_bytes( *stmt, fieldIndex );
            m.alloc(size);
            void const * data = sqlite3_column_blob( *stmt, fieldIndex );
            memcpy( m._pBuf->getBuf(), data, size );
        }
        break;
    case SQLITE_NULL:
        m = winux::mxNull;
        break;
    }
}

bool SqliteResult::fetchRow( winux::Mixed * fields, int type )
{
    if ( _first )
        _first = false;
    else
        _hasRow = ( ( _retCode = sqlite3_step(*_stmt.get()) ) == SQLITE_ROW );

    if ( _hasRow )
    {
        if ( fields != nullptr )
        {
            if ( type == 0 )
            {
                size_t n = this->fieldsCount();
                fields->createCollection();
                size_t i;
                for ( i = 0; i < n; ++i )
                {
                    __FieldValueAssignToMixed( (*fields)[ _fieldNames[i] ], _stmt.get(), (int)i );
                }
            }
            else if ( type == 1 )
            {
                size_t n = this->fieldsCount();
                fields->createArray(n);
                size_t i;
                for ( i = 0; i < n; ++i )
                {
                    __FieldValueAssignToMixed( (*fields)[i], _stmt.get(), (int)i );
                }
            }
        }
        return true;
    }
    return false;
}

bool SqliteResult::fetchArray( winux::MixedArray * fields )
{
    if ( _first )
        _first = false;
    else
        _hasRow = ( ( _retCode = sqlite3_step(*_stmt.get()) ) == SQLITE_ROW );

    if ( _hasRow )
    {
        if ( fields != nullptr )
        {
            fields->clear();
            size_t n = this->fieldsCount();
            fields->resize(n);
            size_t i;
            for ( i = 0; i < n; ++i )
            {
                __FieldValueAssignToMixed( (*fields)[i], _stmt.get(), (int)i );
            }
        }
        return true;
    }
    return false;
}

bool SqliteResult::fetchMap( winux::StringMixedMap * fields )
{
    if ( _first )
        _first = false;
    else
        _hasRow = ( ( _retCode = sqlite3_step(*_stmt.get()) ) == SQLITE_ROW );

    if ( _hasRow )
    {
        if ( fields != nullptr )
        {
            fields->clear();
            size_t n = this->fieldsCount();
            size_t i;
            for ( i = 0; i < n; ++i )
            {
                __FieldValueAssignToMixed( (*fields)[ _fieldNames[i] ], _stmt.get(), (int)i );
            }
        }
        return true;
    }
    return false;
}

winux::String SqliteResult::fieldType( size_t fieldIndex )
{
    int typeIndex = sqlite3_column_type( *_stmt.get(), (int)fieldIndex );
    winux::String types[] = { "", "integer", "real", "text", "blob", "null" };
    return types[typeIndex];
}

winux::String SqliteResult::fieldName( size_t fieldIndex )
{
    return _fieldNames[fieldIndex];
}

size_t SqliteResult::fieldsCount()
{
    return _fieldNames.size();
}

// 这个方法只能采取SQL："select count(*)"的形式取得数据行数，没有其他的API。
size_t SqliteResult::rowsCount()
{
    // 获取from后面的字串
    winux::String sql = _stmt->getSql();

    winux::StrMakeLower(&sql);

    size_t pos = sql.find("from");
    if ( pos != winux::String::npos )
    {
        winux::SharedPointer<SqliteStatement> stmtRowsCount( new SqliteStatement( _stmt->getCnn(), TEXT("select count(*) ") + _stmt->getSql().substr(pos) ) );
        for ( auto it = _stmt->_bindingParams.begin(); it != _stmt->_bindingParams.end(); ++it )
        {
            stmtRowsCount->bind( it->first, it->second );
        }

        SqliteResult res(stmtRowsCount);
        winux::MixedArray f;
        if ( res.fetchArray(&f) )
        {
            return f[0];
        }
    }
    return 0;
}

bool SqliteResult::reset()
{
    if ( !_stmt || !*_stmt.get() ) throw SqliteDbError( 0, "Invalid statement object" );

    _retCode = sqlite3_reset(*_stmt.get());
    return _retCode == SQLITE_OK;
}

// class SqliteModifier ------------------------------------------------------------------------------------
SqliteModifier::SqliteModifier( SqliteConnection * cnn, winux::String const & tableName ) : DbModifier( cnn, tableName )
{
    this->_getTableInfo();
}

SqliteModifier::~SqliteModifier()
{

}

#endif // HAVE_DB_SQLITE

} // namespace eiendb
