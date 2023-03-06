
#include "eiendb_base.hpp"

namespace eiendb
{
// interface IDbConnection --------------------------------------------------
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

size_t IDbConnection::affectedRows()
{
    throw DbError( DbError::dbeNotImplemented, "`size_t affectedRows()` is not implemented" );
}

bool IDbConnection::createDb( winux::String const & database )
{
    throw DbError( DbError::dbeNotImplemented, "`bool createDb( String const & database )` is not implemented" );
}

bool IDbConnection::dropDb( winux::String const & database )
{
    throw DbError( DbError::dbeNotImplemented, "`bool dropDb( String const & database )` is not implemented" );
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

size_t IDbConnection::insertId()
{
    throw DbError( DbError::dbeNotImplemented, "`size_t insertId()` is not implemented" );
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

winux::String IDbConnection::symbolQuotes( winux::String const & str )
{
    throw DbError( DbError::dbeNotImplemented, "`String symbolQuotes( String const & str )` is not implemented" );
}

size_t IDbConnection::loadSql( winux::String const & sqlText, winux::StringArray * arrSql )
{
    throw DbError( DbError::dbeNotImplemented, "`size_t loadSql( String const & sqlText, StringArray * arrSql )` is not implemented" );
}

size_t IDbConnection::loadSqlFile( winux::IFile * sqlScriptFile, winux::StringArray * arrSql )
{
    throw DbError( DbError::dbeNotImplemented, "`size_t loadSqlFile( IFile * sqlScriptFile, StringArray * arrSql )` is not implemented" );
}

size_t IDbConnection::getPrimaryKey( winux::String const & tableName, winux::StringArray * arrKeyColumn )
{
    throw DbError( DbError::dbeNotImplemented, "`size_t getPrimaryKey( String const & tableName, StringArray * arrKeyColumn )` is not implemented" );
}

winux::SharedPointer<IDbStatement> IDbConnection::buildStmt( winux::String const & sql )
{
    throw DbError( DbError::dbeNotImplemented, "`SharedPointer<IDbStatement> buildStmt( String const & sql )` is not implemented" );
}

winux::SharedPointer<IDbStatement> IDbConnection::buildStmt( winux::String const & sql, winux::Mixed const & params )
{
    throw DbError( DbError::dbeNotImplemented, "`SharedPointer<IDbStatement> buildStmt( String const & sql, Mixed const & params )` is not implemented" );
}

winux::SharedPointer<IDbStatement> IDbConnection::buildStmt( winux::String const & sql, winux::MixedArray const & params )
{
    throw DbError( DbError::dbeNotImplemented, "`SharedPointer<IDbStatement> buildStmt( String const & sql, MixedArray const & params )` is not implemented" );
}

winux::SharedPointer<IDbStatement> IDbConnection::buildStmt( winux::String const & sql, winux::StringMixedMap const & params )
{
    throw DbError( DbError::dbeNotImplemented, "`SharedPointer<IDbStatement> buildStmt( String const & sql, StringMixedMap const & params )` is not implemented" );
}

// interface IDbStatement ---------------------------------------------------
bool IDbStatement::build( winux::String const & sql )
{
    throw DbError( DbError::dbeNotImplemented, "`bool build( String const & sql )` is not implemented" );
}

bool IDbStatement::build( winux::String const & sql, winux::Mixed const & params )
{
    throw DbError( DbError::dbeNotImplemented, "`bool build( String const & sql, Mixed const & params )` is not implemented" );
}

bool IDbStatement::build( winux::String const & sql, winux::MixedArray const & params )
{
    throw DbError( DbError::dbeNotImplemented, "`bool build( String const & sql, MixedArray const & params )` is not implemented" );
}

bool IDbStatement::build( winux::String const & sql, winux::StringMixedMap const & params )
{
    throw DbError( DbError::dbeNotImplemented, "`bool build( String const & sql, StringMixedMap const & params )` is not implemented" );
}

bool IDbStatement::bind( size_t paramIndex, winux::Mixed const & val )
{
    throw DbError( DbError::dbeNotImplemented, "`bool bind( size_t paramIndex, Mixed const & val )` is not implemented" );
}

bool IDbStatement::bind( winux::String const & paramName, winux::Mixed const & val )
{
    throw DbError( DbError::dbeNotImplemented, "`bool bind( String const & paramName, Mixed const & val )` is not implemented" );
}

// interface IDbResult ------------------------------------------------------
bool IDbResult::dataSeek( size_t index )
{
    throw DbError( DbError::dbeNotImplemented, "`bool dataSeek( size_t index )` is not implemented" );
}

bool IDbResult::fetchRow( winux::Mixed * fields, int type )
{
    throw DbError( DbError::dbeNotImplemented, "`bool fetchRow( Mixed * fields, int type )` is not implemented" );
}

bool IDbResult::fetchRow( winux::MixedArray * fields )
{
    throw DbError( DbError::dbeNotImplemented, "`bool fetchRow( MixedArray * fields )` is not implemented" );
}

bool IDbResult::fetchRow( winux::StringMixedMap * fields )
{
    throw DbError( DbError::dbeNotImplemented, "`bool fetchRow( StringMixedMap * fields )` is not implemented" );
}

winux::String IDbResult::fieldName( size_t fieldIndex )
{
    throw DbError( DbError::dbeNotImplemented, "`String fieldName( size_t fieldIndex )` is not implemented" );
}

bool IDbResult::free()
{
    throw DbError( DbError::dbeNotImplemented, "`bool free()` is not implemented" );
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

// interface IDbModifier -------------------------------------------------------------------
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

// ----------------------------------------------------------------------------------------
EIENDB_FUNC_IMPL(winux::String) MixedToEscapedStr( winux::Mixed const & m, IDbConnection * cnn, winux::String const & addQuote )
{
    switch ( m._type )
    {
    case winux::Mixed::MT_NULL:
        return "null";
    case winux::Mixed::MT_BOOLEAN:
        return m._boolVal ? "true" : "false";
    case winux::Mixed::MT_BYTE:
        return winux::Mixed( (int)m._btVal );
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

// class DbModifier -----------------------------------------------------------------------
// 生成全部相等条件的WHERE子句
static winux::String __SqlWhereAllEqualSyntax( IDbConnection * cnn, winux::StringArray const & columnName, winux::Mixed const & value )
{
    winux::String s;
    if ( value.isArray() )
    {
        for ( size_t i = 0; i < columnName.size() && i < value._pArr->size(); ++i )
        {
            if ( i != 0 ) s += " AND ";
            s += cnn->symbolQuotes(columnName[i]) + " = " + MixedToEscapedStr( value[i], cnn );
        }
    }
    else if ( value.isCollection() )
    {
        for ( size_t i = 0; i < columnName.size() && i < value._pMap->size(); ++i )
        {
            if ( i != 0 ) s += " AND ";
            s += cnn->symbolQuotes(columnName[i]) + " = " + MixedToEscapedStr( value.getPair(i).second, cnn );
        }
    }
    else
    {
        if ( columnName.size() > 0 )
        {
            s += cnn->symbolQuotes(columnName[0]) + " = " + MixedToEscapedStr( value, cnn );
        }
    }

    return s;
}

DbModifier::DbModifier( IDbConnection * cnn, winux::String const & tableName ) : _cnn(cnn), _tableName(tableName)
{

}

DbModifier::~DbModifier()
{

}

bool DbModifier::addNew( winux::StringArray const & fieldNames, winux::MixedArray const & values )
{
    assert( fieldNames.size() == values.size() );
    assert( _cnn && *_cnn );
    winux::String sql = "INSERT INTO " + _cnn->symbolQuotes(_tableName) + "(";
    winux::String fieldsStr, valuesStr;
    size_t i;
    for ( i = 0; i < fieldNames.size(); ++i )
    {
        if ( i != 0 )
        {
            fieldsStr += ", ";
            valuesStr += ", ";
        }
        fieldsStr += _cnn->symbolQuotes(fieldNames[i]);
        valuesStr += ":" + fieldNames[i];
    }
    sql += fieldsStr + ") VALUES(" + valuesStr + ");";
    winux::SharedPointer<IDbStatement> stmt = _cnn->buildStmt(sql);
    for ( i = 0; i < fieldNames.size(); ++i )
    {
        stmt->bind( ":" + fieldNames[i], values[i] );
    }

    return _cnn->exec(stmt);
}

bool DbModifier::addNew( winux::Mixed const & fields )
{
    assert( _cnn && *_cnn );
    if ( fields.isCollection() )
    {
        winux::String sql = "INSERT INTO " + _cnn->symbolQuotes(_tableName) + "(";
        winux::String fieldsStr, valuesStr;
        size_t i;
        size_t const nFields = fields.getCount();
        for ( i = 0; i < nFields; ++i )
        {
            if ( i != 0 )
            {
                fieldsStr += ", ";
                valuesStr += ", ";
            }
            winux::Mixed::MixedMixedMap::value_type const & field = fields.getPair(i);
            fieldsStr += _cnn->symbolQuotes(field.first);
            valuesStr += ":" + (winux::String)field.first;
        }
        sql += fieldsStr + ") VALUES(" + valuesStr + ");";

        winux::SharedPointer<IDbStatement> stmt = _cnn->buildStmt(sql);
        for ( i = 0; i < nFields; ++i )
        {
            winux::Mixed::MixedMixedMap::value_type const & field = fields.getPair(i);
            stmt->bind( ":" + (winux::String)field.first, field.second );
        }
        return _cnn->exec(stmt);
    }
    else if ( fields.isArray() )
    {
        winux::String sql = "INSERT INTO " + _cnn->symbolQuotes(_tableName) + "";
        winux::String valuesStr;
        size_t i;
        size_t const nFields = fields.getCount();
        for ( i = 0; i < nFields; ++i )
        {
            if ( i != 0 )
            {
                valuesStr += ", ";
            }
            valuesStr += "?";
        }
        sql += " VALUES(" + valuesStr + ");";

        winux::SharedPointer<IDbStatement> stmt = _cnn->buildStmt(sql);
        for ( i = 0; i < nFields; ++i )
        {
            winux::Mixed const & field = fields.get(i);
            stmt->bind( i + 1, field );
        }
        return _cnn->exec(stmt);
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
    sql += itemsStr + " WHERE " + __SqlWhereAllEqualSyntax( _cnn, _prkColumn, prkValue ); //_cnn->symbolQuotes(_prkName) + " = :" + _prkName;

    winux::SharedPointer<IDbStatement> stmt = _cnn->buildStmt(sql);
    for ( i = 0; i < fieldNames.size(); ++i )
    {
        stmt->bind( ":" + fieldNames[i], values[i] );
    }
    //stmt->bind( ":" + _prkName, prkValue );

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
        winux::Mixed::MixedMixedMap::value_type const & field = fields.getPair(i);
        itemsStr += _cnn->symbolQuotes(field.first) + " = :" + (winux::String)field.first;
    }
    sql += itemsStr + " WHERE " + __SqlWhereAllEqualSyntax( _cnn, _prkColumn, prkValue ); //_cnn->symbolQuotes(_prkName) + " = :" + _prkName;

    winux::SharedPointer<IDbStatement> stmt = _cnn->buildStmt(sql);
    for ( i = 0; i < nFields; ++i )
    {
        winux::Mixed::MixedMixedMap::value_type const & field = fields.getPair(i);
        stmt->bind( ":" + (winux::String)field.first, field.second );
    }
    //stmt->bind( ":" + _prkName, prkValue );

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
        winux::Mixed::MixedMixedMap::value_type const & field = fields.getPair(i);
        itemsStr += _cnn->symbolQuotes(field.first) + " = :" + (winux::String)field.first;
    }
    sql += itemsStr + ( where.empty() ? "": " WHERE " + where );

    winux::SharedPointer<IDbStatement> stmt = _cnn->buildStmt(sql);
    for ( i = 0; i < nFields; ++i )
    {
        winux::Mixed::MixedMixedMap::value_type const & field = fields.getPair(i);
        stmt->bind( ":" + (winux::String)field.first, field.second );
    }

    return _cnn->exec(stmt);
}


bool DbModifier::deleteOne( winux::Mixed const & prkValue )
{
    assert( _cnn && *_cnn );
    assert( !_prkColumn.empty() );
    winux::String sql = "DELETE FROM " + _cnn->symbolQuotes(_tableName) + " WHERE " + __SqlWhereAllEqualSyntax( _cnn, _prkColumn, prkValue ); //_cnn->symbolQuotes(_prkName) + " = :" + _prkName;

    winux::SharedPointer<IDbStatement> stmt = _cnn->buildStmt(sql);
    //stmt->bind( ":" + _prkName, prkValue );

    return _cnn->exec(stmt);
}

bool DbModifier::deleteEx( winux::String const & where )
{
    assert( _cnn && *_cnn );
    winux::String sql = "DELETE FROM " + _cnn->symbolQuotes(_tableName) + ( where.empty() ? "": " WHERE " + where );
    winux::SharedPointer<IDbStatement> stmt = _cnn->buildStmt(sql);
    return _cnn->exec(stmt);
}


} // namespace eiendb
