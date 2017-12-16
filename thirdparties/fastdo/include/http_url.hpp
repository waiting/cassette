#ifndef __HTTP_URL_HPP__
#define __HTTP_URL_HPP__

namespace http
{

/** \brief URL类 */
class HTTPWRAPPER_DLL Url
{
public:
    /** \brief URL段解析/使用标记 */
    enum UrlSegmentFlags
    {
        urlSimple = 0, ///< 简单的分段
        urlQueryVars = 1, ///< 解析查询部分的变量.
        urlPathComp = 2,  ///< 将路径部分解析成PathComp数组.
        urlPathMatrixParams = 4 | urlPathComp, ///< 解析路径部分的MatrixURIs参数,当指定此flag时,urlPathComp强制启用.
        urlCanRelative = 8, ///< 可以是相对URL. eg. 'www.abc.com/aa/bb/cc'这种URL的'www.abc.com'不会解析成host部分,而是path部分.
        urlDefault = urlQueryVars | urlCanRelative,
        urlAll = urlPathComp | urlPathMatrixParams | urlQueryVars | urlCanRelative,
    };

public:
    explicit Url( int flags = urlDefault ) : _flags(flags), _isRelative(false)
    {
    }

    explicit Url( winux::String const & url, int flags = urlDefault ) : _flags(flags), _isRelative(false)
    {
        this->parse(url);
    }

    virtual ~Url()
    {
    }

    // attributes -------------------------------------------------------------------------

    /** \brief 获取URL解析和使用标志. */
    winux::uint getFlags() const { return _flags; }

    /** \brief 是否为相对URL.
     *
     *  相对URL指不包括scheme和host部分,并且不以'/'开头的URL. */
    bool isRelative() const { return _isRelative; }

    /** \brief 获取URL方案.
     *
     *  方案决定了应用层协议和默认端口号. */
    winux::String const & getScheme() const { return _scheme; }

    /** \brief 获取用户名. */
    winux::String const & getUser() const { return _user; }

    /** \brief 获取密码. */
    winux::String const & getPwd() const { return _pwd; }

    /** \brief 获取主机名(域名). */
    winux::String const & getHost() const { return _host; }

    /** \brief 获取端口号,如果不指定则为scheme默认决定的端口. */
    winux::String const & getPort() const { return _port; }

    /** \brief 获取未解析的路径段字符串.不以'/'开头.
     *
     *  路径字符串可能包含MatrixURIs params,根据实际情况再决定是否直接用到别的需要路径参数的调用中. */
    winux::String const & getRawPathStr() const { return _rawPathStr; }

    /** \brief 获取路径部件数.
     *
     *  路径部件PathComp由一个只含一个KV对的集合构成.
     *  K为子路径名,V为MatrixURIs params的集合. */
    int getPathCompsCount() const { return _pathComps.getCount(); }

    /** \brief 获取路径.
     *
     *  会根据URL flags自动组装可用的路径字符串. */
    winux::String getPath() const;

    //winux::Mixed const & getPathMatrixParams( winux::String const & pathComp ) const;
    //winux::Mixed const & getPathMatrixParam( winux::String const & pathComp, winux::String const & name ) const;

    /** \brief 获取指定路径部件的MatrixURIs params集合的常引用. */
    winux::Mixed const & getPathMatrixParams( int iPathComp ) const;

    /** \brief 获取指定路径部件的MatrixURIs params集合的指定参数. */
    winux::Mixed const & getPathMatrixParam( int iPathComp, winux::String const & name ) const;

    /** \brief 获取未解析的查询段字符串.不以'?'开头. */
    winux::String const & getRawQueryStr() const { return _rawQueryStr; }

    /** \brief 获取查询参数集合的常引用. */
    winux::Mixed const & getQueryParams() const { return _query; }

    /** \brief 获取指定的查询参数. */
    winux::Mixed const & getQueryParam( winux::String const & name ) const { return _query[name]; }

    /** \brief 获取文章分段名. */
    winux::String const & getFragment() const { return _fragment; }

    // methods ----------------------------------------------------------------------------

    /** \brief 根据flags和存储的信息,组装成整个URL串. */
    winux::String toString() const;

    /** \brief 组装方案段. */
    bool assembleSchemeSegment( winux::String * urlSegment ) const;

    /** \brief 组装主机段. */
    bool assembleHostSegment( winux::String * urlSegment, bool hasSchemeSegment ) const;

    /** \brief 组装路径段. */
    bool assemblePathSegment( winux::String * urlSegment, bool hasHostSegment, bool hasQuerySegment, bool hasFragmentSegment ) const;

    /** \brief 组装查询段. */
    bool assembleQuerySegment( winux::String * urlSegment, bool incStartChar ) const;

    /** \brief 组装Fragment段. */
    bool assembleFragmentSegment( winux::String * urlSegment, bool incStartChar ) const;

    /** \brief 倾泻所有信息成一个Mixed(Collection). */
    winux::Mixed dump() const;

    /** \brief 用URL解析器解析一个URL. */
    Url & parse( winux::String const & url );

    /** \brief 设置URL解析和使用标志. */
    Url & setFlags( winux::uint flags ) { _flags = flags; return *this; }

    /** \brief 设置URL是否为相对.
     *
     *  当这个设置为true时,无法拼装方案和主机段. */
    Url & setRelative( bool relative ) { _isRelative = relative; return *this; }

    /** \brief 设置URL方案. */
    Url & setScheme( winux::String const & scheme ) { _scheme = scheme; return *this; }

    /** \brief 设置URL主机段的用户名(可选). */
    Url & setUser( winux::String const & user ) { _user = user; return *this; }

    /** \brief 设置URL主机段的密码(可选). */
    Url & setPwd( winux::String const & pwd ) { _pwd = pwd; return *this; }

    /** \brief 设置URL主机段的主机名. */
    Url & setHost( winux::String const & host ) { _host = host; return *this; }

    /** \brief 设置URL主机段的端口号(可选). */
    Url & setPort( winux::String const & port ) { _port = port; return *this; }

    /** \brief 设置一个未解析路径串(开头不含'/'),会根据flags决定是否解析成PathComps. */
    Url & setRawPathStr( winux::String const & rawPathStr );

    /** \brief 设置一个PathComps数组,会自动组装一个未解析路径串. */
    Url & setPathComps( winux::Mixed const & pathComps );

    /** \brief 设置一个未解析查询串(开头不含'?'),会根据flags决定是否解析成Query参数集合. */
    Url & setRawQueryStr( winux::String const & rawQueryStr );

    /** \brief 设置一个Query参数集合,会自动组装一个未解析查询串. */
    Url & setQuery( winux::Mixed const & query );

    /** \brief 设置URL Fragment段. */
    Url & setFragment( winux::String const & fragment ) { _fragment = fragment; return *this; }


private:
    winux::uint _flags;
    bool _isRelative;
    // scheme segment
    winux::String _scheme;
    // host segment
    winux::String _user;
    winux::String _pwd;
    winux::String _host;
    winux::String _port;
    // path segment
    winux::String _rawPathStr; // 首不含'/'
    //[ { pathname:{ a:1, b:2 } }, { pathname1:{ a:1, b:2 } }, ... ]
    winux::Mixed _pathComps;
    // query segment
    winux::String _rawQueryStr; // 首不含'?'
    winux::Mixed _query;
    // fragment segment
    winux::String _fragment; // 首不含'#'

    friend class UrlParser;
};

/** \brief URL解析器 */
class HTTPWRAPPER_DLL UrlParser
{
public:
    UrlParser( Url * pUrl ) : _pUrl(pUrl) { }

    /** \brief 解析一个URL. */
    void parse( winux::String const & url );
    /** \brief 解析主机段,主机段必须是'//'之后和'/'之前的字符串,不含'//'. */
    void parseHostSegment( winux::String const & hostSegment );
    /** \brief 解析路径段,路径段必须是'/'之后和'?''#'之前的字符串,不含'/'. */
    void parsePathSegment( winux::String const & pathSegment );
    /** \brief 解析查询段,查询段必须是'?'之后和'#'之前的字符串,不含'?'. */
    void parseQuerySegment( winux::String const & querySegment );

private:
    /** \brief 解析MatrixUriParams,必须是';'分隔的K=V,首字不为';'. */
    void _parseMatrixUriParams( winux::String const & matrixSegment, winux::Mixed * pMatrixParams );

    Url * _pUrl;
};

} // namespace http


#endif // __HTTP_URL_HPP__
