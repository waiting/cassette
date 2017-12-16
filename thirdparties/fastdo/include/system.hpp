#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__

namespace winux
{

/** \brief 命令行变量解析器
 *
 * 该解析器定义了四种变量
 *  - 参数(Params)：由‘前缀字符+名字’和‘下一个命令行值’组成。例如：/file abc.cpp
 *  - 选项(Options)：由‘前缀字符+名字’‘= or :’‘选项值’组成。例如： --std=c99 /Fo:abc_dir
 *  - 旗标(Flags)：由‘前缀字符+名字’组成。例如：--exe --cpp /DLL
 *  - 值：剩余未能识别的都被当成值 */
class WINUX_DLL CommandLineVars
{
public:
    /** \param [in] argc 命令行参数个数，可由main()参数传入
     *  \param [in] argv 命令行参数，可由main()参数传入
     *  \param [in] desiredParams 要识别的参数名（逗号分割的String类型、或者Array类型）
     *  \param [in] desiredOptions 要识别的选项名（逗号分割的String类型、或者Array类型）
     *  \param [in] desiredFlags 要识别的旗标名（逗号分割的String类型、或者Array类型）
     *  \param [in] optionSymbols 选项赋值符号（逗号分割的String类型、或者Array类型）。默认是'='和':' */
    CommandLineVars(
        int argc,
        char * argv[],
        Mixed const & desiredParams,
        Mixed const & desiredOptions,
        Mixed const & desiredFlags,
        Mixed const & optionSymbols = "=,:"
    );
    int getParamsCount() const { return _params.getCount(); }
    int getOptionsCount() const { return _options.getCount(); }
    int getFlagsCount() const { return _flags.getCount(); }
    int getValuesCount() const { return _values.getCount(); }

    bool hasParam( String const & name ) const { return _params.has(name); }
    bool hasOption( String const & name ) const { return _options.has(name); }
    bool hasFlag( String const & name ) const { return _flags.has(name); }
    bool hasValue( String const & value ) const { return _values.has(value); }

    Mixed const & getParam( String const & name, Mixed const & defValue = "" ) const { return this->hasParam(name) ? _params[name] : defValue; }
    Mixed const & getOption( String const & name, Mixed const & defValue = "" ) const { return this->hasOption(name) ? _options[name] : defValue; }
    Mixed const & getValue( int i ) const { return _values[i]; }
    Mixed const & getFlag( int i ) const { return _flags[i]; }

    Mixed & getParams() { return _params; }
    Mixed & getOptions() { return _options; }
    Mixed & getFlags() { return _flags; }
    Mixed & getValues() { return _values; }
private:
    static void __MixedAppendToStringArray( Mixed const & mx, StringArray * arr );

    StringArray _desiredParams;
    StringArray _desiredOptions;
    StringArray _desiredFlags;
    StringArray _optionSymbols;

    Mixed _params; ///< Collection
    Mixed _options; ///< Collection
    Mixed _flags; ///< Array
    Mixed _values; ///< Array
    DISABLE_OBJECT_COPY(CommandLineVars)
};

/** \brief 命令行参数解析(OldVersion)
 *
 *  规则是：
 *  含有前缀的串叫作name，后跟着不含前缀的串叫作value，一对name/value叫作Param
 *  若未出现name，先出现了value，则作为noNameParam
 *  若出现了name，接着又出现另一个name，则前一个name作为noValueParam
 *  name必须包含前缀
 *  noNameParam也被叫Value参数, noValueParam也被叫Flag参数, 这样具有逻辑性的命名方便使用 */
class WINUX_DLL CommandLine
{
    StringStringMap _params;
    StringArray _values;
    StringArray _flags;
    static bool _IsName( String const & arg, StringArray const & prefixes );
public:
    CommandLine( int argc, char * argv[], String const & paramPrefix = "- -- /" );
    int getParamCount() const { return _params.size(); }
    int getValueCount() const { return _values.size(); }
    int getFlagCount() const { return _flags.size(); }
    bool hasParam( String const & name ) const { return isset( _params, name ); }
    bool hasValue( String const & value ) const { return std::find( _values.begin(), _values.end(), value ) != _values.end(); }
    bool hasFlag( String const & name ) const { return std::find( _flags.begin(), _flags.end(), name ) != _flags.end(); }
    String getParam( String const & name, String const & defValue = "" ) const { return isset( _params, name ) ? _params.at(name) : defValue; }
    String getValue( int i ) const { return _values[i]; }
    String getFlag( int i ) const { return _flags[i]; }

    DISABLE_OBJECT_COPY(CommandLine)
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

/** \brief 作用域范围锁定 */
class WINUX_DLL ScopeLock
{
    ILockObj & _lockObj;
public:
    ScopeLock( ILockObj & lockObj ) : _lockObj(lockObj)
    {
        _lockObj.lock();
    }
    ~ScopeLock()
    {
        _lockObj.unlock();
    }
    DISABLE_OBJECT_COPY(ScopeLock)
};

/** \brief 互斥锁 */
class WINUX_DLL MutexLockObj : public ILockObj
{
public:
    MutexLockObj();
    virtual ~MutexLockObj();
    virtual bool tryLock();
    virtual bool lock();
    virtual bool unlock();
private:
    MembersWrapper<struct MutexLockObj_Data> _self;

    // disabled some methods
    DISABLE_OBJECT_COPY(MutexLockObj)
};

/** \brief 函数调用器 */
template < typename _RetType >
class Caller
{
private:
    _RetType _ret;
public:
    template < typename _Fn >
    Caller( _Fn fn )
    {
        if ( fn ) _ret = (*fn)();
    }

    template < typename _Fn, typename _Arg1 >
    Caller( _Fn fn, _Arg1 a1 )
    {
        if ( fn ) _ret = (*fn)( a1 );
    }

    template < typename _Fn, typename _Arg1, typename _Arg2 >
    Caller( _Fn fn, _Arg1 a1, _Arg2 a2 )
    {
        if ( fn ) _ret = (*fn)( a1, a2 );
    }

    template < typename _Fn, typename _Arg1, typename _Arg2, typename _Arg3 >
    Caller( _Fn fn, _Arg1 a1, _Arg2 a2, _Arg3 a3 )
    {
        if ( fn ) _ret = (*fn)( a1, a2, a3 );
    }

    template < typename _Fn, typename _Arg1, typename _Arg2, typename _Arg3, typename _Arg4 >
    Caller( _Fn fn, _Arg1 a1, _Arg2 a2, _Arg3 a3, _Arg4 a4 )
    {
        if ( fn ) _ret = (*fn)( a1, a2, a3, a4 );
    }

    template < typename _Fn, typename _Arg1, typename _Arg2, typename _Arg3, typename _Arg4, typename _Arg5 >
    Caller( _Fn fn, _Arg1 a1, _Arg2 a2, _Arg3 a3, _Arg4 a4, _Arg5 a5 )
    {
        if ( fn ) _ret = (*fn)( a1, a2, a3, a4, a5 );
    }

    template < typename _Fn, typename _Arg1, typename _Arg2, typename _Arg3, typename _Arg4, typename _Arg5, typename _Arg6 >
    Caller( _Fn fn, _Arg1 a1, _Arg2 a2, _Arg3 a3, _Arg4 a4, _Arg5 a5, _Arg6 a6 )
    {
        if ( fn ) _ret = (*fn)( a1, a2, a3, a4, a5, a6 );
    }

    template < typename _Fn, typename _Arg1, typename _Arg2, typename _Arg3, typename _Arg4, typename _Arg5, typename _Arg6, typename _Arg7 >
    Caller( _Fn fn, _Arg1 a1, _Arg2 a2, _Arg3 a3, _Arg4 a4, _Arg5 a5, _Arg6 a6, _Arg7 a7 )
    {
        if ( fn ) _ret = (*fn)( a1, a2, a3, a4, a5, a6, a7 );
    }

    template < typename _Fn, typename _Arg1, typename _Arg2, typename _Arg3, typename _Arg4, typename _Arg5, typename _Arg6, typename _Arg7, typename _Arg8 >
    Caller( _Fn fn, _Arg1 a1, _Arg2 a2, _Arg3 a3, _Arg4 a4, _Arg5 a5, _Arg6 a6, _Arg7 a7, _Arg8 a8 )
    {
        if ( fn ) _ret = (*fn)( a1, a2, a3, a4, a5, a6, a7, a8 );
    }

    template < typename _Fn, typename _Arg1, typename _Arg2, typename _Arg3, typename _Arg4, typename _Arg5, typename _Arg6, typename _Arg7, typename _Arg8, typename _Arg9 >
    Caller( _Fn fn, _Arg1 a1, _Arg2 a2, _Arg3 a3, _Arg4 a4, _Arg5 a5, _Arg6 a6, _Arg7 a7, _Arg8 a8, _Arg9 a9 )
    {
        if ( fn ) _ret = (*fn)( a1, a2, a3, a4, a5, a6, a7, a8, a9 );
    }

    operator _RetType() const { return _ret; }
};

template <>
class Caller<void>
{
public:
    template < typename _Fn >
    Caller( _Fn fn )
    {
        if ( fn ) (*fn)();
    }

    template < typename _Fn, typename _Arg1 >
    Caller( _Fn fn, _Arg1 a1 )
    {
        if ( fn ) (*fn)( a1 );
    }

    template < typename _Fn, typename _Arg1, typename _Arg2 >
    Caller( _Fn fn, _Arg1 a1, _Arg2 a2 )
    {
        if ( fn ) (*fn)( a1, a2 );
    }

    template < typename _Fn, typename _Arg1, typename _Arg2, typename _Arg3 >
    Caller( _Fn fn, _Arg1 a1, _Arg2 a2, _Arg3 a3 )
    {
        if ( fn ) (*fn)( a1, a2, a3 );
    }

    template < typename _Fn, typename _Arg1, typename _Arg2, typename _Arg3, typename _Arg4 >
    Caller( _Fn fn, _Arg1 a1, _Arg2 a2, _Arg3 a3, _Arg4 a4 )
    {
        if ( fn ) (*fn)( a1, a2, a3, a4 );
    }

    template < typename _Fn, typename _Arg1, typename _Arg2, typename _Arg3, typename _Arg4, typename _Arg5 >
    Caller( _Fn fn, _Arg1 a1, _Arg2 a2, _Arg3 a3, _Arg4 a4, _Arg5 a5 )
    {
        if ( fn ) (*fn)( a1, a2, a3, a4, a5 );
    }

    template < typename _Fn, typename _Arg1, typename _Arg2, typename _Arg3, typename _Arg4, typename _Arg5, typename _Arg6 >
    Caller( _Fn fn, _Arg1 a1, _Arg2 a2, _Arg3 a3, _Arg4 a4, _Arg5 a5, _Arg6 a6 )
    {
        if ( fn ) (*fn)( a1, a2, a3, a4, a5, a6 );
    }

    template < typename _Fn, typename _Arg1, typename _Arg2, typename _Arg3, typename _Arg4, typename _Arg5, typename _Arg6, typename _Arg7 >
    Caller( _Fn fn, _Arg1 a1, _Arg2 a2, _Arg3 a3, _Arg4 a4, _Arg5 a5, _Arg6 a6, _Arg7 a7 )
    {
        if ( fn ) (*fn)( a1, a2, a3, a4, a5, a6, a7 );
    }

    template < typename _Fn, typename _Arg1, typename _Arg2, typename _Arg3, typename _Arg4, typename _Arg5, typename _Arg6, typename _Arg7, typename _Arg8 >
    Caller( _Fn fn, _Arg1 a1, _Arg2 a2, _Arg3 a3, _Arg4 a4, _Arg5 a5, _Arg6 a6, _Arg7 a7, _Arg8 a8 )
    {
        if ( fn ) (*fn)( a1, a2, a3, a4, a5, a6, a7, a8 );
    }

    template < typename _Fn, typename _Arg1, typename _Arg2, typename _Arg3, typename _Arg4, typename _Arg5, typename _Arg6, typename _Arg7, typename _Arg8, typename _Arg9 >
    Caller( _Fn fn, _Arg1 a1, _Arg2 a2, _Arg3 a3, _Arg4 a4, _Arg5 a5, _Arg6 a6, _Arg7 a7, _Arg8 a8, _Arg9 a9 )
    {
        if ( fn ) (*fn)( a1, a2, a3, a4, a5, a6, a7, a8, a9 );
    }
};

/** \brief Dll加载器错误 */
class DllLoaderError : public Error
{
public:
    enum {
        DllLoader_FuncNotFound = 0x00000100,
        DllLoader_ModuleNoLoaded
    };

    DllLoaderError( int errType, AnsiString const & errStr ) throw() : Error( errType, errStr ) { }
};

/** \brief DLL动态载入器 */
class WINUX_DLL DllLoader
{
public:

#if defined(_MSC_VER) || defined(WIN32)
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

    // 错误信息
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
        /*Function( Function const & other ) : _pfn(other._pfn) { }
        Function & operator = ( Function const & other )
        {
            if ( this != &other )
            {
                _pfn = other._pfn;
            }
            return *this;
        }//*/

        operator bool() const { return _pfn != NULL; }

        operator PfnType() const { return _pfn; }

        AnsiString const & getFuncName() const { return _funcName; }
        void * get() const { return reinterpret_cast<void *>(_pfn); }

        #include "dllfunc_calltpls.inl"
    };

    /** \brief 获取指定名字的函数地址 */
    int (* funcAddr( AnsiString const & funcName ) )();

    /** \brief 获取指定名字的Function对象，失败抛异常 */
    template < typename _PfnType >
    Function<_PfnType> func( AnsiString const & funcName )
    {
        _PfnType pfn = (_PfnType)this->funcAddr(funcName);
        return Function<_PfnType>( funcName, pfn );
    }

    winux::String dllModuleFile; ///< Dll模块文件
private:
    ModuleHandle _hDllModule;

    DISABLE_OBJECT_COPY(DllLoader)
};


} // namespace winux

#endif // __SYSTEM_HPP__
