#ifndef __HTTP_MISC_HPP__
#define __HTTP_MISC_HPP__

namespace http
{

/** \brief 代表HTTP头部
 *
 *  有两种头域：只能出现一次的域存在headers中，能出现多次的域存在multiHeaders中。提供了两套方法分别操作它们。*/
class HTTPWRAPPER_DLL Header
{
public:
    /** \brief 解析/描述头域字段的值 */
    class HTTPWRAPPER_DLL HeaderField
    {
    public:
        HeaderField() { }
        HeaderField( winux::String const & fieldVal ) { parse(fieldVal); }
        HeaderField( winux::String const & firstVal, winux::Mixed const & options );

        winux::String toString() const;
        HeaderField & parse( winux::String const & fieldVal );
        void clear() { _firstVal.clear(); _options.clear(); }

        winux::String getOption( winux::String const & name ) const { return winux::isset( _options, name ) ? _options.at(name) : ""; }
        void setOption( winux::String const & name, winux::String const & value ) { _options[name] = value; }
        bool hasOption( winux::String const & name ) const { return winux::isset( _options, name ); }

        winux::StringStringMap & getOptions() { return _options; }
        winux::StringStringMap const & getOptions() const { return _options; }
        winux::String getFirstVal() const { return _firstVal; }
        void setFirstVal( winux::String const & firstVal ) { _firstVal = winux::StrLower(firstVal); }
    protected:
        static void _ParseOptions( winux::String const & strOptions, winux::StringStringMap * options );

        winux::String _firstVal;
        winux::StringStringMap _options;
    };
    /** \brief Content-Type域 */
    class HTTPWRAPPER_DLL ContentType : public HeaderField
    {
    public:
        ContentType() { }
        ContentType( winux::String const & fieldVal ) : HeaderField(fieldVal) { }
        ContentType( winux::String const & mimeType, winux::Mixed const & options ) : HeaderField( mimeType, options ) { }

        winux::String getMimeType() const { return _firstVal; }
        void setMimeType( winux::String const & mimeType ) { _firstVal = winux::StrLower(mimeType); }
        winux::String getCharset() const { return getOption("charset"); }
        void setCharset( winux::String const & charset ) { setOption( "charset", winux::StrLower(charset) ); }
        winux::String getBoundary() const { return getOption("boundary"); }
        void setBoundary( winux::String const & boundary ) { setOption( "boundary", boundary ); }
    };
    /** \brief Content-Disposition域 */
    class HTTPWRAPPER_DLL ContentDisposition : public HeaderField
    {
    public:
        ContentDisposition() { }
        ContentDisposition( winux::String const & fieldVal ) : HeaderField(fieldVal) { }
        ContentDisposition( winux::String const & firstVal, winux::Mixed const & options ) : HeaderField( firstVal, options ) { }
    };
public:
    Header();
    /** \brief 清空 */
    void clear();
    winux::String toString( bool isAddCrlfAtEnd = true ) const;
    /** \brief 从"HeaderField: XXXXX"字符串解析头部，可重复调用，不会清空先前解析的结果，要清空请调用clear() */
    Header & parse( winux::String const & headerStr );

    winux::String const & operator [] ( winux::String const & name ) const;
    winux::String & operator [] ( winux::String const & name );

    void setHeader( winux::String const & name, winux::String const & value ) { _headers[name] = value; }
    bool hasHeader( winux::String const & name ) const { return _headers.has(name); }
    void delHeader( winux::String const & name ) { /*_headers.erase(name);*/ _headers.del(name); }

    template < typename _HeaderField >
    _HeaderField & get( winux::String const & name, _HeaderField * field ) const
    {
        field->parse( (*this)[name] );
        return *field;
    }
    template < typename _HeaderField >
    _HeaderField get( winux::String const & name ) const
    {
        _HeaderField f;
        f.parse( (*this)[name] );
        return f;
    }

    /** \brief 设置headers,可以增加新头域 */
    winux::OutStringStreamWrapper operator () ( winux::String const & name );

    /** \brief 修改multiheaders,无法增加新头域
     *
     *  \param i 指定是第几个,base on 0 */
    winux::OutStringStreamWrapper operator () ( winux::String const & name, int i );

    void addMultiHeader( winux::String const & name, winux::String const & value );
    void delMultiHeader( int index );

    /** \brief 设置响应行 格式: HttpVersion StatusCode StatusStr\n eg. HTTP/1.1 200 OK
     *
     *  \param setStatus 指示是否同时设置"Status:"头域，FCGI程序必须用Status修改响应状态码 */
    void setResponseLine( winux::String const & responseLine, bool setStatus = true );

    /** \brief 设置请求行 格式： Method URL HttpVersion\n eg. GET /index.do HTTP/1.1 */
    void setRequestLine( winux::String const & requestLine );

    /** \brief 取得状态行,isAddCrlf指示末尾是否添加‘回车换行’，默认不添加 */
    winux::String getStatusLine( bool isAddCrlf = false ) const;

    winux::String getMethod() const { return _method; }
    winux::String getUrl() const { return _url; }
    winux::String getStatusCode() const { return _statusCode; }
    winux::String getStatusStr() const { return _statusStr; }
    winux::String getVersion() const { return _version; }

    Header & setMethod( winux::String const & method ) { _method = method; return *this; }
    Header & setUrl( winux::String const & url ) { _url = url; return *this; }
    Header & setStatusCode( winux::String const & statusCode ) { _statusCode = statusCode; return *this; }
    Header & setStatusStr( winux::String const & statusStr ) { _statusStr = statusStr; return *this; }
    Header & setVersion( winux::String const & version ) { _version = version; return *this; }

    /** \brief 枚举所有头域一遍，多次调用即可，当完成枚举后返回false。再次调用即可重新枚举。 */
    bool enumHeader( winux::String * name, winux::String * value ) const;

private:
    winux::String _method, _url; // for a request */
    winux::String _statusCode, _statusStr; // for a response
    winux::String _version;

    winux::Mixed _headers; ///< 存放只能一次的头
    std::vector< std::pair< winux::String, winux::String > > _multiHeaders; ///< 存放可多次的头
    bool _hasMultiHeader( winux::String const & name ) const;

    // 枚举相关
    mutable int _curHeaderIndex; // 当前枚举到的头部索引
    mutable int _curMultiHeaderIndex; // 当前枚举到的多头部索引
    mutable bool _isCompleteEnum; // 是否完成枚举，从而进行下一次枚举，默认是true
};

/** \brief 封装了Cookie的http协议细节 */
class HTTPWRAPPER_DLL Cookie
{
    class HTTPWRAPPER_DLL ExpiresVal
    {
        winux::String _v;
    public:
        bool empty() const { return _v.empty(); }
        operator winux::String & () { return _v; }
        operator winux::String const & () const { return _v; }

        void operator = ( int v );
        winux::String & operator = ( winux::String const & v ) { return _v = v; }
    };
public:
    winux::String name;
    winux::String value;
    ExpiresVal expires;
    winux::String domain;
    winux::String path;
    bool secure;
    bool httpOnly;

    bool wantSet; // 该Cookie是否要设置

    Cookie() : secure(false), httpOnly(false), wantSet(false) { }
    Cookie(
        winux::String name,
        winux::String value = "",
        int expires = -1,
        winux::String domain = "",
        winux::String path = "",
        bool secure = false,
        bool httpOnly = false
    );

    operator winux::String const & () const { return this->value; }

    // 设置值时将wantSet标志设真
    winux::String & operator = ( winux::String const & v );
    // 获得一个用于Set-Cookie的字符串,不包含"Set-Cookie:"串
    winux::String toSetString() const;

};

inline std::ostream & operator << ( std::ostream & o, Cookie const & cookie )
{
    o << (winux::String)cookie;
    return o;
}

/** \brief 提供操作Cookies相关的功能 */
class HTTPWRAPPER_DLL Cookies
{
    std::map< winux::String, http::Cookie > _cookies;
public:
    Cookies() { }

    /** \brief 添加一个cookie到内部的映射表，并将_wantSet属性设为true
     *
     * 这会导致一个Set-Cookie发送到响应头 */
    void set(
        winux::String name,
        winux::String value = "",
        int expires = -1,
        winux::String domain = "",
        winux::String path = "",
        bool secure = false,
        bool httpOnly = false
    );

    http::Cookie const & get( winux::String const & name ) const { return _cookies.at(name); }

    /** \brief 清空内部的映射表 */
    void clear() { _cookies.clear(); }

    /** \brief 载入COOKIES */
    void loadCookies( char const * cookiesStr );

    /** \brief 将内部映射表内需要设置的cookie发送到头部 */
    void commitTo( http::Header * header );

    /** \brief 倾泻所有cookie以name=value\\n的形式成为一个字符串
     *
     * \return winux::String */
    winux::String dump() const;

    /** \brief 检测是否有该cookie
     *
     * \return bool */
    bool has( winux::String const & name ) const { return _cookies.find(name) != _cookies.end(); }

    /** \brief operator[]会设置一个cookie到内部的映射表，无论元素是否存在。
     *
     * 这不会导致一个Set-Cookie头被发送。
     * 要想发送一个Set-Cookie设置cookie，你需要对返回的Cookie对象进行赋值操作，或将其wantSet属性设为true。
     * \return Cookie & */
    http::Cookie & operator [] ( winux::String const & name );

    /** \brief 不能对返回的Cookie对象进行赋值。 */
    http::Cookie const & operator [] ( winux::String const & name ) const { return _cookies.at(name); }

};

/** \brief 解析和设置get/post变量 */
class HTTPWRAPPER_DLL Vars
{
    winux::Mixed _vars;
public:
    Vars( char const * varsData = NULL );
    Vars( winux::Mixed const & vars );
    void clear();
    winux::String toString() const;
    winux::Mixed const & operator [] ( winux::String const & name ) const;
    winux::Mixed & operator [] ( winux::String const & name );
    bool has( winux::String const & name ) const;
    /** \brief 从urlencoded数据解析变量，不清空原先变量 */
    Vars & parse( char const * varsData );

    /** \brief 取得指定类型的数据
     *
     *  如果没有则返回指定的默认值 */
    template < typename _Ty >
    _Ty get( winux::String const & name, _Ty defval = _Ty() ) const
    {
        return this->has(name) ? (_Ty)_vars[name] : defval;
    }

    winux::Mixed & getVars() { return _vars; }
};

} // namespace http


#endif // __HTTP_MISC_HPP__
