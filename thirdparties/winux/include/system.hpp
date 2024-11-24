#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__
//
// system 提供一些系统平台相关的功能
//

namespace winux
{
/** \brief 系统相关错误 */
class SystemError : public Error
{
public:
    enum {
        sysNoError,    //!< 无错误
    };

    SystemError( int errType, AnsiString const & errStr ) throw() : Error( errType, errStr ) { }
};


#if defined(OS_WIN)
    typedef HANDLE HPipe;
    typedef HANDLE HProcess;
#else
    typedef int HPipe;
    typedef pid_t HProcess;
#endif

/** \brief 获取当前进程ID */
WINUX_FUNC_DECL(uint) GetPid();
/** \brief 获取当前线程ID */
WINUX_FUNC_DECL(uint) GetTid();

WINUX_FUNC_DECL(size_t) CommandLineToArgvA( AnsiString const & cmd, AnsiStringArray * argv );
WINUX_FUNC_DECL(size_t) CommandLineToArgvW( UnicodeString const & cmd, UnicodeStringArray * argv );
/** \brief 把命令行解析成Argv数组。不支持命令行& && | ||
 *
 *  \param cmd 命令行，不支持命令行& && | ||
 *  \param argv 输出解析到的参数
 *  \return 解析到的参数个数 */
#if defined(_UNICODE) || defined(UNICODE)
inline size_t CommandLineToArgv( UnicodeString const & cmd, UnicodeStringArray * argv ) { return CommandLineToArgvW( cmd, argv ); }
#else
inline size_t CommandLineToArgv( AnsiString const & cmd, AnsiStringArray * argv ) { return CommandLineToArgvA( cmd, argv ); }
#endif

/** \brief 新建子进程执行指定命令，并用管道重定向了标准设备
 *
 *  \param cmd 要运行的程序和命令，如果是内置命令则需要加上cmd.exe或bash等可执行文件名在前
 *  \param hStdinWritePipe STDIN的写端管道，如果传入变量接收这个句柄则表示重定向标准输入，如果不想重定向则传入nullptr
 *  \param hStdoutReadPipe STDOUT的读端管道，如果传入变量接收这个句柄则表示重定向标准输出，如果不想重定向则传入nullptr
 *  \param hStderrReadPipe STDERR的读端管道，如果传入变量接收这个句柄则表示重定向标准错误，如果不想重定向则传入nullptr
 *  \param closeStdinIfStdinWritePipeIsNull 指示当不准备重定向标准输入时是否关闭它，这样子进程就不会等待输入而卡死
 *  \return HProcess 子进程的进程句柄 */
WINUX_FUNC_DECL(HProcess) ExecCommandEx(
    String const & cmd,
    HPipe * hStdinWritePipe,
    HPipe * hStdoutReadPipe,
    HPipe * hStderrReadPipe = NULL,
    bool closeStdinIfStdinWritePipeIsNull = true
);

/** \brief 新建子进程执行指定命令，等待子进程结束，并把字符串重定向了标准设备
 *
 *  \param cmd 要运行的程序和命令，如果是内置命令则需要加上cmd.exe或bash等可执行文件名在前
 *  \param stdinStr 要传入标准输入的内容，如果为空串则不重定向标准输入
 *  \param stdoutStr 要读取的标准输出的内容，如果为空则不重定向标准输出
 *  \param stderrStr 要读取的标准错误的内容，如果为空则不重定向标准错误
 *  \param closeStdinIfStdinStrEmpty 指示当不准备重定向标准输入时是否关闭它，这样子进程就不会等待输入而卡死
 *  \return int 子进程main()的返回值，Linux上此值仅为0~255 */
WINUX_FUNC_DECL(int) ExecCommand(
    String const & cmd,
    String const & stdinStr,
    String * stdoutStr,
    String * stderrStr = NULL,
    bool closeStdinIfStdinStrEmpty = true
);

/** \brief 执行命令，返回标准输出内容
 *
 *  \param cmd 要运行的程序和命令，如果是内置命令则需要加上cmd.exe或bash等可执行文件名在前
 *  \param stdinStr 要传入标准输入的内容，如果为空串则不重定向标准输入
 *  \param stderrStr 要读取的标准错误的内容，如果为空则不重定向标准错误
 *  \param closeStdinIfStdinStrEmpty 指示当不准备重定向标准输入时是否关闭它，这样子进程就不会等待输入而卡死
 *  \return String 标准输出的内容 */
WINUX_FUNC_DECL(String) GetExec(
    String const & cmd,
    String const & stdinStr = TEXT(""),
    String * stderrStr = NULL,
    bool closeStdinIfStdinStrEmpty = true
);


///////////////////////////////////////////////////////////////////////////////////////////

/** \brief 命令行变量解析器
 *
 * 该解析器定义了四种变量\n
 *  - 参数(Params)：由‘前缀字符+名字’和‘下一个命令行值’组成。例如：/file abc.cpp\n
 *  - 选项(Options)：由‘前缀字符+名字’‘= or :’‘选项值’组成。例如： --std=c99 /Fo:abc_dir\n
 *  - 旗标(Flags)：由‘前缀字符+名字’组成。例如：--exe --cpp /DLL\n
 *  - 值：剩余未能识别的都被当成值 */
class WINUX_DLL CommandLineVars
{
public:
    /** \brief 构造函数
     *
     *  \param [in] argc 命令行参数个数，可由main()参数传入
     *  \param [in] argv 命令行参数，可由main()参数传入
     *  \param [in] desiredParams 要识别的参数名（逗号分割的String类型、或者Array类型）
     *  \param [in] desiredOptions 要识别的选项名（逗号分割的String类型、或者Array类型）
     *  \param [in] desiredFlags 要识别的旗标名（逗号分割的String类型、或者Array类型）。如果要识别的旗标名是单个字符(不含前缀)，则命令行中可以一个参数指定多个旗标。以最先出现在要识别的单字符旗标名的前缀为参数前缀。
     *  \param [in] optionSymbols 选项赋值符号（逗号分割的String类型、或者Array类型）。默认是'='和':' */
    CommandLineVars(
        int argc,
        winux::tchar const ** argv,
        Mixed const & desiredParams,
        Mixed const & desiredOptions,
        Mixed const & desiredFlags,
        Mixed const & optionSymbols = TEXT("=,:")
    );

    /** \brief 获取参数个数 */
    size_t getParamsCount() const { return _params.getCount(); }
    /** \brief 获取选项个数 */
    size_t getOptionsCount() const { return _options.getCount(); }
    /** \brief 获取旗标个数 */
    size_t getFlagsCount() const { return _flags.getCount(); }
    /** \brief 获取值个数 */
    size_t getValuesCount() const { return _values.getCount(); }

    /** \brief 是否有此参数 */
    bool hasParam( String const & name ) const { return _params.has(name); }
    /** \brief 是否有此选项 */
    bool hasOption( String const & name ) const { return _options.has(name); }
    /** \brief 是否有此旗标 */
    bool hasFlag( String const & name ) const { return _flags.has(name); }
    /** \brief 是否有此值 */
    bool hasValue( String const & value ) const { return _values.has(value); }

    /** \brief 获取指定名字的参数 */
    Mixed const & getParam( String const & name, Mixed const & defValue = TEXT("") ) const { return this->hasParam(name) ? _params[name] : defValue; }
    /** \brief 获取指定名字的选项 */
    Mixed const & getOption( String const & name, Mixed const & defValue = TEXT("") ) const { return this->hasOption(name) ? _options[name] : defValue; }
    /** \brief 获取指定索引的旗标 */
    Mixed const & getFlag( size_t i ) const { return _flags[i]; }
    /** \brief 获取指定索引的值 */
    Mixed const & getValue( size_t i ) const { return _values[i]; }

    /** \brief 获取指定参数在argv中的索引 */
    size_t getParamIndexInArgv( String const & name ) const { return _paramIndexesInArgv.find(name) != _paramIndexesInArgv.end() ? _paramIndexesInArgv.at(name) : npos; }
    /** \brief 获取指定选项在argv中的索引 */
    size_t getOptionIndexInArgv( String const & name ) const { return _optionIndexesInArgv.find(name) != _optionIndexesInArgv.end() ? _optionIndexesInArgv.at(name) : npos; }
    /** \brief 获取指定旗标在argv中的索引 */
    size_t getFlagIndexInArgv( String const & name ) const { return _flagIndexesInArgv.find(name) != _flagIndexesInArgv.end() ? _flagIndexesInArgv.at(name) : npos; }
    /** \brief 获取指定值在argv中的索引 */
    size_t getValueIndexInArgv( String const & value ) const { return _valueIndexesInArgv.find(value) != _valueIndexesInArgv.end() ? _valueIndexesInArgv.at(value) : npos; }

    /** \brief 获取全部参数 */
    Mixed & getParams() { return _params; }
    /** \brief 获取全部选项 */
    Mixed & getOptions() { return _options; }
    /** \brief 获取全部旗标 */
    Mixed & getFlags() { return _flags; }
    /** \brief 获取全部值 */
    Mixed & getValues() { return _values; }

    /** \brief 倾泻全部 */
    Mixed dump() const
    {
        CommandLineVars * p = const_cast<CommandLineVars *>(this);
        return $c{
            { TEXT("params"), p->getParams() },
            { TEXT("options"), p->getOptions() },
            { TEXT("flags"), p->getFlags() },
            { TEXT("values"), p->getValues() },
        };
    }

    /** \brief 获取argc */
    int getArgc() const { return _argc; }
    /** \brief 获取argv */
    winux::tchar const ** getArgv() const { return _argv; }

private:
    int _argc; // main()命令行参数个数
    winux::tchar const ** _argv; // main()命令行参数

    StringArray _desiredParams; // 要识别的参数名
    StringArray _desiredOptions; // 要识别的选项名
    StringArray _desiredFlags; // 要识别的旗标名
    StringArray _optionSymbols; // 选项赋值符号

    Mixed _params;  // 参数Collection
    std::map< String, size_t > _paramIndexesInArgv;    // 参数在argv中的索引
    Mixed _options; // 选项Collection
    std::map< String, size_t > _optionIndexesInArgv;   // 选项在argv中的索引
    Mixed _flags;   // 旗标Array
    std::map< String, size_t > _flagIndexesInArgv;     // 旗标在argv中的索引
    Mixed _values;  // 值Array
    std::map< String, size_t > _valueIndexesInArgv;    // 值在argv中的索引

    DISABLE_OBJECT_COPY(CommandLineVars)
};

/** \brief 同步锁对象接口 */
class WINUX_DLL ILockObj
{
public:
    virtual ~ILockObj() { }
    virtual bool tryLock() = 0;
    virtual bool lock() = 0;
    virtual bool unlock() = 0;
};

/** \brief 作用域范围保护 */
class WINUX_DLL ScopeGuard
{
public:
    ScopeGuard( ILockObj & lockObj ) : _lockObj(lockObj)
    {
        _lockObj.lock();
    }
    ~ScopeGuard()
    {
        _lockObj.unlock();
    }

private:
    ILockObj & _lockObj;
    DISABLE_OBJECT_COPY(ScopeGuard)
};

/** \brief 原生互斥锁
 *
 *  Windows平台用win32api实现，Linux用pthread实现 */
class WINUX_DLL MutexNative : public ILockObj
{
public:
    MutexNative();
    virtual ~MutexNative();
    virtual bool tryLock() override;
    virtual bool lock() override;
    virtual bool unlock() override;

private:
    MembersWrapper<struct MutexLockObj_Data> _self;

    DISABLE_OBJECT_COPY(MutexNative)
};

/** \brief Dll加载器错误 */
class DllLoaderError : public SystemError
{
public:
    enum {
        dllFuncNotFound = 0x00000100,    //!< 函数未找到
        dllModuleNoLoaded                //!< 模块没加载
    };

    DllLoaderError( int errType, AnsiString const & errStr ) throw() : SystemError( errType, errStr ) { }
};

/** \brief DLL动态载入器 */
class WINUX_DLL DllLoader
{
public:

#if defined(OS_WIN)
    typedef HMODULE ModuleHandle;
#else
    typedef void * ModuleHandle;
#endif
    /** \brief 取得模块路径（含模块文件名）
     *
     *  \param funcInModule 模块中某个函数的地址 */
    static String GetModulePath( void * funcInModule );

    DllLoader();
    DllLoader( String const & dllName );

    ~DllLoader();

    operator bool() const { return _hDllModule != NULL; }
    operator ModuleHandle() const { return _hDllModule; }

    /** \brief 错误信息 */
    char const * errStr() const;

    /** \brief Dll函数动态调用 */
    template < typename _PfnType >
    class Function
    {
    public:
        typedef _PfnType PfnType;
    private:
        AnsiString _funcName;
        PfnType _pfn;
    public:
        Function() : _funcName(""), _pfn(0) { }
        Function( AnsiString const & funcName, PfnType pfn ) : _funcName(funcName), _pfn(pfn) { }

        operator bool() const { return _pfn != NULL; }

        /** \brief 函数名 */
        AnsiString const & getFuncName() const { return _funcName; }

        /** \brief 函数地址 */
        void * get() const { return reinterpret_cast<void *>(_pfn); }

        /** \brief 函数调用 */
        template < typename... _ArgType >
        typename FuncTraits<PfnType>::ReturnType call( _ArgType&& ... arg )
        {
            if ( !_pfn ) throw DllLoaderError( DllLoaderError::dllFuncNotFound, _funcName + " is not found" );
            return (*_pfn)( std::forward<_ArgType>(arg)... );
        }
    };

    /** \brief 获取指定名字的函数地址 */
    void (* funcAddr( AnsiString const & funcName ) )();

    /** \brief 获取指定名字的Function对象，失败抛异常 */
    template < typename _PfnType >
    Function<_PfnType> func( AnsiString const & funcName )
    {
        _PfnType pfn = (_PfnType)this->funcAddr(funcName);
        return Function<_PfnType>( funcName, pfn );
    }

    String dllModuleFile; //!< DLL模块文件

private:
    ModuleHandle _hDllModule;
    AnsiString _errStr;

    DISABLE_OBJECT_COPY(DllLoader)
};

/** \brief 文件映射旗标 */
enum FileMappingFlag
{
    fmfUnspec = 0,      //!< 未指定
    fmfReadOnly = 1,    //!< 只读
    fmfWriteCopy = 2,   //!< 写时拷贝
    fmfReadWrite = 3,   //!< 读写
    fmfExecuteReadOnly = 5, //!< 只读执行
    fmfExecuteWriteCopy = 6, //!< 写时拷贝执行
    fmfExecuteReadWrite = 7, //!< 读写执行
};

/** \brief 文件映射。可以用来读写大文件 */
class FileMapping
{
public:
    /** \brief 构造函数0 */
    FileMapping();

    /** \brief 构造函数1 加载一个文件进行映射
     *
     *  \param filePath 文件路径
     *  \param flag 旗标 */
    FileMapping( String const & filePath, FileMappingFlag flag = fmfWriteCopy );

    /** \brief 构造函数2 加载一个文件进行映射
     *
     *  \param file 文件
     *  \param isPeekFile 是否窥探文件。即外部管理文件资源，自身不持有管理权
     *  \param flag 旗标 */
    FileMapping(
    #if defined(OS_WIN)
        HANDLE file,
    #else
        int file,
    #endif
        bool isPeekFile,
        FileMappingFlag flag
    );

    /** \brief 析构函数 */
    virtual ~FileMapping();

    /** \brief 加载文件并创建映射
     *
     *  \param filePath 文件路径
     *  \param flag 旗标
     *  \return bool */
    bool create( String const & filePath, FileMappingFlag flag );

    /** \brief 加载文件并创建映射
     *
     *  \param file 文件
     *  \param isPeekFile 是否窥探文件。即外部管理文件资源，自身不持有管理权
     *  \param flag 旗标
     *  \return bool */
    bool create(
    #if defined(OS_WIN)
        HANDLE file,
    #else
        int file,
    #endif
        bool isPeekFile,
        FileMappingFlag flag
    );

    /** \brief 销毁映射卸载文件 */
    void destroy();

    /** \brief 加载文件 */
    bool loadFile( String const & filePath, FileMappingFlag flag );

    /** \brief 加载文件 */
    bool loadFile(
    #if defined(OS_WIN)
        HANDLE file,
    #else
        int file,
    #endif
        bool isPeekFile,
        FileMappingFlag flag
    );

    /** \brief 获取文件大小 */
    size_t getFileSize() const;

    /** \brief 卸载文件 */
    void unloadFile();

    /** \brief 映射
     *
     *  \param flag 旗标
     *  \param size 映射大小。0表示以文件大小为准
     *  \param offset 偏移位置
     *  \return bool */
    bool map( FileMappingFlag flag = fmfUnspec, size_t size = 0, offset_t offset = 0 );

    /** \brief 收回映射 */
    void unmap();

    /** \brief 暴露指针 */
    void * get() const { return _p; }

    /** \brief 暴露指针 */
    template < typename _Ty >
    _Ty * get() const { return reinterpret_cast<_Ty *>(_p); }

    /** \brief 获取数据大小 */
    size_t size() const { return _size; }

    operator bool() const;

private:
    // 零初始化
    void _zeroInit();

#if defined(OS_WIN)
    HANDLE _file;
    HANDLE _fileMapping;
#else
    int _file;
#endif
    void * _p; // 内存地址
    size_t _size; // 映射的大小
    FileMappingFlag _flag; // 记下创建标记
    bool _isPeekFile; // 是否为使用外部文件资源，即不管理文件资源

    DISABLE_OBJECT_COPY(FileMapping)
};

/** \brief 共享内存，可以跨进程访问。常用于进程间通讯
 *
 *  Windows基于FileMapping\n
 *  Linux基于shm_open() and mmap() */
class WINUX_DLL SharedMemory
{
public:
    /** \brief 构造函数0 */
    SharedMemory();

    /** \brief 构造函数1
     *
     *  \param shmName 共享内存的名字
     *  \param size 共享内存的大小 */
    SharedMemory( String const & shmName, size_t size );

    virtual ~SharedMemory();

    /** \brief 创建共享内存
     *
     *  \param shmName 共享内存的名字
     *  \param size 共享内存的大小
     *  \return bool */
    bool create( String const & shmName, size_t size );

    /** \brief 销毁共享内存 */
    void destroy();

    /** \brief 锁定内存获取内存地址 */
    void * lock();

    /** \brief 解锁回收内存地址 */
    void unlock();

    /** \brief 获取数据指针（自动lock()） */
    void * get() { return _data ? _data : this->lock(); }


private:
    String _shmName;
#if defined(OS_WIN)
    HANDLE _shm;
#else
    int _shm;
#endif
    void * _data;
    size_t _size;

    DISABLE_OBJECT_COPY(SharedMemory)
};

/** \brief 共享内存（POD类型数据）类模板 */
template < typename _PodType >
class SharedMemoryT : public SharedMemory
{
public:
    /** \brief 构造函数0 */
    SharedMemoryT() { }

    /** \brief 构造函数1
     *
     *  \param shmName 共享内存的名字
     *  \param size 共享内存的大小，如果是-1则取_PodType的大小 */
    SharedMemoryT( String const & shmName, size_t size = -1 )
    {
        this->create( shmName, size );
    }

    /** \brief 创建共享内存
     *
     *  \param shmName 共享内存的名字
     *  \param size 共享内存的大小，如果是-1则取_PodType的大小
     *  \return bool */
    bool create( String const & shmName, size_t size = -1 )
    {
        return SharedMemory::create( shmName, size == -1 ? sizeof(_PodType) : size );
    }

    _PodType * get() { return reinterpret_cast<_PodType *>( this->SharedMemory::get() ); }

    _PodType * operator -> () { return this->get(); }
};

} // namespace winux

#endif // __SYSTEM_HPP__
