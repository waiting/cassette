﻿#ifndef __STRINGS_HPP__
#define __STRINGS_HPP__
//
// strings 提供字符串相关的功能
//

#include <stdarg.h>

namespace winux
{
// 生成不同类型的字符/字符串字面量
#define LITERAL_ITEM_SC( ty, name, pref, content ) \
    static constexpr ty const *name##Str = pref##content; \
    static constexpr ty const name##Char = name##Str[0];

#define LITERAL_ITEM_S( ty, name, pref, content ) \
    static constexpr ty const *name##Str = pref##content;

#define LITERAL_ITEM_LIST( ty, pref ) \
    LITERAL_ITEM_SC(ty, A, pref, "A") \
    LITERAL_ITEM_SC(ty, B, pref, "B") \
    LITERAL_ITEM_SC(ty, C, pref, "C") \
    LITERAL_ITEM_SC(ty, D, pref, "D") \
    LITERAL_ITEM_SC(ty, E, pref, "E") \
    LITERAL_ITEM_SC(ty, F, pref, "F") \
    LITERAL_ITEM_SC(ty, X, pref, "X") \
    LITERAL_ITEM_SC(ty, Z, pref, "Z") \
    LITERAL_ITEM_SC(ty, a, pref, "a") \
    LITERAL_ITEM_SC(ty, b, pref, "b") \
    LITERAL_ITEM_SC(ty, c, pref, "c") \
    LITERAL_ITEM_SC(ty, d, pref, "d") \
    LITERAL_ITEM_SC(ty, e, pref, "e") \
    LITERAL_ITEM_SC(ty, f, pref, "f") \
    LITERAL_ITEM_SC(ty, n, pref, "n") \
    LITERAL_ITEM_SC(ty, r, pref, "r") \
    LITERAL_ITEM_SC(ty, t, pref, "t") \
    LITERAL_ITEM_SC(ty, v, pref, "v") \
    LITERAL_ITEM_SC(ty, x, pref, "x") \
    LITERAL_ITEM_SC(ty, z, pref, "z") \
    LITERAL_ITEM_SC(ty, zero, pref, "0") \
    LITERAL_ITEM_SC(ty, one, pref, "1") \
    LITERAL_ITEM_SC(ty, two, pref, "2") \
    LITERAL_ITEM_SC(ty, three, pref, "3") \
    LITERAL_ITEM_SC(ty, four, pref, "4") \
    LITERAL_ITEM_SC(ty, five, pref, "5") \
    LITERAL_ITEM_SC(ty, six, pref, "6") \
    LITERAL_ITEM_SC(ty, seven, pref, "7") \
    LITERAL_ITEM_SC(ty, eight, pref, "8") \
    LITERAL_ITEM_SC(ty, nine, pref, "9") \
    LITERAL_ITEM_SC(ty, nul, pref, "\0") \
    LITERAL_ITEM_SC(ty, bel, pref, "\a") \
    LITERAL_ITEM_SC(ty, bs, pref, "\b") \
    LITERAL_ITEM_SC(ty, ht, pref, "\t") \
    LITERAL_ITEM_SC(ty, lf, pref, "\n") \
    LITERAL_ITEM_SC(ty, vt, pref, "\v") \
    LITERAL_ITEM_SC(ty, ff, pref, "\f") \
    LITERAL_ITEM_SC(ty, cr, pref, "\r") \
    LITERAL_ITEM_S(ty, empty, pref, "") \
    LITERAL_ITEM_SC(ty, space, pref, " ") \
    LITERAL_ITEM_SC(ty, under, pref, "_") \
    LITERAL_ITEM_SC(ty, dollar, pref, "$") \
    LITERAL_ITEM_SC(ty, slash, pref, "\\") \
    LITERAL_ITEM_SC(ty, divide, pref, "/") \
    LITERAL_ITEM_SC(ty, positive, pref, "+") \
    LITERAL_ITEM_SC(ty, negative, pref, "-") \
    LITERAL_ITEM_SC(ty, quote, pref, "\"") \
    LITERAL_ITEM_SC(ty, apos, pref, "\'") \
    LITERAL_ITEM_SC(ty, sharp, pref, "#") \
    LITERAL_ITEM_SC(ty, caret, pref, "^") \
    LITERAL_ITEM_SC(ty, pipe, pref, "|") \
    LITERAL_ITEM_SC(ty, amp, pref, "&") \
    LITERAL_ITEM_SC(ty, lt, pref, "<") \
    LITERAL_ITEM_SC(ty, gt, pref, ">") \
    LITERAL_ITEM_SC(ty, dbldivide, pref, "//") \
    LITERAL_ITEM_S(ty, crlf, pref, "\r\n") \
    LITERAL_ITEM_S(ty, slash_a, pref, "\\a") \
    LITERAL_ITEM_S(ty, slash_b, pref, "\\b") \
    LITERAL_ITEM_S(ty, slash_t, pref, "\\t") \
    LITERAL_ITEM_S(ty, slash_n, pref, "\\n") \
    LITERAL_ITEM_S(ty, slash_v, pref, "\\v") \
    LITERAL_ITEM_S(ty, slash_f, pref, "\\f") \
    LITERAL_ITEM_S(ty, slash_r, pref, "\\r") \
    LITERAL_ITEM_S(ty, slash_x, pref, "\\x") \
    LITERAL_ITEM_S(ty, cslashes, pref, "\n\r\t\v\a\\\'\"")

template < typename _ChTy >
struct Literal
{
    LITERAL_ITEM_LIST( char, );
};

template <>
struct Literal<char>
{
    LITERAL_ITEM_LIST( char, );
};

template <>
struct Literal<wchar_t>
{
    LITERAL_ITEM_LIST( wchar_t, L );
};

template <>
struct Literal<char16_t>
{
    LITERAL_ITEM_LIST( char16_t, u );
};

template <>
struct Literal<char32_t>
{
    LITERAL_ITEM_LIST( char32_t, U );
};


WINUX_FUNC_DECL(size_t) StrSplitA( AnsiString const & str, AnsiString const & delimList, AnsiStringArray * arr, bool alwaysRetOneElem = false );
WINUX_FUNC_DECL(size_t) StrSplitW( UnicodeString const & str, UnicodeString const & delimList, UnicodeStringArray * arr, bool alwaysRetOneElem = false );
/** \brief 字符串分割
 *
 *  delimList指示出分割符列表，分割符只能是一个字符，函数会把str内分割符间的内容(即使是空串)添加到arr中，返回个数。\n
 *  当alwaysRetOneElem==true时，即使str是空串时也会向arr返回一个数组元素，元素值是空串。
 *  \param str
 *  \param delimList
 *  \param arr
 *  \return size_t */
#if defined(_UNICODE) || defined(UNICODE)
inline size_t StrSplit( UnicodeString const & str, UnicodeString const & delimList, UnicodeStringArray * arr, bool alwaysRetOneElem = false ) { return StrSplitW( str, delimList, arr, alwaysRetOneElem ); }
#else
inline size_t StrSplit( AnsiString const & str, AnsiString const & delimList, AnsiStringArray * arr, bool alwaysRetOneElem = false ) { return StrSplitA( str, delimList, arr, alwaysRetOneElem ); }
#endif


WINUX_FUNC_DECL(size_t) StrSplit2A( AnsiString const & str, AnsiString const & delim, AnsiStringArray * arr, bool alwaysRetOneElem = false );
WINUX_FUNC_DECL(size_t) StrSplit2W( UnicodeString const & str, UnicodeString const & delim, UnicodeStringArray * arr, bool alwaysRetOneElem = false );
/** \brief 字符串分割2
 *
 *  delim指示出分割字符串，函数会把str内分割字符串间的内容(即使是空串)添加到arr中，返回个数。\n
 *  当alwaysRetOneElem==true时，即使str是空串时也会向arr返回一个数组元素，元素值是空串。
 *  \param str
 *  \param delim
 *  \param arr
 *  \return size_t */
#if defined(_UNICODE) || defined(UNICODE)
inline size_t StrSplit2( UnicodeString const & str, UnicodeString const & delim, UnicodeStringArray * arr, bool alwaysRetOneElem = false ) { return StrSplit2W( str, delim, arr, alwaysRetOneElem ); }
#else
inline size_t StrSplit2( AnsiString const & str, AnsiString const & delim, AnsiStringArray * arr, bool alwaysRetOneElem = false ) { return StrSplit2A( str, delim, arr, alwaysRetOneElem ); }
#endif


WINUX_FUNC_DECL(AnsiString) StrJoinA( AnsiString const & delim, AnsiStringArray const & arr );
WINUX_FUNC_DECL(UnicodeString) StrJoinW( UnicodeString const & delim, UnicodeStringArray const & arr );
/** \brief 字符串组合连接 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StrJoin( UnicodeString const & delim, UnicodeStringArray const & arr ) { return StrJoinW( delim, arr ); }
#else
inline AnsiString StrJoin( AnsiString const & delim, AnsiStringArray const & arr ) { return StrJoinA( delim, arr ); }
#endif


WINUX_FUNC_DECL(AnsiString) StrJoinExA( AnsiString const & delim, AnsiStringArray const & arr, size_t start = 0, size_t elemCount = -1 );
WINUX_FUNC_DECL(UnicodeString) StrJoinExW( UnicodeString const & delim, UnicodeStringArray const & arr, size_t start = 0, size_t elemCount = -1 );
/** \brief 字符串组合连接。start表示开始位置，elemCount表示自开始位置的元素数，默认-1表示自开始位置的全部元素 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StrJoinEx( UnicodeString const & delim, UnicodeStringArray const & arr, size_t start = 0, size_t elemCount = -1 ) { return StrJoinExW( delim, arr, start, elemCount ); }
#else
inline AnsiString StrJoinEx( AnsiString const & delim, AnsiStringArray const & arr, size_t start = 0, size_t elemCount = -1 ) { return StrJoinExA( delim, arr, start, elemCount ); }
#endif


WINUX_FUNC_DECL(AnsiString) StrInsertA( AnsiString const & str, size_t start, size_t end, AnsiString const & insert );
WINUX_FUNC_DECL(UnicodeString) StrInsertW( UnicodeString const & str, size_t start, size_t end, UnicodeString const & insert );
/** \brief 在指定位置插入字符串 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StrInsert( UnicodeString const & str, size_t start, size_t end, UnicodeString const & insert ) { return StrInsertW( str, start, end, insert ); }
#else
inline AnsiString StrInsert( AnsiString const & str, size_t start, size_t end, AnsiString const & insert ) { return StrInsertA( str, start, end, insert ); }
#endif


WINUX_FUNC_DECL(AnsiString &) StrMakeReplaceA( AnsiString * str, AnsiString const & findText, AnsiString const & replaceText, size_t offset = 0 );
WINUX_FUNC_DECL(UnicodeString &) StrMakeReplaceW( UnicodeString * str, UnicodeString const & findText, UnicodeString const & replaceText, size_t offset = 0 );
/** \brief 使字符串全文替换 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString & StrMakeReplace( UnicodeString * str, UnicodeString const & findText, UnicodeString const & replaceText, size_t offset = 0 ) { return StrMakeReplaceW( str, findText, replaceText, offset ); }
#else
inline AnsiString & StrMakeReplace( AnsiString * str, AnsiString const & findText, AnsiString const & replaceText, size_t offset = 0 ) { return StrMakeReplaceA( str, findText, replaceText, offset ); }
#endif


WINUX_FUNC_DECL(AnsiString) StrTrimA( AnsiString const & str );
WINUX_FUNC_DECL(UnicodeString) StrTrimW( UnicodeString const & str );
/** \brief 祛除字符串首尾出现的空白字符
*
* \param str
* \return String */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StrTrim( UnicodeString const & str ) { return StrTrimW(str); }
#else
inline AnsiString StrTrim( AnsiString const & str ) { return StrTrimA(str); }
#endif


WINUX_FUNC_DECL(AnsiString &) StrMakeUpperA( AnsiString * str );
WINUX_FUNC_DECL(UnicodeString &) StrMakeUpperW( UnicodeString * str );
/** \brief 使字符串大写 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString & StrMakeUpper( UnicodeString * str ) { return StrMakeUpperW(str); }
#else
inline AnsiString & StrMakeUpper( AnsiString * str ) { return StrMakeUpperA(str); }
#endif

WINUX_FUNC_DECL(AnsiString) StrUpperA( AnsiString str );
WINUX_FUNC_DECL(UnicodeString) StrUpperW( UnicodeString str );
/** \brief 使字符串大写 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StrUpper( UnicodeString str ) { return StrUpperW(str); }
#else
inline AnsiString StrUpper( AnsiString str ) { return StrUpperA(str); }
#endif


WINUX_FUNC_DECL(AnsiString &) StrMakeLowerA( AnsiString * str );
WINUX_FUNC_DECL(UnicodeString &) StrMakeLowerW( UnicodeString * str );
/** \brief 使字符串小写 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString & StrMakeLower( UnicodeString * str ) { return StrMakeLowerW(str); }
#else
inline AnsiString & StrMakeLower( AnsiString * str ) { return StrMakeLowerA(str); }
#endif

WINUX_FUNC_DECL(AnsiString) StrLowerA( AnsiString str );
WINUX_FUNC_DECL(UnicodeString) StrLowerW( UnicodeString str );
/** \brief 使字符串小写 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StrLower( UnicodeString str ) { return StrLowerW(str); }
#else
inline AnsiString StrLower( AnsiString str ) { return StrLowerA(str); }
#endif


WINUX_FUNC_DECL(AnsiString) StrMultipleA( AnsiString const & str, int multiple );
WINUX_FUNC_DECL(UnicodeString) StrMultipleW( UnicodeString const & str, int multiple );
/** \brief 字符串倍数的出现 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StrMultiple( UnicodeString const & str, int multiple ) { return StrMultipleW( str, multiple ); }
#else
inline AnsiString StrMultiple( AnsiString const & str, int multiple ) { return StrMultipleA( str, multiple ); }
#endif


WINUX_FUNC_DECL(AnsiString) StrSubtractA( AnsiString str1, AnsiString const & str2 );
WINUX_FUNC_DECL(UnicodeString) StrSubtractW( UnicodeString str1, UnicodeString const & str2 );
/** \brief 字符串相减，str1 - str2，即去掉str1里与str2相同的部分。限制：str1 >= str2。 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StrSubtract( UnicodeString str1, UnicodeString const & str2 ) { return StrSubtractW( str1, str2 ); }
#else
inline AnsiString StrSubtract( AnsiString str1, AnsiString const & str2 ) { return StrSubtractA( str1, str2 ); }
#endif


/** \brief 字符串转换成数字Flags */
enum StrToXqFlags
{
    stqUnsigned = 1, //!< 无符号处理
    stqNegative = 2, //!< 有解析到负号
    stqOverflow = 4, //!< 发生溢出
    stqReadDigit = 8 //!< 读到一个正确的数字字符
};

WINUX_FUNC_DECL(uint64) StrToXqA( char const * nptr, char const ** endptr, int ibase, int flags );
WINUX_FUNC_DECL(uint64) StrToXqW( wchar const * nptr, wchar const ** endptr, int ibase, int flags );
/** \brief 字符串转换成64位的数字 */
#if defined(_UNICODE) || defined(UNICODE)
inline uint64 StrToXq( wchar const * nptr, wchar const ** endptr, int ibase, int flags ) { return StrToXqW( nptr, endptr, ibase, flags ); }
#else
inline uint64 StrToXq( char const * nptr, char const ** endptr, int ibase, int flags ) { return StrToXqA( nptr, endptr, ibase, flags ); }
#endif

WINUX_FUNC_DECL(uint64) StrToXqU16( char16 const * nptr, char16 const ** endptr, int ibase, int flags );
WINUX_FUNC_DECL(uint64) StrToXqU32( char32 const * nptr, char32 const ** endptr, int ibase, int flags );


WINUX_FUNC_DECL(int64) StrToInt64A( AnsiString const & numStr, int ibase );
WINUX_FUNC_DECL(int64) StrToInt64W( UnicodeString const & numStr, int ibase );
#if defined(_UNICODE) || defined(UNICODE)
inline int64 StrToInt64( UnicodeString const & numStr, int ibase ) { return StrToInt64W( numStr, ibase ); }
#else
inline int64 StrToInt64( AnsiString const & numStr, int ibase ) { return StrToInt64A( numStr, ibase ); }
#endif


WINUX_FUNC_DECL(uint64) StrToUint64A( AnsiString const & numStr, int ibase );
WINUX_FUNC_DECL(uint64) StrToUint64W( UnicodeString const & numStr, int ibase );
#if defined(_UNICODE) || defined(UNICODE)
inline uint64 StrToUint64( UnicodeString const & numStr, int ibase ) { return StrToUint64W( numStr, ibase ); }
#else
inline uint64 StrToUint64( AnsiString const & numStr, int ibase ) { return StrToUint64A( numStr, ibase ); }
#endif


WINUX_FUNC_DECL(AnsiString) AddSlashesA( AnsiString const & str, AnsiString const & charlist );
WINUX_FUNC_DECL(UnicodeString) AddSlashesW( UnicodeString const & str, UnicodeString const & charlist );
/** \brief 加反斜杠 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString AddSlashes( UnicodeString const & str, UnicodeString const & charlist ) { return AddSlashesW( str, charlist ); }
#else
inline AnsiString AddSlashes( AnsiString const & str, AnsiString const & charlist ) { return AddSlashesA( str, charlist ); }
#endif


inline AnsiString AddCSlashesA( AnsiString const & str ) { return AddSlashesA( str, Literal<AnsiString::value_type>::cslashesStr ); }
inline UnicodeString AddCSlashesW( UnicodeString const & str ) { return AddSlashesW( str, Literal<UnicodeString::value_type>::cslashesStr ); }
/** \brief 加C-Style反斜杠 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString AddCSlashes( UnicodeString const & str ) { return AddCSlashesW(str); }
#else
inline AnsiString AddCSlashes( AnsiString const & str ) { return AddCSlashesA(str); }
#endif


WINUX_FUNC_DECL(AnsiString) StripSlashesA( AnsiString const & str, AnsiString const & charlist );
WINUX_FUNC_DECL(UnicodeString) StripSlashesW( UnicodeString const & str, UnicodeString const & charlist );
/** \brief 去掉反斜杠 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StripSlashes( UnicodeString const & str, UnicodeString const & charlist ) { return StripSlashesW( str, charlist ); }
#else
inline AnsiString StripSlashes( AnsiString const & str, AnsiString const & charlist ) { return StripSlashesA( str, charlist ); }
#endif


inline AnsiString StripCSlashesA( AnsiString const & str ) { return StripSlashesA( str, Literal<AnsiString::value_type>::cslashesStr ); }
inline UnicodeString StripCSlashesW( UnicodeString const & str ) { return StripSlashesW( str, Literal<UnicodeString::value_type>::cslashesStr ); }
/** \brief 去掉C-Style反斜杠 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString StripCSlashes( UnicodeString const & str ) { return StripCSlashesW(str); }
#else
inline AnsiString StripCSlashes( AnsiString const & str ) { return StripCSlashesA(str); }
#endif


WINUX_FUNC_DECL(AnsiString) AddQuotesA( AnsiString const & str, AnsiString::value_type quote = Literal<AnsiString::value_type>::quoteChar );
WINUX_FUNC_DECL(UnicodeString) AddQuotesW( UnicodeString const & str, UnicodeString::value_type quote = Literal<UnicodeString::value_type>::quoteChar );
/** \brief double引号 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString AddQuotes( UnicodeString const & str, UnicodeString::value_type quote = Literal<UnicodeString::value_type>::quoteChar ) { return AddQuotesW( str, quote ); }
#else
inline AnsiString AddQuotes( AnsiString const & str, AnsiString::value_type quote = Literal<AnsiString::value_type>::quoteChar ) { return AddQuotesA( str, quote ); }
#endif


WINUX_FUNC_DECL(bool) StrGetLineA( AnsiString * line, AnsiString const & str, size_t * i, AnsiString * nl = nullptr );
WINUX_FUNC_DECL(bool) StrGetLineW( UnicodeString * line, UnicodeString const & str, size_t * i, UnicodeString * nl = nullptr );
/** \brief 获取字符串中的一行,支持unix，windows，mac平台的行分隔方式\n
 *  line 不包含换行符，i 指示起始位置，并获得处理到哪个位置。
 *
 *  如何统一处理3平台的文本文件：\n
 *  最好的办法是在该平台用文本模式打开该平台产生的文本文件，然后操作。\n
 *  然而，现实不像想象的那么美好，多数情况下是处理不同平台下的文本文件，譬如在unix系平台下处理windows或mac的文本文件。\n
 *  由于行分隔不同，导致处理有一定困难。\n
 *  此函数正为此而存在。首先，你需要用二进制模式打开文件，然后读取全部数据，调用此函数取行即可。 */
#if defined(_UNICODE) || defined(UNICODE)
inline bool StrGetLine( UnicodeString * line, UnicodeString const & str, size_t * i, UnicodeString * nl = nullptr ) { return StrGetLineW( line, str, i, nl ); }
#else
inline bool StrGetLine( AnsiString * line, AnsiString const & str, size_t * i, AnsiString * nl = nullptr ) { return StrGetLineA( line, str, i, nl ); }
#endif


/** \brief 整理标识符串的标记 */
enum CollateIdentifierStringFlag : winux::uint
{
    wordRaw = 0x00, //!< 不处理单词
    wordAllUpper = 0x01, //!< 单词大写
    wordAllLower = 0x02, //!< 单词小写
    wordFirstCharUpper = 0x04, //!< 首字母大写

    //nameRaw = 0x00,
    nameSmallHump = 0x10, //!< 小驼峰
    //nameBigHump = 0x20,
    //nameShortLine = 0x40,
    //nameUnderline = 0x80,
};

WINUX_FUNC_DECL(AnsiString) CollateIdentifierToStringA( AnsiString const & identifier, AnsiString const & sep, winux::uint flags = wordFirstCharUpper );
WINUX_FUNC_DECL(UnicodeString) CollateIdentifierToStringW( UnicodeString const & identifier, UnicodeString const & sep, winux::uint flags = wordFirstCharUpper );
/** \brief 整理标识符为字符串 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString CollateIdentifierToString( UnicodeString const & identifier, UnicodeString const & sep, winux::uint flags = wordFirstCharUpper ) { return CollateIdentifierToStringW( identifier, sep, flags ); }
#else
inline AnsiString CollateIdentifierToString( AnsiString const & identifier, AnsiString const & sep, winux::uint flags = wordFirstCharUpper ) { return CollateIdentifierToStringA( identifier, sep, flags ); }
#endif


WINUX_FUNC_DECL(AnsiStringArray) CollateIdentifierToArrayA( AnsiString const & identifier, winux::uint flags = wordFirstCharUpper );
WINUX_FUNC_DECL(UnicodeStringArray) CollateIdentifierToArrayW( UnicodeString const & identifier, winux::uint flags = wordFirstCharUpper );
/** \brief 整理标识符为单词数组 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeStringArray CollateIdentifierToArray( UnicodeString const & identifier, winux::uint flags = wordFirstCharUpper ) { return CollateIdentifierToArrayW( identifier, flags ); }
#else
inline AnsiStringArray CollateIdentifierToArray( AnsiString const & identifier, winux::uint flags = wordFirstCharUpper ) { return CollateIdentifierToArrayA( identifier, flags ); }
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
WINUX_FUNC_DECL(std::vector<int>) KmpCalcNext( char const * substr, size_t sublen );
/** \brief KMP字符串匹配算法：传入已经求好的next进行匹配 */
WINUX_FUNC_DECL(size_t) KmpMatchEx( char const * str, size_t len, char const * substr, size_t sublen, size_t pos, std::vector<int> const & next );
/** \brief KMP字符串匹配算法：匹配 */
WINUX_FUNC_DECL(size_t) KmpMatch( char const * str, size_t len, char const * substr, size_t sublen, size_t pos );


/** \brief 用来使得String能够用operator<<来赋值 */
template < typename _ChTy >
class XStringWriter
{
    XString<_ChTy> * _str;
    std::basic_ostringstream<_ChTy> * _sout;
    bool _isAppend;
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
    std::ostream & operator << ( _AnyType && t )
    {
        assert( _sout != NULL );
        return *_sout << std::forward<_AnyType>(t);
    }
};

using StringWriter = XStringWriter<tchar>;
using AnsiStringWriter = XStringWriter<char>;
using UnicodeStringWriter = XStringWriter<wchar>;

/** \brief KMP多项匹配/替换 */
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
    template < size_t _N >
    MultiMatch( String (&matches)[_N], String (&replaces)[_N] ) : _replaceFunc(NULL), _extra(NULL)
    {
        this->init( StringArray( matches, matches + _N ), StringArray( replaces, replaces + _N ) );
    }

    /** \brief 构造函数4，要求匹配项和替换函数，若不进行替换，则replaceFunc可为NULL */
    template < size_t _N >
    MultiMatch( String (&matches)[_N], ReplaceFuncType replaceFunc, void * extra = NULL ) : _replaceFunc(NULL), _extra(NULL)
    {
        this->init( StringArray( matches, matches + _N ), replaceFunc, extra );
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

    /** \brief 搜索任意一项匹配（非KMP算法）
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
    String replace( String const & str, ssize_t offset = 0, SearchFuncType fnSearch = &MultiMatch::search ) const;

private:
    std::vector<KmpNextValueArray> _nextVals;
    StringArray _matchItems;
    StringArray _replaceItems;
    ReplaceFuncType _replaceFunc;
    void * _extra;
};

/** \brief 设置locale信息 */
class WINUX_DLL Locale
{
    AnsiString _loc;
    AnsiString _prevLoc;
    static AnsiString _clsLc;
public:
    static void Set( char const * lc ) { _clsLc = lc; }
    static char const * Get() { return _clsLc.c_str(); }
    Locale( char const * lc = NULL );
    ~Locale();
    DISABLE_OBJECT_COPY(Locale)
};

/** \brief 返回一个本地字符串里有多少个实际的字符(by local CodePage),用于mbstowcs */
WINUX_FUNC_DECL(size_t) LocalCharsCount( AnsiString const & local );
/** \brief 返回一个unicode字符串转换为多字节字符串最少需要多少字节(by local CodePage),用于wcstombs */
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
inline UnicodeString FormatExVW( size_t cch, wchar const * fmt, va_list args ) { return FormatExVW( cch, fmt, args ); }
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
    union
    {
        char * _psz;
        wchar_t * _pwsz;
    };
    enum { szCharInput, szWCharInput } _type;
    size_t _count;
public:
    explicit SZInput( char * psz, size_t count ) : _psz(psz), _type(szCharInput), _count(count) { }
    explicit SZInput( wchar_t * pwsz, size_t count ) : _pwsz(pwsz), _type(szWCharInput), _count(count) { }
    SZInput & operator = ( char const * pstr );
    SZInput & operator = ( wchar_t const * pwstr );

};

#if defined(__GNUC__) || _MSC_VER >= 1600
/* VC2010以上支持模板取数组大小 */
template < typename _CHAR, uint _N >
SZInput SZ( _CHAR (&sz)[_N] )
{
    return SZInput( sz, _N );
}

#else
/* 否则使用宏定义 */
#define SZ(sz) SZInput( sz, sizeof(sz) / sizeof(sz[0]) )

#endif
/* 如果操作对象是缓冲区指针，则使用SZP宏 */
#define SZP SZInput

// ----------------------------------------------------------------------------------

/** \brief 字符串编码转换 */
class WINUX_DLL Conv
{
public:
    Conv( char const * fromCode, char const * toCode );
    ~Conv();

    /** \brief 进行编码转换
     *
     *  \param srcBuf 需要转换的字符串缓冲区
     *  \param srcSize 缓冲区的大小(in bytes)
     *  \param destBuf 转换得到的结果,函数自动分配内存,用户负责free()释放
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
        size_t outBytes = this->convert( (char *)str.c_str(), ( str.length() + 1 ) * sizeof(typename _String::value_type), (char **)&buf );
        _RetString s = (typename _RetString::pointer)buf;
        free(buf);
        return s;
    }
private:
    MembersWrapper<struct Conv_Data> _self;

    DISABLE_OBJECT_COPY(Conv)
};

/** \brief 本地编码转到指定编码 */
template < typename _ToString >
class ConvTo : public Conv
{
public:
    ConvTo( char const * toCode ) : Conv( "", toCode )
    {
    }

    _ToString convert( AnsiString const & str )
    {
        return this->Conv::convert<_ToString, AnsiString>(str);
    }

    _ToString operator () ( AnsiString const & str )
    {
        typename _ToString::value_type * buf;
        Conv::convert( (char *)str.c_str(), (str.length() + 1) * sizeof(typename AnsiString::value_type), (char **)&buf );
        _ToString s = buf;
        free(buf);
        return s;
    }
};

/** \brief 指定编码转到本地编码 */
template < typename _FromString >
class ConvFrom : public Conv
{
public:
    ConvFrom( char const * fromCode ) : Conv( fromCode, "" )
    {
    }

    AnsiString convert( _FromString const & str )
    {
        return this->Conv::convert<AnsiString, _FromString>(str);
    }

    AnsiString operator () ( _FromString const & str )
    {
        AnsiString::value_type * buf;
        Conv::convert( (char *)str.c_str(), (str.length() + 1) * sizeof(typename _FromString::value_type), (char **)&buf );
        AnsiString s = buf;
        free(buf);
        return s;
    }

};

// UTF-8编码转换
/** \brief 从utf-8转到本地编码 */
WINUX_FUNC_DECL(AnsiString) LocalFromUtf8( AnsiString const & str );
/** \brief 从本地编码转到utf-8 */
WINUX_FUNC_DECL(AnsiString) LocalToUtf8( AnsiString const & str );


} // namespace winux

#endif // __STRINGS_HPP__
