#ifndef __EIENDB_BASE_HPP__
#define __EIENDB_BASE_HPP__

#include "winux.hpp"

/** \brief 数据库通用接口 */
namespace eiendb
{

#ifdef EIENDB_DLL_USE
    #if defined(_MSC_VER) || defined(WIN32)
        #pragma warning( disable: 4251 )
        #ifdef  EIENDB_DLL_EXPORTS
            #define EIENDB_DLL  __declspec(dllexport)
        #else
            #define EIENDB_DLL  __declspec(dllimport)
        #endif

        #define EIENDB_API __stdcall
    #else
        #define EIENDB_DLL
        #define EIENDB_API
    #endif
#else
    #define EIENDB_DLL
    #define EIENDB_API
#endif

#define EIENDB_FUNC_DECL(ret) EIENDB_DLL ret EIENDB_API
#define EIENDB_FUNC_IMPL(ret) ret EIENDB_API


#ifndef interface
#define interface struct
#endif

/** \brief 数据库各种对象资源的指针类型 */
typedef void * DBHandle;

/** \brief eiendb数据库错误 */
class EIENDB_DLL DbError : public winux::Error
{
public:
    enum
    {
        dbeNone, //!< 无错误
        dbeDbSelfError, //!< 数据库自身的错误
        dbeNotImplemented, //!< 方法未实现
        dbeConfigParamsError, //!< 配置参数错误
        dbeMysqlError, //!< MySQL数据库错误
        dbeSqliteError, //!< SQLite数据库错误
    };

    DbError( int errType, winux::AnsiString const & s ) throw() : winux::Error( errType, s ) { }
};

interface IDbConnection;
interface IDbStatement;
interface IDbResult;

/** \brief 数据库连接接口 */
interface EIENDB_DLL IDbConnection
{
    virtual ~IDbConnection() { }

    /** \brief 连接数据库 */
    virtual bool connect();

    /** \brief 关闭连接 */
    virtual bool close();

    /** \brief 确保该连接有效（还活着） */
    virtual void alive();

    /** \brief 选定要操作的数据库
     *
     *  \param database string 数据库名
     *  \return boolean 是否成功 */
    virtual bool selectDb( winux::String const & database );

    /** \brief 设置连接校验字符集
     *
     *  \param charset string 字符集
     *  \pre 连接数据库之后
     *  \return boolean */
    virtual bool setLinkCharset( winux::String const & charset );

    /** \brief 受影响的行数
     *
     *  \pre 在改变数据库内容或结构之后
     *  \return size_t */
    virtual size_t affectedRows();

    /** \brief 创建一个库
     *
     *  \param database string 数据库名
     *  \return boolean */
    virtual bool createDb( winux::String const & database );

    /** \brief 删除数据库
     *
     *  \param database string 数据库名
     *  \return boolean */
    virtual bool dropDb( winux::String const & database );

    /** \brief 获得错误号
     *
     *  \return int
     *  \retval 0 没有错误
     *  \retval 非0 出错,可用 \b error() 查看错误信息 */
    virtual int errNo();

    /** \brief 获得错误信息
     *
     *  \return string */
    virtual winux::String error();

    /** \brief 查询数据结果
     *
     *  \param sql string
     *  \return IDbResult */
    virtual winux::SharedPointer<IDbResult> query( winux::String const & sql );
    virtual winux::SharedPointer<IDbResult> query( winux::SharedPointer<IDbStatement> stmt );

    /** \brief 不需要数据的查询
     *
     *  这函数可以用来执行INSERT、UPDATE、DROP等等一些不需要查询数据的操作
     *  \param sql string
     *  \return bool 是否成功，改变的行数可用affectedRows()取得 */
    virtual bool exec( winux::String const & sql );
    virtual bool exec( winux::SharedPointer<IDbStatement> stmt );

    /** \brief 获得最后一次完成记录插入时的ID值
     *
     *  您可以用执行SQL里的LAST_INSERT_ID()代替此函数
     *  \return size_t */
    virtual size_t insertId();

    /** \brief 普通文本转成SQL语句安全的文本，用做SQL语句的字符串
     *
     *  \param str string 需要换码的字符串
     *  \param addQuote string 表示需要在首尾加上的引号，为空则不加
     *  \return string */
    virtual winux::String escape( winux::String const & str, winux::String const & addQuote = "\'" );

    /** \brief 二进制数据转成SQL语句安全的文本，用做SQL语句的字符串
     *
     *  \param buf void const *
     *  \param size size_t
     *  \param addQuote winux::String const &
     *  \return winux::String */
    virtual winux::String escape( void const * buf, size_t size, winux::String const & addQuote = "\'" );

    /** \brief 获得所有数据库的一个结果集
     *
     *  \return IDbResult */
    virtual winux::SharedPointer<IDbResult> listDbs();

    /** \brief 获得数据库里某表所有字段的一个结果集
     *
     *  \param tableName 表名
     *  \return IDbResult */
    virtual winux::SharedPointer<IDbResult> listFields( winux::String const & tableName );

    /** \brief 获得数据库里所有表的一个结果集
     *
     *  \return IDbResult */
    virtual winux::SharedPointer<IDbResult> listTables();

    /** \brief 取得一张表的DDL */
    virtual winux::String tableDdl( winux::String const & tableName );

    /** \brief 加入标识符的引用标记,如表名,字段名等等
     *
     *  \return string 加入左右俩标记到标识符 */
    virtual winux::String symbolQuotes( winux::String const & str );

    /** \brief 加载SQL到一个字符串数组，返回语句条数 */
    virtual size_t loadSql( winux::String const & sqlText, winux::StringArray * arrSql );
    /** \brief 加载SQL文件到一个字符串数组，返回语句条数 */
    virtual size_t loadSqlFile( winux::IFile * sqlScriptFile, winux::StringArray * arrSql );
    /** \brief 取得表的主键，返回主键字段数 */
    virtual size_t getPrimaryKey( winux::String const & tableName, winux::StringArray * arrKeyColumn );

    /** \brief 构造一个语句对象 */
    virtual winux::SharedPointer<IDbStatement> buildStmt( winux::String const & sql );
    /** \brief 构造一个语句对象 */
    virtual winux::SharedPointer<IDbStatement> buildStmt( winux::String const & sql, winux::Mixed const & params );
    /** \brief 构造一个语句对象 */
    virtual winux::SharedPointer<IDbStatement> buildStmt( winux::String const & sql, winux::MixedArray const & params );
    /** \brief 构造一个语句对象 */
    virtual winux::SharedPointer<IDbStatement> buildStmt( winux::String const & sql, winux::StringMixedMap const & params );

    /** \brief 判断一个连接是否有效 */
    virtual operator bool() const = 0;
};

/** \brief SQL语句接口,构造SQL语句,参数绑定 */
interface EIENDB_DLL IDbStatement
{
    virtual ~IDbStatement() { }
    /** \brief 建立一个SQL语句 */
    virtual bool build( winux::String const & sql );
    /** \brief 建立一个SQL语句，params可以是array或者collection类型 */
    virtual bool build( winux::String const & sql, winux::Mixed const & params );
    /** \brief 建立一个SQL语句，绑定参数，params数组的0索引对应参数索引1，依此类推 */
    virtual bool build( winux::String const & sql, winux::MixedArray const & params );
    /** \brief 建立一个SQL语句，绑定参数，SQL中参数名前要加':' */
    virtual bool build( winux::String const & sql, winux::StringMixedMap const & params );
    /** \brief 绑定参数 参数索引起始为1 */
    virtual bool bind( size_t paramIndex, winux::Mixed const & val );
    /** \brief 绑定参数 按参数名 参数名前需要加':' */
    virtual bool bind( winux::String const & paramName, winux::Mixed const & val );
};

/**    \brief 数据结果操作接口 */
interface EIENDB_DLL IDbResult
{
    virtual ~IDbResult() { }

    /** \brief 数据记录定位
     *
     *  \param index int 0为第一条记录
     *  \return boolean */
    virtual bool dataSeek( size_t index );

    /** \brief 提取为数组或者映射表
     *
     *  \param type int 0:为映射表，1:为数组 */
    virtual bool fetchRow( winux::Mixed * fields, int type = 0 );

    /** \brief 提取为数组(by index) */
    virtual bool fetchRow( winux::MixedArray * fields );

    /** \brief 提取为映射表(by fieldname) */
    virtual bool fetchRow( winux::StringMixedMap * fields );

    /** \brief 获取结果集中指定字段的名称
     *
     *  \param fieldIndex int 字段索引
     *  \return string */
    virtual winux::String fieldName( size_t fieldIndex );

    /** \brief 释放Result资源
     *
     *  \return boolean */
    virtual bool free();

    /** \brief 获取结果里的字段数
     *
     *  \return size_t */
    virtual size_t fieldsCount();

    /** \brief 获取结果里的记录数
     *
     *  \return size_t */
    virtual size_t rowsCount();

    /** \brief 获取在一个结果集中指定字段的类型
     *
     *  \param fieldIndex int 字段索引
     *  \return string */
    virtual winux::String fieldType( size_t fieldIndex );

    /*  \brief 获取一个结果集中指定字段相关的标记
    \param fieldIndex int 字段索引
    \return string
    virtual winux::String fieldFlags( int fieldIndex )
    {
        throw DBException("fieldFlags() not implemented");
    }*/

    /* \brief 返回结果集中指定字段的长度
     *  \param fieldIndex int 字段索引
     *  \return int
    virtual int fieldLen( int fieldIndex )
    {
        throw DBException("fieldLen() not implemented");
    }*/

    /* \brief 获取指定字段所在表的名称
     *  \param fieldIndex int 字段索引
     *  \return string
    virtual winux::String fieldTable( int fieldIndex )
    {
        throw DBException("fieldTable() not implemented");
    }*/

    /* \brief 从结果集里获取一个单元格的内容
    \param row int 记录索引
    \param field int 字段索引
    \return string
    virtual winux::String result( int row, int field )
    {
        throw DBException("result() not implemented");
    }*/

};

/** \brief 修改器接口 */
interface EIENDB_DLL IDbModifier
{
    virtual ~IDbModifier() { }

    /** \brief 添加新记录
     *
     *  \param fieldNames 字段名
     *  \param values 对应值
     *  \return bool */
    virtual bool addNew( winux::StringArray const & fieldNames, winux::MixedArray const & values );

    /** \brief 添加新记录
     *
     *  \param fields collection类型：字段名=>值；array类型：和数据库字段对应的数据
     *  \return bool */
    virtual bool addNew( winux::Mixed const & fields );

    /** \brief 修改一条记录,用主键来指定数据记录
     *
     *  \param fieldNames 字段名
     *  \param values 对应值
     *  \param prkValue 主键值
     *  \return bool */
    virtual bool modify( winux::StringArray const & fieldNames, winux::MixedArray const & values, winux::Mixed const & prkValue );

    /** \brief 修改一条记录,用主键来指定数据记录
     *
     *  \param fields 必须为collection类型：字段名=>对应值
     *  \param prkValue 主键值
     *  \return bool */
    virtual bool modify( winux::Mixed const & fields, winux::Mixed const & prkValue );

    /** \brief 修改记录,用where子句来指定数据记录
     *
     *  \param fieldNames 字段名
     *  \param values 对应值
     *  \param where SQL语句的WHERE子句, 不能含WHERE关键字
     *  \return bool */
    virtual bool modifyEx( winux::StringArray const & fieldNames, winux::MixedArray const & values, winux::String const & where );

    /** \brief 修改记录,用where子句来指定数据记录
     *
     *  \param fields 必须为collection类型：字段名=>对应值
     *  \param where SQL语句的WHERE子句, 不能含WHERE关键字
     *  \return bool */
    virtual bool modifyEx( winux::Mixed const & fields, winux::String const & where );

    /** \brief 删除一条记录,用主键来指定数据记录
     *
     *  \param prkValue mixed 主键值
     *  \return bool */
    virtual bool deleteOne( winux::Mixed const & prkValue );

    /** \brief 删除一条记录,用where子句来指定数据记录
     *
     *  \param where SQL语句的WHERE子句,不能含WHERE关键字.
     *  \return bool 是否成功执行，删除的记录数用IDBConnection::affectedRows()取得 */
    virtual bool deleteEx( winux::String const & where );
};

/** \brief Mixed变量连接进SQL语句时的换码操作 */
EIENDB_FUNC_DECL(winux::String) MixedToEscapedStr( winux::Mixed const & m, IDbConnection * cnn, winux::String const & addQuote = "\'" );

/** \brief 数据库修改器部分通用实现 */
class EIENDB_DLL DbModifier : public IDbModifier
{
public:
    DbModifier( IDbConnection * cnn, winux::String const & tableName );
    virtual ~DbModifier();

    virtual bool addNew( winux::StringArray const & fieldNames, winux::MixedArray const & values ) override;
    virtual bool addNew( winux::Mixed const & fields ) override;
    virtual bool modify( winux::StringArray const & fieldNames, winux::MixedArray const & values, winux::Mixed const & prkValue ) override;
    virtual bool modify( winux::Mixed const & fields, winux::Mixed const & prkValue ) override;
    virtual bool modifyEx( winux::StringArray const & fieldNames, winux::MixedArray const & values, winux::String const & where ) override;
    virtual bool modifyEx( winux::Mixed const & fields, winux::String const & where ) override;
    virtual bool deleteOne( winux::Mixed const & prkValue ) override;
    virtual bool deleteEx( winux::String const & where ) override;

protected:
    virtual void _getTableInfo() = 0;
    IDbConnection * _cnn;
    winux::String _tableName;
    winux::StringArray _prkColumn; // 主键
    winux::StringArray _fieldNames; // 原始字段名
    //bool _prkAutoIncrement;  // 自动增长

    DISABLE_OBJECT_COPY(DbModifier)
};


}

#endif // __EIENDB_BASE_HPP__
