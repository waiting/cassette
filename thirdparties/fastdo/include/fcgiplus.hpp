#ifndef __FCGIPLUS_HPP__
#define __FCGIPLUS_HPP__

#if _MSC_VER > 0
#pragma warning( disable: 4244 )
#endif

#include "http.hpp"

struct FCGX_Stream;
struct FcgiServerData;

/** \brief FastCGI相关封装 */
namespace fcgi
{

#ifdef  FCGIPLUS_DLL_USE
    #if defined(_MSC_VER) || defined(WIN32)
        #pragma warning( disable: 4251 )
        #ifdef  FCGIPLUS_DLL_EXPORTS
            #define FCGIPLUS_DLL  __declspec(dllexport)
        #else
            #define FCGIPLUS_DLL  __declspec(dllimport)
        #endif

        #define FCGIPLUS_API __stdcall
    #else
        #define FCGIPLUS_DLL
        #define FCGIPLUS_API
    #endif
#else
    #define FCGIPLUS_DLL
    #define FCGIPLUS_API
#endif

#define FCGIPLUS_FUNC_DECL(ret) FCGIPLUS_DLL ret FCGIPLUS_API
#define FCGIPLUS_FUNC_IMPL(ret) ret FCGIPLUS_API


class FcgiApp;
class Request;
class Response;
class Cookies;

/** \brief 会话数据的存储 */
typedef winux::StringMixedMap SessionData;
/** \brief 每个用户的会话结构 */
struct SessionStruct
{
    SessionData data;
    winux::uint expires;
    SessionStruct() : expires(0) { }
};

/** \brief 会话服务基类 */
class FCGIPLUS_DLL SessionServer
{
protected:
    winux::uint _cleanTimeInterval; ///< 执行"清理过期session"的时间间隔
    winux::uint _sessionLifeTime; ///< 默认session生命期
public:
    SessionServer() : _cleanTimeInterval(1800), _sessionLifeTime(3600) { }
    virtual ~SessionServer() { }

    winux::uint getCleanTimeInterval() const { return _cleanTimeInterval; }
    winux::uint getSessionLifeTime() const { return _sessionLifeTime; }

    /** \brief 执行清理操作,将过期的session删除 */
    virtual void clean() = 0;
    /** \brief 启动一个session,得到一个SessionData,用来读写入数据 */
    virtual bool startup( winux::String const & sessid, SessionData * sessData ) = 0;
    /** \brief 保存一个session,将SessionData存储,可以指定生命期 */
    virtual bool save( winux::String const & sessid, SessionData const * sessData, winux::uint thisLifeTime = 0 ) = 0;
};

/** \brief 内存式会话服务
 *
 * 存放于FCGI进程内，若FCGI进程崩溃，数据即丢失
 * 若采取这种方式，则整个网站只是一个FCGI程序，否则会话数据无法共享 */
class FCGIPLUS_DLL MemorySessionServer : public SessionServer
{
private:
    /** \brief 会话结构映射表,SessId与SessionStruct相关联 */
    typedef std::map<winux::String, SessionStruct> SessionStructMap;
    SessionStructMap _sessions;
public:
    MemorySessionServer( winux::uint cleanTimeInterval = 1800, winux::uint sessionLifeTime = 1800 );
    virtual void clean();
    virtual bool startup( winux::String const & sessid, SessionData * sessData );
    virtual bool save( winux::String const & sessid, SessionData const * sessData, winux::uint thisLifeTime = 0 );

    DISABLE_OBJECT_COPY(MemorySessionServer)
};

/** \brief 文件式会话服务 */
class FCGIPLUS_DLL FileSessionServer : public SessionServer
{
public:
    FileSessionServer( winux::String const & savePath, winux::uint cleanTimeInterval = 1800, winux::uint sessionLifeTime = 1800 );
    virtual void clean();
    virtual bool startup( winux::String const & sessid, SessionData * sessData );
    virtual bool save( winux::String const & sessid, SessionData const * sessData, winux::uint thisLifeTime = 0 );
private:
    winux::String _savePath; // session文件保存路径
    DISABLE_OBJECT_COPY(FileSessionServer)
};

/** \brief 网站会话
 *
 * 运行机制:
 *   当创建一个Session对象时,若未给定sessid,则向Cookie查询一个sessid.
 *   若没有,则新建一个sessid的cookie,值为一个唯一串,生成方式自定.
 *   通过sessid调用sessionServer启动一个会话. */
class FCGIPLUS_DLL Session
{
    Request & _req;
    SessionData _data;
    winux::String _sessid;             // session唯一标识
    winux::uint _cookieSessIdLifeTime; // sessid cookie生命期
    winux::uint _sessionLifeTime;      // session生命期
    bool _isModified;                  // 是否修改过
public:
    Session( Request & req, winux::String const & sessid = "", winux::uint cookieSessIdLifeTime = 0, winux::uint sessionLifeTime = 0, winux::String const & cookiesPath = "" );
    ~Session();
    winux::Mixed & operator [] ( winux::String const & name );
    winux::Mixed const & operator [] ( winux::String const & name ) const;
    bool has( winux::String const & name ) const;
    winux::Mixed const & get( winux::String const & name ) const;
    void set( winux::String const & name, winux::Mixed const & v );
    bool del( winux::String const & name );

    DISABLE_OBJECT_COPY(Session)
};

/** \brief 代表客户请求 */
class FCGIPLUS_DLL Request
{
private:
    winux::MembersWrapper<struct Request_Data> _self;
public:
    Request();
    ~Request();

    /** \brief 初始化FCGX_Request */
    void init( FcgiApp * app, int sock, int flags );

    /** \brief 完成这次请求,清空缓冲区 */
    void finish();

    /** \brief 等待接收请求，成功返回true，否则返回false。阻塞式
     *
     * 若接收请求成功，会解析加载相关变量，如Cookies,Get,Post变量。 */
    bool accept();

    /** \brief 处理environVars/get/post/cookies数据 */
    bool processData();

    /** \brief 获取环境变量 */
    winux::String const & operator [] ( winux::String const & name ) const;

    /** \brief 输出全部环境变量到Response */
    void dumpEnvTo( Response * rsp, char * * envp = NULL ) const;

    /** \brief 环境变量 */
    winux::StringStringMap environVars;
    /** \brief Cookie变量 read/write */
    http::Cookies cookies;
    /** \brief GET变量 readonly */
    http::Vars get;
    /** \brief POST变量 readonly */
    http::Vars post;
    /** \brief FcgiApp对象指针 */
    FcgiApp * fcgiApp;

    friend class Response;
    friend class FcgiApp;

    DISABLE_OBJECT_COPY(Request)
};

/** \brief Fcgi输出管理器
 *
 *  该类把标准流重定向到指定流(fcgi流)，以便我们用cout输出html\n
 *  该类会把输出流重定向到一个缓冲区流，从而延缓输出，以便我们方便设置HTTP头和计算内容长度 */
class FCGIPLUS_DLL FcgiOutputMgr
{
private:
    winux::MembersWrapper<struct FcgiOutputMgr_Data> _self;

    void _backupStd();
    void _resumeStd();
    void _redirectToFcgi();

    // 尝试编码转换,返回值 0:不需要进行编码转换, 1:需要进行编码转换并且执行了, 2:需要进行编码转换,由于之前已经转换成功,所以直接返回
    int _tryEncodingConvert();
    // 是否需要进行编码转换
    // 不需要转换的情况：a.没有设置本地编码或者目标编码，b.本地编码和目标编码相同，c.MIME不是text类型
    bool _isNeedEncodingConvert() const;

public:
    // FCGX_Stream * fcgiOut, FCGX_Stream * fcgiErr
    FcgiOutputMgr( struct FCGX_Stream * fcgiOut, struct FCGX_Stream * fcgiErr );
    ~FcgiOutputMgr();

    /** \brief 指定本地编码(Response会自动设置本地编码)
     *
     *  本地编码指编译器的默认编码,visualstudio是gbk,gcc是utf-8\n
     *  设置字符串编码信息setLocalEncoding()/setTargetEncoding() */
    void setLocalEncoding( winux::String const & local );

    /** \brief 指定目标编码(Response不会自动设置)
     *
     *  目标编码要和Content-Type的charset对应 */
    void setTargetEncoding( winux::String const & target );

    winux::String getLocalEncoding() const;
    winux::String getTargetEncoding() const;

    /** \brief 目标编码转到本地编码 */
    winux::String convFrom( winux::String const & str ) const;
    /** \brief 本地编码转到目标编码 */
    winux::String convTo( winux::String const & str ) const;

    void echo( winux::String const & str );
    void write( void const * data, int size );
    void printf( char const * format, va_list va );
    /** \brief 获取缓冲输出流中数据的大小 */
    winux::uint getCacheOutStreamSize() const;
    /** \brief 获取编码转换后的内容大小 */
    winux::uint getConvertedContentSize();

    friend class Response;

    DISABLE_OBJECT_COPY(FcgiOutputMgr)
};

/** \brief 代表一个客户响应 */
class FCGIPLUS_DLL Response
{
private:
    FcgiOutputMgr _outputMgr; ///< 输出管理器
    winux::String _mimeType; ///< MIME类型
    bool _autoContentType; ///< 自动设置Content-Type
public:
    Request & req; ///< Request对象的引用
    http::Header & header; ///< 表示头部

    explicit Response( Request & request );
    ~Response();

    void echo( winux::String const & str );
    void write( void const * data, int size );
    void write( winux::Buffer const & buf );
    void printf( char const * format, ... );

    winux::String getLocalEncoding() const { return _outputMgr.getLocalEncoding(); }
    winux::String getTargetEncoding() const { return _outputMgr.getTargetEncoding(); }

    winux::String convFrom( winux::String const & str ) const { return _outputMgr.convFrom(str); }
    winux::String convTo( winux::String const & str ) const { return _outputMgr.convTo(str); }
    /** \brief 设置mime */
    void setMimeType( winux::String const & mime ) { _mimeType = mime; }
    winux::String getMimeType() const { return _mimeType; }

    /** \brief 是否自动设置Content-Type
     *
     *  Response在即将完成输出前会配合MIME和目标编码自动设置Content-Type\n
     *  如果MIME不是文本,则不会设置charset部分 */
    void setAutoContentType( bool autoContentType = true ) { _autoContentType = autoContentType; }

    /** \brief 设置charset，即设置输出目标编码 */
    void setCharset( winux::String const & charset ) { _outputMgr.setTargetEncoding(charset); }
    winux::String getCharset() const { return _outputMgr.getTargetEncoding(); }

    /** \brief import一个do文件，并把它的输出作为自己的输出，若错误则显示错误
     *
     *  \param doFile 文件路径
     *  \param runParam 应传入WebMain()传来的runParam，如果是NULL，将从fcgiApp取得runParam
     *  \param retCode 表示导入do的WebMain()返回代码,为NULL则忽略此值 */
    void import( winux::String const & doFile, void * runParam = NULL, int * retCode = NULL );

    DISABLE_OBJECT_COPY(Response)
};

/** \brief FCGI程序类，代表整个FCGI程序 */
class FCGIPLUS_DLL FcgiApp
{
public:

    /** \brief 构造函数，初始化操作
     *  \param conf 配置对象 */
    FcgiApp( winux::Configure & conf, struct FcgiServerData * servData = NULL );
    ~FcgiApp();

    /** \brief 运行处理 */
    int run( void * runParam );

    /** \brief 取得do传递的参数 */
    void * getParam() const { return _param; };
    template < typename _Ty >
    _Ty getParam() const { return reinterpret_cast<_Ty>(_param); };

    /** \brief 设置do传递参数 */
    void setParam( void * param ) { this->_param = param; }

    /** \brief 取得run()传递的参数 */
    void * getRunParam() const { return _runParam; };
    template < typename _Ty >
    _Ty getRunParam() const { return reinterpret_cast<_Ty>(_runParam); };

    /** \brief 取得服务器相关数据 */
    struct FcgiServerData * getServerData() { return _servData; }
    void setServerData( struct FcgiServerData * data ) { _servData = data; }

    /** \brief 取得/设置SessionServer */
    SessionServer * getSessServ() { return _sessServ; }
    void setSessServ( SessionServer * sessServ ) { this->_sessServ = sessServ; }

    /** \brief 执行do文件的错误码 */
    enum ErrorCode
    {
        Err_Success,
        Err_DoFileNotFound,
        Err_WebMainNotFound,
        Err_InfiniteRecursion,
    };

    /** \brief 执行生成页面内容的过程
     *
     * 默认将以SCRIPT_FILENAME载入动态模块的方式运行处理
     *  \return 0:表示成功,1:表示找不到或无权限,2:表示没有WebMain() */
    virtual ErrorCode execWebMain( Response * rsp, void * runParam, int * retCode );

    /** \brief import一个do文件，并把它的输出作为指定rsp的输出
     *
     *  \param doFile do文件路径
     *  \param rsp 指示输出到的Response对象
     *  \param retCode 表示导入do的WebMain()返回代码,为NULL则忽略此值
     *  \param errDetail 载入do的详细错误信息
     *  \return 0:表示导入成功,1:表示找不到或无权限,2:表示没有WebMain(),3:表示无穷递归调用了 */
    ErrorCode importDoFile( winux::String const & doFile, Response * rsp, void * runParam, int * retCode, winux::String * errDetail );

    /** \brief 产品模式下,显示所有已经加载的do文件 */
    int loadedDoFiles( winux::Mixed * arrDoFiles ) const;

    /** \brief 卸载指定的do文件,如果是当前页面则不会释放 */
    bool unloadDoFile( winux::String const & doFullPath );

    /** \brief 加载一个模块,若已经加载则返回其引用. */
    winux::DllLoader & loadModule( winux::String const & modFile );

    /** \brief 显示已经加载的模块 */
    int loadedModules( winux::Mixed * arrModules ) const;

    /** \brief 卸载一个模块 */
    bool unloadModule( winux::String const & modFile );

    /** \brief 请求 */
    Request request;

    /** \brief 配置 */
    winux::Configure & config;

private:
    ////////////////////////////////////////////////////////////////////////////////
    winux::MembersWrapper<struct FcgiApp_Data> _self;
    SessionServer * _sessServ; // session server
    void * _param; // setParam()传递进来的参数
    void * _runParam; // run()传递进来的参数
    struct FcgiServerData * _servData; // server data
    ////////////////////////////////////////////////////////////////////////////////
    ErrorCode _importDoFileDevelopment( winux::String const & doFullPath, Response * rsp, void * runParam, int * retCode, winux::String * errDetail );
    ErrorCode _importDoFileProduction( winux::String const & doFullPath, Response * rsp, void * runParam, int * retCode, winux::String * errDetail );

    friend class Request;
    DISABLE_OBJECT_COPY(FcgiApp)
};

} // namespace fcgi


#endif // __FCGIPLUS_HPP__
