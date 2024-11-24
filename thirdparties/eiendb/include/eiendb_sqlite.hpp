#ifndef __EIENDB_SQLITE_HPP__
#define __EIENDB_SQLITE_HPP__

#include "eiendb_base.hpp"

struct sqlite3;
struct sqlite3_stmt;

namespace eiendb
{
#ifdef HAVE_DB_SQLITE

/** \brief SQLite数据库错误 */
class EIENDB_DLL SqliteDbError: public DbError
{
public:
    SqliteDbError( int sqliteErrNo, winux::AnsiString const & s ) throw() : DbError( DbError::dbeSqliteError, s ), _sqliteErrNo(sqliteErrNo) { }
    int getSqliteErrCode() const throw() { return _sqliteErrNo; }

private:
    int _sqliteErrNo;
};

class EIENDB_DLL SqliteConnection : public IDbConnection
{
public:
    SqliteConnection( winux::String const & path = "", winux::AnsiString const & dbKey = "", winux::String linkCharset = "", bool doConnect = true );
    virtual ~SqliteConnection();

    virtual bool connect() override;
    virtual bool close() override;
    virtual void alive() override;
    virtual bool setLinkCharset( winux::String const & charset ) override;
    virtual bool selectDb( winux::String const & database ) override;
    virtual bool createDb( winux::String const & database ) override;
    virtual bool dropDb( winux::String const & database ) override;
    virtual size_t affectedRows() override;
    virtual size_t insertId() override;
    virtual int errNo() override;
    virtual winux::String error() override;
    virtual winux::String escape( winux::String const & str, winux::String const & addQuote = "\'" ) override;
    virtual winux::String escape( void const * buf, size_t size, winux::String const & addQuote = "\'" ) override;
    virtual winux::SharedPointer<IDbResult> query( winux::String const & sql ) override;
    virtual winux::SharedPointer<IDbResult> query( winux::SharedPointer<IDbStatement> stmt ) override;
    virtual bool exec( winux::String const & sql ) override;
    virtual bool exec( winux::SharedPointer<IDbStatement> stmt ) override;
    virtual winux::SharedPointer<IDbResult> listDbs() override;
    virtual winux::SharedPointer<IDbResult> listFields( winux::String const & tableName ) override;
    virtual winux::SharedPointer<IDbResult> listTables() override;
    virtual winux::String tableDdl( winux::String const & tableName ) override;
    virtual winux::String symbolQuotes( winux::String const & str, bool periodAsSeparator = true ) override;
    virtual size_t loadSqlFile( winux::IFile * sqlFile, winux::StringArray * arrSql ) override;
    virtual size_t getPrimaryKey( winux::String const & tableName, winux::StringArray * arrKeyColumn ) override;
    virtual winux::SharedPointer<IDbStatement> buildStmt( winux::String const & sql, winux::Mixed const & params = winux::mxNull ) override;
    virtual operator bool() const override { return _db != NULL; }

    operator sqlite3 * () const { return _db; }
    // SQLite字符串转到winux::String，NULL也能正确处理
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

    DISABLE_OBJECT_COPY(SqliteConnection)
};

class EIENDB_DLL SqliteStatement : public IDbStatement
{
public:
    SqliteStatement( SqliteConnection * cnn );
    SqliteStatement( SqliteConnection * cnn, winux::String const & sql, winux::Mixed const & params = winux::mxNull );
    virtual ~SqliteStatement();

    virtual bool build( winux::String const & sql, winux::Mixed const & params = winux::mxNull ) override;
    virtual bool bind( size_t paramIndex, winux::Mixed const & val ) override;
    virtual bool bind( winux::String const & paramName, winux::Mixed const & val ) override;

    operator sqlite3_stmt * () const { return _stmt; }
    SqliteConnection * getCnn() const { return _cnn; }
    winux::String getSql() const { return _sql; }

private:
    void _free();

    SqliteConnection * _cnn;
    sqlite3_stmt * _stmt;
    winux::String _sql;
    std::vector< std::pair< size_t, winux::Mixed > > _bindingParams; // 已绑定的参数
    friend class SqliteResult;

    DISABLE_OBJECT_COPY(SqliteStatement)
};

class EIENDB_DLL SqliteResult : public IDbResult
{
public:
    SqliteResult( winux::SharedPointer<SqliteStatement> stmt );
    virtual ~SqliteResult();
    virtual bool free() override;

    virtual bool fetchRow( winux::Mixed * fields, int type = 0 ) override;
    virtual bool fetchArray( winux::MixedArray * fields ) override;
    virtual bool fetchMap( winux::StringMixedMap * fields ) override;
    virtual winux::String fieldName( size_t fieldIndex ) override;
    virtual size_t fieldsCount() override;
    virtual size_t rowsCount() override;
    virtual winux::String fieldType( size_t fieldIndex ) override;

    // 重置结果集，以便再次fetch
    bool reset();
    int retCode() const { return _retCode; }

private:
    winux::SharedPointer<SqliteStatement> _stmt;
    int _retCode;
    bool _first;
    bool _hasRow;
    winux::StringArray _fieldNames; // 非原始字段名

    DISABLE_OBJECT_COPY(SqliteResult)
};

class EIENDB_DLL SqliteModifier : public DbModifier
{
public:
    SqliteModifier( SqliteConnection * cnn, winux::String const & tableName );
    virtual ~SqliteModifier();

    DISABLE_OBJECT_COPY(SqliteModifier)
};

#endif // HAVE_DB_SQLITE

} // namespace eiendb

#endif // __EIENDB_SQLITE_HPP__
