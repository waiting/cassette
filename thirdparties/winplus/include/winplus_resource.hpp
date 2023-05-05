//////////////////////////////////////////////////////////////////////////
// purpose:  资源相关
// author:   WT
//////////////////////////////////////////////////////////////////////////

#if !defined(__WINPLUS_RESOURCE_HPP__)
#define __WINPLUS_RESOURCE_HPP__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace winplus
{
/** \brief 加载模块资源 */
class WINPLUS_DLL Resource
{
public:
    /** \brief 构造函数0 */
    Resource()
    {
        _zeroInit();
    }

    /** \brief 构造函数1-1 */
    Resource( HMODULE hModule, LPCTSTR lpszName, LPCTSTR lpType, WORD wLanguage = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) )
    {
        _zeroInit();
        this->load( hModule, lpszName, lpType, wLanguage );
    }

    /** \brief 构造函数1-2 */
    Resource( HMODULE hModule, UINT nResourceId, LPCTSTR lpType, WORD wLanguage = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) )
    {
        _zeroInit();
        this->load( hModule, MAKEINTRESOURCE(nResourceId), lpType, wLanguage );
    }

    /** \brief 构造函数2-1 */
    Resource( LPCTSTR lpszName, LPCTSTR lpType, WORD wLanguage = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) )
    {
        _zeroInit();
        this->load( GetModuleHandle(NULL), lpszName, lpType, wLanguage );
    }

    /** \brief 构造函数2-2 */
    Resource( UINT nResourceId, LPCTSTR lpType, WORD wLanguage = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) )
    {
        _zeroInit();
        this->load( GetModuleHandle(NULL), MAKEINTRESOURCE(nResourceId), lpType, wLanguage );
    }

    virtual ~Resource();

    /** \brief 释放 */
    void free();

    /** \brief 加载资源1-1 */
    bool load( HMODULE hModule, LPCTSTR lpszName, LPCTSTR lpType, WORD wLanguage = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) );

    /** \brief 加载资源1-2 */
    bool load( HMODULE hModule, UINT nResourceId, LPCTSTR lpType, WORD wLanguage = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) )
    {
        return this->load( hModule, MAKEINTRESOURCE(nResourceId), lpType, wLanguage );
    }

    /** \brief 加载资源2-1 */
    bool load( LPCTSTR lpszName, LPCTSTR lpType, WORD wLanguage = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) )
    {
        return this->load( GetModuleHandle(NULL), lpszName, lpType, wLanguage );
    }

    /** \brief 加载资源2-2 */
    bool load( UINT nResourceId, LPCTSTR lpType, WORD wLanguage = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) )
    {
        return this->load( GetModuleHandle(NULL), nResourceId, lpType, wLanguage );
    }

    /** \brief 用加载到的资源创建一个IStream* */
    IStreamPtr createStream() const;

    /** \brief 拷贝到目标缓冲区，返回目标缓冲区与资源的大小差
     *
     *  \return 0:恰好相同，正数:目标缓冲区大小大于资源，负数:目标缓冲区大小小于资源 */
    ssize_t copyTo( LPVOID lpDestination, size_t uDestinationSize ) const;

    template < typename _Ty >
    std::remove_const_t<_Ty> const * getData() const { return (std::remove_const_t<_Ty> const *)_lpData; }
    LPCVOID getData() const { return _lpData; }
    size_t getSize() const { return _uSize; }

    operator bool() const { return _lpData != NULL; }
protected:
    void _zeroInit();
private:
    //HRSRC _hRSRC;
    HGLOBAL _hResMemBlock;
    //HMODULE _hModule;
    size_t _uSize;
    LPCVOID _lpData;

    DISABLE_OBJECT_COPY(Resource)
};

} // namespace winplus

#endif // !defined(__WINPLUS_RESOURCE_HPP__)
