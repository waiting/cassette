#include "eiendb_common.hpp"
#include "eiendb_mysql.hpp"
#include "eiendb_sqlite.hpp"
#include "eiendb_pgsql.hpp"
#include <algorithm>
#include <math.h>

namespace eiendb
{

// class MemoryResult -----------------------------------------------------------------------
MemoryResult::MemoryResult( winux::StringArray const & fields ) : _curRowIndex(0), _fieldNames(fields)
{
}

MemoryResult::~MemoryResult()
{
    this->free();
}

bool MemoryResult::dataSeek( size_t index )
{
    _curRowIndex = index;
    return _curRowIndex >= 0 && _curRowIndex < this->rowsCount();
}

bool MemoryResult::fetchArray( winux::MixedArray * fields )
{
    if ( _curRowIndex >= 0 && _curRowIndex < this->rowsCount() )
    {
        ASSIGN_PTR(fields) = _dataRows[_curRowIndex];
        _curRowIndex++;
        return true;
    }
    return false;
}

bool MemoryResult::fetchMap( winux::StringMixedMap * fields )
{
    if ( _curRowIndex >= 0 && _curRowIndex < this->rowsCount() )
    {
        if ( fields != nullptr )
        {
            fields->clear();
            size_t i;
            size_t cntFields = this->fieldsCount();
            winux::MixedArray & row = _dataRows[_curRowIndex];
            for ( i = 0; i < cntFields; ++i )
            {
                fields->operator [] (_fieldNames[i]) = row[i];
            }
        }
        _curRowIndex++;
        return true;
    }
    return false;
}

bool MemoryResult::fetchRow( winux::Mixed * fields, int type )
{
    if ( _curRowIndex >= 0 && _curRowIndex < this->rowsCount() )
    {
        if ( fields != nullptr )
        {
            if ( type == 0 ) // is Collection
            {
                fields->createCollection();
                size_t i;
                size_t cntFields = this->fieldsCount();
                winux::MixedArray & row = _dataRows[_curRowIndex];
                for ( i = 0; i < cntFields; ++i )
                {
                    fields->operator [] (_fieldNames[i]) = row[i];
                }
            }
            else if ( type == 1 ) // is Array
            {
                fields->createArray();
                *fields->_pArr = _dataRows[_curRowIndex];
            }
        }
        _curRowIndex++;
        return true;
    }
    return false;
}

// Value is in one of null/blob/integer/real/text
winux::String MemoryResult::fieldType( size_t fieldIndex )
{
    if ( this->rowsCount() > 0 )
    {
        switch ( _dataRows[0][fieldIndex]._type )
        {
        case winux::Mixed::MT_NULL:
            return $T("null");
        case winux::Mixed::MT_BOOLEAN:
        case winux::Mixed::MT_BYTE:
        case winux::Mixed::MT_SHORT:
        case winux::Mixed::MT_USHORT:
        case winux::Mixed::MT_INT:
        case winux::Mixed::MT_UINT:
        case winux::Mixed::MT_LONG:
        case winux::Mixed::MT_ULONG:
        case winux::Mixed::MT_INT64:
        case winux::Mixed::MT_UINT64:
            return $T("integer");
        case winux::Mixed::MT_FLOAT:
        case winux::Mixed::MT_DOUBLE:
            return $T("real");
        case winux::Mixed::MT_ANSI:
        case winux::Mixed::MT_UNICODE:
            return $T("text");
        case winux::Mixed::MT_ARRAY:
        case winux::Mixed::MT_BINARY:
            return $T("blob");
        }
    }
    return $T("");
}

// class SqlScript ---------------------------------------------------------------
SqlScript::SqlScript( IDbConnection * cnn ) : _cnn(cnn)
{

}

size_t SqlScript::load( winux::IFile * sqlFile )
{
    _sqlArr.clear();
    return _cnn->loadSqlFile( sqlFile, &_sqlArr );
}

size_t SqlScript::exec( bool onErrorNext, bool storeError, ProgressCallback progress, void * param )
{
    _errArr.clear();
    size_t execSql = 0; // 已执行SQL数
    size_t i;
    size_t nSqls = _sqlArr.size();
    for ( i = 0; i < nSqls; ++i )
    {
        try
        {
            _cnn->exec( _sqlArr[i] );
        }
        catch ( DbError const & )
        {
        }

        winux::String errStr;
        if ( _cnn->errNo() )
        {
            errStr = _cnn->error();
        }

        if ( storeError )
            _errArr.push_back(errStr);

        if ( progress != nullptr )
        {
            if ( !(*progress)( this, i, errStr, param ) )
            {
                break;
            }
        }

        if ( _cnn->errNo() && !onErrorNext ) // if occur error
            break;

        ++execSql;
    }
    return execSql;
}

// class SqlBackup -----------------------------------------------------------------------
SqlBackup::SqlBackup( IDbConnection * cnn, winux::IFile * file, IDbConnection * compatible )
: _cnn(cnn), _compatible(compatible), _file(file)
{
    if ( _compatible == nullptr )
    {
        _compatible = _cnn;
    }
}

void SqlBackup::backupTableStructure( winux::String const & tableName )
{
    _file->puts("\n");
    _file->puts("-- -----------------------------\n");
    winux::String tbl = _compatible->symbolQuotes(tableName);
    _file->puts( "-- Table " + tbl + " structure;\n" );
    _file->puts("-- -----------------------------\n");
    _file->puts( "DROP TABLE IF EXISTS " + tbl + ";\n" );
    _file->puts( _cnn->tableDdl(tableName) );
}

void SqlBackup::backupTableData( winux::String const & tableName, bool noDeleteFrom, ProgressCallback progress, void * param )
{
    _file->puts("\n");
    _file->puts("-- -----------------------------\n");
    winux::String tbl = _compatible->symbolQuotes(tableName);
    _file->puts( "-- Table " + tbl + " data;\n" );
    _file->puts("-- -----------------------------\n");
    if ( !noDeleteFrom )
        _file->puts( "DELETE FROM " + tbl + ";\n" );

    winux::String sql = "INSERT INTO " + tbl + " VALUES (";

    // 要进行分页处理，否则内存消耗过大
    size_t nAllRecords = 0;
    {
        winux::SharedPointer<IDbResult> resAllRecords = _cnn->query( "SELECT COUNT(*) FROM " + _cnn->symbolQuotes(tableName) + ";" );
        winux::MixedArray row;
        if ( resAllRecords->fetchArray(&row) ) nAllRecords = row[0];
    }

    // 总页数
    size_t p = 0;
    size_t ppc = 1000;
    size_t pc = (size_t)ceil( nAllRecords / (double)ppc );

    // 进度数据
    BackupProgressData progressData; // 进度数据
    progressData.type = bptBackupData; // 当前执行进度的类型
    progressData.rowsCount = nAllRecords; // 获取总数
    progressData.rowIndex = 0;
    progressData.tblName = tableName;
    progressData.tblsCount = _tableNames.size();
    if ( progressData.tblsCount > 0 )
    {
        // 判断表索引是多少
        winux::StringArray::const_iterator iter;
        if ( ( iter = std::find( _tableNames.begin(), _tableNames.end(), tableName ) ) != _tableNames.end() )
        {
            progressData.tblIndex = iter - _tableNames.begin();
        }
        else
        {
            progressData.tblIndex = winux::npos;
        }
    }

    while ( p < pc )
    {
        _file->puts( winux::Format( "-- Begin %d / %d ------------------------\n", p + 1, pc ) );
        winux::SharedPointer<IDbResult> res = _cnn->query( "SELECT * FROM " + _cnn->symbolQuotes(tableName) + winux::Format(" LIMIT %d, %d;", p * ppc, ppc ) );
        winux::MixedArray f;
        winux::StringArray valueArr;

        while ( res->fetchArray(&f) )
        {
            valueArr.clear();
            for ( winux::MixedArray::const_iterator it = f.begin(); it != f.end(); ++it )
            {
                valueArr.push_back( MixedToEscapedStr( *it, _compatible ) );
            }
            _file->puts( sql + winux::StrJoin( ",", valueArr ) + ");\n" );
            // 如果有进度函数则调用,如果进度函数返回false,则终止
            if ( progress && !(*progress)( this, &progressData, param ) )
            {
                break;
            }
            progressData.rowIndex++;
        }

        _file->puts( winux::Format( "-- End %d / %d --------------------------\n", p + 1, pc ) );

        ++p; // 页号增1
    }
}

void SqlBackup::backupDb( bool backupStructure, bool noDeleteFrom, ProgressCallback progress, void * param )
{
    _file->puts("-- ------------------------------------------------------\n");
    _file->puts("-- This SQL file generated by Eien database backup tools\n");
    _file->puts("-- Eien database backup tools version 1.2.0\n");
    _file->puts("-- \n");
    _file->puts("--  Author: WT\n");
    _file->puts("--      QQ: 162057326\n");
    _file->puts("-- WebSite: fastdo.net\n");
    _file->puts("-- ------------------------------------------------------\n");

    winux::SharedPointer<IDbResult> res = _cnn->listTables();
    _tableNames.clear();
    winux::MixedArray f;
    while ( res->fetchArray(&f) ) _tableNames.push_back(f[0]);

    if ( backupStructure )
    {
        BackupProgressData progressData; // 进度数据
        progressData.type = bptBackupStruct;
        //if ( progress )
        {
            progressData.tblsCount = _tableNames.size();
        }
        size_t i = 0;
        for ( winux::StringArray::const_iterator it = _tableNames.begin(); it != _tableNames.end(); ++it )
        {
            this->backupTableStructure(*it);
            progressData.tblIndex = i;
            progressData.tblName = *it;
            if ( progress && !(*progress)( this, &progressData, param ) )
            {
                break;
            }
            ++i;
        }
    }

    for ( winux::StringArray::const_iterator it = _tableNames.begin(); it != _tableNames.end(); ++it )
    {
        this->backupTableData( *it, noDeleteFrom, progress, param );
    }
}

bool SqlBackup::resumeDb( bool onErrorNext, SqlScript::ProgressCallback progress, void * param )
{
    SqlScript script(_cnn);
    script.load(_file);
    return script.sqls().size() == script.exec( onErrorNext, true, progress, param );
}

// class Database -------------------------------------------------------------------------
Database::Database( winux::Mixed const & configParams ) : _configParams(configParams)
{
    this->_doCreateConnection();
}

Database::~Database()
{

}

winux::SharedPointer<IDbModifier> Database::mdf( winux::String const & tableName )
{
    if ( (bool)*this ) // 必须已经创建了有效的数据库连接
    {
        winux::String driver = _configParams["driver"];
        winux::StrMakeLower(&driver);
        if ( driver == "mysql" )
        {
        #ifdef HAVE_DB_MYSQL
            return winux::SharedPointer<IDbModifier>( new MysqlModifier( static_cast<MysqlConnection *>( this->_cnn.get() ), tableName ) );
        #endif
        }
        else if ( driver == "sqlite" )
        {
        #ifdef HAVE_DB_SQLITE
            return winux::SharedPointer<IDbModifier>( new SqliteModifier( static_cast<SqliteConnection *>( this->_cnn.get() ), tableName ) );
        #endif
        }
        else if ( driver == "pgsql" )
        {
        #ifdef HAVE_DB_PGSQL
            return winux::SharedPointer<IDbModifier>( new PgsqlModifier( static_cast<PgsqlConnection *>( this->_cnn.get() ), tableName ) );
        #endif
        }
    }
    return winux::SharedPointer<IDbModifier>();
}

void Database::_doCreateConnection()
{
    // 验证配置参数类型
    if ( _configParams.isCollection() )
    {
    }
    else if ( _configParams.isString() )
    {
        _configParams = winux::Json(_configParams);
    }
    else
    {
        // 创建数据库连接时配置参数的类型错误
        throw DbError( DbError::dbeConfigParamsError, "Wrong type of configuration parameter when creating a database connection" );
        return;
    }

    if ( !_configParams.has("driver") )
    {
        // 没有找到参数`driver`，也就是数据库驱动程序的名称
        throw DbError( DbError::dbeConfigParamsError, "The `driver` parameter, which is the name of the database driver, was not found" );
        return;
    }

    winux::String driver = _configParams["driver"];
    winux::StrMakeLower(&driver);

    if ( driver.empty() )
    {
        throw DbError( DbError::dbeConfigParamsError, "The database driver name is empty" );
        return;
    }
#ifdef HAVE_DB_MYSQL
    else if ( driver == "mysql" ) // MYSQL
    {
        if ( !_configParams.has("host") )
        {
            throw DbError( DbError::dbeConfigParamsError, "The `host` parameter was not found, it is used by the MySQL driver" );
            return;
        }
        if ( !_configParams.has("user") )
        {
            throw DbError( DbError::dbeConfigParamsError, "The `user` parameter was not found, it is used by the MySQL driver" );
            return;
        }
        if ( !_configParams.has("pwd") )
        {
            throw DbError( DbError::dbeConfigParamsError, "The `pwd` parameter was not found, it is used by the MySQL driver" );
            return;
        }
        if ( !_configParams.has("dbname") )
        {
            throw DbError( DbError::dbeConfigParamsError, "The `dbname` parameter was not found, it is used by the MySQL driver" );
            return;
        }
        if ( !_configParams.has("charset") )
        {
            throw DbError( DbError::dbeConfigParamsError, "The `charset` parameter was not found, it is used by the MySQL driver" );
            return;
        }

        _cnn = winux::MakeSimple( new MysqlConnection(
            _configParams["host"],
            _configParams["user"],
            _configParams["pwd"],
            _configParams["dbname"],
            _configParams["charset"]
        ) );
    }
#endif
#ifdef HAVE_DB_SQLITE
    else if ( driver == "sqlite" ) // SQLITE
    {
        if ( !_configParams.has("path") )
        {
            throw DbError( DbError::dbeConfigParamsError, "The `path` parameter was not found, it is used by the SQLite driver" );
            return;
        }
        if ( !_configParams.has("dbkey") )
        {
            throw DbError( DbError::dbeConfigParamsError, "The `dbkey` parameter was not found, it is used by the SQLite driver" );
            return;
        }
        if ( !_configParams.has("charset") )
        {
            throw DbError( DbError::dbeConfigParamsError, "The `charset` parameter was not found, it is used by the SQLite driver" );
            return;
        }

        _cnn = winux::MakeSimple( new SqliteConnection(
            _configParams["path"],
            _configParams["dbkey"],
            _configParams["charset"]
        ) );
    }
#endif
#ifdef HAVE_DB_PGSQL
    else if ( driver == "pgsql" )
    {
        if ( !_configParams.has("host") )
        {
            throw DbError( DbError::dbeConfigParamsError, "The `host` parameter was not found, it is used by the PostgreSQL driver" );
            return;
        }
        if ( !_configParams.has("user") )
        {
            throw DbError( DbError::dbeConfigParamsError, "The `user` parameter was not found, it is used by the PostgreSQL driver" );
            return;
        }
        if ( !_configParams.has("pwd") )
        {
            throw DbError( DbError::dbeConfigParamsError, "The `pwd` parameter was not found, it is used by the PostgreSQL driver" );
            return;
        }
        if ( !_configParams.has("dbname") )
        {
            throw DbError( DbError::dbeConfigParamsError, "The `dbname` parameter was not found, it is used by the PostgreSQL driver" );
            return;
        }
        if ( !_configParams.has("charset") )
        {
            throw DbError( DbError::dbeConfigParamsError, "The `charset` parameter was not found, it is used by the PostgreSQL driver" );
            return;
        }
        if ( !_configParams.has("schema") )
        {
            _configParams["schema"] = "public";
        }
        if ( !_configParams.has("options") )
        {
            _configParams["options"] = "";
        }
        if ( !_configParams.has("single_row") )
        {
            // _configParams["single_row"] = 0;
        }

        _cnn = winux::MakeSimple( new PgsqlConnection(
            _configParams["host"],
            _configParams["user"],
            _configParams["pwd"],
            _configParams["dbname"],
            _configParams["charset"],
            _configParams["schema"],
            _configParams["options"],
            &_configParams
        ) );
    }
#endif
    else
    {
        throw DbError( DbError::dbeConfigParamsError, "The database driver named `" + driver + "` is not supported" );
        return;
    }
}

} // namespace eiendb
