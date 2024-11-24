#include "utilities.hpp"
#include "system.hpp"
#include "strings.hpp"
#include "smartptr.hpp"
#include "filesys.hpp"

#if !defined(OS_WIN) || defined(HAVE_PTHREAD)
#include <pthread.h>
#endif

#if defined(OS_WIN)
#include <process.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <string.h>
#include <errno.h>
#endif

namespace winux
{
#include "is_x_funcs.inl"

WINUX_FUNC_IMPL(uint) GetPid()
{
#if defined(OS_WIN)
    return (uint)GetCurrentProcessId();
#else
    return (uint)getpid();
#endif
}

WINUX_FUNC_IMPL(uint) GetTid()
{
#if defined(OS_WIN)
    return (uint)GetCurrentThreadId();
#else
    return (uint)syscall(SYS_gettid);
#endif
}

template < typename _ChTy >
inline static void Impl_ParseCommandLineString( XString<_ChTy> const & cmd, size_t * pI, XString<_ChTy> * str )
{
    size_t & i = *pI;
    _ChTy quote = cmd[i];
    i++; // skip left "

    winux::uint slashes = 0;
    while ( i < cmd.length() )
    {
        _ChTy ch = cmd[i];

        if ( ch == quote )
        {
            if ( slashes % 2 ) // 奇数个'\\'
            {
                if ( slashes > 1 )
                {
                    *str += XString<_ChTy>( slashes / 2, Literal<_ChTy>::slashChar );
                }
                slashes = 0;
                *str += ch;
                i++;
            }
            else // 偶数个
            {
                if ( slashes > 0 )
                {
                    *str += XString<_ChTy>( slashes / 2, Literal<_ChTy>::slashChar );
                }
                slashes = 0;
                i++; // skip right "
                break;
            }
        }
        else if ( ch == Literal<_ChTy>::slashChar )
        {
            slashes++;
            i++;
        }
        else
        {
            if ( slashes )
            {
                *str += XString<_ChTy>( slashes, Literal<_ChTy>::slashChar );
                slashes = 0;
            }
            *str += ch;
            i++;
        }
    }
}

template < typename _ChTy >
inline size_t Impl_CommandLineToArgv( XString<_ChTy> const & cmd, XStringArray<_ChTy> * argv )
{
    size_t i = 0;
    winux::uint slashes = 0;
    XString<_ChTy> arg;
    while ( i < cmd.length() )
    {
        _ChTy ch = cmd[i];
        if ( IsSpace(ch) )
        {
            i++;
            while ( i < cmd.length() && IsSpace(cmd[i]) ) i++; // skip more spaces

            if ( slashes )
            {
                arg += XString<_ChTy>( slashes, Literal<_ChTy>::slashChar );
                slashes = 0;
            }
            if ( !arg.empty() ) argv->push_back(arg);
            arg.clear();
        }
        else if ( ch == Literal<_ChTy>::caretChar )
        {
            i++;
            if ( i < cmd.length() && (
                    cmd[i] == Literal<_ChTy>::caretChar ||
                    cmd[i] == Literal<_ChTy>::pipeChar ||
                    cmd[i] == Literal<_ChTy>::ampChar ||
                    cmd[i] == Literal<_ChTy>::ltChar ||
                    cmd[i] == Literal<_ChTy>::gtChar
                )
            )
            {
                arg += cmd[i];
                i++;
            }
        }
        else if (
            ch == Literal<_ChTy>::quoteChar
        #if !defined(OS_WIN)
            || ch == Literal<_ChTy>::aposChar
        #endif
        )
        {
            if ( slashes % 2 ) // 奇数个'\\'
            {
                if ( slashes > 1 )
                {
                    arg += XString<_ChTy>( slashes / 2, Literal<_ChTy>::slashChar );
                }
                slashes = 0;
                arg += ch;
                i++;
            }
            else // 偶数个
            {
                if ( slashes > 0 )
                {
                    arg += XString<_ChTy>( slashes / 2, Literal<_ChTy>::slashChar );
                }
                slashes = 0;
                // 执行字符串解析
                Impl_ParseCommandLineString( cmd, &i, &arg );
            }
        }
        else if ( ch == Literal<_ChTy>::slashChar )
        {
            slashes++;
            i++;
        }
        else
        {
            if ( slashes )
            {
                arg += XString<_ChTy>( slashes, Literal<_ChTy>::slashChar );
                slashes = 0;
            }
            arg += ch;
            i++;
        }
    }

    if ( i == cmd.length() )
    {
        if ( slashes )
        {
            arg += XString<_ChTy>( slashes, Literal<_ChTy>::slashChar );
            slashes = 0;
        }
        if ( !arg.empty() ) argv->push_back(arg);
    }

    return argv->size();
}

WINUX_FUNC_IMPL(size_t) CommandLineToArgvA( AnsiString const & cmd, AnsiStringArray * argv )
{
    return Impl_CommandLineToArgv( cmd, argv );
}

WINUX_FUNC_IMPL(size_t) CommandLineToArgvW( UnicodeString const & cmd, UnicodeStringArray * argv )
{
    return Impl_CommandLineToArgv( cmd, argv );
}

#if defined(OS_WIN)

WINUX_FUNC_IMPL(HProcess) ExecCommandEx(
    String const & cmd,
    HPipe * hStdinWritePipe,
    HPipe * hStdoutReadPipe,
    HPipe * hStderrReadPipe,
    bool closeStdinIfStdinWritePipeIsNull
)
{
    HPipe hStdinPipe[2];
    HPipe hStdoutPipe[2];
    HPipe hStderrPipe[2];
    bool hasStdinPipe = false;
    bool hasStdoutPipe = false;
    bool hasStderrPipe = false;

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    String cmdBuf = cmd;
    BOOL bCreateRet;
    DWORD status = 0;

    if ( hStdinWritePipe != NULL )
    {
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = NULL;
        // stdin的管道不能两端都给子进程继承，只能给读端给子进程，写端不能给，否则子进程cin无法结束。
        sa.bInheritHandle = FALSE;

        HANDLE hStdinPipe0;
        if ( !CreatePipe( &hStdinPipe0, hStdinPipe + 1, &sa, 0 ) ) goto StdPipeError;
        // 把stdin读端的管道让子进程继承
        DuplicateHandle( GetCurrentProcess(), hStdinPipe0, GetCurrentProcess(), hStdinPipe + 0, 0, TRUE, DUPLICATE_SAME_ACCESS );
        // 关闭不可继承的stdin读端管道句柄
        CloseHandle(hStdinPipe0);

        hasStdinPipe = true;
    }
    if ( hStdoutReadPipe != NULL )
    {
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;

        if ( !CreatePipe( hStdoutPipe + 0, hStdoutPipe + 1, &sa, 0 ) ) goto StdPipeError;
        hasStdoutPipe = true;
    }
    if ( hStderrReadPipe != NULL )
    {
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;

        if ( !CreatePipe( hStderrPipe + 0, hStderrPipe + 1, &sa, 0 ) ) goto StdPipeError;
        hasStderrPipe = true;
    }

    si.dwFlags = STARTF_USESTDHANDLES;

    if ( hStdinWritePipe != NULL )
    {
        si.hStdInput = hStdinPipe[0];
        *hStdinWritePipe = hStdinPipe[1];
    }
    else
    {
        if ( !closeStdinIfStdinWritePipeIsNull )
        {
            si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
        }
    }

    if ( hStdoutReadPipe != NULL )
    {
        si.hStdOutput = hStdoutPipe[1];
        *hStdoutReadPipe = hStdoutPipe[0];
    }
    else
    {
        si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    if ( hStderrReadPipe != NULL )
    {
        si.hStdError = hStderrPipe[1];
        *hStderrReadPipe = hStderrPipe[0];
    }
    else
    {
        if ( hStdoutReadPipe != NULL )
        {
            si.hStdError = si.hStdOutput;
        }
        else
        {
            si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
        }
    }

    bCreateRet = CreateProcess( NULL, ( cmdBuf.empty() ? (LPTSTR)TEXT(""): &cmdBuf[0] ), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi );
    if ( !bCreateRet )
    {
        goto StdPipeError;
    }
    CloseHandle(pi.hThread);

    // 关闭子进程已经继承的句柄
    if ( hStdinWritePipe != NULL )
    {
        CloseHandle(hStdinPipe[0]);
    }
    if ( hStdoutReadPipe != NULL )
    {
        CloseHandle(hStdoutPipe[1]);
    }
    if ( hStderrReadPipe != NULL )
    {
        CloseHandle(hStderrPipe[1]);
    }

    return pi.hProcess;

StdPipeError:
    if ( hasStdinPipe )
    {
        CloseHandle( hStdinPipe[0] );
        CloseHandle( hStdinPipe[1] );
    }
    if ( hasStdoutPipe )
    {
        CloseHandle( hStdoutPipe[0] );
        CloseHandle( hStdoutPipe[1] );
    }
    if ( hasStderrPipe )
    {
        CloseHandle( hStderrPipe[0] );
        CloseHandle( hStderrPipe[1] );
    }

    return INVALID_HANDLE_VALUE;
}

WINUX_FUNC_IMPL(int) ExecCommand(
    String const & cmd,
    String const & stdinStr,
    String * stdoutStr,
    String * stderrStr,
    bool closeStdinIfStdinStrEmpty
)
{
    HPipe hStdinWrite = INVALID_HANDLE_VALUE, hStdoutRead = INVALID_HANDLE_VALUE, hStderrRead = INVALID_HANDLE_VALUE;
    HProcess hChildProcess;
    DWORD status = 0;
    hChildProcess = ExecCommandEx(
        cmd,
        ( stdinStr.empty() ? NULL : &hStdinWrite ),
        ( stdoutStr ? &hStdoutRead : NULL ),
        ( stderrStr ? &hStderrRead : NULL ),
        closeStdinIfStdinStrEmpty
    );
    if ( hChildProcess == INVALID_HANDLE_VALUE ) return -1;

    if ( !stdinStr.empty() )
    {
        DWORD bytes;
        AnsiString stdinData = StringToLocal(stdinStr);
        WriteFile( hStdinWrite, stdinData.c_str(), (DWORD)stdinData.length(), &bytes, NULL );
        CloseHandle(hStdinWrite);
    }
    if ( stdoutStr != NULL )
    {
        AnsiString stdoutData;
        stdoutData.reserve(4096);
        char buf[4096];
        DWORD bytes = 0;
        while ( ReadFile( hStdoutRead, buf, 4096, &bytes, NULL ) && bytes )
        {
            stdoutData.append( buf, bytes );
        }
        CloseHandle(hStdoutRead);
        *stdoutStr = LocalToString(stdoutData);
    }
    if ( stderrStr != NULL )
    {
        AnsiString stderrData;
        stderrData.reserve(4096);
        char buf[4096];
        DWORD bytes = 0;
        while ( ReadFile( hStderrRead, buf, 4096, &bytes, NULL ) && bytes )
        {
            stderrData.append( buf, bytes );
        }
        CloseHandle(hStderrRead);
        *stderrStr = LocalToString(stderrData);
    }

    WaitForSingleObject( hChildProcess, INFINITE );
    GetExitCodeProcess( hChildProcess, &status );
    CloseHandle(hChildProcess);
    return status;
}

#else

WINUX_FUNC_IMPL(HProcess) ExecCommandEx(
    String const & cmd,
    HPipe * hStdinWritePipe,
    HPipe * hStdoutReadPipe,
    HPipe * hStderrReadPipe,
    bool closeStdinIfStdinWritePipeIsNull
)
{
    int stdinFds[2];
    int stdoutFds[2];
    int stderrFds[2];
    if ( hStdinWritePipe != NULL )
    {
        pipe(stdinFds);
    }
    if ( hStdoutReadPipe != NULL )
    {
        pipe(stdoutFds);
    }
    if ( hStderrReadPipe != NULL )
    {
        pipe(stderrFds);
    }

    // fork 子进程
    pid_t id = fork();
    if ( id < 0 ) // occur an error
    {
        return id;
    }
    else if ( id > 0 ) // This is the parent process. The `id` represents a child process id when `id` > 0.
    {
        if ( hStdinWritePipe != NULL )
        {
            close(stdinFds[0]); // 父进程不需要stdin读取端

            *hStdinWritePipe = stdinFds[1];
        }

        if ( hStdoutReadPipe != NULL )
        {
            close(stdoutFds[1]); // 父进程不需要stdout写入端

            *hStdoutReadPipe = stdoutFds[0];
        }

        if ( hStderrReadPipe != NULL )
        {
            close(stderrFds[1]); // 父进程不需要stderr写入端

            *hStderrReadPipe = stderrFds[0];
        }

        //int status = 0;
        //waitpid( id, &status, 0 );
        return id;
    }
    else // id == 0, 这里是子进程
    {
        if ( hStdinWritePipe != NULL )
        {
            close(0);
            dup2( stdinFds[0], 0 );
            close(stdinFds[0]);
            close(stdinFds[1]); // 子进程不需要stdin的写入端
        }
        else
        {
            if ( closeStdinIfStdinWritePipeIsNull )
                close(0);
        }

        if ( hStdoutReadPipe != NULL )
        {
            close(1);
            dup2( stdoutFds[1], 1 );
            close(stdoutFds[1]);
            close(stdoutFds[0]); // 子进程不需要stdout的读取端
        }
        if ( hStderrReadPipe != NULL )
        {
            close(2);
            dup2( stderrFds[1], 2 );
            close(stderrFds[1]);
            close(stderrFds[0]); // 子进程不需要stderr的读取端
        }
        else
        {
            close(2);
            dup2( 1, 2 );
        }
        StringArray args;
        std::vector<char *> argv;
        int argc = CommandLineToArgv( cmd, &args );
        for ( int i = 0; i < argc; ++i )
        {
            argv.push_back( strdup( args[i].c_str() ) );
        }
        argv.push_back(NULL);

        // 执行命令
        int rc = execvp( argv[0], &argv[0] );

        if ( rc < 0 && errno == ENOENT )
        {
            //cerr << "The `cmd` is not found!" << endl;
        }

        for ( auto it = argv.begin(); it != argv.end(); ++it )
        {
            if ( *it ) free(*it);
        }
        exit(1);
    }

    return -1;
}

WINUX_FUNC_IMPL(int) ExecCommand(
    String const & cmd,
    String const & stdinStr,
    String * stdoutStr,
    String * stderrStr,
    bool closeStdinIfStdinStrEmpty
)
{
    HPipe hStdinWrite, hStdoutRead, hStderrRead;
    HProcess hChildProcess;
    int status = 0;
    hChildProcess = ExecCommandEx(
        cmd,
        ( stdinStr.empty() ? NULL : &hStdinWrite ),
        ( stdoutStr ? &hStdoutRead : NULL ),
        ( stderrStr ? &hStderrRead : NULL ),
        closeStdinIfStdinStrEmpty
    );
    if ( hChildProcess == (HProcess)-1 ) return -1;

    if ( !stdinStr.empty() )
    {
        int bytes = write( hStdinWrite, stdinStr.c_str(), stdinStr.length() );
        (void)bytes;
        close(hStdinWrite);
    }
    if ( stdoutStr != NULL )
    {
        stdoutStr->reserve(4096);
        char buf[4096];
        int bytes;
        while ( ( bytes = read( hStdoutRead, buf, 4096 ) ) > 0 )
        {
            stdoutStr->append( buf, bytes );
        }
        close(hStdoutRead);
    }
    if ( stderrStr != NULL )
    {
        stderrStr->reserve(4096);
        char buf[4096];
        int bytes;
        while ( ( bytes = read( hStderrRead, buf, 4096 ) ) > 0 )
        {
            stderrStr->append( buf, bytes );
        }
        close(hStderrRead);
    }

    waitpid( hChildProcess, &status, 0 );

    return (char)( ( status & 0xff00 ) >> 8 );
}

#endif

WINUX_FUNC_IMPL(String) GetExec(
    String const & cmd,
    String const & stdinStr,
    String * stderrStr,
    bool closeStdinIfStdinStrEmpty
)
{
    int rc;
    String stdoutStr;
    rc = ExecCommand( cmd, stdinStr, &stdoutStr, stderrStr, closeStdinIfStdinStrEmpty );
    return stdoutStr;
}

// --------------------------------------------------------------------------------------------

inline static void __MixedAppendToStringArray( Mixed const & mx, StringArray * arr )
{
    if ( mx.isArray() )
    {
        size_t n = mx.getCount();
        for ( size_t i = 0; i < n; i++ )
        {
            arr->push_back(mx[i]);
        }
    }
    else if ( mx.isString() )
    {
        String s = mx;
        if ( !s.empty() )
        {
            StringArray tmpArr;
            size_t n = StrSplit( s, TEXT(","), &tmpArr );
            for ( size_t i = 0; i < n; i++ )
            {
                if ( !tmpArr[i].empty() )
                {
                    arr->push_back(tmpArr[i]);
                }
            }
        }
    }
    else if ( !mx.isNull() )
    {
        arr->push_back(mx);
    }
}

// 获取前缀和名称
inline static size_t __ObtainPrefixAndName( String const & str, String * prefix, String * name )
{
    size_t len = 0, i = 0;
    while ( i < str.length() )
    {
        if ( IsWordNoDollar(str[i]) || IsDigit(str[i]) )
        {
            break;
        }
        else
        {
            len++;
        }
        i++;
    }

    *prefix = str.substr( 0, len );
    *name = str.substr(len);

    return len;
}

// 通过要解析的旗标获取前缀和单参数多旗标串
inline static void __ObtainFlagPrefixAndFlagNameList( StringArray const & desiredFlags, String * flagPrefix, String * flagNameList )
{
    Mixed nameLists;
    nameLists.createCollection();
    for ( auto & desiredFlag : desiredFlags )
    {
        String prefix, name;
        __ObtainPrefixAndName( desiredFlag, &prefix, &name );

        if ( name.length() == 1 )
        {
            Mixed & nameList = nameLists[prefix];

            if ( nameList.isString() )
            {
                nameList.ref<String>() += name;
            }
            else
            {
                nameList = name;
            }
        }
    }

    for ( size_t i = 0; i < nameLists.getCount(); i++ )
    {
        auto & pr = nameLists.getPair(i);
        *flagPrefix = pr.first.to<String>();
        *flagNameList = pr.second.to<String>();
        break;
    }
}

// class CommandLineVars ------------------------------------------------------------------
CommandLineVars::CommandLineVars( int argc, winux::tchar const ** argv, Mixed const & desiredParams, Mixed const & desiredOptions, Mixed const & desiredFlags, Mixed const & optionSymbols /*= "=,:" */ ) :
    _argc(argc),
    _argv(argv)
{
    __MixedAppendToStringArray( desiredParams, &_desiredParams );
    __MixedAppendToStringArray( desiredOptions, &_desiredOptions );

    String flagPrefix;
    String flagNameList;
    __MixedAppendToStringArray( desiredFlags, &_desiredFlags );
    __ObtainFlagPrefixAndFlagNameList( _desiredFlags, &flagPrefix, &flagNameList );

    __MixedAppendToStringArray( optionSymbols, &_optionSymbols );

    _params.createCollection();
    _options.createCollection();
    _flags.createArray();
    _values.createArray();

    for ( int i = 1; i < argc; ++i )
    {
        String arg = argv[i];
        bool isJudged = false; // 是否已经判断属于何种变量

        if ( !isJudged )
        {
            // Params
            for ( size_t iDesiredParam = 0; !isJudged && iDesiredParam < _desiredParams.size(); ++iDesiredParam )
            {
                if ( arg == _desiredParams[iDesiredParam] )
                {
                    isJudged = true;

                    _paramIndexesInArgv[_desiredParams[iDesiredParam]] = i;
                    // 找到一个参数，下一个argv[i]就是参数值
                    i++;
                    if ( i < argc )
                    {
                        _params[_desiredParams[iDesiredParam]] = argv[i];
                    }
                    else // 已经是最后一个，参数值只好认为是空
                    {
                        _params[_desiredParams[iDesiredParam]] = TEXT("");
                    }
                }
            }
        }

        if ( !isJudged )
        {
            // Options
            for ( size_t iDesiredOption = 0; !isJudged && iDesiredOption < _desiredOptions.size(); ++iDesiredOption )
            {
                if ( arg.length() < _desiredOptions[iDesiredOption].length() )
                    continue;
                MultiMatch mmFind( _optionSymbols, NULL );
                MultiMatch::MatchResult mr = mmFind.commonSearch(arg);
                String optionName, optionVal;
                if ( mr.pos != -1 )
                {
                    optionName = arg.substr( 0, mr.pos );
                    optionVal = arg.substr( mr.pos + mmFind.getMatchItem(mr.item).length() );
                }
                else
                {
                    optionName = arg;
                    optionVal = TEXT("");
                }

                if ( optionName == _desiredOptions[iDesiredOption] )
                {
                    isJudged = true;

                    _optionIndexesInArgv[_desiredOptions[iDesiredOption]] = i;
                    // 找到一个option
                    _options[optionName] = optionVal;
                }
            }
        }

        if ( !isJudged )
        {
            // Flags
            for ( size_t iDesiredFlag = 0; !isJudged && iDesiredFlag < _desiredFlags.size(); ++iDesiredFlag )
            {
                if ( arg == _desiredFlags[iDesiredFlag] )
                {
                    isJudged = true;

                    _flagIndexesInArgv[_desiredFlags[iDesiredFlag]] = i;
                    // 找到一个flag
                    _flags.addUnique(_desiredFlags[iDesiredFlag]);
                }
            }
        }

        if ( !isJudged )
        {
            size_t noMatchCount = 0;
            // multi flags
            if ( !flagPrefix.empty() && !flagNameList.empty() )
            {
                String argPrefix, argText;
                __ObtainPrefixAndName( arg, &argPrefix, &argText );

                if ( argPrefix == flagPrefix ) // 参数前缀和旗标前缀一样
                {
                    for ( auto & ch : argText )
                    {
                        if ( flagNameList.find(ch) != npos )
                        {
                            isJudged = true;

                            String flag = flagPrefix + ch;
                            _flagIndexesInArgv[flag] = i;
                            // 找到一个flag
                            _flags.addUnique(flag);
                        }
                        else
                        {
                            noMatchCount++;
                        }
                    }
                }
            }

            if ( isJudged && noMatchCount > 0 )
            {
                isJudged = false;
            }
        }

        if ( !isJudged )
        {
            // Values
            _valueIndexesInArgv[arg] = i;
            _values.add(arg);
        }
    }
}

// struct MutexLockObj_Data ---------------------------------------------------------------
struct MutexLockObj_Data
{
#if defined(OS_WIN)
    HANDLE _mutex;
#else
    pthread_mutex_t _mutex;
#endif
};

// class MutexNative --------------------------------------------------------------------------
MutexNative::MutexNative()
{
    _self.create(); //

#if defined(OS_WIN)
    _self->_mutex = CreateMutex( NULL, FALSE, NULL );
#else
    pthread_mutex_init( &_self->_mutex, NULL );
#endif
}

MutexNative::~MutexNative()
{
#if defined(OS_WIN)
    CloseHandle(_self->_mutex);
#else
    pthread_mutex_destroy(&_self->_mutex);
#endif

    _self.destroy(); //
}

bool MutexNative::tryLock()
{
#if defined(OS_WIN)
    return WaitForSingleObject( _self->_mutex, 0 ) == WAIT_OBJECT_0;
#else
    return pthread_mutex_trylock(&_self->_mutex) == 0;
#endif
}

bool MutexNative::lock()
{
#if defined(OS_WIN)
    return WaitForSingleObject( _self->_mutex, INFINITE ) == WAIT_OBJECT_0;
#else
    return pthread_mutex_lock(&_self->_mutex) == 0;
#endif
}

bool MutexNative::unlock()
{
#if defined(OS_WIN)
    return ReleaseMutex(_self->_mutex) != 0;
#else
    return pthread_mutex_unlock(&_self->_mutex) == 0;
#endif
}

// ----------------------------------------------------------------------------------------

#if defined(OS_WIN)

// Windows下获取错误串
inline static String __GetErrorStr( DWORD err )
{
    tchar * buf = NULL;
    DWORD dw = FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL,
        err,
        0,
        (LPTSTR)&buf,
        256,
        NULL
    );
    String str( buf, dw );
    LocalFree(buf);
    return str;
}

#else

#endif

// class DllLoader ------------------------------------------------------------------------
String DllLoader::GetModulePath( void * funcInModule )
{
#if defined(OS_WIN)
    MEMORY_BASIC_INFORMATION mbi;
    HMODULE hMod = ( ( ::VirtualQuery( funcInModule, &mbi, sizeof(mbi) ) != 0 ) ? (HMODULE)mbi.AllocationBase : NULL );

    String sz;
    DWORD dwSize = MAX_PATH >> 1;
    DWORD dwGet = 0;
    //DWORD dwError;
    do
    {
        dwSize <<= 1;
        sz.resize(dwSize);
        dwGet = GetModuleFileName( hMod, &sz[0], dwSize );
        //dwError = GetLastError();
    }
    while ( /* dwError == ERROR_INSUFFICIENT_BUFFER || */ dwSize == dwGet ); // 由于WinXP的错误码与其他的Win不同，所以这里不判断错误码了。只要接收大小和空间大小一样，即视为空间不足。
    return String( sz.c_str(), dwGet );
#else
    Dl_info dlinfo = { 0 };
    int ret = 0;
    ret = dladdr( funcInModule, &dlinfo );
    if ( !ret )
    {
        return GetExecutablePath();
    }
    else
    {
        return dlinfo.dli_fname;
    }
#endif
}

DllLoader::DllLoader() : _hDllModule(NULL)
{

}

DllLoader::DllLoader( String const & dllName ) : _hDllModule(NULL), dllModuleFile(dllName)
{
#if defined(OS_WIN)
    _hDllModule = LoadLibrary( dllName.c_str() );
    if ( !_hDllModule )
    {
        DWORD err = GetLastError();
        _errStr = StringToLocal( __GetErrorStr(err) );
    }
#else
    _hDllModule = dlopen( dllName.c_str(), RTLD_LAZY );
    if ( !_hDllModule )
    {
        _errStr = dlerror();
    }
#endif
}

DllLoader::~DllLoader()
{
    if ( _hDllModule )
    {
    #if defined(OS_WIN)
        FreeLibrary(_hDllModule);
    #else
        dlclose(_hDllModule);
    #endif
        _hDllModule = NULL;
    }
}

char const * DllLoader::errStr() const
{
    return _errStr.c_str();
}

void (* DllLoader::funcAddr( AnsiString const & funcName ) )()
{
    if ( !_hDllModule )
    {
        throw DllLoaderError( DllLoaderError::dllModuleNoLoaded, "`" + StringToLocal(dllModuleFile) + "` module is not loaded" );
    }

#if defined(OS_WIN)
    return (void(*)())GetProcAddress( _hDllModule, funcName.c_str() );
#else
    return (void(*)())dlsym( _hDllModule, funcName.c_str() );
#endif
}

// class FileMapping --------------------------------------------------------------------------
FileMapping::FileMapping()
{
    this->_zeroInit();
}

FileMapping::FileMapping( String const & filePath, FileMappingFlag flag )
{
    this->_zeroInit();
    this->create( filePath, flag );
}

FileMapping::FileMapping(
#if defined(OS_WIN)
    HANDLE file,
#else
    int file,
#endif
    bool isPeekFile,
    FileMappingFlag flag
)
{
    this->_zeroInit();
    this->create( file, isPeekFile, flag );
}

FileMapping::~FileMapping()
{
    this->destroy();
}

bool FileMapping::create( String const & filePath, FileMappingFlag flag )
{
    return this->loadFile( filePath, flag ) && this->map(flag);
}

bool FileMapping::create(
#if defined(OS_WIN)
    HANDLE file,
#else
    int file,
#endif
    bool isPeekFile,
    FileMappingFlag flag
)
{
    return this->loadFile( file, isPeekFile, flag ) && this->map(flag);
}

void FileMapping::destroy()
{
    this->unmap();
    this->unloadFile();
}

bool FileMapping::loadFile( String const & filePath, FileMappingFlag flag )
{
    this->unloadFile();
    _flag = flag;
#if defined(OS_WIN)
    // 计算映射对象的旗标
    static uint fileFlags[] = {
        0,
        GENERIC_READ,
        GENERIC_READ,
        GENERIC_READ | GENERIC_WRITE,
        0,
        GENERIC_EXECUTE | GENERIC_READ,
        GENERIC_EXECUTE | GENERIC_READ,
        GENERIC_EXECUTE | GENERIC_READ | GENERIC_WRITE
    };
    _file = CreateFile( filePath.c_str(), fileFlags[flag], 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr );
    return _file != INVALID_HANDLE_VALUE;
#else
    // 计算映射对象的旗标
    static uint fileFlags[] = {
        0,
        O_RDONLY,
        O_RDONLY,
        O_RDWR | O_CREAT,
        0,
        O_RDONLY,
        O_RDONLY,
        O_RDWR | O_CREAT
    };
    static uint fileMode[] = {
        0,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH,
        0,
        S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH,
        S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH,
        S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH,
    };
    _file = open( filePath.c_str(), fileFlags[flag], fileMode[flag] );
    return _file != -1;
#endif
}

bool FileMapping::loadFile(
#if defined(OS_WIN)
    HANDLE file,
#else
    int file,
#endif
    bool isPeekFile,
    FileMappingFlag flag
)
{
    this->unloadFile();
    _flag = flag;
    _file = file;
    _isPeekFile = isPeekFile;
    return true;
}

size_t FileMapping::getFileSize() const
{
#if defined(OS_WIN)
    LARGE_INTEGER lg;
    lg.LowPart = GetFileSize( _file, (LPDWORD)&lg.HighPart );
    if ( lg.LowPart == INVALID_FILE_SIZE && GetLastError() != NO_ERROR ) lg.QuadPart = 0;
    return (size_t)lg.QuadPart;
#else
    struct stat64 st;
    int rc = fstat64( _file, &st );
    if ( rc < 0 ) st.st_size = 0;
    return (size_t)st.st_size;
#endif
}

void FileMapping::unloadFile()
{
#if defined(OS_WIN)
    if ( !_isPeekFile && _file != INVALID_HANDLE_VALUE )
    {
        CloseHandle(_file);
        _file = INVALID_HANDLE_VALUE;
        _isPeekFile = false;
    }
#else
    if ( !_isPeekFile && _file != -1 )
    {
        close(_file);
        _file = -1;
        _isPeekFile = false;
    }
#endif
}

bool FileMapping::map( FileMappingFlag flag, size_t size, offset_t offset )
{
    this->unmap();

    // 如果是fmfUnspec，则使用记下的标志
    flag = flag == fmfUnspec ? _flag : flag;

    // 记下大小
#if defined(OS_WIN)
    if ( _file != INVALID_HANDLE_VALUE )
#else
    if ( _file != -1 )
#endif
    {
        size_t fsize = this->getFileSize();
        if ( fsize > 0 )
        {
            _size = ( size > 0 ? ( size < fsize ? size : fsize ) : fsize );
        }
        else
        {
            _size = size;
        }
    }
    else
    {
        _size = size;
    }

#if defined(OS_WIN)
    LARGE_INTEGER lg;
    lg.QuadPart = _size;

    // 计算映射对象的旗标
    static uint mappingFlags[] = {
        0,
        PAGE_READONLY,
        PAGE_WRITECOPY,
        PAGE_READWRITE,
        0,
        PAGE_EXECUTE_READ,
        PAGE_EXECUTE_WRITECOPY,
        PAGE_EXECUTE_READWRITE
    };
    // 创建映射对象
    _fileMapping = CreateFileMapping( _file, nullptr, mappingFlags[flag], lg.HighPart, lg.LowPart, nullptr );
    if ( _fileMapping == nullptr ) return false;

    lg.QuadPart = offset;

    // 计算视图旗标
    static uint viewFlags[] = {
        0,
        FILE_MAP_READ,
        FILE_MAP_COPY,
        FILE_MAP_WRITE,
        0,
        FILE_MAP_EXECUTE | FILE_MAP_READ,
        FILE_MAP_EXECUTE | FILE_MAP_COPY,
        FILE_MAP_EXECUTE | FILE_MAP_WRITE
    };
    // 映射视图
    _p = MapViewOfFile( _fileMapping, viewFlags[flag], lg.HighPart, lg.LowPart, 0 );

    if ( _p == nullptr ) return false;
#else
    // 计算视图旗标
    static uint protFlags[] = {
        0,
        PROT_READ,
        PROT_READ | PROT_WRITE,
        PROT_READ | PROT_WRITE,
        0,
        PROT_READ | PROT_EXEC,
        PROT_READ | PROT_WRITE | PROT_EXEC,
        PROT_READ | PROT_WRITE | PROT_EXEC
    };
    static uint mapFlags[] = {
        0,
        MAP_PRIVATE,
        MAP_PRIVATE,
        MAP_SHARED,
        0,
        MAP_PRIVATE | MAP_EXECUTABLE,
        MAP_PRIVATE | MAP_EXECUTABLE,
        MAP_SHARED | MAP_EXECUTABLE
    };
    // 映射视图
    _p = mmap64( nullptr, _size, protFlags[flag], mapFlags[flag] | ( _file == -1 ? MAP_ANONYMOUS : 0 ), _file, offset );

    if ( _p == MAP_FAILED ) return false;
#endif
    return true;
}

void FileMapping::unmap()
{
#if defined(OS_WIN)
    if ( _p != nullptr )
    {
        UnmapViewOfFile(_p);
        _p = nullptr;
    }
    if ( _fileMapping != nullptr )
    {
        CloseHandle(_fileMapping);
        _fileMapping = nullptr;
    }
#else
    if ( _p != MAP_FAILED )
    {
        munmap( _p, _size );
        _p = MAP_FAILED;
    }
#endif
    _size = 0;
}

FileMapping::operator bool() const
{
#if defined(OS_WIN)
    return _fileMapping != nullptr && _p != nullptr;
#else
    return _p != MAP_FAILED;
#endif
}

void FileMapping::_zeroInit()
{
#if defined(OS_WIN)
    _file = INVALID_HANDLE_VALUE;
    _fileMapping = nullptr;
    _p = nullptr;
#else
    _file = -1;
    _p = MAP_FAILED;
#endif
    _size = 0;
    _flag = fmfWriteCopy;
    _isPeekFile = false;
}


// class SharedMemory -------------------------------------------------------------------------
SharedMemory::SharedMemory() :
#if defined(OS_WIN)
    _shm(NULL),
#else
    _shm(-1),
#endif
    _data(NULL),
    _size(0)
{

}

SharedMemory::SharedMemory( String const & shmName, size_t size ) :
#if defined(OS_WIN)
    _shm(NULL),
#else
    _shm(-1),
#endif
    _data(NULL),
    _size(0)
{
    this->create( shmName, size );
}

SharedMemory::~SharedMemory()
{
    this->destroy();
}

bool SharedMemory::create( String const & shmName, size_t size )
{
    this->destroy();

    _shmName = shmName;
    _size = size;

#if defined(OS_WIN)
    union
    {
        struct
        {
            DWORD dwLow;
            DWORD dwHigh;
        };
        size_t size;
    } unSize;
    memset( &unSize, 0, sizeof(unSize) );
    unSize.size = _size;

    // 创建内核对象
    _shm = CreateFileMapping( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, unSize.dwHigh, unSize.dwLow, _shmName.c_str() );
    return _shm != NULL;
#else
    // 创建内核对象
    _shm = shm_open( _shmName.c_str(), O_CREAT | O_RDWR, 0666 );
    if ( _shm < 0 ) return false;
    int rc = ftruncate( _shm, _size );
    if ( rc < 0 ) return false;
    return true;
#endif
}

void SharedMemory::destroy()
{
    this->unlock();

#if defined(OS_WIN)
    if ( _shm != NULL )
    {
        CloseHandle(_shm);
        _shmName.clear();
        _shm = NULL;
        _size = 0;
    }
#else
    if ( _shm > -1 )
    {
        close(_shm);
        shm_unlink( _shmName.c_str() );
        _shmName.clear();
        _shm = -1;
        _size = 0;
    }
#endif
}

void * SharedMemory::lock()
{
    this->unlock();

#if defined(OS_WIN)
    return _data = MapViewOfFile( _shm, FILE_MAP_ALL_ACCESS, 0, 0, 0 );
#else
    return _data = mmap( nullptr, _size, PROT_READ | PROT_WRITE, MAP_SHARED, _shm, 0 );
#endif
}

void SharedMemory::unlock()
{
    if ( _data != nullptr )
    {
    #if defined(OS_WIN)
        UnmapViewOfFile(_data);
    #else
        munmap( _data, _size );
    #endif
        _data = nullptr;
    }
}


} // namespace winux
