#ifndef __EIENTPL_HPP__
#define __EIENTPL_HPP__

#include "winux.hpp"
#include "eienexpr.hpp"

/** \brief 模板引擎 */
namespace eientpl
{
#ifdef EIENTPL_DLL_USE
    #if defined(_MSC_VER) || defined(WIN32)
        #pragma warning( disable: 4251 )
        #ifdef EIENTPL_DLL_EXPORTS
            #define EIENTPL_DLL  __declspec(dllexport)
        #else
            #define EIENTPL_DLL  __declspec(dllimport)
        #endif

        #define EIENTPL_API __stdcall
    #else
        #define EIENTPL_DLL
        #define EIENTPL_API
    #endif
#else
    #define EIENTPL_DLL
    #define EIENTPL_API
#endif

#define EIENTPL_FUNC_DECL(ret) EIENTPL_DLL ret EIENTPL_API
#define EIENTPL_FUNC_IMPL(ret) ret EIENTPL_API

/** \brief 模板错误 */
class EIENTPL_DLL TplError : public winux::Error
{
public:
    enum
    {
        teNone,                ///< 无错误
    };
    TplError( int errNo, winux::AnsiString const & err ) throw() : winux::Error( errNo, err ) { }
};

/** \brief 模板缓存基类 */
class EIENTPL_DLL TemplateCache
{
public:
    virtual ~TemplateCache() {}
    /** \brief 检测是否已经有还有效的缓存，检查其生命期 */
    virtual bool hasCache( winux::String const & cacheName ) const = 0;
    /** \brief 根据模板文件名和缓存标记id计算缓存名 */
    virtual bool getCacheName( winux::String * cacheName, winux::String const & tplFile, winux::String const & cacheId = "" ) const = 0;
    /** \brief 取得缓存内容字符串 */
    virtual bool readCache( winux::String const & cacheName, winux::String * str ) = 0;
    /** \brief 取得缓存内容字符串 */
    virtual winux::String get( winux::String const & cacheName ) = 0;
    /** \brief 写入缓存内容字符串 */
    virtual bool writeCache( winux::String const & cacheName, winux::String const & str ) = 0;
    /** \brief 设置缓存有效时间（即缓存机制是否开启，0为关闭） */
    virtual void setLifeTime( int lifeTime = 0 ) = 0;
    /** \brief 获取缓存有效时间 */
    virtual int getLifeTime() const = 0;
};

/** \brief 模板类 */
class EIENTPL_DLL Template
{
public:
    Template( winux::String const & tplDir = "", winux::String const & localEncoding = "", winux::String const & targetEncoding = "" );
    virtual ~Template();

    void assign( winux::String const & name, winux::Mixed const & v, bool convertEncoding = true );
    void assignPtr( winux::String const & name, winux::Mixed * p, bool convertEncoding = true );
    void clear();

    /** \brief 运行模板文件并输出内容,tplFile会连接_tplDirPath */
    winux::String output( winux::String const & tplFile, winux::String const & cacheId = "" );
    /** \brief 运行模板文本并输出内容 */
    winux::String outputStr( winux::String const & tplStr, winux::String const & cacheId );

    /** \brief 运行模板文件并输出内容，不带缓存，tplFilePath不连接_tplDirPath */
    winux::String _outputInternal( winux::String const & tplFilePath );

    /** \brief 设置相对目录路径
     *
     *  当调用outputStr()函数时,由于<{load}>元素需要计算相对tpl文档的路径,所以需要设置此项 */
    void setTplDirPath( winux::String const & dirPath );
    winux::String const & getTplDirPath() const { return this->_tplDirPath; }

    /** \brief 设置缓存机制是否开启，NULL为关闭 */
    void setCache( TemplateCache * cache = NULL ) { this->_cache = cache; }

    /** \brief 访问变量场景对象，以便直接操作变量 */
    eienexpr::VarContext * getVarContext() { return &this->_varCtx; }
    /** \brief 访问变量值 */
    winux::Mixed const & getVar( winux::String const & name ) const;

    /** \brief 目标编码转到本地编码 */
    winux::String convFrom( winux::String const & str ) const;
    /** \brief 本地编码转到目标编码 */
    winux::String convTo( winux::String const & str ) const;

    winux::String const & getLocalEncoding() const { return _localEncoding; }
    winux::String const & getTargetEncoding() const { return _targetEncoding; }
private:
    winux::String _tplDirPath; // 模板目录
    winux::String _localEncoding; // 本地编码
    winux::String _targetEncoding; // 目标编码
    TemplateCache * _cache; // 模板缓存
    eienexpr::VarContext _varCtx; // 变量场景

    winux::MembersWrapper<struct Template_Data> _self; // 隐藏的数据

    DISABLE_OBJECT_COPY(Template)
};

/** \brief 模板-文件式缓存 */
class EIENTPL_DLL TemplateFileCache : public TemplateCache
{
public:
    TemplateFileCache( int lifeTime, winux::String const & cacheDir, winux::String const & cacheExt = ".cah" );

    virtual bool hasCache( winux::String const & cacheName ) const;
    virtual bool getCacheName( winux::String * cacheName, winux::String const & tplFile, winux::String const & cacheId = "" ) const;
    virtual bool readCache( winux::String const & cacheName, winux::String * str );
    virtual winux::String get( winux::String const & cacheName );
    virtual bool writeCache( winux::String const & cacheName, winux::String const & str );

    virtual void setLifeTime( int lifeTime = 0 );
    virtual int getLifeTime() const;

    void setCacheDirPath( winux::String const & dirPath );
    winux::String const & getCacheDirPath() const { return this->_cacheDirPath; }

    void setCacheExt( winux::String const & extName );
    winux::String const & getCacheExt() const { return this->_cacheExt; }

private:
    int _cacheLifeTime; // 缓存生命期 0表示不缓存
    winux::String _cacheDirPath; // 缓存目录带分隔符
    winux::String _cacheExt; //缓存文件扩展名 默认为`.cah`

};

/** \brief 模板-内存式缓存 */
class EIENTPL_DLL TemplateMemoryCache : public TemplateCache
{
public:
    struct CacheDataItem
    {
        winux::ulong timeStamp;
        winux::String content;
    };

    typedef std::map< winux::String, CacheDataItem > CacheDataStruct;

    TemplateMemoryCache( int lifeTime, CacheDataStruct * cacheData );

    virtual bool hasCache( winux::String const & cacheName ) const;
    virtual bool getCacheName( winux::String * cacheName, winux::String const & tplFile, winux::String const & cacheId = "" ) const;
    virtual bool readCache( winux::String const & cacheName, winux::String * str );
    virtual winux::String get( winux::String const & cacheName );
    virtual bool writeCache( winux::String const & cacheName, winux::String const & str );

    virtual void setLifeTime( int lifeTime = 0 );
    virtual int getLifeTime() const;
private:
    int _cacheLifeTime; // 缓存生命期 0表示不缓存
    CacheDataStruct * _cacheData;
};

}

#endif // __EIENTPL_HPP__