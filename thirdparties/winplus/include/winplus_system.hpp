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

/* 返回模块路径(末尾不含目录分隔符)，输出模块文件名 */
WINPLUS_FUNC_DECL(String) ModulePath( HMODULE mod = NULL, String * fileName = NULL );

/* 获得程序的命令行参数 */
WINPLUS_FUNC_DECL(INT) CommandArguments( StringArray * arr );
/* 使能关机 */
WINPLUS_FUNC_DECL(bool) ShutdownPrivilege( bool enable );

/** \brief Windows错误号转成错误字符串 */
WINPLUS_FUNC_DECL(String) GetErrorStr( DWORD err );

/* 获得描述NT版本的字符串 */
WINPLUS_FUNC_DECL(String) GetNtVersion( void );
/* 获得描述系统版本的字符串 */
WINPLUS_FUNC_DECL(String) GetOsVersion( void );
/* 获得自身模块版本号 */
WINPLUS_FUNC_DECL(String) GetSelfModuleVersion( void );
/* 获得模块版本号 */
WINPLUS_FUNC_DECL(String) GetModuleVersion( String const & moduleFile );


//////////////////////////////////////////////////////////////////////////

/* 共享内存(基于FileMapping)
   共享内存属于Windows内核对象，故可以跨进程访问
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


} // namespace winplus

#endif // !defined(__WINPLUS_SYS_HPP__)
