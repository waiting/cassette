#ifndef __FILESYS_HPP__
#define __FILESYS_HPP__
//
// filesys 文件系统相关的功能
//

#if defined(_MSC_VER) || defined(WIN32)

#else
//#include <dirent.h> // opendir(), readdir()
#include <glob.h>   // glob()
#endif

namespace winux
{
// 特殊平台变量 -------------------------------------------------------------
String const DirSep_WIN = TEXT("\\"); //!< Windows目录分割符
String const LineSep_WIN = TEXT("\r\n"); //!< Windows行分割符
String const PathEnvSep_WIN = TEXT(";"); //!< Windows PATH环境变量路径分割符
String const DirSep_UNIX = TEXT("/"); //!< Unix目录分割符
String const LineSep_UNIX = TEXT("\n"); //!< Unix行分割符
String const PathEnvSep_UNIX = TEXT(":"); //!< Unix PATH环境变量路径分割符
String const DirSep_DARWIN = TEXT("/"); //!< Apple系统目录分割符
String const LineSep_DARWIN = TEXT("\r"); //!< Apple系统行分割符
String const PathEnvSep_DARWIN = TEXT(":"); //!< Apple系统PATH环境变量路径分割符

#if defined(OS_WIN)
String const DirSep = TEXT("\\"); //!< 目录分割符
String const LineSep = TEXT("\r\n"); //!< 行分割符
String const PathEnvSep = TEXT(";"); //!< PATH环境变量路径分割符
#elif defined(OS_DARWIN)
String const DirSep = TEXT("/"); //!< 目录分割符
String const LineSep = TEXT("\r"); //!< 行分割符
String const PathEnvSep = TEXT(":"); //!< PATH环境变量路径分割符
#else
String const DirSep = TEXT("/"); //!< 目录分割符
String const LineSep = TEXT("\n"); //!< 行分割符
String const PathEnvSep = TEXT(":"); //!< PATH环境变量路径分割符
#endif

/** \brief 获取可执行文件的全路径 */
WINUX_FUNC_DECL(String) GetExecutablePath( void );

/** \brief 获取文件目录路径（末尾不含目录分隔符）
 *
 * \param fullPath 文件全路径
 * \param fileName 返回文件名
 * \return String */
WINUX_FUNC_DECL(String) FilePath( String const & fullPath, String * fileName = NULL );

/** \brief 获取文件标题
 *
 * \param fileName String const& 文件名
 * \param extName String* 返回扩展名（不包括'.'）
 * \return String */
WINUX_FUNC_DECL(String) FileTitle( String const & fileName, String * extName = NULL );

/** \brief 判断是否为绝对路径 */
WINUX_FUNC_DECL(bool) IsAbsPath( String const & path );

/** \brief 使路径规则化(末尾不带路径分割符) */
WINUX_FUNC_DECL(String) NormalizePath( String const & path );

/** \brief 根据当前工作目录计算绝对路径，不会检查存在性 */
WINUX_FUNC_DECL(String) RealPath( String const & path );

/** \brief 根据指定工作目录计算绝对路径，不会检查存在性
 *
 *  \param workDirAbsPath 绝对路径指定工作目录，如果为空则效果同RealPath() */
WINUX_FUNC_DECL(String) RealPathEx( String const & path, String const & workDirAbsPath );

/** \brief 返回当前工作目录(末尾不含目录分隔符) */
WINUX_FUNC_DECL(String) GetCurrentDir( void );

/** \brief 设置当前工作目录 */
WINUX_FUNC_DECL(bool) SetCurrentDir( String const & path );

/** \brief 判断是否是一个目录 */
WINUX_FUNC_DECL(bool) IsDir( String const & path );

/** \brief 探测一个路径是存在还是不存在，是目录还是文件
 *
 *  返回true/false表示是否存在, *isDir返回true/false表示是否为目录 */
WINUX_FUNC_DECL(bool) DetectPath( String const & path, bool * isDir = NULL );

/** \brief 获取文件大小（也适用于大于4GB的文件） */
WINUX_FUNC_DECL(uint64) FileSize( String const & filename );

/** \brief 获取文件时间
 *
 *  \param filename String const &
 *  \param ctime time_t * 可传递nullptr表示不获取create time
 *  \param mtime time_t * 可传递nullptr表示不获取modify time
 *  \param atime time_t * 可传递nullptr表示不获取access time
 *  \return bool */
WINUX_FUNC_DECL(bool) FileTime( String const & filename, time_t * ctime, time_t * mtime, time_t * atime );
/** \brief 获取文件创建时间 */
WINUX_FUNC_DECL(time_t) FileCTime( String const & filename );
/** \brief 获取文件修改时间 */
WINUX_FUNC_DECL(time_t) FileMTime( String const & filename );
/** \brief 获取文件访问时间 */
WINUX_FUNC_DECL(time_t) FileATime( String const & filename );

/** \brief 更新文件修改时间，访问时间 */
WINUX_FUNC_DECL(bool) FileTouch( String const & filename, time_t time = (time_t)-1, time_t atime = (time_t)-1 );

/** \brief 路径末尾带分隔符。如果path是空串则不作处理 */
WINUX_FUNC_DECL(String) PathWithSep( String const & path );
/** \brief 路径末尾不带分隔符。如果path是空串则不作处理 */
WINUX_FUNC_DECL(String) PathNoSep( String const & path );
/** \brief 路径末尾带分隔符。如果*path是空串则不作处理 */
WINUX_FUNC_DECL(String &) PathWithSep( String * path );
/** \brief 路径末尾不带分隔符。如果*path是空串则不作处理 */
WINUX_FUNC_DECL(String &) PathNoSep( String * path );

/** \brief 把一个目录路径和一个文件名组合成一个新路径
 * 
 *  如果`dirPath`为空则直接返回`fileName`，如果`fileName`为空则返回`PathWithSep(dirPath)`。 */
WINUX_FUNC_DECL(String) CombinePath( String const & dirPath, String const & fileName );

/** \brief 获取文件夹中的文件和子文件夹。sortType:0结果不排序 1正序 2反序 */
WINUX_FUNC_DECL(void) FolderData( String const & path, StringArray * fileArr, StringArray * subFolderArr, int sortType = 0 );

/** \brief 在指定路径下枚举指定扩展名的文件
 *
 *  \param path 路径
 *  \param ext 可以是扩展名串，也可以是一个数组。Mixed(MT_NULL)表示忽略
 *  \param arrFiles 返回的结果。
 *  \param isRecursive 是否递归。当为false时，arrFiles返回的结果不用区别目录，因此不包含目录部分的路径。
 *  \return size_t 文件数 */
WINUX_FUNC_DECL(size_t) EnumFiles( String const & path, Mixed const & ext, StringArray * arrFiles, bool isRecursive = false );

/** \brief 断开路径与磁盘文件的链接，即删除文件（只能删除文件，不能删除目录）
 *
 *  当一个文件被unlink后，它实际上并没有立即从文件系统中删除，而是变成了“孤立”的状态。这意味着文件的内容仍然占用着磁盘空间，但是不再有任何路径可以访问到这个文件。\n
 *  如果程序通过文件描述符仍然持有对该文件的打开句柄，那么它仍然可以继续访问文件的内容，直到所有打开的句柄都被关闭。\n
 *  当最后一个对该文件的引用（即所有打开的文件描述符）被关闭时，文件才会被彻底删除。 */
WINUX_FUNC_DECL(bool) UnlinkFile( String const & filePath );

/** \brief 删除空目录。如果目录非空则失败 */
WINUX_FUNC_DECL(bool) RemoveDir( String const & dirPath );

/** \brief 删除文件和空目录。如果路径指向的是一个非空目录则失败
 *
 *  删除文件时，不会保留文件内容，即使文件仍然被某些程序打开。 */
WINUX_FUNC_DECL(bool) RemovePath( String const & path );

/** \brief 重命名文件或目录，或者将文件或目录从一个目录移动到另一个目录 */
WINUX_FUNC_DECL(bool) RenamePath( String const & oldPath, String const & newPath );

/** \brief 通用删除。删除文件夹和文件，返回删除的文件夹和文件数 */
WINUX_FUNC_DECL(size_t) CommonDelete( String const & path );

/** \brief 确保目录路径的存在性，如果不存在则创建。
 *
 *  linux下需设置目录权限(8进制数)，默认权限0755\n
 *  owner - group - other\n
 *  R W X - R W X - R W X\n
 *  1 1 1 - 1 0 1 - 1 0 1\n
 *  7       5       5 */
WINUX_FUNC_DECL(bool) MakeDirExists( String const & path, int mode = 0755 );

///////////////////////////////////////////////////////////////////////////////////////////////
/** \brief 文件编码 */
enum FileEncoding
{
    feUnspec,       //!< 未指定
    feMultiByte,    //!< 多字节
    feUtf8,         //!< UTF-8
    feUtf8Bom,      //!< UTF-8 with BOM
    feUtf16Le,      //!< UTF-16小端序
    feUtf16Be,      //!< UTF-16大端序
    feUtf16 = feUtf16Be,    //!< UTF-16大端序
    feUtf32Le,      //!< UTF-32小端序
    feUtf32Be,      //!< UTF-32大端序
    feUtf32 = feUtf32Be     //!< UTF-32大端序
};

/** \brief 识别文件编码
 *
 *  \param[in] content 文件内容
 *  \param[in, out] pIndex 传入起始位置，返回跳过BOM真正文本开始的位置
 *  \param[in] limitSize 限制搜寻大小，默认-1是指按内容大小 */
WINUX_FUNC_DECL(FileEncoding) RecognizeFileEncoding( winux::Buffer const & content, size_t * pIndex, size_t limitSize = -1 );

/** \brief 从文件中换行到程序环境中。b表示是否反序，配合IsLittleEndian()/IsBigEndian()使用 */
template < typename _ChTy >
XString<_ChTy> NewlineFromFile( _ChTy const * content, size_t len, bool b );
template <>
WINUX_FUNC_DECL(XString<char>) NewlineFromFile( char const * content, size_t len, bool b );
template <>
WINUX_FUNC_DECL(XString<wchar>) NewlineFromFile( wchar const * content, size_t len, bool b );
template <>
WINUX_FUNC_DECL(XString<char16>) NewlineFromFile( char16 const * content, size_t len, bool b );
template <>
WINUX_FUNC_DECL(XString<char32>) NewlineFromFile( char32 const * content, size_t len, bool b );

/** \brief 从程序环境中换行到文件中。b表示是否反序，配合IsLittleEndian()/IsBigEndian()使用 */
template < typename _ChTy >
XString<_ChTy> NewlineToFile( _ChTy const * content, size_t len, bool b );
template <>
WINUX_FUNC_DECL(XString<char>) NewlineToFile( char const * content, size_t len, bool b );
template <>
WINUX_FUNC_DECL(XString<wchar>) NewlineToFile( wchar const * content, size_t len, bool b );
template <>
WINUX_FUNC_DECL(XString<char16>) NewlineToFile( char16 const * content, size_t len, bool b );
template <>
WINUX_FUNC_DECL(XString<char32>) NewlineToFile( char32 const * content, size_t len, bool b );

/** \brief 载入文件内容为文本字符串
 *
 *  此函数能识别BOM，即使在多字节字符环境下也要比`FileGetContents()`好用，但效率可能不如它。
 *  \param filename 文件路径
 *  \param encoding 文件编码。默认`feMultiByte`表示系统本地编码。`feUnspec`表示自动从文件识别
 *  \return String */
WINUX_FUNC_DECL(String) FileGetString( String const & filename, FileEncoding encoding = feMultiByte );

/** \brief 载入文件内容为一个`AnsiString`，textMode表示是否为文本模式 */
WINUX_FUNC_DECL(AnsiString) FileGetContents( String const & filename, bool textMode = true );

/** \brief 载入文件内容为一个`Buffer`，textMode表示是否为文本模式 */
WINUX_FUNC_DECL(Buffer) FileGetContentsEx( String const & filename, bool textMode );

/** \brief 把字符串内容写入文件
 *
 *  \param filename 文件路径
 *  \param content 要写的文本内容
 *  \param encoding 文件编码。默认`feMultiByte`表示系统本地编码。若是`feUnspec`则当作`feMultiByte`处理
 *  \return bool */
WINUX_FUNC_DECL(bool) FilePutString( String const & filename, String const & content, FileEncoding encoding = feMultiByte );

/** \brief 把`AnsiString`内容写入文件，textMode表示是否为文本模式 */
WINUX_FUNC_DECL(bool) FilePutContents( String const & filename, AnsiString const & content, bool textMode = true );

/** \brief 把`Buffer`内容写入文件，textMode表示是否为文本模式 */
WINUX_FUNC_DECL(bool) FilePutContentsEx( String const & filename, Buffer const & content, bool textMode );

/** \brief 在给定目录备份指定文件
 *
 *  \param filePath 要备份的文件路径
 *  \param bakDir 备份文件存放的目录（相对于`filePath`）
 *  \param fmt %f是文件标题，%v是版本编号，%e是扩展名（不包含'.'），%E是扩展名（包含'.'）
 *  \return String 备份成功返回文件路径，备份失败返回空串 */
WINUX_FUNC_DECL(String) BackupFile( String const & filePath, String const & bakDir = TEXT(""), String const & fmt = TEXT("%f_v%v%E") );


/** \brief 文件系统错误类 */
class FileSysError : public Error
{
public:
    enum
    {
        fseNone, //!< 没有错误
        fseNotImplemented, //!< 方法未实现
        fseNotFound, //!< 文件系统错误：文件或目录不存在
        fsePermissionDenied, //!< 文件系统错误：无权限拒绝访问
        fseInvalidArgument, //!< 文件系统错误：无效参数
    };
    FileSysError( int errType, AnsiString const & s ) throw() : Error( errType, s ) { }
};

/** \brief 目录文件枚举器 */
class WINUX_DLL DirIterator
{
public:
    /** \brief 构造函数
     *
     *  \param[in] path 路径 */
    DirIterator( String const & path, String const & pattern = TEXT("*") );
    /** \brief 析构函数 */
    ~DirIterator();
    /** \brief 取得路径 */
    String const & getPath() const { return _path; }
    /** \brief 取得文件名 */
    String const & getName() const { return _name; }
    /** \brief 取得文件全路径 */
    String getFullPath() const;
    /** \brief 取得文件真实路径 */
    String getRealPath() const;
    /** \brief 判断是否为文件夹 */
    bool isDir() const;
    /** \brief 下一个文件/文件夹 */
    bool next();

private:
    String _path;
    String _name;
#if defined(_MSC_VER) || defined(WIN32)
    WIN32_FIND_DATA _wfd;
    SimpleHandle<HANDLE> _findFile;
    bool _first;
#else
    #if defined(__USE_LARGEFILE64)
    glob64_t _findFile;
    #else
    glob_t _findFile;
    #endif
    size_t _curr;
#endif

    DISABLE_OBJECT_COPY(DirIterator)
};

/** \brief Seek类型 */
enum SeekType
{
    seekSet = 0, //!< 相对于开头位置
    seekCur = 1, //!< 相对于当前位置
    seekEnd = 2  //!< 相对于结束位置
};

/** \brief 文件接口 */
interface WINUX_DLL IFile
{
    virtual ~IFile() { }

    /** \brief 打开文件 */
    virtual bool open( String const & filename, String const & mode );
    /** \brief 关闭文件 */
    virtual bool close();
    /** \brief 读数据，返回读取的字节数 */
    virtual size_t read( void * buf, size_t size );
    /** \brief 读数据 */
    Buffer read( size_t size );
    /** \brief 读POD类型数据 */
    template < typename _PodType >
    bool readType( _PodType * data, size_t size = sizeof(_PodType) )
    {
        return this->read( data, size ) == size;
    }
    /** \brief 读POD类型数据 */
    template < typename _PodType >
    _PodType readType( size_t size = sizeof(_PodType) )
    {
        _PodType data;
        this->readType( &data, size );
        return data;
    }
    /** \brief 写数据，返回写入的字节数 */
    virtual size_t write( void const * data, size_t size );
    /** \brief 写数据，返回写入的字节数 */
    size_t write( Buffer const & buf );
    /** \brief 写POD类型数据 */
    template < typename _PodType >
    bool writeType( _PodType const & data, size_t size = sizeof(_PodType) )
    {
        return this->write( &data, size ) == size;
    }
    /** \brief 重置文件指针到头 */
    virtual bool rewind();
    /** \brief 移动文件指针，offset表示偏移量 */
    virtual bool seek( offset_t offset, SeekType origin = seekSet );
    /** \brief 获得文件指针位置 */
    virtual size_t tell();
    /** \brief 获取一行字符串，包括换行符。如果返回空串说明文件结束 */
    virtual String getLine();
    /** \brief 输出字符串 */
    virtual int puts( String const & str );
    /** \brief 文件是否结束 */
    virtual bool eof();
    /** \brief 文件大小 */
    virtual size_t size();
    /** \brief 读取整个文件内容，取得数据大小
     *
     *  \param size 输出数据大小
     *  \return 数据指针
     *  \attention 由于文本模式可能存在字符转换，数据大小并不一定等于文件大小 */
    virtual void * entire( size_t * size );
    /** \brief 读取整个文件内容作字符串
     *
     *  \param encoding 文件编码。默认为`feMultiByte`
     *  \param convertNewline 是否转换换行符。如果文件以文本模式打开，此参数通常传递false。UTF-16、UTF-32文本文件应传递true
     *  \return String */
    String entire( FileEncoding encoding = feMultiByte, bool convertNewline = false );
};

/** \brief 内存式文件 */
class WINUX_DLL MemoryFile : public IFile
{
public:
    /** \brief 构造函数0 */
    MemoryFile();
    /** \brief 构造函数1
     *
     *  \param[in] data 数据指针
     *  \param[in] size 数据大小
     *  \param[in] isPeek 是否为窥探模式 */
    MemoryFile( void const * data, size_t size, bool isPeek = false );
    /** \brief 构造函数2
     *
     *  \param[in] buf 数据缓冲区对象
     *  \param[in] isPeek 是否为窥探模式 */
    MemoryFile( Buffer const & buf, bool isPeek = false );
    /** \brief 构造函数3
     *
     *  \param[in] content 内容
     *  \param[in] isPeek 是否为窥探模式 */
    MemoryFile( AnsiString const & content, bool isPeek = false );

    /** \brief 以字符串内容以内存文件的方式读写
     *
     *  与IFile接口定义时不同，内存式文件open()方法的第一个参数指定内存内容。
     *  \param[in] content 字符串内容
     *  \param[in] mode 含"r"表示内部缓冲区为窥探模式 */
    virtual bool open( String const & content, String const & mode = Literal<String::value_type>::nulStr ) override;
    virtual bool close() override;
    virtual size_t read( void * buf, size_t size ) override;
    virtual size_t write( void const * data, size_t size ) override;
    virtual bool rewind() override;
    virtual bool seek( offset_t offset, SeekType origin = seekSet ) override;
    virtual size_t tell() override;
    virtual String getLine() override;
    virtual int puts( String const & str ) override;
    virtual bool eof() override;
    virtual size_t size() override;
    virtual void * entire( size_t * size ) override;

    using IFile::read;
    using IFile::write;
    using IFile::entire;

protected:
    GrowBuffer _buf;
    winux::byte * _p;

    DISABLE_OBJECT_COPY(MemoryFile)
};

/** \brief 文件操作类 */
class WINUX_DLL File : public IFile
{
public:
    File();
    File( String const & filename, String const & mode );
    virtual ~File();

    virtual bool open( String const & filename, String const & mode ) override;
    virtual bool close() override;
    virtual size_t read( void * buf, size_t size ) override;
    virtual size_t write( void const * data, size_t size ) override;
    virtual bool rewind() override;
    virtual bool seek( offset_t offset, SeekType origin = seekSet ) override;
    virtual size_t tell() override;
    virtual String getLine() override;
    virtual bool eof() override;
    virtual size_t size() override;
    virtual void * entire( size_t * size ) override;

    using IFile::read;
    using IFile::write;
    using IFile::entire;

    /** \brief 流式文件API指针 */
    FILE * get() const { return _fp; }

    /** \brief 获取相关联的文件描述符，此fd不可close() */
    int getFd() const;

    #if defined(OS_WIN)
    /** \brief 获取系统平台相关的文件资源柄（Win32文件句柄），此资源柄不可CloseHandle() */
    HANDLE getOsHandle() const;
    #else
    /** \brief 获取系统平台相关的文件资源柄（文件描述符），此资源柄不可close() */
    int getOsHandle() const;
    #endif

    operator bool() const { return _fp != NULL; }

protected:
    FILE * _fp;         //!< FILE*指针

    DISABLE_OBJECT_COPY(File)
};

/** \brief 分块输出文件 */
class WINUX_DLL BlockOutFile : public File
{
public:
    BlockOutFile( String const & filename, bool isTextMode = true, size_t blockSize = 1048576 );

    /** \brief 下一块文件 */
    bool nextBlock();
    virtual size_t write( void const * data, size_t size ) override;
    using File::write;

protected:
    String _dirname;   //!< 目录名   path/to
    String _basename;  //!< 文件名   filename.txt
    String _filetitle; //!< 文件标题 filename
    String _extname;   //!< 扩展名   .txt
    long _fileno;      //!< 文件编号

    bool _isTextMode;  //!< 是否文本模式
    size_t _blockSize; //!< 块大小
    size_t _writtenSize; //!< 以写入数据大小

    DISABLE_OBJECT_COPY(BlockOutFile)
};

/** \brief 分块输入文件 */
class WINUX_DLL BlockInFile : public File
{
public:
    BlockInFile( String const & filename, bool isTextMode = true );

    /** \brief 下一块文件 */
    bool nextBlock();
    virtual String getLine() override;
    virtual bool eof() override;

protected:
    String _dirname;   //!< 目录名   path/to
    String _basename;  //!< 文件名   filename.txt
    String _filetitle; //!< 文件标题 filename
    String _extname;   //!< 扩展名   .txt
    long _index;       //!< 文件索引

    bool _isTextMode;  //!< 是否文本模式
    StringArray _blockFiles; //!< 要加载的分块文件

    DISABLE_OBJECT_COPY(BlockInFile)
};

} // namespace winux

#endif //__FILESYS_HPP__
