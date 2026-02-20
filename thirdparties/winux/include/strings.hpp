#ifndef __STRINGS_HPP__
#define __STRINGS_HPP__
//
// strings 提供字符串相关的功能
//

#include <stdarg.h>

namespace winux
{
/** \brief C字符串转到XString
 *
 *  \param str C字符串。如果是nullptr则处理成空串""
 *  \param len 长度。默认npos，表示遇到NUL字符结束 */
template < typename _ChTy >
inline static XString<_ChTy> CharSeqToString( _ChTy const * str, size_t len = npos )
{
    str = str ? str : winux::Literal<_ChTy>::nulStr;
    return (ssize_t)len < 0 ? XString<_ChTy>(str) : XString<_ChTy>( str, len );
}

/** \brief 字符转成16进制串形式 */
template < typename _RetChTy, typename _ChTy = _RetChTy >
inline static XString<_RetChTy> CharToHexStr( _ChTy ch, bool padZero = false )
{
    _RetChTy str[sizeof(ch) * 2 + 1] = { (_RetChTy)0 };
    byte * pbyt = (byte*)&ch;
    ssize_t i = sizeof(ch) - 1;
    ssize_t j = 0;
    bool hadNonZero = padZero;
    for ( ; i >= 0; --i )
    {
        if ( pbyt[i] && !hadNonZero ) hadNonZero = true;
        if ( hadNonZero )
        {
            str[j * 2] = Literal<_RetChTy>::hexadecLowerStr[((pbyt[i] & (byte)0xF0) >> 4)];
            str[j * 2 + 1] = Literal<_RetChTy>::hexadecLowerStr[(pbyt[i] & (byte)0xF)];
            j++;
        }
    }
    if ( j == 0 )
    {
        str[0] = Literal<_RetChTy>::zeroChar;
        str[1] = Literal<_RetChTy>::zeroChar;
    }
    return str;
}


template < typename _ChTy >
size_t StrSplit( XString<_ChTy> const & str, XString<_ChTy> const & delimList, XStringArray<_ChTy> * arr, bool alwaysRetOneElem = false );
template <>
WINUX_FUNC_DECL(size_t) StrSplit( XString<char> const & str, XString<char> const & delimList, XStringArray<char> * arr, bool alwaysRetOneElem );
template <>
WINUX_FUNC_DECL(size_t) StrSplit( XString<wchar> const & str, XString<wchar> const & delimList, XStringArray<wchar> * arr, bool alwaysRetOneElem );
template <>
WINUX_FUNC_DECL(size_t) StrSplit( XString<char16> const & str, XString<char16> const & delimList, XStringArray<char16> * arr, bool alwaysRetOneElem );
template <>
WINUX_FUNC_DECL(size_t) StrSplit( XString<char32> const & str, XString<char32> const & delimList, XStringArray<char32> * arr, bool alwaysRetOneElem );
/** \brief 字符串分割
 *
 *  函数会把str内分割符间的内容(即使是空串)添加到arr中，返回个数。
 *  \param str 待分割的内容
 *  \param delimList 指示分割符列表，分割符只能是一个字符
 *  \param arr 输出到的数组
 *  \param alwaysRetOneElem 当为true时，即使str是空串时也会向arr返回一个数组元素，元素值是空串。
 *  \return size_t */
#if defined(_UNICODE) || defined(UNICODE)
inline size_t StrSplit( UnicodeString const & str, UnicodeString const & delimList, UnicodeStringArray * arr, bool alwaysRetOneElem = false ) { return StrSplit<wchar>( str, delimList, arr, alwaysRetOneElem ); }
#else
inline size_t StrSplit( AnsiString const & str, AnsiString const & delimList, AnsiStringArray * arr, bool alwaysRetOneElem = false ) { return StrSplit<char>( str, delimList, arr, alwaysRetOneElem ); }
#endif


template < typename _ChTy >
XStringArray<_ChTy> StrSplit( XString<_ChTy> const & str, XString<_ChTy> const & delimList, bool alwaysRetOneElem = false );
template <>
WINUX_FUNC_DECL(XStringArray<char>) StrSplit( XString<char> const & str, XString<char> const & delimList, bool alwaysRetOneElem );
template <>
WINUX_FUNC_DECL(XStringArray<wchar>) StrSplit( XString<wchar> const & str, XString<wchar> const & delimList, bool alwaysRetOneElem );
template <>
WINUX_FUNC_DECL(XStringArray<char16>) StrSplit( XString<char16> const & str, XString<char16> const & delimList, bool alwaysRetOneElem );
template <>
WINUX_FUNC_DECL(XStringArray<char32>) StrSplit( XString<char32> const & str, XString<char32> const & delimList, bool alwaysRetOneElem );
/** \brief 字符串分割
 *
 *  函数会把str内分割符间的内容(即使是空串)添加到arr中，返回个数。
 *  \param str 待分割的内容
 *  \param delimList 指示分割符列表，分割符只能是一个字符
 *  \param alwaysRetOneElem 当为true时，即使str是空串时也会向arr返回一个数组元素，元素值是空串。
 *  \return StringArray */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeStringArray StrSplit( UnicodeString const & str, UnicodeString const & delimList, bool alwaysRetOneElem = false ) { return StrSplit<wchar>( str, delimList, alwaysRetOneElem ); }
#else
inline AnsiStringArray StrSplit( AnsiString const & str, AnsiString const & delimList, bool alwaysRetOneElem = false ) { return StrSplit<char>( str, delimList, alwaysRetOneElem ); }
#endif


template < typename _ChTy >
size_t StrSplit2( XString<_ChTy> const & str, XString<_ChTy> const & delim, XStringArray<_ChTy> * arr, bool alwaysRetOneElem = false );
template <>
WINUX_FUNC_DECL(size_t) StrSplit2( XString<char> const & str, XString<char> const & delim, XStringArray<char> * arr, bool alwaysRetOneElem );
template <>
WINUX_FUNC_DECL(size_t) StrSplit2( XString<wchar> const & str, XString<wchar> const & delim, XStringArray<wchar> * arr, bool alwaysRetOneElem );
template <>
WINUX_FUNC_DECL(size_t) StrSplit2( XString<char16> const & str, XString<char16> const & delim, XStringArray<char16> * arr, bool alwaysRetOneElem );
template <>
WINUX_FUNC_DECL(size_t) StrSplit2( XString<char32> const & str, XString<char32> const & delim, XStringArray<char32> * arr, bool alwaysRetOneElem );
/** \brief 字符串分割2
 *
 *  delim指示出分割字符串，函数会把str内分割字符串间的内容(即使是空串)添加到arr中，返回个数。\n
 *  当alwaysRetOneElem==true时，即使str是空串时也会向arr返回一个数组元素，元素值是空串。
 *  \param str
 *  \param delim
 *  \param arr
 *  \return size_t */
#if defined(_UNICODE) || defined(UNICODE)
inline size_t StrSplit2( UnicodeString const & str, UnicodeString const & delim, UnicodeStringArray * arr, bool alwaysRetOneElem = false ) { return StrSplit2<wchar>( str, delim, arr, alwaysRetOneElem ); }
#else
inline size_t StrSplit2( AnsiString const & str, AnsiString const & delim, AnsiStringArray * arr, bool alwaysRetOneElem = false ) { return StrSplit2<char>( str, delim, arr, alwaysRetOneElem ); }
#endif


template < typename _ChTy >
XStringArray<_ChTy> StrSplit2( XString<_ChTy> const & str, XString<_ChTy> const & delim, bool alwaysRetOneElem = false );
template <>
WINUX_FUNC_DECL(XStringArray<char>) StrSplit2( XString<char> const & str, XString<char> const & delim, bool alwaysRetOneElem );
template <>
WINUX_FUNC_DECL(XStringArray<wchar>) StrSplit2( XString<wchar> const & str, XString<wchar> const & delim, bool alwaysRetOneElem );
template <>
WINUX_FUNC_DECL(XStringArray<char16>) StrSplit2( XString<char16> const & str, XString<char16> const & delim, bool alwaysRetOneElem );
template <>
WINUX_FUNC_DECL(XStringArray<char32>) StrSplit2( XString<char32> const & str, XString<char32> const & delim, bool alwaysRetOneElem );
/** \brief 字符串分割2
 *
 *  delim指示出分割字符串，函数会把str内分割字符串间的内容(即使是空串)添加到arr中，返回个数。\n
 *  当alwaysRetOneElem==true时，即使str是空串时也会向arr返回一个数组元素，元素值是空串。
 *  \param str
 *  \param delim
 *  \return StringArray */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeStringArray StrSplit2( UnicodeString const & str, UnicodeString const & delim, bool alwaysRetOneElem = false ) { return StrSplit2<wchar>( str, delim, alwaysRetOneElem ); }
#else
inline AnsiStringArray StrSplit2( AnsiString const & str, AnsiString const & delim, bool alwaysRetOneElem = false ) { return StrSplit2<char>( str, delim, alwaysRetOneElem ); }
#endif


template < typename _ChTy >
XString<_ChTy> StrJoin( XString<_ChTy> const & delim, XStringArray<_ChTy> const & arr );
template <>
WINUX_FUNC_DECL(XString<char>) StrJoin( XString<char> const & delim, XStringArray<char> const & arr );
template <>
WINUX_FUNC_DECL(XString<wchar>) StrJoin( XString<wchar> const & delim, XStringArray<wchar> const & arr );
template <>
WINUX_FUNC_DECL(XString<char16>) StrJoin( XString<char16> const & delim, XStringArray<char16> const & arr );
template <>
WINUX_FUNC_DECL(XString<char32>) StrJoin( XString<char32> const & delim, XStringArray<char32> const & arr );
/** \brief 字符串组合连接 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StrJoin( UnicodeString const & delim, UnicodeStringArray const & arr ) { return StrJoin<wchar>( delim, arr ); }
#else
inline AnsiString StrJoin( AnsiString const & delim, AnsiStringArray const & arr ) { return StrJoin<char>( delim, arr ); }
#endif


template < typename _ChTy >
XString<_ChTy> StrJoinEx( XString<_ChTy> const & delim, XStringArray<_ChTy> const & arr, size_t start = 0, size_t elemCount = -1  );
template <>
WINUX_FUNC_DECL(XString<char>) StrJoinEx( XString<char> const & delim, XStringArray<char> const & arr, size_t start, size_t elemCount );
template <>
WINUX_FUNC_DECL(XString<wchar>) StrJoinEx( XString<wchar> const & delim, XStringArray<wchar> const & arr, size_t start, size_t elemCount );
template <>
WINUX_FUNC_DECL(XString<char16>) StrJoinEx( XString<char16> const & delim, XStringArray<char16> const & arr, size_t start, size_t elemCount );
template <>
WINUX_FUNC_DECL(XString<char32>) StrJoinEx( XString<char32> const & delim, XStringArray<char32> const & arr, size_t start, size_t elemCount );
/** \brief 字符串组合连接。start表示开始位置，elemCount表示自开始位置的元素数，默认-1表示自开始位置的全部元素 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StrJoinEx( UnicodeString const & delim, UnicodeStringArray const & arr, size_t start = 0, size_t elemCount = -1 ) { return StrJoinEx<wchar>( delim, arr, start, elemCount ); }
#else
inline AnsiString StrJoinEx( AnsiString const & delim, AnsiStringArray const & arr, size_t start = 0, size_t elemCount = -1 ) { return StrJoinEx<char>( delim, arr, start, elemCount ); }
#endif


template < typename _ChTy >
XString<_ChTy> StrInsert( XString<_ChTy> const & str, size_t start, size_t end, XString<_ChTy> const & insert );
template <>
WINUX_FUNC_DECL(XString<char>) StrInsert( XString<char> const & str, size_t start, size_t end, XString<char> const & insert );
template <>
WINUX_FUNC_DECL(XString<wchar>) StrInsert( XString<wchar> const & str, size_t start, size_t end, XString<wchar> const & insert );
template <>
WINUX_FUNC_DECL(XString<char16>) StrInsert( XString<char16> const & str, size_t start, size_t end, XString<char16> const & insert );
template <>
WINUX_FUNC_DECL(XString<char32>) StrInsert( XString<char32> const & str, size_t start, size_t end, XString<char32> const & insert );
/** \brief 在指定位置插入字符串 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StrInsert( UnicodeString const & str, size_t start, size_t end, UnicodeString const & insert ) { return StrInsert<wchar>( str, start, end, insert ); }
#else
inline AnsiString StrInsert( AnsiString const & str, size_t start, size_t end, AnsiString const & insert ) { return StrInsert<char>( str, start, end, insert ); }
#endif


template < typename _ChTy >
XString<_ChTy> & StrMakeReplace( XString<_ChTy> * str, XString<_ChTy> const & findText, XString<_ChTy> const & replaceText, size_t offset = 0 );
template <>
WINUX_FUNC_DECL(XString<char> &) StrMakeReplace( XString<char> * str, XString<char> const & findText, XString<char> const & replaceText, size_t offset );
template <>
WINUX_FUNC_DECL(XString<wchar> &) StrMakeReplace( XString<wchar> * str, XString<wchar> const & findText, XString<wchar> const & replaceText, size_t offset );
template <>
WINUX_FUNC_DECL(XString<char16> &) StrMakeReplace( XString<char16> * str, XString<char16> const & findText, XString<char16> const & replaceText, size_t offset );
template <>
WINUX_FUNC_DECL(XString<char32> &) StrMakeReplace( XString<char32> * str, XString<char32> const & findText, XString<char32> const & replaceText, size_t offset );
/** \brief 使字符串全文替换 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString & StrMakeReplace( UnicodeString * str, UnicodeString const & findText, UnicodeString const & replaceText, size_t offset = 0 ) { return StrMakeReplace<wchar>( str, findText, replaceText, offset ); }
#else
inline AnsiString & StrMakeReplace( AnsiString * str, AnsiString const & findText, AnsiString const & replaceText, size_t offset = 0 ) { return StrMakeReplace<char>( str, findText, replaceText, offset ); }
#endif


template < typename _ChTy >
XString<_ChTy> StrTrim( XString<_ChTy> const & str, size_t * leftSpaceCount = nullptr, size_t * rightSpaceCount = nullptr );
template <>
WINUX_FUNC_DECL(XString<char>) StrTrim( XString<char> const & str, size_t * leftSpaceCount, size_t * rightSpaceCount );
template <>
WINUX_FUNC_DECL(XString<wchar>) StrTrim( XString<wchar> const & str, size_t * leftSpaceCount, size_t * rightSpaceCount );
template <>
WINUX_FUNC_DECL(XString<char16>) StrTrim( XString<char16> const & str, size_t * leftSpaceCount, size_t * rightSpaceCount );
template <>
WINUX_FUNC_DECL(XString<char32>) StrTrim( XString<char32> const & str, size_t * leftSpaceCount, size_t * rightSpaceCount );

template < typename _ChTy >
XString<_ChTy> StrLTrim( XString<_ChTy> const & str, size_t * leftSpaceCount = nullptr );
template <>
WINUX_FUNC_DECL(XString<char>) StrLTrim( XString<char> const & str, size_t * leftSpaceCount );
template <>
WINUX_FUNC_DECL(XString<wchar>) StrLTrim( XString<wchar> const & str, size_t * leftSpaceCount );
template <>
WINUX_FUNC_DECL(XString<char16>) StrLTrim( XString<char16> const & str, size_t * leftSpaceCount );
template <>
WINUX_FUNC_DECL(XString<char32>) StrLTrim( XString<char32> const & str, size_t * leftSpaceCount );

template < typename _ChTy >
XString<_ChTy> StrRTrim( XString<_ChTy> const & str, size_t * rightSpaceCount = nullptr );
template <>
WINUX_FUNC_DECL(XString<char>) StrRTrim( XString<char> const & str, size_t * rightSpaceCount );
template <>
WINUX_FUNC_DECL(XString<wchar>) StrRTrim( XString<wchar> const & str, size_t * rightSpaceCount );
template <>
WINUX_FUNC_DECL(XString<char16>) StrRTrim( XString<char16> const & str, size_t * rightSpaceCount );
template <>
WINUX_FUNC_DECL(XString<char32>) StrRTrim( XString<char32> const & str, size_t * rightSpaceCount );

/** \brief 祛除字符串首尾出现的空白字符
*
* \param str
* \return String */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StrTrim( UnicodeString const & str, size_t * leftSpaceCount = nullptr, size_t * rightSpaceCount = nullptr ) { return StrTrim<wchar>( str, leftSpaceCount, rightSpaceCount ); }
inline UnicodeString StrLTrim( UnicodeString const & str, size_t * leftSpaceCount = nullptr ) { return StrLTrim<wchar>( str, leftSpaceCount ); }
inline UnicodeString StrRTrim( UnicodeString const & str, size_t * rightSpaceCount = nullptr ) { return StrRTrim<wchar>( str, rightSpaceCount ); }
#else
inline AnsiString StrTrim( AnsiString const & str, size_t * leftSpaceCount = nullptr, size_t * rightSpaceCount = nullptr ) { return StrTrim<char>( str, leftSpaceCount, rightSpaceCount ); }
inline AnsiString StrLTrim( AnsiString const & str, size_t * leftSpaceCount = nullptr ) { return StrLTrim<char>( str, leftSpaceCount ); }
inline AnsiString StrRTrim( AnsiString const & str, size_t * rightSpaceCount = nullptr ) { return StrRTrim<char>( str, rightSpaceCount ); }
#endif


template < typename _ChTy >
XString<_ChTy> & StrMakeUpper( XString<_ChTy> * str );
template <>
WINUX_FUNC_DECL(XString<char> &) StrMakeUpper( XString<char> * str );
template <>
WINUX_FUNC_DECL(XString<wchar> &) StrMakeUpper( XString<wchar> * str );
template <>
WINUX_FUNC_DECL(XString<char16> &) StrMakeUpper( XString<char16> * str );
template <>
WINUX_FUNC_DECL(XString<char32> &) StrMakeUpper( XString<char32> * str );
/** \brief 使字符串大写 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString & StrMakeUpper( UnicodeString * str ) { return StrMakeUpper<wchar>(str); }
#else
inline AnsiString & StrMakeUpper( AnsiString * str ) { return StrMakeUpper<char>(str); }
#endif


template < typename _ChTy >
XString<_ChTy> StrUpper( XString<_ChTy> str );
template <>
WINUX_FUNC_DECL(XString<char>) StrUpper( XString<char> str );
template <>
WINUX_FUNC_DECL(XString<wchar>) StrUpper( XString<wchar> str );
template <>
WINUX_FUNC_DECL(XString<char16>) StrUpper( XString<char16> str );
template <>
WINUX_FUNC_DECL(XString<char32>) StrUpper( XString<char32> str );
/** \brief 使字符串大写 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StrUpper( UnicodeString str ) { return StrUpper<wchar>(str); }
#else
inline AnsiString StrUpper( AnsiString str ) { return StrUpper<char>(str); }
#endif


template < typename _ChTy >
XString<_ChTy> & StrMakeLower( XString<_ChTy> * str );
template <>
WINUX_FUNC_DECL(XString<char> &) StrMakeLower( XString<char> * str );
template <>
WINUX_FUNC_DECL(XString<wchar> &) StrMakeLower( XString<wchar> * str );
template <>
WINUX_FUNC_DECL(XString<char16> &) StrMakeLower( XString<char16> * str );
template <>
WINUX_FUNC_DECL(XString<char32> &) StrMakeLower( XString<char32> * str );
/** \brief 使字符串小写 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString & StrMakeLower( UnicodeString * str ) { return StrMakeLower<wchar>(str); }
#else
inline AnsiString & StrMakeLower( AnsiString * str ) { return StrMakeLower<char>(str); }
#endif


template < typename _ChTy >
XString<_ChTy> StrLower( XString<_ChTy> str );
template <>
WINUX_FUNC_DECL(XString<char>) StrLower( XString<char> str );
template <>
WINUX_FUNC_DECL(XString<wchar>) StrLower( XString<wchar> str );
template <>
WINUX_FUNC_DECL(XString<char16>) StrLower( XString<char16> str );
template <>
WINUX_FUNC_DECL(XString<char32>) StrLower( XString<char32> str );
/** \brief 使字符串小写 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StrLower( UnicodeString str ) { return StrLower<wchar>(str); }
#else
inline AnsiString StrLower( AnsiString str ) { return StrLower<char>(str); }
#endif


template < typename _ChTy >
XString<_ChTy> StrMultiple( XString<_ChTy> const & str, size_t multiple );
template <>
WINUX_FUNC_DECL(XString<char>) StrMultiple( XString<char> const & str, size_t multiple );
template <>
WINUX_FUNC_DECL(XString<wchar>) StrMultiple( XString<wchar> const & str, size_t multiple );
template <>
WINUX_FUNC_DECL(XString<char16>) StrMultiple( XString<char16> const & str, size_t multiple );
template <>
WINUX_FUNC_DECL(XString<char32>) StrMultiple( XString<char32> const & str, size_t multiple );
/** \brief 字符串倍数的出现 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StrMultiple( UnicodeString const & str, size_t multiple ) { return StrMultiple<wchar>( str, multiple ); }
#else
inline AnsiString StrMultiple( AnsiString const & str, size_t multiple ) { return StrMultiple<char>( str, multiple ); }
#endif


template < typename _ChTy >
XString<_ChTy> StrSubtract( XString<_ChTy> str1, XString<_ChTy> const & str2 );
template <>
WINUX_FUNC_DECL(XString<char>) StrSubtract( XString<char> str1, XString<char> const & str2 );
template <>
WINUX_FUNC_DECL(XString<wchar>) StrSubtract( XString<wchar> str1, XString<wchar> const & str2 );
template <>
WINUX_FUNC_DECL(XString<char16>) StrSubtract( XString<char16> str1, XString<char16> const & str2 );
template <>
WINUX_FUNC_DECL(XString<char32>) StrSubtract( XString<char32> str1, XString<char32> const & str2 );
/** \brief 字符串相减，str1 - str2，即去掉str1里与str2相同的部分。限制：str1 >= str2。 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StrSubtract( UnicodeString str1, UnicodeString const & str2 ) { return StrSubtract<wchar>( str1, str2 ); }
#else
inline AnsiString StrSubtract( AnsiString str1, AnsiString const & str2 ) { return StrSubtract<char>( str1, str2 ); }
#endif


/** \brief 字符串转换成数字Flags */
enum StrToXqFlags
{
    stqUnsigned = 1, //!< 无符号处理
    stqNegative = 2, //!< 有解析到负号
    stqOverflow = 4, //!< 发生溢出
    stqReadDigit = 8 //!< 读到一个正确的数字字符
};

template < typename _ChTy >
uint64 StrToXq( _ChTy const * nptr, _ChTy const ** endptr, int ibase, int flags );
template <>
WINUX_FUNC_DECL(uint64) StrToXq( char const * nptr, char const ** endptr, int ibase, int flags );
template <>
WINUX_FUNC_DECL(uint64) StrToXq( wchar const * nptr, wchar const ** endptr, int ibase, int flags );
template <>
WINUX_FUNC_DECL(uint64) StrToXq( char16 const * nptr, char16 const ** endptr, int ibase, int flags );
template <>
WINUX_FUNC_DECL(uint64) StrToXq( char32 const * nptr, char32 const ** endptr, int ibase, int flags );
/** \brief 字符串转换成64位的数字 */
#if defined(_UNICODE) || defined(UNICODE)
inline uint64 StrToXq( wchar const * nptr, wchar const ** endptr, int ibase, int flags ) { return StrToXq<wchar>( nptr, endptr, ibase, flags ); }
#else
inline uint64 StrToXq( char const * nptr, char const ** endptr, int ibase, int flags ) { return StrToXq<char>( nptr, endptr, ibase, flags ); }
#endif


template < typename _ChTy >
int64 StrToInt64( XString<_ChTy> const & numStr, int ibase );
template <>
WINUX_FUNC_DECL(int64) StrToInt64( XString<char> const & numStr, int ibase );
template <>
WINUX_FUNC_DECL(int64) StrToInt64( XString<wchar> const & numStr, int ibase );
template <>
WINUX_FUNC_DECL(int64) StrToInt64( XString<char16> const & numStr, int ibase );
template <>
WINUX_FUNC_DECL(int64) StrToInt64( XString<char32> const & numStr, int ibase );
#if defined(_UNICODE) || defined(UNICODE)
inline int64 StrToInt64( UnicodeString const & numStr, int ibase ) { return StrToInt64<wchar>( numStr, ibase ); }
#else
inline int64 StrToInt64( AnsiString const & numStr, int ibase ) { return StrToInt64<char>( numStr, ibase ); }
#endif


template < typename _ChTy >
uint64 StrToUInt64( XString<_ChTy> const & numStr, int ibase );
template <>
WINUX_FUNC_DECL(uint64) StrToUInt64( XString<char> const & numStr, int ibase );
template <>
WINUX_FUNC_DECL(uint64) StrToUInt64( XString<wchar> const & numStr, int ibase );
template <>
WINUX_FUNC_DECL(uint64) StrToUInt64( XString<char16> const & numStr, int ibase );
template <>
WINUX_FUNC_DECL(uint64) StrToUInt64( XString<char32> const & numStr, int ibase );
#if defined(_UNICODE) || defined(UNICODE)
inline uint64 StrToUInt64( UnicodeString const & numStr, int ibase ) { return StrToUInt64<wchar>( numStr, ibase ); }
#else
inline uint64 StrToUInt64( AnsiString const & numStr, int ibase ) { return StrToUInt64<char>( numStr, ibase ); }
#endif


template < typename _ChTy >
inline XString<_ChTy> AddSlashes( XString<_ChTy> const & str, XString<_ChTy> const & charlist );
template <>
WINUX_FUNC_DECL(XString<char>) AddSlashes( XString<char> const & str, XString<char> const & charlist );
template <>
WINUX_FUNC_DECL(XString<wchar>) AddSlashes( XString<wchar> const & str, XString<wchar> const & charlist );
template <>
WINUX_FUNC_DECL(XString<char16>) AddSlashes( XString<char16> const & str, XString<char16> const & charlist );
template <>
WINUX_FUNC_DECL(XString<char32>) AddSlashes( XString<char32> const & str, XString<char32> const & charlist );
/** \brief 加反斜杠 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString AddSlashes( UnicodeString const & str, UnicodeString const & charlist ) { return AddSlashes<wchar>( str, charlist ); }
#else
inline AnsiString AddSlashes( AnsiString const & str, AnsiString const & charlist ) { return AddSlashes<char>( str, charlist ); }
#endif


template < typename _ChTy >
inline XString<_ChTy> AddCSlashes( XString<_ChTy> const & str ) { return AddSlashes<_ChTy>( str, Literal<_ChTy>::cslashesStr ); }
/** \brief 加C-Style反斜杠 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString AddCSlashes( UnicodeString const & str ) { return AddCSlashes<wchar>(str); }
#else
inline AnsiString AddCSlashes( AnsiString const & str ) { return AddCSlashes<char>(str); }
#endif


template < typename _ChTy >
XString<_ChTy> StripSlashes( XString<_ChTy> const & str, XString<_ChTy> const & charlist );
template <>
WINUX_FUNC_DECL(XString<char>) StripSlashes( XString<char> const & str, XString<char> const & charlist );
template <>
WINUX_FUNC_DECL(XString<wchar>) StripSlashes( XString<wchar> const & str, XString<wchar> const & charlist );
template <>
WINUX_FUNC_DECL(XString<char16>) StripSlashes( XString<char16> const & str, XString<char16> const & charlist );
template <>
WINUX_FUNC_DECL(XString<char32>) StripSlashes( XString<char32> const & str, XString<char32> const & charlist );
/** \brief 去掉反斜杠 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StripSlashes( UnicodeString const & str, UnicodeString const & charlist ) { return StripSlashes<wchar>( str, charlist ); }
#else
inline AnsiString StripSlashes( AnsiString const & str, AnsiString const & charlist ) { return StripSlashes<char>( str, charlist ); }
#endif


template < typename _ChTy >
inline XString<_ChTy> StripCSlashes( XString<_ChTy> const & str ) { return StripSlashes<_ChTy>( str, Literal<_ChTy>::cslashesStr ); }
/** \brief 去掉C-Style反斜杠 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StripCSlashes( UnicodeString const & str ) { return StripCSlashes<wchar>(str); }
#else
inline AnsiString StripCSlashes( AnsiString const & str ) { return StripCSlashes<char>(str); }
#endif


template < typename _ChTy >
XString<_ChTy> AddQuotes( XString<_ChTy> const & str, _ChTy quote = Literal<_ChTy>::quoteChar );
template <>
WINUX_FUNC_DECL(XString<char>) AddQuotes( XString<char> const & str, char quote );
template <>
WINUX_FUNC_DECL(XString<wchar>) AddQuotes( XString<wchar> const & str, wchar quote );
template <>
WINUX_FUNC_DECL(XString<char16>) AddQuotes( XString<char16> const & str, char16 quote );
template <>
WINUX_FUNC_DECL(XString<char32>) AddQuotes( XString<char32> const & str, char32 quote );
/** \brief double引号 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString AddQuotes( UnicodeString const & str, UnicodeString::value_type quote = Literal<UnicodeString::value_type>::quoteChar ) { return AddQuotes<wchar>( str, quote ); }
#else
inline AnsiString AddQuotes( AnsiString const & str, AnsiString::value_type quote = Literal<AnsiString::value_type>::quoteChar ) { return AddQuotes<char>( str, quote ); }
#endif


template < typename _ChTy >
bool StrGetLine( XString<_ChTy> * line, XString<_ChTy> const & str, size_t * i, XString<_ChTy> * nl = nullptr );
template <>
WINUX_FUNC_DECL(bool) StrGetLine( XString<char> * line, XString<char> const & str, size_t * i, XString<char> * nl );
template <>
WINUX_FUNC_DECL(bool) StrGetLine( XString<wchar> * line, XString<wchar> const & str, size_t * i, XString<wchar> * nl );
template <>
WINUX_FUNC_DECL(bool) StrGetLine( XString<char16> * line, XString<char16> const & str, size_t * i, XString<char16> * nl );
template <>
WINUX_FUNC_DECL(bool) StrGetLine( XString<char32> * line, XString<char32> const & str, size_t * i, XString<char32> * nl );
/** \brief 获取字符串中的一行。支持unix，windows，mac平台的行分隔方式\n
 *
 *  \param line 获取到的一行字符串。如果nl!=nullptr则不包含换行符，反之则包含。
 *  \param str 要处理的字符串。
 *  \param i 指示起始位置，并返回处理到哪个位置。
 *  \param nl 如果nl!=nullptr则line不包含换行符，反之则包含。
 *
 *  如何统一处理3平台的文本文件：\n
 *  最好的办法是在该平台用文本模式打开该平台产生的文本文件，然后操作。\n
 *  然而，现实不像想象的那么美好，多数情况下是处理不同平台下的文本文件，譬如在unix系平台下处理windows或mac的文本文件。\n
 *  由于行分隔不同，导致处理有一定困难。\n
 *  此函数正为此而存在。首先，你需要用二进制模式打开文件，然后读取全部数据，调用此函数取行即可。 */
#if defined(_UNICODE) || defined(UNICODE)
inline bool StrGetLine( UnicodeString * line, UnicodeString const & str, size_t * i, UnicodeString * nl = nullptr ) { return StrGetLine<wchar>( line, str, i, nl ); }
#else
inline bool StrGetLine( AnsiString * line, AnsiString const & str, size_t * i, AnsiString * nl = nullptr ) { return StrGetLine<char>( line, str, i, nl ); }
#endif


template < typename _ChTy >
bool StrGetLine( XString<_ChTy> * line, _ChTy const * str, size_t len, size_t * i, XString<_ChTy> * nl = nullptr );
template <>
WINUX_FUNC_DECL(bool) StrGetLine( XString<char> * line, char const * str, size_t len, size_t * i, XString<char> * nl );
template <>
WINUX_FUNC_DECL(bool) StrGetLine( XString<wchar> * line, wchar const * str, size_t len, size_t * i, XString<wchar> * nl );
template <>
WINUX_FUNC_DECL(bool) StrGetLine( XString<char16> * line, char16 const * str, size_t len, size_t * i, XString<char16> * nl );
template <>
WINUX_FUNC_DECL(bool) StrGetLine( XString<char32> * line, char32 const * str, size_t len, size_t * i, XString<char32> * nl );
/** \brief 获取字符串中的一行。支持unix，windows，mac平台的行分隔方式\n
 *
 *  \param line 获取到的一行字符串。如果nl!=nullptr则不包含换行符，反之则包含。
 *  \param str 要处理的字符串。
 *  \param len 字符串长度。
 *  \param i 指示起始位置，并返回处理到哪个位置。
 *  \param nl 如果nl!=nullptr则line不包含换行符，反之则包含。
 *
 *  如何统一处理3平台的文本文件：\n
 *  最好的办法是在该平台用文本模式打开该平台产生的文本文件，然后操作。\n
 *  然而，现实不像想象的那么美好，多数情况下是处理不同平台下的文本文件，譬如在unix系平台下处理windows或mac的文本文件。\n
 *  由于行分隔不同，导致处理有一定困难。\n
 *  此函数正为此而存在。首先，你需要用二进制模式打开文件，然后读取全部数据，调用此函数取行即可。 */
#if defined(_UNICODE) || defined(UNICODE)
inline bool StrGetLine( UnicodeString * line, UnicodeString::value_type const * str, size_t len, size_t * i, UnicodeString * nl = nullptr ) { return StrGetLine<wchar>( line, str, len, i, nl ); }
#else
inline bool StrGetLine( AnsiString * line, AnsiString::value_type const * str, size_t len, size_t * i, AnsiString * nl = nullptr ) { return StrGetLine<char>( line, str, len, i, nl ); }
#endif


/** \brief 获取指定位置的行内容
 *
 *  \param content 内容
 *  \param pos 位置索引
 *  \param lineHead 返回line开头索引
 *  \param lineTail 返回line尾部索引（+1则是END位置）
 *  \param line 返回line */
template < typename _ChTy >
void GetLineByPos( XString<_ChTy> const & content, ssize_t pos, ssize_t * lineHead, ssize_t * lineTail, XString<_ChTy> * line = nullptr );
template <>
WINUX_FUNC_DECL(void) GetLineByPos( XString<char> const & content, ssize_t pos, ssize_t * lineHead, ssize_t * lineTail, XString<char> * line );
template <>
WINUX_FUNC_DECL(void) GetLineByPos( XString<wchar> const & content, ssize_t pos, ssize_t * lineHead, ssize_t * lineTail, XString<wchar> * line );
template <>
WINUX_FUNC_DECL(void) GetLineByPos( XString<char16> const & content, ssize_t pos, ssize_t * lineHead, ssize_t * lineTail, XString<char16> * line );
template <>
WINUX_FUNC_DECL(void) GetLineByPos( XString<char32> const & content, ssize_t pos, ssize_t * lineHead, ssize_t * lineTail, XString<char32> * line );


/** \brief 整理标识符串的标记 */
enum CollateIdentifierStringFlag : winux::uint
{
    wordRaw = 0x00, //!< 不处理单词
    wordAllUpper = 0x01, //!< 单词大写
    wordAllLower = 0x02, //!< 单词小写
    wordFirstCharUpper = 0x03, //!< 首字母大写

    nameLowerCamelCase = 0x10 | wordFirstCharUpper, //!< 小驼峰 camelCase
    nameUpperCamelCase = 0x20 | wordFirstCharUpper, //!< 大驼峰 CamelCase
    namePascalCase = nameUpperCamelCase, //!< 帕斯卡命名 PascalCase
    nameSnakeCase = 0x30 | wordAllLower, //!< snake_case
    nameKebabCase = 0x40 | wordAllLower, //!< kebab-case
    nameScreamingSnakeCase = 0x50 | wordAllUpper, //!< SCREAMING_SNAKE_CASE
};

template < typename _ChTy >
XString<_ChTy> CollateIdentifierToString( XString<_ChTy> const & identifier, XString<_ChTy> const & sep, winux::uint flags = wordFirstCharUpper );
template <>
WINUX_FUNC_DECL(XString<char>) CollateIdentifierToString( XString<char> const & identifier, XString<char> const & sep, winux::uint flags );
template <>
WINUX_FUNC_DECL(XString<wchar>) CollateIdentifierToString( XString<wchar> const & identifier, XString<wchar> const & sep, winux::uint flags );
template <>
WINUX_FUNC_DECL(XString<char16>) CollateIdentifierToString( XString<char16> const & identifier, XString<char16> const & sep, winux::uint flags );
template <>
WINUX_FUNC_DECL(XString<char32>) CollateIdentifierToString( XString<char32> const & identifier, XString<char32> const & sep, winux::uint flags );
/** \brief 整理标识符为字符串 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString CollateIdentifierToString( UnicodeString const & identifier, UnicodeString const & sep, winux::uint flags = wordFirstCharUpper ) { return CollateIdentifierToString<wchar>( identifier, sep, flags ); }
#else
inline AnsiString CollateIdentifierToString( AnsiString const & identifier, AnsiString const & sep, winux::uint flags = wordFirstCharUpper ) { return CollateIdentifierToString<char>( identifier, sep, flags ); }
#endif


template < typename _ChTy >
XStringArray<_ChTy> CollateIdentifierToArray( XString<_ChTy> const & identifier, winux::uint flags = wordFirstCharUpper );
template <>
WINUX_FUNC_DECL(XStringArray<char>) CollateIdentifierToArray( XString<char> const & identifier, winux::uint flags );
template <>
WINUX_FUNC_DECL(XStringArray<wchar>) CollateIdentifierToArray( XString<wchar> const & identifier, winux::uint flags );
template <>
WINUX_FUNC_DECL(XStringArray<char16>) CollateIdentifierToArray( XString<char16> const & identifier, winux::uint flags );
template <>
WINUX_FUNC_DECL(XStringArray<char32>) CollateIdentifierToArray( XString<char32> const & identifier, winux::uint flags );
/** \brief 整理标识符为单词数组 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeStringArray CollateIdentifierToArray( UnicodeString const & identifier, winux::uint flags = wordFirstCharUpper ) { return CollateIdentifierToArray<wchar>( identifier, flags ); }
#else
inline AnsiStringArray CollateIdentifierToArray( AnsiString const & identifier, winux::uint flags = wordFirstCharUpper ) { return CollateIdentifierToArray<char>( identifier, flags ); }
#endif


/** \brief KMP匹配算法：求子串next值 */
template < typename _IndexType, typename _ChTy >
inline static std::vector<_IndexType> _Templ_KmpCalcNext( _ChTy const * substr, size_t sublen )
{
    std::vector<_IndexType> next( sublen + 1 );
    ssize_t j = 0, k = -1;
    next[0] = -1;
    while ( j < (ssize_t)sublen )
    {
        if ( k == -1 || substr[j] == substr[k] )
        {
            j++;
            k++;
            //if ( substr[j] != substr[k] )
                next[j] = (_IndexType)k;
            //else
            //    next[j] = next[k];
        }
        else
        {
            k = next[k];
        }
    }
    return next;
}

/** \brief KMP匹配算法：传入已经求好的next进行匹配 */
template < typename _IndexType, typename _ChTy >
inline static size_t _Templ_KmpMatchEx( _ChTy const * str, size_t len, _ChTy const * substr, size_t sublen, size_t pos, std::vector<_IndexType> const & next )
{
    ssize_t i, j;
    i = pos;
    j = 0;
    while ( i < (ssize_t)len && j < (ssize_t)sublen )
    {
        if ( str[i] == substr[j] )
        {
            j++;
            i++;
        }
        else
        {
            j = next[j];

            if ( j == -1 )
            {
                j = 0;
                i++;
            }
        }
    }
    return j == (ssize_t)sublen ? i - (ssize_t)sublen : -1;
}

/** \brief KMP匹配算法：匹配 */
template < typename _IndexType, typename _ChTy >
inline static size_t _Templ_KmpMatch( _ChTy const * str, size_t len, _ChTy const * substr, size_t sublen, size_t pos )
{
    return _Templ_KmpMatchEx<_IndexType, _ChTy>( str, len, substr, sublen, pos, _Templ_KmpCalcNext( substr, sublen ) );
}

/** \brief KMP字符串匹配算法：求子串next值 */
template < typename _ChTy >
inline std::vector<int> KmpCalcNext( _ChTy const * substr, size_t sublen )
{
    return _Templ_KmpCalcNext< int, _ChTy >( substr, sublen );
}
/** \brief KMP字符串匹配算法：传入已经求好的next进行匹配 */
template < typename _ChTy >
inline size_t KmpMatchEx( _ChTy const * str, size_t len, _ChTy const * substr, size_t sublen, size_t pos, std::vector<int> const & next )
{
    return _Templ_KmpMatchEx< int, _ChTy >( str, len, substr, sublen, pos, next );
}
/** \brief KMP字符串匹配算法：匹配 */
template < typename _ChTy >
inline size_t KmpMatch( _ChTy const * str, size_t len, _ChTy const * substr, size_t sublen, size_t pos )
{
    return KmpMatchEx<_ChTy>( str, len, substr, sublen, pos, KmpCalcNext( substr, sublen ) );
}


/** \brief 用来使得String能够用operator<<来赋值 */
template < typename _ChTy >
class XStringWriter
{
public:
    /** \brief 构造函数1
     *
     *  \param str 要赋值的字符串
     *  \param isAppend 是否用“添加”的模式,默认是false */
    XStringWriter( XString<_ChTy> * str, bool isAppend = false ) : _str(str), _sout(NULL), _isAppend(isAppend)
    {
        _sout = new std::basic_ostringstream<_ChTy>();
    }
    XStringWriter( XStringWriter const & other ) : _str(NULL), _sout(NULL)
    {
        this->operator = ( const_cast<XStringWriter &>(other) );
    }
    ~XStringWriter()
    {
        if ( _str )
        {
            if ( _isAppend )
            {
                *_str += _sout->str();
            }
            else
            {
                *_str = _sout->str();
            }
        }
        if ( _sout ) delete _sout;
    }
    XStringWriter & operator = ( XStringWriter & other )
    {
        if ( this != &other )
        {
            _str = other._str;
            _sout = other._sout;
            _isAppend = other._isAppend;

            other._str = NULL;
            other._sout = NULL;
            other._isAppend = false;
        }
        return *this;
    }

    template < typename _AnyType >
    std::basic_ostream<_ChTy> & operator << ( _AnyType && t )
    {
        return *_sout << std::forward<_AnyType>(t);
    }

private:
    XString<_ChTy> * _str;
    std::basic_ostringstream<_ChTy> * _sout;
    bool _isAppend;
};

using StringWriter = XStringWriter<tchar>;
using AnsiStringWriter = XStringWriter<char>;
using UnicodeStringWriter = XStringWriter<wchar>;
using UnicodeString16Writer = XStringWriter<char16>;
using Utf16StringWriter = XStringWriter<char16>;
using UnicodeString32Writer = XStringWriter<char32>;
using Utf32StringWriter = XStringWriter<char32>;

/** \brief 多项匹配/替换 */
class WINUX_DLL MultiMatch
{
public:
    struct MatchResult
    {
        ssize_t pos; //!< 匹配到的位置
        ssize_t item; //!< 匹配项的索引
    };

    struct MatchState
    {
        ssize_t j; //!< 下一次从子串开始的位置
        ssize_t markpos; //!< 标记位置，表示进行到这个位置了，该从这个位置开始
        MatchResult mr; //!< 匹配结果
        bool prevCharMatch; //!< 上一个字符是否匹配
        bool matched; //!< 是否完成匹配，完成匹配就不用再进行

        void reset()
        {
            memset( this, 0, sizeof(*this) );
            mr.pos = -1;
            mr.item = -1;
        }
    };

    //! \brief NEXT值数组类型
    typedef std::vector<short> KmpNextValueArray;

    //! \brief 匹配状态数组类型
    typedef std::vector<MatchState> MatchStates;

    //! \brief 替换函数的类型
    typedef String (* ReplaceFuncType)( MultiMatch const * matchObj, ssize_t item, void * extra );

    //! \brief 搜索算法函数类型
    typedef MatchResult ( MultiMatch:: * SearchFuncType )( String const & str, ssize_t offset ) const;

    /** \brief 构造函数0
     *
     *  之后需要自己调用addMatchReplacePair()、addMatch()、setReplaceFunc()设置相关参数 */
    MultiMatch();

    /** \brief 构造函数1，要求匹配项和替换项 */
    MultiMatch( StringArray const & matches, StringArray const & replaces );

    /** \brief 构造函数2，要求匹配项和替换函数，若不进行替换，则replaceFunc可为NULL */
    MultiMatch( StringArray const & matches, ReplaceFuncType replaceFunc, void * extra = NULL );

    /** \brief 构造函数3，要求匹配项和替换项 */
    template < size_t _Count >
    MultiMatch( String (&matches)[_Count], String (&replaces)[_Count] ) : _replaceFunc(NULL), _extra(NULL)
    {
        this->init( StringArray( matches, matches + _Count ), StringArray( replaces, replaces + _Count ) );
    }

    /** \brief 构造函数4，要求匹配项和替换函数，若不进行替换，则replaceFunc可为NULL */
    template < size_t _Count >
    MultiMatch( String (&matches)[_Count], ReplaceFuncType replaceFunc, void * extra = NULL ) : _replaceFunc(NULL), _extra(NULL)
    {
        this->init( StringArray( matches, matches + _Count ), replaceFunc, extra );
    }

    /** \brief 初始化（匹配项，替换项） */
    void init( StringArray const & matches, StringArray const & replaces );

    /** \brief 初始化（匹配项，替换项回调函数） */
    void init( StringArray const & matches, ReplaceFuncType replaceFunc, void * extra = NULL );

    /** \brief 添加一对匹配替换项，返回要匹配的项数 */
    ssize_t addMatchReplacePair( String const & match, String const & replace );

    /** \brief 添加要匹配项 */
    ssize_t addMatch( String const & match );

    /** \brief 获取指定匹配项 */
    String const & getMatchItem( ssize_t item ) const;

    /** \brief 设置指定匹配项 */
    void setMatchItem( ssize_t item, String const & match );

    /** \brief 获取指定替换项 */
    String getReplaceItem( ssize_t item ) const;

    /** \brief 设置指定替换项 */
    void setReplaceItem( ssize_t item, String const & replace );

    /** \brief 设置替换项 */
    void setReplaceItems( StringArray const & replaces );

    /** \brief 设置新的替换项回调函数，返回旧的替换项回调函数 */
    ReplaceFuncType setReplaceFunc( ReplaceFuncType newReplaceFunc, void * extra = NULL );

    /** \brief 清空匹配项和替换项数据 */
    void clear();

    /** \brief 搜索任意一项匹配（KMP）
     *
     *  \param str 字符串
     *  \param offset 偏移，表示从哪个位置开始搜
     *  \return MatchResult(pos,item) 返回的pos考虑了offset值 */
    MatchResult search( String const & str, ssize_t offset = 0 ) const;

    /** \brief 搜索任意一项匹配（KMP贪婪模式，尽可能匹配长的项）
     *
     *  \param str 字符串
     *  \param offset 偏移，表示从哪个位置开始搜
     *  \return MatchResult(pos,item) 返回的pos考虑了offset值 */
    MatchResult greedSearch( String const & str, ssize_t offset = 0 ) const;

    /** \brief 搜索任意一项匹配（贪婪模式，非KMP算法）
     *
     *  \param str 字符串
     *  \param offset 偏移，表示从哪个位置开始搜
     *  \return MatchResult(pos,item) 返回的pos考虑了offset值 */
    MatchResult commonSearch( String const & str, ssize_t offset = 0 ) const;

    /** \brief 替换。搜索str中的matches，并替换成replaces
     *
     *  \param str 目标字符串
     *  \param offset 偏移，表示从哪个位置开始搜
     *  \param fnSearch 算法选择，请指定函数（search,greedSearch,commonSearch） */
    String replace( String const & str, ssize_t offset = 0, SearchFuncType fnSearch = &MultiMatch::commonSearch ) const;

private:
    std::vector<KmpNextValueArray> _nextVals;
    mutable MatchStates _states;
    StringArray _matchItems;
    StringArray _replaceItems;
    ReplaceFuncType _replaceFunc;
    void * _extra;
};

/** \brief 设置locale信息 */
class WINUX_DLL Locale
{
public:
    static void Set( char const * lc ) { _clsLc = lc; }
    static char const * Get() { return _clsLc.c_str(); }
    Locale( char const * lc = NULL );
    ~Locale();

private:
    AnsiString _loc;
    AnsiString _prevLoc;
    static AnsiString _clsLc;
    DISABLE_OBJECT_COPY(Locale)
};


/** \brief 返回一个本地字符串里有多少个实际的字符(by local CodePage)，用于mbstowcs */
WINUX_FUNC_DECL(size_t) LocalCharsCount( AnsiString const & local );
/** \brief 返回一个unicode字符串转换为多字节字符串最少需要多少字节(by local CodePage)，用于wcstombs */
WINUX_FUNC_DECL(size_t) UnicodeMinLength( UnicodeString const & unicode );
/** \brief Unicode转换到本地Ansi */
WINUX_FUNC_DECL(AnsiString) UnicodeToLocal( UnicodeString const & unicode );
/** \brief 本地Ansi转到Unicode */
WINUX_FUNC_DECL(UnicodeString) LocalToUnicode( AnsiString const & local );

/** \brief 兼容字符串与Unicode、Local字符串相互转换 */
WINUX_FUNC_DECL(String) LocalToString( AnsiString const & local );
/** \brief 兼容字符串与Unicode、Local字符串相互转换 */
WINUX_FUNC_DECL(String) UnicodeToString( UnicodeString const & unicode );
/** \brief 兼容字符串与Unicode、Local字符串相互转换 */
WINUX_FUNC_DECL(AnsiString) StringToLocal( String const & str );
/** \brief 兼容字符串与Unicode、Local字符串相互转换 */
WINUX_FUNC_DECL(UnicodeString) StringToUnicode( String const & str );


WINUX_DLL AnsiString FormatExVA( size_t cch, char const * fmt, va_list args );
WINUX_DLL UnicodeString FormatExVW( size_t cch, wchar const * fmt, va_list args );

WINUX_DLL AnsiString FormatExA( size_t cch, char const * fmt, ... );
WINUX_DLL UnicodeString FormatExW( size_t cch, wchar const * fmt, ... );

WINUX_DLL AnsiString FormatA( char const * fmt, ... );
WINUX_DLL UnicodeString FormatW( wchar const * fmt, ... );

#if defined(_UNICODE) || defined(UNICODE)
/** \brief 格式化字符串0 */
inline UnicodeString FormatExV( size_t cch, wchar const * fmt, va_list args ) { return FormatExVW( cch, fmt, args ); }
/** \brief 格式化字符串1 */
WINUX_DLL UnicodeString FormatEx( size_t cch, wchar const * fmt, ... );
/** \brief 格式化字符串2 */
WINUX_DLL UnicodeString Format( wchar const * fmt, ... );
#else
/** \brief 格式化字符串0 */
inline AnsiString FormatExV( size_t cch, char const * fmt, va_list args ) { return FormatExVA( cch, fmt, args ); }
/** \brief 格式化字符串1 */
WINUX_DLL AnsiString FormatEx( size_t cch, char const * fmt, ... );
/** \brief 格式化字符串2 */
WINUX_DLL AnsiString Format( char const * fmt, ... );
#endif

/** \brief 填充based-zero字符串缓冲区包装类 */
class WINUX_DLL SZInput
{
public:
    explicit SZInput( char * psz, size_t count ) : _psz(psz), _type(szCharInput), _count(count) { }
    explicit SZInput( wchar_t * pwsz, size_t count ) : _pwsz(pwsz), _type(szWCharInput), _count(count) { }
    SZInput & operator = ( char const * pstr );
    SZInput & operator = ( wchar_t const * pwstr );

private:
    union
    {
        char * _psz;
        wchar_t * _pwsz;
    };
    enum { szCharInput, szWCharInput } _type;
    size_t _count;
};

#if defined(__GNUC__) || _MSC_VER >= 1600
/** \brief VC2010以上支持模板取数组大小 */
template < typename _CHAR, uint _Count >
SZInput SZ( _CHAR (&sz)[_Count] )
{
    return SZInput( sz, _Count );
}

#else
/** \brief 否则使用宏定义 */
#define SZ(sz) SZInput( sz, sizeof(sz) / sizeof(sz[0]) )

#endif
/** \brief 如果操作对象是缓冲区指针，则使用SZP宏 */
#define SZP SZInput

// ----------------------------------------------------------------------------------

/** \brief 字符串编码转换 */
class WINUX_DLL Conv
{
public:
    Conv( AnsiString const & fromCode, AnsiString const & toCode );
    ~Conv();

    /** \brief 进行编码转换
     *
     *  \param srcBuf 需要转换的字符串缓冲区
     *  \param srcSize 缓冲区的大小(in bytes)
     *  \param destBuf 转换得到的结果，函数自动分配内存，用户负责Buffer.Free()释放
     *  \return size_t 输出到destBuf的字节数 */
    size_t convert( char const * srcBuf, size_t srcSize, char * * destBuf );

    /** \brief 进行编码转换
     *
     *  \param _RetString 返回类型
     *  \param _String 要转换的字符串类型 */
    template < typename _RetString, typename _String >
    _RetString convert( _String const & str )
    {
        typename _RetString::pointer buf;
        size_t n = this->convert( (char *)str.c_str(), str.length() * sizeof(typename _String::value_type), (char **)&buf );
        _RetString s( (typename _RetString::pointer)buf, n / sizeof(typename _RetString::value_type) );
        Buffer::Free(buf);
        return s;
    }

private:
    PlainMembers<struct Conv_Data, 8> _self;

    DISABLE_OBJECT_COPY(Conv)
};

/** \brief 本地编码转到指定编码 */
template < typename _ToString >
class ConvTo : public Conv
{
public:
    ConvTo( AnsiString const & toCode ) : Conv( "", toCode )
    {
    }

    _ToString convert( AnsiString const & str )
    {
        return this->Conv::convert<_ToString, AnsiString>(str);
    }

    _ToString operator () ( AnsiString const & str )
    {
        typename _ToString::value_type * buf;
        size_t n = Conv::convert( (char *)str.c_str(), str.length() * sizeof(typename AnsiString::value_type), (char **)&buf );
        _ToString s( buf, n / sizeof(typename _ToString::value_type) );
        Buffer::Free(buf);
        return s;
    }
};

/** \brief 指定编码转到本地编码 */
template < typename _FromString >
class ConvFrom : public Conv
{
public:
    ConvFrom( AnsiString const & fromCode ) : Conv( fromCode, "" )
    {
    }

    AnsiString convert( _FromString const & str )
    {
        return this->Conv::convert<AnsiString, _FromString>(str);
    }

    AnsiString operator () ( _FromString const & str )
    {
        AnsiString::value_type * buf;
        size_t n = Conv::convert( (char *)str.c_str(), str.length() * sizeof(typename _FromString::value_type), (char **)&buf );
        AnsiString s( buf, n / sizeof(AnsiString::value_type) );
        Buffer::Free(buf);
        return s;
    }
};

/** \brief Unicode转换器。可以使UTF-8、UTF-16、UTF-32字符串相互自由转换，字节序为平台默认 */
class WINUX_DLL UnicodeConverter
{
public:
    UnicodeConverter( Utf8String const & utf8str );
    UnicodeConverter( UnicodeString const & wstr );
    UnicodeConverter( Utf16String const & utf16str );
    UnicodeConverter( Utf32String const & utf32str );

    Utf8String toUtf8( size_t newCap = 0 ) const;
    UnicodeString toUnicode( size_t newCap = 0 ) const;
    Utf16String toUtf16( size_t newCap = 0 ) const;
    Utf32String toUtf32( size_t newCap = 0 ) const;

    size_t calcUtf8Length() const;
    size_t calcUnicodeLength() const;
    size_t calcUtf16Length() const;
    size_t calcUtf32Length() const;

private:
    template < typename _Ty >
    _Ty const * _q() const { return reinterpret_cast<_Ty const *>(_p); }

    void const * _p;
    enum {
        ucUtf8,
        ucUnicode,
        ucUtf16,
        ucUtf32
    } _type;
};

/** \brief `UnicodeConverter`简单别名 */
using UC = UnicodeConverter;
// 定义一些转换宏
#define $u8(s) winux::UnicodeConverter(s).toUtf8()
#define $L(s) winux::UnicodeConverter(s).toUnicode()
#define $u(s) winux::UnicodeConverter(s).toUtf16()
#define $U(s) winux::UnicodeConverter(s).toUtf32()

// UTF-8编码转换
// Windows平台默认local就不是utf8，因此不用设置LOCAL_ISNT_UTF8。若设置LOCAL_IS_UTF8，则认为local是utf8
// Unix-like平台默认local就是utf8，因此不用设置LOCAL_IS_UTF8。若设置LOCAL_ISNT_UTF8，则认为local不是utf8
#if defined(OS_WIN) && !defined(LOCAL_IS_UTF8) || defined(LOCAL_ISNT_UTF8)
/** \brief 从utf-8转到本地编码 */
WINUX_FUNC_DECL(AnsiString) LocalFromUtf8( AnsiString const & str );
/** \brief 从本地编码转到utf-8 */
WINUX_FUNC_DECL(AnsiString) LocalToUtf8( AnsiString const & str );
#define LOCAL_FROM_UTF8(s) winux::LocalFromUtf8(s)
#define LOCAL_TO_UTF8(s) winux::LocalToUtf8(s)
#else
/** \brief 从utf-8转到本地编码 */
inline static AnsiString LocalFromUtf8( AnsiString const & str ) { return str; }
/** \brief 从本地编码转到utf-8 */
inline static AnsiString LocalToUtf8( AnsiString const & str ) { return str; }
#define LOCAL_FROM_UTF8(s) s
#define LOCAL_TO_UTF8(s) s
#endif


} // namespace winux

#endif // __STRINGS_HPP__
