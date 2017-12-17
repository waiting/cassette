#ifndef __EIENDB_SQLITE_HPP__
#define __EIENDB_SQLITE_HPP__

#include "eiendb_base.hpp"

struct sqlite3;
struct sqlite3_stmt;

namespace eiendb
{

/** \brief 数据库错误 */
class EIENDB_DLL SQLiteDbError: public DbError
{
    int _sqliteErrNo;
public:
    SQLiteDbError( int sqliteErrNo, winux::AnsiString const & s ) throw() : DbError( DbError::dbeDbSelfError, s ), _sqliteErrNo(sqliteErrNo) { }
    int getSqLiteErrCode() const throw() { return _sqliteErrNo; }
};

class EIENDB_DLL SQLiteConnection : public IDBConnection
{
public:
    SQLiteConnection( winux::String const & path = "", winux::AnsiString const & dbKey = "", winux::String linkCharset = "", bool doConnect = true );
    virtual ~SQLiteConnection();

    virtual bool connect();
    virtual bool close();
    virtual void alive();
    virtual bool setLinkCharset( winux::String const & charset );
    virtual bool selectDb( winux::String const & database );
    virtual bool createDb( winux::String const & database );
    virtual bool dropDb( winux::String const & database );
    virtual winux::ulong affectedRows();
    virtual int errNo();
    virtual winux::String error();
    // 只是简单的做escape(),内部不会进行utf8换码,引号double
    virtual winux::String escape( winux::String const & str, winux::String const & addQuote = "\'" );
    // 将二进制数据转换成X'******'的形式
    virtual winux::String escape( winux::byte const * buf, int size, winux::String const & addQuote = "\'" );
    // 执行SQL语句查询并返回结果集
    virtual winux::SharedPointer<IDBResult> query( winux::String const & sql );
    virtual winux::SharedPointer<IDBResult> query( winux::SharedPointer<IDBStatement> stmt );
    // 执行SQL语句,返回bool代表是否成功
    virtual bool exec( winux::String const & sql );
    virtual bool exec( winux::SharedPointer<IDBStatement> stmt );
    virtual winux::ulong insertId();
    virtual winux::SharedPointer<IDBResult> listDbs();
    // 执行 PRAGMA table_info(tablename)
    virtual winux::SharedPointer<IDBResult> listFields( winux::String const & tableName );
    // 执行 SELECT name FROM sqlite_master WHERE type = 'table' AND substr( name, 1, 7 ) <> 'sqlite_' ORDER BY name;
    virtual winux::SharedPointer<IDBResult> listTables();
    virtual winux::String tableDdl( winux::String const & tableName );
    virtual winux::String symbolQuotes( winux::String const & str );
/////////////////////////////////////////////////////////////////////////////////////////
    virtual winux::SharedPointer<IDBStatement> buildStmt( winux::String const & sql );
    virtual winux::SharedPointer<IDBStatement> buildStmt( winux::String const & sql, winux::Mixed const & params );
    virtual winux::SharedPointer<IDBStatement> buildStmt( winux::String const & sql, winux::MixedArray const & params );
    virtual winux::SharedPointer<IDBStatement> buildStmt( winux::String const & sql, winux::StringMixedMap const & params );

    virtual operator bool() const { return _db != NULL; }
    operator sqlite3 * () const { return _db; }
    // SQLite字符串转到winux::String,NULL也能正确处理
    winux::String sqliteStrToString( char const * s );
    // winux::String转到SQLite字符串
    winux::AnsiString stringToSqliteStr( winux::String const & s );
private:
    bool _open( winux::String const & database, winux::uint flags );

    sqlite3 * _db;
    winux::String _path; // 输入的路径参数
    winux::String _dirPath; // 解析到的目录路径
    winux::String _database; // 解析到的数据库名
    winux::String _linkCharset; // 连接校验字符集
    winux::AnsiString _dbKey; // 数据库密码,可以为二进制数据

    DISABLE_OBJECT_COPY(SQLiteConnection)
};

class EIENDB_DLL SQLiteStatement : public IDBStatement
{
public:
    SQLiteStatement( SQLiteConnection * cnn );
    SQLiteStatement( SQLiteConnection * cnn, winux::String const & sql );
    SQLiteStatement( SQLiteConnection * cnn, winux::String const & sql, winux::Mixed const & params );
    SQLiteStatement( SQLiteConnection * cnn, winux::String const & sql, winux::MixedArray const & params );
    SQLiteStatement( SQLiteConnection * cnn, winux::String const & sql, winux::StringMixedMap const & params );
    virtual ~SQLiteStatement();
    //SQLiteStatement( SQLiteStatement & other );
    //SQLiteStatement & operator = ( SQLiteStatement & other );

    virtual bool build( winux::String const & sql );
    virtual bool build( winux::String const & sql, winux::Mixed const & params );
    virtual bool build( winux::String const & sql, winux::MixedArray const & params );
    virtual bool build( winux::String const & sql, winux::StringMixedMap const & params );
    virtual bool bind( int paramIndex, winux::Mixed const & val );
    virtual bool bind( winux::String const & paramName, winux::Mixed const & val );

    operator sqlite3_stmt * () const { return _stmt; }
    SQLiteConnection * getCnn() const { return _cnn; }
    winux::String getSql() const { return _sql; }

private:
    void free();

    SQLiteConnection * _cnn;
    sqlite3_stmt * _stmt;
    winux::String _sql;
    std::vector< std::pair< int, winux::Mixed > > _bindingParams; // 已绑定的参数
    friend class SQLiteResult;

    DISABLE_OBJECT_COPY(SQLiteStatement)
};

class EIENDB_DLL SQLiteResult : public IDBResult
{
public:
    SQLiteResult( winux::SharedPointer<SQLiteStatement> stmt );
    virtual ~SQLiteResult();

    virtual bool fetchRow( winux::Mixed * fields, int type = 0 );
    virtual bool fetchRow( winux::MixedArray * fields );
    virtual bool fetchRow( winux::StringMixedMap * fields );
    virtual winux::String fieldName( int fieldIndex );
    virtual winux::ulong fieldsCount();
    virtual winux::ulong rowsCount();
    virtual bool free();
    virtual winux::String fieldType( int fieldIndex );

    // 重置结果集,以便再次fetch
    bool reset();
    int retCode() const { return _retCode; }
private:
    winux::SharedPointer<SQLiteStatement> _stmt;
    int _retCode;
    bool _first;
    bool _hasRow;
    winux::StringArray _fieldNames; // 非原始字段名

    DISABLE_OBJECT_COPY(SQLiteResult)
};

class EIENDB_DLL SQLiteModifier : public DBModifier
{
public:
    SQLiteModifier( SQLiteConnection * cnn, winux::String const & tableName );
    virtual ~SQLiteModifier();

/*  virtual bool addNew( winux::StringArray const & fieldNames, winux::MixedArray const & values );
    virtual bool addNew( winux::Mixed const & fields );
    virtual bool modify( winux::StringArray const & fieldNames, winux::MixedArray const & values, winux::Mixed const & prkValue );
    virtual bool modify( winux::Mixed const & fields, winux::Mixed const & prkValue );
    virtual int modifyEx( winux::StringArray const & fieldNames, winux::MixedArray const & values, winux::String const & where );
    virtual int modifyEx( winux::Mixed const & fields, winux::String const & where );
    virtual bool deleteOne( winux::Mixed const & prkValue );
    virtual int deleteEx( winux::String const & where ); */

protected:
    virtual void _getTableInfo();
/*  SQLiteConnection * _cnn;
    winux::String _tableName;
    winux::StringArray _fieldNames; // 原始字段名
    winux::String _prkName;         // 主键名
    //bool _prkAutoIncrement;  // 自动增长 */

    DISABLE_OBJECT_COPY(SQLiteModifier)
};

}

#endif // __EIENDB_SQLITE_HPP__
