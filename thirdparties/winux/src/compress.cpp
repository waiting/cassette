//
// winux的压缩功能
//
#include "utilities.hpp"
#include "smartptr.hpp"
#include "strings.hpp"
#include "filesys.hpp"
#include "compress.hpp"

#include "zip.h"
#include "unzip.h"

namespace winux
{
// struct Zip_Data ------------------------------------------------------------------------
struct Zip_Data
{
    HZIP hzip;

    Zip_Data() : hzip(0)
    {
    }
};

// class Zip ------------------------------------------------------------------------------
String Zip::Message( ZRESULT code )
{
    String s;
    s.resize(128);
    FormatZipMessageZ( code, &s[0], 128 );
    return s.c_str();
}

Zip::Zip()
{
    _self.create(); //

}

Zip::Zip( String const & filename, char const * password /*= NULL */ )
{
    _self.create(); //

    this->create( filename, password );
}

Zip::Zip( void * buf, uint32 size, char const * password /*= NULL */ )
{
    _self.create(); //

    this->create( buf, size, password );
}

Zip::~Zip()
{
    this->close();

    _self.destroy(); //
}

bool Zip::create( String const & filename, char const * password /*= NULL */ )
{
    this->close();

    _self->hzip = CreateZip( filename.c_str(), password );

#if defined(__linux__) || ( defined(__GNUC__) && !defined(_WIN32) )
    return _self->hzip != NULL;
#else
    return _self->hzip != INVALID_HANDLE_VALUE;
#endif
}

bool Zip::create( void * buf, uint32 size, char const * password /*= NULL */ )
{
    this->close();

    _self->hzip = CreateZip( buf, size, password );

#if defined(__linux__) || ( defined(__GNUC__) && !defined(_WIN32) )
    return _self->hzip != NULL;
#else
    return _self->hzip != INVALID_HANDLE_VALUE;
#endif
}

ZRESULT Zip::close()
{
    ZRESULT zr = CloseZipZ(_self->hzip);
    _self->hzip = 0;
    return zr;
}

ZRESULT Zip::addFile( String const & dstPathInZip, String const & srcFilename )
{
    return ZipAdd( _self->hzip, dstPathInZip.c_str(), srcFilename.c_str() );
}

ZRESULT Zip::addFile( String const & dstPathInZip, void * src, uint32 size )
{
    return ZipAdd( _self->hzip, dstPathInZip.c_str(), src, size );
}

ZRESULT Zip::addFolder( String const & dstPathInZip )
{
    return ZipAddFolder( _self->hzip, dstPathInZip.c_str() );
}

static void __ZipAll( Zip * z, String const & dirPath, String const & dstPath )
{
    DirIterator di(dirPath);
    while ( di.next() )
    {
        if ( di.getName() == TEXT(".") || di.getName() == TEXT("..") )
        {
            continue;
        }
        if ( di.isDir() )
        {
            //printf( "subdir. %s | %s\n", di.getFullPath().c_str(), (dstPath + DirSep + di.getName()).c_str() );
            String dst = dstPath + DirSep + di.getName();
            z->addFolder(dst);
            __ZipAll( z, di.getFullPath(), dst );
        }
        else
        {
            String dst = dstPath + DirSep + di.getName();
            z->addFile( dst, di.getFullPath() );
            //printf( "file. %s | %s\n", di.getFullPath().c_str(), (dstPath + DirSep + di.getName()).c_str() );
            //printf( "%s\n", di.getFullPath().c_str() );
        }
    }
}

void Zip::zipAll( String const & dirPath )
{
    String realPath = RealPath(dirPath);
    String dstDir;
    FilePath( realPath, &dstDir );
    //printf( "%s\n", dstDir.c_str() );
    this->addFolder(dstDir);

    __ZipAll( this, dirPath, dstDir );
}

ZRESULT Zip::getMemory( void * * buf, unsigned long * size )
{
    return ZipGetMemory( _self->hzip, buf, size );
}

// struct Unzip_Data ----------------------------------------------------------------------
struct Unzip_Data
{
    HZIP hzip;
    Unzip_Data() : hzip(0)
    {
    }
};

// class Unzip ----------------------------------------------------------------------------
String Unzip::Message( ZRESULT code /*= ZR_RECENT */ )
{
    String s;
    s.resize(128);
    FormatZipMessageU( code, &s[0], 128 );
    return s.c_str();

}

Unzip::Unzip()
{
    _self.create(); //

}

Unzip::Unzip( String const & filename, char const * password /*= NULL */ )
{
    _self.create(); //

    this->open( filename, password );
}

Unzip::Unzip( void * zipbuf, uint32 size, char const * password /*= NULL */ )
{
    _self.create(); //

    this->open( zipbuf, size, password );
}

Unzip::~Unzip()
{
    this->close();

    _self.destroy(); //
}

bool Unzip::open( String const & filename, char const * password /*= NULL */ )
{
    this->close();

    _self->hzip = OpenZip( filename.c_str(), password );

#if defined(__linux__) || ( defined(__GNUC__) && !defined(_WIN32) )
    return _self->hzip != NULL;
#else
    return _self->hzip != INVALID_HANDLE_VALUE;
#endif
}

bool Unzip::open( void * zipbuf, uint32 size, char const * password /*= NULL */ )
{
    this->close();

    _self->hzip = OpenZip( zipbuf, size, password );

#if defined(__linux__) || ( defined(__GNUC__) && !defined(_WIN32) )
    return _self->hzip != NULL;
#else
    return _self->hzip != INVALID_HANDLE_VALUE;
#endif

}

ZRESULT Unzip::close()
{
    ZRESULT zr = CloseZipU(_self->hzip);
    _self->hzip = 0;
    return zr;
}

int Unzip::getEntriesCount() const
{
    ZIPENTRY ze;
    GetZipItem( _self->hzip, -1, &ze );
    return ze.index;
}

ZRESULT Unzip::getEntry( int index, ZipEntry * entry )
{
    ZIPENTRY ze;
    ZRESULT zr;

    zr = GetZipItem( _self->hzip, index, &ze );
    if ( entry && zr == ZR_OK )
    {
        entry->index = ze.index;
        entry->name = ze.name;
        entry->attr = ze.attr;
        entry->atime = ze.atime;
        entry->ctime = ze.ctime;
        entry->mtime = ze.mtime;
        entry->comp_size = ze.comp_size;
        entry->unc_size = ze.unc_size;
    }

    return zr;
}

ZRESULT Unzip::findEntry( String const & name, bool caseInsensitive, int * index, ZipEntry * entry )
{
    ZIPENTRY ze;
    ZRESULT zr;
    zr = FindZipItem( _self->hzip, name.c_str(), caseInsensitive, index, &ze );
    if ( entry && zr == ZR_OK )
    {
        entry->index = ze.index;
        entry->name = ze.name;
        entry->attr = ze.attr;
        entry->atime = ze.atime;
        entry->ctime = ze.ctime;
        entry->mtime = ze.mtime;
        entry->comp_size = ze.comp_size;
        entry->unc_size = ze.unc_size;
    }
    return zr;
}

ZRESULT Unzip::unzipEntry( int index, String const & outFilename )
{
    return UnzipItem( _self->hzip, index, outFilename.c_str() );
}

ZRESULT Unzip::unzipEntry( int index, void * buf, uint32 size )
{
    return UnzipItem( _self->hzip, index, buf, size );
}

void Unzip::unzipAll( String const & dirPath )
{
    if ( !dirPath.empty() )
    {
        this->setUnzipBaseDir(dirPath);
    }

    int nEntries = this->getEntriesCount();
    for ( int i = 0; i < nEntries; ++i )
    {
        ZipEntry ze;
        this->getEntry( i, &ze );
        this->unzipEntry( i, ze.name );
    }
}

ZRESULT Unzip::setUnzipBaseDir( String const & dirPath )
{
    return SetUnzipBaseDir( _self->hzip, dirPath.c_str() );
}


} // namespace winux
