#ifndef __EIENDB_MYSQL_HPP__
#define __EIENDB_MYSQL_HPP__

#include "eiendb_base.hpp"

struct st_mysql;
struct st_mysql_res;
struct st_mysql_stmt;

namespace eiendb
{
/** \brief MySQL数据库错误 */
class EIENDB_DLL MysqlDbError: public DbError
{
    int _mysqlErrNo;
public:
    MysqlDbError( int mysqlErrNo, winux::AnsiString const & s ) : DbError( DbError::dbeMysqlError, s ), _mysqlErrNo(mysqlErrNo) { }
    int getMysqlErrCode() const throw() { return _mysqlErrNo; }
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
    virtual winux::String symbolQuotes( winux::String const & str ) override;

    virtual size_t loadSql( winux::String const & sqlText, winux::StringArray * arrSql ) override;
    virtual size_t loadSqlFile( winux::IFile * sqlScriptFile, winux::StringArray * arrSql ) override;
    virtual size_t getPrimaryKey( winux::String const & tableName, winux::StringArray * arrKeyColumn ) override;

    virtual winux::SharedPointer<IDbStatement> buildStmt( winux::String const & sql ) override;
    virtual winux::SharedPointer<IDbStatement> buildStmt( winux::String const & sql, winux::Mixed const & params ) override;
    virtual winux::SharedPointer<IDbStatement> buildStmt( winux::String const & sql, winux::MixedArray const & params ) override;
    virtual winux::SharedPointer<IDbStatement> buildStmt( winux::String const & sql, winux::StringMixedMap const & params ) override;

    virtual operator bool() const override { return _mysql != NULL; }
    operator struct st_mysql * () const { return _mysql; }
private:
    struct st_mysql * _mysql; // 代表MYSQL连接
    winux::String _hostAddr;  // 主机地址
    int _port;         // 端口
    winux::String _user;      // 用户名
    winux::String _pwd;       // 密码
    winux::String _dbName;    // 数据库名
    winux::String _linkCharset; // 连接校验字符集

    DISABLE_OBJECT_COPY(MysqlConnection)
};

class EIENDB_DLL MysqlStatement : public IDbStatement
{
public:
    MysqlStatement( MysqlConnection * cnn );
    MysqlStatement( MysqlConnection * cnn, winux::String const & sql );
    MysqlStatement( MysqlConnection * cnn, winux::String const & sql, winux::Mixed const & params );
    MysqlStatement( MysqlConnection * cnn, winux::String const & sql, winux::MixedArray const & params );
    MysqlStatement( MysqlConnection * cnn, winux::String const & sql, winux::StringMixedMap const & params );
    virtual ~MysqlStatement();
    /** 建立一个SQL语句 */
    virtual bool build( winux::String const & sql ) override;
    virtual bool build( winux::String const & sql, winux::Mixed const & params ) override;
    virtual bool build( winux::String const & sql, winux::MixedArray const & params ) override;
    virtual bool build( winux::String const & sql, winux::StringMixedMap const & params ) override;
    /** 绑定参数 参数索引起始为1 */
    virtual bool bind( size_t paramIndex, winux::Mixed const & val ) override;
    /** 绑定参数 按参数名 参数名前需要加: */
    virtual bool bind( winux::String const & paramName, winux::Mixed const & val ) override;

    operator bool() const { return !_orgSql.empty(); }
    MysqlConnection * getCnn() const { return _cnn; }
    winux::String const & getSql();

    // paramIndex base on 1 
    winux::Mixed const & getParam( size_t paramIndex ) const;
    // paramName prefix with ':'
    winux::Mixed const & getParam( winux::String const & paramName ) const;
private:
    void _generateSql();

    MysqlConnection * _cnn;
    winux::String _orgSql; // 原SQL
    winux::String _sql;
    bool _isGeneratedSql; // 是否已经产生SQL
    bool _isNameBinding; // 是否采用参数名称绑定
    std::vector< std::pair< size_t, winux::Mixed > > _bindingParams; // 已绑定的参数
    std::vector< std::pair< winux::String, winux::Mixed > > _bindingNameParams; // 已绑定的名称参数

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
    virtual bool fetchRow( winux::MixedArray * fields ) override;
    virtual bool fetchRow( winux::StringMixedMap * fields ) override;
    virtual winux::String fieldName( size_t fieldIndex ) override;
    virtual size_t fieldsCount() override;
    virtual size_t rowsCount() override;
    virtual winux::String fieldType( size_t fieldIndex ) override;

    bool reset();
    operator bool() const { return _mysqlRes != NULL; }
    operator struct st_mysql_res * () const { return _mysqlRes; }
private:
    struct st_mysql_res * _mysqlRes;
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

    /*virtual bool addNew( winux::StringArray const & fieldNames, winux::MixedArray const & values );
    virtual bool addNew( winux::Mixed const & fields );
    virtual bool modify( winux::StringArray const & fieldNames, winux::MixedArray const & values, winux::Mixed const & prkValue );
    virtual bool modify( winux::Mixed const & fields, winux::Mixed const & prkValue );
    virtual int modifyEx( winux::StringArray const & fieldNames, winux::MixedArray const & values, winux::String const & where );
    virtual int modifyEx( winux::Mixed const & fields, winux::String const & where );
    virtual bool deleteOne( winux::Mixed const & prkValue );
    virtual int deleteEx( winux::String const & where );*/

protected:
    virtual void _getTableInfo() override;
/*    MysqlConnection * _cnn;
    winux::String _tableName;
    winux::StringArray _fieldNames; // 原始字段名
    winux::String _prkName;         // 主键名 */

    DISABLE_OBJECT_COPY(MysqlModifier)
};

}

#endif // __EIENDB_MYSQL_HPP__
