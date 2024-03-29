﻿#ifndef __FILESYS_HPP__
#define __FILESYS_HPP__
//
// filesys 文件系统相关的功能
//

#if defined(_MSC_VER) || defined(WIN32)

#else
#include <dirent.h>
#endif

namespace winux
{

// 特殊平台变量 -------------------------------------------------------------
#if defined(_MSC_VER) || defined(WIN32)
String const DirSep = "\\"; //!< 目录分割符
String const LineSep = "\r\n"; //!< 行分割符
String const PathEnvSep = ";"; //!< PATH环境变量路径分割符
#else
String const DirSep = "/"; //!< 目录分割符
String const LineSep = "\n"; //!< 行分割符
String const PathEnvSep = ":"; //!< PATH环境变量路径分割符
#endif

/** \brief 获取可执行文件的全路径 */
WINUX_FUNC_DECL(String) GetExecutablePath( void );

/** \brief 获取路径名(末尾不含目录分隔符)
 *
 * \param fullPath String const&
 * \param fileName String*
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

/** \brief 获取文件大小 */
WINUX_FUNC_DECL(ulong) FileSize( String const & filename );
/** \brief 获取文件大小（大于4GB的文件） */
WINUX_FUNC_DECL(uint64) FileSize64( String const & filename );

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

/** \brief 更新文件修改时间,访问时间 */
WINUX_FUNC_DECL(bool) FileTouch( String const & filename, time_t time = (time_t)-1, time_t atime = (time_t)-1 );

/** \brief 路径分隔符整理 */
WINUX_FUNC_DECL(String) PathWithSep( String const & path );
WINUX_FUNC_DECL(String) PathNoSep( String const & path );
WINUX_FUNC_DECL(String &) PathWithSep( String * path );
WINUX_FUNC_DECL(String &) PathNoSep( String * path );

/** \brief 把一个目录路径和一个文件名组合成一个新路径 */
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

/** \brief 通用删除,删除文件夹和文件,返回删除的文件夹和文件数 */
WINUX_FUNC_DECL(size_t) CommonDelete( String const & path );

/** \brief 确保目录路径的存在性，如果不存在则创建。
 *
 *  linux下需设置目录权限(8进制数),默认权限0755\n
 *  owner - group - other\n
 *  R W X - R W X - R W X\n
 *  1 1 1 - 1 0 1 - 1 0 1\n
 *  7       5       5 */
WINUX_FUNC_DECL(bool) MakeDirExists( String const & path, int mode = 0755 );

/** \brief 载入文件内容为一个AnsiString，textMode表示是否为文本模式 */
WINUX_FUNC_DECL(AnsiString) FileGetContents( String const & filename, bool textMode = true );

/** \brief 载入文件内容为一个Buffer，textMode表示是否为文本模式 */
WINUX_FUNC_DECL(Buffer) FileGetContentsEx( String const & filename, bool textMode );

/** \brief 把AnsiString内容写入文件，textMode表示是否为文本模式 */
WINUX_FUNC_DECL(bool) FilePutContents( String const & filename, AnsiString const & content, bool textMode = true );

/** \brief 把Buffer内容写入文件，textMode表示是否为文本模式 */
WINUX_FUNC_DECL(bool) FilePutContentsEx( String const & filename, Buffer const & content, bool textMode );

/** \brief 日志 */
WINUX_FUNC_DECL(void) WriteLog( String const & s );

/** \brief 二进制日志 */
WINUX_FUNC_DECL(void) WriteBinLog( void const * data, size_t size );

//#define __LOG__
#ifdef __LOG__
#define LOG(s) winux::WriteLog(s)
#define BIN_LOG(d,s) winux::WriteBinLog((d),(s))
#else
#define LOG(s)
#define BIN_LOG(d,s)
#endif

#ifndef interface
#define interface struct
#endif

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
private:
    String _path;
    String _name;
#if defined(_MSC_VER) || defined(WIN32)
    WIN32_FIND_DATA _wfd;
    SimpleHandle<HANDLE> _findFile;
    bool _first;
#else
    SimpleHandle<DIR*> _findFile;
#endif
public:
    DirIterator( String const & path );
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

    DISABLE_OBJECT_COPY(DirIterator)
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
    /** \brief 写数据，返回写入字节数 */
    virtual size_t write( void const * data, size_t size );
    /** \brief 写数据，返回写入字节数 */
    virtual size_t write( Buffer const & buf );
    /** \brief 重置文件指针到头 */
    virtual bool rewind();
    /** \brief 移动文件指针，offset表示偏移量 */
    virtual bool seek( offset_t offset );
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
    /** \brief 取得缓冲区 */
    virtual void * buffer( size_t * size );
    /** \brief 取得缓冲区 */
    virtual AnsiString buffer();
};

/** \brief 文件操作类 */
class WINUX_DLL File : public IFile
{
protected:
    String _filename;
    FILE * _fp;
    bool _autoload; //!< 当以读取模式打开时，是否自动载入数据到缓冲区
    size_t _fileSize; //!< 文件的字节大小，这和数据加载大小不一定相同
    size_t _loadedSize; //!< 实际加载的字节大小，这和文件大小不一定相同
    Buffer _buf;

    void _loadData();
public:
    File( String const & filename, String const & mode, bool autoload = true );
    virtual ~File();

    virtual bool open( String const & filename, String const & mode ) override;
    virtual bool close() override;
    virtual size_t read( void * buf, size_t size ) override;
    virtual size_t write( void const * data, size_t size ) override;
    virtual size_t write( Buffer const & buf ) override;
    virtual bool rewind() override;
    virtual bool seek( offset_t offset ) override;
    virtual size_t tell() override;
    virtual String getLine() override;
    virtual int puts( String const & str ) override;
    virtual bool eof() override;
    virtual size_t size() override;
    virtual void * buffer( size_t * size ) override;
    virtual AnsiString buffer() override;

    size_t loadedSize() const { return _loadedSize; }
    operator bool() const { return _fp != NULL; }

    DISABLE_OBJECT_COPY(File)
};

/** \brief 分块输出文件 */
class WINUX_DLL BlockOutFile : public File
{
protected:
    String _dirname;   //!< 目录名   path/to
    String _basename;  //!< 文件名   filename.txt
    String _filetitle; //!< 文件标题 filename
    String _extname;   //!< 扩展名   .txt
    long _fileno;      //!< 文件编号

    bool _isTextMode;  //!< 是否文本模式
    size_t _blockSize; //!< 块大小

public:
    BlockOutFile( String const & filename, bool isTextMode = true, size_t blockSize = 1048576 );

    bool nextBlock();
    virtual size_t write( void const * data, size_t size ) override;
    virtual size_t write( Buffer const & buf ) override;
    virtual int puts( String const & str ) override;

    DISABLE_OBJECT_COPY(BlockOutFile)
};

/** \brief 分块输入文件 */
class WINUX_DLL BlockInFile : public File
{
protected:
    String _dirname;   //!< 目录名   path/to
    String _basename;  //!< 文件名   filename.txt
    String _filetitle; //!< 文件标题 filename
    String _extname;   //!< 扩展名   .txt
    long _index;       //!< 文件索引

    bool _isTextMode;  //!< 是否文本模式
    StringArray _blockFiles; //!< 要加载的分块文件
public:
    BlockInFile( String const & filename, bool isTextMode = true );

    bool nextBlock();
    virtual bool eof() override;

    DISABLE_OBJECT_COPY(BlockInFile)
};

}

#endif //__FILESYS_HPP__
