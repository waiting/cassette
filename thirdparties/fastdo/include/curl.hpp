#ifndef __CURL_HPP__
#define __CURL_HPP__

// curl.h的一些类型
typedef void CURL;
struct curl_slist;
struct curl_httppost;

namespace eiennet
{

/** \brief curl错误 */
class EIENNET_DLL CUrlError : public winux::Error
{
public:
    CUrlError( int errNo, char const * err ) : winux::Error( errNo, err ) { }
};

/** \brief curl库初始化 */
class EIENNET_DLL CUrlLib
{
public:
    static char const * GetVersion();

    CUrlLib();
    ~CUrlLib();
    DISABLE_OBJECT_COPY(CUrlLib)
};

/** \brief libcurl的字符串链表 */
class EIENNET_DLL SList
{
    struct curl_slist * _slist;
public:
    SList();
    virtual ~SList();
    void free();
    void append( char const * s );

    operator struct curl_slist * () const { return _slist; }
    DISABLE_OBJECT_COPY(SList)
};

/** \brief http post请求以“multipart/formdata”方式发送数据的支持类 */
class EIENNET_DLL PostMultipart
{
    struct curl_httppost * _first;
    struct curl_httppost * _last;
public:
    PostMultipart();
    virtual ~PostMultipart();
    void free();
    /** \brief 添加一个变量值 */
    void addVar( winux::String const & name, winux::String const & value );
    /** \brief 添加一个文件,以filename路径指示的文件为内容 */
    void addFile( winux::String const & name, winux::String const & filename, winux::String const & showfilename = "" );
    /** \brief 添加一个文件,以缓冲区的内容为文件内容,以showfilename为文件名 */
    void addFile( winux::String const & name, char const * data, long size, winux::String const & showfilename );

    operator struct curl_httppost * () const { return _first; }
    DISABLE_OBJECT_COPY(PostMultipart)
};

/** \brief libcurl低层封装,主要提供了CURL句柄资源管理功能 */
class EIENNET_DLL CUrl
{
private:
    CURL * _curl;
    int _errNo;
    winux::AnsiString _errBuf;
protected:
    // '写'回调函数，回调函数将转到成员虚函数进行响应
    static size_t WriteCallback( char * buf, size_t itemSize, size_t count, void * data );
    // 当发生'写'动作
    virtual size_t OnWrite( char * buf, size_t itemSize, size_t count );

    // '读'回调函数
    static size_t ReadCallback( char * buf, size_t itemSize, size_t count, void * data );
    // 当发生'读'动作
    virtual size_t OnRead( char * buf, size_t itemSize, size_t count );

    // '头部'数据回调函数
    static size_t HeaderCallback( char * buf, size_t itemSize, size_t count, void * data );
    // 当'头部'数据到
    virtual size_t OnHeader( char * buf, size_t itemSize, size_t count );

    // '进度'回调函数
    static int ProgressCallback( void * data, double dltotal, double dlnow, double ultotal, double ulnow );
    // 当'下载进度'操作
    virtual int OnDownloadProgress( double dltotal, double dlnow );
    // 当'上传进度'操作
    virtual int OnUploadProgress( double ultotal, double ulnow );
public:
    // 错误码转成文本信息
    //static char const * getErrNoStr( int errNo );

    /** \brief isInit为true将自动调用init()，否则不调用。 */
    CUrl( bool isInit = true );
    CUrl( CUrl const & other );
    virtual ~CUrl();

    /** \brief 释放清理自身curl，复制其他curl，并且拷贝选项设置，并设置自己的一些选项：譬如回调函数对象指针 */
    CUrl & operator = ( CUrl const & other );

    /** \brief 初始化curl */
    void init();
    /** \brief 释放清理curl */
    void cleanup();

    /** \brief 执行操作 */
    bool perform();

    /** \brief 复位各选项 */
    void reset();

    // set options ------------------------------------------------
    /** \brief 设置成http GET请求方式。也可用于把curl恢复成初始状态。
     *
     *  If `b` is 1, this forces the HTTP request to get back to using GET.
     *  Usable if a POST, HEAD, PUT, etc has been used previously using the same curl handle. */
    void setHttpGet( bool b = true );

    /** \brief 设置成http POST请求方式
     *
     *  A parameter set to 1 tells libcurl to do a regular HTTP post.
     *  This will also make the library use a "Content-Type: application/x-www-form-urlencoded" header. */
    void setHttpPost( bool b = true );

    /** \brief 设置URL,url包含scheme信息 */
    void setUrl( winux::String const & url );

    /** \brief 设置超时秒数 */
    void setTimeout( winux::ulong timeout );

    /** \brief 设定错误信息缓冲区，缓冲区大小必须是CURL_ERROR_SIZE=256 */
    void setErrorBuffer( char * errBuf );

    /** \brief 是否关闭进度功能，默认关闭 */
    void setNoProgress( bool b );

    /** \brief 设置用户名 */
    void setUsername( winux::String const & username );

    /** \brief 设置用户密码 */
    void setPassword( winux::String const & password );

    /** \brief 设置Post数据 */
    void setPostFields( char const * data );

    /** \brief 设置Post数据长度 */
    void setPostFieldSize( long size );

    /** \brief 设置Post多部分数据 */
    void setPostMultipart( PostMultipart const & data );

    /** \brief 设置HTTP头 */
    void setHttpHeader( SList const & headers );

    /** \brief 是否显示详细信息，默认不显示 */
    void setVerbose( bool b );

    /** \brief 设置Cookies写入文件 */
    void setCookieJar( winux::String const & filename );

    // set handlers -----------------------------------------------
    typedef size_t (* WriteFunction)( char * buf, size_t itemSize, size_t count, void * data );
    /** \brief 设置'写'处理函数,默认已设为WriteDataCallback() */
    void setWriteHandler( WriteFunction handler );
    /** \brief 设置'写'处理函数的自定义参数,默认设为this,使从静态成员函数传递到普通成员虚函数进行处理 */
    void setWriteHandlerData( void * data );

    typedef size_t (* ReadFunction)( char * buf, size_t itemSize, size_t count, void * data );
    void setReadHandler( ReadFunction handler );
    void setReadHandlerData( void * data );

    typedef size_t (* HeaderFunction)( char * buf, size_t itemSize, size_t count, void * data );
    void setHeaderHandler( HeaderFunction handler );
    void setHeaderHandlerData( void * data );

    typedef int (* ProgressFunction)( void * data, double dltotal, double dlnow, double ultotal, double ulnow );
    void setProgressHandler( ProgressFunction handler );
    void setProgressHandlerData( void * data );

    // attributes -------------------------------------------------
    operator CURL * () const { return _curl; }

    /** \brief 执行后得到的错误码(CURLcode) */
    int errNo() const { return _errNo; }

    /** \brief 执行后得到的错误码字符串 */
    char const * errNoStr() const;

    /** \brief 从错误缓冲区取得错误信息 */
    char const * error() const;

};

/** \brief CURL的HTTP协议封装，默认30秒超时 */
class EIENNET_DLL HttpUrl : public CUrl
{
protected:
    // 当发生'写'动作
    virtual size_t OnWrite( char * buf, size_t itemSize, size_t count );

private:
    winux::AnsiString _response;
public:
    HttpUrl();
    virtual ~HttpUrl();

    /** \brief 取得客户端响应数据
     *
     *  可能是二进制数据，因此以AnsiString对象的引用方式返回，请按照字节缓冲区的方式处理它。 */
    winux::AnsiString const & getResponse() const;

    /** \brief 获取响应数据的大小（字节） */
    winux::ulong getResponseSize() const;

    /** \brief 取得客户端响应数据字符串指针
     *
     *  当数据确定是ANSI文本时可以使用这个方法 */
    char const * getResponseStr( winux::ulong * size = NULL ) const;

    /** \brief 发起HTTP GET请求 */
    bool get( winux::String const & url, http::Header const & headers = http::Header() );
    /** \brief 发起HTTP POST请求 */
    bool post( winux::String const & url, winux::Mixed const & postVars = winux::Mixed(), http::Header const & headers = http::Header() );
    /** \brief 以“multipart/formdata”方式发起HTTP POST请求 */
    bool post( winux::String const & url, PostMultipart const & multipart, http::Header const & headers = http::Header() );

};


} // namespace eiennet

#endif // __CURL_HPP__