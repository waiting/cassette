#include "system_detection.inl"

#if defined(CL_MINGW) // for mingw
    #ifdef __STRICT_ANSI__
    #undef __STRICT_ANSI__
    #endif
#endif

#include "utilities.hpp"
#include "strings.hpp"

#if defined(__GNUC__) || defined(HAVE_ICONV)
#include <iconv.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <math.h>
#include <iomanip>

#if defined(OS_WIN) // IS_WINDOWS
    #include <mbstring.h>
    #include <tchar.h>

    #ifdef UNICODE
    #define _vsntprintf _vsnwprintf
    #else
    #define _vsntprintf _vsnprintf
    #endif

#else
    #include <errno.h>
    #define _vsnprintf vsnprintf
    #define _vsnwprintf vswprintf

    #ifdef UNICODE
    #define _vsntprintf vswprintf
    #define _tcsstr wcsstr
    #else
    #define _vsntprintf vsnprintf
    #define _tcsstr strstr
    #endif

    #define swprintf_s swprintf
#endif

namespace winux
{

#include "is_x_funcs.inl"

// 字符是否在给定的列表中
template < typename _ChTy >
inline static bool InCharList( _ChTy ch, XString<_ChTy> const & charlist )
{
    return charlist.find(ch) != XString<_ChTy>::npos;
}


template < typename _ChTy >
inline static size_t Impl_StrSplit( XString<_ChTy> const & str, XString<_ChTy> const & delimList, XStringArray<_ChTy> * arr, bool alwaysRetOneElem )
{
    if ( !alwaysRetOneElem && str.empty() ) return 0;

    size_t count = 0;
    XString<_ChTy> tmp;

    for ( size_t i = 0; i < str.length(); i++ )
    {
        if ( InCharList( str[i], delimList ) )
        {
            arr->push_back(tmp);
            count++;
            tmp.clear();
        }
        else
        {
            tmp += str[i];
        }
    }
    arr->push_back(tmp);
    count++;
    return count;
}

template <>
WINUX_FUNC_IMPL(size_t) StrSplit( XString<char> const & str, XString<char> const & delimList, XStringArray<char> * arr, bool alwaysRetOneElem )
{
    return Impl_StrSplit( str, delimList, arr, alwaysRetOneElem );
}

template <>
WINUX_FUNC_IMPL(size_t) StrSplit( XString<wchar> const & str, XString<wchar> const & delimList, XStringArray<wchar> * arr, bool alwaysRetOneElem )
{
    return Impl_StrSplit( str, delimList, arr, alwaysRetOneElem );
}

template <>
WINUX_FUNC_IMPL(size_t) StrSplit( XString<char16> const & str, XString<char16> const & delimList, XStringArray<char16> * arr, bool alwaysRetOneElem )
{
    return Impl_StrSplit( str, delimList, arr, alwaysRetOneElem );
}

template <>
WINUX_FUNC_IMPL(size_t) StrSplit( XString<char32> const & str, XString<char32> const & delimList, XStringArray<char32> * arr, bool alwaysRetOneElem )
{
    return Impl_StrSplit( str, delimList, arr, alwaysRetOneElem );
}

template <>
WINUX_FUNC_IMPL(XStringArray<char>) StrSplit( XString<char> const & str, XString<char> const & delimList, bool alwaysRetOneElem )
{
    XStringArray<char> arr;
    Impl_StrSplit( str, delimList, &arr, alwaysRetOneElem );
    return arr;
}

template <>
WINUX_FUNC_IMPL(XStringArray<wchar>) StrSplit( XString<wchar> const & str, XString<wchar> const & delimList, bool alwaysRetOneElem )
{
    XStringArray<wchar> arr;
    Impl_StrSplit( str, delimList, &arr, alwaysRetOneElem );
    return arr;
}

template <>
WINUX_FUNC_IMPL(XStringArray<char16>) StrSplit( XString<char16> const & str, XString<char16> const & delimList, bool alwaysRetOneElem )
{
    XStringArray<char16> arr;
    Impl_StrSplit( str, delimList, &arr, alwaysRetOneElem );
    return arr;
}

template <>
WINUX_FUNC_IMPL(XStringArray<char32>) StrSplit( XString<char32> const & str, XString<char32> const & delimList, bool alwaysRetOneElem )
{
    XStringArray<char32> arr;
    Impl_StrSplit( str, delimList, &arr, alwaysRetOneElem );
    return arr;
}


template < typename _ChTy >
inline static size_t Impl_StrSplit2( XString<_ChTy> const & str, XString<_ChTy> const & delim, XStringArray<_ChTy> * arr, bool alwaysRetOneElem )
{
    if ( !alwaysRetOneElem && str.empty() ) return 0;
    if ( delim.empty() )
    {
        arr->push_back(str);
        return 1;
    }

    size_t count = 0;
    size_t cur = 0;
    size_t found = cur;

    while ( ( found = str.find( delim, cur ) ) != XString<_ChTy>::npos )
    {
        arr->push_back( str.substr( cur, found - cur ) );
        count++;
        cur = found + delim.length();
    }
    arr->push_back( str.substr(cur) );
    count++;
    return count;
}

template <>
WINUX_FUNC_IMPL(size_t) StrSplit2( XString<char> const & str, XString<char> const & delim, XStringArray<char> * arr, bool alwaysRetOneElem )
{
    return Impl_StrSplit2( str, delim, arr, alwaysRetOneElem );
}

template <>
WINUX_FUNC_IMPL(size_t) StrSplit2( XString<wchar> const & str, XString<wchar> const & delim, XStringArray<wchar> * arr, bool alwaysRetOneElem )
{
    return Impl_StrSplit2( str, delim, arr, alwaysRetOneElem );
}

template <>
WINUX_FUNC_IMPL(size_t) StrSplit2( XString<char16> const & str, XString<char16> const & delim, XStringArray<char16> * arr, bool alwaysRetOneElem )
{
    return Impl_StrSplit2( str, delim, arr, alwaysRetOneElem );
}

template <>
WINUX_FUNC_IMPL(size_t) StrSplit2( XString<char32> const & str, XString<char32> const & delim, XStringArray<char32> * arr, bool alwaysRetOneElem )
{
    return Impl_StrSplit2( str, delim, arr, alwaysRetOneElem );
}

template <>
WINUX_FUNC_IMPL(XStringArray<char>) StrSplit2( XString<char> const & str, XString<char> const & delim, bool alwaysRetOneElem )
{
    XStringArray<char> arr;
    Impl_StrSplit2( str, delim, &arr, alwaysRetOneElem );
    return arr;
}

template <>
WINUX_FUNC_IMPL(XStringArray<wchar>) StrSplit2( XString<wchar> const & str, XString<wchar> const & delim, bool alwaysRetOneElem )
{
    XStringArray<wchar> arr;
    Impl_StrSplit2( str, delim, &arr, alwaysRetOneElem );
    return arr;
}

template <>
WINUX_FUNC_IMPL(XStringArray<char16>) StrSplit2( XString<char16> const & str, XString<char16> const & delim, bool alwaysRetOneElem )
{
    XStringArray<char16> arr;
    Impl_StrSplit2( str, delim, &arr, alwaysRetOneElem );
    return arr;
}

template <>
WINUX_FUNC_IMPL(XStringArray<char32>) StrSplit2( XString<char32> const & str, XString<char32> const & delim, bool alwaysRetOneElem )
{
    XStringArray<char32> arr;
    Impl_StrSplit2( str, delim, &arr, alwaysRetOneElem );
    return arr;
}


template < typename _ChTy >
inline static XString<_ChTy> Impl_StrJoin( XString<_ChTy> const & delim, XStringArray<_ChTy> const & arr )
{
    XString<_ChTy> res;
    size_t count = arr.size();
    size_t i;
    for ( i = 0; i < count; i++ )
    {
        if ( i != 0 )
        {
            res += delim;
        }
        res += arr[i];
    }
    return res;
}

template <>
WINUX_FUNC_IMPL(XString<char>) StrJoin( XString<char> const & delim, XStringArray<char> const & arr )
{
    return Impl_StrJoin( delim, arr );
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) StrJoin( XString<wchar> const & delim, XStringArray<wchar> const & arr )
{
    return Impl_StrJoin( delim, arr );
}

template <>
WINUX_FUNC_IMPL(XString<char16>) StrJoin( XString<char16> const & delim, XStringArray<char16> const & arr )
{
    return Impl_StrJoin( delim, arr );
}

template <>
WINUX_FUNC_IMPL(XString<char32>) StrJoin( XString<char32> const & delim, XStringArray<char32> const & arr )
{
    return Impl_StrJoin( delim, arr );
}


template < typename _ChTy >
inline static XString<_ChTy> Impl_StrJoinEx( XString<_ChTy> const & delim, XStringArray<_ChTy> const & arr, size_t start, size_t elemCount )
{
    XString<_ChTy> res;
    size_t count = 0; //arr.size() < start ? 0 : ( elemCount < arr.size() - start ? elemCount : arr.size() - start );
    if ( arr.size() < start )
    {
        count = 0;
    }
    else
    {
        count = arr.size() - start;
        if ( elemCount < count ) count = elemCount;
    }
    for ( size_t i = 0; i < count; i++ )
    {
        if ( i != 0 )
        {
            res += delim;
        }
        res += arr[i + start];
    }
    return res;
}

template <>
WINUX_FUNC_IMPL(XString<char>) StrJoinEx( XString<char> const & delim, XStringArray<char> const & arr, size_t start, size_t elemCount )
{
    return Impl_StrJoinEx( delim, arr, start, elemCount );
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) StrJoinEx( XString<wchar> const & delim, XStringArray<wchar> const & arr, size_t start, size_t elemCount )
{
    return Impl_StrJoinEx( delim, arr, start, elemCount );
}

template <>
WINUX_FUNC_IMPL(XString<char16>) StrJoinEx( XString<char16> const & delim, XStringArray<char16> const & arr, size_t start, size_t elemCount )
{
    return Impl_StrJoinEx( delim, arr, start, elemCount );
}

template <>
WINUX_FUNC_IMPL(XString<char32>) StrJoinEx( XString<char32> const & delim, XStringArray<char32> const & arr, size_t start, size_t elemCount )
{
    return Impl_StrJoinEx( delim, arr, start, elemCount );
}


template < typename _ChTy >
inline static XString<_ChTy> Impl_StrInsert( XString<_ChTy> const & str, size_t start, size_t end, XString<_ChTy> const & insert )
{
    XString<_ChTy> res;
    res += str.substr( 0, start );
    res += insert;
    if ( end < str.length() )
        res += str.substr(end);
    return res;
}

template <>
WINUX_FUNC_IMPL(XString<char>) StrInsert( XString<char> const & str, size_t start, size_t end, XString<char> const & insert )
{
    return Impl_StrInsert( str, start, end, insert );
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) StrInsert( XString<wchar> const & str, size_t start, size_t end, XString<wchar> const & insert )
{
    return Impl_StrInsert( str, start, end, insert );
}

template <>
WINUX_FUNC_IMPL(XString<char16>) StrInsert( XString<char16> const & str, size_t start, size_t end, XString<char16> const & insert )
{
    return Impl_StrInsert( str, start, end, insert );
}

template <>
WINUX_FUNC_IMPL(XString<char32>) StrInsert( XString<char32> const & str, size_t start, size_t end, XString<char32> const & insert )
{
    return Impl_StrInsert( str, start, end, insert );
}


template < typename _ChTy >
inline static XString<_ChTy> & Impl_StrMakeReplace( XString<_ChTy> * str, XString<_ChTy> const & findText, XString<_ChTy> const & replaceText, size_t offset )
{
    size_t pos = offset;
    while ( ( pos = str->find( findText, pos ) ) != XString<_ChTy>::npos )
    {
        str->replace( pos, findText.length(), replaceText );
        pos += replaceText.length();
    }
    return *str;
}

template <>
WINUX_FUNC_IMPL(XString<char> &) StrMakeReplace( XString<char> * str, XString<char> const & findText, XString<char> const & replaceText, size_t offset )
{
    return Impl_StrMakeReplace( str, findText, replaceText, offset );
}

template <>
WINUX_FUNC_IMPL(XString<wchar> &) StrMakeReplace( XString<wchar> * str, XString<wchar> const & findText, XString<wchar> const & replaceText, size_t offset )
{
    return Impl_StrMakeReplace( str, findText, replaceText, offset );
}

template <>
WINUX_FUNC_IMPL(XString<char16> &) StrMakeReplace( XString<char16> * str, XString<char16> const & findText, XString<char16> const & replaceText, size_t offset )
{
    return Impl_StrMakeReplace( str, findText, replaceText, offset );
}

template <>
WINUX_FUNC_IMPL(XString<char32> &) StrMakeReplace( XString<char32> * str, XString<char32> const & findText, XString<char32> const & replaceText, size_t offset )
{
    return Impl_StrMakeReplace( str, findText, replaceText, offset );
}


enum StrTrimFlags
{
    trimLeft = 1,
    trimRight = 2
};

template < typename _ChTy >
inline static XString<_ChTy> Impl_StrTrim( XString<_ChTy> const & str, int trimFlags, size_t * leftSpaceCount = nullptr, size_t * rightSpaceCount = nullptr )
{
    _ChTy const * p1, * p2;
    p1 = str.c_str();
    p2 = str.c_str() + str.length();
    if ( trimFlags & trimLeft )
    {
        size_t c = 0;
        while ( p1 < p2 && ( *p1 == Literal<_ChTy>::spaceChar || *p1 == Literal<_ChTy>::crChar || *p1 == Literal<_ChTy>::lfChar || *p1 == Literal<_ChTy>::htChar || *p1 == Literal<_ChTy>::vtChar ) )
        {
            p1++;
            c++;
        }
        ASSIGN_PTR(leftSpaceCount) = c;
    }
    if ( trimFlags & trimRight )
    {
        size_t c = 0;
        while ( p2 > p1 && ( p2[-1] == Literal<_ChTy>::spaceChar || p2[-1] == Literal<_ChTy>::crChar || p2[-1] == Literal<_ChTy>::lfChar || p2[-1] == Literal<_ChTy>::htChar || p2[-1] == Literal<_ChTy>::vtChar ) )
        {
            p2--;
            c++;
        }
        ASSIGN_PTR(rightSpaceCount) = c;
    }
    return XString<_ChTy>( p1, p2 );
}

template <>
WINUX_FUNC_IMPL(XString<char>) StrTrim( XString<char> const & str, size_t * leftSpaceCount, size_t * rightSpaceCount )
{
    return Impl_StrTrim( str, trimLeft | trimRight, leftSpaceCount, rightSpaceCount );
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) StrTrim( XString<wchar> const & str, size_t * leftSpaceCount, size_t * rightSpaceCount )
{
    return Impl_StrTrim( str, trimLeft | trimRight, leftSpaceCount, rightSpaceCount );
}

template <>
WINUX_FUNC_IMPL(XString<char16>) StrTrim( XString<char16> const & str, size_t * leftSpaceCount, size_t * rightSpaceCount )
{
    return Impl_StrTrim( str, trimLeft | trimRight, leftSpaceCount, rightSpaceCount );
}

template <>
WINUX_FUNC_IMPL(XString<char32>) StrTrim( XString<char32> const & str, size_t * leftSpaceCount, size_t * rightSpaceCount )
{
    return Impl_StrTrim( str, trimLeft | trimRight, leftSpaceCount, rightSpaceCount );
}


template <>
WINUX_FUNC_IMPL(XString<char>) StrLTrim( XString<char> const & str, size_t * leftSpaceCount )
{
    return Impl_StrTrim( str, trimLeft, leftSpaceCount, nullptr );
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) StrLTrim( XString<wchar> const & str, size_t * leftSpaceCount )
{
    return Impl_StrTrim( str, trimLeft, leftSpaceCount, nullptr );
}

template <>
WINUX_FUNC_IMPL(XString<char16>) StrLTrim( XString<char16> const & str, size_t * leftSpaceCount )
{
    return Impl_StrTrim( str, trimLeft, leftSpaceCount, nullptr );
}

template <>
WINUX_FUNC_IMPL(XString<char32>) StrLTrim( XString<char32> const & str, size_t * leftSpaceCount )
{
    return Impl_StrTrim( str, trimLeft, leftSpaceCount, nullptr );
}


template <>
WINUX_FUNC_IMPL(XString<char>) StrRTrim( XString<char> const & str, size_t * rightSpaceCount )
{
    return Impl_StrTrim( str, trimRight, nullptr, rightSpaceCount );
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) StrRTrim( XString<wchar> const & str, size_t * rightSpaceCount )
{
    return Impl_StrTrim( str, trimRight, nullptr, rightSpaceCount );
}

template <>
WINUX_FUNC_IMPL(XString<char16>) StrRTrim( XString<char16> const & str, size_t * rightSpaceCount )
{
    return Impl_StrTrim( str, trimRight, nullptr, rightSpaceCount );
}

template <>
WINUX_FUNC_IMPL(XString<char32>) StrRTrim( XString<char32> const & str, size_t * rightSpaceCount )
{
    return Impl_StrTrim( str, trimRight, nullptr, rightSpaceCount );
}


template < typename _ChTy >
inline static XString<_ChTy> & Impl_StrMakeUpper( XString<_ChTy> * str )
{
    XString<_ChTy> & rStr = *str;
    for ( size_t i = 0; i < rStr.length(); ++i )
    {
        auto & ch = rStr[i];
        if ( IsAlphabet(ch) ) ch &= ~0x20;
    }
    return rStr;
}

template < typename _ChTy >
inline static XString<_ChTy> & Impl_StrMakeLower( XString<_ChTy> * str )
{
    XString<_ChTy> & rStr = *str;
    for ( size_t i = 0; i < rStr.length(); ++i )
    {
        auto & ch = rStr[i];
        if ( IsAlphabet(ch) ) ch |= 0x20;
    }
    return rStr;
}

template <>
WINUX_FUNC_IMPL(XString<char> &) StrMakeUpper( XString<char> * str )
{
    return Impl_StrMakeUpper(str);
}

template <>
WINUX_FUNC_IMPL(XString<wchar> &) StrMakeUpper( XString<wchar> * str )
{
    return Impl_StrMakeUpper(str);
}

template <>
WINUX_FUNC_IMPL(XString<char16> &) StrMakeUpper( XString<char16> * str )
{
    return Impl_StrMakeUpper(str);
}

template <>
WINUX_FUNC_IMPL(XString<char32> &) StrMakeUpper( XString<char32> * str )
{
    return Impl_StrMakeUpper(str);
}

template <>
WINUX_FUNC_IMPL(XString<char>) StrUpper( XString<char> str )
{
    Impl_StrMakeUpper(&str);
    return str;
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) StrUpper( XString<wchar> str )
{
    Impl_StrMakeUpper(&str);
    return str;
}

template <>
WINUX_FUNC_IMPL(XString<char16>) StrUpper( XString<char16> str )
{
    Impl_StrMakeUpper(&str);
    return str;
}

template <>
WINUX_FUNC_IMPL(XString<char32>) StrUpper( XString<char32> str )
{
    Impl_StrMakeUpper(&str);
    return str;
}

template <>
WINUX_FUNC_IMPL(XString<char> &) StrMakeLower( XString<char> * str )
{
    return Impl_StrMakeLower(str);
}

template <>
WINUX_FUNC_IMPL(XString<wchar> &) StrMakeLower( XString<wchar> * str )
{
    return Impl_StrMakeLower(str);
}

template <>
WINUX_FUNC_IMPL(XString<char16> &) StrMakeLower( XString<char16> * str )
{
    return Impl_StrMakeLower(str);
}

template <>
WINUX_FUNC_IMPL(XString<char32> &) StrMakeLower( XString<char32> * str )
{
    return Impl_StrMakeLower(str);
}

template <>
WINUX_FUNC_IMPL(XString<char>) StrLower( XString<char> str )
{
    Impl_StrMakeLower(&str);
    return str;
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) StrLower( XString<wchar> str )
{
    Impl_StrMakeLower(&str);
    return str;
}

template <>
WINUX_FUNC_IMPL(XString<char16>) StrLower( XString<char16> str )
{
    Impl_StrMakeLower(&str);
    return str;
}

template <>
WINUX_FUNC_IMPL(XString<char32>) StrLower( XString<char32> str )
{
    Impl_StrMakeLower(&str);
    return str;
}


template < typename _ChTy >
inline static XString<_ChTy> Impl_StrMultiple( XString<_ChTy> const & str, size_t multiple )
{
    XString<_ChTy> r;
    for ( size_t i = 0; i < multiple; i++ ) r += str;
    return r;
}

template <>
WINUX_FUNC_IMPL(XString<char>) StrMultiple( XString<char> const & str, size_t multiple )
{
    return Impl_StrMultiple( str, multiple );
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) StrMultiple( XString<wchar> const & str, size_t multiple )
{
    return Impl_StrMultiple( str, multiple );
}

template <>
WINUX_FUNC_IMPL(XString<char16>) StrMultiple( XString<char16> const & str, size_t multiple )
{
    return Impl_StrMultiple( str, multiple );
}

template <>
WINUX_FUNC_IMPL(XString<char32>) StrMultiple( XString<char32> const & str, size_t multiple )
{
    return Impl_StrMultiple( str, multiple );
}


template < typename _ChTy >
inline static XString<_ChTy> Impl_StrSubtract( XString<_ChTy> str1, XString<_ChTy> const & str2 )
{
    Impl_StrMakeReplace<_ChTy>( &str1, str2, Literal<_ChTy>::nulStr, 0 );
    return str1;
}

template <>
WINUX_FUNC_IMPL(XString<char>) StrSubtract( XString<char> str1, XString<char> const & str2 )
{
    return Impl_StrSubtract( str1, str2 );
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) StrSubtract( XString<wchar> str1, XString<wchar> const & str2 )
{
    return Impl_StrSubtract( str1, str2 );
}

template <>
WINUX_FUNC_IMPL(XString<char16>) StrSubtract( XString<char16> str1, XString<char16> const & str2 )
{
    return Impl_StrSubtract( str1, str2 );
}

template <>
WINUX_FUNC_IMPL(XString<char32>) StrSubtract( XString<char32> str1, XString<char32> const & str2 )
{
    return Impl_StrSubtract( str1, str2 );
}


#if defined(__GNUC__) /*&& !defined(WIN32)*/
#define _UI64_MAX 0xffffffffffffffffull
#define _I64_MAX 9223372036854775807ll
#define _I64_MIN (-9223372036854775807ll - 1ll)
#endif

template < typename _ChTy >
inline static uint64 Impl_StrToXq( _ChTy const * nptr, _ChTy const ** endptr, int ibase, int flags )
{
    _ChTy const * p;
    _ChTy c;
    uint64 number;
    uint digval;
    uint64 maxval;

    p = nptr; /* p is our scanning pointer */
    number = 0; /* start with zero */

    c = *p++; /* read char */

    while ( isspace((int)(unsigned char)c) )
        c = *p++; /* skip whitespace */

    if ( c == Literal<_ChTy>::negativeChar )
    {
        flags |= stqNegative; /* remember minus sign */
        c = *p++;
    }
    else if ( c == Literal<_ChTy>::positiveChar )
        c = *p++; /* skip sign */

    if ( ibase < 0 || ibase == 1 || ibase > 36 )
    {
        /* bad base! */
        if ( endptr )
        {
            /* store beginning of string in endptr */
            *endptr = nptr;
        }
        return 0L; /* return 0 */
    }
    else if ( ibase == 0 )
    {
        /* determine base free-lance, based on first two chars of string */
        if ( c != Literal<_ChTy>::zeroChar )
            ibase = 10;
        else if ( ( *p | 0x20 ) == Literal<_ChTy>::xChar )
            ibase = 16;
        else
            ibase = 8;
    }

    if ( ibase == 16 )
    {
        /* we might have 0x in front of number; remove if there */
        if ( c == Literal<_ChTy>::zeroChar && ( *p | 0x20 ) == Literal<_ChTy>::xChar )
        {
            ++p;
            c = *p++; /* advance past prefix */
        }
    }

    /* if our number exceeds this, we will overflow on multiply */
    maxval = _UI64_MAX / ibase;

    for ( ; ; ) /* exit in middle of loop */
    {
        /* convert c to value */
        if ( isdigit((int)(unsigned char)c) )
            digval = c - Literal<_ChTy>::zeroChar;
        else if ( isalpha((int)(unsigned char)c) )
            digval = toupper(c) - Literal<_ChTy>::AChar + 10;
        else
            break;

        if ( digval >= (unsigned)ibase )
            break; /* exit loop if bad digit found */

        /* record the fact we have read one digit */
        flags |= stqReadDigit;

        /* we now need to compute number = number * base + digval,
           but we need to know if overflow occured.  This requires
           a tricky pre-check. */

        if ( number < maxval || ( number == maxval && (uint64)digval <= _UI64_MAX % ibase ) )
        {
            /* we won't overflow, go ahead and multiply */
            number = number * ibase + digval;
        }
        else
        {
            /* we would have overflowed -- set the overflow flag */
            flags |= stqOverflow;
        }

        c = *p++; /* read next digit */
    }

    --p; /* point to place that stopped scan */

    if ( !( flags & stqReadDigit ) )
    {
        /* no number there; return 0 and point to beginning of string */
        if (endptr)
        {
            /* store beginning of string in endptr later on */
            p = nptr;
        }
        number = 0L; /* return 0 */
    }
    else if (
        ( flags & stqOverflow ) ||
        (
            !( flags & stqUnsigned ) &&
            (
                ( ( flags & stqNegative ) && ( number > -_I64_MIN ) ) || ( !( flags & stqNegative ) && ( number > _I64_MAX ) ) 
            ) 
        ) 
    )
    {
        /* overflow or signed overflow occurred */
        errno = ERANGE;
        if ( flags & stqUnsigned )
            number = _UI64_MAX;
        else if ( flags & stqNegative )
            number = _I64_MIN;
        else
            number = _I64_MAX;
    }

    if ( endptr != NULL )
    {
        /* store pointer to char that stopped the scan */
        *endptr = p;
    }

    if ( flags & stqNegative )
    {
        /* negate result if there was a neg sign */
        number = (uint64)(-(int64)number);
    }

    return number; /* done. */
}

template <>
WINUX_FUNC_IMPL(uint64) StrToXq( char const * nptr, char const ** endptr, int ibase, int flags )
{
    return Impl_StrToXq( nptr, endptr, ibase, flags );
}

template <>
WINUX_FUNC_IMPL(uint64) StrToXq( wchar const * nptr, wchar const ** endptr, int ibase, int flags )
{
    return Impl_StrToXq( nptr, endptr, ibase, flags );
}

template <>
WINUX_FUNC_IMPL(uint64) StrToXq( char16 const * nptr, char16 const ** endptr, int ibase, int flags )
{
    return Impl_StrToXq( nptr, endptr, ibase, flags );
}

template <>
WINUX_FUNC_IMPL(uint64) StrToXq( char32 const * nptr, char32 const ** endptr, int ibase, int flags )
{
    return Impl_StrToXq( nptr, endptr, ibase, flags );
}


template <>
WINUX_FUNC_IMPL(int64) StrToInt64( XString<char> const & numStr, int ibase )
{
    return (int64)Impl_StrToXq<char>( numStr.c_str(), nullptr, ibase, 0 );
}

template <>
WINUX_FUNC_IMPL(int64) StrToInt64( XString<wchar> const & numStr, int ibase )
{
    return (int64)Impl_StrToXq<wchar>( numStr.c_str(), nullptr, ibase, 0 );
}

template <>
WINUX_FUNC_IMPL(int64) StrToInt64( XString<char16> const & numStr, int ibase )
{
    return (int64)Impl_StrToXq<char16>( numStr.c_str(), nullptr, ibase, 0 );
}

template <>
WINUX_FUNC_IMPL(int64) StrToInt64( XString<char32> const & numStr, int ibase )
{
    return (int64)Impl_StrToXq<char32>( numStr.c_str(), nullptr, ibase, 0 );
}


template <>
WINUX_FUNC_IMPL(uint64) StrToUInt64( XString<char> const & numStr, int ibase )
{
    return Impl_StrToXq<char>( numStr.c_str(), nullptr, ibase, stqUnsigned );
}

template <>
WINUX_FUNC_IMPL(uint64) StrToUInt64( XString<wchar> const & numStr, int ibase )
{
    return Impl_StrToXq<wchar>( numStr.c_str(), nullptr, ibase, stqUnsigned );
}

template <>
WINUX_FUNC_IMPL(uint64) StrToUInt64( XString<char16> const & numStr, int ibase )
{
    return Impl_StrToXq<char16>( numStr.c_str(), nullptr, ibase, stqUnsigned );
}

template <>
WINUX_FUNC_IMPL(uint64) StrToUInt64( XString<char32> const & numStr, int ibase )
{
    return Impl_StrToXq<char32>( numStr.c_str(), nullptr, ibase, stqUnsigned );
}

// add/strip slashes internal functions ------------------------------------------------------------------
template < typename _ChTy >
inline static bool IsSpecial( _ChTy ch )
{
    return
        ch == Literal<_ChTy>::aChar ||
        ch == Literal<_ChTy>::bChar ||
        ch == Literal<_ChTy>::tChar ||
        ch == Literal<_ChTy>::nChar ||
        ch == Literal<_ChTy>::vChar ||
        ch == Literal<_ChTy>::fChar ||
        ch == Literal<_ChTy>::rChar
    ;
}

template < typename _ChTy >
inline static _ChTy NumberStringToChar( _ChTy const * number, int base )
{
    _ChTy const * endptr;
    return (_ChTy)Impl_StrToXq<_ChTy>( number, &endptr, base, 0 );
}

template < typename _ChTy >
inline static _ChTy SpecialToChar( _ChTy special )
{
    switch ( special )
    {
    case Literal<_ChTy>::aChar: // 响铃(BEL) 07H/7
        special = Literal<_ChTy>::belChar;
        break;
    case Literal<_ChTy>::bChar: // 退格符(BS) 08H/8
        special = Literal<_ChTy>::bsChar;
        break;
    case Literal<_ChTy>::tChar: // 水平制表符(HT) 09H/9
        special = Literal<_ChTy>::htChar;
        break;
    case Literal<_ChTy>::nChar: // 换行符(LF) 0AH/10
        special = Literal<_ChTy>::lfChar;
        break;
    case Literal<_ChTy>::vChar: // 垂直制表(VT) 0BH/11
        special = Literal<_ChTy>::vtChar;
        break;
    case Literal<_ChTy>::fChar: // 换页符(FF) 0CH/12
        special = Literal<_ChTy>::ffChar;
        break;
    case Literal<_ChTy>::rChar: // 回车符(CR) 0DH/13
        special = Literal<_ChTy>::crChar;
        break;
    }
    return special;
}

template < size_t _Size > struct UT;
template <> struct UT<1> { using type = uint8; };
template <> struct UT<2> { using type = uint16; };
template <> struct UT<4> { using type = uint32; };
template <> struct UT<8> { using type = uint64; };

template < typename _ChTy >
inline static XString<_ChTy> Impl_AddSlashes( XString<_ChTy> const & str, XString<_ChTy> const & charlist )
{
    XString<_ChTy> slashes;
    for ( auto it = str.begin(); it != str.end(); it++ )
    {
        _ChTy ch = *it;
        if ( charlist.find(ch) != XString<_ChTy>::npos )
        {
            XString<_ChTy> slash;
            switch ( ch )
            {
            case Literal<_ChTy>::belChar: // 响铃(BEL)  07H/7
                slash = Literal<_ChTy>::slash_aStr;
                break;
            case Literal<_ChTy>::bsChar: // 退格符(BS) 08H/8
                slash = Literal<_ChTy>::slash_bStr;
                break;
            case Literal<_ChTy>::htChar: // 水平制表符(HT) 09H/9
                slash = Literal<_ChTy>::slash_tStr;
                break;
            case Literal<_ChTy>::lfChar: // 换行符(LF) 0AH/10
                slash = Literal<_ChTy>::slash_nStr;
                break;
            case Literal<_ChTy>::vtChar: // 垂直制表(VT) 0BH/11
                slash = Literal<_ChTy>::slash_vStr;
                break;
            case Literal<_ChTy>::ffChar: // 换页符(FF) 0CH/12
                slash = Literal<_ChTy>::slash_fStr;
                break;
            case Literal<_ChTy>::crChar: // 回车符(CR) 0DH/13
                slash = Literal<_ChTy>::slash_rStr;
                break;
            default:
                {
                    using MyUT = typename UT<sizeof(_ChTy)>::type;
                    if ( IsSpecial(ch) || ch <= Literal<_ChTy>::spaceChar || (MyUT)ch > (MyUT)0x7f )
                    {
                        slash = Literal<_ChTy>::slash_xStr + CharToHexStr<_ChTy>(ch);
                    }
                    else
                    {
                        slash += Literal<_ChTy>::slashChar;
                        slash += ch;
                    }
                }
                break;
            }

            slashes += slash;
        }
        else
        {
            slashes += ch;
        }
    }
    return slashes;
}

template <>
WINUX_FUNC_IMPL(XString<char>) AddSlashes( XString<char> const & str, XString<char> const & charlist )
{
    return Impl_AddSlashes( str, charlist );
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) AddSlashes( XString<wchar> const & str, XString<wchar> const & charlist )
{
    return Impl_AddSlashes( str, charlist );
}

template <>
WINUX_FUNC_IMPL(XString<char16>) AddSlashes( XString<char16> const & str, XString<char16> const & charlist )
{
    return Impl_AddSlashes( str, charlist );
}

template <>
WINUX_FUNC_IMPL(XString<char32>) AddSlashes( XString<char32> const & str, XString<char32> const & charlist )
{
    return Impl_AddSlashes( str, charlist );
}


template < typename _ChTy >
inline static XString<_ChTy> Impl_StripSlashes( XString<_ChTy> const & str, XString<_ChTy> const & charlist )
{
    using MyUT = typename UT< sizeof(_ChTy) >::type;

    XString<_ChTy> result;
    size_t octMaxLen = (size_t)ceil( log((MyUT)(-1)) / log(8) );
    size_t hexMaxLen = (size_t)ceil( log((MyUT)(-1)) / log(16) );

    for ( auto it = str.begin(); it != str.end(); )
    {
        _ChTy const current = *it;
        if ( current == Literal<_ChTy>::slashChar )
        {
            it++; // skip '\\'
            if ( it != str.end() ) // \后有字符
            {
                for ( ; it != str.end(); it++ )
                {
                    _ChTy ch0 = *it; // ch0表示\后的一个字符
                    if ( IsOct(ch0) )
                    {
                        XString<_ChTy> ch0s;
                        for ( ; it != str.end(); it++ )
                        {
                            ch0 = *it;
                            if ( IsOct(ch0) && ch0s.length() < octMaxLen )
                            {
                                ch0s += ch0;
                            }
                            else
                            {
                                break;
                            }
                        }
                        if ( ch0s.length() > 0 )
                        {
                            _ChTy c1 = NumberStringToChar( ch0s.c_str(), 8 );
                            if ( InCharList( c1, charlist ) )
                            {
                                result += c1;
                            }
                            else
                            {
                                result += Literal<_ChTy>::slashChar;
                                result += ch0s;
                            }
                        }

                        break;
                    }
                    else if ( ch0 == Literal<_ChTy>::xChar ) // is x 16进制
                    {
                        it++; // skip 'x'
                        if ( it != str.end() )
                        {
                            if ( IsHex(*it) )
                            {
                                XString<_ChTy> ch0s;
                                for ( ; it != str.end(); it++ )
                                {
                                    ch0 = *it;
                                    if ( IsHex(ch0) && ch0s.length() < hexMaxLen )
                                    {
                                        ch0s += ch0;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }

                                if ( ch0s.length() > 0 )
                                {
                                    _ChTy c2 = NumberStringToChar( ch0s.c_str(), 16 );
                                    if ( InCharList( c2, charlist ) )
                                    {
                                        result += c2;
                                        break;
                                    }
                                    else
                                    {
                                        result += Literal<_ChTy>::slash_xStr;
                                        result += ch0s;
                                        break;
                                    }
                                }
                                break;
                            }
                            else // x后不是16进制字符
                            {
                                result += Literal<_ChTy>::slash_xStr;
                                break;
                            }
                        }
                        else // 后面已经没有字符
                        {
                            result += Literal<_ChTy>::slash_xStr;
                            break;
                        }
                    }
                    else if ( IsSpecial(ch0) )
                    {
                        _ChTy special = SpecialToChar(ch0);
                        // 查看CharList是否含有此字符
                        if ( InCharList( special, charlist ) )
                        {
                            result += special;
                            it++; // skip ch0
                            break;
                        }
                        else
                        {
                            result += Literal<_ChTy>::slashChar;
                            result += ch0;
                            it++; // skip ch0
                            break;
                        }
                    }
                    else if ( InCharList( ch0, charlist ) )
                    {
                        result += ch0;
                        it++; // skip ch0
                        break;
                    }
                    else // 都不是
                    {
                        result += Literal<_ChTy>::slashChar;
                        break;
                    }
                } // end for loop
            }
            else // \后没有字符
            {
                result += current;
            }
        }
        else // 当前字符不是 \ .
        {
            result += current;
            it++;
        }
    }
    return result;
}

template <>
WINUX_FUNC_IMPL(XString<char>) StripSlashes( XString<char> const & str, XString<char> const & charlist )
{
    return Impl_StripSlashes( str, charlist );
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) StripSlashes( XString<wchar> const & str, XString<wchar> const & charlist )
{
    return Impl_StripSlashes( str, charlist );
}

template <>
WINUX_FUNC_IMPL(XString<char16>) StripSlashes( XString<char16> const & str, XString<char16> const & charlist )
{
    return Impl_StripSlashes( str, charlist );
}

template <>
WINUX_FUNC_IMPL(XString<char32>) StripSlashes( XString<char32> const & str, XString<char32> const & charlist )
{
    return Impl_StripSlashes( str, charlist );
}


template < typename _ChTy >
inline static XString<_ChTy> Impl_AddQuotes( XString<_ChTy> const & str, _ChTy quote )
{
    XString<_ChTy> res;
    using MyConstIterator = typename XString<_ChTy>::const_iterator;
    MyConstIterator it;
    for ( it = str.begin(); it != str.end(); ++it )
    {
        if ( *it == quote )
        {
            res += XString<_ChTy>( 1, quote ) + *it;
        }
        else
        {
            res += *it;
        }
    }
    return res;
}

template <>
WINUX_FUNC_IMPL(XString<char>) AddQuotes( XString<char> const & str, char quote )
{
    return Impl_AddQuotes( str, quote );
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) AddQuotes( XString<wchar> const & str, wchar quote )
{
    return Impl_AddQuotes( str, quote );
}

template <>
WINUX_FUNC_IMPL(XString<char16>) AddQuotes( XString<char16> const & str, char16 quote )
{
    return Impl_AddQuotes( str, quote );
}

template <>
WINUX_FUNC_IMPL(XString<char32>) AddQuotes( XString<char32> const & str, char32 quote )
{
    return Impl_AddQuotes( str, quote );
}


template < typename _ChTy >
inline static bool Impl_StrGetLine( XString<_ChTy> * line, _ChTy const * str, size_t len, size_t * pI, XString<_ChTy> * nl )
{
    size_t & i = *pI;
    if ( i >= len ) return false;

    line->clear();

    _ChTy nlchs[3] = { 0 }; // 换行符
    size_t start = i;

    while ( i < len )
    {
        _ChTy ch = str[i];
        switch ( ch )
        {
        case Literal<_ChTy>::lfChar:
        case Literal<_ChTy>::crChar:
            {
                if ( i - start > 0 )
                {
                    line->assign( str + start, i - start ); // 不包括 \r \n
                }

                switch ( ch )
                {
                case Literal<_ChTy>::lfChar:
                    nlchs[0] = ch;
                    ++i; // skip '\n'
                    break;
                case Literal<_ChTy>::crChar:
                    nlchs[0] = ch;
                    ++i; // skip '\r'
                    if ( i < len && ( ch = str[i] ) == Literal<_ChTy>::lfChar )
                    {
                        nlchs[1] = ch;
                        ++i; // skip '\n'
                    }
                    break;
                }

                goto RETURN;
            }
            break;
        default:
            ++i;
            break;
        }
    }

    if ( i == len )
    {
        line->assign( str + start, i - start );
    }

RETURN:
    if ( nl != nullptr )
    {
        *nl = nlchs;
    }
    else
    {
        *line += nlchs;
    }
    return true;
}

template <>
WINUX_FUNC_IMPL(bool) StrGetLine( XString<char> * line, XString<char> const & str, size_t * i, XString<char> * nl )
{
    return Impl_StrGetLine( line, str.c_str(), str.length(), i, nl );
}

template <>
WINUX_FUNC_IMPL(bool) StrGetLine( XString<wchar> * line, XString<wchar> const & str, size_t * i, XString<wchar> * nl )
{
    return Impl_StrGetLine( line, str.c_str(), str.length(), i, nl );
}

template <>
WINUX_FUNC_IMPL(bool) StrGetLine( XString<char16> * line, XString<char16> const & str, size_t * i, XString<char16> * nl )
{
    return Impl_StrGetLine( line, str.c_str(), str.length(), i, nl );
}

template <>
WINUX_FUNC_IMPL(bool) StrGetLine( XString<char32> * line, XString<char32> const & str, size_t * i, XString<char32> * nl )
{
    return Impl_StrGetLine( line, str.c_str(), str.length(), i, nl );
}

template <>
WINUX_FUNC_IMPL(bool) StrGetLine( XString<char> * line, char const * str, size_t len, size_t * i, XString<char> * nl )
{
    return Impl_StrGetLine( line, str, len, i, nl );
}

template <>
WINUX_FUNC_IMPL(bool) StrGetLine( XString<wchar> * line, wchar const * str, size_t len, size_t * i, XString<wchar> * nl )
{
    return Impl_StrGetLine( line, str, len, i, nl );
}

template <>
WINUX_FUNC_IMPL(bool) StrGetLine( XString<char16> * line, char16 const * str, size_t len, size_t * i, XString<char16> * nl )
{
    return Impl_StrGetLine( line, str, len, i, nl );
}

template <>
WINUX_FUNC_IMPL(bool) StrGetLine( XString<char32> * line, char32 const * str, size_t len, size_t * i, XString<char32> * nl )
{
    return Impl_StrGetLine( line, str, len, i, nl );
}


template < typename _ChTy >
inline static void Impl_GetLineByPos( XString<_ChTy> const & content, ssize_t pos, ssize_t * lineHead, ssize_t * lineTail, XString<_ChTy> * line )
{
    if ( content[pos] == Literal<_ChTy>::lfChar )
    {
        *lineHead = pos - 1;
        *lineTail = pos;
    }
    else
    {
        *lineHead = pos;
        *lineTail = pos + 1;
    }

    while ( *lineHead > -1 && content[*lineHead] != Literal<_ChTy>::lfChar ) --*lineHead; // 向前找'\n'
    ++*lineHead; // skip '\n' or 恢复索引到0

    while ( *lineTail < (ssize_t)content.length() && content[*lineTail] != Literal<_ChTy>::lfChar ) ++*lineTail; // 向后找'\n'
    ASSIGN_PTR(line) = content.substr( *lineHead, *lineTail - *lineHead );
    if ( *lineTail > 0 ) --*lineTail; // 指向尾字符而不是END
}

template <>
WINUX_FUNC_IMPL(void) GetLineByPos( XString<char> const & content, ssize_t pos, ssize_t * lineHead, ssize_t * lineTail, XString<char> * line )
{
    return Impl_GetLineByPos( content, pos, lineHead, lineTail, line );
}

template <>
WINUX_FUNC_IMPL(void) GetLineByPos( XString<wchar> const & content, ssize_t pos, ssize_t * lineHead, ssize_t * lineTail, XString<wchar> * line )
{
    return Impl_GetLineByPos( content, pos, lineHead, lineTail, line );
}

template <>
WINUX_FUNC_IMPL(void) GetLineByPos( XString<char16> const & content, ssize_t pos, ssize_t * lineHead, ssize_t * lineTail, XString<char16> * line )
{
    return Impl_GetLineByPos( content, pos, lineHead, lineTail, line );
}

template <>
WINUX_FUNC_IMPL(void) GetLineByPos( XString<char32> const & content, ssize_t pos, ssize_t * lineHead, ssize_t * lineTail, XString<char32> * line )
{
    return Impl_GetLineByPos( content, pos, lineHead, lineTail, line );
}


// 整理单词
template < typename _ChTy >
inline static XString<_ChTy> & Impl_CollateWord( XString<_ChTy> * word, winux::uint flags )
{
    if ( word->empty() ) return *word;
    switch ( flags & 0xF )
    {
    case wordAllUpper:
        Impl_StrMakeUpper(word);
        break;
    case wordAllLower:
        Impl_StrMakeLower(word);
        break;
    case wordFirstCharUpper:
        {
            Impl_StrMakeLower(word);
            auto & ch = word->at(0);
            if ( IsLowerAlphabet(ch) )
            {
                ch &= ~0x20;
            }
        }
        break;
    default:
        break;
    }
    return *word;
}

template < typename _ChTy >
inline static void Impl_StoreWordToIdentifierString( XString<_ChTy> * resIdentifier, XString<_ChTy> & tmpWord, XString<_ChTy> const & sep, winux::uint flags )
{
    Impl_CollateWord( &tmpWord, flags );
    switch ( flags )
    {
    case nameLowerCamelCase:
        if ( resIdentifier->empty() )
        {
            if ( !tmpWord.empty() ) tmpWord[0] |= 0x20;
            *resIdentifier += tmpWord;
        }
        break;
    case nameUpperCamelCase: // or namePascalCase
        *resIdentifier += tmpWord;
        break;
    case nameSnakeCase:
        *resIdentifier += tmpWord + ( sep.empty() ? Literal<_ChTy>::nulStr : Literal<_ChTy>::underStr ); // "_"
        break;
    case nameKebabCase:
        *resIdentifier += tmpWord + ( sep.empty() ? Literal<_ChTy>::nulStr : Literal<_ChTy>::negativeStr ); // "-"
        break;
    case nameScreamingSnakeCase:
        *resIdentifier += tmpWord + ( sep.empty() ? Literal<_ChTy>::nulStr : Literal<_ChTy>::underStr ); // "_"
        break;
    default:
        *resIdentifier += tmpWord + sep;
        break;
    }
}

template < typename _ChTy >
inline static void Impl_StoreWordToArray( XStringArray<_ChTy> * resWords, XString<_ChTy> & tmpWord, winux::uint flags )
{
    Impl_CollateWord( &tmpWord, flags );
    switch ( flags )
    {
    case nameLowerCamelCase:
        if ( resWords->empty() )
        {
            if ( !tmpWord.empty() ) tmpWord[0] |= 0x20;
            resWords->push_back(tmpWord);
        }
        break;
    default:
        resWords->push_back(tmpWord);
        break;
    }
}

enum _StoreWordType
{
    storeIdentifier,
    storeArray
};

template < typename _ChTy >
inline static void Impl_CollateIdentifierToX(
    XString<_ChTy> const & identifier,
    XString<_ChTy> const & sep,
    winux::uint flags,
    _StoreWordType storeType,
    XString<_ChTy> * resIdentifier,
    XStringArray<_ChTy> * resWords
)
{
    XString<_ChTy> tmpWord;

    // 拆词算法
    for ( size_t i = 0; i < identifier.length(); )
    {
        _ChTy ch = identifier[i];

        if ( IsUpperAlphabet(ch) ) // 是大写字母
        {
            if ( tmpWord.empty() )
            {
                tmpWord += ch;
                i++;
            }
            else
            {
                if ( i + 1 < identifier.length() )
                {
                    if ( IsLowerAlphabet(identifier[i + 1]) ) // 后一个字符是小写字符，则说明该字符是一个单词的开头。
                    {
                        switch ( storeType )
                        {
                        case winux::storeIdentifier:
                            Impl_StoreWordToIdentifierString( resIdentifier, tmpWord, sep, flags );
                            break;
                        case winux::storeArray:
                            Impl_StoreWordToArray( resWords, tmpWord, flags );
                            break;
                        }
                        tmpWord = ch;
                    }
                    else
                    {
                        if ( IsLowerAlphabet(tmpWord[tmpWord.length() - 1]) )
                        {
                            switch ( storeType )
                            {
                            case winux::storeIdentifier:
                                Impl_StoreWordToIdentifierString( resIdentifier, tmpWord, sep, flags );
                                break;
                            case winux::storeArray:
                                Impl_StoreWordToArray( resWords, tmpWord, flags );
                                break;
                            }
                            tmpWord = ch;
                        }
                        else
                        {
                            tmpWord += ch;
                        }
                    }
                }
                else
                {
                    tmpWord += ch;
                }

                i++;
            }
        }
        else if ( IsLowerAlphabet(ch) ) // 是小写字母
        {
            tmpWord += ch;
            i++;
        }
        else if ( IsDelimChar(ch) ) // 是分隔符
        {
            switch ( storeType )
            {
            case winux::storeIdentifier:
                Impl_StoreWordToIdentifierString( resIdentifier, tmpWord, sep, flags );
                break;
            case winux::storeArray:
                Impl_StoreWordToArray( resWords, tmpWord, flags );
                break;
            }
            tmpWord.clear();
            i++;
        }
        else // 是其他字符 
        {
            tmpWord += ch;
            i++;
        }
    }

    if ( !tmpWord.empty() )
    {
        switch ( storeType )
        {
        case winux::storeIdentifier:
            Impl_StoreWordToIdentifierString<_ChTy>( resIdentifier, tmpWord, Literal<_ChTy>::nulStr, flags );
            break;
        case winux::storeArray:
            Impl_StoreWordToArray<_ChTy>( resWords, tmpWord, flags );
            break;
        }
    }
}

template < typename _ChTy >
inline static XString<_ChTy> Impl_CollateIdentifierToString( XString<_ChTy> const & identifier, XString<_ChTy> const & sep, winux::uint flags )
{
    XString<_ChTy> resIdentifier;
    Impl_CollateIdentifierToX<_ChTy>( identifier, sep, flags, storeIdentifier, &resIdentifier, nullptr );
    return resIdentifier;
}

template <>
WINUX_FUNC_IMPL(XString<char>) CollateIdentifierToString( XString<char> const & identifier, XString<char> const & sep, winux::uint flags )
{
    return Impl_CollateIdentifierToString( identifier, sep, flags );
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) CollateIdentifierToString( XString<wchar> const & identifier, XString<wchar> const & sep, winux::uint flags )
{
    return Impl_CollateIdentifierToString( identifier, sep, flags );
}

template <>
WINUX_FUNC_IMPL(XString<char16>) CollateIdentifierToString( XString<char16> const & identifier, XString<char16> const & sep, winux::uint flags )
{
    return Impl_CollateIdentifierToString( identifier, sep, flags );
}

template <>
WINUX_FUNC_IMPL(XString<char32>) CollateIdentifierToString( XString<char32> const & identifier, XString<char32> const & sep, winux::uint flags )
{
    return Impl_CollateIdentifierToString( identifier, sep, flags );
}

template < typename _ChTy >
inline static XStringArray<_ChTy> Impl_CollateIdentifierToArray( XString<_ChTy> const & identifier, winux::uint flags )
{
    XStringArray<_ChTy> resWords;
    Impl_CollateIdentifierToX<_ChTy>( identifier, Literal<_ChTy>::nulStr, flags, storeArray, nullptr, &resWords );
    return resWords;
}

template <>
WINUX_FUNC_IMPL(XStringArray<char>) CollateIdentifierToArray( XString<char> const & identifier, winux::uint flags )
{
    return Impl_CollateIdentifierToArray( identifier, flags );
}

template <>
WINUX_FUNC_IMPL(XStringArray<wchar>) CollateIdentifierToArray( XString<wchar> const & identifier, winux::uint flags )
{
    return Impl_CollateIdentifierToArray( identifier, flags );
}

template <>
WINUX_FUNC_IMPL(XStringArray<char16>) CollateIdentifierToArray( XString<char16> const & identifier, winux::uint flags )
{
    return Impl_CollateIdentifierToArray( identifier, flags );
}

template <>
WINUX_FUNC_IMPL(XStringArray<char32>) CollateIdentifierToArray( XString<char32> const & identifier, winux::uint flags )
{
    return Impl_CollateIdentifierToArray( identifier, flags );
}

// class MultiMatch ---------------------------------------------------------------------------
MultiMatch::MultiMatch() : _replaceFunc(NULL), _extra(NULL)
{
}

MultiMatch::MultiMatch( StringArray const & matches, StringArray const & replaces ) : _replaceFunc(NULL), _extra(NULL)
{
    this->init( matches, replaces );
}

MultiMatch::MultiMatch( StringArray const & matches, ReplaceFuncType replaceFunc, void * extra ) : _replaceFunc(NULL), _extra(NULL)
{
    this->init( matches, replaceFunc, extra );
}

void MultiMatch::init( StringArray const & matches, StringArray const & replaces )
{
    for ( size_t i = 0; i < matches.size(); ++i ) _states.emplace_back();
    _matchItems = matches;
    for ( auto it = _matchItems.begin(); it != _matchItems.end(); it++ )
    {
        _nextVals.push_back( _Templ_KmpCalcNext<short>( it->c_str(), it->length() ) );
    }
    _replaceItems = replaces;
    _replaceFunc = NULL;
    _extra = NULL;
}

void MultiMatch::init( StringArray const & matches, ReplaceFuncType replaceFunc, void * extra )
{
    for ( size_t i = 0; i < matches.size(); ++i ) _states.emplace_back();
    _matchItems = matches;
    for ( auto it = _matchItems.begin(); it != _matchItems.end(); it++ )
    {
        _nextVals.push_back( _Templ_KmpCalcNext<short>( it->c_str(), it->length() ) );
    }
    _replaceFunc = replaceFunc;
    _extra = extra;
}

ssize_t MultiMatch::addMatchReplacePair( String const & match, String const & replace )
{
    _states.emplace_back();
    _matchItems.push_back(match);
    _nextVals.push_back( _Templ_KmpCalcNext<short>( match.c_str(), match.length() ) );
    _replaceItems.push_back(replace);
    return (ssize_t)_nextVals.size();
}

ssize_t MultiMatch::addMatch( String const & match )
{
    _states.emplace_back();
    _matchItems.push_back(match);
    _nextVals.push_back( _Templ_KmpCalcNext<short>( match.c_str(), match.length() ) );
    return (ssize_t)_nextVals.size();
}

String const & MultiMatch::getMatchItem( ssize_t item ) const
{
    return _matchItems[item];
}

void MultiMatch::setMatchItem( ssize_t item, String const & match )
{
    _matchItems[item] = match;
    _nextVals[item] = _Templ_KmpCalcNext<short>( match.c_str(), match.length() );
}

String MultiMatch::getReplaceItem( ssize_t item ) const
{
    if ( _replaceFunc )
        return _replaceFunc( this, item, _extra );
    else
        return _replaceItems[item];
}

void MultiMatch::setReplaceItem( ssize_t item, String const & replace )
{
    _replaceItems[item] = replace;
}

void MultiMatch::setReplaceItems( StringArray const & replaces )
{
    _replaceItems = replaces;
}

MultiMatch::ReplaceFuncType MultiMatch::setReplaceFunc( ReplaceFuncType newReplaceFunc, void * extra )
{
    ReplaceFuncType oldFunc = _replaceFunc;
    _replaceFunc = newReplaceFunc;
    _extra = extra;
    return oldFunc;
}

void MultiMatch::clear()
{
    this->_states.clear();
    this->_matchItems.clear();
    this->_nextVals.clear();
    this->_replaceItems.clear();
    this->_extra = NULL;
    this->_replaceFunc = NULL;
}

MultiMatch::MatchResult MultiMatch::search( String const & str, ssize_t offset ) const
{
    MatchResult res = { -1, -1 };
    ssize_t count = (ssize_t)_matchItems.size();
    for ( auto & state : this->_states ) state.reset();
    ssize_t i; // 主字符串当前字符位置
    bool nomove = false; // 不移动i
    for ( i = offset; i < (ssize_t)str.length(); )
    {
        for ( ssize_t curr = 0; curr < count; curr++ ) // 各个匹配项进行匹配
        {
            auto & state = this->_states[curr];

            if ( i < state.markpos ) continue;

            auto & currItem = _matchItems[curr];
            auto currItemLen = (ssize_t)currItem.length();

            if ( state.j < currItemLen )
            {
                if ( str[i] == currItem[state.j] )
                {
                    state.j++;
                    state.markpos = i + 1;

                    if ( state.j == currItemLen ) // 表示当前项已经匹配成功
                    {
                        res.item = curr;
                        res.pos = i + 1 - currItemLen;
                        goto RETURN;
                    }
                }
                else
                {
                    state.j = _nextVals[curr][state.j];

                    if ( state.j == -1 )
                    {
                        state.j = 0;
                        state.markpos = i + 1;
                    }
                    else
                    {
                        nomove = true;
                    }
                }
            }
        }

        if ( nomove )
        {
            nomove = false;
        }
        else
        {
            i++;
        }
    }

RETURN:
    return res;
}

MultiMatch::MatchResult MultiMatch::greedSearch( String const & str, ssize_t offset ) const
{
    MatchResult res = { -1, -1 };
    ssize_t count = (ssize_t)_matchItems.size();
    for ( auto & state : this->_states ) state.reset();
    ssize_t i; // 主字符串当前字符位置
    bool nomove = false; // 不移动i
    bool hasMatched = false; // 已有匹配
    for ( i = offset; i < (ssize_t)str.length(); )
    {
        for ( ssize_t curr = 0; curr < count; curr++ ) // 各个匹配项进行匹配
        {
            auto & currItem = _matchItems[curr];
            auto currItemLen = (ssize_t)currItem.length();
            auto & state = this->_states[curr];

            if ( state.matched || i < state.markpos ) continue;

            if ( state.j < currItemLen ) // 模式串尚未匹配完全
            {
                if ( str[i] == currItem[state.j] )
                {
                    state.prevCharMatch = true;

                    state.j++;
                    state.markpos = i + 1;

                    if ( state.j == currItemLen ) // 表示模式串匹配成功
                    {
                        res.item = state.mr.item = curr;
                        res.pos = state.mr.pos = i + 1 - currItemLen;
                        state.matched = true;
                        hasMatched = true;
                        //goto RETURN;
                    }
                }
                else
                {
                    state.prevCharMatch = false;

                    state.j = _nextVals[curr][state.j];

                    if ( state.j == -1 )
                    {
                        state.j = 0;
                        state.markpos = i + 1;
                    }
                    else
                    {
                        nomove = true;
                    }
                }
            }
        }

        if ( hasMatched )
        {
            // 有匹配，并且其余prevCharMatch都是false，才可终止
            size_t cPrevCharMatches = 0;
            size_t matchMaxLen = 0;
            for ( auto & state : this->_states )
            {
                if ( state.matched )
                {
                    // 选最长的match
                    if ( state.j > (ssize_t)matchMaxLen )
                    {
                        matchMaxLen = state.j;
                        res = state.mr;
                    }
                }
                else
                {
                    if ( state.prevCharMatch ) cPrevCharMatches++;
                }
            }
            if ( cPrevCharMatches == 0 )
            {
                goto RETURN;
            }
        }

        if ( nomove )
        {
            nomove = false;
        }
        else
        {
            i++;
        }
    }

RETURN:
    return res;
}

MultiMatch::MatchResult MultiMatch::commonSearch( String const & str, ssize_t offset ) const
{
    MatchResult r = { -1, -1 };
    for ( size_t i = offset; i < str.length(); ++i )
    {
        size_t matchItemCount = _matchItems.size();
        for ( size_t curr = 0; curr < matchItemCount; ++curr )
        {
            auto & matchItem = _matchItems[curr];

            if ( (ssize_t)str.length() - (ssize_t)i < (ssize_t)matchItem.length() )
                continue;

            bool isMatch = true;
            for ( size_t j = 0; j < matchItem.length(); ++j )
            {
                if ( str[j + i] != matchItem[j] )
                {
                    isMatch = false;
                    break;
                }
            }
            if ( isMatch )
            {
                r.item = (ssize_t)curr;
                r.pos = (ssize_t)i;
                return r;
            }
        }
    }
    return r;
}

String MultiMatch::replace( String const & str, ssize_t offset, SearchFuncType fnSearch ) const
{
    ssize_t start = 0;
    String s = TEXT("");
    MatchResult r = (this->*fnSearch)( str, start + offset );
    while ( r.pos != -1 )
    {
        s += str.substr( start, r.pos - start );

        if ( _replaceFunc )
            s += _replaceFunc( this, r.item, _extra );
        else
            s += _replaceItems[r.item];

        start = r.pos + (ssize_t)_matchItems[r.item].length();
        r = (this->*fnSearch)( str, start );
    }
    s += str.substr(start);
    return s;
}

// ---------------------------------------------------------------------------
// 本地字符串和unicode字符串互转支持
// ---------------------------------------------------------------------------
AnsiString Locale::_clsLc = "";

Locale::Locale( char const * lc )
{
    _loc = lc ? lc : _clsLc;
    _prevLoc = setlocale( LC_ALL, NULL );
    setlocale( LC_ALL, _loc.c_str() );
}

Locale::~Locale()
{
    setlocale( LC_ALL, _prevLoc.c_str() );
}

#if !defined(OS_WIN)
size_t mbslen( char const * str, int size )
{
    Locale __setLoc;
    size_t cch = 0;
    char const * p = str;
    int oneCharSize = -1;
    while ( p - str < size && ( oneCharSize = mblen( p, MB_CUR_MAX ) ) > 0 )
    {
        p += oneCharSize;
        cch++;
    }
    return cch;
}
#endif

WINUX_FUNC_IMPL(size_t) LocalCharsCount( AnsiString const & local )
{
    Locale __setLoc;
    size_t cnt = mbstowcs( NULL, local.c_str(), 0 );
    if ( cnt != (size_t)-1 ) return cnt;
#if defined(_MSC_VER) || defined(WIN32)
    return _mbslen( (unsigned char *)local.c_str() );
#else
    return mbslen( local.c_str(), local.length() );
#endif
}

WINUX_FUNC_IMPL(size_t) UnicodeMinLength( UnicodeString const & unicode )
{
    Locale __setLoc;
    size_t len = wcstombs( NULL, unicode.c_str(), 0 );
    if ( len != (size_t)-1 ) return len;
    len = 0;
    for ( auto it = unicode.begin(); it != unicode.end(); ++it )
        len += ((unsigned int)*it) > 0xFFU ? 2 : 1;
    return len;
}

WINUX_FUNC_IMPL(AnsiString) UnicodeToLocal( UnicodeString const & unicode )
{
    Locale __setLoc;
    AnsiString r;
    r.resize( UnicodeMinLength(unicode) + 1 );
    size_t n = wcstombs( &r[0], unicode.c_str(), r.size() );
    return AnsiString( r.c_str(), n );
}

WINUX_FUNC_IMPL(UnicodeString) LocalToUnicode( AnsiString const & local )
{
    Locale __setLoc;
    UnicodeString r;
    r.resize( LocalCharsCount(local) + 1 );
    size_t n = mbstowcs( &r[0], local.c_str(), local.length() );
    return UnicodeString( r.c_str(), n );
}

WINUX_FUNC_IMPL(String) LocalToString( AnsiString const & local )
{
#ifdef UNICODE
    return LocalToUnicode(local);
#else
    return local;
#endif
}

WINUX_FUNC_IMPL(String) UnicodeToString( UnicodeString const & unicode )
{
#ifdef UNICODE
    return unicode;
#else
    return UnicodeToLocal(unicode);
#endif
}

WINUX_FUNC_IMPL(AnsiString) StringToLocal( String const & str )
{
#ifdef UNICODE
    return UnicodeToLocal(str);
#else
    return str;
#endif
}

WINUX_FUNC_IMPL(UnicodeString) StringToUnicode( String const & str )
{
#ifdef UNICODE
    return str;
#else
    return LocalToUnicode(str);
#endif
}

AnsiString FormatExVA( size_t cch, char const * fmt, va_list args )
{
    AnsiString str;
    str.resize( cch + 1 );
    _vsnprintf( &str[0], cch, fmt, args );
    return str.c_str();
}

UnicodeString FormatExVW( size_t cch, wchar const * fmt, va_list args )
{
    UnicodeString str;
    str.resize( cch + 1 );
    _vsnwprintf( &str[0], cch, fmt, args );
    return str.c_str();
}

AnsiString FormatExA( size_t cch, char const * fmt, ... )
{
    va_list args;
    va_start( args, fmt );
    return FormatExVA( cch, fmt, args );
}

UnicodeString FormatExW( size_t cch, wchar const * fmt, ... )
{
    va_list args;
    va_start( args, fmt );
    return FormatExVW( cch, fmt, args );
}

AnsiString FormatA( char const * fmt, ... )
{
    va_list args;
    va_start( args, fmt );

    #if defined(_MSC_VER) || defined(WIN32)
    int c = _vscprintf( fmt, args );
    return FormatExVA( c, fmt, args );
    #else
    char * buf = NULL;
    vasprintf( &buf, fmt, args );
    AnsiString s = buf;
    free(buf);
    return s;
    #endif
}

UnicodeString FormatW( wchar const * fmt, ... )
{
    va_list args;
    va_start( args, fmt );

    #if defined(_MSC_VER) || defined(WIN32)
    int c = _vscwprintf( fmt, args );
    return FormatExVW( c, fmt, args );
    #else
    int c = 4096;
    return FormatExVW( c, fmt, args );
    #endif
}

// 兼容版实现
#if defined(_UNICODE) || defined(UNICODE)
UnicodeString FormatEx( size_t cch, wchar const * fmt, ... )
{
    va_list args;
    va_start( args, fmt );
    return FormatExV( cch, fmt, args );
}

UnicodeString Format( wchar const * fmt, ... )
{
    va_list args;
    va_start( args, fmt );

    #if defined(_MSC_VER) || defined(WIN32)
    int c = _vscwprintf( fmt, args );
    return FormatExV( c, fmt, args );
    #else
    int c = 4096;
    return FormatExV( c, fmt, args );
    #endif
}
#else
AnsiString FormatEx( size_t cch, char const * fmt, ... )
{
    va_list args;
    va_start( args, fmt );
    return FormatExV( cch, fmt, args );
}

AnsiString Format( char const * fmt, ... )
{
    va_list args;
    va_start( args, fmt );

    #if defined(_MSC_VER) || defined(WIN32)
    int c = _vscprintf( fmt, args );
    return FormatExV( c, fmt, args );
    #else
    char * buf = NULL;
    vasprintf( &buf, fmt, args );
    AnsiString s = buf;
    free(buf);
    return s;
    #endif
}
#endif

// class SZInput ------------------------------------------------------------------------------------
SZInput & SZInput::operator = ( char const * pstr )
{
    pstr = pstr ? pstr : "";
    switch ( _type )
    {
    case szCharInput:
        {
            size_t uTextLen = strlen(pstr);
            if ( _count - 1 < uTextLen )
            {
                memcpy( _psz, pstr, sizeof(char) * ( _count - 1 ) );
                _psz[_count - 1] = 0;
            }
            else
            {
                memcpy( _psz, pstr, sizeof(char) * uTextLen );
                _psz[uTextLen] = 0;
            }
        }
        break;
    case szWCharInput:
        {
            UnicodeString str = LocalToUnicode(pstr);
            size_t uTextLen = str.length();
            if ( _count - 1 < uTextLen )
            {
                memcpy( _pwsz, str.c_str(), sizeof(wchar_t) * ( _count - 1 ) );
                _pwsz[_count - 1] = 0;
            }
            else
            {
                memcpy( _pwsz, str.c_str(), sizeof(wchar_t) * uTextLen );
                _pwsz[uTextLen] = 0;
            }
        }
        break;
    }
    return *this;
}

SZInput & SZInput::operator = ( wchar_t const * pwstr )
{
    pwstr = pwstr ? pwstr : L"";
    switch ( _type )
    {
    case szCharInput:
        {
            AnsiString str = UnicodeToLocal(pwstr);
            size_t uTextLen = str.length();
            if ( _count - 1 < uTextLen )
            {
                memcpy( _psz, str.c_str(), sizeof(char) * ( _count - 1 ) );
                _psz[_count - 1] = 0;
            }
            else
            {
                memcpy( _psz, str.c_str(), sizeof(char) * uTextLen );
                _psz[uTextLen] = 0;
            }
        }
        break;
    case szWCharInput:
        {
            size_t uTextLen = wcslen(pwstr);
            if ( _count - 1 < uTextLen )
            {
                memcpy( _pwsz, pwstr, sizeof(wchar_t) * ( _count - 1 ) );
                _pwsz[_count - 1] = 0;
            }
            else
            {
                memcpy( _pwsz, pwstr, sizeof(wchar_t) * uTextLen );
                _pwsz[uTextLen] = 0;
            }
        }
        break;
    }
    return *this;
}

// struct Conv_Data -----------------------------------------------------------------------
struct Conv_Data
{
#if defined(__GNUC__) || defined(HAVE_ICONV)
    //转换句柄
    iconv_t _cd;
#else
    uint _fromCP;
    uint _toCP;
#endif

};

// class Conv -----------------------------------------------------------------------------
#if !defined(__GNUC__) && !defined(HAVE_ICONV)
static struct __ConvLangCodePage
{
    std::map< AnsiString, uint > _convLangCP;
    __ConvLangCodePage()
    {
        _convLangCP[""] = CP_ACP;
        _convLangCP["ASCII"] = CP_ACP;
        _convLangCP["CHAR"] = CP_ACP;
        _convLangCP["UTF-7"] = CP_UTF7;
        _convLangCP["UTF-8"] = CP_UTF8;
        _convLangCP["WCHAR_T"] = 1200;
        _convLangCP["UCS-2LE"] = 1200;
        _convLangCP["UTF-16LE"] = 1200;
        _convLangCP["UCS-2"] = 1201;
        _convLangCP["UTF-16"] = 1201;
        _convLangCP["UCS-2BE"] = 1201;
        _convLangCP["UTF-16BE"] = 1201;
        _convLangCP["GBK"] = 936;
        _convLangCP["SHIFT_JIS"] = 932;
        _convLangCP["BIG5"] = 950;
    }

    // 根据语言串获取代码页编码
    uint operator [] ( AnsiString cp ) const
    {
        if ( !cp.empty() ) StrMakeUpper(&cp);

        if ( cp.length() > 1 && cp.substr( 0, 2 ) == "CP" )
        {
            return atoi( cp.c_str() + 2 );
        }
        if ( isset( _convLangCP, cp ) )
        {
            return _convLangCP.at(cp);
        }
        return 0;
    }
    // 是否为宽字符编码
    bool isWideChar( uint cp ) const { return cp == 1200 || cp == 1201; }
    // 判断该代码页在执行WideCharToMultiByte()和MultiByteToWideChar()时dwFlags是否必须为0
    bool isFlagMustZero( uint cp ) const
    {
        return
            cp == 50220 ||
            cp == 50221 ||
            cp == 50222 ||
            cp == 50225 ||
            cp == 50227 ||
            cp == 50229 ||
            cp == 52936 ||
            cp == 54936 ||
            cp >= 57002 && cp <= 57011 ||
            cp == 65000 ||
            cp == 65001;
    }

} __LangCP;

// UCS-2 大小端次序转换
static void __Ucs2LeBe( wchar * inOutBuf, size_t cch )
{
    for ( size_t i = 0; i < cch; ++i )
    {
        union
        {
            wchar ch;
            struct
            {
                byte low;
                byte hig;
            };
        } a, b;
        a.ch = inOutBuf[i];
        b.low = a.hig;
        b.hig = a.low;
        inOutBuf[i] = b.ch;
    }
}

// 多字节到宽字符，*str2自动分配内存，用户负责Buffer.Free()。返回写入*str2缓冲区的字节数。
static size_t __MbsToWcs( uint cp1, char const * str1, size_t size1, uint cp2, wchar * * str2 )
{
    // 你得先获得缓冲区大小，字符数
    size_t cch = MultiByteToWideChar(
        cp1,
        0,
        str1,
        (int)size1,
        NULL,
        0
    );
    size_t size2 = sizeof(wchar) * ( cch + 1 ) ;
    *str2 = (wchar *)Buffer::Alloc(size2); // 分配内存
    memset( *str2, 0, size2 );
    // 这才进行转换
    size2 = sizeof(wchar) * MultiByteToWideChar(
        cp1,
        0,
        str1,
        (int)size1,
        *str2,
        (int)( cch + 1 )
    );
    // 如果是UCS-2BE
    if ( cp2 == 1201 )
    {
        __Ucs2LeBe( *str2, cch );
    }
    return size2;
}

// 宽字符到多字节，注意：size1必须是字节数（即宽字符数*sizeof(wchar)），*str2自动分配内存，用户负责Buffer.Free()。返回写入*str2缓冲区的字节数。
static size_t __WcsToMbs( uint cp1, wchar const * str1, size_t size1, uint cp2, char * * str2 )
{
    UnicodeString strTmp( str1, size1 / sizeof(wchar) );
    if ( cp1 == 1201 )
    {
        if ( !strTmp.empty() ) __Ucs2LeBe( &strTmp[0], (uint)strTmp.length() );
        str1 = strTmp.c_str();
    }

    size_t length = WideCharToMultiByte(
        cp2,
        0 /*| ( cp2 == CP_UTF8 ? WC_ERR_INVALID_CHARS : 0 )*/,
        str1,
        (int)( size1 / sizeof(wchar) ),
        NULL,
        0,
        NULL,
        NULL
    );
    size_t size2 = sizeof(char) * ( length + 1 );
    *str2 = (char *)Buffer::Alloc(size2);
    memset( *str2, 0, size2 );

    size2 = WideCharToMultiByte(
        cp2,
        0 /*| ( cp2 == CP_UTF8 ? WC_ERR_INVALID_CHARS : 0 )*/,
        str1,
        (int)( size1 / sizeof(wchar) ),
        *str2,
        (int)( length + 1 ),
        NULL,
        NULL
    );
    if ( size2 == 0 )
    {
        //LOG( Format("GetLastError() %X", GetLastError() ) );
    }

    return size2;
}

// 多字节字符到多字节字符，*str2自动分配内存，用户负责Buffer.Free()，返回写入*str2缓冲区的字节数
static size_t __MbsToMbs( uint cp1, char const * str1, size_t size1, uint cp2, char * * str2 )
{
    // 首先要转到Unicode
    wchar * tmpUcs;
    size_t tmpUcsSize = __MbsToWcs( cp1, str1, size1, 0, &tmpUcs );
    // 然后再转换成多字节
    size_t size2 = __WcsToMbs( 0, tmpUcs, tmpUcsSize, cp2, str2 );
    Buffer::Free(tmpUcs);

    return size2;
}

// 字符串编码转换，返回写入*str2缓冲区的字节数
static size_t __StrConvert( uint cp1, char const * str1, size_t size1, uint cp2, char ** str2 )
{
    size_t size2 = 0;
    // 4种方式,1 m to w, 2 w to m, 3 m to m, 4 w to w
    if ( __LangCP.isWideChar(cp1) ) // cp1 is w
    {
        if ( __LangCP.isWideChar(cp2) ) // cp2 is w
        {
            // w to w, directly copy
            size_t cch1 = size1 / sizeof(wchar);
            size2 = cch1 * sizeof(wchar);
            *str2 = (char *)Buffer::Alloc(size2);
            memset( *str2, 0, size2 );
            memcpy( *str2, str1, size1 );
            if ( cp1 != cp2 )
            {
                __Ucs2LeBe( (wchar *)*str2, cch1 );
            }
            return cch1 * sizeof(wchar);
        }
        else // cp2 is m
        {
            // w to m
            return __WcsToMbs( cp1, (wchar *)str1, size1, cp2, str2 );
        }
    }
    else // cp1 is m
    {
        if ( __LangCP.isWideChar(cp2) ) // cp2 is w
        {
            return __MbsToWcs( cp1, str1, size1, cp2, (wchar **)str2 );
        }
        else // cp2 is m
        {
            return __MbsToMbs( cp1, str1, size1, cp2, str2 );
        }
    }
    return 0;
}

#endif

Conv::Conv( AnsiString const & fromCode, AnsiString const & toCode )
{
    _self.create(); //

#if defined(__GNUC__) || defined(HAVE_ICONV)
    _self->_cd = iconv_open( toCode.c_str(), fromCode.c_str() );

    #if _LIBICONV_VERSION > 0x0108
    int optval;
    // 字译功能
    optval = 1;
    iconvctl( _self->_cd, ICONV_SET_TRANSLITERATE, &optval );
    // 丢弃无效序列
    optval = 1;
    iconvctl( _self->_cd, ICONV_SET_DISCARD_ILSEQ, &optval );
    #endif

#else
    _self->_fromCP = __LangCP[fromCode];
    _self->_toCP = __LangCP[toCode];
#endif
}

Conv::~Conv()
{
#if defined(__GNUC__) || defined(HAVE_ICONV)
    iconv_close(_self->_cd);
#endif

    _self.destroy(); //
}

size_t Conv::convert( char const * srcBuf, size_t srcSize, char * * destBuf )
{
#if defined(__GNUC__) || defined(HAVE_ICONV)
    size_t r = 0, outBytes = 0, err = 0;
    size_t destSize = srcSize + 1;
    *destBuf = nullptr;
    do
    {
        char * srcP = (char *)srcBuf;
        size_t srcN = srcSize;
        destSize <<= 1;
        *destBuf = (char *)Buffer::Realloc( *destBuf, destSize );
        char * buf = *destBuf;
        size_t outBytesLeft = destSize;
        memset( *destBuf, 0, destSize );
        // 返回不可逆转换的字符个数
        r = iconv( _self->_cd, &srcP, &srcN, &buf, &outBytesLeft );
        outBytes = destSize - outBytesLeft; // 求得输出的字节数
        err = errno;
    } while ( r == -1 && err == E2BIG );
    return outBytes;
#else
    return __StrConvert( _self->_fromCP, srcBuf, (uint)srcSize, _self->_toCP, destBuf );
#endif
}

// class UnicodeConverter static methods ------------------------------------------------------
inline static void _Utf8_ToUtf16( Utf16String * p, char const * str, size_t len, size_t newCap = 0 )
{
    if ( newCap )
    {
        p->reserve(newCap);
    }
    else
    {
        p->reserve( len * 2 / 3 );
    }

    size_t index = 0;
    uint32 codepoint;
    while ( index < len )
    {
        auto ch = str[index];
        index++;
        if ( ( ch & 0xC0 ) != 0x80 ) // UTF-8字符的起始字节
        {
            if ( ( ch & 0x80 ) == 0x00 ) // 1字节UTF-8编码
            {
                codepoint = ch;
            }
            else if ( ( ch & 0xE0 ) == 0xC0 ) // 2字节UTF-8编码
            {
                codepoint = ch & 0x1F;
            }
            else if ( ( ch & 0xF0 ) == 0xE0 ) // 3字节UTF-8编码
            {
                codepoint = ch & 0x0F;
            }
            else if ( ( ch & 0xF8 ) == 0xF0 ) // 4字节UTF-8编码
            {
                codepoint = ch & 0x07;
            }

            // 读取后续字节
            size_t bytes = ( ch & 0xE0 ) == 0xC0 ? 1 : ( ch & 0xF0 ) == 0xE0 ? 2 : ( ch & 0xF8 ) == 0xF0 ? 3 : 0;
            for ( size_t i = 0; i < bytes; ++i )
            {
                codepoint = ( codepoint << 6 ) | ( str[index] & 0x3F );
                index++;
            }

            // 转换UTF-32码点为UTF-16编码
            if ( codepoint <= 0xFFFF ) // BMP字符
            {
                *p += (char16)codepoint;
            }
            else // 非BMP字符，使用代理对
            {
                codepoint -= 0x10000;

                *p += (char16)( ( codepoint >> 10 ) + 0xD800 );
                *p += (char16)( ( codepoint & 0x3FF ) + 0xDC00 );
            }
        }
    }
}
inline static void _Utf8_ToUtf32( Utf32String * p, char const * str, size_t len, size_t newCap = 0 )
{
    if ( newCap )
    {
        p->reserve(newCap);
    }
    else
    {
        p->reserve( len * 1 / 2 );
    }

    size_t index = 0;
    uint32 codepoint;
    while ( index < len )
    {
        auto ch = str[index];
        index++;
        if ( ( ch & 0xC0 ) != 0x80 ) // UTF-8字符的起始字节
        {
            if ( ( ch & 0x80 ) == 0x00 ) // 1字节UTF-8编码
            {
                codepoint = ch;
            }
            else if ( ( ch & 0xE0 ) == 0xC0 ) // 2字节UTF-8编码
            {
                codepoint = ch & 0x1F;
            }
            else if ( ( ch & 0xF0 ) == 0xE0 ) // 3字节UTF-8编码
            {
                codepoint = ch & 0x0F;
            }
            else if ( ( ch & 0xF8 ) == 0xF0 ) // 4字节UTF-8编码
            {
                codepoint = ch & 0x07;
            }

            // 读取后续字节
            size_t bytes = ( ch & 0xE0 ) == 0xC0 ? 1 : ( ch & 0xF0 ) == 0xE0 ? 2 : ( ch & 0xF8 ) == 0xF0 ? 3 : 0;
            for ( size_t i = 0; i < bytes; ++i )
            {
                codepoint = ( codepoint << 6 ) | ( str[index] & 0x3F );
                index++;
            }

            *p += (char32)codepoint;
        }
    }
}

inline static void _Utf16_ToUtf8( Utf8String * p, char16 const * str, size_t len, size_t newCap = 0 )
{
    if ( newCap )
    {
        p->reserve(newCap);
    }
    else
    {
        p->reserve( len * 5 / 2 );
    }

    size_t index = 0;
    uint32 codepoint;
    while ( index < len )
    {
        auto ch = str[index];
        auto ch2 = str[index + 1];
        if ( ( ch >= 0xD800 && ch <= 0xDBFF ) && ( ch2 >= 0xDC00 && ch2 <= 0xDFFF ) )
        {
            // 代理对，4字节UTF-8编码
            // 计算UTF-32码点
            codepoint = ( ( ch - 0xD800 ) << 10 ) + ( ch2 - 0xDC00 ) + 0x10000;
            *p += (char)( 0xF0 | ( codepoint >> 18 ) );
            *p += (char)( 0x80 | ( ( codepoint >> 12 ) & 0x3F ) );
            *p += (char)( 0x80 | ( ( codepoint >> 6 ) & 0x3F ) );
            *p += (char)( 0x80 | ( codepoint & 0x3F ) );
            index += 2;
        }
        else if ( ch <= 0x7F )
        {
            // 1字节UTF-8编码
            *p += (char)ch;
            index += 1;
        }
        else if ( ch <= 0x07FF )
        {
            // 2字节UTF-8编码
            *p += (char)( 0xC0 | ( ch >> 6 ) );
            *p += (char)( 0x80 | ( ch & 0x3F ) );
            index += 1;
        }
        else
        {
            // 3字节UTF-8编码
            *p += (char)( 0xE0 | ( ch >> 12 ) );
            *p += (char)( 0x80 | ( ( ch >> 6 ) & 0x3F ) );
            *p += (char)( 0x80 | ( ch & 0x3F ) );
            index += 1;
        }
    }
}
inline static void _Utf16_ToUtf32( Utf32String * p, char16 const * str, size_t len, size_t newCap = 0 )
{
    if ( newCap )
    {
        p->reserve(newCap);
    }
    else
    {
        p->reserve( len * 4 / 5 );
    }

    size_t index = 0;
    uint32 codepoint;
    while ( index < len )
    {
        auto ch = str[index];
        auto ch2 = str[index + 1];
        if ( ( ch >= 0xD800 && ch <= 0xDBFF ) && ( ch2 >= 0xDC00 && ch2 <= 0xDFFF ) )
        {
            // 代理对，4字节UTF-8编码
            // 计算UTF-32码点
            codepoint = ( ( ch - 0xD800 ) << 10 ) + ( ch2 - 0xDC00 ) + 0x10000;
            *p += (char32)codepoint;
            index += 2;
        }
        else
        {
            // 1~3字节UTF-8编码
            *p += (char32)ch;
            index += 1;
        }
    }
}

inline static void _Utf32_ToUtf8( Utf8String * p, char32 const * str, size_t len, size_t newCap = 0 )
{
    if ( newCap )
    {
        p->reserve(newCap);
    }
    else
    {
        p->reserve( len * 3 );
    }

    size_t index = 0;
    while ( index < len )
    {
        auto ch = str[index];
        if ( ch <= 0x7F )
        {
            // 1字节UTF-8编码
            *p += (char)ch;
            index += 1;
        }
        else if ( ch <= 0x07FF )
        {
            // 2字节UTF-8编码
            *p += (char)( 0xC0 | ( ch >> 6 ) );
            *p += (char)( 0x80 | ( ch & 0x3F ) );
            index += 1;
        }
        else if ( ch <= 0xFFFF )
        {
            // 3字节UTF-8编码
            *p += (char)( 0xE0 | ( ch >> 12 ) );
            *p += (char)( 0x80 | ( ( ch >> 6 ) & 0x3F ) );
            *p += (char)( 0x80 | ( ch & 0x3F ) );
            index += 1;
        }
        else
        {
            // 4字节UTF-8编码
            *p += (char)( 0xF0 | ( ch >> 18 ) );
            *p += (char)( 0x80 | ( ( ch >> 12 ) & 0x3F ) );
            *p += (char)( 0x80 | ( ( ch >> 6 ) & 0x3F ) );
            *p += (char)( 0x80 | ( ch & 0x3F ) );
            index += 1;
        }
    }
}
inline static void _Utf32_ToUtf16( Utf16String * p, char32 const * str, size_t len, size_t newCap = 0 )
{
    if ( newCap )
    {
        p->reserve(newCap);
    }
    else
    {
        p->reserve( len * 3 / 2 );
    }

    size_t index = 0;
    while ( index < len )
    {
        auto ch = str[index];
        if ( ch <= 0xFFFF ) // BMP字符
        {
            *p += (char16)ch;
            index += 1;
        }
        else // 非BMP字符，使用代理对
        {
            ch -= 0x10000;

            *p += (char16)( ( ch >> 10 ) + 0xD800 );
            *p += (char16)( ( ch & 0x3FF ) + 0xDC00 );
            index += 1;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
inline static size_t _Utf8_CalcUtf16Length( char const * str, size_t len )
{
    size_t utf16length = 0;
    for ( size_t i = 0; i < len; i++ )
    {
        char ch = str[i];
        if ( ( ch & 0xC0 ) != 0x80 ) // UTF-8字符的起始字节
        {
            if ( ( ch & 0xF8 ) == 0xF0 ) // 4字节UTF-8编码
            {
                utf16length += 2;
            }
            else
            {
                utf16length++;
            }
        }
    }
    return utf16length;
}
inline static size_t _Utf8_CalcUtf32Length( char const * str, size_t len )
{
    size_t utf32length = 0;
    for ( size_t i = 0; i < len; ++i )
    {
        if ( ( str[i] & 0xC0 ) != 0x80 ) // UTF-8字符的起始字节，或ANSI字符
        {
            utf32length++;
        }
    }
    return utf32length;
}

inline static size_t _Utf16_CalcUtf8Length( char16 const * str, size_t len )
{
    size_t utf8length = 0;

    for ( size_t i = 0; i < len; )
    {
        if ( ( str[i] >= 0xD800 && str[i] <= 0xDBFF ) && ( str[i + 1] >= 0xDC00 && str[i + 1] <= 0xDFFF ) )
        {
            utf8length += 4; // 代理对，4字节UTF-8编码
            i += 2;
        }
        else if ( str[i] <= 0x7F )
        {
            utf8length += 1; // 1字节UTF-8编码
            i += 1;
        }
        else if ( str[i] <= 0x07FF )
        {
            utf8length += 2; // 2字节UTF-8编码
            i += 1;
        }
        else
        {
            utf8length += 3; // 3字节UTF-8编码
            i += 1;
        }
    }
    return utf8length;
}
inline static size_t _Utf16_CalcUtf32Length( char16 const * str, size_t len )
{
    size_t utf32length = 0;
    for ( size_t i = 0; i < len; )
    {
        if ( ( str[i] >= 0xD800 && str[i] <= 0xDBFF ) && ( str[i + 1] >= 0xDC00 && str[i + 1] <= 0xDFFF ) )
        {
            i += 2;
        }
        else
        {
            i++;
        }
        utf32length++;
    }
    return utf32length;
}

inline static size_t _Utf32_CalcUtf8Length( char32 const * str, size_t len )
{
    size_t utf8length = 0;
    for ( size_t i = 0; i < len; i++ )
    {
        if ( str[i] < 0x80 )
        {
            utf8length++;
        }
        else if ( str[i] < 0x0800 )
        {
            utf8length += 2;
        }
        else if ( str[i] < 0x10000 )
        {
            utf8length += 3;
        }
        else
        {
            utf8length += 4;
        }
    }
    return utf8length;
}
inline static size_t _Utf32_CalcUtf16Length( char32 const * str, size_t len )
{
    size_t utf16length = 0;
    for ( size_t i = 0; i < len; i++ )
    {
        if ( str[i] < 0x10000 )
        {
            utf16length++;
        }
        else
        {
            utf16length += 2;
        }
    }
    return utf16length;
}


///////////////////////////////////////////////////////////////////////////////////////////
// 用模板特化解决`wchar_t`平台位长不同的问题
///////////////////////////////////////////////////////////////////////////////////////////
template < size_t >
struct WCharT;

template <>
struct WCharT< sizeof(char16) >
{
    static void _Utf8_ToUnicode( UnicodeString * p, char const * str, size_t len, size_t newCap = 0 )
    {
        _Utf8_ToUtf16( (Utf16String *)p, str, len, newCap );
    }

    static void _Unicode_ToUtf8( Utf8String * p, wchar const * str, size_t len, size_t newCap = 0 )
    {
        _Utf16_ToUtf8( p, (char16 const *)str, len, newCap );
    }
    static void _Unicode_ToUtf16( Utf16String * p, wchar const * str, size_t len, size_t newCap = 0 )
    {
        p->assign( (char16 const *)str, len );
    }
    static void _Unicode_ToUtf32( Utf32String * p, wchar const * str, size_t len, size_t newCap = 0 )
    {
        _Utf16_ToUtf32( p, (char16 const *)str, len, newCap );
    }

    static void _Utf16_ToUnicode( UnicodeString * p, char16 const * str, size_t len, size_t newCap = 0 )
    {
        p->assign( (wchar const *)str, len );
    }

    static void _Utf32_ToUnicode( UnicodeString * p, char32 const * str, size_t len, size_t newCap = 0 )
    {
        _Utf32_ToUtf16( (Utf16String *)p, str, len, newCap );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    static size_t _Utf8_CalcUnicodeLength( char const * str, size_t len ) { return _Utf8_CalcUtf16Length( str, len ); }

    static size_t _Unicode_CalcUtf8Length( wchar const * str, size_t len ) { return _Utf16_CalcUtf8Length( (char16 const *)str, len ); }
    static size_t _Unicode_CalcUtf16Length( wchar const * str, size_t len ) { return len; }
    static size_t _Unicode_CalcUtf32Length( wchar const * str, size_t len ) { return _Utf16_CalcUtf32Length( (char16 const *)str, len ); }

    static size_t _Utf16_CalcUnicodeLength( char16 const * str, size_t len ) { return len; }

    static size_t _Utf32_CalcUnicodeLength( char32 const * str, size_t len ) { return _Utf32_CalcUtf16Length( str, len ); }
};

template <>
struct WCharT< sizeof(char32) >
{
    static void _Utf8_ToUnicode( UnicodeString * p, char const * str, size_t len, size_t newCap = 0 )
    {
        _Utf8_ToUtf32( (Utf32String *)p, str, len, newCap );
    }

    static void _Unicode_ToUtf8( Utf8String * p, wchar const * str, size_t len, size_t newCap = 0 )
    {
        _Utf32_ToUtf8( p, (char32 const *)str, len, newCap );
    }
    static void _Unicode_ToUtf16( Utf16String * p, wchar const * str, size_t len, size_t newCap = 0 )
    {
        _Utf32_ToUtf16( p, (char32 const *)str, len, newCap );
    }
    static void _Unicode_ToUtf32( Utf32String * p, wchar const * str, size_t len, size_t newCap = 0 )
    {
        p->assign( (char32 const *)str, len );
    }

    static void _Utf16_ToUnicode( UnicodeString * p, char16 const * str, size_t len, size_t newCap = 0 )
    {
        _Utf16_ToUtf32( (Utf32String *)p, str, len, newCap );
    }

    static void _Utf32_ToUnicode( UnicodeString * p, char32 const * str, size_t len, size_t newCap = 0 )
    {
        p->assign( (wchar const *)str, len );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    static size_t _Utf8_CalcUnicodeLength( char const * str, size_t len ) { return _Utf8_CalcUtf32Length( str, len ); }

    static size_t _Unicode_CalcUtf8Length( wchar const * str, size_t len ) { return _Utf32_CalcUtf8Length( (char32 const *)str, len ); }
    static size_t _Unicode_CalcUtf16Length( wchar const * str, size_t len ) { return _Utf32_CalcUtf16Length( (char32 const *)str, len ); }
    static size_t _Unicode_CalcUtf32Length( wchar const * str, size_t len ) { return len; }

    static size_t _Utf16_CalcUnicodeLength( char16 const * str, size_t len ) { return _Utf16_CalcUtf32Length( str, len ); }

    static size_t _Utf32_CalcUnicodeLength( char32 const * str, size_t len ) { return len; }
};

///////////////////////////////////////////////////////////////////////////////////////////
inline static void _Utf8_ToUnicode( UnicodeString * p, char const * str, size_t len, size_t newCap = 0 )
{
    WCharT<sizeof(wchar)>::_Utf8_ToUnicode( p, str, len, newCap );
}

inline static void _Unicode_ToUtf8( Utf8String * p, wchar const * str, size_t len, size_t newCap = 0 )
{
    WCharT<sizeof(wchar)>::_Unicode_ToUtf8( p, str, len, newCap );
}
inline static void _Unicode_ToUtf16( Utf16String * p, wchar const * str, size_t len, size_t newCap = 0 )
{
    WCharT<sizeof(wchar)>::_Unicode_ToUtf16( p, str, len, newCap );
}
inline static void _Unicode_ToUtf32( Utf32String * p, wchar const * str, size_t len, size_t newCap = 0 )
{
    WCharT<sizeof(wchar)>::_Unicode_ToUtf32( p, str, len, newCap );
}

inline static void _Utf16_ToUnicode( UnicodeString * p, char16 const * str, size_t len, size_t newCap = 0 )
{
    WCharT<sizeof(wchar)>::_Utf16_ToUnicode( p, str, len, newCap );
}

inline static void _Utf32_ToUnicode( UnicodeString * p, char32 const * str, size_t len, size_t newCap = 0 )
{
    WCharT<sizeof(wchar)>::_Utf32_ToUnicode( p, str, len, newCap );
}

///////////////////////////////////////////////////////////////////////////////////////////
inline static size_t _Utf8_CalcUnicodeLength( char const * str, size_t len )
{
    return WCharT<sizeof(wchar)>::_Utf8_CalcUnicodeLength( str, len );
}

inline static size_t _Unicode_CalcUtf8Length( wchar const * str, size_t len )
{
    return WCharT<sizeof(wchar)>::_Unicode_CalcUtf8Length( str, len );
}
inline static size_t _Unicode_CalcUtf16Length( wchar const * str, size_t len )
{
    return WCharT<sizeof(wchar)>::_Unicode_CalcUtf16Length( str, len );
}
inline static size_t _Unicode_CalcUtf32Length( wchar const * str, size_t len )
{
    return WCharT<sizeof(wchar)>::_Unicode_CalcUtf32Length( str, len );
}

inline static size_t _Utf16_CalcUnicodeLength( char16 const * str, size_t len )
{
    return WCharT<sizeof(wchar)>::_Utf16_CalcUnicodeLength( str, len );
}

inline static size_t _Utf32_CalcUnicodeLength( char32 const * str, size_t len )
{
    return WCharT<sizeof(wchar)>::_Utf32_CalcUnicodeLength( str, len );
}

// class UnicodeConverter methods -------------------------------------------------------------
UnicodeConverter::UnicodeConverter( Utf8String const & utf8str )
{
    _p = &utf8str;
    _type = ucUtf8;
}

UnicodeConverter::UnicodeConverter( UnicodeString const & wstr )
{
    _p = &wstr;
    _type = ucUnicode;
}

UnicodeConverter::UnicodeConverter( Utf16String const & utf16str )
{
    _p = &utf16str;
    _type = ucUtf16;
}

UnicodeConverter::UnicodeConverter( Utf32String const & utf32str )
{
    _p = &utf32str;
    _type = ucUtf32;
}

Utf8String UnicodeConverter::toUtf8( size_t newCap ) const
{
    switch ( _type )
    {
    case ucUtf8:
        return *_q<Utf8String>();
        break;
    case ucUnicode:
        {
            Utf8String s;
            _Unicode_ToUtf8( &s, _q<UnicodeString>()->c_str(), _q<UnicodeString>()->length(), newCap );
            return s;
        }
        break;
    case ucUtf16:
        {
            Utf8String s;
            _Utf16_ToUtf8( &s, _q<Utf16String>()->c_str(), _q<Utf16String>()->length(), newCap );
            return s;
        }
        break;
    case ucUtf32:
        {
            Utf8String s;
            _Utf32_ToUtf8( &s, _q<Utf32String>()->c_str(), _q<Utf32String>()->length(), newCap );
            return s;
        }
        break;
    default:
        return u8"";
        break;
    }
}

UnicodeString UnicodeConverter::toUnicode( size_t newCap ) const
{
    switch ( _type )
    {
    case ucUtf8:
        {
            UnicodeString s;
            _Utf8_ToUnicode( &s, _q<Utf8String>()->c_str(), _q<Utf8String>()->length(), newCap );
            return s;
        }
        break;
    case ucUnicode:
        return *_q<UnicodeString>();
        break;
    case ucUtf16:
        {
            UnicodeString s;
            _Utf16_ToUnicode( &s, _q<Utf16String>()->c_str(), _q<Utf16String>()->length(), newCap );
            return s;
        }
        break;
    case ucUtf32:
        {
            UnicodeString s;
            _Utf32_ToUnicode( &s, _q<Utf32String>()->c_str(), _q<Utf32String>()->length(), newCap );
            return s;
        }
        break;
    default:
        return L"";
        break;
    }
}

Utf16String UnicodeConverter::toUtf16( size_t newCap ) const
{
    switch ( _type )
    {
    case ucUtf8:
        {
            Utf16String s;
            _Utf8_ToUtf16( &s, _q<Utf8String>()->c_str(), _q<Utf8String>()->length(), newCap );
            return s;
        }
        break;
    case ucUnicode:
        {
            Utf16String s;
            _Unicode_ToUtf16( &s, _q<UnicodeString>()->c_str(), _q<UnicodeString>()->length(), newCap );
            return s;
        }
        break;
    case ucUtf16:
        return *_q<Utf16String>();
        break;
    case ucUtf32:
        {
            Utf16String s;
            _Utf32_ToUtf16( &s, _q<Utf32String>()->c_str(), _q<Utf32String>()->length(), newCap );
            return s;
        }
        break;
    default:
        return u"";
        break;
    }
}

Utf32String UnicodeConverter::toUtf32( size_t newCap ) const
{
    switch ( _type )
    {
    case ucUtf8:
        {
            Utf32String s;
            _Utf8_ToUtf32( &s, _q<Utf8String>()->c_str(), _q<Utf8String>()->length(), newCap );
            return s;
        }
        break;
    case ucUnicode:
        {
            Utf32String s;
            _Unicode_ToUtf32( &s, _q<UnicodeString>()->c_str(), _q<UnicodeString>()->length(), newCap );
            return s;
        }
        break;
    case ucUtf16:
        {
            Utf32String s;
            _Utf16_ToUtf32( &s, _q<Utf16String>()->c_str(), _q<Utf16String>()->length(), newCap );
            return s;
        }
        break;
    case ucUtf32:
        return *_q<Utf32String>();
        break;
    default:
        return U"";
        break;
    }
}

size_t UnicodeConverter::calcUtf8Length() const
{
    switch ( _type )
    {
    case ucUtf8:
        return _q<Utf8String>()->length();
        break;
    case ucUnicode:
        return _Unicode_CalcUtf8Length( _q<UnicodeString>()->c_str(), _q<UnicodeString>()->length() );
        break;
    case ucUtf16:
        return _Utf16_CalcUtf8Length( _q<Utf16String>()->c_str(), _q<Utf16String>()->length() );
        break;
    case ucUtf32:
        return _Utf32_CalcUtf8Length( _q<Utf32String>()->c_str(), _q<Utf32String>()->length() );
        break;
    default:
        return 0;
        break;
    }
}

size_t UnicodeConverter::calcUnicodeLength() const
{
    switch ( _type )
    {
    case ucUtf8:
        return _Utf8_CalcUnicodeLength( _q<Utf8String>()->c_str(), _q<Utf8String>()->length() );
        break;
    case ucUnicode:
        return _q<UnicodeString>()->length();
        break;
    case ucUtf16:
        return _Utf16_CalcUnicodeLength( _q<Utf16String>()->c_str(), _q<Utf16String>()->length() );
        break;
    case ucUtf32:
        return _Utf32_CalcUnicodeLength( _q<Utf32String>()->c_str(), _q<Utf32String>()->length() );
        break;
    default:
        return 0;
        break;
    }
}

size_t UnicodeConverter::calcUtf16Length() const
{
    switch ( _type )
    {
    case ucUtf8:
        return _Utf8_CalcUtf16Length( _q<Utf8String>()->c_str(), _q<Utf8String>()->length() );
        break;
    case ucUnicode:
        return _Unicode_CalcUtf16Length( _q<UnicodeString>()->c_str(), _q<UnicodeString>()->length() );
        break;
    case ucUtf16:
        return _q<Utf16String>()->length();
        break;
    case ucUtf32:
        return _Utf32_CalcUtf16Length( _q<Utf32String>()->c_str(), _q<Utf32String>()->length() );
        break;
    default:
        return 0;
        break;
    }
}

size_t UnicodeConverter::calcUtf32Length() const
{
    switch ( _type )
    {
    case ucUtf8:
        return _Utf8_CalcUtf32Length( _q<Utf8String>()->c_str(), _q<Utf8String>()->length() );
        break;
    case ucUnicode:
        return _Unicode_CalcUtf32Length( _q<UnicodeString>()->c_str(), _q<UnicodeString>()->length() );
        break;
    case ucUtf16:
        return _Utf16_CalcUtf32Length( _q<Utf16String>()->c_str(), _q<Utf16String>()->length() );
        break;
    case ucUtf32:
        return _q<Utf32String>()->length();
        break;
    default:
        return 0;
        break;
    }
}

WINUX_FUNC_IMPL(AnsiString) LocalFromUtf8( AnsiString const & str )
{
    ConvFrom<AnsiString> conv("UTF-8");
    return conv(str);
}

WINUX_FUNC_IMPL(AnsiString) LocalToUtf8( AnsiString const & str )
{
    ConvTo<AnsiString> conv("UTF-8");
    return conv(str);
}


} // namespace winux
