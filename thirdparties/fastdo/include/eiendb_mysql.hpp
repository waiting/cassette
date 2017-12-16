#ifndef __EIENDB_MYSQL_HPP__
#define __EIENDB_MYSQL_HPP__

#include "eiendb-base.hpp"

struct st_mysql;
struct st_mysql_res;
struct st_mysql_stmt;

namespace eiendb
{
/** \brief MySQL数据库错误 */
class EIENDB_DLL MySQLDbError: public DbError
{
    int _mysqlErrNo;
public:
    MySQLDbError( int mysqlErrNo, winux::AnsiString const & s ) : DbError( DbError::dbeDbSelfError, s ), _mysqlErrNo(mysqlErrNo) { }
    int getMySqlErrCode() const throw() { return _mysqlErrNo; }
};

class EIENDB_DLL MySQLConnection : public IDBConnection
{
public:
    MySQLConnection( winux::String const & host, winux::String const & user, winux::String const & pwd, winux::String const & dbName, winux::String const & linkCharset, bool doConnect = true );
    virtual ~MySQLConnection();

    virtual bool connect();
    virtual bool close();
    virtual void alive();
    virtual bool setLinkCharset( winux::String const & charset );
    virtual bool selectDb( winux::String const & database );
    virtual bool createDb( winux::String const & database );
    virtual bool dropDb( winux::String const & database );
    virtual winux::ulong affectedRows();
    virtual winux::ulong insertId();
    virtual int errNo();
    virtual winux::String error();
    virtual winux::String escape( winux::String const & str, winux::String const & addQuote = "\'" );
    virtual winux::String escape( winux::byte const * buf, int size, winux::String const & addQuote = "\'" );
    virtual winux::String symbolQuotes( winux::String const & str );
    virtual bool exec( winux::String const & sql );
    virtual bool exec( winux::SharedPointer<IDBStatement> stmt );
    virtual winux::SharedPointer<IDBResult> query( winux::String const & sql );
    virtual winux::SharedPointer<IDBResult> query( winux::SharedPointer<IDBStatement> stmt );
    virtual winux::SharedPointer<IDBResult> listDbs();
    virtual winux::SharedPointer<IDBResult> listFields( winux::String const & tableName );
    virtual winux::SharedPointer<IDBResult> listTables();
    virtual winux::String tableDdl( winux::String const & tableName );

    virtual winux::SharedPointer<IDBStatement> buildStmt( winux::String const & sql );
    virtual winux::SharedPointer<IDBStatement> buildStmt( winux::String const & sql, winux::Mixed const & params );
    virtual winux::SharedPointer<IDBStatement> buildStmt( winux::String const & sql, winux::MixedArray const & params );
    virtual winux::SharedPointer<IDBStatement> buildStmt( winux::String const & sql, winux::StringMixedMap const & params );

    virtual operator bool() const { return _mysql != NULL; }
    operator struct st_mysql * () const { return _mysql; }
private:
    struct st_mysql * _mysql; // 代表MYSQL连接
    winux::String _hostAddr;  // 主机地址
    int _port;         // 端口
    winux::String _user;      // 用户名
    winux::String _pwd;       // 密码
    winux::String _dbName;    // 数据库名
    winux::String _linkCharset; // 连接校验字符集

    DISABLE_OBJECT_COPY(MySQLConnection)
};

class EIENDB_DLL MySQLStatement : public IDBStatement
{
public:
    MySQLStatement( MySQLConnection * cnn );
    MySQLStatement( MySQLConnection * cnn, winux::String const & sql );
    MySQLStatement( MySQLConnection * cnn, winux::String const & sql, winux::Mixed const & params );
    MySQLStatement( MySQLConnection * cnn, winux::String const & sql, winux::MixedArray const & params );
    MySQLStatement( MySQLConnection * cnn, winux::String const & sql, winux::StringMixedMap const & params );
    virtual ~MySQLStatement();
    /** 建立一个SQL语句 */
    virtual bool build( winux::String const & sql );
    virtual bool build( winux::String const & sql, winux::Mixed const & params );
    virtual bool build( winux::String const & sql, winux::MixedArray const & params );
    virtual bool build( winux::String const & sql, winux::StringMixedMap const & params );
    /** 绑定参数 参数索引起始为1 */
    virtual bool bind( int paramIndex, winux::Mixed const & val );
    /** 绑定参数 按参数名 参数名前需要加: */
    virtual bool bind( winux::String const & paramName, winux::Mixed const & val );

    operator bool() const { return !_orgSql.empty(); }
    MySQLConnection * getCnn() const { return _cnn; }
    winux::String const & getSql();

    // paramIndex base on 1 
    winux::Mixed const & getParam( int paramIndex ) const;
    // paramName prefix with ':'
    winux::Mixed const & getParam( winux::String const & paramName ) const;
private:
    void _generateSql();

    MySQLConnection * _cnn;
    winux::String _orgSql; // 原SQL
    winux::String _sql;
    bool _isGeneratedSql; // 是否已经产生SQL
    bool _isNameBinding; // 是否采用参数名称绑定
    std::vector< std::pair< int, winux::Mixed > > _bindingParams; // 已绑定的参数
    std::vector< std::pair< winux::String, winux::Mixed > > _bindingNameParams; // 已绑定的名称参数

    DISABLE_OBJECT_COPY(MySQLStatement)
};

class EIENDB_DLL MySQLResult : public IDBResult
{
public:
    MySQLResult( winux::SharedPointer<MySQLStatement> stmt );
    virtual ~MySQLResult();
    virtual bool free();

    virtual bool dataSeek( int index );
    virtual bool fetchRow( winux::Mixed * fields, int type = 0 );
    virtual bool fetchRow( winux::MixedArray * fields );
    virtual bool fetchRow( winux::StringMixedMap * fields );
    virtual winux::String fieldName( int fieldIndex );
    virtual winux::ulong fieldsCount();
    virtual winux::ulong rowsCount();
    virtual winux::String fieldType( int fieldIndex );

    bool reset();
    operator bool() const { return _mysqlRes != NULL; }
    operator struct st_mysql_res * () const { return _mysqlRes; }
private:
    struct st_mysql_res * _mysqlRes;
    winux::SharedPointer<MySQLStatement> _stmt;
    winux::StringArray _fieldNames; // 非原始字段名
    std::vector<winux::uint> _fieldTypes; // 字段类型
    std::vector<winux::uint> _fieldFlags; // 字段Flag

    DISABLE_OBJECT_COPY(MySQLResult)
};

class EIENDB_DLL MySQLModifier : public DBModifier
{
public:
    MySQLModifier( MySQLConnection * cnn, winux::String const & tableName );
    virtual ~MySQLModifier();

/*    virtual bool addNew( winux::StringArray const & fieldNames, winux::MixedArray const & values );
    virtual bool addNew( winux::Mixed const & fields );
    virtual bool modify( winux::StringArray const & fieldNames, winux::MixedArray const & values, winux::Mixed const & prkValue );
    virtual bool modify( winux::Mixed const & fields, winux::Mixed const & prkValue );
    virtual int modifyEx( winux::StringArray const & fieldNames, winux::MixedArray const & values, winux::String const & where );
    virtual int modifyEx( winux::Mixed const & fields, winux::String const & where );
    virtual bool deleteOne( winux::Mixed const & prkValue );
    virtual int deleteEx( winux::String const & where );*/

protected:
    virtual void _getTableInfo();
/*    MySQLConnection * _cnn;
    winux::String _tableName;
    winux::StringArray _fieldNames; // 原始字段名
    winux::String _prkName;         // 主键名 */

    DISABLE_OBJECT_COPY(MySQLModifier)
};

}

#endif // __EIENDB_MYSQL_HPP__
