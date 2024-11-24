#ifndef __EIENDB_MYSQL_HPP__
#define __EIENDB_MYSQL_HPP__

#include "eiendb_base.hpp"

namespace eiendb
{
#ifdef HAVE_DB_MYSQL

/** \brief MySQL数据库错误 */
class EIENDB_DLL MysqlDbError: public DbError
{
public:
    MysqlDbError( int mysqlErrNo, winux::AnsiString const & s ) throw() : DbError( DbError::dbeMysqlError, s ), _mysqlErrNo(mysqlErrNo) { }
    int getMysqlErrCode() const throw() { return _mysqlErrNo; }

private:
    int _mysqlErrNo;
};

class EIENDB_DLL MysqlConnection : public IDbConnection
{
public:
    MysqlConnection( winux::String const & host, winux::String const & user, winux::String const & pwd, winux::String const & dbName, winux::String const & linkCharset, bool doConnect = true );
    virtual ~MysqlConnection();

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
    virtual bool exec( winux::String const & sql ) override;
    virtual bool exec( winux::SharedPointer<IDbStatement> stmt ) override;
    virtual winux::SharedPointer<IDbResult> query( winux::String const & sql ) override;
    virtual winux::SharedPointer<IDbResult> query( winux::SharedPointer<IDbStatement> stmt ) override;
    virtual winux::SharedPointer<IDbResult> listDbs() override;
    virtual winux::SharedPointer<IDbResult> listFields( winux::String const & tableName ) override;
    virtual winux::SharedPointer<IDbResult> listTables() override;
    virtual winux::String tableDdl( winux::String const & tableName ) override;
    virtual winux::String symbolQuotes( winux::String const & str, bool periodAsSeparator = true ) override;
    virtual size_t loadSqlFile( winux::IFile * sqlFile, winux::StringArray * arrSql ) override;
    virtual size_t getPrimaryKey( winux::String const & tableName, winux::StringArray * arrKeyColumn ) override;
    virtual winux::SharedPointer<IDbStatement> buildStmt( winux::String const & sql, winux::Mixed const & params = winux::mxNull ) override;
    virtual operator bool() const override { return _mysql != NULL; }

    operator DbHandle() const { return _mysql; }

private:
    DbHandle _mysql; // 代表MYSQL连接
    winux::String _hostAddr;  // 主机地址
    int _port;         // 端口
    winux::String _user;      // 用户名
    winux::String _pwd;       // 密码
    winux::String _dbName;    // 数据库名
    winux::String _linkCharset; // 连接校验字符集

    DISABLE_OBJECT_COPY(MysqlConnection)
};

class EIENDB_DLL MysqlStatement : public DbStatement
{
public:
    MysqlStatement( MysqlConnection * cnn ) : DbStatement(cnn) { }
    MysqlStatement( MysqlConnection * cnn, winux::String const & sql, winux::Mixed const & params = winux::mxNull ) : DbStatement( cnn, sql, params ) { }

    MysqlConnection * getCnn() const { return static_cast<MysqlConnection*>( this->DbStatement::getCnn() ); }

    DISABLE_OBJECT_COPY(MysqlStatement)
};

class EIENDB_DLL MysqlResult : public IDbResult
{
public:
    MysqlResult( winux::SharedPointer<MysqlStatement> stmt );
    virtual ~MysqlResult();
    virtual bool free() override;

    virtual bool dataSeek( size_t index ) override;
    virtual bool fetchRow( winux::Mixed * fields, int type = 0 ) override;
    virtual bool fetchArray( winux::MixedArray * fields ) override;
    virtual bool fetchMap( winux::StringMixedMap * fields ) override;
    virtual winux::String fieldName( size_t fieldIndex ) override;
    virtual size_t fieldsCount() override;
    virtual size_t rowsCount() override;
    virtual winux::String fieldType( size_t fieldIndex ) override;

    bool reset();
    operator bool() const { return _mysqlRes != NULL; }
    operator DbResHandle() const { return _mysqlRes; }

private:
    DbResHandle _mysqlRes;
    winux::SharedPointer<MysqlStatement> _stmt;
    winux::StringArray _fieldNames; // 非原始字段名
    std::vector<winux::uint> _fieldTypes; // 字段类型
    std::vector<winux::uint> _fieldFlags; // 字段Flag

    DISABLE_OBJECT_COPY(MysqlResult)
};

class EIENDB_DLL MysqlModifier : public DbModifier
{
public:
    MysqlModifier( MysqlConnection * cnn, winux::String const & tableName );
    virtual ~MysqlModifier();

    DISABLE_OBJECT_COPY(MysqlModifier)
};

#endif // HAVE_DB_MYSQL

} // namespace eiendb

#endif // __EIENDB_MYSQL_HPP__
