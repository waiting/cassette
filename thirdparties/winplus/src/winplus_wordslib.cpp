#include "winplus_definitions.hpp"
#include "winplus_system.hpp"
#include "winplus_wordslib.hpp"
#include "winplus_strings.hpp"
#include "strings.hpp"

namespace winplus
{

inline bool IsAlpha( int c )
{
    return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z';
}

inline bool IsSpace( int c )
{
    return c >= 0x9 && c <= 0xd || c == 0x20;
}

inline bool IsDigit( int c )
{
    return c >= '0' && c <= '9';
}

WordsLib::WordsLib( AnsiString const & filename ) throw(WordsLibException) :
    _file(INVALID_HANDLE_VALUE),
    _fileMapping(NULL),
    filesize(0),
    _dataOffset(0),
    _p(NULL),
    _data(NULL),
    _dataSize(0),
    _curIndex(-1)
{
    _file = CreateFileA( filename.c_str(), /*FILE_ALL_ACCESS*/GENERIC_READ, /*FILE_SHARE_READ*/0, NULL, OPEN_EXISTING, 0, NULL );
    if ( _file == INVALID_HANDLE_VALUE )
    {
        DWORD dwErr = GetLastError();
        throw WordsLibException( StringToLocal( GetErrorStr(dwErr) ) + "词库文件`" + filename + "`未找到" );
    }

    filesize = (uint)GetFileSize( _file, NULL );
    _fileMapping = CreateFileMapping( _file, NULL, PAGE_READONLY, 0, 0, NULL );

    this->readHeaders(); // 读取头信息

    if ( this->type != "equal" )
    {
        throw WordsLibException("不支持的词库");
    }

    // 指针指向词汇数据
    _dataSize = filesize - _dataOffset;
    _p = (LPBYTE)MapViewOfFile( _fileMapping, FILE_MAP_READ, 0, 0, 0 );
    _data = _p + _dataOffset;
}

WordsLib::~WordsLib()
{
    if ( _p )
    {
        UnmapViewOfFile(_p);
        _p = _data = NULL;
        _dataSize = 0;
    }

    if ( _fileMapping != NULL )
        CloseHandle(_fileMapping);

    if ( _file != INVALID_HANDLE_VALUE ) 
        CloseHandle(_file);
}

void WordsLib::readHeaders()
{
    LPBYTE p = (LPBYTE)MapViewOfFile( _fileMapping, FILE_MAP_READ, 0, 0, 0 );
    CHAR * s = (CHAR *)p;
    int i;
    for ( i = 0; i < filesize - 3; ++i )
    {
        if ( s[i] == '\r' && s[i + 1] == '\n' &&  s[i + 2] == '\r' && s[i + 3] == '\n' )
        {
            break;
        }
    }
    i += 2; // inc "\r\n"
    AnsiString headerStr;
    headerStr.resize(i);
    CopyMemory( &headerStr[0], s, i );

    _dataOffset = i + 2; // skip "\r\n"

    UnmapViewOfFile(p);

    // 解析
    this->header.clear();
    decltype(this->header) tmpHeader;
    AnsiStringArray arrHeaders;
    int count = (int)StrSplit2( headerStr, AnsiString("\r\n"), &arrHeaders );

    for ( i = 0; i < count; ++i )
    {
        AnsiStringArray kv;
        int c = (int)StrSplit( arrHeaders[i], AnsiString(":"), &kv );
        if ( c > 1 )
        {
            AnsiString k = StrTrim(kv[0]);
            StrMakeLower(&k);
            tmpHeader[k] = StrTrim(kv[1]);
        }
        else if ( c > 0 )
        {
            AnsiString k = StrTrim(kv[0]);
            StrMakeLower(&k);
            tmpHeader[k] = "";
        }
    }
    // 检测编码
    this->encoding = isset( tmpHeader, "encoding" ) ? tmpHeader["encoding"] : "";
    if ( this->encoding.length() > 0 ) _strlwr(&this->encoding[0]);
    if ( this->encoding == "utf-8" )
    {
        // utf-8编码
        auto it = tmpHeader.begin();
        for ( ; it != tmpHeader.end(); ++it )
        {
            this->header[it->first] = it->second;
        }
    }
    else // 当作本地编码处理
    {
        auto it = tmpHeader.begin();
        for ( ; it != tmpHeader.end(); ++it )
        {
            this->header[it->first] = it->second;
        }
    }

    // 赋值给变量
    this->name = this->getHeader( "name", "" );
    this->desc = this->getHeader( "desc", "" );
    this->type = this->getHeader( "type", "equal" );
    this->compress = this->getHeader( "compress", "none" );
    this->itemsize = Mixed( this->getHeader( "itemsize", "32" ) );
    this->wordscount = Mixed( this->getHeader( "wordscount", "0" ) );
}

AnsiString WordsLib::getHeader( AnsiString const & key, AnsiString const & defval )
{
    return isset( this->header, key ) && this->header[key] != "" ? this->header[key] : defval;
}

AnsiString WordsLib::prev() const
{
    if ( _curIndex > -1 )
        _curIndex--;

    return this->at(_curIndex);
}

AnsiString WordsLib::next() const
{
    if ( _curIndex < this->wordscount )
        _curIndex++;

    return this->at(_curIndex);
}

AnsiString WordsLib::at( int i ) const
{
    if ( i < 0 || i > this->wordscount - 1 )
    {
        return "";
    }

    AnsiString word;
    word.resize(this->itemsize);
    CopyMemory( &word[0], _data + i * this->itemsize, this->itemsize );
    return word.c_str();
}

AnsiString WordsLib::seek( int index ) const
{
    index = index < -1 ? -1 : index;
    index = index > this->wordscount ? this->wordscount : index;
    _curIndex = index;
    return this->at(_curIndex);
}

int WordsLib::find( AnsiString const & word, int first, int last ) const
{
    while ( first <= last )
    {
        int mid = first + ( last - first ) / 2;
        int cmp = this->at(mid).compare(word);
        if ( cmp == 0 )
        {
            return mid;
        }
        else if ( cmp < 0 )
        {
            first = mid + 1;
        }
        else
        {
            last = mid - 1;
        }
    }
    return -1;
}

int WordsLib::findEx( AnsiString const & word, int first, int last, int * count ) const
{
    count && ( *count = 0 );
    // 首先,先搜匹配word的一个词的位置
    int posMatch = -1;
    while ( first <= last )
    {
        int mid = first + ( last - first ) / 2;
        int cmp = StrMatch( word, this->at(mid) );
        if (cmp == 0)
        {
            posMatch = mid;
            break;
        }
        else if (cmp < 0)
        {
            last = mid - 1;
        }
        else
        {
            first = mid + 1;
        }
    }
    // 搜到
    if ( posMatch != -1 )
    {
        AnsiString text;
        int pos;
        // 向前
        this->seek(posMatch);
        pos = posMatch;
        int prevCount = 0;
        if ( pos > 0 )
        {
            text = this->prev();
            pos--;
            while ( StrMatch( word, text ) == 0 )
            {
                prevCount++;
                if ( pos > 0 )
                {
                    text = this->prev();
                    pos--;
                }
                else
                {
                    break;
                }
            }
        }
        // 向后
        this->seek(posMatch);
        pos = posMatch;
        int nextCount = 0;
        if ( pos < this->wordscount - 1 )
        {
            text = this->next();
            pos++;
            while ( StrMatch( word, text ) == 0 )
            {
                nextCount++;
                if ( pos < this->wordscount - 1 )
                {
                    text = this->next();
                    pos++;
                }
                else
                {
                    break;
                }
            }
        }

        count && ( *count = 1 + prevCount + nextCount );
        posMatch -= prevCount;
    }

    return posMatch;
}

// 多字节字符串里读取一个完整字符
inline static int __MbsOneChar( char const * & p, char ch[5], bool isUtf8 )
{
    int cch = 0;

    if ( isUtf8 )
    {
        ch[0] = *p++;
        cch++;
        if ( ( ch[0] & 0xF0 ) == 0b11110000 )
        {
            ch[1] = *p++;
            cch++;
            ch[2] = *p++;
            cch++;
            ch[3] = *p++;
            cch++;
        }
        else if ( ( ch[0] & 0xE0 ) == 0b11100000 )
        {
            ch[1] = *p++;
            cch++;
            ch[2] = *p++;
            cch++;
        }
        else if ( ( ch[0] & 0xC0 ) == 0b11000000 )
        {
            ch[1] = *p++;
            cch++;
        }
    }
    else // Local code
    {
        ch[0] = *p++;
        cch++;
        if ( ch[0] & 0x80 )
        {
            ch[1] = *p++;
            cch++;
        }
    }

    return cch;
}

int WordsLib::splitWords( AnsiString const & text, AnsiStringArray * arrWords ) const
{
    auto const * p = text.c_str();
    AnsiString str = "";
    AnsiString strMat = "";
    int haveMatch = -1;
    int cch = 0;
    int first = 0;
    int last = this->wordscount - 1;
    bool isUtf8 = this->encoding == "utf-8";
    while ( *p )
    {
        char ch[5] = { 0 };
        cch = __MbsOneChar( p, ch, isUtf8 );
        str += ch;

        int pos = -1, count = 0;
        pos = this->findEx( str, first, last, &count );
        if ( pos != -1 )
        {
            first = pos;
            last = pos + count - 1;
            haveMatch = pos;
            strMat = str;
            //p++;
        }
        else // 搜不到
        {
            if ( haveMatch != -1 )
            {
                // 退回去
                //p -= cch - 1;
                p -= cch;
                if ( this->at(haveMatch) == strMat )
                {
                    arrWords->push_back(strMat);
                }
                haveMatch = -1;
            }
            else
            {
                AnsiString ansiStr = "";
                while ( *p && ( IsAlpha(*p) || IsDigit(*p) || *p == '_' || *p == '.' ) )
                {
                    ansiStr += *p;
                    p++;
                }
                if ( ansiStr.empty() )
                {
                    //p++;
                }
                else
                {
                    arrWords->push_back(ansiStr);
                }
            }
            first = 0;
            last = this->wordscount - 1;
            str = "";
        }
    }

    // 结束循环再判断一次
    if ( haveMatch != -1 )
    {
        if ( this->at(haveMatch) == strMat )
        {
            arrWords->push_back(strMat);
        }
        haveMatch = -1;
    }

    return (int)arrWords->size();
}

int WordsLib::splitWords2( AnsiString const & text, AnsiStringArray * arrWords ) const
{
    auto const * p = text.c_str();
    auto const * q = p;
    AnsiString str = "";
    AnsiString strMat = "";
    int haveMatch = -1;
    int cch = 0;
    int first = 0;
    int last = this->wordscount - 1;
    bool isUtf8 = this->encoding == "utf-8";
    while ( *p )
    {
        char ch[5] = { 0 };
        cch = __MbsOneChar( p, ch, isUtf8 );
        str += ch;

        int pos = -1, count = 0;
        pos = this->findEx( str, first, last, &count );
        if ( pos != -1 )
        {
            first = pos;
            last = pos + count - 1;
            haveMatch = pos;
            strMat = str;
            //p++;
        }
        else // 搜不到
        {
            if ( haveMatch != -1 )
            {
                // 退回去
                //p -= cch - 1;
                p -= cch;
                if ( this->at(haveMatch) == strMat )
                {
                    AnsiString s( q, p - strMat.length() );
                    s = StrTrim(s);
                    if ( !s.empty() ) arrWords->push_back(s);
                    q = p;
                    arrWords->push_back(strMat);
                }
                haveMatch = -1;
            }
            else
            {
                AnsiString ansiStr = "";
                while ( *p && ( IsAlpha(*p) || IsDigit(*p) /*|| *p == '_' */|| *p == '.' ) )
                {
                    ansiStr += *p;
                    p++;
                }
                if ( ansiStr.empty() )
                {
                    //p++;
                }
                else
                {
                    AnsiString s( q, p - ansiStr.length() );
                    s = StrTrim(s);
                    if ( !s.empty() ) arrWords->push_back(s);
                    q = p;
                    arrWords->push_back(ansiStr);
                }
            }
            first = 0;
            last = this->wordscount - 1;
            str = "";
        }
    }

    // 结束循环再判断一次
    if ( haveMatch != -1 )
    {
        if ( this->at(haveMatch) == strMat )
        {
            AnsiString s( q, p - strMat.length() );
            s = StrTrim(s);
            if ( !s.empty() ) arrWords->push_back(s);
            q = p;
            arrWords->push_back(strMat);
        }
        haveMatch = -1;
    }

    return (int)arrWords->size();
}

int WordsLib::StrMatch( AnsiString const & str1, AnsiString const & str2 )
{
    size_t len1 = str1.length();
    size_t len2 = str2.length();
    size_t i;
    for ( i = 0; i < len1 && i < len2; i++ )
    {
        if ( (BYTE)str1[i] > (BYTE)str2[i] )
        {
            return 1;
        }
        else if ( (BYTE)str1[i] < (BYTE)str2[i] )
        {
            return -1;
        }
    }
    if ( i == len1 )
    {
        return 0;
    }
    else // i == len2
    {
        return 1;
    }
}

} // namespace winplus