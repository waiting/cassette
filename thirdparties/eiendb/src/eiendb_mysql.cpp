#include "eiendb_common.hpp"
#include "eiendb_mysql.hpp"

#ifdef HAVE_DB_MYSQL
#include <mysql.h>
#endif

namespace eiendb
{
#ifdef HAVE_DB_MYSQL

#include "is_x_funcs.inl"

#define TRY_LOAD_LINE( ifile, i, line ) ( i < line.length() || ( i = 0, ( line = ifile->getLine() ).length() ) )

static winux::String __MysqlLoadSqlFile_String( winux::IFile * ifile, winux::String::size_type & i, winux::String & line )
{
    winux::String::value_type quote = line[i];
    winux::String s;

    i++; // skip quote
    s += quote;

    while ( TRY_LOAD_LINE( ifile, i, line ) )
    {
        winux::String::value_type ch = line[i];
        if ( ch == quote ) // 匹配引号结束
        {
            s += ch;
            i++;
            break;
        }
        else if ( ch == '\\' ) // 转义字符
        {
            s += ch;
            i++;
            while ( TRY_LOAD_LINE( ifile, i, line ) )
            {
                s += line[i];
                i++;
                break;
            }
        }
        else // 其他字符
        {
            s += ch;
            i++;
        }
    }

    return s;
}

static size_t __MysqlLoadSqlFile( winux::IFile * ifile, winux::StringArray * arrSql, IDbConnection * cnn )
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
            sql += __MysqlLoadSqlFile_String( ifile, i, line );
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

// 字符集编码串处理。因为mysql识别的字符编码串和HTML的有一定区别，在此进行转换，统一用HTML式的
static inline winux::String HtmlCharsetToMysqlCharset( winux::String const & charset )
{
    thread_local winux::StringStringMap::value_type ps[] = {
        //winux::StringStringMap::value_type( "gbk", "gbk" ),
        winux::StringStringMap::value_type( "utf-8", "utf8mb4" ),
        //winux::StringStringMap::value_type( "big5", "big5" ),
    };
    thread_local winux::StringStringMap m( ps, ps + countof(ps) );
    if ( m.find(charset) != m.end() )
    {
        return m[charset];
    }
    else
    {
        return charset;
    }
}

// class MysqlConnection ----------------------------------------------------------------------------------------------
MysqlConnection::MysqlConnection(
    winux::String const & host,
    winux::String const & user,
    winux::String const & pwd,
    winux::String const & dbName,
    winux::String const & linkCharset,
    bool doConnect
) : _mysql(nullptr), _port(0), _user(user), _pwd(pwd), _dbName(dbName), _linkCharset(linkCharset)
{
    // 解析端口号
    size_t pos = host.find(':');
    if ( pos != winux::String::npos )
    {
        _hostAddr = host.substr( 0, pos );
        _port = winux::Mixed( host.substr( pos + 1 ) );
    }
    else
    {
        _hostAddr = host;
        _port = 0;
    }

    winux::StrMakeLower(&_linkCharset);
    doConnect && this->connect();
}

MysqlConnection::~MysqlConnection()
{
    this->close();
}

bool MysqlConnection::connect()
{
    this->close();
    _mysql = mysql_init(nullptr);
    if ( !_mysql ) return false;
    MYSQL * cnn = mysql_real_connect( (MYSQL *)_mysql, _hostAddr.c_str(), _user.c_str(), _pwd.c_str(), _dbName.c_str(), _port, nullptr, 0 );
    bool r = _mysql == cnn;
    if ( !r ) // 连接失败
    {
        winux::String err = this->error();
        int errNo = this->errNo();
        this->close();
        throw MysqlDbError( errNo, err );
    }
    // 设置连接校验编码
    this->setLinkCharset(_linkCharset);
    return r;
}

bool MysqlConnection::close()
{
    if ( _mysql != nullptr )
    {
        mysql_close( (MYSQL *)_mysql );
        _mysql = nullptr;
        return true;
    }
    return false;
}

void MysqlConnection::alive()
{
    if ( _mysql != nullptr )
    {
        int rc = mysql_ping( (MYSQL *)_mysql );
        if ( rc != 0 )
        {
            this->connect();
        }
    }
    else
    {
        this->connect();
    }
}

bool MysqlConnection::setLinkCharset( winux::String const & charset )
{
    _linkCharset = charset;
    winux::StrMakeLower(&_linkCharset);

    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );

    if ( !charset.empty() )
    {
        return 0 == mysql_set_character_set( (MYSQL *)_mysql, HtmlCharsetToMysqlCharset(_linkCharset).c_str() );
    }
    return false;
}

bool MysqlConnection::selectDb( winux::String const & database )
{
    _dbName = database;
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    return 0 == mysql_select_db( (MYSQL *)_mysql, _dbName.c_str() );
}

bool MysqlConnection::createDb( winux::String const & database )
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    winux::String sql = "CREATE DATABASE " + this->symbolQuotes(database);
    if ( !_linkCharset.empty() ) sql += " DEFAULT CHARSET=" + HtmlCharsetToMysqlCharset(_linkCharset);
    {
    }
    if ( mysql_real_query( (MYSQL *)_mysql, sql.c_str(), (winux::ulong)sql.length() ) )
        return false;
    return this->selectDb(database);
}

bool MysqlConnection::dropDb( winux::String const & database )
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    winux::String sql = "DROP DATABASE " + this->symbolQuotes(database);
    return 0 == mysql_real_query( (MYSQL *)_mysql, sql.c_str(), (winux::ulong)sql.length() );
}

size_t MysqlConnection::affectedRows()
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    return (winux::ulong)mysql_affected_rows( (MYSQL *)_mysql );
}

size_t MysqlConnection::insertId()
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    return (winux::ulong)mysql_insert_id( (MYSQL *)_mysql );
}

int MysqlConnection::errNo()
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    return mysql_errno( (MYSQL *)_mysql );
}

winux::String MysqlConnection::error()
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    char const * s = mysql_error( (MYSQL *)_mysql );
    return s ? s : "";
}

winux::String MysqlConnection::escape( winux::String const & str, winux::String const & addQuote )
{
    return this->escape( (winux::byte const *)str.c_str(), str.length(), addQuote );
}

winux::String MysqlConnection::escape( void const * buf, size_t size, winux::String const & addQuote )
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    winux::String r;
    if ( size > 0 )
    {
        winux::Buffer szBuf;
        szBuf.alloc( size * 2 + 1, false );
        winux::ulong n = mysql_real_escape_string( (MYSQL *)_mysql, szBuf.get<char>(), (char const *)buf, (winux::ulong)size );
        szBuf._setSize(n);
        r = szBuf.toString<winux::String::value_type>();
    }
    else
    {
        r = "";
    }

    if ( !addQuote.empty() )
    {
        return addQuote + r + addQuote;
    }

    return r;
}

bool MysqlConnection::exec( winux::String const & sql )
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    int rc = mysql_real_query( (MYSQL *)_mysql, sql.c_str(), (winux::ulong)sql.length() );
    if ( rc ) // 语句出错
    {
        throw MysqlDbError( this->errNo(), this->error() );
        return false;
    }
    else
    {
        return true;
    }
}

bool MysqlConnection::exec( winux::SharedPointer<IDbStatement> stmt )
{
    MysqlStatement * pStmt = static_cast<MysqlStatement *>( stmt.get() );
    return this->exec( pStmt->getSql() );
}

winux::SharedPointer<IDbResult> MysqlConnection::query( winux::String const & sql )
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    return winux::SharedPointer<IDbResult>( new MysqlResult( winux::MakeShared( new MysqlStatement( this, sql ) ) ) );
}

winux::SharedPointer<IDbResult> MysqlConnection::query( winux::SharedPointer<IDbStatement> stmt )
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    return winux::SharedPointer<IDbResult>( new MysqlResult( stmt.ensureCast<MysqlStatement>() ) );
}

winux::SharedPointer<IDbResult> MysqlConnection::listDbs()
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    winux::String fields[] = { "Databases" };
    auto p = winux::MakeShared( new MemoryResult(fields) );

    MYSQL_RES * res = mysql_list_dbs( (MYSQL *)_mysql, nullptr );
    if ( res != nullptr )
    {
        MYSQL_ROW row;
        while ( ( row = mysql_fetch_row(res) ) )
        {
            winux::String f[] = { winux::CharSeqToString(row[0]) };
            p->addRow(f);
        }
        mysql_free_result(res);
    }

    return p;
}

winux::SharedPointer<IDbResult> MysqlConnection::listFields( winux::String const & tableName )
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    // 弃用 DESCRIBE `table1`;
    winux::String fields[] = { "Fields" };
    auto p = winux::MakeShared( new MemoryResult(fields) );
    auto rs = this->query( "SHOW COLUMNS FROM " + this->symbolQuotes(tableName) );
    winux::MixedArray f;
    while ( rs->fetchArray(&f) )
    {
        winux::String ff[] = { f[0].toString<winux::tchar>() };
        p->addRow(ff);
    }
    return p;
}

winux::SharedPointer<IDbResult> MysqlConnection::listTables()
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    winux::String fields[] = { "Tables" };
    auto p = winux::MakeShared( new MemoryResult(fields) );
    auto rs = this->query("SHOW FULL TABLES WHERE Table_type='BASE TABLE'");
    winux::MixedArray f;
    while ( rs->fetchArray(&f) )
    {
        winux::String ff[] = { f[0].toString<winux::tchar>() };
        p->addRow(ff);
    }
    return p;
}

winux::String MysqlConnection::tableDdl( winux::String const & tableName )
{
    auto rs = this->query( "SHOW CREATE TABLE " + this->symbolQuotes(tableName) );
    winux::String ddl;
    winux::MixedArray f;
    while ( rs->fetchArray(&f) )
    {
        ddl += f[1].toString<winux::tchar>();
        ddl += ";\n";
    }
    return ddl;
}

winux::String MysqlConnection::symbolQuotes( winux::String const & str, bool periodAsSeparator )
{
    if ( periodAsSeparator )
    {
        winux::StringArray arr;
        winux::StrSplit( str, ".", &arr );
        return "`" + winux::StrJoin( "`.`", arr ) + "`";
    }
    else
    {
        return "`" + str + "`";
    }
}

size_t MysqlConnection::loadSqlFile( winux::IFile * sqlFile, winux::StringArray * arrSql )
{
    return __MysqlLoadSqlFile( sqlFile, arrSql, this );
}

size_t MysqlConnection::getPrimaryKey( winux::String const & tableName, winux::StringArray * arrKeyColumn )
{
    /**
        SELECT
            k.column_name,
            t.table_name,
            table_schema
        FROM
            information_schema.table_constraints t
        JOIN information_schema.key_column_usage k USING (
            constraint_name,
            table_schema,
            table_name
        )
        WHERE
            t.constraint_type = 'PRIMARY KEY'
        AND t.table_schema = 'prjms_dev'
        AND t.table_name = 'prjms_documents'
     */
    // SELECT * FROM INFORMATION_SCHEMA.`KEY_COLUMN_USAGE` WHERE table_schema='prjms_dev' AND table_name='prjms_documents' AND constraint_name='PRIMARY'
    // SELECT COLUMN_NAME FROM information_schema.KEY_COLUMN_USAGE WHERE TABLE_SCHEMA='prjms_dev' AND TABLE_NAME='prjms_documents' AND CONSTRAINT_NAME='PRIMARY' ORDER BY ORDINAL_POSITION;

    winux::SharedPointer<IDbResult> rs = this->query( "SELECT COLUMN_NAME FROM information_schema.KEY_COLUMN_USAGE WHERE TABLE_SCHEMA=" + this->escape(_dbName) + " AND TABLE_NAME=" + this->escape(tableName) + " AND CONSTRAINT_NAME='PRIMARY' ORDER BY ORDINAL_POSITION;" );
    arrKeyColumn->clear();
    winux::MixedArray f;
    while ( rs->fetchArray(&f) )
    {
        arrKeyColumn->push_back(f[0]);
    }

    return arrKeyColumn->size();
}

winux::SharedPointer<IDbStatement> MysqlConnection::buildStmt( winux::String const & sql, winux::Mixed const & params )
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    return winux::SharedPointer<IDbStatement>( new MysqlStatement( this, sql, params ) );
}

// class MysqlStatement ---------------------------------------------------------------------


// class MysqlResult ------------------------------------------------------------------------
MysqlResult::MysqlResult( winux::SharedPointer<MysqlStatement> stmt ) : _mysqlRes(nullptr), _stmt(stmt)
{
    if ( !_stmt || !*_stmt.get() ) throw MysqlDbError( 0, "Invalid statement object" );

    // 执行 mysql_real_query()
    winux::String sql = _stmt->getSql();
    int rc = mysql_real_query( (MYSQL *)(DbHandle)*_stmt->getCnn(), sql.c_str(), (winux::ulong)sql.length() );
    // query()执行后3种情况，a. sql错误，执行错误。b. 执行无错，没有数据。c. 执行无错，有数据。
    if ( rc == 0 ) // 执行无错
    {
        winux::uint n = mysql_field_count( (MYSQL *)(DbHandle)*_stmt->getCnn() );
        if ( n > 0 ) // 有结果集
        {
            _mysqlRes = mysql_store_result( (MYSQL *)(DbHandle)*_stmt->getCnn() );
            if ( _mysqlRes != nullptr )
            {
                // 取得字段名，类型
                for ( winux::uint i = 0; i < n; ++i )
                {
                    MYSQL_FIELD * field = mysql_fetch_field_direct( (MYSQL_RES *)_mysqlRes, i );
                    _fieldNames.push_back(field->name);
                    _fieldTypes.push_back( (winux::uint)field->type );
                    _fieldFlags.push_back(field->flags);
                }
            }
        }
        //else // 没有结果集，可能执行的是UPDATE INSERT ALTER等语句
        //{
        //}
    }
    else
    {
        throw MysqlDbError( _stmt->getCnn()->errNo(), _stmt->getCnn()->error() );
    }
}

MysqlResult::~MysqlResult()
{
    this->free();
}

bool MysqlResult::free()
{
    if ( _mysqlRes != nullptr )
    {
        mysql_free_result( (MYSQL_RES *)_mysqlRes );
        _mysqlRes = nullptr;
        return true;
    }
    return false;
}

bool MysqlResult::dataSeek( size_t index )
{
    // 判断MySQLStatement对象是否有效
    // 判断语句是否有效
    if ( !_stmt || !*_stmt.get() ) throw MysqlDbError( 0, "Invalid statement object" );

    MysqlConnection * cnn = _stmt->getCnn();

    if ( !*cnn ) throw MysqlDbError( 0, "MySQL no valid connection" );

    if ( _mysqlRes == nullptr ) throw MysqlDbError( 0, "The MySQL result set is NULL" );

    mysql_data_seek( (MYSQL_RES *)_mysqlRes, index );
    if ( cnn->errNo() )
    {
        return false;
    }
    return true;
}

#define IS_UNSIGNED_FIELD(f) ( ((f) & BinVal(00100000)) != 0 )

// 把字段值赋给winux::Mixed，当前必须能获取数据行才能调用
inline static void __FieldValueAssignToMixed( winux::Mixed & m, winux::uint type, winux::uint flag, char const * data, winux::ulong len )
{
    if ( data == nullptr && len == 0 ) // 数据是NULL值
    {
        m = winux::mxNull;
    }
    else
    {
        switch ( type )
        {
        case MYSQL_TYPE_DECIMAL: // 超精确数字作string类型输出
            m = data;
            break;
        case MYSQL_TYPE_TINY: // 1字节数字
            {
                winux::ulong v = 0;
                winux::ParseULong( data, &v );
                if ( IS_UNSIGNED_FIELD(flag) )
                {
                    m = (winux::byte)v;
                }
                else
                {
                    m = (winux::int8)v;
                }
            }
            break;
        case MYSQL_TYPE_SHORT:
            {
                winux::ulong v = 0;
                winux::ParseULong( data, &v );
                if ( IS_UNSIGNED_FIELD(flag) )
                {
                    m = (winux::ushort)v;
                }
                else
                {
                    m = (short)v;
                }
            }
            break;
        case MYSQL_TYPE_LONG:
            {
                winux::ulong v = 0;
                winux::ParseULong( data, &v );
                if ( IS_UNSIGNED_FIELD(flag) )
                {
                    m = (winux::ulong)v;
                }
                else
                {
                    m = (long)v;
                }
            }
            break;
        case MYSQL_TYPE_FLOAT:
            {
                float v = 0;
                winux::ParseFloat( data, &v );
                m = v;
            }
            break;
        case MYSQL_TYPE_DOUBLE:
            {
                double v = 0;
                winux::ParseDouble( data, &v );
                m = v;
            }
            break;
        case MYSQL_TYPE_NULL:
            m = winux::mxNull;
            break;
        case MYSQL_TYPE_TIMESTAMP: // 时间戳类型作字符串输出
            m = data;
            break;
        case MYSQL_TYPE_LONGLONG: // 64位数字
            {
                winux::uint64 v = 0;
                winux::ParseUInt64( data, &v );
                if ( IS_UNSIGNED_FIELD(flag) )
                {
                    m = (winux::uint64)v;
                }
                else
                {
                    m = (winux::int64)v;
                }
            }
            break;
        case MYSQL_TYPE_INT24:
            {
                winux::ulong v = 0;
                winux::ParseULong( data, &v );
                if ( IS_UNSIGNED_FIELD(flag) )
                {
                    m = (winux::uint)v;
                }
                else
                {
                    m = (int)v;
                }
            }
            break;
        case MYSQL_TYPE_DATE: // 日期时间类型作字符串输出
        case MYSQL_TYPE_TIME:
        case MYSQL_TYPE_DATETIME:
        case MYSQL_TYPE_YEAR:
        case MYSQL_TYPE_NEWDATE:
            m = data;
            break;
        case MYSQL_TYPE_VARCHAR: // 字符串
            m = data;
            break;
        case MYSQL_TYPE_BIT: // 位数据，暂时没有好手段，总之先作为字符串输出，总不会丢失。
            m.assign( data, len );
            break;
        case MYSQL_TYPE_NEWDECIMAL:
            m = data;
            break;
        case MYSQL_TYPE_ENUM: // 枚举和集合类型作字符串输出
        case MYSQL_TYPE_SET:
            m = data;
            break;
        case MYSQL_TYPE_TINY_BLOB: // 以下BLOB也作为字符串输出，但是是计数字符串
        case MYSQL_TYPE_MEDIUM_BLOB:
        case MYSQL_TYPE_LONG_BLOB:
        case MYSQL_TYPE_BLOB:
            m.assign( data, len );
            break;
        case MYSQL_TYPE_VAR_STRING: // 以下都作为字符串输出
        case MYSQL_TYPE_STRING:
        case MYSQL_TYPE_GEOMETRY:
            m = data;
            break;
        default:
            m = winux::mxNull;
            break;
        }
    }
}

bool MysqlResult::fetchRow( winux::Mixed * fields, int type )
{
    if ( _mysqlRes == nullptr ) throw MysqlDbError( 0, "The MySQL result set is NULL" );

    MYSQL_ROW row;
    if ( ( row = mysql_fetch_row( (MYSQL_RES *)_mysqlRes ) ) )
    {
        if ( fields != nullptr )
        {
            if ( type == 0 )
            {
                // 获取字段数
                size_t n = _fieldNames.size();
                fields->createCollection();
                winux::ulong * lens = mysql_fetch_lengths( (MYSQL_RES *)_mysqlRes );
                for ( size_t i = 0; i < n; ++i )
                {
                    __FieldValueAssignToMixed( (*fields)[ _fieldNames[i] ], _fieldTypes[i], _fieldFlags[i], row[i], lens[i] );
                }
            }
            else if ( type == 1 )
            {
                // 获取字段数
                size_t n = _fieldNames.size();
                fields->createArray(n);
                winux::ulong * lens = mysql_fetch_lengths( (MYSQL_RES *)_mysqlRes );
                for ( size_t i = 0; i < n; ++i )
                {
                    __FieldValueAssignToMixed( (*fields)[i], _fieldTypes[i], _fieldFlags[i], row[i], lens[i] );
                }
            }
        }
        return true;
    }
    return false;
}

bool MysqlResult::fetchArray( winux::MixedArray * fields )
{
    if ( _mysqlRes == nullptr ) throw MysqlDbError( 0, "The MySQL result set is NULL" );

    MYSQL_ROW row;
    if ( ( row = mysql_fetch_row( (MYSQL_RES *)_mysqlRes ) ) )
    {
        if ( fields != nullptr )
        {
            fields->clear();
            // 获取字段数
            size_t n = _fieldNames.size();
            fields->resize(n);
            winux::ulong * lens = mysql_fetch_lengths( (MYSQL_RES *)_mysqlRes );
            for ( size_t i = 0; i < n; ++i )
            {
                __FieldValueAssignToMixed( (*fields)[i], _fieldTypes[i], _fieldFlags[i], row[i], lens[i] );
            }
        }
        return true;
    }
    return false;
}

bool MysqlResult::fetchMap( winux::StringMixedMap * fields )
{
    if ( _mysqlRes == nullptr ) throw MysqlDbError( 0, "The MySQL result set is NULL" );

    MYSQL_ROW row;
    if ( ( row = mysql_fetch_row( (MYSQL_RES *)_mysqlRes ) ) )
    {
        if ( fields != nullptr )
        {
            fields->clear();
            // 获取字段数
            size_t n = _fieldNames.size();
            winux::ulong * lens = mysql_fetch_lengths( (MYSQL_RES *)_mysqlRes );
            for ( size_t i = 0; i < n; ++i )
            {
                __FieldValueAssignToMixed( (*fields)[ _fieldNames[i] ], _fieldTypes[i], _fieldFlags[i], row[i], lens[i] );
            }
        }
        return true;
    }
    return false;
}

winux::String MysqlResult::fieldName( size_t fieldIndex )
{
    if ( fieldIndex < this->fieldsCount() )
    {
        return _fieldNames[fieldIndex];
    }
    return winux::Literal<winux::String::value_type>::nulStr;
}

size_t MysqlResult::fieldsCount()
{
    return _fieldNames.size();
}

size_t MysqlResult::rowsCount()
{
    if ( _mysqlRes == nullptr ) throw MysqlDbError( 0, "The MySQL result set is NULL" );

    return (size_t)mysql_num_rows( (MYSQL_RES *)_mysqlRes );
}

static winux::String __MySqlFieldTypeStr( enum enum_field_types type )
{
    switch ( type )
    {
    case MYSQL_TYPE_DECIMAL:
        return "DECIMAL";
        break;
    case MYSQL_TYPE_TINY:
        return "TINY";
        break;
    case MYSQL_TYPE_SHORT:
        return "SHORT";
        break;
    case MYSQL_TYPE_LONG:
        return "LONG";
        break;
    case MYSQL_TYPE_FLOAT:
        return "FLOAT";
        break;
    case MYSQL_TYPE_DOUBLE:
        return "DOUBLE";
        break;
    case MYSQL_TYPE_NULL:
        return "NULL";
        break;
    case MYSQL_TYPE_TIMESTAMP:
        return "TIMESTAMP";
        break;
    case MYSQL_TYPE_LONGLONG:
        return "LONGLONG";
        break;
    case MYSQL_TYPE_INT24:
        return "INT24";
        break;
    case MYSQL_TYPE_DATE:
        return "DATE";
        break;
    case MYSQL_TYPE_TIME:
        return "TIME";
        break;
    case MYSQL_TYPE_DATETIME:
        return "DATETIME";
        break;
    case MYSQL_TYPE_YEAR:
        return "YEAR";
        break;
    case MYSQL_TYPE_NEWDATE:
        return "NEWDATE";
        break;
    case MYSQL_TYPE_VARCHAR:
        return "VARCHAR";
        break;
    case MYSQL_TYPE_BIT:
        return "BIT";
        break;
    case MYSQL_TYPE_NEWDECIMAL:
        return "NEWDECIMAL";
        break;
    case MYSQL_TYPE_ENUM:
        return "ENUM";
        break;
    case MYSQL_TYPE_SET:
        return "SET";
        break;
    case MYSQL_TYPE_TINY_BLOB:
        return "TINY_BLOB";
        break;
    case MYSQL_TYPE_MEDIUM_BLOB:
        return "MEDIUM_BLOB";
        break;
    case MYSQL_TYPE_LONG_BLOB:
        return "LONG_BLOB";
        break;
    case MYSQL_TYPE_BLOB:
        return "BLOB";
        break;
    case MYSQL_TYPE_VAR_STRING:
        return "VAR_STRING";
        break;
    case MYSQL_TYPE_STRING:
        return "STRING";
        break;
    case MYSQL_TYPE_GEOMETRY:
        return "GEOMETRY";
        break;
    default:
        return "";
        break;
    }
}

winux::String MysqlResult::fieldType( size_t fieldIndex )
{
    if ( fieldIndex < this->fieldsCount() )
    {
        return __MySqlFieldTypeStr( (enum_field_types)_fieldTypes[fieldIndex] );
    }
    return winux::Literal<winux::String::value_type>::nulStr;
}

bool MysqlResult::reset()
{
    return this->dataSeek(0);
}

// class MysqlModifier ------------------------------------------------------------------------------
MysqlModifier::MysqlModifier( MysqlConnection * cnn, winux::String const & tableName ) : DbModifier( cnn, tableName )
{
    this->_getTableInfo();
}

MysqlModifier::~MysqlModifier()
{

}

#endif // HAVE_DB_MYSQL

} // namespace eiendb
