//////////////////////////////////////////////////////////////////////////
// purpose: 建立词库和分词功能
// author:  WT
// date:    2013-05-31
//////////////////////////////////////////////////////////////////////////

#if !defined(__WINPLUS_WORDSLIB_HPP__)
#define __WINPLUS_WORDSLIB_HPP__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace winplus
{

// 定义词库文件 ----------------------------------------------------------
/*
    词库文件由两部分组成：文件头、词汇数据。
    文件头采用纯文本形式描述词库的重要属性：词库名称，备注信息，词库类型，压缩方式，编码，词汇量，单项大小
    词汇数据可以是压缩的，也可以是非压缩的，可以是等长项，也可以是不等长的，这些由文件头指出

    以下定义词库文件格式

        {}     表示整个文件
        ()     表示一个整体
        CRLF   代表一个回车换行符
        SPACE  代表非CRLF的空白字符
        +      代表1个或多个
        *      代表0个或多个
        ?      代表0个或1个
        |      代表选项，这个或那个
        //     代表注释说明
    
    文件头的属性名大小写不敏感
    （注意，下面定义文本中空白字符都是便于阅读的，实际不存在）
    {
        (name: (SPACE*) 字符数据 CRLF)?
        (desc: (SPACE*) 字符数据 CRLF)?
        type: (SPACE*) (equal | unequal) CRLF // 描述词项是等长或不等长，默认equal
                                              // 等长词项的情况下，便于利用seek()或者文件内存映射定位，但是存储会比较占空间
                                              // 不等长的情况下，需要在内存中构建一个线性结构存储所有词汇，便于搜索，但存储会节省空间
        itemsize: (SPACE*) 数字 CRLF          // 词项大小(bytes)，当type是等长情况下必须指定，不等长情况下忽略。
        compress: (SPACE*) (none | gzip) CRLF // 词汇数据压缩方式，不压缩或是gzip压缩，默认none
                                              // 在压缩的情况下，会创建一个新的不压缩的词库文件，便于操作
        encoding: (SPACE*) 编码识别串 CRLF    // 默认本地编码
        wordscount: (SPACE*) 数字 CRLF        // 指示词库所含词汇
        CRLF
        词汇数据
    }
 */

/* WordsLibException */
class WINPLUS_DLL WordsLibException : public Error
{
public:
    WordsLibException( AnsiString const & what ) : Error( -1, what ) { }
};

class WINPLUS_DLL WordsLib
{
public:
    WordsLib( AnsiString const & filename ) throw(WordsLibException);
    ~WordsLib();

    // 当str1等于str2.substr(0, strlen(str1))时返回0，大于返回1，小于返回-1
    static int StrMatch( AnsiString const & str1, AnsiString const & str2 );
    // 搜索一个词(精确匹配), 返回其在词库中的索引, 否则返回-1
    int find( AnsiString const & word, int first, int last ) const;
    int find( AnsiString const & word ) const
    {
        return this->find( word, 0, wordscount - 1 );
    }
    // 搜索一个串(模糊匹配), 返回其匹配词库中第一个词的索引以及匹配到的词数, 否则返回-1
    int findEx( AnsiString const & word, int first, int last, int * count ) const;
    int findEx( AnsiString const & word, int * count ) const
    {
        return this->findEx( word, 0, wordscount - 1, count );
    }
    // 分词1 把text拆分成词组。只会把词库有的词放入arrWords
    int splitWords( AnsiString const & text, AnsiStringArray * arrWords ) const;
    // 分词2 把text拆分成词组。会把非词库内容合在一起作为一词放入arrWords
    int splitWords2( AnsiString const & text, AnsiStringArray * arrWords ) const;

    AnsiString prev() const;
    AnsiString next() const;
    AnsiString at( int i ) const;
    AnsiString seek( int index ) const;

    std::map<AnsiString, AnsiString> header;
    AnsiString name, desc, type, compress, encoding;
    int itemsize, wordscount;
    uint filesize;

protected:
    AnsiString getHeader( AnsiString const & key, AnsiString const & defval );

private:
    // 读取头部信息并解析、存储
    void readHeaders();
    
    HANDLE _file, _fileMapping;
    int _dataOffset; // 词汇数据偏移
    LPBYTE _p, _data;
    uint _dataSize; // 实际词汇数据大小
    mutable int _curIndex;

    DISABLE_OBJECT_COPY(WordsLib)
};

} // namespace winplus

#endif // !defined(__WINPLUS_WORDSLIB_HPP__)
