
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

WINPLUS_FUNC_IMPL(IStreamPtr) CreateStreamFromBuffer( LPCVOID buffer, DWORD size )
{
    HRESULT hr;
    IStreamPtr stream;
    HGLOBAL block = GlobalAlloc( GMEM_MOVEABLE, size );
    if ( block == NULL )
    {
        return NULL;
    }
    
    LPVOID data = GlobalLock(block);
    CopyMemory( data, buffer, size );
    GlobalUnlock(block);

    hr = CreateStreamOnHGlobal( block, TRUE, &stream ); // 创建 IStream
    if ( FAILED(hr) )
    {
        GlobalFree(block);
        return NULL;
    }
    return stream;
}

WINPLUS_FUNC_IMPL(IStreamPtr) CreateStreamFromResourceEx( HMODULE module, UINT resourceId, LPCTSTR type )
{
    IStreamPtr stream;
    HRSRC resource = FindResource( module, MAKEINTRESOURCE(resourceId), type);
    if ( resource != NULL )
    {
        DWORD size = SizeofResource( module, resource );
        HGLOBAL resBlock = LoadResource( module, resource );
        LPVOID data = LockResource(resBlock);
        stream = CreateStreamFromBuffer( data, size );
        UnlockResource(resBlock);
        FreeResource(resBlock);
    }

    return stream;
}

WINPLUS_FUNC_IMPL(IStreamPtr) CreateStreamFromResource( UINT resourceId, LPCTSTR type )
{
    return CreateStreamFromResourceEx( GetModuleHandle(NULL), resourceId, type );
}

} // namespace winplus
