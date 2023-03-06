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

WordsLib::WordsLib( String const & filename ) throw(WordsLibException)
:
_file(INVALID_HANDLE_VALUE),
    _fileMapping(NULL),
    filesize(0),
    _dataOffset(0),
    _p(NULL),
    _data(NULL),
    _dataSize(0),
    _curIndex(-1)
{
    _file = CreateFile( filename.c_str(), /*FILE_ALL_ACCESS*/GENERIC_READ, /*FILE_SHARE_READ*/0, NULL, OPEN_EXISTING, 0, NULL );
    if ( _file == INVALID_HANDLE_VALUE )
    {
        DWORD dwErr = GetLastError();
        throw WordsLibException( ( GetErrorStr(dwErr) + "词库文件`" + filename + "`未找到" ).c_str() );
    }

    filesize = (uint)GetFileSize( _file, NULL );
    _fileMapping = CreateFileMapping( _file, NULL, PAGE_READONLY, 0, 0, NULL );

    this->readHeaders();// 读取头信息

    if ( this->type != TEXT("equal") )
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
    AnsiString headers;
    headers.resize(i);
    CopyMemory( &headers[0], s, i );

    _dataOffset = i + 2; // skip "\r\n"

    UnmapViewOfFile(p);

    // 解析
    header.clear();
    StringStringMap tmpHeader;
    AnsiStringArray arrHeaders;
    int count = (int)StrSplit2( headers, "\r\n", &arrHeaders );

    for ( i = 0; i < count; ++i )
    {
        AnsiStringArray kv;
        int c = (int)StrSplit( arrHeaders[i], ":", &kv );
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
    encoding = isset( tmpHeader, "encoding" ) ? Mixed(tmpHeader["encoding"]) : Mixed("");
    _strlwr(&encoding[0]);
    if ( encoding == "utf-8" )
    {
        // utf-8编码
        auto it = tmpHeader.begin();
        for ( ; it != tmpHeader.end(); ++it )
        {
            header[Utf8ToString(it->first)] = Utf8ToString(it->second);
        }
    }
    else // 当作本地编码处理
    {
        auto it = tmpHeader.begin();
        for ( ; it != tmpHeader.end(); ++it )
        {
            header[LocalToString(it->first)] = LocalToString(it->second);
        }
    }

    // 赋值给变量
    name = this->getHeader( TEXT("name"), TEXT("") );
    desc = this->getHeader( TEXT("desc"), TEXT("") );
    type = this->getHeader( TEXT("type"), TEXT("equal") );
    compress = this->getHeader( TEXT("compress"), TEXT("none") );
    itemsize = Mixed( this->getHeader( TEXT("itemsize"), TEXT("32") ) );
    wordscount = Mixed( this->getHeader( TEXT("wordscount"), TEXT("0") ) );

}

String WordsLib::getHeader( String const & key, String const & defval )
{
    return isset( header, key ) && header[key] != TEXT("") ? header[key] : defval;
}

String WordsLib::prev() const
{
    if ( _curIndex > -1 )
        _curIndex--;

    return this->at(_curIndex);
}

String WordsLib::next() const
{
    if ( _curIndex < wordscount )
        _curIndex++;

    return this->at(_curIndex);
}

String WordsLib::at( int i ) const
{
    if ( i < 0 || i > wordscount - 1 )
    {
        return TEXT("");
    }
    String word;
    word.resize(itemsize);
    CopyMemory( &word[0], _data + i * itemsize, itemsize );
    return word.c_str();
}

String WordsLib::seek( int index ) const
{
    index = index < -1 ? -1 : index;
    index = index > wordscount ? wordscount : index;
    _curIndex = index;
    return this->at(_curIndex);
}

int WordsLib::find( String const & word, int first, int last ) const
{
    while ( first <= last )
    {
        int mid = first + (last - first) / 2;
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

int WordsLib::findEx( String const & word, int first, int last, int * count ) const
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
        String text;
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
        if ( pos < wordscount - 1 )
        {
            text = this->next();
            pos++;
            while ( StrMatch( word, text ) == 0 )
            {
                nextCount++;
                if ( pos < wordscount - 1 )
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

int WordsLib::splitWords( String const & text, StringArray * arrWords ) const
{
    TCHAR const * p = text.c_str();
    String str = TEXT("");
    String strMat = TEXT("");
    int haveMatch = -1;
    int first, last;
    int cch = 0, i = 0;
    first = 0;
    last = wordscount - 1;
    while ( p && *p )
    {
        TCHAR ch[4] = {0};
        ch[0] = *p;
        cch = 1;
#ifndef UNICODE
        if ( ch[0] & 0x80 )
        {
            p++;
            ch[1] = *p;
            cch = 2;
        }
#endif
        str += ch;

        int pos = -1, count = 0;
        pos = this->findEx( str, first, last, &count );
        if ( pos != -1 )
        {
            first = pos;
            last = pos + count - 1;
            haveMatch = pos;
            strMat = str;
            p++;
        }
        else // 搜不到
        {
            if ( haveMatch != -1 )
            {
                if ( this->at(haveMatch) == strMat )
                {
                    arrWords->push_back(strMat);
                }
                haveMatch = -1;
                // 退回去
                p -= cch - 1;
            }
            else
            {
                String ansiStr = TEXT("");
                while ( *p && ( IsAlpha(*p) || IsDigit(*p) || *p == '_' || *p == '.' ) )
                {
                    ansiStr += *p;
                    p++;
                }
                if ( ansiStr.empty() )
                    p++;
                else
                    arrWords->push_back(ansiStr);
            }
            first = 0;
            last = wordscount - 1;
            str = TEXT("");
        }
        i++;
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

int WordsLib::StrMatch( String const & str1, String const & str2 )
{
    int len1 = (int)str1.length();
    int len2 = (int)str2.length();
    int i;
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