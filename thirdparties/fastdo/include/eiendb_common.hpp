/** 数据库相关支持类封装 */

#ifndef __EIENDB_COMMON_HPP__
#define __EIENDB_COMMON_HPP__

#include "eiendb_base.hpp"

namespace eiendb
{

/** \brief Mixed变量连接进SQL语句时的换码操作 */
EIENDB_FUNC_DECL(winux::String) MixedToEscapedStr( winux::Mixed const & m, IDBConnection * cnn );

/** \brief 内存结果集
 *
 *  主要用于兼容接口方法。
 *  譬如，某些数据库的某些数据并不能通过query得到，而是其他方法。
 *  但又需要用IDBResult接口去操作。 */
class EIENDB_DLL MemoryResult : public IDBResult
{
public:
    MemoryResult( winux::StringArray const & fields );

    #if defined(__GNUC__) || _MSC_VER > 1200
    template < winux::uint n >
    MemoryResult( winux::String (&fields)[n] ) : _curRowIndex(0), _fieldNames( fields, fields + n )
    {
    }
    #endif

    virtual ~MemoryResult();

    /** \brief 添加一数据行 */
    template < typename _Ty >
    int addRow( std::vector<_Ty> const & values )
    {
        winux::MixedArray newRow;
        typename std::vector<_Ty>::const_iterator it;
        for ( it = values.begin(); it != values.end(); ++it )
        {
            newRow.push_back(*it);
        }
        int index = _dataRows.size();
        _dataRows.push_back(newRow);
        return index;
    }

    #if defined(__GNUC__) || _MSC_VER > 1200
    template < typename _Ty, winux::uint n >
    int addRow( _Ty (&values)[n] )
    {
        winux::MixedArray newRow;
        int i;
        for ( i = 0; i < (int)n; ++i )
        {
            newRow.push_back(values[i]);
        }
        int index = _dataRows.size();
        _dataRows.push_back(newRow);
        return index;
    }
    #endif

    virtual bool dataSeek( int index );
    virtual bool fetchRow( winux::Mixed * fields, int type = 0 );
    virtual bool fetchRow( winux::MixedArray * fields );
    virtual bool fetchRow( winux::StringMixedMap * fields );
    virtual winux::String fieldName( int fieldIndex ) { return _fieldNames[fieldIndex]; }
    virtual bool free() { return true; }
    virtual winux::ulong fieldsCount() { return (int)_fieldNames.size(); }
    virtual winux::ulong rowsCount() { return (int)_dataRows.size(); }
    virtual winux::String fieldType( int fieldIndex );

private:
    MemoryResult( MemoryResult const & );
    MemoryResult & operator = ( MemoryResult const & );

    int _curRowIndex; // 当前行索引
    winux::StringArray _fieldNames; // 字段名称信息
    std::vector<winux::MixedArray> _dataRows;// 作为内部数据
};

/** \brief SQL脚本执行器,执行多条语句,保存结果和错误信息 */
class EIENDB_DLL SQLScript
{
private:
    winux::StringArray _sqlArr;
    winux::StringArray _errArr;
    //std::vector< winux::SharedPointer<IDBResult> > _resArr;
    IDBConnection * _cnn;

public:
    SQLScript( IDBConnection * cnn );

    /** \brief 加载SQL文本，返回识别的SQL条数 */
    winux::ulong loadSql( winux::String const & sqlText );

    /** \brief 加载SQL文件，返回识别的SQL条数 */
    winux::ulong load( winux::IFile * sqlFile );

    typedef bool (* ProgressCallback)( SQLScript * script, winux::ulong iCurSql, winux::String const & errStr, void * param );

    /** \brief 执行多条SQL，返回成功执行的SQL条数
     *
     *  \param progress 回调函数指示执行进度，返回值false表示终止执行，返回值true继续执行 */
    winux::ulong exec(
        bool onErrorNext = false,
        bool storeError = true,
        ProgressCallback progress = NULL,
        void * param = NULL
    );

    winux::StringArray const & sqls() { return this->_sqlArr; }
    winux::StringArray const & errors() { return this->_errArr; }
};

/** \brief 数据库备份器,把数据备份成SQL脚本,方便跨数据库转移 */
class EIENDB_DLL SQLBackup
{
private:
    /** \brief '目标'数据库连接,要备份/恢复的数据库 */
    IDBConnection * _cnn;
    /** \brief '兼容到'数据库连接,用于跨数据库导出SQL脚本 */
    IDBConnection * _compatible;
    /** \brief 导入/导出文件的接口. 注意读写权限 */
    winux::IFile * _file;

    winux::StringArray _tableNames;
public:
    /** \brief 备份器构造函数
     *  \param cnn 连接器对象
     *  \param file 操作的文件
     *  \param compatible 兼容连接用来使数据按它的规则导出
     *
     *  譬如MySQL导到SQLite. 此时cnn设为mysql, compatible设为sqlite.
     *  数据将按照sqlite的语法规则导出sql脚本, 以便sqlite数据库执行导入.(只对数据备份有效,结构备份不行) */
    SQLBackup( IDBConnection * cnn, winux::IFile * file, IDBConnection * compatible = NULL );

    /** \brief 备份进度种类 */
    enum BackupProgressType
    {
        bptNone, bptBackupData, bptBackupStruct
    };
    /** \brief 备份的进度数据 */
    struct BackupProgressData
    {
        enum BackupProgressType type;
        winux::String tblName;
        int tblIndex;
        int tblsCount;
        int rowIndex;  ///< vaild if type=bptBackupData
        int rowsCount; ///< vaild if type=bptBackupData
        BackupProgressData() : type(bptNone), tblIndex(0), tblsCount(0), rowIndex(0), rowsCount(0) { }
    };
    typedef bool (* ProgressCallback)( SQLBackup * backup, BackupProgressData * data, void * param );

    /** \brief 备份表结构 */
    void backupTableStructure( winux::String const & tableName );

    /** \brief 备份表数据
     *
     *  \param noDeleteFrom 指示要不要生成DELETE FROM语句，从而在恢复数据时保留原数据 */
    void backupTableData( winux::String const & tableName, bool noDeleteFrom = false, ProgressCallback progress = NULL, void * param = NULL );

    /** \brief 备份数据库 */
    void backupDb( bool backupStructure = true, bool noDeleteFrom = false, ProgressCallback progress = NULL, void * param = NULL );
    /** \brief 恢复数据库 */
    bool resumeDb( bool onErrorNext = false, SQLScript::ProgressCallback progress = NULL, void * param = NULL );
};

/** \brief 数据库操作 */
class EIENDB_DLL Database
{
public:
    /** \brief 构造一个数据库操作对象
        \param configParams 配置参数是一个Mixed(Collection)类型

        MYSQL：
        {
            driver: "mysql",
            host: 数据库ip地址,
            user: 用户名,
            pwd: 密码,
            dbname: 数据库名,
            charset: 校验字符集
        }
        \n
        SQLITE：
        {
            driver: "sqlite",
            path: 数据库路径,
            dbkey: 数据库密码,
            charset: 校验字符集
        } */
    Database( winux::Mixed const & configParams );
    virtual ~Database();
    /** \brief 判断数据库连接是否有效 */
    operator bool() const { return (bool)_cnn && (bool)*_cnn.get(); }
    /** \brief 获取Connection指针 */
    IDBConnection * cnn() { return this->_cnn.get(); }
    /** \brief 当以`->`操作Database对象时，实际转到对Connection指针的操作 */
    IDBConnection * operator -> () { return this->_cnn.get(); }
    /** \brief 创建一个Modifier对象以修改数据 */
    winux::SharedPointer<IDBModifier> mdf( winux::String const & tableName );
    /** \brief 获取配置参数的引用 */
    winux::Mixed const & config() const { return _configParams; }
    //winux::Mixed & config() { return _configParams; }
private:
    void _doCreateConnection();
    winux::Mixed _configParams;
    winux::SimplePointer<IDBConnection> _cnn;
};

}

#endif // __EIENDB_COMMON_HPP__
