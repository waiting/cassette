#include "eiendb_base.hpp"

namespace eiendb
{
#include "is_x_funcs.inl"

// interface IDbConnection ------------------------------------------------------------------
bool IDbConnection::connect()
{
    throw DbError( DbError::dbeNotImplemented, "`bool connect()` is not implemented" );
}

bool IDbConnection::close()
{
    throw DbError( DbError::dbeNotImplemented, "`bool close()` is not implemented" );
}

void IDbConnection::alive()
{
    throw DbError( DbError::dbeNotImplemented, "`void alive()` is not implemented" );
}

bool IDbConnection::selectDb( winux::String const & database )
{
    throw DbError( DbError::dbeNotImplemented, "`bool selectDB( String const & database )` is not implemented" );
}

bool IDbConnection::setLinkCharset( winux::String const & charset )
{
    throw DbError( DbError::dbeNotImplemented, "`bool setLinkCharset( String const & charset )` is not implemented" );
}

bool IDbConnection::createDb( winux::String const & database )
{
    throw DbError( DbError::dbeNotImplemented, "`bool createDb( String const & database )` is not implemented" );
}

bool IDbConnection::dropDb( winux::String const & database )
{
    throw DbError( DbError::dbeNotImplemented, "`bool dropDb( String const & database )` is not implemented" );
}

size_t IDbConnection::affectedRows()
{
    throw DbError( DbError::dbeNotImplemented, "`size_t affectedRows()` is not implemented" );
}

size_t IDbConnection::insertId()
{
    throw DbError( DbError::dbeNotImplemented, "`size_t insertId()` is not implemented" );
}

int IDbConnection::errNo()
{
    throw DbError( DbError::dbeNotImplemented, "`int errNo()` is not implemented" );
}

winux::String IDbConnection::error()
{
    throw DbError( DbError::dbeNotImplemented, "`String error()` is not implemented" );
}

winux::SharedPointer<IDbResult> IDbConnection::query( winux::String const & sql )
{
    throw DbError( DbError::dbeNotImplemented, "`SharedPointer<IDbResult> query( String const & sql )` is not implemented" );
}

winux::SharedPointer<IDbResult> IDbConnection::query( winux::SharedPointer<IDbStatement> stmt )
{
    throw DbError( DbError::dbeNotImplemented, "`SharedPointer<IDbResult> query( SharedPointer<IDbStatement> stmt )` is not implemented" );
}

bool IDbConnection::exec( winux::String const & sql )
{
    throw DbError( DbError::dbeNotImplemented, "`bool exec( String const & sql )` is not implemented" );
}

bool IDbConnection::exec( winux::SharedPointer<IDbStatement> stmt )
{
    throw DbError( DbError::dbeNotImplemented, "`bool exec( SharedPointer<IDbStatement> stmt )` is not implemented" );
}

winux::String IDbConnection::escape( winux::String const & str, winux::String const & addQuote )
{
    throw DbError( DbError::dbeNotImplemented, "`String escape( String const & str, String const & addQuote )` is not implemented" );
}

winux::String IDbConnection::escape( void const * buf, size_t size, winux::String const & addQuote )
{
    throw DbError( DbError::dbeNotImplemented, "`String escape( void const * buf, size_t size, String const & addQuote )` is not implemented" );
}

winux::SharedPointer<IDbResult> IDbConnection::listDbs()
{
    throw DbError( DbError::dbeNotImplemented, "`SharedPointer<IDbResult> listDbs()` is not implemented" );
}

winux::SharedPointer<IDbResult> IDbConnection::listFields( winux::String const & tableName )
{
    throw DbError( DbError::dbeNotImplemented, "`SharedPointer<IDbResult> listFields( String const & tableName )` is not implemented" );
}

winux::SharedPointer<IDbResult> IDbConnection::listTables()
{
    throw DbError( DbError::dbeNotImplemented, "`SharedPointer<IDbResult> listTables()` is not implemented" );
}

winux::String IDbConnection::tableDdl( winux::String const & tableName )
{
    throw DbError( DbError::dbeNotImplemented, "`String tableDdl( String const & tableName )` is not implemented" );
}

winux::String IDbConnection::symbolQuotes( winux::String const & str, bool periodAsSeparator )
{
    throw DbError( DbError::dbeNotImplemented, "`String symbolQuotes( String const & str )` is not implemented" );
}

size_t IDbConnection::loadSqlFile( winux::IFile * sqlFile, winux::StringArray * arrSql )
{
    throw DbError( DbError::dbeNotImplemented, "`size_t loadSqlFile( IFile * sqlScriptFile, StringArray * arrSql )` is not implemented" );
}

size_t IDbConnection::getPrimaryKey( winux::String const & tableName, winux::StringArray * arrKeyColumn )
{
    throw DbError( DbError::dbeNotImplemented, "`size_t getPrimaryKey( String const & tableName, StringArray * arrKeyColumn )` is not implemented" );
}

winux::SharedPointer<IDbStatement> IDbConnection::buildStmt( winux::String const & sql, winux::Mixed const & params )
{
    throw DbError( DbError::dbeNotImplemented, "`SharedPointer<IDbStatement> buildStmt( String const & sql, Mixed const & params )` is not implemented" );
}

IDbConnection::operator bool() const
{
    throw DbError( DbError::dbeNotImplemented, "`operator bool()` is not implemented" );
}

// interface IDbStatement -------------------------------------------------------------------
bool IDbStatement::build( winux::String const & sql, winux::Mixed const & params )
{
    throw DbError( DbError::dbeNotImplemented, "`bool build( String const & sql, Mixed const & params )` is not implemented" );
}

bool IDbStatement::bind( size_t paramIndex, winux::Mixed const & val )
{
    throw DbError( DbError::dbeNotImplemented, "`bool bind( size_t paramIndex, Mixed const & val )` is not implemented" );
}

bool IDbStatement::bind( winux::String const & paramName, winux::Mixed const & val )
{
    throw DbError( DbError::dbeNotImplemented, "`bool bind( String const & paramName, Mixed const & val )` is not implemented" );
}

// interface IDbResult ----------------------------------------------------------------------
bool IDbResult::free()
{
    throw DbError( DbError::dbeNotImplemented, "`bool free()` is not implemented" );
}

bool IDbResult::dataSeek( size_t index )
{
    throw DbError( DbError::dbeNotImplemented, "`bool dataSeek( size_t index )` is not implemented" );
}

bool IDbResult::fetchRow( winux::Mixed * fields, int type )
{
    throw DbError( DbError::dbeNotImplemented, "`bool fetchRow( Mixed * fields, int type )` is not implemented" );
}

bool IDbResult::fetchArray( winux::MixedArray * fields )
{
    throw DbError( DbError::dbeNotImplemented, "`bool fetchRow( MixedArray * fields )` is not implemented" );
}

bool IDbResult::fetchMap( winux::StringMixedMap * fields )
{
    throw DbError( DbError::dbeNotImplemented, "`bool fetchRow( StringMixedMap * fields )` is not implemented" );
}

winux::String IDbResult::fieldName( size_t fieldIndex )
{
    throw DbError( DbError::dbeNotImplemented, "`String fieldName( size_t fieldIndex )` is not implemented" );
}

size_t IDbResult::fieldsCount()
{
    throw DbError( DbError::dbeNotImplemented, "`size_t fieldsCount()` is not implemented" );
}

size_t IDbResult::rowsCount()
{
    throw DbError( DbError::dbeNotImplemented, "`size_t rowsCount()` is not implemented" );
}

winux::String IDbResult::fieldType( size_t fieldIndex )
{
    throw DbError( DbError::dbeNotImplemented, "`String fieldType( size_t fieldIndex )` is not implemented" );
}

// interface IDbModifier --------------------------------------------------------------------
bool IDbModifier::addNew( winux::StringArray const & fieldNames, winux::MixedArray const & values )
{
    throw DbError( DbError::dbeNotImplemented,"`bool addNew( StringArray const & fieldNames, MixedArray const & values )` is not implemented" );
}

bool IDbModifier::addNew( winux::Mixed const & fields )
{
    throw DbError( DbError::dbeNotImplemented, "`bool addNew( Mixed const & fields )` is not implemented" );
}

bool IDbModifier::modify( winux::StringArray const & fieldNames, winux::MixedArray const & values, winux::Mixed const & prkValue )
{
    throw DbError( DbError::dbeNotImplemented, "`bool modify( StringArray const & fieldNames, MixedArray const & values, Mixed const & prkValue )` is not implemented" );
}

bool IDbModifier::modify( winux::Mixed const & fields, winux::Mixed const & prkValue )
{
    throw DbError( DbError::dbeNotImplemented, "`bool modify( Mixed const & fields, Mixed const & prkValue )` is not implemented" );
}

bool IDbModifier::modifyEx( winux::StringArray const & fieldNames, winux::MixedArray const & values, winux::String const & where )
{
    throw DbError( DbError::dbeNotImplemented, "`bool modifyEx( StringArray const & fieldNames, MixedArray const & values, String const & where )` is not implemented" );
}

bool IDbModifier::modifyEx( winux::Mixed const & fields, winux::String const & where )
{
    throw DbError( DbError::dbeNotImplemented, "`bool modifyEx( Mixed const & fields, String const & where )` is not implemented" );
}

bool IDbModifier::deleteOne( winux::Mixed const & prkValue )
{
    throw DbError( DbError::dbeNotImplemented, "`bool deleteOne( Mixed const & prkValue )` is not implemented" );
}

bool IDbModifier::deleteEx( winux::String const & where )
{
    throw DbError( DbError::dbeNotImplemented, "`bool deleteEx( String const & where )` is not implemented" );
}

// ------------------------------------------------------------------------------------------
EIENDB_FUNC_IMPL(winux::String) MixedToEscapedStr( winux::Mixed const & m, IDbConnection * cnn, winux::String const & addQuote )
{
    switch ( m._type )
    {
    case winux::Mixed::MT_NULL:
        return "null";
    case winux::Mixed::MT_BOOLEAN:
        return m._boolVal ? "true" : "false";
    case winux::Mixed::MT_CHAR:
        {
            char sz[12] = { 0 };
            sprintf( sz, "%d", (int)m._chVal );
            return sz;
        }
    case winux::Mixed::MT_BYTE:
        {
            char sz[12] = { 0 };
            sprintf( sz, "%u", (winux::uint)m._btVal );
            return sz;
        }
    case winux::Mixed::MT_SHORT:
    case winux::Mixed::MT_USHORT:
    case winux::Mixed::MT_INT:
    case winux::Mixed::MT_UINT:
    case winux::Mixed::MT_LONG:
    case winux::Mixed::MT_ULONG:
    case winux::Mixed::MT_FLOAT:
    case winux::Mixed::MT_INT64:
    case winux::Mixed::MT_UINT64:
    case winux::Mixed::MT_DOUBLE:
        return m;
    case winux::Mixed::MT_ANSI:
    case winux::Mixed::MT_UNICODE:
    case winux::Mixed::MT_ARRAY:
    case winux::Mixed::MT_COLLECTION:
        return cnn->escape( m, addQuote );
    case winux::Mixed::MT_BINARY:
        return cnn->escape( (winux::byte *)m.getBuf(), m.getSize(), addQuote );
    default:
        return "null";
    }
}

// class DbStatement ------------------------------------------------------------------------
DbStatement::DbStatement( IDbConnection * cnn ) : _cnn(cnn), _isGeneratedSql(false), _isNameBinding(false)
{

}

DbStatement::DbStatement( IDbConnection * cnn, winux::String const & sql, winux::Mixed const & params ) : _cnn(cnn), _isGeneratedSql(false), _isNameBinding(false)
{
    this->build( sql, params );
}

bool DbStatement::build( winux::String const & sql, winux::Mixed const & params )
{
    if ( !*_cnn ) throw DbError( DbError::dbeInvalidDbConn, "Invalid database connection" );

    this->_orgSql = sql;

    bool r = true;
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

    this->_generateSql();
    return r;
}

// 绑定参数 按参数索引 参数索引起始为1
bool DbStatement::bind( size_t paramIndex, winux::Mixed const & val )
{
    this->_isNameBinding = false;
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

    _isGeneratedSql = false;
    return true;
}

// 绑定参数 按参数名 参数名前需加':'
bool DbStatement::bind( winux::String const & paramName, winux::Mixed const & val )
{
    this->_isNameBinding = true;
    bool isFound = false;
    for ( auto it = _bindingNameParams.begin(); it != _bindingNameParams.end(); ++it )
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
        _bindingNameParams.push_back( std::make_pair( paramName, val ) );
    }

    _isGeneratedSql = false;
    return true;
}

winux::String const & DbStatement::getSql()
{
    this->_generateSql();
    return _sql;
}

static void __IndexParamGenerateSql( winux::String const & sql, winux::String * outSql, DbStatement * stmt )
{
    outSql->clear();
    winux::String::value_type symbolQuote = stmt->getCnn()->symbolQuotes( "", false )[0];
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

static void __NameParamGenerateSql( winux::String const & sql, winux::String * outSql, DbStatement * stmt )
{
    outSql->clear();
    winux::String::value_type symbolQuote = stmt->getCnn()->symbolQuotes( "", false )[0];
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

void DbStatement::_generateSql()
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

winux::Mixed const & DbStatement::getParam( size_t paramIndex ) const
{
    thread_local winux::Mixed undefined;
    for ( auto it = _bindingParams.begin(); it != _bindingParams.end(); ++it )
    {
        if ( paramIndex == it->first )
        {
            return it->second;
        }
    }
    return undefined;
}

winux::Mixed const & DbStatement::getParam( winux::String const & paramName ) const
{
    thread_local winux::Mixed undefined;
    for ( auto it = _bindingNameParams.begin(); it != _bindingNameParams.end(); ++it )
    {
        if ( paramName == it->first )
        {
            return it->second;
        }
    }
    return undefined;
}

// 生成全部相等条件的WHERE子句
inline static winux::String __SqlWhereAllEqualSyntax( IDbConnection * cnn, winux::StringArray const & columnName, winux::Mixed const & value )
{
    winux::String str;
    if ( value.isArray() )
    {
        for ( size_t i = 0; i < columnName.size() && i < value._pArr->size(); ++i )
        {
            if ( i != 0 ) str += " AND ";
            str += cnn->symbolQuotes(columnName[i]) + " = " + MixedToEscapedStr( value[i], cnn );
        }
    }
    else if ( value.isCollection() )
    {
        for ( size_t i = 0; i < columnName.size() && i < value._pColl->getCount(); ++i )
        {
            if ( i != 0 ) str += " AND ";
            str += cnn->symbolQuotes(columnName[i]) + " = " + MixedToEscapedStr( value.getPair(i).second, cnn );
        }
    }
    else
    {
        if ( columnName.size() > 0 )
        {
            str += cnn->symbolQuotes(columnName[0]) + " = " + MixedToEscapedStr( value, cnn );
        }
    }
    return str;
}

// class DbModifier -------------------------------------------------------------------------
DbModifier::DbModifier( IDbConnection * cnn, winux::String const & tableName ) : _cnn(cnn), _tableName(tableName)
{

}

DbModifier::~DbModifier()
{

}

bool DbModifier::addNew( winux::StringArray const & fieldNames, winux::MixedArray const & values )
{
    assert( _cnn && *_cnn );
    assert( fieldNames.size() == values.size() );
    // 构建语句
    winux::String sql = _buildInsertInto( &fieldNames, values.size() ) + ";";
    // 数据绑定
    winux::SharedPointer<IDbStatement> stmt = _cnn->buildStmt(sql);
    for ( size_t i = 0; i < fieldNames.size(); ++i )
    {
        stmt->bind( ":" + fieldNames[i], values[i] );
    }
    return _execInsertInto(stmt);
}

bool DbModifier::addNew( winux::Mixed const & fields )
{
    assert( _cnn && *_cnn );
    if ( fields.isCollection() )
    {
        size_t const nFields = fields.getCount();
        winux::StringArray keys;
        fields.getKeys(&keys);
        // 构建语句
        winux::String sql = _buildInsertInto( &keys, nFields ) + ";";
        // 数据绑定
        winux::SharedPointer<IDbStatement> stmt = _cnn->buildStmt(sql);
        for ( size_t i = 0; i < nFields; ++i )
        {
            winux::MixedMixedPair const & field = fields.getPair(i);
            stmt->bind( ":" + field.first.to<winux::String>(), field.second );
        }
        return _execInsertInto(stmt);
    }
    else if ( fields.isArray() )
    {
        size_t const nFields = fields.getCount();
        // 构建语句
        winux::String sql = _buildInsertInto( nullptr, nFields ) + ";";
        // 数据绑定
        winux::SharedPointer<IDbStatement> stmt = _cnn->buildStmt(sql);
        for ( size_t i = 0; i < nFields; ++i )
        {
            stmt->bind( i + 1, fields.get(i) );
        }
        return _execInsertInto(stmt);
    }
    return false;
}

bool DbModifier::modify( winux::StringArray const & fieldNames, winux::MixedArray const & values, winux::Mixed const & prkValue )
{
    assert( fieldNames.size() == values.size() );
    assert( _cnn && *_cnn );
    assert( !_prkColumn.empty() );
    winux::String sql = "UPDATE " + _cnn->symbolQuotes(_tableName) + " SET ";
    winux::String itemsStr;
    size_t i;
    for ( i = 0; i < fieldNames.size(); ++i )
    {
        if ( i != 0 )
        {
            itemsStr += ", ";
        }
        itemsStr += _cnn->symbolQuotes(fieldNames[i]) + " = :" + fieldNames[i];
    }
    sql += itemsStr + " WHERE " + __SqlWhereAllEqualSyntax( _cnn, _prkColumn, prkValue );

    winux::SharedPointer<IDbStatement> stmt = _cnn->buildStmt(sql);
    for ( i = 0; i < fieldNames.size(); ++i )
    {
        stmt->bind( ":" + fieldNames[i], values[i] );
    }
    return _cnn->exec(stmt);
}

bool DbModifier::modify( winux::Mixed const & fields, winux::Mixed const & prkValue )
{
    assert( fields.isCollection() );
    assert( _cnn && *_cnn );
    assert( !_prkColumn.empty() );
    winux::String sql = "UPDATE " + _cnn->symbolQuotes(_tableName) + " SET ";
    winux::String itemsStr;
    size_t i;
    size_t const nFields = fields.getCount();
    for ( i = 0; i < nFields; ++i )
    {
        if ( i != 0 )
        {
            itemsStr += ", ";
        }
        winux::MixedMixedPair const & field = fields.getPair(i);
        itemsStr += _cnn->symbolQuotes(field.first) + " = :" + (winux::String)field.first;
    }
    sql += itemsStr + " WHERE " + __SqlWhereAllEqualSyntax( _cnn, _prkColumn, prkValue );

    winux::SharedPointer<IDbStatement> stmt = _cnn->buildStmt(sql);
    for ( i = 0; i < nFields; ++i )
    {
        winux::MixedMixedPair const & field = fields.getPair(i);
        stmt->bind( ":" + (winux::String)field.first, field.second );
    }
    return _cnn->exec(stmt);
}

bool DbModifier::modifyEx( winux::StringArray const & fieldNames, winux::MixedArray const & values, winux::String const & where )
{
    assert( fieldNames.size() == values.size() );
    assert( _cnn && *_cnn );
    winux::String sql = "UPDATE " + _cnn->symbolQuotes(_tableName) + " SET ";
    winux::String itemsStr;
    size_t i;
    for ( i = 0; i < fieldNames.size(); ++i )
    {
        if ( i != 0 )
        {
            itemsStr += ", ";
        }
        itemsStr += _cnn->symbolQuotes(fieldNames[i]) + " = :" + fieldNames[i];
    }
    sql += itemsStr + ( where.empty() ? "": " WHERE " + where );

    winux::SharedPointer<IDbStatement> stmt = _cnn->buildStmt(sql);
    for ( i = 0; i < fieldNames.size(); ++i )
    {
        stmt->bind( ":" + fieldNames[i], values[i] );
    }

    return _cnn->exec(stmt);
}

bool DbModifier::modifyEx( winux::Mixed const & fields, winux::String const & where )
{
    assert( fields.isCollection() );
    assert( _cnn && *_cnn );
    winux::String sql = "UPDATE " + _cnn->symbolQuotes(_tableName) + " SET ";
    winux::String itemsStr;
    size_t i;
    size_t const nFields = fields.getCount();
    for ( i = 0; i < nFields; ++i )
    {
        if ( i != 0 )
        {
            itemsStr += ", ";
        }
        winux::MixedMixedPair const & field = fields.getPair(i);
        itemsStr += _cnn->symbolQuotes(field.first) + " = :" + (winux::String)field.first;
    }
    sql += itemsStr + ( where.empty() ? "": " WHERE " + where );

    winux::SharedPointer<IDbStatement> stmt = _cnn->buildStmt(sql);
    for ( i = 0; i < nFields; ++i )
    {
        winux::MixedMixedPair const & field = fields.getPair(i);
        stmt->bind( ":" + (winux::String)field.first, field.second );
    }

    return _cnn->exec(stmt);
}

bool DbModifier::deleteOne( winux::Mixed const & prkValue )
{
    assert( _cnn && *_cnn );
    assert( !_prkColumn.empty() );
    winux::String sql = "DELETE FROM " + _cnn->symbolQuotes(_tableName) + " WHERE " + __SqlWhereAllEqualSyntax( _cnn, _prkColumn, prkValue );
    winux::SharedPointer<IDbStatement> stmt = _cnn->buildStmt(sql);
    return _cnn->exec(stmt);
}

bool DbModifier::deleteEx( winux::String const & where )
{
    assert( _cnn && *_cnn );
    winux::String sql = "DELETE FROM " + _cnn->symbolQuotes(_tableName) + ( where.empty() ? "": " WHERE " + where );
    winux::SharedPointer<IDbStatement> stmt = _cnn->buildStmt(sql);
    return _cnn->exec(stmt);
}

void DbModifier::_getTableInfo()
{
    assert( _cnn && *_cnn );
    //if ( !_cnn || !*_cnn ) throw DbError( DbError::dbeInvalidDbConn, "Invalid database connection" );

    // 获取字段名
    auto rs = _cnn->listFields(_tableName);
    winux::MixedArray row;
    while ( rs->fetchArray(&row) )
    {
        _fieldNames.push_back(row[0]);
    }
    // 获取主键列
    _cnn->getPrimaryKey( _tableName, &_prkColumn );
}

winux::String DbModifier::_buildInsertInto( winux::StringArray const * fieldNames, size_t valuesCount )
{
    winux::String sql, valuesStr;
    sql.reserve(64);
    valuesStr.reserve(64);
    sql += "INSERT INTO " + _cnn->symbolQuotes(_tableName) + ( fieldNames ? "(" : "" );
    for ( size_t i = 0; i < valuesCount; ++i )
    {
        if ( i != 0 )
        {
            if ( fieldNames ) sql += ", ";
            valuesStr += ", ";
        }
        if ( fieldNames )
        {
            sql += _cnn->symbolQuotes( (*fieldNames)[i] );
            valuesStr += ":" + (*fieldNames)[i];
        }
        else
        {
            valuesStr += "?";
        }
    }
    sql += ( fieldNames ? ") VALUES(" : " VALUES(" ) + valuesStr + ")";
    return sql;
}

bool DbModifier::_execInsertInto( winux::SharedPointer<IDbStatement> stmt )
{
    return _cnn->exec(stmt);
}


} // namespace eiendb
