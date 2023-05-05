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

/** \brief 判断一个字符串是否可展开，即是否包含%varname% */
WINPLUS_FUNC_DECL(bool) IsExpandString( String const & str );
/** \brief 展开字符串中存在的'%变量%' */
WINPLUS_FUNC_DECL(String) ExpandVars( String const & str );

/** \brief 通过HWND获得程序路径和进程ID以及线程ID */
WINPLUS_FUNC_DECL(String) GetAppPathByHwnd( HWND hwnd, DWORD * pProcessId = NULL, DWORD * pThreadId = NULL );

/** \brief 通过进程ID获得主窗口HWND */
WINPLUS_FUNC_DECL(HWND) GetMainWindowByProcessId( DWORD dwProcessId );

/** \brief 通过线程ID获得主窗口HWND */
WINPLUS_FUNC_DECL(HWND) GetMainWindowByThreadId( DWORD dwThreadId );

/** \brief 返回模块路径(末尾不含目录分隔符)，输出模块文件名 */
WINPLUS_FUNC_DECL(String) ModulePath( HMODULE mod = NULL, String * fileName = NULL );

/** \brief 获得程序的命令行参数 */
WINPLUS_FUNC_DECL(UINT) CommandArgumentArray( StringArray * argArr );
/** \brief 构造一个const char*[]，以vector<const char*>返回
 *
 *  不要用返回的vector的元素个数作为命令行参数个数，因为它始终多一个nullptr元素
 *  \param argArr 由CommandArgumentArray()取得的命令行参数数组，并且请保持其有效，如果其被摧毁，那么本函数返回的参数列表也就无效了 */
WINPLUS_FUNC_DECL(std::vector<String::value_type const *>) CommandArgs( StringArray * argArr );

/** \brief 使能关机 */
WINPLUS_FUNC_DECL(bool) ShutdownPrivilege( bool enable );

/** \brief 设置文件/目录时间，如果想设置当前时间请传递-1，如果不想设置某个时间请传递0 */
WINUX_FUNC_DECL(bool) SetFileTime( String const & filename, time_t ctime, time_t mtime, time_t atime );

/** \brief Windows错误号转成错误字符串 */
WINPLUS_FUNC_DECL(String) GetErrorStr( DWORD err );

/** \brief 获得描述NT版本的字符串 */
WINPLUS_FUNC_DECL(String) GetNtVersion( void );
/** \brief 获得描述系统版本的字符串 */
WINPLUS_FUNC_DECL(String) GetOsVersion( void );
/** \brief 获得自身模块版本号 */
WINPLUS_FUNC_DECL(String) GetSelfModuleVersion( void );
/** \brief 获得模块版本号 */
WINPLUS_FUNC_DECL(String) GetModuleVersion( String const & moduleFile );

///////////////////////////////////////////////////////////////////////////////////////////

/** \brief 注册表

    注册表数据类型与Mixed类型映射关系
    REG_NONE:       MT_NULL
    REG_SZ:         MT_ANSI or MT_UNICODE
    REG_EXPAND_SZ:  MT_COLLECTION { 2: "%VALUE%" }
    REG_BINARY:     MT_BINARY
    REG_DWORD:      MT_BOOLEAN, MT_BYTE, MT_SHORT, MT_USHORT, MT_INT, MT_UINT, MT_LONG, MT_ULONG
    REG_MULTI_SZ:   MT_ARRAY
    REG_QWORD:      MT_INT64 or MT_UINT64
 */
class WINPLUS_DLL Registry
{
public:
    /** \brief 根据Mixed类型判断RegValue数据类型
     *
     *  \param v Mixed const &
     *  \return DWORD */
    static DWORD ValueType( Mixed const & v );

    /** \brief 只取值（由于REG_EXPAND_SZ映射为MT_COLLECTION的原因，导致REG_EXPAND_SZ字符串取用不变，这个静态方法则处理这种情况）
     *
     *  \param v Mixed const & 由getValue()获取到的值
     *  \return winux::Mixed const & */
    static Mixed const & Value( Mixed const & v );

    /** \brief 判断键或值是否存在
     *
     *  \param key String const & 键
     *  \param lpValueName LPCTSTR 值名。如果是nullptr则只判断键是否存在，如果为空串判断键默认值是否存在，其他则判断指定值是否存在
     *  \return bool */
    static bool Exists( String const & key, LPCTSTR lpValueName = nullptr );

    /** \brief 删除键（如果有子键则失败）或删除值
     *
     *  \param key 键
     *  \param lpValueName 值名。如果是空串则删除键的默认值，如果是nullptr则删除键
     *  \return bool */
    static bool Delete( String const & key, LPCTSTR lpValueName = nullptr );

    /** \brief 强制删除键（递归删除所有子键，请慎用）
     *
     *  \param key String const &
     *  \return bool */
    static bool ForceDelete( String const & key );

public:
    /** \brief 构造函数1
     *
     *  \param hkey HKEY 键句柄，也可以是预定义根键
     *  \param subKey String const & 子键
     *  \param isCreateOnNotExists bool 当键名不存在时是否创建 */
    Registry( HKEY hkey, String const & subKey, bool isCreateOnNotExists = false );

    /** \brief 构造函数2
     *
     *  \param key String const & 键
     *  \param isCreateOnNotExists bool 当键名不存在时是否创建 */
    Registry( String const & key, bool isCreateOnNotExists = false );

    ~Registry();

    /** \brief 获取指定名称的值
     *
     *  \param name 值名。如果为空串则读取KEY的默认值
     *  \param defval 如果读取失败则使用这个值返回
     *  \return Mixed */
    Mixed getValue( String const & name, Mixed const & defval = Mixed() ) const;

    /** \brief 设置指定名称的值
     *
     *  \param name 值名。如果为空串则设置KEY的默认值
     *  \param v 值
     *  \param dwType 注册表值类型。如果为-1，则根据不同类型的值设置不同的注册表值类型；如果不为-1，则以此参数为准设置注册表值类型
     *  \return bool */
    bool setValue( String const & name, Mixed const & v, DWORD dwType = -1 );

    /** \brief 删除值（可删除键的默认值）
     *
     *  \param name 要删除的值名。如果是空串则删除键的默认值（数值未设置）
     *  \return bool */
    bool delValue( String const & name );

    /** \brief 枚举值
     *
     *  \param name String *
     *  \param pv Mixed *
     *  \return bool */
    bool enumValues( String * name, Mixed * pv ) const;

    /** \brief 枚举键 */
    bool enumKeys( String * subKey ) const;

    /** \brief 枚举重置 */
    void enumReset();

    /** \brief 判断值是否存在
     *
     *  \param name String const &
     *  \return bool */
    bool hasValue( String const & name ) const;

    /** \brief 是否打开注册表句柄 */
    operator bool() const { return _hkey != nullptr; }

    /** \brief 当前键 */
    HKEY key() const { return _hkey; }

    /** \brief 错误码 */
    LSTATUS errNo() const { return _err; }

    /** \brief 错误字符串 */
    String error() const { return GetErrorStr(_err); }

private:
    HKEY _hkey;
    mutable LSTATUS _err;
    mutable DWORD _indexEnumValues;
    mutable DWORD _indexEnumKeys;

    DISABLE_OBJECT_COPY(Registry)
};

///////////////////////////////////////////////////////////////////////////////////////////

/** \brief 共享内存(基于FileMapping)
 *
 *  共享内存属于Windows内核对象，故可以跨进程访问。常用于进程间通讯 */
template < typename _Ty > class SharedMemory
{
public:
    SharedMemory() : _data(NULL), _hShared(NULL) {}
    SharedMemory( String const & name, size_t size ) : _data(NULL), _hShared(NULL)
    {
        this->create( name, size );
    }
    SharedMemory( String const & name ) : _data(NULL), _hShared(NULL)
    {
        this->create(name);
    }

    ~SharedMemory()
    {
        this->free();
    }

    bool create( String const & name, size_t size )
    {
        this->free();
        union
        {
            struct
            {
                DWORD dwLow;
                DWORD dwHigh;
            };
            size_t size;
        } v;
        ZeroMemory( &v, sizeof(v) );
        _size = v.size = size;

        _hShared = CreateFileMapping( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, v.dwHigh, v.dwLow, name.c_str() );
        return _hShared != NULL;
    }

    bool create( String const & name )
    {
        return this->create( name, sizeof(_Ty) );
    }

    bool open( String const & name, size_t size = (size_t)-1 )
    {
        this->free();
        _size = size == (size_t)-1 ? sizeof(_Ty) : size;
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
            _size = 0;
        }
    }

    _Ty * lock()
    {
        this->unlock();
        return _data = reinterpret_cast<_Ty *>( MapViewOfFile( _hShared, FILE_MAP_ALL_ACCESS, 0, 0, 0 ) );
    }

    _Ty * operator -> ()
    {
        return _data ? _data : this->lock();
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
    size_t _size;
    HANDLE _hShared;

    SharedMemory( SharedMemory const & );
    SharedMemory & operator = ( SharedMemory const & );
};


} // namespace winplus

#endif // !defined(__WINPLUS_SYS_HPP__)
