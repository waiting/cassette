#include "utilities.hpp"
#include "encoding.hpp"
#include "strings.hpp"
#include "md5.hpp"
#include "mystdint.h"
#include "sha1.h"
#include "sha2.h"

namespace winux
{
#include "is_x_funcs.inl"

//static String const __UrlEncodeTable[] = {
//    "%00", "%01", "%02", "%03", "%04", "%05", "%06", "%07", "%08", "%09", "%0A", "%0B", "%0C", "%0D", "%0E", "%0F",
//    "%10", "%11", "%12", "%13", "%14", "%15", "%16", "%17", "%18", "%19", "%1A", "%1B", "%1C", "%1D", "%1E", "%1F",
//    /* */"+", /*!*/"%21", /*"*/"%22", /*#*/"%23", /*$*/"%24", /*%*/"%25", /*&*/"%26", /*'*/"%27", /*(*/"%28", /*)*/"%29", /***/"%2A", /*+*/"%2B", /*,*/"%2C", "-", ".", /* / */"%2F",
//    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", /*:*/"%3A", /*;*/"%3B", /*<*/"%3C", /*=*/"%3D", /*>*/"%3E", /*?*/"%3F",
//    /*@*/"%40", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
//    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", /*[*/"%5B", /* \ */"%5C", /*]*/"%5D", /*^*/"%5E", "_",
//    /*`*/ "%60", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
//    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", /*{*/"%7B", /*|*/"%7C", /*}*/"%7D", /*~*/"%7E", "%7F",
//    "%80", "%81", "%82", "%83", "%84", "%85", "%86", "%87", "%88", "%89", "%8A", "%8B", "%8C", "%8D", "%8E", "%8F",
//    "%90", "%91", "%92", "%93", "%94", "%95", "%96", "%97", "%98", "%99", "%9A", "%9B", "%9C", "%9D", "%9E", "%9F",
//    "%A0", "%A1", "%A2", "%A3", "%A4", "%A5", "%A6", "%A7", "%A8", "%A9", "%AA", "%AB", "%AC", "%AD", "%AE", "%AF",
//    "%B0", "%B1", "%B2", "%B3", "%B4", "%B5", "%B6", "%B7", "%B8", "%B9", "%BA", "%BB", "%BC", "%BD", "%BE", "%BF",
//    "%C0", "%C1", "%C2", "%C3", "%C4", "%C5", "%C6", "%C7", "%C8", "%C9", "%CA", "%CB", "%CC", "%CD", "%CE", "%CF",
//    "%D0", "%D1", "%D2", "%D3", "%D4", "%D5", "%D6", "%D7", "%D8", "%D9", "%DA", "%DB", "%DC", "%DD", "%DE", "%DF",
//    "%E0", "%E1", "%E2", "%E3", "%E4", "%E5", "%E6", "%E7", "%E8", "%E9", "%EA", "%EB", "%EC", "%ED", "%EE", "%EF",
//    "%F0", "%F1", "%F2", "%F3", "%F4", "%F5", "%F6", "%F7", "%F8", "%F9", "%FA", "%FB", "%FC", "%FD", "%FE", "%FF",
//};
//
//static String const __UriComponentEncodeTable[] = {
//    "%00", "%01", "%02", "%03", "%04", "%05", "%06", "%07", "%08", "%09", "%0A", "%0B", "%0C", "%0D", "%0E", "%0F",
//    "%10", "%11", "%12", "%13", "%14", "%15", "%16", "%17", "%18", "%19", "%1A", "%1B", "%1C", "%1D", "%1E", "%1F",
//    /* */"%20", /*!*/"%21", /*"*/"%22", /*#*/"%23", /*$*/"%24", /*%*/"%25", /*&*/"%26", /*'*/"%27", /*(*/"%28", /*)*/"%29", /***/"%2A", /*+*/"%2B", /*,*/"%2C", "-", ".", /* / */"%2F",
//    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", /*:*/"%3A", /*;*/"%3B", /*<*/"%3C", /*=*/"%3D", /*>*/"%3E", /*?*/"%3F",
//    /*@*/"%40", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
//    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", /*[*/"%5B", /* \ */"%5C", /*]*/"%5D", /*^*/"%5E", "_",
//    /*`*/ "%60", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
//    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", /*{*/"%7B", /*|*/"%7C", /*}*/"%7D", "~", "%7F",
//    "%80", "%81", "%82", "%83", "%84", "%85", "%86", "%87", "%88", "%89", "%8A", "%8B", "%8C", "%8D", "%8E", "%8F",
//    "%90", "%91", "%92", "%93", "%94", "%95", "%96", "%97", "%98", "%99", "%9A", "%9B", "%9C", "%9D", "%9E", "%9F",
//    "%A0", "%A1", "%A2", "%A3", "%A4", "%A5", "%A6", "%A7", "%A8", "%A9", "%AA", "%AB", "%AC", "%AD", "%AE", "%AF",
//    "%B0", "%B1", "%B2", "%B3", "%B4", "%B5", "%B6", "%B7", "%B8", "%B9", "%BA", "%BB", "%BC", "%BD", "%BE", "%BF",
//    "%C0", "%C1", "%C2", "%C3", "%C4", "%C5", "%C6", "%C7", "%C8", "%C9", "%CA", "%CB", "%CC", "%CD", "%CE", "%CF",
//    "%D0", "%D1", "%D2", "%D3", "%D4", "%D5", "%D6", "%D7", "%D8", "%D9", "%DA", "%DB", "%DC", "%DD", "%DE", "%DF",
//    "%E0", "%E1", "%E2", "%E3", "%E4", "%E5", "%E6", "%E7", "%E8", "%E9", "%EA", "%EB", "%EC", "%ED", "%EE", "%EF",
//    "%F0", "%F1", "%F2", "%F3", "%F4", "%F5", "%F6", "%F7", "%F8", "%F9", "%FA", "%FB", "%FC", "%FD", "%FE", "%FF",
//};


// UrlEncode ----------------------------------------------------------------------------------
template < typename _ChTy >
inline static XString<_ChTy> Impl_UrlEncode( void const * buf, size_t size )
{
    XString<_ChTy> r;
    r.reserve( size_t(size * 2.718) );
    _ChTy sz[4] = { '%' };
    byte const * data = (byte const *)buf;
    for ( size_t i = 0; i < size; ++i )
    {
        _ChTy const c = data[i];
        if (
            c == '-' || c == '_' || c == '.' ||
            ( c >= '0' && c <= '9' ) ||
            ( c >= 'A' && c <= 'Z' ) ||
            ( c >= 'a' && c <= 'z' )
        )
        {
            r += c;
        }
        else if ( c == ' ' )
        {
            r += '+';
        }
        else
        {
            unsigned char tmp;
            tmp = ( c & 0xF );
            sz[2] = tmp < 10 ? tmp + '0' : tmp - 10 + 'A';
            tmp = ( ( c >> 4 ) & 0xF );
            sz[1] = tmp < 10 ? tmp + '0' : tmp - 10 + 'A';
            r += sz;
        }
    }
    return r;
}

template <>
WINUX_FUNC_IMPL(XString<char>) UrlEncode( void const * buf, size_t size )
{
    return Impl_UrlEncode<char>( buf, size );
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) UrlEncode( void const * buf, size_t size )
{
    return Impl_UrlEncode<wchar>( buf, size );
}

template <>
WINUX_FUNC_IMPL(XString<char16>) UrlEncode( void const * buf, size_t size )
{
    return Impl_UrlEncode<char16>( buf, size );
}

template <>
WINUX_FUNC_IMPL(XString<char32>) UrlEncode( void const * buf, size_t size )
{
    return Impl_UrlEncode<char32>( buf, size );
}


// UrlDecode ----------------------------------------------------------------------------------
template < typename _ChTy, typename _Fx >
inline static void Impl_UrlDecode( XString<_ChTy> const & encodedStr, _Fx && fnAppend )
{
    AnsiString::value_type low, hig;
    for ( auto it = encodedStr.begin(); it != encodedStr.end(); )
    {
        if ( *it == '+' )
        {
            fnAppend(' ');
            ++it;
        }
        else if ( *it != '%' )
        {
            fnAppend( (AnsiString::value_type)*it );
            ++it;
        }
        else // *it == '%'
        {
            ++it; // skip '%'
            if ( it != encodedStr.end() )
            {
                hig = (AnsiString::value_type)*it;
                bool isHex = IsHex(hig);
                if ( isHex )
                {
                    hig = hig > '9' ? hig - 'A' + 10 : hig - '0';
                }
                else // is not a hex value.
                {
                    fnAppend('%'); // 把跳过的'%'也加上
                    fnAppend(hig);
                }

                ++it;
                if (  it != encodedStr.end() )
                {
                    low = (AnsiString::value_type)*it;

                    if ( isHex )
                    {
                        low = low > '9' ? low - 'A' + 10 : low - '0' ;
                        fnAppend( (AnsiString::value_type)( low | ( hig << 4 ) ) );
                    }
                    else // not hex
                    {
                        fnAppend(low);
                    }

                    ++it;
                }
            }
            else // 说明已经是最后一个字符
            {
                fnAppend('%');
            }
        }
    }
}

template <>
WINUX_FUNC_IMPL(AnsiString) UrlDecode( XString<char> const & encodedStr )
{
    AnsiString res;
    res.reserve( encodedStr.length() * 4 / 5 );
    Impl_UrlDecode( encodedStr, [&res] ( char ch ) { res += ch; } );
    return res;
}

template <>
WINUX_FUNC_IMPL(AnsiString) UrlDecode( XString<wchar> const & encodedStr )
{
    AnsiString res;
    res.reserve( encodedStr.length() * 4 / 5 );
    Impl_UrlDecode( encodedStr, [&res] ( char ch ) { res += ch; } );
    return res;
}

template <>
WINUX_FUNC_IMPL(AnsiString) UrlDecode( XString<char16> const & encodedStr )
{
    AnsiString res;
    res.reserve( encodedStr.length() * 4 / 5 );
    Impl_UrlDecode( encodedStr, [&res] ( char ch ) { res += ch; } );
    return res;
}

template <>
WINUX_FUNC_IMPL(AnsiString) UrlDecode( XString<char32> const & encodedStr )
{
    AnsiString res;
    res.reserve( encodedStr.length() * 4 / 5 );
    Impl_UrlDecode( encodedStr, [&res] ( char ch ) { res += ch; } );
    return res;
}

template <>
WINUX_FUNC_IMPL(Buffer) UrlDecodeBuffer( XString<char> const & encodedStr )
{
    GrowBuffer res( encodedStr.length() * 4 / 5 );
    Impl_UrlDecode( encodedStr, [&res] ( char ch ) { res.appendType(ch); } );
    return std::move(res);
}

template <>
WINUX_FUNC_IMPL(Buffer) UrlDecodeBuffer( XString<wchar> const & encodedStr )
{
    GrowBuffer res( encodedStr.length() * 4 / 5 );
    Impl_UrlDecode( encodedStr, [&res] ( char ch ) { res.appendType(ch); } );
    return std::move(res);
}

template <>
WINUX_FUNC_IMPL(Buffer) UrlDecodeBuffer( XString<char16> const & encodedStr )
{
    GrowBuffer res( encodedStr.length() * 4 / 5 );
    Impl_UrlDecode( encodedStr, [&res] ( char ch ) { res.appendType(ch); } );
    return std::move(res);
}

template <>
WINUX_FUNC_IMPL(Buffer) UrlDecodeBuffer( XString<char32> const & encodedStr )
{
    GrowBuffer res( encodedStr.length() * 4 / 5 );
    Impl_UrlDecode( encodedStr, [&res] ( char ch ) { res.appendType(ch); } );
    return std::move(res);
}


// UriComponentEncode -------------------------------------------------------------------------
template < typename _ChTy >
inline static XString<_ChTy> Impl_UriComponentEncode( void const * buf, size_t size )
{
    XString<_ChTy> r;
    r.reserve( size_t(size * 2.718) );
    _ChTy sz[4] = { '%' };
    byte const * data = (byte const *)buf;
    for ( size_t i = 0; i < size; ++i )
    {
        _ChTy const c = data[i];
        if (
            c == '-' || c == '_' || c == '.' || c == '~' ||
            ( c >= '0' && c <= '9' ) ||
            ( c >= 'A' && c <= 'Z' ) ||
            ( c >= 'a' && c <= 'z' )
        )
        {
            r += c;
        }
        else
        {
            unsigned char tmp;
            tmp = ( c & 0xF );
            sz[2] = tmp < 10 ? tmp + '0' : tmp - 10 + 'A';
            tmp = ( ( c >> 4 ) & 0xF );
            sz[1] = tmp < 10 ? tmp + '0' : tmp - 10 + 'A';
            r += sz;
        }
    }
    return r;
}

template <>
WINUX_FUNC_IMPL(XString<char>) UriComponentEncode( void const * buf, size_t size )
{
    return Impl_UriComponentEncode<char>( buf, size );
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) UriComponentEncode( void const * buf, size_t size )
{
    return Impl_UriComponentEncode<wchar>( buf, size );
}

template <>
WINUX_FUNC_IMPL(XString<char16>) UriComponentEncode( void const * buf, size_t size )
{
    return Impl_UriComponentEncode<char16>( buf, size );
}

template <>
WINUX_FUNC_IMPL(XString<char32>) UriComponentEncode( void const * buf, size_t size )
{
    return Impl_UriComponentEncode<char32>( buf, size );
}


// UriComponentDecode -------------------------------------------------------------------------
template < typename _ChTy, typename _Fx >
inline static void Impl_UriComponentDecode( XString<_ChTy> const & encodedStr, _Fx && fnAppend )
{
    AnsiString::value_type low, hig;
    for ( auto it = encodedStr.begin(); it != encodedStr.end(); )
    {
        if ( *it != '%' )
        {
            fnAppend( (AnsiString::value_type)*it );
            ++it;
        }
        else // *it == '%'
        {
            ++it; // skip '%'
            if ( it != encodedStr.end() )
            {
                hig = (AnsiString::value_type)*it;
                bool isHex = IsHex(hig);
                if ( isHex )
                {
                    hig = hig > '9' ? hig - 'A' + 10 : hig - '0';
                }
                else // is not a hex value.
                {
                    fnAppend('%'); // 把跳过的'%'也加上
                    fnAppend(hig);
                }

                ++it;
                if (  it != encodedStr.end() )
                {
                    low = (AnsiString::value_type)*it;

                    if ( isHex )
                    {
                        low = low > '9' ? low - 'A' + 10 : low - '0' ;
                        fnAppend( (AnsiString::value_type)( low | ( hig << 4 ) ) );
                    }
                    else // not hex
                    {
                        fnAppend(low);
                    }

                    ++it;
                }
            }
            else // 说明已经是最后一个字符
            {
                fnAppend('%');
            }
        }
    }
}

template <>
WINUX_FUNC_IMPL(AnsiString) UriComponentDecode( XString<char> const & encodedStr )
{
    AnsiString res;
    res.reserve( encodedStr.length() * 4 / 5 );
    Impl_UriComponentDecode( encodedStr, [&res] ( char ch ) { res += ch; } );
    return res;
}

template <>
WINUX_FUNC_IMPL(AnsiString) UriComponentDecode( XString<wchar> const & encodedStr )
{
    AnsiString res;
    res.reserve( encodedStr.length() * 4 / 5 );
    Impl_UriComponentDecode( encodedStr, [&res] ( char ch ) { res += ch; } );
    return res;
}

template <>
WINUX_FUNC_IMPL(AnsiString) UriComponentDecode( XString<char16> const & encodedStr )
{
    AnsiString res;
    res.reserve( encodedStr.length() * 4 / 5 );
    Impl_UriComponentDecode( encodedStr, [&res] ( char ch ) { res += ch; } );
    return res;
}

template <>
WINUX_FUNC_IMPL(AnsiString) UriComponentDecode( XString<char32> const & encodedStr )
{
    AnsiString res;
    res.reserve( encodedStr.length() * 4 / 5 );
    Impl_UriComponentDecode( encodedStr, [&res] ( char ch ) { res += ch; } );
    return res;
}

template <>
WINUX_FUNC_IMPL(Buffer) UriComponentDecodeBuffer( XString<char> const & encodedStr )
{
    GrowBuffer res( encodedStr.length() * 4 / 5 );
    Impl_UriComponentDecode( encodedStr, [&res] ( char ch ) { res.appendType(ch); } );
    return std::move(res);
}

template <>
WINUX_FUNC_IMPL(Buffer) UriComponentDecodeBuffer( XString<wchar> const & encodedStr )
{
    GrowBuffer res( encodedStr.length() * 4 / 5 );
    Impl_UriComponentDecode( encodedStr, [&res] ( char ch ) { res.appendType(ch); } );
    return std::move(res);
}

template <>
WINUX_FUNC_IMPL(Buffer) UriComponentDecodeBuffer( XString<char16> const & encodedStr )
{
    GrowBuffer res( encodedStr.length() * 4 / 5 );
    Impl_UriComponentDecode( encodedStr, [&res] ( char ch ) { res.appendType(ch); } );
    return std::move(res);
}

template <>
WINUX_FUNC_IMPL(Buffer) UriComponentDecodeBuffer( XString<char32> const & encodedStr )
{
    GrowBuffer res( encodedStr.length() * 4 / 5 );
    Impl_UriComponentDecode( encodedStr, [&res] ( char ch ) { res.appendType(ch); } );
    return std::move(res);
}


WINUX_FUNC_IMPL(String) UrlEncodeString( String const & str, bool asUtf8 )
{
#if defined(_UNICODE) || defined(UNICODE)
    if ( asUtf8 )
    {
        return UrlEncode( UnicodeConverter(str).toUtf8() );
    }
    else
    {
        return UrlEncode( UnicodeToLocal(str) );
    }
#else
    if ( asUtf8 )
    {
        return UrlEncode( LocalToUtf8(str) );
    }
    else
    {
        return UrlEncode(str);
    }
#endif // UNICODE
}

WINUX_FUNC_IMPL(String) UrlDecodeString( String const & encodedStr, bool isUtf8 )
{
    AnsiString r = UrlDecode(encodedStr);
#if defined(_UNICODE) || defined(UNICODE)
    if ( isUtf8 )
    {
        return UnicodeConverter(r).toUnicode();
    }
    else
    {
        return LocalToUnicode(r);
    }
#else
    if ( isUtf8 )
    {
        return LocalFromUtf8(r);
    }
    else
    {
        return r;
    }
#endif // UNICODE
}

WINUX_FUNC_IMPL(String) UriComponentEncodeString( String const & str, bool asUtf8 )
{
#if defined(_UNICODE) || defined(UNICODE)
    if ( asUtf8 )
    {
        return UriComponentEncode( UnicodeConverter(str).toUtf8() );
    }
    else
    {
        return UriComponentEncode( UnicodeToLocal(str) );
    }
#else
    if ( asUtf8 )
    {
        return UriComponentEncode( LocalToUtf8(str) );
    }
    else
    {
        return UriComponentEncode(str);
    }
#endif // UNICODE
}

WINUX_FUNC_IMPL(String) UriComponentDecodeString( String const & encodedStr, bool isUtf8 )
{
    AnsiString r = UriComponentDecode(encodedStr);
#if defined(_UNICODE) || defined(UNICODE)
    if ( isUtf8 )
    {
        return UnicodeConverter(r).toUnicode();
    }
    else
    {
        return LocalToUnicode(r);
    }
#else
    if ( isUtf8 )
    {
        return LocalFromUtf8(r);
    }
    else
    {
        return r;
    }
#endif // UNICODE
}


WINUX_FUNC_IMPL(String) HtmlEncode( String const & str )
{
    thread_local MultiMatch mm( { TEXT("&amp;"), TEXT("&lt;"), TEXT("&gt;"), TEXT("&nbsp;") }, { TEXT("&"), TEXT("<"), TEXT(">"), TEXT(" ") } );
    return mm.replace(str);
}

WINUX_FUNC_IMPL(String) HtmlDecode( String const & str )
{
    thread_local MultiMatch mm( { TEXT("&amp;"), TEXT("&lt;"), TEXT("&gt;"), TEXT("&nbsp;") }, { TEXT("&"), TEXT("<"), TEXT(">"), TEXT(" ") } );
    return mm.replace(str);
}

#define _BASE64_ENCODE_CHARS(encodename, encodemacro) \
encodemacro(encodename, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/")

#define __BASE64_ENCODE_CHARS_CHAR(name, val) static constexpr char const *name = val;
#define __BASE64_ENCODE_CHARS_WCHAR(name, val) static constexpr wchar const *name = L##val;
#define __BASE64_ENCODE_CHARS_CHAR16(name, val) static constexpr char16 const *name = u##val;
#define __BASE64_ENCODE_CHARS_CHAR32(name, val) static constexpr char32 const *name = U##val;

#define __BASE64_CHAR(ch) ch

template < typename _ChTy > struct _Base64;
template <> struct _Base64<char>
{
    _BASE64_ENCODE_CHARS(EncodeChars, __BASE64_ENCODE_CHARS_CHAR)
};
template <> struct _Base64<wchar>
{
    _BASE64_ENCODE_CHARS(EncodeChars, __BASE64_ENCODE_CHARS_WCHAR)
};
template <> struct _Base64<char16>
{
    _BASE64_ENCODE_CHARS(EncodeChars, __BASE64_ENCODE_CHARS_CHAR16)
};
template <> struct _Base64<char32>
{
    _BASE64_ENCODE_CHARS(EncodeChars, __BASE64_ENCODE_CHARS_CHAR32)
};

template < typename _ChTy >
inline static XString<_ChTy> Impl_Base64Encode( void const * buf, size_t size )
{
    XString<_ChTy> outStr;
    byte c1, c2, c3;
    size_t i = 0;
    byte const * data = (byte const *)buf;
    while ( i < size )
    {
        // read the first byte
        c1 = data[i++];
        if ( i == size ) // pad with "="
        {
            outStr += _Base64<_ChTy>::EncodeChars[ c1 >> 2 ];
            outStr += _Base64<_ChTy>::EncodeChars[ ( c1 & 0x3 ) << 4 ];
            outStr += Literal<_ChTy>::eqStr;
            outStr += Literal<_ChTy>::eqStr;
            break;
        }

        // read the second byte
        c2 = data[i++];
        if ( i == size ) // pad with "="
        {
            outStr += _Base64<_ChTy>::EncodeChars[ c1 >> 2 ];
            outStr += _Base64<_ChTy>::EncodeChars[ ( ( c1 & 0x3 ) << 4 ) | ( ( c2 & 0xF0 ) >> 4 ) ];
            outStr += _Base64<_ChTy>::EncodeChars[ ( c2 & 0xF ) << 2 ];
            outStr += Literal<_ChTy>::eqStr;
            break;
        }

        // read the third byte
        c3 = data[i++];
        // convert into four bytes String
        outStr += _Base64<_ChTy>::EncodeChars[ c1 >> 2 ];
        outStr += _Base64<_ChTy>::EncodeChars[ ( ( c1 & 0x3 ) << 4 ) | ( ( c2 & 0xF0 ) >> 4 ) ];
        outStr += _Base64<_ChTy>::EncodeChars[ ( ( c2 & 0xF ) << 2 ) | ( ( c3 & 0xC0 ) >> 6 ) ];
        outStr += _Base64<_ChTy>::EncodeChars[ c3 & 0x3F ];
    }
    return outStr;
}

template <>
WINUX_FUNC_IMPL(XString<char>) Base64Encode( void const * buf, size_t size )
{
    return Impl_Base64Encode<char>( buf, size );
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) Base64Encode( void const * buf, size_t size )
{
    return Impl_Base64Encode<wchar>( buf, size );
}

template <>
WINUX_FUNC_IMPL(XString<char16>) Base64Encode( void const * buf, size_t size )
{
    return Impl_Base64Encode<char16>( buf, size );
}

template <>
WINUX_FUNC_IMPL(XString<char32>) Base64Encode( void const * buf, size_t size )
{
    return Impl_Base64Encode<char32>( buf, size );
}


static constexpr char const __Base64DecodeChars[] = {
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
   52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
   -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
   -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
};

template < typename _ChTy, typename _Fx >
inline static void Impl_Base64Decode( XString<_ChTy> const & base64Str, _Fx && fnAppend )
{
    size_t len = base64Str.length();
    size_t i = 0;
    char c1, c2, c3, c4;
    while ( i < len )
    {
        // read the first unsigned char
        do {
            c1 = __Base64DecodeChars[ base64Str[i++] ];
        } while ( i < len && c1 == -1 );

        if ( c1 == -1 ) break;

        // read the second unsigned char
        do {
            c2 = __Base64DecodeChars[ base64Str[i++] ];
        } while ( i < len && c2 == -1 );

        if ( c2 == -1 ) break;

        // assamble the first unsigned char
        fnAppend( ( c1 << 2 ) | ( ( c2 & 0x30 ) >> 4 ) );

        // read the third unsigned char
        do {
            c3 = (char)base64Str[i++];
            if ( c3 == 61 ) // meet with "=", break
                return;
            c3 = __Base64DecodeChars[c3];
        } while ( i < len && c3 == -1);

        if ( c3 == -1 ) break;

        // assamble the second unsigned char
        fnAppend( ( ( c2 & 0xF ) << 4 ) | ( ( c3 & 0x3C ) >> 2 ) );

        // read the fourth unsigned char
        do {
            c4 = (char)base64Str[i++];
            if ( c4 == 61 ) // meet with "=", break
                return;
            c4 = __Base64DecodeChars[c4];
        } while ( i < len && c4 == -1 );

        if ( c4 == -1 ) break;

        // assamble the third unsigned char
        fnAppend( ( ( c3 & 0x03 ) << 6 ) | c4 );
    }
}

template <>
WINUX_FUNC_IMPL(AnsiString) Base64Decode( XString<char> const & base64Str )
{
    AnsiString res;
    Impl_Base64Decode( base64Str, [&res] ( char ch ) { res += ch; } );
    return res;
}

template <>
WINUX_FUNC_IMPL(AnsiString) Base64Decode( XString<wchar> const & base64Str )
{
    AnsiString res;
    Impl_Base64Decode( base64Str, [&res] ( char ch ) { res += ch; } );
    return res;
}

template <>
WINUX_FUNC_IMPL(AnsiString) Base64Decode( XString<char16> const & base64Str )
{
    AnsiString res;
    Impl_Base64Decode( base64Str, [&res] ( char ch ) { res += ch; } );
    return res;
}

template <>
WINUX_FUNC_IMPL(AnsiString) Base64Decode( XString<char32> const & base64Str )
{
    AnsiString res;
    Impl_Base64Decode( base64Str, [&res] ( char ch ) { res += ch; } );
    return res;
}


template <>
WINUX_FUNC_IMPL(Buffer) Base64DecodeBuffer( XString<char> const & base64Str )
{
    GrowBuffer res(32);
    Impl_Base64Decode( base64Str, [&res] ( char ch ) { res.appendType(ch); } );
    return std::move(res);
}

template <>
WINUX_FUNC_IMPL(Buffer) Base64DecodeBuffer( XString<wchar> const & base64Str )
{
    GrowBuffer res(32);
    Impl_Base64Decode( base64Str, [&res] ( char ch ) { res.appendType(ch); } );
    return std::move(res);
}

template <>
WINUX_FUNC_IMPL(Buffer) Base64DecodeBuffer( XString<char16> const & base64Str )
{
    GrowBuffer res(32);
    Impl_Base64Decode( base64Str, [&res] ( char ch ) { res.appendType(ch); } );
    return std::move(res);
}

template <>
WINUX_FUNC_IMPL(Buffer) Base64DecodeBuffer( XString<char32> const & base64Str )
{
    GrowBuffer res(32);
    Impl_Base64Decode( base64Str, [&res] ( char ch ) { res.appendType(ch); } );
    return std::move(res);
}


template < typename _ChTy >
inline static XString<_ChTy> Impl_BufferToHex( Buffer const & buf )
{
    XString<_ChTy> r;
    byte * pbyt = buf.getBuf<byte>();
    for ( size_t i = 0; i < buf.getSize(); ++i )
        r += CharToHexStr<_ChTy>(pbyt[i]);
    return r;
}

template <>
WINUX_FUNC_IMPL(XString<char>) BufferToHex( Buffer const & buf )
{
    return Impl_BufferToHex<char>(buf);
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) BufferToHex( Buffer const & buf )
{
    return Impl_BufferToHex<wchar>(buf);
}

template <>
WINUX_FUNC_IMPL(XString<char16>) BufferToHex( Buffer const & buf )
{
    return Impl_BufferToHex<char16>(buf);
}

template <>
WINUX_FUNC_IMPL(XString<char32>) BufferToHex( Buffer const & buf )
{
    return Impl_BufferToHex<char32>(buf);
}


template < typename _ChTy >
inline static int8 HexToInt8( _ChTy ch )
{
    return ( ch >= Literal<_ChTy>::zeroChar && ch <= Literal<_ChTy>::nineChar ) ? int8( ch - Literal<_ChTy>::zeroChar ) : int8( ( ch | 0x20 ) - Literal<_ChTy>::aChar + 10 );
}

template < typename _ChTy >
inline static Buffer Impl_BufferFromHex( XString<_ChTy> const & hexStr )
{
    Buffer buf( nullptr, hexStr.length() / 2 );
    for ( size_t i = 0; i < hexStr.length() / 2; ++i )
        buf[i] = ( HexToInt8(hexStr[i * 2]) << 4 ) | HexToInt8(hexStr[i * 2 + 1]);
    return buf;
}

template <>
WINUX_FUNC_IMPL(Buffer) HexToBuffer( XString<char> const & hexStr )
{
    return Impl_BufferFromHex(hexStr);
}

template <>
WINUX_FUNC_IMPL(Buffer) HexToBuffer( XString<wchar> const & hexStr )
{
    return Impl_BufferFromHex(hexStr);
}

template <>
WINUX_FUNC_IMPL(Buffer) HexToBuffer( XString<char16> const & hexStr )
{
    return Impl_BufferFromHex(hexStr);
}

template <>
WINUX_FUNC_IMPL(Buffer) HexToBuffer( XString<char32> const & hexStr )
{
    return Impl_BufferFromHex(hexStr);
}


WINUX_FUNC_IMPL(Buffer) Md5( void const * buf, size_t size )
{
    MD5 md5;
    md5.update( (char const *)buf, (unsigned int)size );
    md5.finalize();
    return md5.digestres();
}

WINUX_FUNC_IMPL(Buffer) Sha1( void const * buf, size_t size )
{
    Buffer res( nullptr, SHA1_DIGEST_SIZE );
    sha1_ctx ctx;
    sha1_init(&ctx);
    sha1_update( &ctx, buf, size );
    sha1_final( &ctx, (uint8_t *)res.getBuf() );
    return res;
}

WINUX_FUNC_IMPL(Buffer) Sha224( void const * buf, size_t size )
{
    Buffer res( nullptr, SHA224_DIGEST_SIZE );
    sha224_ctx ctx;
    sha224_init(&ctx);
    sha224_update( &ctx, (unsigned char *)buf, (unsigned int)size );
    sha224_final( &ctx, res.getBuf<unsigned char>() );
    return res;
}

WINUX_FUNC_IMPL(Buffer) Sha256( void const * buf, size_t size )
{
    Buffer res( nullptr, SHA256_DIGEST_SIZE );
    sha256_ctx ctx;
    sha256_init(&ctx);
    sha256_update( &ctx, (unsigned char *)buf, (unsigned int)size );
    sha256_final( &ctx, res.getBuf<unsigned char>() );
    return res;
}

WINUX_FUNC_IMPL(Buffer) Sha384( void const * buf, size_t size )
{
    Buffer res( nullptr, SHA384_DIGEST_SIZE );
    sha384_ctx ctx;
    sha384_init(&ctx);
    sha384_update( &ctx, (unsigned char *)buf, (unsigned int)size );
    sha384_final( &ctx, res.getBuf<unsigned char>() );
    return res;
}

WINUX_FUNC_IMPL(Buffer) Sha512( void const * buf, size_t size )
{
    Buffer res( nullptr, SHA512_DIGEST_SIZE );
    sha512_ctx ctx;
    sha512_init(&ctx);
    sha512_update( &ctx, (unsigned char *)buf, (unsigned int)size );
    sha512_final( &ctx, res.getBuf<unsigned char>() );
    return res;
}

static int __EncryptInternal( byte const keyIfOnlyOne, byte const * data, size_t dataSize, byte * buf, size_t bufSize, size_t * outSize )
{
    if ( dataSize < 1 )
    {
        *outSize = 0;
        return ERR_DATASIZE;
    }
    else if ( dataSize == 1 )
    {
        if ( bufSize > 0 )
        {
            buf[0] = data[0] ^ keyIfOnlyOne;
            *outSize = 1;
        }
    }
    else
    {
        size_t i = 0, j = 0;
        while ( i < dataSize && j < bufSize )
        {
            buf[j] = data[i];
            ++i;
            ++j;
        }
        *outSize = j;
        for ( i = 0; i < *outSize - 1; ++i )
        {
            buf[i] = buf[i] ^ buf[i + 1];
        }
        buf[i] = buf[i] ^ buf[0];
    }
    return ERR_SUCCEED;
}

static int __DecryptInternal( byte const keyIfOnlyOne, byte const * encryptData, size_t dataSize, byte * buf, size_t bufSize, size_t * outSize )
{
    if ( dataSize < 1 )
    {
        *outSize = 0;
        return ERR_DATASIZE;
    }
    else if ( dataSize == 1 )
    {
        if ( bufSize > 0 )
        {
            buf[0] = encryptData[0] ^ keyIfOnlyOne;
            *outSize = 1;
        }
    }
    else
    {
        size_t i = 0, j = 0;
        while ( i < dataSize && j < bufSize )
        {
            buf[j] = encryptData[i];
            ++i;
            ++j;
        }
        *outSize = j;
        buf[*outSize - 1] = buf[*outSize - 1] ^ buf[0];
        for ( i = *outSize - 1 - 1; (offset_t)i >= 0; --i )
        {
            buf[i] = buf[i] ^ buf[i + 1];
        }
    }
    return ERR_SUCCEED;
}

WINUX_FUNC_IMPL(int) RawEncrypt( byte const * data, size_t dataSize, byte * buf, size_t bufSize, size_t * outSize )
{
    int ret;
    ret = __EncryptInternal( 0x25, data, dataSize, buf, bufSize, outSize );
    return ret;
}

WINUX_FUNC_IMPL(int) RawDecrypt( byte const * encryptData, size_t dataSize, byte * buf, size_t bufSize, size_t * outSize )
{
    int ret;
    ret = __DecryptInternal( 0x25, encryptData, dataSize, buf, bufSize, outSize );
    return ret;

}

WINUX_FUNC_IMPL(AnsiString) EncryptContent( AnsiString const & content )
{
    AnsiString res(content);
    size_t outSize = 0;
    if ( res.size() > 0 )
        RawEncrypt( (byte *)content.c_str(), content.size(), (byte *)&res[0], res.size(), &outSize );
    return res;
}

WINUX_FUNC_IMPL(Buffer) EncryptContent( Buffer const & content )
{
    Buffer res;
    res.setBuf( content.getBuf(), content.getSize(), false );
    size_t outSize = 0;
    if ( res.getSize() > 0 )
        RawEncrypt( content.getBuf<byte>(), content.getSize(), res.getBuf<byte>(), res.getSize(), &outSize );
    return res;
}

WINUX_FUNC_IMPL(AnsiString) DecryptContent( AnsiString const & encryptContent )
{
    AnsiString res(encryptContent);
    size_t outSize = 0;
    if ( res.size() > 0 )
        RawDecrypt( (byte *)encryptContent.c_str(), encryptContent.size(), (byte *)&res[0], res.size(), &outSize );
    return res;
}

WINUX_FUNC_IMPL(Buffer) DecryptContent( Buffer const & encryptContent )
{
    Buffer res;
    res.setBuf( encryptContent.getBuf(), encryptContent.getSize(), false );
    size_t outSize = 0;
    if ( res.getSize() > 0 )
        RawDecrypt( encryptContent.getBuf<byte>(), encryptContent.getSize(), res.getBuf<byte>(), res.getSize(), &outSize );
    return res;
}


} // namespace winux
