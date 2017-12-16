//////////////////////////////////////////////////////////////////////////
// purpose: 系统环境相关
// author:  WT
//////////////////////////////////////////////////////////////////////////

#if !defined(__WINPLUS_SYS_HPP__)
#define __WINPLUS_SYS_HPP__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace winplus
{

//////////////////////////////////////////////////
// 一些常量
//////////////////////////////////////////////////

/* 目录分割符 */
extern WINPLUS_DLL String const dirSep;// = TEXT("\\");
extern WINPLUS_DLL String const dirSepUnix;// = TEXT("/");
extern WINPLUS_DLL String const dirSepMac;// = TEXT("/");

/* windows行分割符 */
extern WINPLUS_DLL String const lineSep;// = TEXT("\r\n");
/* unix行分割符 */
extern WINPLUS_DLL String const lineSepUnix;// = TEXT("\n");
/* mac行分割符 */
extern WINPLUS_DLL String const lineSepMac;// = TEXT("\r");

/* 判断一个字符串是否可展开，即是否包含%varname% */
WINPLUS_FUNC_DECL(bool) IsExpandString( String const & str );
/* 展开字符串中存在的'%变量%' */
WINPLUS_FUNC_DECL(String) ExpandVars( String const & str );

/* 通过HWND获得程序路径和进程ID */
WINPLUS_FUNC_DECL(String) GetAppPathFromHWND( HWND hWnd, DWORD * processId = NULL );
/* 返回当前工作目录(末尾不含目录分隔符) */
WINPLUS_FUNC_DECL(String) GetCurrentDir( void );
/* 返回模块路径(末尾不含目录分隔符)，输出模块文件名 */
WINPLUS_FUNC_DECL(String) ModulePath( HMODULE module = NULL, String * fileName = NULL );
/* 获取路径名(末尾不含目录分隔符) */
WINPLUS_FUNC_DECL(String) FilePath( String const & fullPath, String * fileName = NULL );
/* 获取一个文件名的标题和扩展名 */
WINPLUS_FUNC_DECL(String) FileTitle( String const & fileName, String * extName = NULL );
/* 判断是否是一个目录 */
WINPLUS_FUNC_DECL(bool) IsDir( String const & path );
/* 获取文件大小 */
WINPLUS_FUNC_DECL(uint64) FileSize( String const & fullpath );

/* 路径分隔符整理 */
WINPLUS_FUNC_DECL(String) PathWithSep( String const & path );
WINPLUS_FUNC_DECL(String) PathNoSep( String const & path );

/* 获得程序的命令行参数 */
WINPLUS_FUNC_DECL(INT) CommandArguments( StringArray * arr );
/* 使能关机 */
WINPLUS_FUNC_DECL(bool) ShutdownPrivilege( bool enable );

/* 获得描述NT版本的字符串 */
WINPLUS_FUNC_DECL(String) GetNtVersion( void );
/* 获得描述系统版本的字符串 */
WINPLUS_FUNC_DECL(String) GetOsVersion( void );
/* 获得自身模块版本号 */
WINPLUS_FUNC_DECL(String) GetSelfModuleVersion( void );
/* 获得模块版本号 */
WINPLUS_FUNC_DECL(String) GetModuleVersion( String const & moduleFile );


// 文件遍历搜索

////////////////////////////////////////////////
// 获取文件夹中的文件和子文件夹
// sort_type:
// 0 - 不排序, 1 - 排序, 2 - 逆序
////////////////////////////////////////////////
WINPLUS_FUNC_DECL(void) FolderData( String const & path, StringArray * files, StringArray * sub_folders, int sort_type = 0 );
////////////////////////////////////////////////
// 在指定路径下枚举指定扩展名的文件
// files: 将收到文件的完整路径数组
// is_recursion: 是否递归搜索
////////////////////////////////////////////////
WINPLUS_FUNC_DECL(int) EnumFiles( String const & path, StringArray const & extnames, StringArray * files, bool is_recursion = true );

//////////////////////////////////////////////////////////////////////////

/* 共享内存(基于FileMapping)
   共享内存属于Windows内核对象,故可以跨进程访问
   常用于进程间通讯 */
template < typename _Ty > class SharedMemory
{
public:
    SharedMemory() : _data(NULL), _hShared(NULL) {}
    SharedMemory( String const & name, int size )
    : _data(NULL), _hShared(NULL)
    {
        this->create( name, size );
    }
    ~SharedMemory()
    {
        this->free();
    }


    bool create( String const & name, int size )
    {
        this->free();
        _hShared = CreateFileMapping( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, name.c_str() );
        return _hShared != NULL;
    }
    bool open( String const & name )
    {
        this->free();
        _hShared = OpenFileMapping( FILE_MAP_ALL_ACCESS, FALSE, name.c_str() );
        return _hShared != NULL;
    }
    void free()
    {
        this->unlock();
        if ( _hShared != NULL )
        {
            CloseHandle(_hShared);
            _hShared = NULL;
        }
    }

    _Ty * lock()
    {
        this->unlock();
        return _data = reinterpret_cast<_Ty *>( MapViewOfFile( _hShared, FILE_MAP_ALL_ACCESS, 0, 0, 0 ) );
    }

    _Ty * operator -> ()
    {
        return _data ? _data : lock();
    }

    void unlock()
    {
        if ( _data != NULL )
        {
            UnmapViewOfFile(_data);
            _data = NULL;
        }
    }

private:
    _Ty * _data;
    HANDLE _hShared;

    SharedMemory( SharedMemory const & );
    SharedMemory & operator = ( SharedMemory const & );
};

/* 命令行类
   用于从命令行中提取指定参数 */
class WINPLUS_DLL CommandLine
{
    StringArray _args;
    String _paramPrefix;
    // 参数名比较
    bool compareEqv( String const & name, String const & arg ) const;
    // 是否是参数
    bool isParamName( String const & arg ) const;
public:
    CommandLine( String const & paramPrefix = TEXT("-/") );
    String getParam( String const & name, String const & defValue = TEXT("") ) const;
    int getParamInt( String const & name, int defValue = 0 ) const;
    bool include( String const & name, int * valueIndex = NULL ) const;
    bool includeValue( String const & value ) const;
};

/* DLL动态载入器 */
class DllLoader
{
public:
    DllLoader( LPCTSTR dllName ) : _hDll(NULL)
    {
        _hDll = LoadLibrary(dllName);
    }
    ~DllLoader()
    {
        if ( _hDll )
        {
            FreeLibrary(_hDll);
            _hDll = NULL;
        }
    }
    operator bool() const { return _hDll != NULL; }
    operator HMODULE() const { return _hDll; }

private:
    HMODULE _hDll;

    DllLoader( DllLoader const & );
    DllLoader & operator = ( DllLoader const & );
};

/* Dll函数动态调用 */
template < typename _PfnType >
class DllFunction
{
    _PfnType _pfn;
public:
    DllFunction( HMODULE module, LPCSTR procName )
    {
        _pfn = (_PfnType)GetProcAddress( module, procName );
    }

    operator bool() const { return _pfn != NULL; }
    operator _PfnType() const { return _pfn; }
};

} // namespace winplus

#endif // !defined(__WINPLUS_SYS_HPP__)
