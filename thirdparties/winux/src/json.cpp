#define MIXED_REF_NO_EXCEPTION // Mixed::ref*()不抛异常
#include "utilities.hpp"
#include "json.hpp"
#include "strings.hpp"
#include "encoding.hpp"
#include <vector>

#if defined(OS_WIN) // IS_WINDOWS
    #include <mbstring.h>
    #include <tchar.h>
#else
    #include <wchar.h>
    #ifdef UNICODE
    #define _vsntprintf vswprintf
    #define _tcsstr wcsstr
    #define _tcstol wcstol
    #define _tcstoul wcstoul
    #else
    #define _vsntprintf vsnprintf
    #define _tcsstr strstr
    #define _tcstol strtol
    #define _tcstoul strtoul
    #endif
#endif

namespace winux
{

enum JsonParseContext
{
    jsonData,
    jsonNumber,
    jsonString, jsonStrAntiSlashes,
    jsonArray, jsonArrElem,
    jsonObject, jsonObjAttr, jsonObjAttrKey, jsonObjAttrVal,
    jsonIdentifier,
};

bool JsonParseData( std::vector<JsonParseContext> & jpc, String const & json, int & i, Mixed * val );

#include "is_x_funcs.inl"

inline static tchar NumberStringToChar( const tchar * number, int base )
{
    tchar * endptr;
    return (tchar)_tcstol( number, &endptr, base );
}

inline static winux::ulong NumberStringToNumber( const tchar * number, int base )
{
    tchar * endptr;
    return (winux::ulong)_tcstoul( number, &endptr, base );
}

// 解析数字
bool JsonParseNumber( std::vector<JsonParseContext> & jpc, String const & json, int & i, Mixed * numVal )
{
    String numStr;
    bool isFloat = false; // check include '.'
    bool isNegative = false; // check include '-'
    int bits = 0;
    while ( i < (int)json.length() )
    {
        String::value_type ch = json[i];
        if ( IsDigit(ch) )
        {
            numStr += ch;
            ++bits;
            ++i;
        }
        else if ( ch == '-' && numStr.empty() )
        {
            isNegative = true;
            numStr += '-';
            ++i;
        }
        else if ( ch == '.' && numStr.find('.') == String::npos )
        {
            isFloat = true;
            numStr += '.';
            ++i;
        }
        else
        {
            break;
        }
    }

    if ( isFloat )
    {
        numVal->free();
        numVal->_type = Mixed::MT_DOUBLE;
        ParseDouble( numStr, &numVal->_dblVal );
    }
    else
    {
        if ( bits > 9 ) // 大于9位数就当作int64
        {
            numVal->free();
            numVal->_type = Mixed::MT_INT64;
            ParseUInt64( numStr, (uint64 *)&numVal->_i64Val );
        }
        else
        {
            numVal->free();
            numVal->_type = Mixed::MT_INT;
            ParseULong( numStr, (ulong *)&numVal->_iVal );
        }
    }
    return true;

}

thread_local bool __byteOrderForUtf16 = true;
WINUX_FUNC_IMPL(bool) JsonSetByteOrderForUtf16( bool isLittleEndian )
{
    auto old = __byteOrderForUtf16;
    __byteOrderForUtf16 = isLittleEndian;
    return old;
}

#if defined(_UNICODE) || defined(UNICODE)
thread_local String __convertToCharsetForUtf16 = IsBigEndian() ? $T("UTF-16BE") : $T("UTF-16LE");
#else
thread_local String __convertToCharsetForUtf16 = $T("");
#endif
WINUX_FUNC_IMPL(String) JsonSetConvertToCharsetForUtf16( String const & charset )
{
    auto old = __convertToCharsetForUtf16;
    __convertToCharsetForUtf16 = charset;
    return old;
}

// 解析转义字符
bool JsonParseStrAntiSlashes( std::vector<JsonParseContext> & jpc, String const & json, int & i, String * str )
{
    ++i; // skip '\'
    while ( i < (int)json.length() )
    {
        String::value_type ch = json[i];
        if ( ch == 'a' )
        {
            *str += '\a';
            ++i;
            break;
        }
        else if ( ch == 'b' )
        {
            *str += '\b';
            ++i;
            break;
        }
        else if ( ch == 't' )
        {
            *str += '\t';
            ++i;
            break;
        }
        else if ( ch == 'n' )
        {
            *str += '\n';
            ++i;
            break;
        }
        else if ( ch == 'v' )
        {
            *str += '\v';
            ++i;
            break;
        }
        else if ( ch == 'f' )
        {
            *str += '\f';
            ++i;
            break;
        }
        else if ( ch == 'r' )
        {
            *str += '\r';
            ++i;
            break;
        }
        else if ( IsOct(ch) )
        {
            String octStr;
            for ( ; i < (int)json.length(); ++i )
            {
                ch = json[i];
                if ( IsOct(ch) && octStr.length() < 3 )
                {
                    octStr += ch;
                }
                else
                {
                    break;
                }
            }

            *str += NumberStringToChar( octStr.c_str(), 8 );

            break;
        }
        else if ( ( ch | 0x20 ) == 'x' ) // 解析\xHH
        {
            ++i; // skip 'x'
            String hexStr;
            for ( ; i < (int)json.length(); ++i )
            {
                ch = json[i];
                if ( IsHex(ch) && hexStr.length() < 2 )
                {
                    hexStr += ch;
                }
                else
                {
                    break;
                }
            }

            *str += NumberStringToChar( hexStr.c_str(), 16 );

            break;
        }
        else if ( ( ch | 0x20 ) == 'u' ) // 解析\uHHHH
        {
            ++i; // skip 'u'
            String hexStr;
            for ( ; i < (int)json.length(); ++i )
            {
                ch = json[i];
                if ( IsHex(ch) && hexStr.length() < 4 )
                {
                    hexStr += ch;
                }
                else
                {
                    break;
                }
            }

            // 编码处理
            winux::uint16 code0 = (winux::uint16)NumberStringToNumber( hexStr.c_str(), 16 );
            UnicodeString16 chars;
            chars += (UnicodeString16::value_type)code0;
            if ( code0 >= 0xD800 && code0 <= 0xDBFF ) // 是UTF16代理对，尝试读取下一'\uHHHH'。若失败则退回原位
            {
                int saveI = i;
                if ( i < (int)json.length() )
                {
                    if ( json[i] == '\\' )
                    {
                        ++i; // skip '\\'
                        if ( i < (int)json.length() )
                        {
                            if ( ( json[i] | 0x20 ) == 'u' )
                            {
                                ++i; // skip 'u'
                                String hexStr;
                                for ( ; i < (int)json.length(); ++i )
                                {
                                    ch = json[i];
                                    if ( IsHex(ch) && hexStr.length() < 4 )
                                    {
                                        hexStr += ch;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                                winux::uint16 code1 = (winux::uint16)NumberStringToNumber( hexStr.c_str(), 16 );
                                if ( code1 >= 0xDC00 && code1 <= 0xDFFF )
                                {
                                    chars += (UnicodeString16::value_type)code1;
                                }
                                else
                                {
                                    i = saveI;
                                }
                            }
                            else
                            {
                                i = saveI;
                            }
                        }
                        else
                        {
                            i = saveI;
                        }
                    }
                    else
                    {
                        i = saveI;
                    }
                }
            }
            AnsiString convertToCharsetForUtf16 = StringToLocal(__convertToCharsetForUtf16);
            Conv conv( ( __byteOrderForUtf16 ? "UTF-16LE" : "UTF-16BE" ), convertToCharsetForUtf16 );
        #if defined(_UNICODE) || defined(UNICODE)
            *str += UnicodeConverter( conv.convert< UnicodeString16, UnicodeString16 >(chars) ).toUnicode();
        #else
            *str += conv.convert< AnsiString, UnicodeString16 >(chars);
        #endif
            break;
        }
        else // 其余加\的字符都照原样输出
        {
            *str += ch;
            ++i;
            break;
        }
    }
    return true;
}

// 解析字符串
bool JsonParseString( std::vector<JsonParseContext> & jpc, String const & json, int & i, Mixed * strVal )
{
    String::value_type quote = json[i]; // 记下是什么引号

    strVal->assign( $T("") ); // 初始化字符串

    String * str = &strVal->ref<String>();
    ++i; // skip quote

    while ( i < (int)json.length() )
    {
        String::value_type ch = json[i];
        if ( ch == quote )
        {
            ++i;
            break;
        }
        else if ( ch == '\\' ) // 进入转义字符解析
        {
            jpc.push_back(jsonStrAntiSlashes);
            JsonParseStrAntiSlashes( jpc, json, i, str );
            jpc.pop_back();
        }
        else
        {
            *str += ch;
            ++i;
        }
    }
    return true;
}

// 解析数组
bool JsonParseArray( std::vector<JsonParseContext> & jpc, String const & json, int & i, Mixed * arrVal )
{
    ++i; // skip '['

    arrVal->createArray(); // 创建数组

    jpc.push_back(jsonArrElem);

    Mixed elem; // 解析到的数据
    bool hasElem = false; // 解析到元素

    while ( i < (int)json.length() )
    {
        String::value_type ch = json[i];
        if ( IsSpace(ch) )
        {
            ++i; // 跳过空白字符
        }
        else if ( ch == ',' ) // 另一个数组元素
        {
            ++i; // skip ','
            if ( hasElem )
            {
                arrVal->add(elem);
                hasElem = false;
            }
        }
        else if ( ch == ']' ) // 结束
        {
            ++i;
            break;
        }
        else
        {
            if ( !hasElem )
            {
                JsonParseData( jpc, json, i, &elem );
                hasElem = true;
            }
            else // 跳过多余数据
            {
                ++i;
            }
        }
    }

    if ( hasElem )
    {
        arrVal->add(elem);
        hasElem = false;
    }

    jpc.pop_back();

    return true;
}

// 解析一个对象(map)
bool JsonParseObject( std::vector<JsonParseContext> & jpc, String const & json, int & i, Mixed * objVal )
{
    ++i; // skip '{'
    objVal->createCollection();
    jpc.push_back(jsonObjAttr);

    Mixed key, val;
    bool hasKey = false, hasVal = false;
    bool currIsKey = true;

    while ( i < (int)json.length() )
    {
        String::value_type ch = json[i];
        if ( IsSpace(ch) )
        {
            ++i; // 跳过空白字符
        }
        else if ( ch == ',' )
        {
            ++i;
            if ( hasKey && hasVal )
            {
                (*objVal)[key] = val;
            }
            else if ( hasKey && !hasVal )
            {
                (*objVal)[key] = mxNull;
            }

            hasKey = hasVal = false;
            currIsKey = true;
        }
        else if ( ch == ':' )
        {
            ++i;
            currIsKey = false;
        }
        else if ( ch == '}' )
        {
            ++i;
            break;
        }
        else
        {
            if ( currIsKey )
            {
                jpc.push_back(jsonObjAttrKey);
                if ( ch == '\"' || ch == '\'' ) // 是字符串
                {
                    JsonParseString( jpc, json, i, &key );
                }
                else
                {
                    key.assign( $T("") );
                    for ( ; i < (int)json.length(); ++i )
                    {
                        ch = json[i];
                        if ( IsSpace(ch) || ch == ':' || ch == ',' || ch == '}' )
                        {
                            break;
                        }
                        else
                        {
                            key.ref<String>() += ch;
                        }
                    }
                }
                jpc.pop_back();

                hasKey = true;
            }
            else
            {
                jpc.push_back(jsonObjAttrVal);
                JsonParseData( jpc, json, i, &val );
                jpc.pop_back();
                hasVal = true;
            }
        }
    }

    if ( hasKey && hasVal )
    {
        (*objVal)[key] = val;
    }
    else if ( hasKey && !hasVal )
    {
        (*objVal)[key] = mxNull;
    }
    hasKey = hasVal = false;
    currIsKey = true;

    jpc.pop_back();

    return true;
}

// 解析一个标识符，这个标识符可能是个常量或者未定义
bool JsonParseIdentifier( std::vector<JsonParseContext> & jpc, String const & json, int & i, Mixed * val )
{
    String identifier;
    while ( i < (int)json.length() )
    {
        String::value_type ch = json[i];
        if ( IsWord(ch) )
        {
            identifier += ch;
            ++i;
        }
        else
        {
            break;
        }
    }

    // 常量辨别
    if ( identifier == $T("false") )
    {
        *val = false;
    }
    else if ( identifier == $T("true") )
    {
        *val = true;
    }
    else // 其他未定义常量通通设定为MT_NULL
    {
        val->free();
    }

    return true;
}

// 解析一个json数据
bool JsonParseData( std::vector<JsonParseContext> & jpc, String const & json, int & i, Mixed * val )
{
    val->free();

    while ( i < (int)json.length() )
    {
        String::value_type ch = json[i];
        if ( IsSpace(ch) ) // 是空白字符，跳过
        {
            ++i;
        }
        else if ( IsDigit(ch) || ch == '-' || ch == '.' ) // 可能是数字，进入数字解析
        {
            jpc.push_back(jsonNumber);
            JsonParseNumber( jpc, json, i, val );
            jpc.pop_back();
            return true;
            break;
        }
        else if ( ch == '\"' || ch == '\'' ) // 是字符串，进入字符串解析
        {
            jpc.push_back(jsonString);
            JsonParseString( jpc, json, i, val );
            jpc.pop_back();
            return true;
            break;
        }
        else if ( ch == '[' ) // 进入数组解析
        {
            jpc.push_back(jsonArray);
            JsonParseArray( jpc, json, i, val );
            jpc.pop_back();
            return true;
            break;
        }
        else if ( ch == '{' ) // 进入对象解析
        {
            jpc.push_back(jsonObject);
            JsonParseObject( jpc, json, i, val );
            jpc.pop_back();
            return true;
            break;
        }
        else if ( IsWord(ch) ) // 进入标识符解析
        {
            jpc.push_back(jsonIdentifier);
            JsonParseIdentifier( jpc, json, i, val );
            jpc.pop_back();
            return true;
            break;
        }
        else // 其他字符
        {
        //OTHER_CHARS:
            if ( jpc.back() == jsonArrElem )
            {
                if ( ch == ',' || ch == ']' )
                {
                    break;
                }
                else
                {
                    ++i;
                }
            }
            else if ( jpc.back() == jsonObjAttr )
            {
                if ( ch == ',' || ch == ':' || ch == '}' )
                {
                    break;
                }
                else
                {
                    ++i;
                }
            }
            else if ( jpc.back() == jsonObjAttrVal )
            {
                if ( ch == ',' || ch == '}' )
                {
                    break;
                }
                else
                {
                    ++i;
                }
            }
            else
            {
                ++i;
            }
        }
    }
    return false;
}

WINUX_FUNC_IMPL(bool) ParseJson( String const & json, Mixed * val )
{
    std::vector<JsonParseContext> jpc; // 解析场景
    jpc.push_back(jsonData);
    int i = 0; // 起始位置
    return JsonParseData( jpc, json, i, val );
}

WINUX_FUNC_IMPL(Mixed) Json( String const & json )
{
    Mixed v;
    ParseJson( json, &v );
    return v;
}

// JSON键名是否用字符串形式表示
template < typename _ChTy >
inline static bool IsKeyNameUseString( XString<_ChTy> const & key )
{
    if ( key.empty() ) return true;
    for ( auto it = key.begin(); it != key.end(); ++it )
    {
        if ( !( IsDigit(*it) || IsWord(*it) ) )
        {
            return true;
        }
    }
    return false;
}

template < typename _ChTy >
inline static XString<_ChTy> Impl_Recursive_MixedToJsonEx( int level, Mixed const & v, bool autoKeyQuotes, XString<_ChTy> const & spacer, XString<_ChTy> const & newline )
{
    XString<_ChTy> s;
    switch ( v._type )
    {
    case Mixed::MT_NULL:
        s += Literal<_ChTy>::nullValueStr;
        break;
    case Mixed::MT_BOOLEAN:
        s += v._boolVal ? Literal<_ChTy>::boolTrueStr : Literal<_ChTy>::boolFalseStr;
        break;
    case Mixed::MT_CHAR:
        s += Literal<_ChTy>::aposStr + XString<_ChTy>( 1, (_ChTy)v._chVal ) + Literal<_ChTy>::aposStr;
        break;
    case Mixed::MT_ANSI:
    case Mixed::MT_UNICODE:
        s += Literal<_ChTy>::quoteStr + AddCSlashes<_ChTy>( v.toString<_ChTy>() ) + Literal<_ChTy>::quoteStr;
        break;
    case Mixed::MT_BINARY:
        s += Literal<_ChTy>::quoteStr + Base64EncodeBuffer<_ChTy>(*v._pBuf) + Literal<_ChTy>::quoteStr;
        break;
    case Mixed::MT_ARRAY:
        {
            s += Literal<_ChTy>::LSquareBRKStr + ( newline.empty() || v._pArr->size() == 0 ? Literal<_ChTy>::spaceStr : newline );
            for ( size_t i = 0; i < v._pArr->size(); ++i )
            {
                if ( i != 0 ) s += Literal<_ChTy>::commaStr + ( newline.empty() ? Literal<_ChTy>::spaceStr : newline );
                s += ( spacer.empty() ? Literal<_ChTy>::nulStr : StrMultiple<_ChTy>( spacer, level + 1 ) ) + Impl_Recursive_MixedToJsonEx( level + 1, v[i], autoKeyQuotes, spacer, newline );
            }
            s += ( newline.empty() || v._pArr->size() == 0 ? Literal<_ChTy>::spaceStr : newline ) + ( spacer.empty() || v._pArr->size() == 0 ? Literal<_ChTy>::nulStr : StrMultiple<_ChTy>( spacer, level ) ) + Literal<_ChTy>::RSquareBRKStr;
        }
        break;
    case Mixed::MT_COLLECTION:
        {
            s += Literal<_ChTy>::LCurlyBRKStr + ( newline.empty() || v._pColl->getCount() == 0 ? Literal<_ChTy>::spaceStr : newline );
            for ( auto it = v._pColl->refKeysArray().begin(); it != v._pColl->refKeysArray().end(); ++it )
            {
                if ( it != v._pColl->refKeysArray().begin() ) s += Literal<_ChTy>::commaStr + ( newline.empty() ? Literal<_ChTy>::spaceStr : newline );
                // key
                if ( it->isString() )
                {
                    XString<_ChTy> k = *it;
                    s += ( spacer.empty() ? Literal<_ChTy>::nulStr : StrMultiple<_ChTy>( spacer, level + 1 ) ) + ( autoKeyQuotes ? ( IsKeyNameUseString(k) ? Literal<_ChTy>::quoteStr + AddCSlashes<_ChTy>(k) + Literal<_ChTy>::quoteStr : k ) : ( Literal<_ChTy>::quoteStr + AddCSlashes<_ChTy>(k) + Literal<_ChTy>::quoteStr ) );
                }
                else
                {
                    s += ( spacer.empty() ? Literal<_ChTy>::nulStr : StrMultiple<_ChTy>( spacer, level + 1 ) ) + Impl_Recursive_MixedToJsonEx( level + 1, *it, autoKeyQuotes, spacer, newline );
                }
                s += Literal<_ChTy>::colonStr + XString<_ChTy>( newline.empty() ? Literal<_ChTy>::nulStr : Literal<_ChTy>::spaceStr );
                // value
                s += Impl_Recursive_MixedToJsonEx( level + 1, v._pColl->at(*it), autoKeyQuotes, spacer, newline );
            }
            s += ( newline.empty() || v._pColl->getCount() == 0 ? Literal<_ChTy>::spaceStr : newline ) + ( spacer.empty() || v._pColl->getCount() == 0 ? Literal<_ChTy>::nulStr : StrMultiple<_ChTy>( spacer, level ) ) + Literal<_ChTy>::RCurlyBRKStr;
        }
        break;
    default:
        s += v.toString<_ChTy>();
        break;
    }
    return s;
}

WINUX_FUNC_IMPL(AnsiString) MixedToJsonExA( Mixed const & v, bool autoKeyQuotes, AnsiString const & spacer, AnsiString const & newline )
{
    return Impl_Recursive_MixedToJsonEx( 0, v, autoKeyQuotes, spacer, newline );
}

WINUX_FUNC_IMPL(UnicodeString) MixedToJsonExW( Mixed const & v, bool autoKeyQuotes, UnicodeString const & spacer, UnicodeString const & newline )
{
    return Impl_Recursive_MixedToJsonEx( 0, v, autoKeyQuotes, spacer, newline );
}

WINUX_FUNC_IMPL(AnsiString) MixedToJsonA( Mixed const & v, bool autoKeyQuotes )
{
    AnsiString s;
    switch ( v._type )
    {
    case Mixed::MT_NULL:
        s += "null";
        break;
    case Mixed::MT_BOOLEAN:
        s += v._boolVal ? "true" : "false";
        break;
    case Mixed::MT_CHAR:
        s += "'" + AnsiString( 1, v._chVal ) + "'";
        break;
    case Mixed::MT_ANSI:
    case Mixed::MT_UNICODE:
        s += "\"" + AddCSlashes<char>( v.toAnsi() ) + "\"";
        break;
    case Mixed::MT_BINARY:
        s += "\"" + Base64EncodeBuffer<char>(*v._pBuf) + "\"";
        break;
    case Mixed::MT_ARRAY:
        {
            s += "[ ";
            for ( size_t i = 0; i < v.getCount(); ++i )
            {
                if ( i != 0 ) s += ", ";
                s += MixedToJsonA( v[i] , autoKeyQuotes );
            }
            s += " ]";
        }
        break;
    case Mixed::MT_COLLECTION:
        {
            s += "{ ";
            for ( auto it = v._pColl->refKeysArray().begin(); it != v._pColl->refKeysArray().end(); ++it )
            {
                if ( it != v._pColl->refKeysArray().begin() ) s += ", ";
                // key
                if ( it->isString() )
                {
                    AnsiString k = *it;
                    s += autoKeyQuotes ? ( IsKeyNameUseString(k) ? "\"" + AddCSlashes<char>(k) + "\"" : k ) : ( "\"" + AddCSlashes<char>(k) + "\"" );
                }
                else
                {
                    s += MixedToJsonA( *it, autoKeyQuotes );
                }
                s += ":";
                // value
                s += MixedToJsonA( v._pColl->at(*it), autoKeyQuotes );
            }
            s += " }";
        }
        break;
    default:
        s += v.toAnsi();
        break;
    }
    return s;
}

WINUX_FUNC_IMPL(UnicodeString) MixedToJsonW( Mixed const & v, bool autoKeyQuotes )
{
    UnicodeString s;
    switch ( v._type )
    {
    case Mixed::MT_NULL:
        s += L"null";
        break;
    case Mixed::MT_BOOLEAN:
        s += v._boolVal ? L"true" : L"false";
        break;
    case Mixed::MT_CHAR:
        s += L"'" + UnicodeString( 1, v._chVal ) + L"'";
        break;
    case Mixed::MT_ANSI:
    case Mixed::MT_UNICODE:
        s += L"\"" + AddCSlashes<wchar>( v.toUnicode() ) + L"\"";
        break;
    case Mixed::MT_BINARY:
        s += L"\"" + Base64EncodeBuffer<wchar>(*v._pBuf) + L"\"";
        break;
    case Mixed::MT_ARRAY:
        {
            s += L"[ ";
            for ( size_t i = 0; i < v.getCount(); ++i )
            {
                if ( i != 0 ) s += L", ";
                s += MixedToJsonW( v[i], autoKeyQuotes );
            }
            s += L" ]";
        }
        break;
    case Mixed::MT_COLLECTION:
        {
            s += L"{ ";
            for ( auto it = v._pColl->refKeysArray().begin(); it != v._pColl->refKeysArray().end(); ++it )
            {
                if ( it != v._pColl->refKeysArray().begin() ) s += L", ";
                // key
                if ( it->isString() )
                {
                    UnicodeString k = *it;
                    s += autoKeyQuotes ? ( IsKeyNameUseString(k) ? L"\"" + AddCSlashes<wchar>(k) + L"\"" : k ): ( L"\"" + AddCSlashes<wchar>(k) + L"\"" );
                }
                else
                {
                    s += MixedToJsonW( *it, autoKeyQuotes );
                }
                s += L":";
                // value
                s += MixedToJsonW( v._pColl->at(*it), autoKeyQuotes );
            }
            s += L" }";
        }
        break;
    default:
        s += v.toUnicode();
        break;
    }
    return s;
}

} // namespace winux
