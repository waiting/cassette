
#include "winplus_definitions.hpp"
#include "winplus_compoundfile.hpp"

namespace winplus
{

WINPLUS_FUNC_IMPL(IStreamPtr) CreateNewStream()
{
    HRESULT hr;
    IStreamPtr stream;
    hr = CreateStreamOnHGlobal( NULL, TRUE, &stream ); // 创建 IStream
    if ( FAILED(hr) )
    {
        return NULL;
    }
    return stream;
}

WINPLUS_FUNC_IMPL(IStreamPtr) CreateStreamExistingFile( String const & fileName )
{
    IStreamPtr stream;
    HRESULT hr;
    HANDLE file = CreateFile(
        fileName.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if ( INVALID_HANDLE_VALUE == file )
    {
        return NULL;
    }
    DWORD fileSize, tmpSize;
    fileSize = GetFileSize( file, &tmpSize ); // 获得文件大小
    HGLOBAL block = GlobalAlloc( GMEM_MOVEABLE, fileSize );
    if ( block == NULL )
    {
        CloseHandle(file);
        return NULL;
    }
    
    LPVOID buffer = GlobalLock(block);
    ReadFile( file, buffer, fileSize, &tmpSize, NULL );
    GlobalUnlock(block);
    
    CloseHandle(file); // 内容已拷贝到内存,再无需File
    
    hr = CreateStreamOnHGlobal( block, TRUE, &stream ); // 创建 IStream
    if ( FAILED(hr) )
    {
        GlobalFree(block);
        return NULL;
    }

    return stream;
}

WINPLUS_FUNC_IMPL(IStreamPtr) CreateStreamFromBuffer( LPCVOID lpBuffer, size_t size )
{
    HRESULT hr;
    IStreamPtr streamPtr;
    HGLOBAL hMemBlock = GlobalAlloc( GMEM_MOVEABLE, size );
    if ( hMemBlock == NULL ) return NULL;
    
    LPVOID data = GlobalLock(hMemBlock);
    CopyMemory( data, lpBuffer, size );
    GlobalUnlock(hMemBlock);

    hr = CreateStreamOnHGlobal( hMemBlock, TRUE, &streamPtr ); // 创建 IStream
    if ( FAILED(hr) )
    {
        GlobalFree(hMemBlock);
        return NULL;
    }
    return streamPtr;
}

WINPLUS_FUNC_IMPL(IStreamPtr) CreateStreamFromResourceEx( HMODULE hModule, LPCTSTR lpszName, LPCTSTR lpszType )
{
    IStreamPtr streamPtr;
    HRSRC hrsrcResource = FindResource( hModule, lpszName, lpszType );
    if ( hrsrcResource != NULL )
    {
        DWORD dwSize = SizeofResource( hModule, hrsrcResource );
        HGLOBAL hResBlock = LoadResource( hModule, hrsrcResource );
        LPVOID lpData = LockResource(hResBlock);
        streamPtr = CreateStreamFromBuffer( lpData, dwSize );
        UnlockResource(hResBlock);
        FreeResource(hResBlock);
    }
    return streamPtr;
}

WINPLUS_FUNC_IMPL(IStreamPtr) CreateStreamFromResource( LPCTSTR lpszName, LPCTSTR lpszType )
{
    return CreateStreamFromResourceEx( GetModuleHandle(NULL), lpszName, lpszType );
}

WINPLUS_FUNC_IMPL(IStreamPtr) CreateStreamFromResourceEx( HMODULE hModule, UINT uResourceId, LPCTSTR lpszType )
{
    return CreateStreamFromResourceEx( hModule, MAKEINTRESOURCE(uResourceId), lpszType );
}

WINPLUS_FUNC_IMPL(IStreamPtr) CreateStreamFromResource( UINT uResourceId, LPCTSTR lpszType )
{
    return CreateStreamFromResourceEx( GetModuleHandle(NULL), MAKEINTRESOURCE(uResourceId), lpszType );
}

} // namespace winplus
