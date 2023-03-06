
#include "eiendb_common.hpp"
#include "eiendb_mysql.hpp"

#ifdef HAVE_MYSQL_DRIVER
#include "mysql.h"
#endif

namespace eiendb
{
#include "is_x_funcs.inl"

#define TRY_LOAD_LINE( ifile, i, line ) ( i < line.length() || ( i = 0, ( ( line = ifile->getLine() ).length() || !ifile->eof() && ( line = ifile->getLine() ).length() ) ) )

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
            i++;
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
            if ( sql.empty() )
            {
                if ( IsSpace(ch) )
                {
                }
                else
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

static winux::String __MysqlLoadSql_String( winux::String const & sqlText, winux::String::size_type & i )
{
    winux::String::value_type quote = sqlText[i];
    winux::String s;

    i++; // skip quote
    s += quote;

    while ( i < sqlText.length() )
    {
        winux::String::value_type ch = sqlText[i];
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
            while ( i < sqlText.length() )
            {
                s += sqlText[i];
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

static size_t __MysqlLoadSql( winux::String const & sqlText, winux::StringArray * arrSql, IDbConnection * cnn )
{
    winux::String sql;
    size_t nSql = 0;

    winux::String::size_type i = 0;
    bool lineHeadStart = true;
    while ( i < sqlText.length() )
    {
        winux::String::value_type ch = sqlText[i];
        if ( ch == '\r' || ch == '\n' )
        {
            if ( ch == '\r' )
            {
                // 当前字符是'\r'时，检测下一个字符是否为'\n'，如果是，则当作整体换行标记
                if ( i + 1 < sqlText.length() && sqlText[i + 1] == '\n' )
                {
                    i++; // skip '\r'
                    ch = sqlText[i];
                }
            }

            // 换行标记 \r or \n or \r\n
            ++i; // skip '\r' or '\n'

            // add '\n' into sql
            if ( !sql.empty() )
            {
                sql += '\n';
            }
            lineHeadStart = true;
        }
        else if ( lineHeadStart && ch == '-' ) // 一行开头
        {
            lineHeadStart = false;
            i++; // skip first '-'
            if ( i < sqlText.length() )
            {
                ch = sqlText[i];
                if ( ch == '-' ) // "--"开头时作为注释行跳过
                {
                    ++i; // skip second '-'
                    // skip a line
                    while ( i < sqlText.length() )
                    {
                        if ( sqlText[i] == '\r' || sqlText[i] == '\n' )
                        {
                            break;
                        }
                        ++i;
                    }
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
            lineHeadStart = false;
            sql += __MysqlLoadSql_String( sqlText, i );
        }
        else if ( ch == ';' )
        {
            lineHeadStart = false;
            sql += ch;
            arrSql->push_back(sql);
            sql.clear();
            nSql++;
            i++;
        }
        else
        {
            lineHeadStart = false;
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
static inline winux::String HtmlCharsetToMySqlCharset( winux::String const & charset )
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
MysqlConnection::MysqlConnection( winux::String const & host, winux::String const & user, winux::String const & pwd, winux::String const & dbName, winux::String const & linkCharset, bool doConnect )
: _mysql(nullptr), _port(0), _user(user), _pwd(pwd), _dbName(dbName), _linkCharset(linkCharset)
{
    // 解析端口号
    winux::String::size_type pos = host.find(':');
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
    MYSQL * cnn = mysql_real_connect( _mysql, _hostAddr.c_str(), _user.c_str(), _pwd.c_str(), _dbName.c_str(), _port, NULL, 0 );
    if ( _mysql != cnn ) // 连接失败
    {
        winux::String err = this->error();
        int errNo = this->errNo();
        (void)errNo;
        this->close();
        throw MysqlDbError( errNo, err.c_str() );
        return false;
    }
    // 设置连接校验编码
    this->setLinkCharset(_linkCharset);
    return true;
}

bool MysqlConnection::close()
{
    if ( _mysql != nullptr )
    {
        mysql_close(_mysql);
        _mysql = nullptr;
        return true;
    }
    return false;
}

void MysqlConnection::alive()
{
    if ( _mysql != nullptr )
    {
        int rc = mysql_ping(_mysql);
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
        return 0 == mysql_set_character_set( _mysql, HtmlCharsetToMySqlCharset(_linkCharset).c_str() );
    }
    return false;
}

bool MysqlConnection::selectDb( winux::String const & database )
{
    _dbName = database;
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    return 0 == mysql_select_db( _mysql, _dbName.c_str() );
}

bool MysqlConnection::createDb( winux::String const & database )
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    winux::String sql = "CREATE DATABASE " + this->symbolQuotes(database);
    if ( !_linkCharset.empty() ) sql += " DEFAULT CHARSET=" + HtmlCharsetToMySqlCharset(_linkCharset);
    {
    }
    if ( mysql_real_query( _mysql, sql.c_str(), (winux::ulong)sql.length() ) )
        return false;
    return this->selectDb(database);
}

bool MysqlConnection::dropDb( winux::String const & database )
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    winux::String sql = "DROP DATABASE " + this->symbolQuotes(database);
    return 0 == mysql_real_query( _mysql, sql.c_str(), (winux::ulong)sql.length() );
}

size_t MysqlConnection::affectedRows()
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    return (winux::ulong)mysql_affected_rows(_mysql);
}

size_t MysqlConnection::insertId()
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    return (winux::ulong)mysql_insert_id(_mysql);
}

int MysqlConnection::errNo()
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    return mysql_errno(_mysql);
}

winux::String MysqlConnection::error()
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    char const * s = mysql_error(_mysql);
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
        r.resize( size * 2 );
        mysql_real_escape_string( _mysql, &r[0], (char const *)buf, (winux::ulong)size );
    }
    else
    {
        r = "";
    }

    if ( !addQuote.empty() )
    {
        return addQuote + r.c_str() + addQuote;
    }

    return r.c_str();
}

bool MysqlConnection::exec( winux::String const & sql )
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    int rc = mysql_real_query( _mysql, sql.c_str(), (winux::ulong)sql.length() );
    if ( rc ) // 语句出错
    {
        throw MysqlDbError( this->errNo(), this->error().c_str() );
        return false;
    }
    else
    {
        return true;
    }
}

bool MysqlConnection::exec( winux::SharedPointer<IDbStatement> stmt )
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    MysqlStatement * pStmt = static_cast<MysqlStatement *>( stmt.get() );
    return this->exec( pStmt->getSql() );
}

winux::SharedPointer<IDbResult> MysqlConnection::query( winux::String const & sql )
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    return winux::SharedPointer<IDbResult>( new MysqlResult( winux::SharedPointer<MysqlStatement>( new MysqlStatement( this, sql ) ) ) );
}

winux::SharedPointer<IDbResult> MysqlConnection::query( winux::SharedPointer<IDbStatement> stmt )
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    winux::SharedPointer<MysqlStatement> stmt2 = stmt.ensureCast<MysqlStatement>();
    return winux::SharedPointer<IDbResult>( new MysqlResult(stmt2) );
}

winux::SharedPointer<IDbResult> MysqlConnection::listDbs()
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    winux::String fields[] = { "Databases" };
    MemoryResult * p = new MemoryResult(fields);

    MYSQL_RES * res = mysql_list_dbs( _mysql, NULL );
    if ( res != nullptr )
    {
        MYSQL_ROW row;
        while ( ( row = mysql_fetch_row(res) ) )
        {
            winux::String f[] = { row[0] ? row[0] : "" };
            p->addRow(f);
        }
        mysql_free_result(res);
    }

    return winux::SharedPointer<IDbResult>(p);
}

winux::SharedPointer<IDbResult> MysqlConnection::listFields( winux::String const & tableName )
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    winux::String fields[] = { "Fields" };
    MemoryResult * p = new MemoryResult(fields);

    winux::SharedPointer<IDbResult> rs = this->query( "SHOW COLUMNS FROM " + this->symbolQuotes(tableName) );
    winux::MixedArray f;
    while ( rs->fetchRow(&f) )
    {
        winux::String ff[] = { (winux::String)f[0] };
        p->addRow(ff);
    }

    return winux::SharedPointer<IDbResult>(p);
}

winux::SharedPointer<IDbResult> MysqlConnection::listTables()
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    winux::String fields[] = { "Tables" };
    MemoryResult * p = new MemoryResult(fields);

    winux::SharedPointer<IDbResult> rs = this->query( "SHOW FULL TABLES WHERE Table_type='BASE TABLE'" );
    winux::MixedArray f;
    while ( rs->fetchRow(&f) )
    {
        winux::String ff[] = { (winux::String)f[0] };
        p->addRow(ff);
    }

    return winux::SharedPointer<IDbResult>(p);
}

winux::String MysqlConnection::tableDdl( winux::String const & tableName )
{
    winux::SharedPointer<IDbResult> rs = this->query( "SHOW CREATE TABLE " + this->symbolQuotes(tableName) );
    winux::String ddl;
    winux::MixedArray f;
    while ( rs->fetchRow(&f) )
    {
        ddl += (winux::String)f[1];
        ddl += ";\n";
    }
    return ddl;
}

winux::String MysqlConnection::symbolQuotes( winux::String const & str )
{
    return "`" + str + "`";
}

size_t MysqlConnection::loadSql( winux::String const & sqlText, winux::StringArray * arrSql )
{
    return __MysqlLoadSql( sqlText, arrSql, this );
}

size_t MysqlConnection::loadSqlFile( winux::IFile * sqlScriptFile, winux::StringArray * arrSql )
{
    return __MysqlLoadSqlFile( sqlScriptFile, arrSql, this );
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
    while ( rs->fetchRow(&f) )
    {
        arrKeyColumn->push_back(f[0]);
    }

    return arrKeyColumn->size();
}

winux::SharedPointer<IDbStatement> MysqlConnection::buildStmt( winux::String const & sql )
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    return winux::SharedPointer<IDbStatement>( new MysqlStatement( this, sql ) );
}

winux::SharedPointer<IDbStatement> MysqlConnection::buildStmt( winux::String const & sql, winux::Mixed const & params )
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    return winux::SharedPointer<IDbStatement>( new MysqlStatement( this, sql, params ) );
}

winux::SharedPointer<IDbStatement> MysqlConnection::buildStmt( winux::String const & sql, winux::MixedArray const & params )
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    return winux::SharedPointer<IDbStatement>( new MysqlStatement( this, sql, params ) );
}

winux::SharedPointer<IDbStatement> MysqlConnection::buildStmt( winux::String const & sql, winux::StringMixedMap const & params )
{
    if ( _mysql == nullptr ) throw MysqlDbError( 0, "MySQL no valid connection" );
    return winux::SharedPointer<IDbStatement>( new MysqlStatement( this, sql, params ) );
}

// class MysqlStatement ------------------------------------------------------------------------------------
MysqlStatement::MysqlStatement( MysqlConnection * cnn ) : _cnn(cnn), _isGeneratedSql(false), _isNameBinding(false)
{

}

MysqlStatement::MysqlStatement( MysqlConnection * cnn, winux::String const & sql ) : _cnn(cnn), _isGeneratedSql(false), _isNameBinding(false)
{
    this->build(sql);
}

MysqlStatement::MysqlStatement( MysqlConnection * cnn, winux::String const & sql, winux::Mixed const & params ) : _cnn(cnn), _isGeneratedSql(false), _isNameBinding(false)
{
    this->build( sql, params );
}

MysqlStatement::MysqlStatement( MysqlConnection * cnn, winux::String const & sql, winux::MixedArray const & params ) : _cnn(cnn), _isGeneratedSql(false), _isNameBinding(false)
{
    this->build( sql, params );
}

MysqlStatement::MysqlStatement( MysqlConnection * cnn, winux::String const & sql, winux::StringMixedMap const & params ) : _cnn(cnn), _isGeneratedSql(false), _isNameBinding(false)
{
    this->build( sql, params );
}

MysqlStatement::~MysqlStatement()
{

}

bool MysqlStatement::build( winux::String const & sql )
{
    if ( !*_cnn ) throw MysqlDbError( 0, "MySQL no valid connection" );

    this->_orgSql = sql;
    this->_generateSql();
    return true;
}

bool MysqlStatement::build( winux::String const & sql, winux::Mixed const & params )
{
    if ( !*_cnn ) throw MysqlDbError( 0, "MySQL no valid connection" );

    this->_orgSql = sql;

    bool r = true;
    if ( params.isArray() )
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
            winux::Mixed::MixedMixedMap::value_type const & pr = params.getPair(i);
            bool bindOk = this->bind( (winux::String)pr.first, pr.second );
            if ( r && !bindOk ) r = false;
        }
    }
    else //只是一个值，作为bind(1,val)处理
    {
        bool bindOk = this->bind( 1, params );
        if ( r && !bindOk ) r = false;
    }

    this->_generateSql();
    return r;

}

bool MysqlStatement::build( winux::String const & sql, winux::MixedArray const & params )
{
    if ( !*_cnn ) throw MysqlDbError( 0, "MySQL no valid connection" );

    this->_orgSql = sql;

    size_t i;
    bool r = true;
    for ( i = 0; i < params.size(); ++i )
    {
        bool bindOk = this->bind( i + 1, params[i] );
        if ( r && !bindOk ) r = false;
    }
    this->_generateSql();
    return r;
}

bool MysqlStatement::build( winux::String const & sql, winux::StringMixedMap const & params )
{
    if ( !*_cnn ) throw MysqlDbError( 0, "MySQL no valid connection" );

    this->_orgSql = sql;

    winux::StringMixedMap::const_iterator it;
    bool r = true;
    for ( it = params.begin(); it != params.end(); ++it )
    {
        bool bindOk = this->bind( it->first, it->second );
        if ( r && !bindOk ) r = false;
    }
    this->_generateSql();
    return r;
}

bool MysqlStatement::bind( size_t paramIndex, winux::Mixed const & val )
{
    this->_isNameBinding = false;
    // 把绑定的参数记下（参数索引，参数值）,以便之后语句更改重绑定
    bool isFound = false;
    std::vector< std::pair< size_t, winux::Mixed > >::iterator it;
    for ( it = _bindingParams.begin(); it != _bindingParams.end(); ++it )
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
        _bindingParams.push_back( std::pair< size_t, winux::Mixed >( paramIndex, val ) );
    }

    _isGeneratedSql = false;
    return true;
}

bool MysqlStatement::bind( winux::String const & paramName, winux::Mixed const & val )
{
    this->_isNameBinding = true;
    bool isFound = false;
    std::vector< std::pair< winux::String, winux::Mixed > >::iterator it;
    for ( it = _bindingNameParams.begin(); it != _bindingNameParams.end(); ++it )
    {
        if ( it->first == paramName )
        {
            isFound = true;
            it->second = val;
            break;
        }
    }
    if ( !isFound )
    {
        _bindingNameParams.push_back( std::pair< winux::String, winux::Mixed >( paramName, val ) );
    }

    _isGeneratedSql = false;
    return true;
}

winux::String const & MysqlStatement::getSql()
{
    this->_generateSql();
    return _sql;
}

static void __IndexParamGenerateSql( winux::String const & sql, winux::String * outSql, MysqlStatement * stmt )
{
    outSql->clear();
    winux::String::value_type symbolQuote = stmt->getCnn()->symbolQuotes("")[0];
    size_t i = 0, start = 0;
    size_t paramIndex = 1;
    while ( i < sql.length() )
    {
        winux::String::value_type ch = sql[i];
        if ( ch == '\'' || ch == '\"' ) // 字符串
        {
            winux::String::value_type quote = ch;
            ++i; // skip ' or "
            while ( i < sql.length() )
            {
                ch = sql[i];
                if ( ch == '\\' ) // 转义字符
                {
                    ++i; // skip '\\'
                    while ( i < sql.length() )
                    {
                        ++i;
                        break;
                    }
                }
                else if ( ch == quote ) // 结束字符串
                {
                    ++i;
                    break;
                }
                else
                {
                    ++i;
                }
            }
        }
        else if ( ch == symbolQuote ) // 标识符
        {
            ++i;
            // skip `
            while ( i < sql.length() )
            {
                ch = sql[i];
                if ( ch == symbolQuote ) // 结束标识符
                {
                    ++i;
                    break;
                }
                else
                {
                    ++i;
                }
            }
        }
        else if ( ch == '?' ) // 参数占位符
        {
            *outSql += sql.substr( start, i - start );
            *outSql += MixedToEscapedStr( stmt->getParam(paramIndex), stmt->getCnn() );
            ++paramIndex;
            start = i + 1;
            ++i;
        }
        else
        {
            ++i;
        }
    }
    *outSql += sql.substr(start);
}

static void __NameParamGenerateSql( winux::String const & sql, winux::String * outSql, MysqlStatement * stmt )
{
    outSql->clear();
    winux::String::value_type symbolQuote = stmt->getCnn()->symbolQuotes("")[0];
    size_t i = 0, start = 0;
    while ( i < sql.length() )
    {
        winux::String::value_type ch = sql[i];
        if ( ch == '\'' || ch == '\"' ) // 字符串
        {
            winux::String::value_type quote = ch;
            ++i; // skip ' or "
            while ( i < sql.length() )
            {
                ch = sql[i];
                if ( ch == '\\' ) // 转义字符
                {
                    ++i; // skip '\\'
                    while ( i < sql.length() )
                    {
                        ++i;
                        break;
                    }
                }
                else if ( ch == quote ) // 结束字符串
                {
                    ++i;
                    break;
                }
                else
                {
                    ++i;
                }
            }
        }
        else if ( ch == symbolQuote ) // 标识符
        {
            ++i;
            // skip `
            while ( i < sql.length() )
            {
                ch = sql[i];
                if ( ch == symbolQuote ) // 结束标识符
                {
                    ++i;
                    break;
                }
                else
                {
                    ++i;
                }
            }
        }
        else if ( ch == ':' ) // 参数占位串
        {
            size_t pos = i; // 占位串开始位置
            ++i; // skip :
            winux::String paramName = ":";
            while ( i < sql.length() )
            {
                ch = sql[i];
                if ( IsWordNoDollar(ch) || IsDigit(ch) ) // 是否为合法标识符字符
                {
                    paramName += ch;
                    ++i;
                }
                else
                {
                    break;
                }
            }

            *outSql += sql.substr( start, pos - start );
            *outSql += MixedToEscapedStr( stmt->getParam(paramName), stmt->getCnn() );

            start = i;
            ++i;
        }
        else
        {
            ++i;
        }
    }
    *outSql += sql.substr(start);
}

void MysqlStatement::_generateSql()
{
    if ( !_isGeneratedSql )
    {
        if ( _isNameBinding )
        {
            __NameParamGenerateSql( _orgSql, &_sql, this );
        }
        else
        {
            __IndexParamGenerateSql( _orgSql, &_sql, this );
        }
        _isGeneratedSql = true;
    }

}

winux::Mixed const & MysqlStatement::getParam( size_t paramIndex ) const
{
    thread_local winux::Mixed undefined;
    std::vector< std::pair< size_t, winux::Mixed > >::const_iterator it;
    for ( it = _bindingParams.begin(); it != _bindingParams.end(); ++it )
    {
        if ( paramIndex == it->first )
        {
            return it->second;
        }
    }
    return undefined;
}

winux::Mixed const & MysqlStatement::getParam( winux::String const & paramName ) const
{
    thread_local winux::Mixed undefined;
    std::vector< std::pair< winux::String, winux::Mixed > >::const_iterator it;
    for ( it = _bindingNameParams.begin(); it != _bindingNameParams.end(); ++it )
    {
        if ( paramName == it->first )
        {
            return it->second;
        }
    }
    return undefined;
}

// class MysqlResult ----------------------------------------------------------------------------------------
MysqlResult::MysqlResult( winux::SharedPointer<MysqlStatement> stmt ) : _mysqlRes(NULL), _stmt(stmt)
{
    if ( !_stmt || !*_stmt.get() ) throw MysqlDbError( 0, "Invalid statement object" );

    // 执行mysql_real_query()
    winux::String sql = _stmt->getSql();
    int rc = mysql_real_query( *_stmt->getCnn(), sql.c_str(), (winux::ulong)sql.length() );
    // query()执行后3种情况，a. sql错误，执行错误。b. 执行无错，没有数据。c. 执行无错，有数据。
    if ( rc == 0 ) //执行无错
    {
        winux::uint n = mysql_field_count( *_stmt->getCnn() );
        if ( n > 0 ) // 有结果集
        {
            _mysqlRes = mysql_store_result( *_stmt->getCnn() );
            if ( _mysqlRes != nullptr )
            {
                // 取得字段名，类型
                winux::uint i;
                for ( i = 0; i < n; ++i )
                {
                    MYSQL_FIELD * field = mysql_fetch_field_direct( _mysqlRes, i );
                    _fieldNames.push_back(field->name);
                    _fieldTypes.push_back( (winux::uint)field->type );
                    _fieldFlags.push_back(field->flags);
                }
            }
        }
        //else // 没有结果集,可能执行的是UPDATE INSERT ALTER等语句
        //{
        //}
    }
    else
    {
        throw MysqlDbError( _stmt->getCnn()->errNo(), _stmt->getCnn()->error().c_str() );
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
        mysql_free_result(_mysqlRes);
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

    mysql_data_seek( _mysqlRes, index );
    if ( cnn->errNo() )
    {
        return false;
    }
    return true;
}

#define IS_UNSIGNED_FIELD(f) ( ((f) & BinVal(00100000)) != 0 )

// 把字段值赋给winux::Mixed,当前必须能获取数据行才能调用
inline static void __FieldValueAssignToMixed( winux::Mixed & m, winux::uint type, winux::uint flag, char const * data, winux::ulong len )
{
    if ( data == NULL && len == 0 )//数据是NULL值
    {
        m = winux::Mixed();
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
                winux::Mixed::ParseULong( data, &v );
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
                winux::Mixed::ParseULong( data, &v );
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
                winux::Mixed::ParseULong( data, &v );
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
                double v = 0;
                winux::Mixed::ParseDouble( data, &v );
                m = (float)v;
            }
            break;
        case MYSQL_TYPE_DOUBLE:
            {
                double v = 0;
                winux::Mixed::ParseDouble( data, &v );
                m = v;
            }
            break;
        case MYSQL_TYPE_NULL:
            m = winux::Mixed();
            break;
        case MYSQL_TYPE_TIMESTAMP: // 时间戳类型作字符串输出
            m = data;
            break;
        case MYSQL_TYPE_LONGLONG: // 64位数字
            {
                winux::uint64 v = 0;
                winux::Mixed::ParseUInt64( data, &v );
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
                winux::Mixed::ParseULong( data, &v );
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
            m = winux::Mixed();
            break;
        }
    }

}

bool MysqlResult::fetchRow( winux::MixedArray * fields )
{
    if ( _mysqlRes == nullptr ) throw MysqlDbError( 0, "The MySQL result set is NULL" );

    MYSQL_ROW row;
    if ( ( row = mysql_fetch_row(_mysqlRes) ) )
    {
        if ( fields != nullptr )
        {
            fields->clear();
            // 获取字段数
            size_t n = _fieldNames.size();
            fields->resize(n);
            winux::ulong * lens = mysql_fetch_lengths(_mysqlRes);
            size_t i;
            for ( i = 0; i < n; ++i )
            {
                __FieldValueAssignToMixed( (*fields)[i], _fieldTypes[i], _fieldFlags[i], row[i], lens[i] );
            }
        }
        return true;
    }
    return false;
}

bool MysqlResult::fetchRow( winux::StringMixedMap * fields )
{
    if ( _mysqlRes == nullptr ) throw MysqlDbError( 0, "The MySQL result set is NULL" );

    MYSQL_ROW row;
    if ( ( row = mysql_fetch_row(_mysqlRes) ) )
    {
        if ( fields != nullptr )
        {
            fields->clear();
            // 获取字段数
            size_t n = _fieldNames.size();
            winux::ulong * lens = mysql_fetch_lengths(_mysqlRes);
            size_t i;
            for ( i = 0; i < n; ++i )
            {
                __FieldValueAssignToMixed( (*fields)[ _fieldNames[i] ], _fieldTypes[i], _fieldFlags[i], row[i], lens[i] );
            }
        }
        return true;
    }
    return false;
}

bool MysqlResult::fetchRow( winux::Mixed * fields, int type )
{
    if ( _mysqlRes == nullptr ) throw MysqlDbError( 0, "The MySQL result set is NULL" );

    MYSQL_ROW row;
    if ( ( row = mysql_fetch_row(_mysqlRes) ) )
    {
        if ( fields != nullptr )
        {
            if ( type == 0 )
            {
                // 获取字段数
                size_t n = _fieldNames.size();
                fields->createCollection();
                winux::ulong * lens = mysql_fetch_lengths(_mysqlRes);
                size_t i;
                for ( i = 0; i < n; ++i )
                {
                    __FieldValueAssignToMixed( fields->operator [] (_fieldNames[i]), _fieldTypes[i], _fieldFlags[i], row[i], lens[i] );
                }
            }
            else if ( type == 1 )
            {
                // 获取字段数
                size_t n = _fieldNames.size();
                fields->createArray(n);
                winux::ulong * lens = mysql_fetch_lengths(_mysqlRes);
                size_t i;
                for ( i = 0; i < n; ++i )
                {
                    __FieldValueAssignToMixed( fields->operator [] (i), _fieldTypes[i], _fieldFlags[i], row[i], lens[i] );
                }
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
    return winux::Literal<winux::String::value_type>::emptyStr;
}

size_t MysqlResult::fieldsCount()
{
    return _fieldNames.size();
}

size_t MysqlResult::rowsCount()
{
    if ( _mysqlRes == nullptr ) throw MysqlDbError( 0, "The MySQL result set is NULL" );

    return (size_t)mysql_num_rows(_mysqlRes);
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
    return winux::Literal<winux::String::value_type>::emptyStr;
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

void MysqlModifier::_getTableInfo()
{
    if ( !_cnn || !*_cnn ) throw MysqlDbError( 0, "MySQL no valid connection" );

    // DESCRIBE `table1`;
    winux::SharedPointer<IDbStatement> stmt = _cnn->buildStmt( "DESCRIBE " + _cnn->symbolQuotes(_tableName) );
    winux::SharedPointer<MysqlStatement> stmt2 = stmt.ensureCast<MysqlStatement>();

    MysqlResult rs(stmt2);
    winux::MixedArray row;
    //bool isHasPrimaryKey = false;
    while ( rs.fetchRow(&row) )
    {
        _fieldNames.push_back(row[0]);
/*
        if ( !isHasPrimaryKey && (winux::AnsiString)(row[3]) == "PRI" )
        {
            _prkName = (winux::String)row[0];
            isHasPrimaryKey = true;
        }*/
    }

    _cnn->getPrimaryKey( _tableName, &_prkColumn );
}

}
