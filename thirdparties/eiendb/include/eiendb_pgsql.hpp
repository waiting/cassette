#ifndef __EIENDB_PGSQL_HPP__
#define __EIENDB_PGSQL_HPP__

#include "eiendb_base.hpp"

namespace eiendb
{
#ifdef HAVE_DB_PGSQL

/** \brief PostgreSQL数据库错误 */
class EIENDB_DLL PgsqlDbError : public DbError
{
public:
    PgsqlDbError( int pgsqlErrNo, winux::AnsiString const & s ) throw() : DbError( DbError::dbePgsqlError, s ), _pgsqlErrNo(pgsqlErrNo) { }
    int getPgsqlErrCode() const throw() { return _pgsqlErrNo; }

private:
    int _pgsqlErrNo;
};

class EIENDB_DLL PgsqlConnection : public IDbConnection
{
public:
    /** \brief PgsqlConnection 构造函数
     *
     *  \param host 主机或主机列表 IP[:PORT][, ...]
     *  \param user 用户名
     *  \param pwd 密码
     *  \param dbName 数据库名
     *  \param linkCharset 连接校验字符集
     *  \param schema 模式名。默认"public"
     *  \param options 发送给服务器的命令行选项 eg. "-c geqo=off"
     *  \param dbConfig 一些数据库特有的配置
     *  \param doConnect 是否连接数据库。默认true */
    PgsqlConnection(
        winux::String const & host,
        winux::String const & user,
        winux::String const & pwd,
        winux::String const & dbName,
        winux::String const & linkCharset,
        winux::String const & schema = "public",
        winux::String const & options = "",
        winux::Mixed * dbConfig = nullptr,
        bool doConnect = true
    );
    virtual ~PgsqlConnection();

    virtual bool connect() override;
    virtual bool close() override;
    virtual void alive() override;
    virtual bool selectDb( winux::String const & database ) override;
    virtual bool setLinkCharset( winux::String const & charset ) override;
    virtual bool createDb( winux::String const & database ) override;
    virtual bool dropDb( winux::String const & database ) override;
    virtual size_t affectedRows() override;
    virtual size_t insertId() override;
    virtual int errNo() override;
    virtual winux::String error() override;
    virtual winux::SharedPointer<IDbResult> query( winux::String const & sql ) override;
    virtual winux::SharedPointer<IDbResult> query( winux::SharedPointer<IDbStatement> stmt ) override;
    virtual bool exec( winux::String const & sql ) override;
    virtual bool exec( winux::SharedPointer<IDbStatement> stmt ) override;
    virtual winux::String escape( winux::String const & str, winux::String const & addQuote = "\'" ) override;
    virtual winux::String escape( void const * buf, size_t size, winux::String const & addQuote = "\'" ) override;
    virtual winux::SharedPointer<IDbResult> listDbs() override;
    virtual winux::SharedPointer<IDbResult> listFields( winux::String const & tableName ) override;
    virtual winux::SharedPointer<IDbResult> listTables() override;
    virtual winux::String tableDdl( winux::String const & tableName ) override;
    virtual winux::String symbolQuotes( winux::String const & str, bool periodAsSeparator = true ) override;
    virtual size_t loadSqlFile( winux::IFile * sqlFile, winux::StringArray * arrSql ) override;
    virtual size_t getPrimaryKey( winux::String const & tableName, winux::StringArray * arrKeyColumn ) override;
    virtual winux::SharedPointer<IDbStatement> buildStmt( winux::String const & sql, winux::Mixed const & params = winux::mxNull ) override;
    virtual operator bool() const override;

    operator DbHandle() const;

private:
    winux::MembersWrapper<struct PgsqlConnection_Data> _self;
    friend class PgsqlResult;
    friend class PgsqlModifier;
    DISABLE_OBJECT_COPY(PgsqlConnection)
};

class EIENDB_DLL PgsqlStatement : public DbStatement
{
public:
    PgsqlStatement( PgsqlConnection * cnn ) : DbStatement(cnn) { }
    PgsqlStatement( PgsqlConnection * cnn, winux::String const & sql, winux::Mixed const & params = winux::mxNull ) : DbStatement( cnn, sql, params ) { }

    PgsqlConnection * getCnn() const { return static_cast<PgsqlConnection *>( this->DbStatement::getCnn() ); }

    DISABLE_OBJECT_COPY(PgsqlStatement)
};

class EIENDB_DLL PgsqlResult : public IDbResult
{
public:
    PgsqlResult( winux::SharedPointer<PgsqlStatement> stmt, bool singleRow = false );
    virtual ~PgsqlResult();
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
    operator bool() const;

private:
    winux::MembersWrapper<struct PgsqlResult_Data> _self;
    DISABLE_OBJECT_COPY(PgsqlResult)
};

class EIENDB_DLL PgsqlModifier : public DbModifier
{
public:
    PgsqlModifier( PgsqlConnection * cnn, winux::String const & tableName );
    virtual ~PgsqlModifier();

protected:
    virtual bool _execInsertInto( winux::SharedPointer<IDbStatement> stmt ) override;

    DISABLE_OBJECT_COPY(PgsqlModifier)
};

#endif // HAVE_DB_PGSQL

} // namespace eiendb

#endif // __EIENDB_PGSQL_HPP__
