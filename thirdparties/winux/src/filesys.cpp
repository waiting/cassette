#if !defined(SAG_COM) && ( (defined(WIN64) || defined(_WIN64) || defined(__WIN64__)) || (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)) )

#else
#define __USE_LARGEFILE64
#endif

#include "utilities.hpp"
#include "encoding.hpp"
#include "smartptr.hpp"
#include "strings.hpp"
#include "filesys.hpp"
#include "time.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <functional>

#if defined(OS_WIN)
    #include <sys/utime.h>
    #include <direct.h>
    #include <io.h>
    #include <process.h>
    #include <tchar.h>
#else
    #include <utime.h>
    #include <unistd.h>
    #include <errno.h>
    #include <wchar.h>
#endif

#if defined(OS_WIN)
    #ifdef UNICODE
        #define _tgetcwd _wgetcwd
        #define _tchdir _wchdir
        #define _ttoi _wtoi
        #define _tcsrchr wcsrchr
    #else
        #define _tgetcwd _getcwd
        #define _tchdir _chdir
        #define _ttoi atoi
        #define _tcsrchr strrchr
    #endif
#else // LINUX
    #ifdef UNICODE
        #define _tgetcwd wgetcwd
        #define _tchdir wchdir
        #define _ttoi wtoi
        #define _tcsrchr wcsrchr
        #define _tcsftime wcsftime
        #define _fgetts fgetws
        #define _tcslen wcslen
    #else
        #define _tgetcwd getcwd
        #define _tchdir chdir
        #define _ttoi atoi
        #define _tcsrchr strrchr
        #define _tcsftime strftime
        #define _fgetts fgets
        #define _tcslen strlen
    #endif
    // Linux别名
    #define _getcwd getcwd
    #define _stat stat
    #define _stat64 stat64
    #define _fstat64 fstat64
    #define _mkdir mkdir
    #define _stricmp strcasecmp
    #define _wcsicmp wcscasecmp
    #define _utimbuf utimbuf
    #define O_TEXT 0
    #define O_BINARY 0
#endif

namespace winux
{
#include "is_x_funcs.inl"

inline static String::size_type __StrRFindDirSep( String const & str )
{
    for ( size_t i = str.length(); i--; )
    {
        if ( str[i] == '/' || str[i] == '\\' )
        {
            return i;
        }
    }
    return String::npos;
}

WINUX_FUNC_IMPL(String) GetExecutablePath( void )
{
    String path;
    size_t n = 130;
    ssize_t nGet;
    do
    {
        n <<= 1;
        path.resize(n);
    #if defined(OS_WIN)
        nGet = GetModuleFileName( NULL, &path[0], (DWORD)path.size() );
    #else
        nGet = readlink( "/proc/self/exe", &path[0], path.size() );
    #endif
    } while ( nGet == (ssize_t)path.size() );
    return String( path.c_str(), nGet );
}

WINUX_FUNC_IMPL(String) FilePath( String const & fullPath, String * fileName )
{
    String path;
    String::size_type pos = __StrRFindDirSep(fullPath);
    if ( pos != String::npos )
    {
        path = fullPath.substr( 0, pos );
        if ( fileName != NULL ) *fileName = fullPath.substr( pos + 1 );
    }
    else
    {
        path = TEXT("");
        if ( fileName != NULL ) *fileName = fullPath;
    }
    return path;
}

WINUX_FUNC_IMPL(String) FileTitle( String const & fileName, String * extName )
{
    String fileTitle;
    String::size_type pos = fileName.rfind('.');
    if ( pos != String::npos )
    {
        fileTitle = fileName.substr( 0, pos );
        if ( extName != NULL ) *extName = fileName.substr( pos + 1 );
    }
    else
    {
        fileTitle = fileName;
        if ( extName != NULL ) *extName = TEXT("");
    }
    return fileTitle;
}

WINUX_FUNC_IMPL(bool) IsAbsPath( String const & path )
{
#if defined(OS_WIN)
    return path.empty() ? false : ( ( path[0] == '/' || path[0] == '\\' ) || ( path.length() > 1 && path[1] == ':' ) );
#else
    return path.empty() ? false : path[0] == DirSep[0];
#endif
}

WINUX_FUNC_IMPL(String) NormalizePath( String const & path )
{
    StringArray pathSubs;
    size_t n = StrSplit( path, TEXT("/\\"), &pathSubs );
    size_t i, c = n;
    for ( i = 0; i < c; )
    {
        if ( i > 0 && pathSubs[i - 1] != TEXT("..") && !IsAbsPath( pathSubs[i - 1] + DirSep ) && pathSubs[i] == TEXT("..") )
        {
            size_t k;
            for ( k = i + 1; k < c; k++ )
            {
                pathSubs[k - 2] = pathSubs[k];
            }
            c -= 2;
            --i;
        }
        else if ( pathSubs[i] == TEXT(".") )
        {
            size_t k;
            for ( k = i + 1; k < c; k++ )
            {
                pathSubs[k - 1] = pathSubs[k];
            }
            c -= 1;
        }
        else
            ++i;
    }

    String r;
    for ( i = 0; i < c; ++i )
    {
        r += pathSubs[i];
        if ( i != c - 1 )
        {
            r += DirSep;
        }
    }

    if ( r.length() > 1 && r[r.length() - 1] == DirSep[0] )
    {
        r = r.substr( 0, r.length() - 1 );
    }

    return r;
}

WINUX_FUNC_IMPL(String) RealPath( String const & path )
{
    if ( path.empty() )
    {
        return GetCurrentDir();
    }
    else
    {
        // 判断是绝对路径还是相对路径
        if ( path[0] == '/' || path[0] == '\\' ) // 是绝对路径，但windows上要确定逻辑盘符
        {
        #if defined(OS_WIN)
            return NormalizePath( GetCurrentDir().substr( 0, 2 ) + path );
        #else
            return NormalizePath(path);
        #endif
        }
        else if ( path.length() > 1 && path[1] == ':' ) // 是绝对路径
        {
            return NormalizePath(path);
        }
        else // 是相对路径，依据当前路径计算
        {
            return NormalizePath( GetCurrentDir() + DirSep + path );
        }
    }
}

WINUX_FUNC_IMPL(String) RealPathEx( String const & path, String const & workDirAbsPath )
{
    if ( workDirAbsPath.empty() )
    {
        return RealPath(path);
    }

    if ( path.empty() )
    {
        return workDirAbsPath;
    }
    else
    {
        // 判断是绝对路径还是相对路径
        if ( path[0] == '/' || path[0] == '\\' ) // 是绝对路径，但windows上要确定逻辑盘符
        {
        #if defined(OS_WIN)
            if ( workDirAbsPath.length() > 1 && workDirAbsPath[1] == ':' ) // 工作目录绝对路径是以 X: 开头
            {
                return NormalizePath( workDirAbsPath.substr( 0, 2 ) + path );
            }
            else
            {
                return NormalizePath(path);
            }
        #else
            return NormalizePath(path);
        #endif
        }
        else if ( path.length() > 1 && path[1] == ':' ) // 是绝对路径
        {
            return NormalizePath(path);
        }
        else // 是相对路径，依据工作目录路径计算
        {
            return NormalizePath( workDirAbsPath + DirSep + path );
        }
    }
}

WINUX_FUNC_IMPL(String) GetCurrentDir( void )
{
    String::value_type const * p;
    String buf;
    int size = 130;
    do
    {
        size <<= 1;
        buf.resize(size);
        p = _tgetcwd( &buf[0], size );
    }
    while ( !p && errno == ERANGE );
    return p ? p : Literal<String::value_type>::nulStr;
}

WINUX_FUNC_IMPL(bool) SetCurrentDir( String const & path )
{
    return !_tchdir( path.c_str() );
}

WINUX_FUNC_IMPL(bool) IsDir( String const & path )
{
    struct _stat st = { 0 };
    int r;
#if defined(OS_WIN)
    r = _tstat( path.c_str(), &st );
    return r == 0 && ( st.st_mode & S_IFDIR );
#else
    r = _stat( path.c_str(), &st );
    return r == 0 && S_ISDIR(st.st_mode);
#endif
}

WINUX_FUNC_IMPL(bool) DetectPath( String const & path, bool * isDir )
{
    ASSIGN_PTR(isDir) = false;
    struct _stat st = { 0 };
    int r;
#if defined(OS_WIN)
    r = _tstat( path.c_str(), &st );
#else
    r = _stat( path.c_str(), &st );
#endif
    if ( r == 0 )
        ASSIGN_PTR(isDir) = ( st.st_mode & S_IFDIR ) != 0;
    return !( r == -1 && errno == ENOENT );
}

WINUX_FUNC_IMPL(uint64) FileSize( String const & filename )
{
#if defined(CL_MINGW)
    struct __stat64 st = { 0 };
#else
    struct _stat64 st = { 0 };
#endif
#if defined(OS_WIN)
    _tstat64( filename.c_str(), &st );
#else
    _stat64( filename.c_str(), &st );
#endif
    return st.st_size;
}

WINUX_FUNC_IMPL(bool) FileTime( String const & filename, time_t * ctime, time_t * mtime, time_t * atime )
{
    struct _stat64 st = { 0 };
    bool r;
#if defined(OS_WIN)
    r = 0 == _tstat64( filename.c_str(), &st );
#else
    r = 0 == _stat64( filename.c_str(), &st );
#endif
    ASSIGN_PTR(ctime) = st.st_ctime;
    ASSIGN_PTR(mtime) = st.st_mtime;
    ASSIGN_PTR(atime) = st.st_atime;
    return r;
}

WINUX_FUNC_IMPL(time_t) FileCTime( String const & filename )
{
    struct _stat64 st = { 0 };
#if defined(OS_WIN)
    _tstat64( filename.c_str(), &st );
#else
    _stat64( filename.c_str(), &st );
#endif
    return st.st_ctime;
}

WINUX_FUNC_IMPL(time_t) FileMTime( String const & filename )
{
    struct _stat64 st = { 0 };
#if defined(OS_WIN)
    _tstat64( filename.c_str(), &st );
#else
    _stat64( filename.c_str(), &st );
#endif
    return st.st_mtime;
}

WINUX_FUNC_IMPL(time_t) FileATime( String const & filename )
{
    struct _stat64 st = { 0 };
#if defined(OS_WIN)
    _tstat64( filename.c_str(), &st );
#else
    _stat64( filename.c_str(), &st );
#endif
    return st.st_atime;
}

WINUX_FUNC_IMPL(bool) FileTouch( String const & filename, time_t time, time_t atime )
{
    struct _utimbuf tbuf;
    time = ( time == (time_t)-1 ? GetUtcTime() : time );
    atime = ( atime == (time_t)-1 ? time : atime );
    tbuf.modtime = time;
    tbuf.actime = atime;

#if defined(OS_WIN)
    return 0 == _tutime( filename.c_str(), &tbuf );
#else
    return 0 == utime( filename.c_str(), &tbuf );
#endif
}

WINUX_FUNC_IMPL(String) PathWithSep( String const & path )
{
    String r;
    if ( !path.empty() )
    {
        auto ch = path[path.length() - 1];
        if ( ch != '\\' && ch != '/' ) // 末尾不是分隔符
        {
            r = path + DirSep;
        }
        else // 末尾是分隔符
        {
            r = path;
            r[r.length() - 1] = DirSep[0];
        }
    }
    return r;
}

WINUX_FUNC_IMPL(String) PathNoSep( String const & path )
{
    String r;
    if ( !path.empty() )
    {
        auto ch = path[path.length() - 1];
        if ( ch != '\\' && ch != '/' ) // 末尾不是分隔符
        {
            r = path;
        }
        else // 末尾是分隔符
        {
            r = path.substr( 0, path.length() - 1 );
        }
    }
    return r;
}

WINUX_FUNC_IMPL(String &) PathWithSep( String * path )
{
    if ( !path->empty() )
    {
        auto ch = (*path)[path->length() - 1];
        if ( ch != '\\' && ch != '/' ) // 末尾不是分隔符
        {
            *path += DirSep;
        }
        else // 末尾是分隔符
        {
            (*path)[path->length() - 1] = DirSep[0];
        }
    }
    return *path;
}

WINUX_FUNC_IMPL(String &) PathNoSep( String * path )
{
    if ( !path->empty() )
    {
        auto ch = (*path)[path->length() - 1];
        if ( ch == '\\' || ch == '/' ) // 末尾是分隔符
        {
            *path = path->substr( 0, path->length() - 1 );
        }
    }
    return *path;
}

WINUX_FUNC_IMPL(String) CombinePath( String const & dirPath, String const & fileName )
{
    return dirPath.empty() ? fileName : PathWithSep(dirPath) + fileName;
}

WINUX_FUNC_IMPL(void) FolderData( String const & path, StringArray * fileArr, StringArray * subFolderArr, int sortType )
{
    DirIterator iter(path);
    IF_PTR(fileArr)->clear();
    IF_PTR(subFolderArr)->clear();
    while ( iter.next() )
    {
        String const & name = iter.getName();
        if ( name == TEXT(".") || name == TEXT("..") ) continue;

        if ( iter.isDir() )
        {
            IF_PTR(subFolderArr)->push_back(name);
        }
        else
        {
            IF_PTR(fileArr)->push_back(name);
        }
    }
    switch ( sortType )
    {
    case 1:
        if ( fileArr ) std::sort( fileArr->begin(), fileArr->end(), std::less<String>() );
        if ( subFolderArr ) std::sort( subFolderArr->begin(), subFolderArr->end(), std::less<String>() );
        break;
    case 2:
        if ( fileArr ) std::sort( fileArr->begin(), fileArr->end(), std::greater<String>() );
        if ( subFolderArr ) std::sort( subFolderArr->begin(), subFolderArr->end(), std::greater<String>() );
        break;
    }
}

WINUX_FUNC_IMPL(size_t) EnumFiles( String const & path, Mixed const & ext, StringArray * arrFiles, bool isRecursive )
{
    size_t filesCount = 0;
    StringArray files, dirs, exts;
    StringArray::const_iterator it;
    FolderData( path, &files, &dirs );

    if ( ext.isArray() )
    {
        ext.getArray(&exts);
    }
    else if ( !ext.isNull() )
    {
        exts.push_back(ext);
    }

    for ( it = files.begin(); it != files.end(); ++it )
    {
        String extName;
        FileTitle( *it, &extName );
        if ( exts.empty() || ValueIsInArray( exts, extName, true ) )
        {
            IF_PTR(arrFiles)->push_back( isRecursive ? CombinePath( path, *it ) : *it );
            ++filesCount;
        }
    }

    if ( isRecursive )
    {
        for ( it = dirs.begin(); it != dirs.end(); ++it )
        {
            filesCount += EnumFiles( CombinePath( path, *it ), ext, arrFiles, isRecursive );
        }
    }
    return filesCount;
}

WINUX_FUNC_IMPL(bool) UnlinkFile( String const & filePath )
{
#if defined(OS_WIN)
    return _tunlink( filePath.c_str() ) == 0;
#else
    return unlink( filePath.c_str() ) == 0;
#endif
}

WINUX_FUNC_IMPL(bool) RemoveDir( String const & dirPath )
{
#if defined(OS_WIN)
    return _trmdir( dirPath.c_str() ) == 0;
#else
    return rmdir( dirPath.c_str() ) == 0;
#endif
}

WINUX_FUNC_IMPL(bool) RemovePath( String const & path )
{
#if defined(OS_WIN)
    return _tremove( path.c_str() ) == 0;
#else
    return remove( path.c_str() ) == 0;
#endif
}

WINUX_FUNC_IMPL(bool) RenamePath( String const & oldPath, String const & newPath )
{
#if defined(OS_WIN)
    return _trename( oldPath.c_str(), newPath.c_str() ) == 0;
#else
    return rename( oldPath.c_str(), newPath.c_str() ) == 0;
#endif
}

WINUX_FUNC_IMPL(size_t) CommonDelete( String const & path )
{
    size_t deletedCount = 0;
    if ( IsDir(path) )
    {
        StringArray files, dirs;
        FolderData( path, &files, &dirs );
        StringArray::const_iterator it;
        for ( it = files.begin(); it != files.end(); ++it )
        {
            if ( UnlinkFile( CombinePath( path, *it ) ) ) // delete file success
                deletedCount++;
        }

        for ( it = dirs.begin(); it != dirs.end(); ++it )
        {
            deletedCount += CommonDelete( CombinePath( path, *it ) );
        }

        if ( RemoveDir(path) ) // delete dir success
            deletedCount++;
    }
    else
    {
        if ( UnlinkFile(path) )
            deletedCount++;
    }
    return deletedCount;
}

WINUX_FUNC_IMPL(bool) MakeDirExists( String const & path, int mode )
{
    StringArray subPaths;
    size_t const n = StrSplit( path, TEXT("/\\"), &subPaths );
    size_t i;
    String existsPath;
    for ( i = 0; i < n; ++i )
    {
        String subPath = subPaths[i];
        if ( i == 0 && subPath.empty() ) // 首项为空，表明为linux平台绝对路径
        {
            existsPath += DirSep;
        }
        else if ( i == 0 && subPath.length() > 1 && subPath[1] == ':' ) // 首项长度大于1，并且第二个为':'，表明为windows平台绝对路径
        {
            existsPath += subPath + DirSep;
        }
        else if ( !subPath.empty() ) // 子项不为空
        {
            existsPath += subPath;
            if ( !DetectPath(existsPath) ) // 不存在则创建
            {
            #if defined(OS_WIN)
                if ( _tmkdir( existsPath.c_str() ) ) return false;
            #else
                if ( _mkdir( existsPath.c_str(), mode ) ) return false;
            #endif
            }
            if ( i != n - 1 )
                existsPath += DirSep;
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
WINUX_FUNC_IMPL(FileEncoding) RecognizeFileEncoding( winux::Buffer const & content, size_t * pIndex, size_t limitSize )
{
    FileEncoding fileEncoding = feMultiByte;
    size_t & i = *pIndex;
    size_t k;
    limitSize = limitSize < content.getSize() ? limitSize : content.getSize();
    if ( i < limitSize )
    {
        if ( content[i] == 0xEF )
        {
            i++;
            if ( i < limitSize )
            {
                if ( content[i] == 0xBB )
                {
                    i++;
                    if ( i < limitSize )
                    {
                        if ( content[i] == 0xBF )
                        {
                            i++;
                            fileEncoding = feUtf8Bom;
                        }
                    }
                }
            }
        }
        else if ( content[i] == 0xFF )
        {
            i++;
            if ( i < limitSize )
            {
                if ( content[i] == 0xFE )
                {
                    i++;
                    fileEncoding = feUtf16Le;
                    if ( i < limitSize )
                    {
                        if ( content[i] == 0x00 )
                        {
                            i++;
                            if ( i < limitSize )
                            {
                                if ( content[i] == 0x00 )
                                {
                                    i++;
                                    fileEncoding = feUtf32Le;
                                }
                            }
                        }
                    }
                }
            }
        }
        else if ( content[i] == 0xFE )
        {
            i++;
            if ( i < limitSize )
            {
                if ( content[i] == 0xFF )
                {
                    i++;
                    fileEncoding = feUtf16Be;
                }
            }
        }
        else if ( content[i] == 0x00 )
        {
            i++;
            if ( i < limitSize )
            {
                if ( content[i] == 0x00 )
                {
                    i++;
                    if ( i < limitSize )
                    {
                        if ( content[i] == 0xFE )
                        {
                            i++;
                            if ( i < limitSize )
                            {
                                if ( content[i] == 0xFF )
                                {
                                    i++;
                                    fileEncoding = feUtf32Be;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if ( fileEncoding == feMultiByte )
    {
        int bytesOfHighestBit1 = 0; // 最高位是1的字节数
        for ( k = 0; k < limitSize; k++ )
        {
            if ( ( content[k] & 0x80 ) == 0x80 )
            {
                bytesOfHighestBit1++;

                if ( bytesOfHighestBit1 == 1 )
                {
                    if ( ( content[k] & 0xF0 ) == 0xF0 )
                    {
                        k++;
                        if ( k < limitSize )
                        {
                            if ( ( content[k] & 0xC0 ) == 0x80 )
                            {
                                k++;
                                if ( k < limitSize )
                                {
                                    if ( ( content[k] & 0xC0 ) == 0x80 )
                                    {
                                        k++;
                                        if ( k < limitSize )
                                        {
                                            if ( ( content[k] & 0xC0 ) == 0x80 )
                                            {
                                                fileEncoding = feUtf8;
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if ( ( content[k] & 0xE0 ) == 0xE0 )
                    {
                        k++;
                        if ( k < limitSize )
                        {
                            if ( ( content[k] & 0xC0 ) == 0x80 )
                            {
                                k++;
                                if ( k < limitSize )
                                {
                                    if ( ( content[k] & 0xC0 ) == 0x80 )
                                    {
                                        fileEncoding = feUtf8;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    else if ( ( content[k] & 0xC0 ) == 0xC0 )
                    {
                        k++;
                        if ( k < limitSize )
                        {
                            if ( ( content[k] & 0xC0 ) == 0x80 )
                            {
                                fileEncoding = feUtf8;
                                break;
                            }
                        }
                    }
                }
                else if ( bytesOfHighestBit1 == 2 )
                {
                    bytesOfHighestBit1 = 0;
                }
            }
            else
            {
                bytesOfHighestBit1 = 0;
            }
        }
    }
    return fileEncoding;
}

// 调整字节序。b表示是否反序，配合IsLittleEndian()/IsBigEndian()使用
template < typename _Ty >
inline static _Ty _AdjustByteOrder( _Ty v, bool b )
{
    if ( b )
    {
        return InvertByteOrder(v);
    }
    else
    {
        return v;
    }
}

template < typename _ChTy >
inline static winux::XString<_ChTy> Impl_NewlineFromFile( _ChTy const * content, size_t len, bool b )
{
#if defined(OS_WIN)
    winux::XString<_ChTy> r2;
    for ( size_t i = 0; i < len; i++ )
    {
        if ( content[i] == _AdjustByteOrder( winux::Literal<_ChTy>::crChar, b ) )
        {
            i++;
            if ( i < len )
            {
                if ( content[i] == _AdjustByteOrder( winux::Literal<_ChTy>::lfChar, b ) )
                {
                    r2 += _AdjustByteOrder( winux::Literal<_ChTy>::lfChar, b );
                }
                else
                {
                    r2 += _AdjustByteOrder( winux::Literal<_ChTy>::crChar, b );
                    r2 += content[i];
                }
            }
            else
            {
                r2 += _AdjustByteOrder( winux::Literal<_ChTy>::crChar, b );
            }
        }
        else
        {
            r2 += content[i];
        }
    }
    return r2;
#elif defined(OS_DARWIN)
    winux::XString<_ChTy> r2;
    for ( size_t i = 0; i < len; i++ )
    {
        if ( content[i] == _AdjustByteOrder( winux::Literal<_ChTy>::crChar, b ) )
        {
            r2 += _AdjustByteOrder( winux::Literal<_ChTy>::lfChar, b );
        }
        else
        {
            r2 += content[i];
        }
    }
    return r2;
#else
    return XString<_ChTy>( content, len );
#endif
}

template < typename _ChTy >
inline static winux::XString<_ChTy> Impl_NewlineToFile( _ChTy const * content, size_t len, bool b )
{
#if defined(OS_WIN)
    winux::XString<_ChTy> r2;
    for ( size_t i = 0; i < len; i++ )
    {
        if ( content[i] == _AdjustByteOrder( winux::Literal<_ChTy>::lfChar, b ) )
        {
            r2 += _AdjustByteOrder( winux::Literal<_ChTy>::crChar, b );
            r2 += _AdjustByteOrder( winux::Literal<_ChTy>::lfChar, b );
        }
        else
        {
            r2 += content[i];
        }
    }
    return r2;
#elif defined(OS_DARWIN)
    winux::XString<_ChTy> r2;
    for ( size_t i = 0; i < len; i++ )
    {
        if ( content[i] == _AdjustByteOrder( winux::Literal<_ChTy>::lfChar, b ) )
        {
            r2 += _AdjustByteOrder( winux::Literal<_ChTy>::crChar, b );
        }
        else
        {
            r2 += content[i];
        }
    }
    return r2;
#else
    return XString<_ChTy>( content, len );
#endif
}

template <>
WINUX_FUNC_IMPL(XString<char>) NewlineFromFile( char const * content, size_t len, bool b )
{
    return Impl_NewlineFromFile( content, len, b );
}
template <>
WINUX_FUNC_IMPL(XString<wchar>) NewlineFromFile( wchar const * content, size_t len, bool b )
{
    return Impl_NewlineFromFile( content, len, b );
}
template <>
WINUX_FUNC_IMPL(XString<char16>) NewlineFromFile( char16 const * content, size_t len, bool b )
{
    return Impl_NewlineFromFile( content, len, b );
}
template <>
WINUX_FUNC_IMPL(XString<char32>) NewlineFromFile( char32 const * content, size_t len, bool b )
{
    return Impl_NewlineFromFile( content, len, b );
}

template <>
WINUX_FUNC_IMPL(XString<char>) NewlineToFile( char const * content, size_t len, bool b )
{
    return Impl_NewlineToFile( content, len, b );
}
template <>
WINUX_FUNC_IMPL(XString<wchar>) NewlineToFile( wchar const * content, size_t len, bool b )
{
    return Impl_NewlineToFile( content, len, b );
}
template <>
WINUX_FUNC_IMPL(XString<char16>) NewlineToFile( char16 const * content, size_t len, bool b )
{
    return Impl_NewlineToFile( content, len, b );
}
template <>
WINUX_FUNC_IMPL(XString<char32>) NewlineToFile( char32 const * content, size_t len, bool b )
{
    return Impl_NewlineToFile( content, len, b );
}

///////////////////////////////////////////////////////////////////////////////////////////////
static String _ContentGetString( Buffer const & content, FileEncoding encoding, bool convertNewline )
{
    size_t i = 0;
    // 编码未指定则自动识别
    if ( encoding == feUnspec ) encoding = RecognizeFileEncoding( content, &i, 1024 );

    switch ( encoding )
    {
    case winux::feMultiByte:
        {
            AnsiString str = convertNewline ? NewlineFromFile( content.get<char>() + i, content.size() - i, false ) : AnsiString( content.get<char>() + i, content.size() - i );
        #if defined(_UNICODE) || defined(UNICODE)
            return LocalToUnicode(str);
        #else
            return str;
        #endif
        }
        break;
    case winux::feUtf8:
        {
            AnsiString str = convertNewline ? NewlineFromFile( content.get<char>() + i, content.size() - i, false ) : AnsiString( content.get<char>() + i, content.size() - i );
        #if defined(_UNICODE) || defined(UNICODE)
            return UnicodeConverter(str).toUnicode();
        #else
            return LocalFromUtf8(str);
        #endif
        }
        break;
    case winux::feUtf8Bom:
        {
            if ( i == 0 ) i = 3; // skip BOM
            AnsiString str = convertNewline ? NewlineFromFile( content.get<char>() + i, content.size() - i, false ) : AnsiString( content.get<char>() + i, content.size() - i );
        #if defined(_UNICODE) || defined(UNICODE)
            return UnicodeConverter(str).toUnicode();
        #else
            return LocalFromUtf8(str);
        #endif
        }
        break;
    case winux::feUtf16Le:
        {
            if ( i == 0 ) i = 2; // skip BOM
            Utf16String str = convertNewline ?
                NewlineFromFile( (Utf16String::value_type *)( content.get<winux::byte>() + i ), ( content.size() - i ) / sizeof(Utf16String::value_type), IsBigEndian() ) :
                Utf16String( (Utf16String::value_type *)( content.get<winux::byte>() + i ), ( content.size() - i ) / sizeof(Utf16String::value_type) )
            ;
            if ( IsBigEndian() )
            {
                if ( str.length() > 0 ) InvertByteOrderArray( &str[0], str.length() );
            }
        #if defined(_UNICODE) || defined(UNICODE)
            return UnicodeConverter(str).toUnicode();
        #else
            return UnicodeToLocal( UnicodeConverter(str).toUnicode() );
        #endif
        }
        break;
    case winux::feUtf16Be:
        {
            if ( i == 0 ) i = 2; // skip BOM
            Utf16String str = convertNewline ?
                NewlineFromFile( (Utf16String::value_type *)( content.get<winux::byte>() + i ), ( content.size() - i ) / sizeof(Utf16String::value_type), IsLittleEndian() ) :
                Utf16String( (Utf16String::value_type *)( content.get<winux::byte>() + i ), ( content.size() - i ) / sizeof(Utf16String::value_type) )
            ;
            if ( IsLittleEndian() )
            {
                if ( str.length() > 0 ) InvertByteOrderArray( &str[0], str.length() );
            }
        #if defined(_UNICODE) || defined(UNICODE)
            return UnicodeConverter(str).toUnicode();
        #else
            return UnicodeToLocal( UnicodeConverter(str).toUnicode() );
        #endif
        }
        break;
    case winux::feUtf32Le:
        {
            if ( i == 0 ) i = 4; // skip BOM
            Utf32String str = convertNewline ?
                NewlineFromFile( (Utf32String::value_type *)( content.get<winux::byte>() + i ), ( content.size() - i ) / sizeof(Utf32String::value_type), IsBigEndian() ) :
                Utf32String( (Utf32String::value_type *)( content.get<winux::byte>() + i ), ( content.size() - i ) / sizeof(Utf32String::value_type) )
            ;
            if ( IsBigEndian() )
            {
                if ( str.length() > 0 ) InvertByteOrderArray( &str[0], str.length() );
            }
        #if defined(_UNICODE) || defined(UNICODE)
            return UnicodeConverter(str).toUnicode();
        #else
            return UnicodeToLocal( UnicodeConverter(str).toUnicode() );
        #endif
        }
        break;
    case winux::feUtf32Be:
        {
            if ( i == 0 ) i = 4; // skip BOM
            Utf32String str = convertNewline ?
                NewlineFromFile( (Utf32String::value_type *)( content.get<winux::byte>() + i ), ( content.size() - i ) / sizeof(Utf32String::value_type), IsLittleEndian() ) :
                Utf32String( (Utf32String::value_type *)( content.get<winux::byte>() + i ), ( content.size() - i ) / sizeof(Utf32String::value_type) )
            ;
            if ( IsLittleEndian() )
            {
                if ( str.length() > 0 ) InvertByteOrderArray( &str[0], str.length() );
            }
        #if defined(_UNICODE) || defined(UNICODE)
            return UnicodeConverter(str).toUnicode();
        #else
            return UnicodeToLocal( UnicodeConverter(str).toUnicode() );
        #endif
        }
        break;
    }
    return String();
}

WINUX_FUNC_IMPL(String) FileGetString( String const & filename, FileEncoding encoding )
{
    Buffer content = FileGetContentsEx( filename, false );
    return _ContentGetString( content, encoding, true );
}

WINUX_FUNC_IMPL(AnsiString) FileGetContents( String const & filename, bool textMode )
{
    AnsiString content;
    try
    {
        SimpleHandle<int> fd(
        #if defined(OS_WIN)
            _topen(
        #else
            open(
        #endif
                filename.c_str(),
                O_RDONLY | ( textMode ? O_TEXT : O_BINARY )
            ),
            -1,
            close
        );
        if ( fd )
        {
            int readBytes = 0, currRead = 0;
            char buf[4096];
            do
            {
                if ( ( currRead = read( fd.get(), buf, 4096 ) ) < 1 ) break;
                content.append( buf, currRead );
                readBytes += currRead;
            } while ( currRead > 0 );
        }
    }
    catch ( std::exception const & )
    {
    }
    return content;
}

WINUX_FUNC_IMPL(Buffer) FileGetContentsEx( String const & filename, bool textMode )
{
    GrowBuffer content;
    try
    {
        SimpleHandle<int> fd(
        #if defined(OS_WIN)
            _topen(
        #else
            open(
        #endif
                filename.c_str(),
                O_RDONLY | ( textMode ? O_TEXT : O_BINARY )
            ),
            -1,
            close
        );
        if ( fd )
        {
            int readBytes = 0, currRead = 0;
            char buf[4096];
            do
            {
                if ( ( currRead = read( fd.get(), buf, 4096 ) ) < 1 ) break;
                content.append( buf, currRead );
                readBytes += currRead;
            } while ( currRead > 0 );
        }
    }
    catch ( std::exception const & )
    {
    }
    return content;
}

static void _ContentPutString( GrowBuffer * output, String const & content, FileEncoding encoding, bool convertNewline )
{
    switch ( encoding )
    {
    case winux::feUnspec:
    case winux::feMultiByte:
        {
        #if defined(_UNICODE) || defined(UNICODE)
            AnsiString str = UnicodeToLocal(content);
            output->appendString( convertNewline ? NewlineToFile( str.c_str(), str.length(), false ) : str );
        #else
            output->appendString( convertNewline ? NewlineToFile( content.c_str(), content.length(), false ) : content );
        #endif
        }
        break;
    case winux::feUtf8:
        {
        #if defined(_UNICODE) || defined(UNICODE)
            AnsiString str = UnicodeConverter(content).toUtf8();
            output->appendString( convertNewline ? NewlineToFile( str.c_str(), str.length(), false ) : str );
        #else
            AnsiString str = LocalToUtf8(content);
            output->appendString( convertNewline ? NewlineToFile( str.c_str(), str.length(), false ) : str );
        #endif
        }
        break;
    case winux::feUtf8Bom:
        {
        #if defined(_UNICODE) || defined(UNICODE)
            AnsiString str = UnicodeConverter(content).toUtf8();
            output->appendType( { '\xef', '\xbb', '\xbf' } );
            output->appendString( convertNewline ? NewlineToFile( str.c_str(), str.length(), false ) : str );
        #else
            AnsiString str = LocalToUtf8(content);
            output->appendType( { '\xef', '\xbb', '\xbf' } );
            output->appendString( convertNewline ? NewlineToFile( str.c_str(), str.length(), false ) : str );
        #endif
        }
        break;
    case winux::feUtf16Le:
        {
        #if defined(_UNICODE) || defined(UNICODE)
            Utf16String str = UnicodeConverter(content).toUtf16();
        #else
            Utf16String str = UnicodeConverter( LocalToUnicode(content) ).toUtf16();
        #endif
            if ( IsBigEndian() )
            {
                if ( str.length() > 0 ) InvertByteOrderArray( &str[0], str.length() );
            }
            output->appendType( { '\xff', '\xfe' } );
            output->appendString( convertNewline ? NewlineToFile( str.c_str(), str.length(), IsBigEndian() ) : str );
        }
        break;
    case winux::feUtf16Be:
        {
        #if defined(_UNICODE) || defined(UNICODE)
            Utf16String str = UnicodeConverter(content).toUtf16();
        #else
            Utf16String str = UnicodeConverter( LocalToUnicode(content) ).toUtf16();
        #endif
            if ( IsLittleEndian() )
            {
                if ( str.length() > 0 ) InvertByteOrderArray( &str[0], str.length() );
            }
            output->appendType( { '\xfe', '\xff' } );
            output->appendString( convertNewline ? NewlineToFile( str.c_str(), str.length(), IsLittleEndian() ) : str );
        }
        break;
    case winux::feUtf32Le:
        {
        #if defined(_UNICODE) || defined(UNICODE)
            Utf32String str = UnicodeConverter(content).toUtf32();
        #else
            Utf32String str = UnicodeConverter( LocalToUnicode(content) ).toUtf32();
        #endif
            if ( IsBigEndian() )
            {
                if ( str.length() > 0 ) InvertByteOrderArray( &str[0], str.length() );
            }
            output->appendType( { '\xff', '\xfe', '\0', '\0' } );
            output->appendString( convertNewline ? NewlineToFile( str.c_str(), str.length(), IsBigEndian() ) : str );
        }
        break;
    case winux::feUtf32Be:
        {
        #if defined(_UNICODE) || defined(UNICODE)
            Utf32String str = UnicodeConverter(content).toUtf32();
        #else
            Utf32String str = UnicodeConverter( LocalToUnicode(content) ).toUtf32();
        #endif
            if ( IsLittleEndian() )
            {
                if ( str.length() > 0 ) InvertByteOrderArray( &str[0], str.length() );
            }
            output->appendType( { '\0', '\0', '\xfe', '\xff' } );
            output->appendString( convertNewline ? NewlineToFile( str.c_str(), str.length(), IsLittleEndian() ) : str );
        }
        break;
    }
}

WINUX_FUNC_IMPL(bool) FilePutString( String const & filename, String const & content, FileEncoding encoding )
{
    GrowBuffer output;
    _ContentPutString( &output, content, encoding, true );
    return FilePutContentsEx( filename, output, false );
}

WINUX_FUNC_IMPL(bool) FilePutContents( String const & filename, AnsiString const & content, bool textMode )
{
    bool r = false;
    try
    {
        SimpleHandle<int> fd(
        #if defined(OS_WIN)
            _topen(
        #else
            open(
        #endif
                filename.c_str(),
                O_CREAT | O_TRUNC | O_WRONLY | ( textMode ? O_TEXT : O_BINARY ),
            #if defined(_MSC_VER) || defined(WIN32)
                S_IREAD | S_IWRITE
            #else
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
            #endif
            ),
            -1,
            close
        );
        if ( fd )
        {
            int writtenBytes = write( fd.get(), content.c_str(), (uint)content.size() );
            if ( writtenBytes == (int)content.size() )
                r = true;
        }
        /*else
        {
            switch ( errno )
            {
            case EACCES:
                printf("Tried to open read-only file for writing, file's sharing mode does not allow specified operations, or given path is directory.");
                break;
            case EEXIST:
                printf("_O_CREAT and _O_EXCL flags specified, but filename already exists.");
                break;
            }
        }*/
    }
    catch ( std::exception const & )
    {
    }
    return r;
}

WINUX_FUNC_IMPL(bool) FilePutContentsEx( String const & filename, Buffer const & content, bool textMode )
{
    bool r = false;
    try
    {
        SimpleHandle<int> fd(
        #if defined(OS_WIN)
            _topen(
        #else
            open(
        #endif
                filename.c_str(),
                O_CREAT | O_TRUNC | O_WRONLY | ( textMode ? O_TEXT : O_BINARY ),
            #if defined(_MSC_VER) || defined(WIN32)
                S_IREAD | S_IWRITE
            #else
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
            #endif
            ),
            -1,
            close
        );
        if ( fd )
        {
            int writtenBytes = write( fd.get(), content.getBuf(), (uint)content.getSize() );
            if ( writtenBytes == (int)content.getSize() )
                r = true;
        }
        /*else
        {
            switch ( errno )
            {
            case EACCES:
                printf("Tried to open read-only file for writing, file's sharing mode does not allow specified operations, or given path is directory.");
                break;
            case EEXIST:
                printf("_O_CREAT and _O_EXCL flags specified, but filename already exists.");
                break;
            }
        }*/
    }
    catch ( std::exception const & )
    {
    }
    return r;
}

WINUX_FUNC_IMPL(String) BackupFile( String const & filePath, String const & bakDir, String const & fmt )
{
    String r;

    // 文件内容
    Buffer fileContent = FileGetContentsEx( filePath, false );
    // 文件sha1
    Buffer fileSha1 = Sha1(fileContent);

    String fileName, fileTitle, extName;
    String dirPath = FilePath( filePath, &fileName );
    fileTitle = FileTitle( fileName, &extName );

    // 计算备份目录路径并使之存在
    String bakDirPath = IsAbsPath(bakDir) ? bakDir : CombinePath( dirPath, bakDir );
    if ( !DetectPath(bakDirPath) ) MakeDirExists(bakDirPath);

    // 计算备份文件路径
    for ( size_t v = 0; ; v++ ) // 文件版本编号
    {
        String bakFileName;
        size_t i = 0;
        while ( i < fmt.length() )
        {
            winux::String::value_type ch = fmt[i];
            if ( ch == '%' )
            {
                if ( i + 1 < fmt.length() )
                {
                    i++; // skip '%'
                    switch ( fmt[i] )
                    {
                    case 'f':
                        bakFileName += fileTitle;
                        i++;
                        break;
                    case 'e':
                        bakFileName += extName;
                        i++;
                        break;
                    case 'E':
                        bakFileName += extName.empty() ? TEXT("") : TEXT(".") + extName;
                        i++;
                        break;
                    case 'v':
                        bakFileName += Format( TEXT("%u"), v );
                        i++;
                        break;
                    case '%':
                        bakFileName += '%';
                        i++;
                        break;
                    }
                }
                else
                {
                    bakFileName += ch;
                    i++;
                }
            }
            else
            {
                bakFileName += ch;
                i++;
            }
        }

        String bakFilePath = CombinePath( bakDirPath, bakFileName );
        // 备份文件是否存在
        if ( DetectPath(bakFilePath) ) // 存在检查sha1
        {
            Buffer bakSha1 = Sha1( FileGetContentsEx( bakFilePath, false ) );
            if ( fileSha1 == bakSha1 )
            {
                r = bakFilePath;
                break;
            }
        }
        else // 如果不存在，直接备份
        {
            if ( FilePutContentsEx( bakFilePath, fileContent, false ) ) r = bakFilePath;
            break;
        }
    }

    return r;
}


// class DirIterator ---------------------------------------------------------------
#if defined(OS_WIN)
DirIterator::DirIterator( String const & path, String const & pattern )
: _path(path), _findFile( FindFirstFile( CombinePath( path, pattern ).c_str(), &_wfd ), INVALID_HANDLE_VALUE, FindClose ), _first(true)
{

}
#else
DirIterator::DirIterator( String const & path, String const & pattern )
: _path(path), _curr(-1)
{
    int rc;
    memset( &_findFile, 0, sizeof(_findFile) );
    #if defined(__USE_LARGEFILE64)
    rc = glob64( CombinePath( path, pattern ).c_str(), 0, nullptr, &_findFile );
    #else
    rc = glob( CombinePath( path, pattern ).c_str(), 0, nullptr, &_findFile );
    #endif
    if ( rc == 0 )
    {
        _curr = 0;
    }
}
#endif

DirIterator::~DirIterator()
{
#if defined(OS_WIN)

#else
    #if defined(__USE_LARGEFILE64)
    globfree64(&_findFile);
    #else
    globfree(&_findFile);
    #endif
#endif
}

bool DirIterator::next()
{
#if defined(OS_WIN)
    if ( _first )
    {
        _first = false;
        if ( _findFile )
        {
            _name = _wfd.cFileName;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if ( FindNextFile( _findFile.get(), &_wfd ) )
        {
            _name = _wfd.cFileName;
            return true;
        }
        else
        {
            return false;
        }
    }
#else
    if ( _curr < _findFile.gl_pathc )
    {
        _path = FilePath( _findFile.gl_pathv[_curr], &_name );
        _curr++;
        return true;
    }
    else
    {
        _curr = 0;
        return false;
    }
#endif
}

String DirIterator::getFullPath() const
{
    return CombinePath( _path, _name );
}

String DirIterator::getRealPath() const
{
    return RealPath( this->getFullPath() );
}

bool DirIterator::isDir() const
{
    return IsDir( this->getFullPath() );
}

// interface IFile ------------------------------------------------------------------
bool IFile::open( String const & filename, String const & mode )
{
    throw FileSysError( FileSysError::fseNotImplemented, "This method is not implemented" );
}

bool IFile::close()
{
    throw FileSysError( FileSysError::fseNotImplemented, "This method is not implemented" );
}

size_t IFile::read( void * buf, size_t size )
{
    throw FileSysError( FileSysError::fseNotImplemented, "This method is not implemented" );
}

Buffer IFile::read( size_t size )
{
    Buffer buf;
    buf.alloc(size);
    this->read( buf.getBuf(), buf.getSize() );
    return buf;
}

size_t IFile::write( void const * data, size_t size )
{
    throw FileSysError( FileSysError::fseNotImplemented, "This method is not implemented" );
}

size_t IFile::write( Buffer const & buf )
{
    return this->write( buf.getBuf(), buf.getSize() );
}

bool IFile::rewind()
{
    throw FileSysError( FileSysError::fseNotImplemented, "This method is not implemented" );
}

bool IFile::seek( offset_t offset, SeekType origin )
{
    throw FileSysError( FileSysError::fseNotImplemented, "This method is not implemented" );
}

size_t IFile::tell()
{
    throw FileSysError( FileSysError::fseNotImplemented, "This method is not implemented" );
}

String IFile::getLine()
{
    throw FileSysError( FileSysError::fseNotImplemented, "This method is not implemented" );
}

int IFile::puts( String const & str )
{
#if defined(_UNICODE) || defined(_UNICODE)
    AnsiString data = StringToLocal(str);
    return (int)this->write( data.c_str(), data.length() );
#else
    return (int)this->write( str.c_str(), str.length() * sizeof(str[0]) );
#endif
}

bool IFile::eof()
{
    throw FileSysError( FileSysError::fseNotImplemented, "This method is not implemented" );
}

size_t IFile::size()
{
    throw FileSysError( FileSysError::fseNotImplemented, "This method is not implemented" );
}

void * IFile::entire( size_t * size )
{
    //throw FileSysError( FileSysError::fseNotImplemented, "This method is not implemented" );
    *size = 0;
    return nullptr;
}

String IFile::entire( FileEncoding encoding, bool convertNewline )
{
    size_t len;
    void * buf = this->entire(&len);
    return _ContentGetString( Buffer( buf, len, true ), encoding, convertNewline );
}

// class MemoryFile -----------------------------------------------------------------------
MemoryFile::MemoryFile() : _p(nullptr)
{
}

MemoryFile::MemoryFile( void const * data, size_t size, bool isPeek ) : _p(nullptr)
{
    _buf.setBuf( data, size, isPeek );
    _p = _buf.begin();
}

MemoryFile::MemoryFile( Buffer const & buf, bool isPeek ) : _p(nullptr)
{
    _buf.setBuf( buf.getBuf(), buf.getSize(), isPeek || buf.isPeek() );
    _p = _buf.begin();
}

MemoryFile::MemoryFile( AnsiString const & content, bool isPeek ) : _p(nullptr)
{
    _buf.setBuf( content.c_str(), content.size(), isPeek );
    _p = _buf.begin();
}

bool MemoryFile::open( String const & content, String const & mode )
{
    _buf.setBuf( content.c_str(), sizeof(String::value_type) * content.size(), mode.find('r') != String::npos );
    _p = _buf.begin();
    return true;
}

bool MemoryFile::close()
{
    _buf.free();
    _p = nullptr;
    return true;
}

size_t MemoryFile::read( void * buf, size_t size )
{
    size_t remainingSize = _buf.end() - _p;
    if ( size > remainingSize ) // 获取大小大于剩余大小，以剩余大小为准
    {
        memcpy( buf, _p, remainingSize );
        _p += remainingSize;
        return remainingSize;
    }
    else
    {
        memcpy( buf, _p, size );
        _p += size;
        return size;
    }
}

size_t MemoryFile::write( void const * data, size_t size )
{
    size_t remainingSize = _buf.end() - _p;
    if ( size > remainingSize ) // 欲写入大小大于剩余大小
    {
        _buf.append( nullptr, size - remainingSize );
        _p = _buf.end() - size;
        memcpy( _p, data, size );
        _p += size;
    }
    else
    {
        memcpy( _p, data, size );
        _p += size;
    }
    return size;
}

bool MemoryFile::rewind()
{
    _p = _buf.begin();
    return _p != nullptr;
}

bool MemoryFile::seek( offset_t offset, SeekType origin )
{
    if ( _p != nullptr )
    {
        switch ( origin )
        {
        case winux::seekSet:
            _p = _buf.begin() + offset;
            break;
        case winux::seekCur:
            _p = _p + offset;
            break;
        case winux::seekEnd:
            _p = _buf.end() + offset;
            break;
        }
        return _p < _buf.end();
    }
    return false;
}

size_t MemoryFile::tell()
{
    return _p - _buf.begin();
}

String MemoryFile::getLine()
{
    String line;
    size_t i = _p - _buf.begin();
    StrGetLine( &line, _buf.getBuf<String::value_type>(), _buf.getSize() / sizeof(String::value_type), &i );
    _p = _buf.begin() + i;
    return line;
}

int MemoryFile::puts( String const & str )
{
    return (int)this->write( str.c_str(), str.length() * sizeof(str[0]) );
}

bool MemoryFile::eof()
{
    return _p == nullptr || _p >= _buf.end();
}

size_t MemoryFile::size()
{
    return _buf.getSize();
}

void * MemoryFile::entire( size_t * size )
{
    ASSIGN_PTR(size) = _buf.getSize();
    return _buf.getBuf();
}

// class File ---------------------------------------------------------------------------------
File::File() : _fp(nullptr)
{
}

File::File( String const & filename, String const & mode ) : _fp(nullptr)
{
    this->open( filename, mode );
}

File::~File()
{
    this->close();
}

bool File::open( String const & filename, String const & mode )
{
    this->close();
    if ( !filename.empty() )
    {
    #if defined(OS_WIN)
        _fp = _tfopen( filename.c_str(), mode.c_str() );
    #else
        _fp = fopen( filename.c_str(), mode.c_str() );
    #endif
        return _fp != nullptr;
    }
    return false;
}

bool File::close()
{
    if ( _fp != nullptr )
    {
        fclose(_fp);
        _fp = nullptr;
        return true;
    }
    return false;
}

size_t File::read( void * buf, size_t size )
{
    if ( _fp == nullptr ) return 0;
    return fread( buf, 1, size, _fp );
}

size_t File::write( void const * data, size_t size )
{
    if ( _fp == nullptr ) return 0;
    return fwrite( data, 1, size, _fp );
}

bool File::rewind()
{
    if ( _fp == nullptr ) return false;
    ::rewind(_fp);
    return true;
}

bool File::seek( offset_t offset, SeekType origin )
{
    if ( _fp == nullptr ) return false;
    return !fseek( _fp, (long)offset, (int)origin );
}

size_t File::tell()
{
    if ( _fp == nullptr ) return 0;
    return ftell(_fp);
}

winux::String File::getLine()
{
    String line;
    if ( _fp == nullptr ) return line;
    size_t const NBUF = 512;
    String::value_type sz[NBUF];
    bool hasLineSep = false;
    do
    {
        memset( sz, 0, NBUF * sizeof(String::value_type) );
        if ( _fgetts( sz, NBUF, _fp ) )
        {
            String::size_type len = _tcslen(sz); // 获得读取到的字符串长度
            hasLineSep = sz[len - 1] == Literal<String::value_type>::lfChar; // 判断是否读取到换行符
            if ( hasLineSep )
            {
                line.append( sz, len );
                return line;
            }
            else
            {
                line.append( sz, len );
            }
        }
        else
        {
            break;
        }
    } while ( !hasLineSep );
    return line;
}

bool File::eof()
{
    if ( _fp == nullptr ) return true;
    return feof(_fp) != 0;
}

size_t File::size()
{
    if ( _fp == nullptr ) return 0;
    struct _stat64 st;
    int rc = _fstat64( this->getFd(), &st );
    if ( rc < 0 ) st.st_size = 0;
    return (size_t)st.st_size;
}

void * File::entire( size_t * size )
{
    size_t fileSize = this->size();

    thread_local Buffer buf; // 用于加载文件数据的缓冲区

    buf.alloc(fileSize);
    *size = this->read( buf.getBuf(), buf.getSize() );
    this->rewind();

    return buf.getBuf();
}

int File::getFd() const
{
    return fileno(_fp);
}

#if defined(OS_WIN)
HANDLE File::getOsHandle() const
{
    return (HANDLE)_get_osfhandle( this->getFd() );
}
#else
int File::getOsHandle() const
{
    return this->getFd();
}
#endif

// class BlockOutFile ---------------------------------------------------------------------
BlockOutFile::BlockOutFile( String const & filename, bool isTextMode, size_t blockSize )
{
    _fileno = 1;
    _isTextMode = isTextMode;
    _blockSize = blockSize;
    _writtenSize = 0;
    // 分析文件路径
    _dirname = FilePath( filename, &_basename );
    _filetitle = FileTitle( _basename, &_extname );

    this->nextBlock(); // 第一块
}

bool BlockOutFile::nextBlock()
{
    this->close(); // 关闭先前的那块
    bool r = this->open(
        CombinePath( _dirname, _filetitle + TEXT("_") + (String)Mixed(_fileno) + TEXT(".") + _extname ),
        ( _isTextMode ? TEXT("w") : TEXT("wb") )
    );
    if ( r )
    {
        _fileno++;
    }
    return r;
}

size_t BlockOutFile::write( void const * data, size_t size )
{
    _writtenSize += size;
    if ( _writtenSize > _blockSize )
    {
        this->nextBlock();
        _writtenSize = size;
    }
    return File::write( data, size );
}

// class BlockInFile ----------------------------------------------------------------------

// 找并提取下划线数字部分
static bool __FindAndExtractDigit( String const & str, String * part1, String * partDigit )
{
    String::size_type pos;
    if ( ( pos = str.rfind('_') ) != String::npos )
    {
        *part1 = str.substr( 0, pos + 1 );
        ++pos; // skip '_'
        while ( pos < str.length() )
        {
            if ( IsDigit(str[pos]) )
            {
                *partDigit += str[pos];
            }
            else
            {
                break;
            }
            ++pos;
        }
        if ( !partDigit->empty() )
        {
            return true;
        }
    }
    return false;
}

BlockInFile::BlockInFile( String const & filename, bool isTextMode ) : _index(0), _isTextMode(isTextMode)
{
    // 分析文件路径
    _dirname = FilePath( filename, &_basename );
    String fileTitle = FileTitle( _basename, &_extname );
    // 处理文件标题
    // 如果有分块数字,则搜索其他分块,并判断存在性
    String strMaxFileNo;
    if ( __FindAndExtractDigit( fileTitle, &_filetitle, &strMaxFileNo ) )
    {
        int maxFileNo = (int)Mixed(strMaxFileNo);
        int i;
        for ( i = 1; i <= maxFileNo; ++i )
        {
            String curFileName = CombinePath( _dirname, _filetitle + (String)Mixed(i) + TEXT(".") + _extname );
            if ( DetectPath(curFileName) )
            {
                _blockFiles.push_back(curFileName);
            }
        }
        bool flag = true;
        for ( ; flag; ++i )
        {
            String curFileName = CombinePath( _dirname, _filetitle + (String)Mixed(i) + TEXT(".") + _extname );
            if ( ( flag = DetectPath(curFileName) ) )
            {
                _blockFiles.push_back(curFileName);
            }
        }
    }
    else // 如果无,则自动从1开始添加并判断存在性
    {
        int i = 1;
        bool flag = true;
        _filetitle = fileTitle;
        for ( ; flag; ++i )
        {
            String curFileName = CombinePath( _dirname, _filetitle + TEXT("_") + (String)Mixed(i) + TEXT(".") + _extname );
            if ( ( flag = DetectPath(curFileName) ) )
            {
                _blockFiles.push_back(curFileName);
            }
        }
    }

    this->nextBlock();
}

bool BlockInFile::nextBlock()
{
    if ( _index >= (long)_blockFiles.size() )
    {
        return false;
    }
    bool r = this->open( _blockFiles[_index], ( _isTextMode ? TEXT("r") : TEXT("rb") ) );
    if ( r )
    {
        _index++;
    }
    return r;
}

String BlockInFile::getLine()
{
    String line;
    // 若文件未结束，并且行尾不是换行符，继续读行
    while ( !this->eof() && ( line.length() == 0 || line[line.length() - 1] != Literal<String::value_type>::lfChar ) )
    {
        line += File::getLine();
    }
    return line;
}

bool BlockInFile::eof()
{
    bool r = File::eof();
    if ( r )
    {
        return !this->nextBlock();
    }
    return r;
}


} // namespace winux
