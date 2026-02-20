#ifndef __EIENDB_COMMON_HPP__
#define __EIENDB_COMMON_HPP__

#include "eiendb_base.hpp"

namespace eiendb
{
/** \brief 内存结果集
 *
 *  主要用于兼容接口方法。\n
 *  譬如，某些数据库的某些数据并不能通过query得到，而是其他方法。但又需要用IDbResult接口去操作。 */
class EIENDB_DLL MemoryResult : public IDbResult
{
public:
    MemoryResult( winux::StringArray const & fields );

    #if defined(__GNUC__) || _MSC_VER > 1200
    template < size_t _N >
    MemoryResult( winux::String (&fields)[_N] ) : _curRowIndex(0), _fieldNames( fields, fields + _N )
    {
    }
    #endif

    virtual ~MemoryResult();

    virtual bool free() override { return true; }

    /** \brief 添加一数据行，返回索引值 */
    template < typename _Ty >
    size_t addRow( std::vector<_Ty> const & values )
    {
        winux::MixedArray newRow;
        typename std::vector<_Ty>::const_iterator it;
        for ( it = values.begin(); it != values.end(); ++it )
        {
            newRow.push_back(*it);
        }
        size_t index = _dataRows.size();
        _dataRows.push_back(newRow);
        return index;
    }

    #if defined(__GNUC__) || _MSC_VER > 1200
    template < typename _Ty, size_t _N >
    size_t addRow( _Ty (&values)[_N] )
    {
        winux::MixedArray newRow;
        size_t i;
        for ( i = 0; i < _N; ++i )
        {
            newRow.push_back(values[i]);
        }
        size_t index = _dataRows.size();
        _dataRows.push_back(newRow);
        return index;
    }
    #endif

    virtual bool dataSeek( size_t index ) override;
    virtual bool fetchRow( winux::Mixed * fields, int type = 0 ) override;
    virtual bool fetchArray( winux::MixedArray * fields ) override;
    virtual bool fetchMap( winux::StringMixedMap * fields ) override;
    virtual winux::String fieldName( size_t fieldIndex ) override { return _fieldNames[fieldIndex]; }
    virtual size_t fieldsCount() override { return _fieldNames.size(); }
    virtual size_t rowsCount() override { return _dataRows.size(); }
    virtual winux::String fieldType( size_t fieldIndex ) override;

private:
    size_t _curRowIndex; // 当前行索引
    winux::StringArray _fieldNames; // 字段名称信息
    std::vector<winux::MixedArray> _dataRows; // 作为内部数据

    DISABLE_OBJECT_COPY(MemoryResult)
};

/** \brief SQL脚本执行器。执行多条语句，保存结果和错误信息。 */
class EIENDB_DLL SqlScript
{
public:
    /** \brief 构造函数1
     *
     *  \param cnn 数据库连接 */
    SqlScript( IDbConnection * cnn );

    /** \brief 加载SQL文件，返回识别的SQL条数 */
    size_t load( winux::IFile * sqlFile );

    /** \brief SqlScript的进度回调函数指针类型 */
    typedef bool (* ProgressCallback)( SqlScript * script, size_t iCurSql, winux::String const & errStr, void * param );

    /** \brief 执行多条SQL，返回成功执行的SQL条数
     *
     *  \param onErrorNext 是否错误时执行下条SQL
     *  \param storeError 是否存下错误信息
     *  \param progress 回调函数指示执行进度，返回值false表示终止执行，返回值true继续执行
     *  \param param 回调函数的自定义参数
     *  \return size_t */
    size_t exec(
        bool onErrorNext = false,
        bool storeError = true,
        ProgressCallback progress = NULL,
        void * param = NULL
    );

    /** \brief SQL语句数组 */
    winux::StringArray const & sqls() { return this->_sqlArr; }

    /** \brief 错误信息数组 */
    winux::StringArray const & errors() { return this->_errArr; }

private:
    winux::StringArray _sqlArr;
    winux::StringArray _errArr;
    //std::vector< winux::SharedPointer<IDbResult> > _resArr;
    IDbConnection * _cnn;
};

/** \brief 数据库备份器。把数据备份成SQL脚本，方便跨数据库转移。 */
class EIENDB_DLL SqlBackup
{
public:
    /** \brief 备份器构造函数
     *
     *  譬如MySQL导到SQLite。此时cnn设为mysql，compatible设为sqlite。\n
     *  数据将按照sqlite的语法规则导出sql脚本，以便sqlite数据库执行导入（只对数据备份有效，结构备份不行）。
     *  \param cnn 连接器对象
     *  \param file 操作的文件
     *  \param compatible 兼容连接用来使数据按它的规则导出 */
    SqlBackup( IDbConnection * cnn, winux::IFile * file, IDbConnection * compatible = NULL );

    /** \brief 备份进度种类 */
    enum BackupProgressType
    {
        bptNone, //!< 不在备份
        bptBackupData, //!< 备份数据
        bptBackupStruct //!< 备份结构
    };

    /** \brief 备份的进度数据 */
    struct BackupProgressData
    {
        enum BackupProgressType type; //!< 备份进度的类型：数据备份，结构备份
        winux::String tblName; //!< 表名
        size_t tblIndex; //!< 表索引
        size_t tblsCount; //!< 表数
        size_t rowIndex;  //!< 行索引 vaild if type=bptBackupData
        size_t rowsCount; //!< 行数 vaild if type=bptBackupData
        BackupProgressData() : type(bptNone), tblIndex(0), tblsCount(0), rowIndex(0), rowsCount(0) { }
    };

    /** \brief SqlBackup的进度回调函数指针类型 */
    typedef bool (* ProgressCallback)( SqlBackup * backup, BackupProgressData * data, void * param );

    /** \brief 备份表结构 */
    void backupTableStructure( winux::String const & tableName );

    /** \brief 备份表数据
     *
     *  \param noDeleteFrom 指示要不要生成DELETE FROM语句，从而在恢复数据时保留原数据 */
    void backupTableData( winux::String const & tableName, bool noDeleteFrom = false, ProgressCallback progress = NULL, void * param = NULL );

    /** \brief 备份数据库 */
    void backupDb( bool backupStructure = true, bool noDeleteFrom = false, ProgressCallback progress = NULL, void * param = NULL );
    /** \brief 恢复数据库 */
    bool resumeDb( bool onErrorNext = false, SqlScript::ProgressCallback progress = NULL, void * param = NULL );

private:
    /** \brief '目标'数据库连接，要备份/恢复的数据库 */
    IDbConnection * _cnn;
    /** \brief '兼容到'数据库连接，用于跨数据库导出SQL脚本 */
    IDbConnection * _compatible;
    /** \brief 导入/导出文件的接口，注意读写权限 */
    winux::IFile * _file;
    /** \brief 表名数组 */
    winux::StringArray _tableNames;
};

/** \brief 数据库操作 */
class EIENDB_DLL Database
{
public:
    /** \brief 构造一个数据库操作对象
     *
     *  MYSQL：
     *  {
     *      driver: "mysql",
     *      host: 数据库IP地址[:端口号],
     *      user: 用户名,
     *      pwd: 密码,
     *      dbname: 数据库名,
     *      charset: 校验字符集
     *  }
     *  \n
     *  PGSQL：
     *  {
     *      driver: "pgsql",
     *      host: 数据库IP地址[:端口号][, ...],
     *      user: 用户名,
     *      pwd: 密码,
     *      dbname: 数据库名,
     *      charset: 校验字符集,
     *      schema: PostgreSQL模式名，默认"public",
     *      options: 传递到客户端命令行的选项，默认"",
     *      single_row: 单行查询模式 0:默认多行模式 1:保持单行模式 2:一次单行模式
     *  }
     *  \n
     *  SQLITE：
     *  {
     *      driver: "sqlite",
     *      path: 数据库路径,
     *      dbkey: 数据库密码,
     *      charset: 校验字符集
     *  }
     *  \param configParams 配置参数是一个Mixed(Collection)类型
     */
    Database( winux::Mixed const & configParams );
    virtual ~Database();
    /** \brief 判断数据库连接是否有效 */
    operator bool() const { return (bool)_cnn && (bool)*_cnn.get(); }
    /** \brief 获取Connection指针 */
    IDbConnection * cnn() { return this->_cnn.get(); }
    /** \brief 当以`->`操作Database对象时，实际转到对Connection指针的操作 */
    IDbConnection * operator -> () { return this->_cnn.get(); }
    /** \brief 创建一个Modifier对象以修改数据 */
    winux::SharedPointer<IDbModifier> mdf( winux::String const & tableName );
    /** \brief 获取配置参数的引用 */
    winux::Mixed const & config() const { return _configParams; }
    /** \brief 获取配置参数的引用 */
    winux::Mixed & config() { return _configParams; }

private:
    void _doCreateConnection();
    winux::Mixed _configParams;
    winux::SimplePointer<IDbConnection> _cnn;
};


} // namespace eiendb

#endif // __EIENDB_COMMON_HPP__
