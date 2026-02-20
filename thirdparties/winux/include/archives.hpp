#ifndef __ARCHIVES_HPP__
#define __ARCHIVES_HPP__
//
// archives 提供一些文档的读写功能
//

namespace winux
{
/** \brief 配置文件类 */
class WINUX_DLL Configure
{
public:
    static String const ConfigVarsSlashChars;

    /** \brief 构造函数0 */
    Configure();

    /** \brief 构造函数1
     *
     *  \param configFile 配置文件的路径 */
    Configure( String const & configFile );

    /** \brief 载入配置文件，返回加载的配置变量个数。不会清空原数据 */
    int load( String const & configFile );

    /** \brief 判断是否含有该变量 */
    bool has( String const & name ) const { return _rawParams.find(name) != _rawParams.end(); }

    /** \brief 按指定方式获取变量值 */
    String get( String const & name, bool stripslashes = false, bool expand = false ) const;

    /** \brief 获取变量未展开的值 */
    String operator [] ( String const & name ) const;

    /** \brief 获取变量展开的值 */
    String operator () ( String const & name ) const;

    /** \brief 以RAW方式设置一个配置变量
     *
     * 必须是单行字符串值，特殊字符必须反转义 */
    void setRaw( String const & name, String const & value );

    /** \brief 设置一个配置变量
     *
     *  值会自动反转义，因此无法包含$(XXX)型的内部待展开变量，因为set()内部会自动反转义变成\\$\\(XXX\\)。
     *  需要设置$(XXX)型内部待展开变量的请使用setRaw()。 */
    void set( String const & name, String const & value );

    /** \brief 删除一个配置变量 */
    bool del( String const & name );

    /** \brief 清空所有配置变量 */
    void clear();

    /** \brief 取得内部StringStringMap引用 */
    StringStringMap const & getAll() const { return _rawParams; }

private:
    String _configFile; //!< 配置文件
    StringStringMap _rawParams; //!< 未StripSlashes处理的数据集合

    // 查找配置引用变量
    static int _FindConfigRef( String const & str, int offset, int * length, String * name );
    // 展开指定名称引用变量的值
    String _expandVarNoStripSlashes( String const & name, StringArray * chains ) const;
    // 展开指定值中的引用变量的值
    String _expandValNoStripSlashes( String const & val, StringArray * chains ) const;

    // 返回加载的配置变量个数
    int _load( String const & configFile, StringStringMap * rawParams, StringArray * loadFileChains );
};

/** \brief 更强大的配置文件类
 *
 *  支持表达式 */
class WINUX_DLL ConfigureSettings
{
public:
    /** \brief 构造函数1
     *
     *  如果需要设置配置文件的外部变量，必须先调用set()，然后才load()配置文件。
     *  \param settingsFile 配置文件路径 */
    ConfigureSettings( String const & settingsFile = $T("") );
    ~ConfigureSettings();
    ConfigureSettings( ConfigureSettings const & other );
    ConfigureSettings( ConfigureSettings && other );
    ConfigureSettings & operator = ( ConfigureSettings const & other );
    ConfigureSettings & operator = ( ConfigureSettings && other );

    /** \brief 加载设置文件
     *
     *  \return 返回顶层KeyName数 */
    size_t load( String const & settingsFile );

    /** \brief 更新表达式并计算结果。（当你修改表达式后应该执行这个函数一次）
     *
     *  \param multiname 此参数不是表达式，而是一系列键名。可以用任何表达式可以识别的符号隔开（例如 > , . ），如果键名含空格应该用引号包起来。
     *  \param updateExprStr 更新的表达式，为空表示不更改表达式，只重新计算更新值 */
    Mixed & update( String const & multiname, String const & updateExprStr = $T("") );

    /** \brief 以根变量场景执行表达式并返回引用，如果不能执行则返回内部一个引用 */
    Mixed & execRef( String const & exprStr ) const;
    /** \brief 以根变量场景执行表达式并返回值，如果不能执行则返回默认值 */
    Mixed execVal( String const & exprStr, Mixed const & defval = mxNull ) const;

    /** \brief 获取此名字的设置（只读） */
    Mixed const & operator [] ( String const & name ) const;
    /** \brief 获取此名字的设置 */
    Mixed & operator [] ( String const & name );

    /** \brief 判断是否有此名字的设置 */
    bool has( String const & name ) const;

    /** \brief 获取此名字的设置（只读） */
    Mixed const & get( String const & name ) const;

    /** \brief 设置此名字的设置 */
    ConfigureSettings & set( String const & name, Mixed const & value );

    /** \brief 值 */
    Mixed const & val() const;
    /** \brief 值 */
    Mixed & val();

    /** \brief 表达式 */
    Mixed const & expr() const;
    /** \brief 表达式 */
    Mixed & expr();

    /** \brief 获取设置文件目录 */
    String getSettingsDir() const;

private:
    size_t _load( String const & settingsFile, winux::Mixed * collAsVal, winux::Mixed * collAsExpr, StringArray * loadFileChains );

    String _settingsFile; // 设置文件
    Mixed _collectionVal; // 存储值
    Mixed _collectionExpr; // 存储表达式串
#if defined(OS_WIN)
    PlainMembers<struct ConfigureSettings_Data, 96> _self;
#else
    PlainMembers<struct ConfigureSettings_Data, 136> _self;
#endif
};

/** \brief CSV文件写入器 */
class WINUX_DLL CsvWriter
{
public:
    CsvWriter( IFile * outputFile );

    /** \brief 写入所有记录和列标头 */
    void write( Mixed const & records, Mixed const & columnHeaders = mxNull );
    /** \brief CSV文件写入一条记录 */
    void writeRecord( Mixed const & record );

private:
    IFile * _outputFile;
};

/** \brief CSV文件读取器 */
class WINUX_DLL CsvReader
{
public:
    CsvReader( IFile * inputFile, bool hasColumnHeaders = false );
    CsvReader( String const & content, bool hasColumnHeaders = false );

    /** \brief 获取一行记录，再次[]可获得某一列 */
    Mixed const & operator [] ( int iRow ) const { return _records[iRow]; }
    /** \brief 如果从CSV中读取了列标头，那可以通过列标头名获取一行记录中的某一列 */
    Mixed const & operator () ( int iRow, String const & name ) const { return _records[iRow][ _columns[name] ]; }
    /** \brief 获取读取到的记录数 */
    size_t getCount() const { return _records.getCount(); }

    /** \brief 获取所有记录的引用，可直接操作 */
    Mixed & getRecords() { return _records; }
    /** \brief 获取所有列标头 */
    Mixed & getColumnHeaders() { return _columns; }
    /** \brief 解析CSV数据，hasColumnHeaders表示CSV中第一行是否为列标头行 */
    void read( String const & content, bool hasColumnHeaders = false );

private:
    Mixed _columns; // 第一行列名代表的索引
    Mixed _records;
};

/** \brief 文本文档类。可载入文本文件自动识别BOM文件编码，转换为指定编码。\n

    核心参数有三个：\n
    文件编码（如果是直接写文档则需要指定一个文件编码）\n
    内容编码（当给文本文档写入内容时，内容的编码）\n
    多字节编码（当文件编码为多字节时，这个参数指示是哪一个多字节编码）\n
    BOM字节序：\n
    [00 00 FE FF] UTF32BE\n
    [FF FE 00 00] UTF32LE\n
    [FE FF] UTF16BE\n
    [FF FE] UTF16LE\n
    [EF BB BF] UTF8BOM */
class WINUX_DLL TextArchive
{
public:
    /** \brief 构造函数1
     *
     *  \param fileEncoding 文件编码，如果是直接写文档则需要指定一个文件编码
     *  \param contentEncoding 内容编码，当给文本文档写入内容时，内容的编码
     *  \param mbsEncoding 多字节字符串编码，当文件编码为多字节时，这个参数指示是哪一个多字节编码 */
    TextArchive( FileEncoding fileEncoding = feMultiByte, winux::AnsiString const & contentEncoding = "", winux::AnsiString const & mbsEncoding = "" ) : _fileEncoding(fileEncoding), _contentEncoding(contentEncoding), _mbsEncoding(mbsEncoding)
    {
    }

    /** \brief 设置文件编码。文件编码当载入文本文件时会自动设置。但如果一开始就只是写文本文档，还是需要手动设置文件编码。 */
    void setFileEncoding( FileEncoding fileEncoding )
    {
        this->_fileEncoding = fileEncoding;
    }

    /** \brief 获取文件编码 */
    FileEncoding const & getFileEncoding() const
    {
        return this->_fileEncoding;
    }

    /** \brief 设置内容编码。当给文本文档写入内容时，内容默认是什么编码，用这个函数指定。 */
    void setContentEncoding( winux::AnsiString const & contentEncoding )
    {
        this->_contentEncoding = contentEncoding;
    }

    /** \brief 获取内容编码 */
    winux::AnsiString const & getContentEncoding() const
    {
        return this->_contentEncoding;
    }

    /** \brief 设置多字节编码。当文件编码为多字节时，这个参数指示是哪一个多字节编码。 */
    void setMultiByteEncoding( winux::AnsiString const & mbsEncoding )
    {
        this->_mbsEncoding = mbsEncoding;
    }

    /** \brief 获取多字节编码 */
    winux::AnsiString const & getMultiByteEncoding() const
    {
        return this->_mbsEncoding;
    }

    /** \brief 载入文本文件
     *
     *  \param filePath 文件路径
     *  \param isConvert 是否转换编码
     *  \param targetEncoding 转换到此编码
     *  \param mbsEncoding 多字节编码 */
    void load( winux::String const & filePath, bool isConvert, winux::AnsiString const & targetEncoding = "", winux::AnsiString const & mbsEncoding = "" )
    {
        this->load( winux::FileGetContentsEx( filePath, false ), isConvert, targetEncoding, mbsEncoding );
    }

    /** \brief 载入文本文件
     *
     *  \param f 文件对象
     *  \param isConvert 是否转换编码
     *  \param targetEncoding 转换到此编码
     *  \param mbsEncoding 多字节编码 */
    void load( winux::IFile * f, bool isConvert, winux::AnsiString const & targetEncoding = "", winux::AnsiString const & mbsEncoding = "" )
    {
        this->load( f->buffer(true), isConvert, targetEncoding, mbsEncoding );
    }

    /** \brief 载入指定内容
     *
     *  \param content 载入的内容
     *  \param isConvert 是否转换编码
     *  \param targetEncoding 转换到此编码
     *  \param mbsEncoding 多字节编码 */
    void load( winux::Buffer const & content, bool isConvert, winux::AnsiString const & targetEncoding = "", winux::AnsiString const & mbsEncoding = "" )
    {
        this->setMultiByteEncoding(mbsEncoding);

        size_t i = 0;
        this->_recognizeEncode( content, &i );
        this->_processContent( winux::Buffer( content.getBuf<winux::byte>() + i, content.getSize() - i, true ), isConvert, targetEncoding );
    }

    /** \brief 写入字符串内容 */
    template < typename _ChTy >
    void write( winux::XString<_ChTy> const & content )
    {
        this->write( winux::Buffer( (void *)content.c_str(), content.length() * sizeof(_ChTy), true ) );
    }

    /** \brief 写入字符串内容
     *
     *  \param content 字符串内容
     *  \param encoding 字符串内容的编码 */
    template < typename _ChTy >
    void write( winux::XString<_ChTy> const & content, winux::AnsiString const & encoding )
    {
        this->write( winux::Buffer( (void *)content.c_str(), content.length() * sizeof(_ChTy), true ), encoding );
    }

    /** \brief 写入内容 */
    void write( winux::Buffer const & content )
    {
        this->_pureContent.append(content);
    }

    /** \brief 写入内容
     *
     *  \param content 内容
     *  \param encoding 内容的编码 */
    void write( winux::Buffer const & content, winux::AnsiString const & encoding )
    {
        this->setContentEncoding(encoding);
        this->_pureContent.append(content);
    }

    /** \brief 保存本对象的内容到缓冲区 */
    void save( winux::GrowBuffer * output )
    {
        this->save( output, this->_fileEncoding );
    }

    /** \brief 保存本对象的内容到文件 */
    void save( winux::IFile * f )
    {
        this->save( f, this->_fileEncoding );
    }

    /** \brief 保存本对象的内容到文件 */
    void save( winux::String const & filePath )
    {
        this->save( filePath, this->_fileEncoding );
    }

    /** \brief 保存本对象的内容到缓冲区
     *
     *  \param output 缓冲区
     *  \param fileEncoding 文件编码 */
    void save( winux::GrowBuffer * output, FileEncoding fileEncoding )
    {
        this->saveEx( this->_pureContent, this->_contentEncoding, output, fileEncoding );
    }

    /** \brief 保存本对象的内容到文件
     *
     *  \param f 文件对象
     *  \param fileEncoding 文件编码 */
    void save( winux::IFile * f, FileEncoding fileEncoding )
    {
        this->saveEx( this->_pureContent, this->_contentEncoding, f, fileEncoding );
    }

    /** \brief 保存本对象的内容到文件
     *
     *  \param filePath 文件路径
     *  \param fileEncoding 文件编码 */
    void save( winux::String const & filePath, FileEncoding fileEncoding )
    {
        this->saveEx( this->_pureContent, this->_contentEncoding, filePath, fileEncoding );
    }

    /** \brief 保存内容到缓冲区
     *
     *  \param content 内容
     *  \param encoding 内容的编码
     *  \param output 输出的缓冲区
     *  \param fileEncoding 文件编码 */
    void saveEx( winux::Buffer const & content, winux::AnsiString const & encoding, winux::GrowBuffer * output, FileEncoding fileEncoding );

    /** \brief 保存内容到文件对象
     *
     *  \param content 内容
     *  \param encoding 内容的编码
     *  \param f 输出的文件对象
     *  \param fileEncoding 文件编码 */
    void saveEx( winux::Buffer const & content, winux::AnsiString const & encoding, winux::IFile * f, FileEncoding fileEncoding )
    {
        winux::GrowBuffer buf;
        this->saveEx( content, encoding, &buf, fileEncoding );
        f->write(buf);
    }

    /** \brief 保存内容到文件
     *
     *  \param content 内容
     *  \param encoding 内容的编码
     *  \param filePath 输出的文件路径
     *  \param fileEncoding 文件编码 */
    void saveEx( winux::Buffer const & content, winux::AnsiString const & encoding, winux::String const & filePath, FileEncoding fileEncoding )
    {
        winux::File file( filePath, $T("wb") );
        this->saveEx( content, encoding, &file, fileEncoding );
    }

    /** \brief 转成指定字符的字符串 */
    template < typename _ChTy >
    winux::XString<_ChTy> toString() const
    {
        return this->_pureContent.toString<_ChTy>();
    }

    /** \brief 清空文本文档的内容 */
    void clear()
    {
        this->_pureContent.free();
    }

private:
    // 处理内容成pure content
    void _processContent( winux::Buffer const & content, bool isConvert, winux::AnsiString const & encoding = "" );
    // 识别BOM文件编码，*pI返回跳过BOM真正文本开始的位置
    void _recognizeEncode( winux::Buffer const & content, size_t * pI );

    FileEncoding _fileEncoding; // 文件编码
    winux::GrowBuffer _pureContent; // 纯内容
    mutable winux::AnsiString _contentEncoding; // 内容编码
    mutable winux::AnsiString _mbsEncoding; // 多字节编码，当文件编码是MultiByte时，此成员指定是哪个多字节编码
};


} // namespace winux

#endif // __ARCHIVES_HPP__
