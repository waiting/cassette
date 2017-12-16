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

class WINPLUS_DLL Resource
{
public:
    Resource( HMODULE hModule, UINT nResourceId, LPCTSTR lpType, WORD wLanguage = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) )
    {
        _construct( hModule, MAKEINTRESOURCE(nResourceId), lpType, wLanguage );
    }
    Resource( UINT nResourceId, LPCTSTR lpType, WORD wLanguage = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) )
    {
        _construct( GetModuleHandle(NULL), MAKEINTRESOURCE(nResourceId), lpType, wLanguage );
    }
    Resource( LPCTSTR lpszName, LPCTSTR lpType, WORD wLanguage = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) )
    {
        _construct( GetModuleHandle(NULL), lpszName, lpType, wLanguage );
    }
    Resource( HMODULE hModule, LPCTSTR lpszName, LPCTSTR lpType, WORD wLanguage = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) )
    {
        _construct( hModule, lpszName, lpType, wLanguage );
    }
    Resource()
    {
        _construct();
    }
    ~Resource();

    bool load( HMODULE hModule, LPCTSTR lpszName, LPCTSTR lpType, WORD wLanguage = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) );
    bool load( LPCTSTR lpszName, LPCTSTR lpType, WORD wLanguage = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) )
    {
        return load( GetModuleHandle(NULL), lpszName, lpType, wLanguage );
    }
    bool load( HMODULE hModule, UINT nResourceId, LPCTSTR lpType, WORD wLanguage = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) )
    {
        return load( hModule, MAKEINTRESOURCE(nResourceId), lpType, wLanguage );
    }
    bool load( UINT nResourceId, LPCTSTR lpType, WORD wLanguage = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) )
    {
        return load( GetModuleHandle(NULL), nResourceId, lpType, wLanguage );
    }
    void free();
    // 拷贝到目标缓冲区,返回目标缓冲区与资源的大小差
    // 0:恰好相同,正数:目标缓冲区大小大于资源,负数:目标缓冲区大小小于资源
    int copyTo( LPVOID lpDestination, DWORD dwDestinationSize ) const;

    LPCVOID getData() const { return _lpData; }
    DWORD getSize() const { return _dwSize; }

    operator bool() const { return _lpData != NULL; }
protected:
    void _construct();
    void _construct( HMODULE hModule, LPCTSTR lpszName, LPCTSTR lpType, WORD wLanguage = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) );
private:
    HRSRC _hRSRC;
    HGLOBAL _hResMemBlock;
    HMODULE _hModule;
    DWORD _dwSize;
    LPCVOID _lpData;
    Resource( Resource const & );
    Resource & operator = ( Resource const & );
};

} // namespace winplus

#endif // !defined(__WINPLUS_RESOURCE_HPP__)
