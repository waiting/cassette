#include "winplus_definitions.hpp"
#include "winplus_resource.hpp"

namespace winplus
{

Resource::~Resource()
{
    free();
}

void Resource::free()
{
    if ( _lpData && _hResMemBlock )
        UnlockResource(_hResMemBlock);
    if ( _hResMemBlock )
        FreeResource(_hResMemBlock);
}

bool Resource::load( HMODULE hModule, LPCTSTR lpszName, LPCTSTR lpType, WORD wLanguage /*= MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) */ )
{
    _hModule = hModule;
    _hRSRC = FindResourceEx(_hModule, lpType, lpszName, wLanguage );
    if ( !_hRSRC ) return false;
    _dwSize = SizeofResource( _hModule, _hRSRC );
    _hResMemBlock = LoadResource( _hModule, _hRSRC );
    if ( !_hResMemBlock ) return false;
    _lpData = LockResource(_hResMemBlock);
    if ( !_lpData ) return false;
    return true;
}

int Resource::copyTo( LPVOID lpDestination, DWORD dwDestinationSize ) const
{
    DWORD dwSize = dwDestinationSize < getSize() ? dwDestinationSize : getSize();
    if ( lpDestination != NULL && dwSize > 0 )
        CopyMemory( lpDestination, getData(), dwSize );
    return (int)( dwDestinationSize - getSize() );
}

void Resource::_construct( HMODULE hModule, LPCTSTR lpszName, LPCTSTR lpType, WORD wLanguage /*= MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) */ )
{
    _construct();
    load( hModule, lpszName, lpType, wLanguage );
}

void Resource::_construct()
{
    _hModule = NULL;
    _hResMemBlock = NULL;
    _hRSRC = NULL;
    _dwSize = 0;
    _lpData = NULL;
}

} // namespace winplus
