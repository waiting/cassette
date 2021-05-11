//////////////////////////////////////////////////////////////////////////
// purpose: WinPlus库相关定义
// author:  WT
//////////////////////////////////////////////////////////////////////////

#if !defined(__WINPLUS_DEFINITIONS_HPP__)
#define __WINPLUS_DEFINITIONS_HPP__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )
#pragma warning( disable : 4663 )
#pragma warning( disable : 4239 )
#pragma warning( disable : 4018 )
#pragma warning( disable : 4290 )

#ifdef __GNUC__
#define WINVER 0x0501
#define _WIN32_IE 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

// 以下是有DLL和无DLL的兼容性宏
// 当存在DLL时(生成DLL或者使用DLL), WINPLUS_DLL_EXPORTS宏用于决定是导出(生成)还是导入(使用).
// WINPLUS_API用于决定调用约定, 有dll时为stdcall, 无dll时为默认.
// 当无DLL时, 宏为空白, 用以兼容.
#ifndef WINPLUS_DLL_USE

    #define WINPLUS_DLL
    #define WINPLUS_API

#else

    #pragma warning( disable: 4251 )
    #ifdef  WINPLUS_DLL_EXPORTS
        #define WINPLUS_DLL __declspec(dllexport)
    #else
        #define WINPLUS_DLL __declspec(dllimport)
    #endif

    #define WINPLUS_API __stdcall

#endif

#define WINPLUS_FUNC_DECL(ret) WINPLUS_DLL ret WINPLUS_API
#define WINPLUS_FUNC_IMPL(ret) ret WINPLUS_API

// win32 platform --------------------------------------------------------
#include <windows.h>
#include <comdef.h> // com wrapper
#include <shlobj.h> // shell 外壳
#include <gdiplus.h> // gdi+
//#include <dsclient.h> // DsClient
// cpp stl ---------------------------------------------------------------
#include <vector>
#include <list>
#include <map>
#include <string>
#include <iostream>
#include <exception>
// c standard functions --------------------------------------------------
#include <math.h>
#include <io.h>
// custom macro definitions ----------------------------------------------
namespace winplus
{

#ifdef NOMINMAX

    #ifndef max
        #define max(a,b)            (((a) > (b)) ? (a) : (b))
    #endif

    #ifndef min
        #define min(a,b)            (((a) < (b)) ? (a) : (b))
    #endif

#endif

#ifndef countof
#define countof(arr) ( sizeof(arr) / sizeof(arr[0]) )
#endif

// 缓冲区转换为ansi_string二进制串
#define BufferToAnsiString( buf, size ) AnsiString( (char const *)(buf), (size_t)(size) )

// if pointer
#define IfPTR(ptr) if( (ptr) != NULL ) (ptr)
#define AssignPTR(ptr) if( (ptr) != NULL ) *(ptr)

// 类型支持 --------------------------------------------------------------

typedef std::basic_string<CHAR> AnsiString;
typedef AnsiString LocalString;
typedef std::basic_string<WCHAR> UnicodeString;
typedef std::basic_string<TCHAR> String;

typedef std::vector<AnsiString> AnsiStringArray;
typedef std::vector<LocalString> LocalStringArray;
typedef std::vector<UnicodeString> UnicodeStringArray;
typedef std::vector<String> StringArray;

typedef std::list<AnsiString> AnsiStringList;
typedef std::list<LocalString> LocalStringList;
typedef std::list<UnicodeString> UnicodeStringList;
typedef std::list<String> StringList;

typedef std::map< AnsiString, AnsiString > AnsiStringMap;
typedef std::map<String, String> StringMap;
// basic types -----------------------------------------------------------
typedef wchar_t wchar;

typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned short ushort;

#ifdef __GNUC__
typedef unsigned long long uint64;
typedef unsigned long long ulonglong;
typedef long long int64;
typedef long long longlong;
#else
typedef unsigned __int64 uint64;
typedef unsigned __int64 ulonglong;
typedef __int64 int64;
typedef __int64 longlong;
#endif

#ifndef byte
typedef unsigned char byte;
#endif

// 混合体，能表示多种类型的值，MT_ARRAY和MTBINARY暂不支持 ----------------
enum MixedType
{
    MT_NULL,
    MT_BOOLEAN,
    MT_BYTE,
    MT_SHORT, MT_USHORT,
    MT_INT, MT_UINT,
    MT_LONG, MT_ULONG,
    MT_FLOAT,
    MT_INT64, MT_UINT64,
    MT_DOUBLE,
    MT_STRING, MT_UNICODE,
    MT_ARRAY,
    MT_BINARY,
};

/* 混合值结构体 */
struct MIXED
{
    MixedType type;
    uint buf_size;  // 当是指针时，此域表示缓冲区长度
    union
    {
        bool boolVal;
        byte btVal;
        short shVal;
        ushort ushVal;
        int iVal;
        uint uiVal;
        long lVal;
        ulong ulVal;
        float fltVal;
        int64 i64Val;
        uint64 ui64Val;
        double dblVal;
        struct // 字符串
        {
            int str_length; // 字符串长度：字符数
            union
            {
                PSTR pStr;
                PWSTR pWStr;
            };
        };
        struct // 数组
        {
            int elements_count;
            struct MIXED * pArray;
        };
        byte * pBinary; // 二进制数据
    };
};

/* 混合类型 */
class WINPLUS_DLL Mixed : public MIXED
{
public:
    Mixed( void );
    Mixed( AnsiString const & str ); // 多字节字符串
    Mixed( UnicodeString const & str ); // Unicode字符串
    Mixed( PCSTR str ); // 多字节字符串
    Mixed( PCWSTR str ); // Unicode字符串
    Mixed( bool boolVal );
    Mixed( byte btVal );
    Mixed( short shVal );
    Mixed( ushort ushVal );
    Mixed( int iVal );
    Mixed( uint uiVal );
    Mixed( long lVal );
    Mixed( ulong ulVal );
    Mixed( float fltVal );
    Mixed( int64 i64Val );
    Mixed( uint64 ui64Val );
    Mixed( double dblVal );
    Mixed( Mixed const & other );
    ~Mixed( void );
    Mixed & operator = ( Mixed const & other );
    // 类型转换
    operator AnsiString( void ) const;
    operator UnicodeString( void ) const;
    operator bool( void ) const;
    operator byte( void ) const;
    operator short( void ) const;
    operator ushort( void ) const;
    operator int( void ) const;
    operator uint( void ) const;
    operator long( void ) const;
    operator ulong( void ) const;
    operator float( void ) const;
    operator int64( void ) const;
    operator uint64( void ) const;
    operator double( void ) const;
protected:
    void free( void );
    void assign( PCSTR str );
    void assign( PCWSTR str );
    void assign( bool boolVal );
    void assign( byte btVal );
    void assign( short shVal );
    void assign( ushort ushVal );
    void assign( int iVal );
    void assign( uint uiVal );
    void assign( long lVal );
    void assign( ulong ulVal );
    void assign( float fltVal );
    void assign( int64 i64Val );
    void assign( uint64 ui64Val );
    void assign( double dblVal );

    void _construct();
};

// map 支持 --------------------------------------------------------------
/* 检测map中是否有该键的值 */
template < typename _MAP, typename _KEY >
inline bool isset( _MAP const & m, _KEY const & k )
{
    return m.find(k) != m.end();
}

// 字符串转换到64位整数 --------------------------------------------------
WINPLUS_FUNC_DECL(int64) StrToInt64( AnsiString const & num_str, int ibase );
WINPLUS_FUNC_DECL(uint64) StrToUint64( AnsiString const & num_str, int ibase );

/* 随机数,随机产生n1~n2的数字. 包括n1,n2本身 */
WINPLUS_FUNC_DECL(int) Random( int n1, int n2 );
// 模板元编程支持 --------------------------------------------------------
/* 调用一个返回void的函数或函数对象,返回一个数字
   通常是为了在初始化语句中方便调用返回void的函数 */
template < typename FN >
int VoidReturnInt( FN fn )
{
    fn();
    return 1;
}
template < typename FN, typename ARG1 >
int VoidReturnInt( FN fn, ARG1 a1 )
{
    fn(a1);
    return 1;
}
template < typename FN, typename ARG1, typename ARG2 >
int VoidReturnInt( FN fn, ARG1 a1, ARG2 a2 )
{
    fn( a1, a2 );
    return 1;
}
template < typename FN, typename ARG1, typename ARG2, typename ARG3 >
int VoidReturnInt( FN fn, ARG1 a1, ARG2 a2, ARG3 a3 )
{
    fn( a1, a2, a3 );
    return 1;
}


/* 二进制数,编译时计算, 1开头(基于10进制) */
template < uint64 n > struct Bin1
{
    enum { val = ( Bin1< n / 10 >::val  << 1 ) + n % 10 };
};

template <> struct Bin1<0>
{
    enum { val = 0 };
};

/* 二进制数,编译时计算, 0开头(基于8进制) */
template < uint64 n > struct Bin0
{
    enum { val = ( Bin0< n / 8 >::val  << 1 ) + n % 8 };
};

template <> struct Bin0<0>
{
    enum { val = 0 };
};

} // namespace winplus

#endif // !defined(__WINPLUS_DEFINITIONS_HPP__)
