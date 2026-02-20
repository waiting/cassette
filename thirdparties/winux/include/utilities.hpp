#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__
//
// utilities 提供基础的宏，模板，类，函数
//

// 各平台条件编译宏检测
#include "system_detection.inl"

#if _MSC_VER > 0 && _MSC_VER < 1201
#pragma warning( disable: 4786 )
#endif
#if _MSC_VER > 0
#pragma warning( disable : 4996 )
#endif

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <tuple>
#include <queue>
#include <functional>
#include <algorithm>
#include <atomic>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#if defined(OS_WIN)
#include <windows.h>
#endif

/** \brief 跨平台基础功能库 */
namespace winux
{
// 一些宏定义 ----------------------------------------------------------------------------------
/*  Dll相关宏定义:
    WINUX_DLL_USE     - 此宏开关表示有DLL参与,包括生成DLL或者导入DLL,不定义此宏和一般的使用源码没区别
    WINUX_DLL_EXPORTS - 此宏开关表示是生成DLL(dllexport)还是导入DLL(dllimport),linux平台下忽略
    WINUX_DLL         - 标记函数、类、变量,用于标明其要从DLL导出还是导入,linux平台下忽略
    WINUX_API         - 标记函数调用约定,Win下Dll参与时为stdcall,否则为空白默认,linux平台下忽略
    WINUX_FUNC_DECL   - 标记函数声明
    WINUX_FUNC_IMPL   - 标记函数实现 */
#ifdef WINUX_DLL_USE
    #if defined(_MSC_VER) || defined(WIN32)
        #pragma warning( disable: 4251 )
        #pragma warning( disable: 4275 )
        #ifdef  WINUX_DLL_EXPORTS
            #define WINUX_DLL  __declspec(dllexport)
        #else
            #define WINUX_DLL  __declspec(dllimport)
        #endif

        #define WINUX_API __stdcall
    #else
        #define WINUX_DLL
        #define WINUX_API
    #endif
#else
    #define WINUX_DLL
    #define WINUX_API
#endif

#define WINUX_FUNC_DECL(ret) WINUX_DLL ret WINUX_API
#define WINUX_FUNC_IMPL(ret) ret WINUX_API

#ifndef countof
    #define countof(arr) ( sizeof(arr) / sizeof(arr[0]) )
#endif

#if !defined($T)
    #if defined(_UNICODE) || defined(UNICODE)
        #define $T(__x) L##__x
    #else
        #define $T(__x) __x
    #endif
#endif

#ifndef interface
#define interface struct
#endif

// 禁止类的对象赋值/拷贝构造
// DISABLE_OBJECT_COPY
#define DISABLE_OBJECT_COPY( clsname ) private:\
clsname( clsname const & ) = delete;\
clsname & operator = ( clsname const & ) = delete;

// 如果指针非NULL
// IF_PTR
#define IF_PTR(ptr) if ( (ptr) != NULL ) (ptr)
// ASSIGN_PTR
#define ASSIGN_PTR(ptr) if ( (ptr) != NULL ) (*(ptr))

// 不使用一个变量
#ifndef UNUSED
#define UNUSED(s)
#endif

// 给类添加一个属性和相关的数据成员，自动添加get/set方法
#define DEFINE_ATTR_MEMBER( ty, name, memname ) \
public:\
    ty const & get##name() const { return this->memname; }\
    void set##name( ty const & v ) { this->memname = v; }\
private:\
    ty memname;

// 给类添加一个只读属性和相关的数据成员，自动添加get方法
#define DEFINE_ATTR_MEMBER_READONLY( ty, name, memname ) \
public:\
    ty const & get##name() const { return this->memname; }\
private:\
    ty memname;

// 给类添加一个属性和相关的数据成员，自动添加get/set方法
#define DEFINE_ATTR_MEMBER_VAL( ty, name, memname ) \
public:\
    ty get##name() const { return this->memname; }\
    void set##name( ty v ) { this->memname = v; }\
private:\
    ty memname;

// 给类添加一个只读属性和相关的数据成员，自动添加get方法
#define DEFINE_ATTR_MEMBER_VAL_READONLY( ty, name, memname ) \
public:\
    ty get##name() const { return this->memname; }\
private:\
    ty memname;

// 给类添加一个属性的get/set方法
#define DEFINE_ATTR_METHOD( ty, name, memname ) \
public:\
    ty const & get##name() const { return this->memname; }\
    void set##name( ty const & v ) { this->memname = v; }

// 给类添加一个属性的get方法
#define DEFINE_ATTR_METHOD_READONLY( ty, name, memname ) \
public:\
    ty const & get##name() const { return this->memname; }

// 给类添加一个属性的get/set方法
#define DEFINE_ATTR_METHOD_VAL( ty, name, memname ) \
public:\
    ty get##name() const { return this->memname; }\
    void set##name( ty v ) { this->memname = v; }

// 给类添加一个属性的get方法
#define DEFINE_ATTR_METHOD_VAL_READONLY( ty, name, memname ) \
public:\
    ty get##name() const { return this->memname; }

// 给类添加一个属性，自动添加get/set方法，需提供实现
#define DEFINE_ATTR( ty, name, getcode, setcode ) \
public:\
    ty const & get##name() const { getcode; }\
    void set##name( ty const & _VAL_ ) { setcode; }

// 给类添加一个只读属性，自动添加get方法，需提供实现
#define DEFINE_ATTR_READONLY( ty, name, getcode ) \
public:\
    ty const & get##name() const { getcode; }

// 给类添加一个属性，自动添加get/set方法，需提供实现
#define DEFINE_ATTR_VAL( ty, name, getcode, setcode ) \
public:\
    ty get##name() const { getcode; }\
    void set##name( ty _VAL_ ) { setcode; }

// 给类添加一个只读属性，自动添加get方法，需提供实现
#define DEFINE_ATTR_VAL_READONLY( ty, name, getcode ) \
public:\
    ty get##name() const { getcode; }

// 给类定义一个NewInstance静态方法
#define DEFINE_FUNC_NEWINSTANCE( cls, ret, paramtypes, params ) \
inline static ret * NewInstance##paramtypes \
{ \
    return new cls##params;\
}

// 给类定义一个自定义事件相关的成员和函数，默认实现
#define DEFINE_CUSTOM_EVENT(evtname, paramtypes, calledparams) \
public: \
    using evtname##HandlerFunction = std::function< void paramtypes >; \
    void on##evtname##Handler( evtname##HandlerFunction handler ) \
    { \
        this->_##evtname##Handler = handler; \
    } \
protected: \
    evtname##HandlerFunction _##evtname##Handler; \
public: \
    virtual void on##evtname paramtypes \
    { \
        if ( this->_##evtname##Handler ) this->_##evtname##Handler calledparams; \
    }

// 给类定义一个自定义事件相关的成员和函数，带返回值并自己实现
#define DEFINE_CUSTOM_EVENT_RETURN_EX(ret, evtname, paramtypes) \
public: \
    using evtname##HandlerFunction = std::function< ret paramtypes >; \
    void on##evtname##Handler( evtname##HandlerFunction handler ) \
    { \
        this->_##evtname##Handler = handler; \
    } \
protected: \
    evtname##HandlerFunction _##evtname##Handler; \
public: \
    virtual ret on##evtname paramtypes


// 判断GCC版本大于给定版本
#define GCC_VERSION_GREAT_THAN(Major,Minor,Patchlevel) \
( __GNUC__ > Major || ( __GNUC__ == Major && ( __GNUC_MINOR__ > Minor || ( __GNUC_MINOR__ == Minor && __GNUC_PATCHLEVEL__ > Patchlevel ) ) ) )

// WINUX基本类型 -----------------------------------------------------------------------------------
typedef int int32;
typedef unsigned int uint, uint32;
typedef unsigned long ulong;
typedef short int16;
typedef unsigned short ushort, uint16;
typedef char int8;
typedef unsigned char uint8;

typedef char16_t char16;
typedef char32_t char32;

typedef intptr_t offset_t, ssize_t;
typedef size_t usize_t;

#if defined(CL_VC)
typedef wchar_t wchar;
typedef unsigned __int64 uint64;
typedef unsigned __int64 ulonglong;
typedef __int64 int64;
typedef __int64 longlong;
#else
typedef wchar_t wchar;
typedef unsigned long long uint64;
typedef unsigned long long ulonglong;
typedef long long int64;
typedef long long longlong;
#endif

#if defined(_UNICODE) || defined(UNICODE)
typedef wchar tchar;
#else
typedef char tchar;
#endif

#ifndef byte
typedef unsigned char byte;
#endif

class Mixed;
// STL wrappers
template < typename _ChTy >
using XString = std::basic_string< _ChTy, std::char_traits<_ChTy>, std::allocator<_ChTy> >;

typedef XString<char> AnsiString, Utf8String;
typedef XString<wchar> UnicodeString;
typedef XString<char16> UnicodeString16, Utf16String;
typedef XString<char32> UnicodeString32, Utf32String;
typedef XString<tchar> String;

template < typename _ChTy >
using XStringArray = std::vector< XString<_ChTy> >;

typedef XStringArray<char> AnsiStringArray, Utf8StringArray;
typedef XStringArray<wchar> UnicodeStringArray;
typedef XStringArray<char16> UnicodeString16Array;
typedef XStringArray<char32> UnicodeString32Array;
typedef XStringArray<char16> Utf16StringArray;
typedef XStringArray<char32> Utf32StringArray;
typedef XStringArray<tchar> StringArray;

typedef std::map<String, String> StringStringMap;
typedef std::pair<String, String> StringStringPair;

typedef std::vector<Mixed> MixedArray;
typedef std::map<String, Mixed> StringMixedMap;
typedef std::pair<String, Mixed> StringMixedPair;
//typedef std::map<Mixed, Mixed> MixedMixedMap;
//typedef std::pair<Mixed, Mixed> MixedMixedPair;

// WINUX常用常量 ---------------------------------------------------------------------------
/** \brief 非位置，值为-1。 */
static constexpr size_t const npos = static_cast<size_t>(-1);
/** \brief Mixed(MT_NULL)常量对象 */
extern WINUX_DLL Mixed const mxNull;
// 字符串相关字面常量
#include "literal_str.inl"

// 模板元编程支持 ---------------------------------------------------------------------------
/** \brief 字符类型规范 */
template < typename _ChTy > struct CharSpec {};
template <> struct CharSpec<char> { using Type = char; };
template <> struct CharSpec<wchar> { using Type = wchar; };
template <> struct CharSpec<char16> { using Type = char16; };
template <> struct CharSpec<char32> { using Type = char32; };

/** \brief 字符类型约束 */
template < typename _ChTy >
using CharTypeConstrain = typename CharSpec< typename std::decay<_ChTy>::type >::Type;

/** \brief 二进制数。编译时计算，0开头(基于8进制) */
template < uint64 n > struct Bin0
{
    enum : uint64 { val = ( Bin0< n / 8 >::val << 1 ) + n % 8 };
};
template <> struct Bin0<0>
{
    enum : uint64 { val = 0 };
};

// 二进制数 macro包装
#define BinVal(x) winux::Bin0<0##x>::val

/** \brief 引用参数包装 */
template < typename _Ty >
class RefParam
{
    _Ty & _r;
public:
    typedef _Ty ParamType;
    typedef _Ty & ParamTypeRef;

    explicit RefParam( ParamTypeRef r ) : _r(r) { }
    operator ParamTypeRef () { return _r; }
};

/** \brief 向模板参数传递引用型参数 */
template < typename _Ty >
inline RefParam<_Ty> Ref( _Ty & r )
{
    return RefParam<_Ty>(r);
}

/** \brief Tuple参数序列 */
template < size_t... _Index >
struct IndexSequence { };

// 构建一个IndexSequence< 0, 1, 2, ..., _Num - 1 >
template < size_t _Num, typename _IdxSeq = IndexSequence<> >
struct MakeIndexSequence;

template < size_t _Num, size_t... _Index >
struct MakeIndexSequence< _Num, IndexSequence<_Index...> > : MakeIndexSequence< _Num - 1, IndexSequence< _Index..., sizeof...(_Index) > > { };

template < size_t... _Index >
struct MakeIndexSequence< 0, IndexSequence<_Index...> >
{
    using Type = IndexSequence<_Index...>;
};

/** \brief 函数特征 */
#include "func_traits.inl"

/** \brief Runable模板 */
#include "func_runable.inl"

/** \brief Invoker模板 */
#include "func_invoker.inl"

/** \brief 函数包装,用来将不同调用约定的函数统一包装成默认约定 */
template < typename _PfnType, _PfnType pfn >
struct FuncWrapper
{
    template < typename... _ArgType >
    static typename FuncTraits<_PfnType>::ReturnType func( _ArgType&& ...arg )
    {
        return (*pfn)( std::forward<_ArgType>(arg)... );
    }
};

/** \brief MAP赋值器 */
template < typename _KTy, typename _VTy, typename _Pr, typename _Alloc >
class MapAssigner
{
public:
    typedef std::map< _KTy, _VTy, _Pr, _Alloc > MapType;

    MapAssigner( MapType * m ) : _m(m) { }
    MapAssigner & operator () ( _KTy const & k, _VTy const & v )
    {
        (*_m)[k] = v;
        return *this;
    }
    operator MapType & () { return *_m; }

private:
    MapType * _m;
};

/** \brief Array赋值器 */
template < typename _Ty, typename _Alloc >
class ArrayAssigner
{
public:
    typedef std::vector< _Ty, _Alloc > VectorType;

    ArrayAssigner( VectorType * a ) : _a(a) { }
    ArrayAssigner & operator () ( _Ty const & v )
    {
        _a->push_back(v);
        return *this;
    }
    operator VectorType & () { return *_a; }

private:
    VectorType * _a;
};

/** \brief 给容器赋值 */
template < typename _KTy, typename _VTy, typename _Pr, typename _Alloc >
inline MapAssigner< _KTy, _VTy, _Pr, _Alloc > Assign( std::map< _KTy, _VTy, _Pr, _Alloc > * m )
{
    return MapAssigner< _KTy, _VTy, _Pr, _Alloc >(m);
}

/** \brief 给容器赋值 */
template < typename _Ty, typename _Alloc >
inline ArrayAssigner<_Ty, _Alloc> Assign( std::vector<_Ty, _Alloc> * a )
{
    return ArrayAssigner<_Ty, _Alloc>(a);
}

/** \brief 成员隐藏（By pointer） */
template < typename _TargetCls >
class Members
{
public:
    Members() : _p(nullptr)
    {
        this->_create();
    }

    template < typename... _ArgType >
    Members( _ArgType&&... arg ) : _p(nullptr)
    {
        this->_create( std::forward<_ArgType>(arg)... );
    }

    ~Members()
    {
        this->_destroy();
    }

    Members( Members const & other ) : _p(nullptr)
    {
        this->_create();
        *_p = *other._p;
    }
    /** \brief 拷贝赋值，必须保证create()已经调用 */
    Members & operator = ( Members const & other )
    {
        if ( &other != this )
        {
            *_p = *other._p;
        }
        return *this;
    }

#ifndef MOVE_SEMANTICS_DISABLED
    Members( Members && other ) : _p(nullptr)
    {
        _p = other._p;
        other._p = nullptr;
    }
    /** \brief 移动赋值，必须保证create()已经调用 */
    Members & operator = ( Members && other )
    {
        if ( &other != this )
        {
            this->_destroy();
            _p = other._p;
            other._p = nullptr;
        }
        return *this;
    }
#endif

    _TargetCls * get()
    {
        return _p;
    }
    _TargetCls * get() const
    {
        return _p;
    }

    _TargetCls * operator -> ()
    {
        return _p;
    }
    _TargetCls const * operator -> () const
    {
        return _p;
    }

    operator _TargetCls & ()
    {
        return *_p;
    }
    operator _TargetCls const & () const
    {
        return *_p;
    }

    operator bool() const
    {
        return _p != nullptr;
    }

private:
    /** \brief 必须在使用者类的析构函数里最后一个调用 */
    void _destroy()
    {
        if ( _p )
        {
            delete (_TargetCls *)_p;
            _p = nullptr;
        }
    }

    /** \brief 必须在使用者类的构造函数里第一个调用 */
    template < typename... _ArgType >
    void _create( _ArgType&&... arg )
    {
        this->_destroy();
        _p = new _TargetCls( std::forward<_ArgType>(arg)... );
    }

    _TargetCls * _p;
};

/** \brief Plain成员隐藏（By plain block） */
template < typename _TargetCls, size_t _MembersSize >
class PlainMembers
{
public:
    PlainMembers()
    {
        this->_create<sizeof(_TargetCls)>();
    }

    template < typename... _ArgType >
    PlainMembers( _ArgType&&... arg )
    {
        this->_create<sizeof(_TargetCls)>( std::forward<_ArgType>(arg)... );
    }

    ~PlainMembers()
    {
        this->_destroy();
    }

    PlainMembers( PlainMembers const & other )
    {
        this->_create<sizeof(_TargetCls)>();
        *this->get() = *other.get();
    }
    /** \brief 拷贝赋值，必须保证create()已经调用 */
    PlainMembers & operator = ( PlainMembers const & other )
    {
        if ( &other != this )
        {
            *this->get() = *other.get();
        }
        return *this;
    }

#ifndef MOVE_SEMANTICS_DISABLED
    PlainMembers( PlainMembers && other )
    {
        this->_create<sizeof(_TargetCls)>();
        *this->get() = std::move( *other.get() );
    }
    /** \brief 移动赋值，必须保证create()已经调用 */
    PlainMembers & operator = ( PlainMembers && other )
    {
        if ( &other != this )
        {
            *this->get() = std::move( *other.get() );
        }
        return *this;
    }
#endif

    _TargetCls * get()
    {
        return reinterpret_cast<_TargetCls*>((char*)_block);
    }
    _TargetCls * get() const
    {
        return reinterpret_cast<_TargetCls*>((char*)_block);
    }

    _TargetCls * operator -> ()
    {
        return this->get();
    }
    _TargetCls const * operator -> () const
    {
        return this->get();
    }

    operator _TargetCls & ()
    {
        return *this->get();
    }
    operator _TargetCls const & () const
    {
        return *this->get();
    }

    operator bool() const
    {
        return _MembersSize >= sizeof(_TargetCls);
    }

private:
    /** \brief 必须在使用者类的析构函数里最后一个调用 */
    void _destroy()
    {
        this->get()->~_TargetCls();
    }

    /** \brief 必须在使用者类的构造函数里第一个调用 */
    template < size_t _TargetClsSize, typename... _ArgType >
    void _create( _ArgType&&... arg )
    {
        static_assert( !( _TargetClsSize > _MembersSize ), "Error: sizeof(_TargetCls) > _MembersSize, _MembersSize is too small" );
        new(_block) _TargetCls( std::forward<_ArgType>(arg)... );
    }

    char _block[_MembersSize];
};

/** \brief 静态New()支持 */
template < typename _Ty >
class EnableStaticNew
{
public:
    template < typename... _ArgType >
    static _Ty * New( _ArgType&&... arg )
    {
        return new _Ty( std::forward<_ArgType>(arg)... );
    }
};

#define FRIEND_ENABLE_STATIC_NEW \
    template < typename _Ty0 > \
    friend class winux::EnableStaticNew


// 一些函数模板和函数 ------------------------------------------------------------------------
/** \brief 计算字符串长度 */
template < typename _ChTy, typename = CharTypeConstrain<_ChTy> >
inline static size_t StrLen( _ChTy const * str )
{
    size_t len = 0;
    while ( str[len] ) len++;
    return len;
}

/** \brief 反转字节序。`_Ty`必须是基本数字类型 */
template < typename _Ty >
inline static _Ty InvertByteOrder( _Ty v )
{
    _Ty v2 = 0;
    for ( int i = 0; i < sizeof(_Ty); ++i ) v2 |= ( ( v >> i * 8 ) & 0xFFU ) << ( sizeof(_Ty) - 1 - i ) * 8;
    return v2;
}

/** \brief 反转字节序。`_Ty`必须是基本数字类型 */
template < typename _Ty >
inline static void InvertByteOrderArray( _Ty * p, size_t count )
{
    for ( size_t k = 0; k < count; k++ )
    {
        p[k] = InvertByteOrder(p[k]);
    }
}

/** \brief 反转字节序 */
WINUX_FUNC_DECL(void) InvertByteOrder( void * buf, size_t size );

/** \brief 判断编译环境是否为小端序 */
inline static bool IsLittleEndian()
{
    constexpr winux::uint16 judgeHostOrder = 0x0102;
    return *(winux::byte*)&judgeHostOrder == 0x02;
}

/** \brief 判断编译环境是否为大端序 */
inline static bool IsBigEndian()
{
    constexpr winux::uint16 judgeHostOrder = 0x0102;
    return *(winux::byte*)&judgeHostOrder == 0x01;
}

/** \brief 内存比较 */
WINUX_FUNC_DECL(int) MemoryCompare( void const * buf1, size_t n1, void const * buf2, size_t n2 );

/** \brief 内存比较（不区分大小写） */
WINUX_FUNC_DECL(int) MemoryCompareI( void const * buf1, size_t n1, void const * buf2, size_t n2 );

/** \brief 字符串比较 */
template < typename _ChTy >
inline static int StringCompare( XString<_ChTy> const & str1, XString<_ChTy> const & str2 )
{
    size_t n = str1.length() < str2.length() ? str1.length() : str2.length();
    for ( size_t i = 0; i < n; ++i )
    {
        int diff = str1[i] - str2[i];
        if ( diff != 0 ) return diff;
    }
    return (int)( str1.length() - str2.length() );
}

/** \brief 字符串（大小写无关）比较 */
template < typename _ChTy >
inline static int StringCompareI( XString<_ChTy> const & str1, XString<_ChTy> const & str2 )
{
    size_t n = str1.length() < str2.length() ? str1.length() : str2.length();
    for ( size_t i = 0; i < n; ++i )
    {
        int diff = tolower(str1[i]) - tolower(str2[i]);
        if ( diff != 0 ) return diff;
    }
    return (int)( str1.length() - str2.length() );
}

/** \brief array less */
template < typename _Ty >
inline static bool ArrayLess( _Ty const * arr1, size_t count1, _Ty const * arr2, size_t count2 )
{
    size_t n = count1 < count2 ? count1 : count2;
    size_t i;
    for ( i = 0; i < n; ++i )
    {
        if ( arr1[i] != arr2[i] ) break;
    }
    if ( i < n )
    {
        return arr1[i] < arr2[i];
    }
    else // i == n
    {
        return count1 < count2;
    }
}

/** \brief array greater */
template < typename _Ty >
inline static bool ArrayGreater( _Ty const * arr1, size_t count1, _Ty const * arr2, size_t count2 )
{
    size_t n = count1 < count2 ? count1 : count2;
    size_t i;
    for ( i = 0; i < n; ++i )
    {
        if ( arr1[i] != arr2[i] ) break;
    }
    if ( i < n )
    {
        return arr1[i] > arr2[i];
    }
    else // i == n
    {
        return count1 > count2;
    }
}

/** \brief array equal */
template < typename _Ty >
inline static bool ArrayEqual( _Ty const * arr1, size_t count1, _Ty const * arr2, size_t count2 )
{
    size_t n = count1 < count2 ? count1 : count2;
    for ( size_t i = 0; i < n; ++i )
    {
        if ( arr1[i] != arr2[i] ) return false;
    }
    return count1 == count2;
}

/** \brief array less */
template < typename _Ty >
inline static bool ArrayLess( std::vector<_Ty> const & arr1, std::vector<_Ty> const & arr2 )
{
    auto p1 = arr1.empty() ? nullptr : &arr1.front();
    auto p2 = arr2.empty() ? nullptr : &arr2.front();
    return ArrayLess( p1, arr1.size(), p2, arr2.size() );
}

/** \brief array less */
template < typename _Ty >
inline static bool ArrayGreater( std::vector<_Ty> const & arr1, std::vector<_Ty> const & arr2 )
{
    auto p1 = arr1.empty() ? nullptr : &arr1.front();
    auto p2 = arr2.empty() ? nullptr : &arr2.front();
    return ArrayGreater( p1, arr1.size(), p2, arr2.size() );
}

/** \brief array equal */
template < typename _Ty >
inline static bool ArrayEqual( std::vector<_Ty> const & arr1, std::vector<_Ty> const & arr2 )
{
    auto p1 = arr1.empty() ? nullptr : &arr1.front();
    auto p2 = arr2.empty() ? nullptr : &arr2.front();
    return ArrayEqual( p1, arr1.size(), p2, arr2.size() );
}

class Collection;

/** \brief collection less */
WINUX_FUNC_DECL(bool) CollectionLess( Collection const & coll1, Collection const & coll2 );

/** \brief collection greater */
WINUX_FUNC_DECL(bool) CollectionGreater( Collection const & coll1, Collection const & coll2 );

/** \brief collection equal */
WINUX_FUNC_DECL(bool) CollectionEqual( Collection const & coll1, Collection const & coll2 );

/** \brief 检测map中是否有该键的值 */
template < typename _MAP, typename _KEY >
inline bool isset( _MAP const & m, _KEY const & k )
{
    return m.find(k) != m.end();
}

/** \brief 将C数组转换成vector */
template < typename _Ty >
inline std::vector<_Ty> ToArray( _Ty * arr, uint count )
{
    return std::vector<_Ty>( arr, arr + count );
}

template < typename _Ty, uint _Count >
inline std::vector<_Ty> ToArray( _Ty (&arr)[_Count] )
{
    return std::vector<_Ty>( arr, arr + _Count );
}

/** \brief 调用一个返回void的函数或函数对象，返回一个数字
 *
 *  通常是为了在初始化语句中方便调用返回void的函数 */
template < typename _Fx, typename... _ArgType >
inline int VoidReturnInt( _Fx fn, _ArgType&& ...arg )
{
    fn( std::forward<_ArgType>(arg)... );
    return 1;
}

// ----------------------------------------------------------------------------------------

/** \brief 错误类 */
class Error : public std::exception
{
public:
    enum
    {
        // 错误号详见errno.h: EXXXX
    };

    Error() throw() : _errType(0) { }
    Error( int errType, AnsiString const & errStr ) throw() : _errType(errType), _errStr(errStr) { }
    virtual ~Error() throw() { }
    virtual int getErrType() const throw() { return _errType; }
    virtual char const * what() const throw() { return _errStr.c_str(); }

private:
    int _errType; // 错误类型（错误号）
    AnsiString _errStr; // 错误内容字符串
};

/** \brief 判断一个字符串值是否在一个字符串数组里,默认大小写敏感 */
WINUX_FUNC_DECL(bool) ValueIsInArray( StringArray const & arr, String const & val, bool caseInsensitive = false );

/** \brief 随机数,随机产生n1~n2的数字. 包括n1,n2本身 */
WINUX_FUNC_DECL(int) Random( int n1, int n2 );


// 类型解析功能 -------------------------------------------------------------------------
/** \brief parse bool */
WINUX_FUNC_DECL(bool) ParseBool( AnsiString const & str, bool * boolVal );
/** \brief parse bool */
WINUX_FUNC_DECL(bool) ParseBool( UnicodeString const & str, bool * boolVal );
/** \brief parse int */
WINUX_FUNC_DECL(bool) ParseInt( AnsiString const & str, int * iVal );
/** \brief parse int */
WINUX_FUNC_DECL(bool) ParseInt( UnicodeString const & str, int * iVal );
/** \brief parse uint */
WINUX_FUNC_DECL(bool) ParseUInt( AnsiString const & str, uint * uiVal );
/** \brief parse uint */
WINUX_FUNC_DECL(bool) ParseUInt( UnicodeString const & str, uint * uiVal );
/** \brief parse long */
WINUX_FUNC_DECL(bool) ParseLong( AnsiString const & str, long * lVal );
/** \brief parse long */
WINUX_FUNC_DECL(bool) ParseLong( UnicodeString const & str, long * lVal );
/** \brief parse ulong */
WINUX_FUNC_DECL(bool) ParseULong( AnsiString const & str, ulong * ulVal );
/** \brief parse ulong */
WINUX_FUNC_DECL(bool) ParseULong( UnicodeString const & str, ulong * ulVal );
/** \brief parse int64 */
WINUX_FUNC_DECL(bool) ParseInt64( AnsiString const & str, int64 * i64Val );
/** \brief parse int64 */
WINUX_FUNC_DECL(bool) ParseInt64( UnicodeString const & str, int64 * i64Val );
/** \brief parse uint64 */
WINUX_FUNC_DECL(bool) ParseUInt64( AnsiString const & str, uint64 * ui64Val );
/** \brief parse uint64 */
WINUX_FUNC_DECL(bool) ParseUInt64( UnicodeString const & str, uint64 * ui64Val );
/** \brief parse float */
WINUX_FUNC_DECL(bool) ParseFloat( AnsiString const & str, float * fltVal );
/** \brief parse float */
WINUX_FUNC_DECL(bool) ParseFloat( UnicodeString const & str, float * fltVal );
/** \brief parse double */
WINUX_FUNC_DECL(bool) ParseDouble( AnsiString const & str, double * dblVal );
/** \brief parse double */
WINUX_FUNC_DECL(bool) ParseDouble( UnicodeString const & str, double * dblVal );

// -------------------------------------------------------------------------------
class GrowBuffer;

/** \brief 缓冲区，表示内存中一块二进制数据(利用malloc/realloc进行内存分配) */
class WINUX_DLL Buffer
{
public:
    static void * Alloc( size_t size );
    static void * Realloc( void * p, size_t newSize );
    static void Free( void * p );

public:
    /** \brief 默认构造函数 */
    Buffer();

    /** \brief 构造函数1 从一个缓冲区创建Buffer，可以指定是否为窥视模式
     *
     *  处于窥视模式时将不负责管理资源的释放 */
    Buffer( void const * buf, size_t size, bool isPeek = false );

    /** \brief 构造函数2 从一个XString创建Buffer，可以指定是否为窥视模式
     *
     *  处于窥视模式时将不负责管理资源的释放 */
    template < typename _ChTy >
    Buffer( XString<_ChTy> const & data, bool isPeek = false )
    {
        this->_copyConstruct( data.c_str(), data.size() * sizeof(_ChTy), isPeek );
    }

    /** \brief 构造函数3 从字符指针创建Buffer，可以指定是否为窥视模式 */
    template < typename _ChTy, typename = CharTypeConstrain<_ChTy> >
    Buffer( _ChTy const * str, size_t len = npos, bool isPeek = false )
    {
        this->_copyConstruct( str, ( len == npos ? StrLen<_ChTy>(str) * sizeof(_ChTy) : len * sizeof(_ChTy) ), isPeek );
    }

    /** \brief 析构函数 */
    virtual ~Buffer();

    /** \brief 拷贝构造函数（只会拷贝数据大小长度的空间，而不是容量大小） */
    Buffer( Buffer const & other );

    /** \brief 拷贝赋值函数（只会拷贝数据大小长度的空间，而不是容量大小） */
    Buffer & operator = ( Buffer const & other );

#ifndef MOVE_SEMANTICS_DISABLED
    /** \brief 移动构造函数 */
    Buffer( Buffer && other );
    /** \brief 移动赋值操作 */
    Buffer & operator = ( Buffer && other );
    /** \brief 移动构造函数1 */
    Buffer( GrowBuffer && other );
    /** \brief 移动赋值操作1 */
    Buffer & operator = ( GrowBuffer && other );
#endif

    /** \brief 设置缓冲区，当isPeek为false时拷贝数据缓冲区
     *
     *  \param buf 缓冲区指针。如果为nullptr则分配空间后不进行数据拷贝
     *  \param size 数据大小
     *  \param capacity 缓冲区容量
     *  \param isPeek 是否窥探模式。默认false拷贝数据 */
    void setBuf( void const * buf, size_t size, size_t capacity, bool isPeek );

    /** \brief 设置缓冲区，当isPeek为false时拷贝数据缓冲区
     *
     *  \param buf 缓冲区指针。如果为nullptr则分配空间后不进行数据拷贝
     *  \param size 数据大小
     *  \param isPeek 是否窥探模式。默认false拷贝数据 */
    void setBuf( void const * buf, size_t size, bool isPeek ) { this->setBuf( buf, size, size, isPeek ); }

    /** \brief 分配容量大小，当setDataSize为true时设置数据长度 */
    void alloc( size_t capacity, bool setDataSize = true );

    /** \brief 重新调整容量的大小，保留数据内容
     *
     *  \attention 如果新的容量小于数据大小，多余的数据会被丢弃 */
    void realloc( size_t newCapacity );

    /** \brief 把窥探模式变为拷贝模式，如果copyCapacity为true时连容量也一起拷贝，否则只拷贝数据。
     *
     *  \return 如果本来就不是窥探模式则直接返回false，由窥探模式切到拷贝则返回true */
    bool peekCopy( bool copyCapacity = false );

    /** \brief 使Buffer对象不再管理内存资源 */
    void * detachBuf( size_t * size = nullptr );

    /** \brief 释放缓冲区 */
    void free();

    /** \brief 暴露缓冲区指针 */
    void * getBuf() const { return _buf; }

    /** \brief 暴露缓冲区指针 */
    template < typename _Ty >
    _Ty * getBuf() const { return reinterpret_cast<_Ty *>(_buf); }

    /** \brief 暴露缓冲区指针 */
    void * get() const { return _buf; }

    /** \brief 暴露缓冲区指针 */
    template < typename _Ty >
    _Ty * get() const { return reinterpret_cast<_Ty *>(_buf); }

    /** \brief 暴露缓冲区指定位置指针 */
    winux::byte * getAt( ssize_t i ) const { return reinterpret_cast<winux::byte *>(_buf) + i; }

    /** \brief 暴露缓冲区指定位置指针 */
    template < typename _Ty >
    _Ty * getAt( ssize_t i ) const { return reinterpret_cast<_Ty *>(_buf) + i; }

    /** \brief 获取指定索引的字节 */
    winux::byte & operator [] ( size_t i ) { return reinterpret_cast<winux::byte *>(_buf)[i]; }
    /** \brief 获取指定索引的字节 */
    winux::byte const & operator [] ( size_t i ) const { return reinterpret_cast<winux::byte const *>(_buf)[i]; }

    // Iterator 相关
    winux::byte * begin() { return reinterpret_cast<winux::byte *>(_buf); }
    winux::byte * end() { return reinterpret_cast<winux::byte *>(_buf) + _dataSize; }
    winux::byte const * begin() const { return reinterpret_cast<winux::byte *>(_buf); }
    winux::byte const * end() const { return reinterpret_cast<winux::byte *>(_buf) + _dataSize; }

    /** \brief 获取数据大小 */
    size_t getSize() const { return _dataSize; }

    /** \brief 获取数据字节大小 */
    size_t size() const { return _dataSize; }

    /** \brief 获取大小 */
    template < typename _Ty >
    size_t size() const { return _dataSize / sizeof(_Ty); }

    /** \brief 获取大小 */
    template < typename _Ty >
    size_t count() const { return _dataSize / sizeof(_Ty); }

    /** \brief 设置数据大小，不能超过容量大小（不建议外部调用） */
    void _setSize( size_t dataSize ) { _dataSize = ( dataSize > _capacity ? _capacity : dataSize ); }

    /** \brief 获取容量大小 */
    size_t getCapacity() const { return _capacity; }

    /** \brief 获取容量大小 */
    size_t capacity() const { return _capacity; }

    /** \brief 是否为窥探模式 */
    bool isPeek() const { return _isPeek; }

    /** \brief 判断是否为一个有效的Buffer */
    operator bool() const { return _buf != NULL; }

    /** \brief 转换到字符串 */
    template < typename _ChTy >
    XString<_ChTy> toString() const { return XString<_ChTy>( (_ChTy*)_buf, _dataSize / sizeof(_ChTy) ); }

    /** \brief 转换到AnsiString */
    AnsiString toAnsi() const { return this->toString<AnsiString::value_type>(); }

    /** \brief 转换到UnicodeString */
    UnicodeString toUnicode() const { return this->toString<UnicodeString::value_type>(); }

    // 比较操作符 --------------------------------------------------------------------------
    bool operator == ( Buffer const & other ) const { return MemoryCompare( this->getBuf(), this->getSize(), other.getBuf(), other.getSize() ) == 0; }
    bool operator < ( Buffer const & other ) const { return MemoryCompare( this->getBuf(), this->getSize(), other.getBuf(), other.getSize() ) < 0; }
    bool operator > ( Buffer const & other ) const { return MemoryCompare( this->getBuf(), this->getSize(), other.getBuf(), other.getSize() ) > 0; }
    bool operator != ( Buffer const & other ) const { return !this->operator == (other); }
    bool operator >= ( Buffer const & other ) const { return !this->operator < (other); }
    bool operator <= ( Buffer const & other ) const { return !this->operator > (other); }

protected:
    // 零初始化
    void _zeroInit();
    // 拷贝构造（不会判断自身原有资源情况）
    void _copyConstruct( void const * buf, size_t size, bool isPeek );

    void * _buf; //!< 缓冲区
    size_t _dataSize; //!< 数据的大小
    size_t _capacity; //!< 容量
    bool _isPeek; //!< 是否为窥视模式

    friend class GrowBuffer;
};

/** \brief 数据包，用来表示一些POD结构体用于网络通信。通常这类结构体实际大小 > sizeof(这类结构体) */
template < typename _PodType >
class Packet : public Buffer
{
public:
    Packet( size_t size = 0 )
    {
        if ( size > 0 ) this->alloc(size);
    }

    /** \brief 暴露缓冲区指针 */
    _PodType * get() const { return reinterpret_cast<_PodType *>(_buf); }

    _PodType * operator -> () { return reinterpret_cast<_PodType *>(_buf); }
    _PodType const * operator -> () const { return reinterpret_cast<_PodType *>(_buf); }
};

/** \brief 高效的可增长缓冲区，1.33倍冗余量 */
class WINUX_DLL GrowBuffer : public Buffer
{
public:
    /** \brief 构造函数，初始化缓冲区的大小 */
    explicit GrowBuffer( size_t capacity = 0 );
    GrowBuffer( GrowBuffer const & other );
    GrowBuffer & operator = ( GrowBuffer const & other );
    explicit GrowBuffer( Buffer const & other );
    GrowBuffer & operator = ( Buffer const & other );

#ifndef MOVE_SEMANTICS_DISABLED
    /** \brief 移动构造函数 */
    GrowBuffer( GrowBuffer && other );
    /** \brief 移动赋值操作 */
    GrowBuffer & operator = ( GrowBuffer && other );
    /** \brief 移动构造函数1 */
    GrowBuffer( Buffer && other );
    /** \brief 移动赋值操作1 */
    GrowBuffer & operator = ( Buffer && other );
#endif

    /** \brief 添加数据：C语言缓冲区
     *
     *  \param[in] data 数据缓冲区地址。若为nullptr，则只增涨数据空间不复制数据
     *  \param[in] size 数据大小 */
    void append( void const * data, size_t size );

    /** \brief 添加数据：`Buffer`对象 */
    void append( Buffer const & data ) { this->append( (void const *)data.getBuf(), data.getSize() ); }

    /** \brief 添加数据：`XString`对象 */
    template < typename _ChTy >
    void appendString( XString<_ChTy> const & data ) { this->append( (void const *)data.c_str(), data.size() * sizeof(_ChTy) ); }

    /** \brief 添加数据：`C串` */
    template < typename _ChTy, typename = CharTypeConstrain<_ChTy> >
    void appendString( _ChTy const * str, size_t len = npos )
    {
        this->append( (void const *)str, ( len == npos ? StrLen<_ChTy>(str) : len ) * sizeof(_ChTy) );
    }

    /** \brief 添加数据：POD类型变量 */
    template < typename _PodType >
    void appendType( _PodType const & data, size_t size = sizeof(_PodType) ) { this->append( (void const *)&data, size ); }

    /** \brief 添加数据：POD类型数组 */
    template < typename _PodType, size_t _Count >
    void appendType( _PodType const (&data)[_Count] ) { this->append( (void const *)&data, _Count * sizeof(data[0]) ); }

    /** \brief 添加数据：POD类型`std::initializer_list` */
    template < typename _PodType >
    void appendType( std::initializer_list<_PodType> list )
    {
        for ( auto const & e : list )
        {
            this->appendType<_PodType>( e, sizeof(e) );
        }
    }

    /** \brief 擦除数据，自动紧缩 */
    void erase( size_t start, size_t count = (size_t)-1 );

protected:
    friend class Buffer;
};

/** \brief memcmp() */
inline static int BufferCompare( Buffer const & buf1, Buffer const & buf2 )
{
    return MemoryCompare( buf1.getBuf(), buf1.getSize(), buf2.getBuf(), buf2.getSize() );
}

/** \brief memicmp() */
inline static int BufferCompareI( Buffer const & buf1, Buffer const & buf2 )
{
    return MemoryCompareI( buf1.getBuf(), buf1.getSize(), buf2.getBuf(), buf2.getSize() );
}

// 混合体相关 ------------------------------------------------------------------------------
/** \brief 混合体错误 */
class MixedError : public Error
{
public:
    enum
    {
        meNoError,          //!< 没有错误
        meIsNull,           //!< 值是Null因此无法操作
        meCantConverted,    //!< 不能转换到某种类型
        meUnexpectedType,   //!< 意料外的类型，该类型不能执行这个操作
        meOutOfArrayRange,  //!< 数组越界
        meKeyNoExist,       //!< 集合不存在指定键
    };

    MixedError( int errType, AnsiString const & errStr ) throw() : Error( errType, errStr ) { }
};

/** \brief `Mixed`构造数组辅助类 */
struct $a
{
    std::initializer_list<Mixed> _list;
    $a( std::initializer_list<Mixed> && list ) : _list( std::move(list) )
    {
    }
};

/** \brief `Mixed`构造集合辅助类 */
struct $c
{
    std::initializer_list< std::pair< Mixed, Mixed > > _list;
    $c( std::initializer_list< std::pair< Mixed, Mixed > > && list ) : _list( std::move(list) )
    {
    }
};

/** \brief Mixed集合类型的`less`谓词 */
class WINUX_DLL MixedLess
{
public:
    bool operator () ( Mixed const & v1, Mixed const & v2 ) const;
};
typedef std::map< Mixed, Mixed, MixedLess > MixedMixedMap;

/** \brief Mixed集合类型的`less`谓词，大小写无关 */
class WINUX_DLL MixedLessI
{
public:
    bool operator () ( Mixed const & v1, Mixed const & v2 ) const;
};
typedef std::map< Mixed, Mixed, MixedLessI > MixedMixedMapI;

typedef std::pair< Mixed const, Mixed > MixedMixedPair;


/** \brief 集合类 */
class WINUX_DLL Collection
{
public:
    explicit Collection( bool caseInsensitive = false );
    ~Collection();

    Collection( Collection const & other );
    Collection & operator = ( Collection const & other );

    Collection( Collection && other );
    Collection & operator = ( Collection && other );

    /** \brief 构造函数 用map构造Collection */
    template < typename _KTy, typename _VTy, typename _Pr, typename _Alloc >
    explicit Collection( std::map< _KTy, _VTy, _Pr, _Alloc > const & m, bool caseInsensitive = false )
    {
        this->_zeroInit();
        this->assign< _KTy, _VTy, _Pr, _Alloc >( m, caseInsensitive );
    }

    /** \brief 构造函数 用pair[]构造Collection */
    template < typename _KTy, typename _VTy, size_t _Count >
    explicit Collection( std::pair< _KTy, _VTy > (&pairs)[_Count], bool caseInsensitive = false )
    {
        this->_zeroInit();
        this->assign< _KTy, _VTy, _Count >( pairs, caseInsensitive );
    }

    /** \brief 构造函数 用$c构造Collection */
    explicit Collection( $c && coll, bool caseInsensitive = false );

    /** \brief 构造函数 用Mixed构造Collection */
    explicit Collection( Mixed const & coll, bool caseInsensitive = false );

    /** \brief 创建集合 */
    void create( bool caseInsensitive = false );

    /** \brief 销毁集合 */
    void destroy();

    /** \brief 清空集合 */
    void clear();

    /** \brief 获取全部键名，返回键名个数 */
    template < typename _Ty, typename _Alloc >
    size_t getKeys( std::vector< _Ty, _Alloc > * keys ) const
    {
        for ( auto it = this->_pKeysArr->begin(); it != this->_pKeysArr->end(); ++it )
            keys->push_back(*it);
        return keys->size();
    }

    /** \brief 获取映射表，返回键值对个数 */
    template < typename _KTy, typename _VTy, typename _Pr, typename _Alloc >
    size_t getMap( std::map< _KTy, _VTy, _Pr, _Alloc > * m ) const;

    /** \brief 判断容器是否为空 */
    bool isEmpty() const { return this->getCount() == 0; }

    /** \brief 获取Collection元素个数 */
    size_t getCount() const { return this->_pKeysArr != NULL ? this->_pKeysArr->size() : 0; }

    /** \brief 下标操作 */
    Mixed & operator [] ( Mixed const & k );

    /** \brief 下标操作 使兼容字符串指针 */
    template < typename _ChTy >
    Mixed & operator [] ( _ChTy const * k ) { return this->operator [] ( Mixed(k) ); }

    /** \brief at操作 */
    Mixed & at( Mixed const & k );
    /** \brief const at操作 */
    Mixed const & at( Mixed const & k ) const;

    /** \brief 取得指定'Key'的元素，不存在则返回默认值 */
    Mixed const & get( Mixed const & k, Mixed const & defval ) const;

    /** \brief Collection获取'键值对'索引操作 */
    MixedMixedPair & getPair( size_t i );
    /** \brief Collection获取'键值对'索引操作 */
    MixedMixedPair const & getPair( size_t i ) const;

    /** \brief Collection设置'键值对'索引操作 */
    void setPair( size_t i, Mixed const & k, Mixed const & v );

    /** \brief 往Collection添加一个pair */
    void addPair( Mixed const & k, Mixed const & v );

    /** \brief 往Collection添加一个pair */
    void addPair( Mixed const & k, Mixed && v );

    /** \brief 删除一个元素，k代表键名 */
    void del( Mixed const & k );

    /** \brief 判断键名是否存在 */
    bool has( Mixed const & k ) const;

    /** \brief 反转集合内元素顺序 */
    void reverse();

    MixedArray & refKeysArray() { return *this->_pKeysArr; }
    MixedArray const & refKeysArray() const { return *this->_pKeysArr; }

    bool getCaseInsensitive() const { return this->_caseInsensitive; }

    MixedMixedMap & refMap() { return *this->_pMap; }
    MixedMixedMap const & refMap() const { return *this->_pMap; }

    MixedMixedMapI & refMapI() { return *this->_pMapI; }
    MixedMixedMapI const & refMapI() const { return *this->_pMapI; }

    /** \brief 用map给Collection赋值 */
    template < typename _KTy, typename _VTy, typename _Pr, typename _Alloc >
    void assign( std::map< _KTy, _VTy, _Pr, _Alloc > const & m, bool caseInsensitive = false )
    {
        this->_implAssign(
            &Collection::_fxMap<MixedMixedMap, _KTy, _VTy, _Pr, _Alloc>,
            &Collection::_fxMap<MixedMixedMapI, _KTy, _VTy, _Pr, _Alloc>,
            m,
            caseInsensitive
        );
    }

    /** \brief 用pair[]给Collection赋值 */
    template < typename _KTy, typename _VTy, size_t _Count >
    void assign( std::pair< _KTy, _VTy > (&pairs)[_Count], bool caseInsensitive = false )
    {
        this->_implAssign(
            &Collection::_fxPairs<MixedMixedMap, _KTy, _VTy, _Count>,
            &Collection::_fxPairs<MixedMixedMapI, _KTy, _VTy, _Count>,
            pairs,
            caseInsensitive
        );
    }

    /** \brief 用$c给Collection赋值 */
    void assign( $c && coll, bool caseInsensitive = false )
    {
        this->_implAssign(
            &Collection::_fxC<MixedMixedMap>,
            &Collection::_fxC<MixedMixedMapI>,
            std::move(coll),
            caseInsensitive
        );
    }

private:
    // 零初始化
    void _zeroInit()
    {
        memset( this, 0, sizeof(*this) );
    }
    // 拷贝构造（不会判断自身原有资源情况）
    void _copyConstruct( Collection const & other );

    // 给数组加入一个唯一键名
    bool _addUniqueKey( Mixed const & k );

    template < typename _MAP, typename _KTy, typename _VTy, typename _Pr, typename _Alloc >
    void _fxMap( _MAP * pMap, std::map< _KTy, _VTy, _Pr, _Alloc > const & m )
    {
        for ( auto it = m.begin(); it != m.end(); ++it )
        {
            this->_addUniqueKey(it->first);
            (*pMap)[it->first] = it->second;
        }
    }

    template < typename _MAP, typename _KTy, typename _VTy, size_t _Count >
    void _fxPairs( _MAP * pMap, std::pair< _KTy, _VTy > (&pairs)[_Count] )
    {
        for ( size_t i = 0; i < _Count; ++i )
        {
            this->_addUniqueKey(pairs[i].first);
            (*pMap)[pairs[i].first] = pairs[i].second;
        }
    }

    template < typename _MAP >
    void _fxC( _MAP * pMap, $c && coll );

    template < typename _Fx1, typename _Fx2, typename _Ty >
    void _implAssign( _Fx1 fn, _Fx2 fnI, _Ty && m, bool caseInsensitive );

private:
    MixedArray * _pKeysArr; //!< 数组
    union
    {
        MixedMixedMap * _pMap;      //!< 映射表
        MixedMixedMapI * _pMapI;    //!< 映射表大小写无关
    };
    bool _caseInsensitive; //!< `false`表示大小写相关KEY的MAP，`true`表示大小写无关KEY的MAP

    friend class Mixed;
};


/** \brief 输出Mixed类型的字符串表示 */
WINUX_FUNC_DECL(AnsiString const &) TypeStringA( Mixed const & v );
/** \brief 输出Mixed类型的字符串表示 */
WINUX_FUNC_DECL(UnicodeString const &) TypeStringW( Mixed const & v );

/** \brief 混合体，能表示多种类型的值 */
class WINUX_DLL Mixed
{
public:
#ifdef MIXED_REF_NO_EXCEPTION
#define MIXED_REF_TYPE_METHOD( mt, ty, memb, funcname )\
    ty& ref##funcname() { return memb; }\
    ty const& ref##funcname() const { return memb; }
#define MIXED_REF_TYPE_METHOD_OUTER( mt, ty, memb, funcname )\
    template<> inline ty& Mixed::ref<ty>() { return memb; }\
    template<> inline ty const& Mixed::ref<ty>() const { return memb; }
#else
#define MIXED_REF_TYPE_METHOD( mt, ty, memb, funcname )\
    ty& ref##funcname() { if ( this->_type != mt ) throw MixedError( MixedError::meUnexpectedType, "ref"#funcname"(): " + TypeStringA(*this) + " can not be referenced as a "#mt ); return memb; }\
    ty const& ref##funcname() const { if ( this->_type != mt ) throw MixedError( MixedError::meUnexpectedType, "ref"#funcname"(): " + TypeStringA(*this) + " can not be referenced as a "#mt ); return memb; }
#define MIXED_REF_TYPE_METHOD_OUTER( mt, ty, memb, funcname )\
    template<> inline ty& Mixed::ref<ty>() { if ( this->_type != mt ) throw MixedError( MixedError::meUnexpectedType, "ref<"#ty">(): " + TypeStringA(*this) + " can not be referenced as a "#mt ); return memb; }\
    template<> inline ty const& Mixed::ref<ty>() const { if ( this->_type != mt ) throw MixedError( MixedError::meUnexpectedType, "ref<"#ty">(): " + TypeStringA(*this) + " can not be referenced as a "#mt ); return memb; }
#endif

#define MIXED_TYPE_ENUM_ITEM( item, nouse1, nouse2, nouse3 ) item,
#define MIXED_TYPE_ENUM_ITEMSTRINGA( item, nouse1, nouse2, nouse3 ) #item,
#define MIXED_TYPE_ENUM_ITEMSTRINGW( item, nouse1, nouse2, nouse3 ) L ## #item,

#define MIXED_TYPE_LIST(funcmacro) \
    funcmacro( MT_BOOLEAN/**< 布尔型，true 或 false */, bool, _boolVal, Bool ) \
    funcmacro( MT_CHAR/**< 字符型，8位有符号整数 */, char, _chVal, Char ) \
    funcmacro( MT_BYTE/**< 字节型，8位无符号整数 */, byte, _btVal, Byte ) \
    funcmacro( MT_SHORT/**< 短整型，16位有符号整数 */, short, _shVal, Short ) \
    funcmacro( MT_USHORT/**< 短整型，16位无符号整数 */, ushort, _ushVal, UShort ) \
    funcmacro( MT_INT/**< 整型，平台位长有符号整数 */, int, _iVal, Int ) \
    funcmacro( MT_UINT/**< 整型，平台位长无符号整数 */, uint, _uiVal, UInt ) \
    funcmacro( MT_LONG/**< 长整型，32位有符号整数 */, long, _lVal, Long ) \
    funcmacro( MT_ULONG/**< 长整型，32位无符号整数 */, ulong, _ulVal, ULong ) \
    funcmacro( MT_INT64/**< 整型，64位有符号整数 */, int64, _i64Val, Int64 ) \
    funcmacro( MT_UINT64/**< 整型，64位无符号整数 */, uint64, _ui64Val, UInt64 ) \
    funcmacro( MT_FLOAT/**< 单精度浮点型，32位 */, float, _fltVal, Float ) \
    funcmacro( MT_DOUBLE/**< 双精度浮点型，64位 */, double, _dblVal, Double ) \
    funcmacro( MT_ANSI/**< 多字节字符串类型，`char`序列 */, AnsiString, *_pStr, Ansi ) \
    funcmacro( MT_UNICODE/**< 宽字符串类型，`wchar_t`序列 */, UnicodeString, *_pWStr, Unicode ) \
    funcmacro( MT_BINARY/**< 二进制数据类型，字节序列 */, Buffer, *_pBuf, Buffer ) \
    funcmacro( MT_ARRAY/**< 数组类型，容器 */, MixedArray, *_pArr, Array ) \
    funcmacro( MT_COLLECTION/**< Collection类型，容器 */, Collection, *_pColl, Collection )

    /** \brief 混合体类型识别常量 */
    enum MixedType : uint
    {
        MT_NULL/**< null，缺省默认类型 */,
        MIXED_TYPE_LIST(MIXED_TYPE_ENUM_ITEM)
    };

    union
    {
        AnsiString * _pStr;     //!< 多字节字符串，`char`序列
        UnicodeString * _pWStr; //!< 宽字符串，`wchar_t`序列
        Buffer * _pBuf;         //!< 缓冲区，字节序列。利用`Buffer`对象存储的二进制数据
        MixedArray * _pArr;     //!< 数组容器。利用`std::vector&lt;Mixed&gt;`存储的`Mixed`数组
        Collection * _pColl;    //!< 集合容器。利用数组存储key体现次序，然后用Map存储k/v对

        double _dblVal;     //!< 双精度浮点类型
        uint64 _ui64Val;    //!< 无符号64位整数
        int64 _i64Val;      //!< 有符号64位整数
        float _fltVal;      //!< 单精度浮点类型
        ulong _ulVal;       //!< 无符号长整型
        long _lVal;         //!< 有符号长整型
        uint _uiVal;        //!< 无符号整型
        int _iVal;          //!< 有符号整型
        ushort _ushVal;     //!< 无符号短整型
        short _shVal;       //!< 有符号短整型
        byte _btVal;        //!< 无符号字节型
        char _chVal;        //!< 字符型
        bool _boolVal;      //!< 布尔型
    };
    MixedType _type; //!< 当前类型

public:
    Mixed();
    Mixed( std::nullptr_t );
    Mixed( void const * ) = delete; //!< 删除void*的构造函数

    // 基本类型构造函数 -------------------------------------------------------------------------
    Mixed( bool boolVal );
    Mixed( char chVal );
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

    // 字符串构造函数 ---------------------------------------------------------------------------
    Mixed( AnsiString const & str );                //!< 多字节字符串
    Mixed( UnicodeString const & str );             //!< Unicode字符串
    Mixed( AnsiString && str );                //!< 多字节字符串
    Mixed( UnicodeString && str );             //!< Unicode字符串
    Mixed( char const * str, size_t len = npos );   //!< 多字节字符串
    Mixed( wchar const * str, size_t len = npos );  //!< Unicode字符串

    // Buffer构造函数 --------------------------------------------------------------------------
    Mixed( Buffer const & buf );
    Mixed( void const * binaryData, size_t size, bool isPeek = false );
    Mixed( Buffer && buf );
    Mixed( GrowBuffer && buf );

    // Array构造函数 ---------------------------------------------------------------------------
    /** \brief 数组构造函数 */
    Mixed( Mixed * arr, size_t count );

    /** \brief 数组构造函数 */
    template < typename _Ty, typename _Alloc >
    Mixed( std::vector< _Ty, _Alloc > const & arr )
    {
        _zeroInit();
        this->assign< _Ty, _Alloc >(arr);
    }

    /** \brief 数组构造函数 */
    template < typename _Ty, size_t _Count >
    Mixed( _Ty (&arr)[_Count] )
    {
        _zeroInit();
        this->assign< _Ty, _Count >(arr);
    }

    /** \brief 数组构造函数 */
    Mixed( std::initializer_list<Mixed> && list );

    /** \brief 数组构造函数 */
    Mixed( $a && arr );

    // Collection构造函数 ----------------------------------------------------------------------
    /** \brief Collection构造函数 */
    template < typename _KTy, typename _VTy, typename _Pr, typename _Alloc >
    Mixed( std::map< _KTy, _VTy, _Pr, _Alloc > const & m, bool caseInsensitive = false )
    {
        _zeroInit();
        this->assign< _KTy, _VTy, _Pr, _Alloc >( m, caseInsensitive );
    }

    /** \brief Collection构造函数 */
    template < typename _KTy, typename _VTy, size_t _Count >
    Mixed( std::pair< _KTy, _VTy > (&pairs)[_Count], bool caseInsensitive = false )
    {
        _zeroInit();
        this->assign< _KTy, _VTy, _Count >( pairs, caseInsensitive );
    }

    /** \brief Collection构造函数 */
    Mixed( $c && coll, bool caseInsensitive = false );

    /** \brief Collection构造函数 */
    Mixed( Collection const & coll, bool caseInsensitive = false );
 
    // 拷贝和移动 ------------------------------------------------------------------------------
    /** \brief 拷贝构造函数 */
    Mixed( Mixed const & other );
    /** \brief 拷贝赋值操作 */
    Mixed & operator = ( Mixed const & other );

    /** \brief 移动构造函数 */
    Mixed( Mixed && other );
    /** \brief 移动赋值操作 */
    Mixed & operator = ( Mixed && other );

    /** \brief 析构函数 */
    ~Mixed();

    /** \brief 释放相关资源 */
    void free();

    /** \brief 取得类型 */
    MixedType type() const { return this->_type; }

    // 取得相关类型的引用 --------------------------------------------------------------------
    template < typename _ChTy >
    XString<_ChTy> & refString();
    template < typename _ChTy >
    XString<_ChTy> const & refString() const;

    template < typename _Ty >
    _Ty & ref();
    template < typename _Ty >
    _Ty const & ref() const;

    // 生成 Mixed 引用类型方法
    MIXED_TYPE_LIST(MIXED_REF_TYPE_METHOD)

    // 类型转换 ----------------------------------------------------------------------------
    operator bool() const;
    //operator char() const;
    operator byte() const;
    operator short() const;
    operator ushort() const;
    operator int() const;
    operator uint() const;
    operator long() const;
    operator ulong() const;
    operator float() const;
    operator int64() const;
    operator uint64() const;
    operator double() const;
    operator AnsiString() const;
    operator UnicodeString() const;
    operator Buffer() const;
    operator MixedArray() const;
    operator Collection() const;

    bool toBool() const { return this->operator bool(); }
    char toChar() const;// { return this->operator char(); }
    byte toByte() const { return this->operator winux::byte(); }
    short toShort() const { return this->operator short(); }
    ushort toUShort() const { return this->operator winux::ushort(); }
    int toInt() const { return this->operator int(); }
    uint toUInt() const { return this->operator winux::uint(); }
    long toLong() const { return this->operator long(); }
    ulong toULong() const { return this->operator winux::ulong(); }
    float toFloat() const { return this->operator float(); }
    int64 toInt64() const { return this->operator winux::int64(); }
    uint64 toUInt64() const { return this->operator winux::uint64(); }
    double toDouble() const { return this->operator double(); }
    template < typename _ChTy >
    XString<_ChTy> toString() const;
    AnsiString toAnsi() const { return this->operator AnsiString(); }
    UnicodeString toUnicode() const { return this->operator UnicodeString(); }
    Buffer toBuffer() const { return this->operator Buffer(); }
    MixedArray toArray() const { return this->operator MixedArray(); }
    Collection toCollection() const { return this->operator Collection(); }
    template < typename _Ty >
    _Ty to() const;

    // 比较操作符 --------------------------------------------------------------------------
    bool operator == ( Mixed const & other ) const;
    bool operator < ( Mixed const & other ) const;
    bool operator > ( Mixed const & other ) const;
    bool operator != ( Mixed const & other ) const { return !this->operator == (other); }
    bool operator >= ( Mixed const & other ) const { return !this->operator < (other); }
    bool operator <= ( Mixed const & other ) const { return !this->operator > (other); }

    // 判定特殊类型 -------------------------------------------------------------------------
    bool isNull() const { return this->_type == MT_NULL; }
    bool isNumeric() const { return this->_type > MT_NULL && this->_type < MT_ANSI; }
    bool isInteger() const { return this->isNumeric() && this->_type != MT_FLOAT && this->_type != MT_DOUBLE; }
    bool isString() const { return this->_type == MT_ANSI || this->_type == MT_UNICODE; }
    bool isSequence() const { return this->_type >= MT_ANSI && this->_type <= MT_BINARY; }
    bool isAnsi() const { return this->_type == MT_ANSI; }
    bool isUnicode() const { return this->_type == MT_UNICODE; }
    bool isBinary() const { return this->_type == MT_BINARY; }
    bool isContainer() const { return this->_type == MT_ARRAY || this->_type == MT_COLLECTION; }
    bool isArray() const { return this->_type == MT_ARRAY; }
    bool isCollection() const { return this->_type == MT_COLLECTION; }

    // 创建相关类型 -------------------------------------------------------------------------
    /** \brief 创建一个字符串，根据_ChTy设置type为`MT_ANSI`或`MT_UNICODE` */
    template < typename _ChTy >
    Mixed & createString();
    /** \brief 创建一个字符串，根据_ChTy设置type为`MT_ANSI`或`MT_UNICODE` */
    template < typename _ChTy >
    Mixed & createString( XString<_ChTy> const & str );
    /** \brief 创建一个字符串，根据_ChTy设置type为`MT_ANSI`或`MT_UNICODE` */
    template < typename _ChTy >
    Mixed & createString( XString<_ChTy> && str );

    /** \brief 创建一个Ansi字符串，并设置type为`MT_ANSI` */
    Mixed & createAnsi( AnsiString const & str );
    /** \brief 创建一个Ansi字符串，并设置type为`MT_ANSI` */
    Mixed & createAnsi( AnsiString && str = Literal<char>::emptyStr );
    /** \brief 创建一个Unicode字符串，并设置type为`MT_UNICODE` */
    Mixed & createUnicode( UnicodeString const & str );
    /** \brief 创建一个Unicode字符串，并设置type为`MT_UNICODE` */
    Mixed & createUnicode( UnicodeString && str = Literal<wchar>::emptyStr );
    /** \brief 创建一个缓冲区，自动把先前的数据清空，并设置type为`MT_BINARY` */
    Mixed & createBuffer( size_t size = 0 );
    /** \brief 创建一个数组，自动把先前的数据清空，并设置type为`MT_ARRAY` */
    Mixed & createArray( size_t count = 0 );
    /** \brief 创建一个集合，自动把先前的数据清空，并设置type为`MT_COLLECTION` */
    Mixed & createCollection( bool caseInsensitive = false );

    // Buffer有关操作 ----------------------------------------------------------------------
    /** \brief 分配一块内存，自动释放先前数据，并设置type为`MT_BINARY` */
    void alloc( size_t size, bool setDataSize = true );

    /** \brief 把窥探模式下的`MT_BINARY`类型变为拷贝模式，如果`copyCapacity`为true时连容量也一起拷贝，否则只拷贝数据。
     *
     *  即使`Mixed`不是`MT_BINARY`类型也不会出错，会直接返回false */
    bool peekCopy( bool copyCapacity = false );

    /** \brief 暴露缓冲区指针
     *
     *  即使`Mixed`不是`MT_BINARY`类型也不会出错，会直接返回NULL */
    void * getBuf() const;

    // Array/Collection有关的操作 ----------------------------------------------------------
    /** \brief 取得数组全部元素，返回元素个数
     *
     *  必须是`MT_ARRAY`类型 */
    template < typename _Ty, typename _Alloc >
    size_t getArray( std::vector< _Ty, _Alloc > * arr ) const
    {
        if ( !this->isArray() ) throw MixedError( MixedError::meUnexpectedType, TypeStringA(*this) + " can't support getArray()" );
        for ( auto it = this->_pArr->begin(); it != this->_pArr->end(); ++it )
            arr->push_back(*it);
        return arr->size();
    }

    /** \brief 获取全部键名，返回键名个数
     *
     *  必须是`MT_COLLECTION`类型 */
    template < typename _KTy, typename _Alloc >
    size_t getKeys( std::vector< _KTy, _Alloc > * keys ) const
    {
        if ( !this->isCollection() ) throw MixedError( MixedError::meUnexpectedType, TypeStringA(*this) + " can't support getKeys()" );
        return this->_pColl->getKeys(keys);
    }

    /** \brief 获取映射表，返回键值对个数
     *
     *  必须是`MT_COLLECTION`类型 */
    template < typename _KTy, typename _VTy, typename _Pr, typename _Alloc >
    size_t getMap( std::map< _KTy, _VTy, _Pr, _Alloc > * m ) const
    {
        if ( !this->isCollection() ) throw MixedError( MixedError::meUnexpectedType, TypeStringA(*this) + " can't support getMap()" );
        return this->_pColl->getMap(m);
    }

    /** \brief 判断序列或容器是否为空
     *
     *  当`Mixed`是序列或容器类型时，判断是否为空。其他类型时都会返回空。 */
    bool isEmpty() const
    {
        switch ( this->_type )
        {
        case MT_ANSI:
            return this->_pStr->empty();
        case MT_UNICODE:
            return this->_pWStr->empty();
        case MT_BINARY:
            return this->_pBuf->getSize() == 0;
        case MT_ARRAY:
            return this->_pArr->empty();
        case MT_COLLECTION:
            return this->_pColl->isEmpty();
        default:
            return true;
        }
    }

    /** \brief 取得序列类型的大小
     *
     *  即使`Mixed`不是序列类型也不会出错，会直接返回0 */
    size_t getSize() const
    {
        switch ( this->_type )
        {
        case MT_ANSI:
            return this->_pStr->size();
        case MT_UNICODE:
            return this->_pWStr->size();
        case MT_BINARY:
            return this->_pBuf->getSize();
        default:
            return 0;
        }
    }

    /** \brief 取得容器内元素个数
     *
     *  即使`Mixed`不是容器类型也不会报错，此时会返回0 */
    size_t getCount() const
    {
        switch ( this->_type )
        {
        case MT_ARRAY:
            return this->_pArr->size();
        case MT_COLLECTION:
            return this->_pColl->getCount();
        default:
            return 0;
        }
    }

    /** \brief 下标操作 */
    Mixed & operator [] ( Mixed const & k );
    /** \brief const下标操作 */
    Mixed const & operator [] ( Mixed const & k ) const;
    /** \brief 下标操作 使兼容字符串指针 */
    template < typename _ChTy >
    Mixed & operator [] ( _ChTy const * k ) { return this->operator [] ( Mixed(k) ); }
    /** \brief const下标操作 使兼容字符串指针 */
    template < typename _ChTy >
    Mixed const & operator [] ( _ChTy const * k ) const { return this->operator [] ( Mixed(k) ); }

    /** \brief 当`Mixed`为Array或Collection类型时，`get<_Ty>()`能把指定'索引/Key'的元素按照指定类型取出来 */
    template < typename _Ty >
    _Ty get( Mixed const & k, Mixed const & defval = mxNull ) const { return this->get( k, defval ).to<_Ty>(); }

    /** \brief 当Mixed为Array或Collection类型时，取得指定'索引/Key'的元素，不存在则返回默认值 */
    Mixed const & get( Mixed const & k, Mixed const & defval = mxNull ) const;

    /** \brief Collection获取'键值对'索引操作 */
    MixedMixedPair & getPair( size_t i );
    /** \brief Collection获取'键值对'索引操作 */
    MixedMixedPair const & getPair( size_t i ) const;

    /** \brief Collection设置'键值对'索引操作 */
    Mixed & setPair( size_t i, Mixed const & k, Mixed const & v );

    class CollectionAssigner
    {
    public:
        CollectionAssigner( Mixed * mx ) : _mx(mx) { }
        CollectionAssigner & operator()( Mixed const & k, Mixed const & v )
        {
            if ( _mx->isCollection() ) _mx->_pColl->addPair( k, v );
            return *this;
        }
        CollectionAssigner & operator()( Mixed const & k, Mixed && v )
        {
            if ( _mx->isCollection() ) _mx->_pColl->addPair( k, std::move(v) );
            return *this;
        }
        operator Mixed & () { return *_mx; }

    private:
        Mixed * _mx;
    };

    /** \brief 往Collection添加数据
     *
     *  如果不是Collection，则自动释放之前数据，创建Collection */
    CollectionAssigner addPair( bool caseInsensitive = false )
    {
        if ( this->_type != MT_COLLECTION ) this->createCollection(caseInsensitive);
        return CollectionAssigner(this);
    }

    /** \brief 往Collection添加一个pair。非Collection类型调用此函数会抛异常 */
    Mixed & addPair( Mixed const & k, Mixed const & v );

    class ArrayAssigner
    {
    public:
        ArrayAssigner( Mixed * mx ) : _mx(mx) { }
        ArrayAssigner & operator()( Mixed const & v )
        {
            if ( _mx->isArray() ) _mx->_pArr->push_back(v);
            return *this;
        }
        ArrayAssigner & operator()( Mixed && v )
        {
            if ( _mx->isArray() ) _mx->_pArr->push_back( std::move(v) );
            return *this;
        }
        operator Mixed & () { return *_mx; }

    private:
        Mixed * _mx;
    };

    /** \brief 往Array添加数据
     *
     *  如果不是Array，则自动释放之前数据，创建Array */
    ArrayAssigner add()
    {
        if ( this->_type != MT_ARRAY ) this->createArray();
        return ArrayAssigner(this);
    }

    /** \brief 往数组里加一个元素，返回索引值，非Array类型调用此函数会抛异常 */
    size_t add( Mixed const & v );

    /** \brief 往数组里加一个元素，返回索引值，非Array类型调用此函数会抛异常 */
    size_t add( Mixed && v );

    /** \brief 往数组里加一个唯一元素，返回索引值，非Array类型调用此函数会抛异常 */
    size_t addUnique( Mixed const & v );

    /** \brief 往数组里加一个唯一元素，返回索引值，非Array类型调用此函数会抛异常 */
    size_t addUnique( Mixed && v );

    /** \brief 删除一个元素，操作类型可以是Array或Collection，k分别代表索引或键名 */
    void del( Mixed const & k );

    /** \brief 判断元素是否存在，Array判断值是否存在，Collection判断键名是否存在
     *
     *  即使Mixed不是Array/Collection类型也不会报错，此时会返回false。 */
    bool has( Mixed const & ek ) const;

    /** \brief 合并另一个容器或添加一个元素
     *
     *  非Array/Collection类型调用此函数会抛异常 */
    Mixed & merge( Mixed const & v );

    /** \brief 反转容器内元素顺序/反转多字节数据字节序 */
    Mixed & reverse();

    // 赋值操作 --------------------------------------------------------------------------------
    // 基本类型赋值函数 -------------------------------------------------------------------------
    void assign( bool boolVal );
    void assign( char chVal );
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

    // 字符串赋值函数 ---------------------------------------------------------------------------
    void assign( AnsiString const & str );
    void assign( UnicodeString const & str );
    void assign( AnsiString && str );
    void assign( UnicodeString && str );
    void assign( char const * str, size_t len = npos );
    void assign( wchar const * str, size_t len = npos );

    // Buffer赋值函数 --------------------------------------------------------------------------
    void assign( Buffer const & buf );
    void assign( void const * binaryData, size_t size, bool isPeek = false );
    void assign( Buffer && buf );
    void assign( GrowBuffer && buf );

    // Array赋值函数 ---------------------------------------------------------------------------
    /** \brief 用vector给Array赋值 */
    template < typename _Ty, typename _Alloc >
    void assign( std::vector< _Ty, _Alloc > const & arr )
    {
        if ( this->_type == MT_ARRAY )
        {
            this->_pArr->clear();
            for ( size_t i = 0; i < arr.size(); ++i )
            {
                this->_pArr->push_back(arr[i]);
            }
        }
        else
        {
            this->free();
            this->_type = MT_ARRAY;
            this->_pArr = new MixedArray( arr.size() );
            for ( size_t i = 0; i < arr.size(); ++i )
            {
                this->_pArr->at(i) = arr[i];
            }
        }
    }

    /** \brief 用C数组给Array赋值 */
    template < typename _Ty, size_t _Count >
    void assign( _Ty (&arr)[_Count] )
    {
        if ( this->_type == MT_ARRAY )
        {
            this->_pArr->clear();
            for ( size_t i = 0; i < _Count; ++i )
            {
                this->_pArr->push_back(arr[i]);
            }
        }
        else
        {
            this->free();
            this->_type = MT_ARRAY;
            this->_pArr = new MixedArray(_Count);
            for ( size_t i = 0; i < _Count; ++i )
            {
                this->_pArr->at(i) = arr[i];
            }
        }
    }

    /** \brief 数组赋值 */
    void assign( Mixed * arr, size_t count );

    /** \brief 用initializer_list给Array赋值 */
    void assign( std::initializer_list<Mixed> && list );

    /** \brief 用$a给Array赋值 */
    void assign( $a && arr );

    // Collection赋值函数 ----------------------------------------------------------------------
    /** \brief 用map给Collection赋值 */
    template < typename _KTy, typename _VTy, typename _Pr, typename _Alloc >
    void assign( std::map< _KTy, _VTy, _Pr, _Alloc > const & m, bool caseInsensitive = false )
    {
        if ( this->_type == MT_COLLECTION )
        {
            this->_pColl->assign( m, caseInsensitive );
        }
        else
        {
            this->free();
            this->_type = MT_COLLECTION;
            this->_pColl = new Collection( m, caseInsensitive );
        }
    }

    /** \brief 用pairs给Collection赋值 */
    template < typename _KTy, typename _VTy, size_t _Count >
    void assign( std::pair< _KTy, _VTy > (&pairs)[_Count], bool caseInsensitive = false )
    {
        if ( this->_type == MT_COLLECTION )
        {
            this->_pColl->assign( pairs, caseInsensitive );
        }
        else
        {
            this->free();
            this->_type = MT_COLLECTION;
            this->_pColl = new Collection( pairs, caseInsensitive );
        }
    }

    /** \brief 用$c给Collection赋值 */
    void assign( $c && coll, bool caseInsensitive = false );

    /** \brief 用Collection赋值 */
    void assign( Collection const & coll, bool caseInsensitive = false );

    // JSON相关操作 ----------------------------------------------------------------------------
    String myJson( bool autoKeyQuotes = true, String const & spacer = $T(""), String const & newline = $T("") ) const;
    String json() const;
    Mixed & json( String const & jsonStr );

private:
    // 零初始化
    void _zeroInit() { memset( this, 0, sizeof(Mixed) ); }
    // 拷贝构造（不会判断自身原有资源情况）
    void _copyConstruct( Mixed const & other );
    // 拷贝赋值
    void _copyAssignment( Mixed const & other );
};

// class Mixed inline functions ---------------------------------------------------------------
template <>
inline XString<char> & Mixed::refString<char>()
{
    return this->refAnsi();
}

template <>
inline XString<char> const & Mixed::refString<char>() const
{
    return this->refAnsi();
}

template <>
inline XString<wchar> & Mixed::refString<wchar>()
{
    return this->refUnicode();
}

template <>
inline XString<wchar> const & Mixed::refString<wchar>() const
{
    return this->refUnicode();
}

// 生成 Mixed 引用类型模板特化方法
MIXED_TYPE_LIST(MIXED_REF_TYPE_METHOD_OUTER)

/** \brief Mixed to AnsiString */
template <>
inline XString<char> Mixed::toString<char>() const { return this->toAnsi(); }

/** \brief Mixed to UnicodeString */
template <>
inline XString<wchar> Mixed::toString<wchar>() const { return this->toUnicode(); }

template <>
inline Mixed Mixed::to<Mixed>() const { return *this; }
template <>
inline bool Mixed::to<bool>() const { return this->operator bool(); }
template <>
inline char Mixed::to<char>() const { return this->toChar(); }
template <>
inline byte Mixed::to<byte>() const { return this->operator winux::byte(); }
template <>
inline short Mixed::to<short>() const { return this->operator short(); }
template <>
inline ushort Mixed::to<ushort>() const { return this->operator winux::ushort(); }
template <>
inline int Mixed::to<int>() const { return this->operator int(); }
template <>
inline uint Mixed::to<uint>() const { return this->operator winux::uint(); }
template <>
inline long Mixed::to<long>() const { return this->operator long(); }
template <>
inline ulong Mixed::to<ulong>() const { return this->operator winux::ulong(); }
template <>
inline float Mixed::to<float>() const { return this->operator float(); }
template <>
inline int64 Mixed::to<int64>() const { return this->operator winux::int64(); }
template <>
inline uint64 Mixed::to<uint64>() const { return this->operator winux::uint64(); }
template <>
inline double Mixed::to<double>() const { return this->operator double(); }
template <>
inline AnsiString Mixed::to<AnsiString>() const { return this->operator AnsiString(); }
template <>
inline UnicodeString Mixed::to<UnicodeString>() const { return this->operator UnicodeString(); }
template <>
inline Buffer Mixed::to<Buffer>() const { return this->operator Buffer(); }
template <>
inline MixedArray Mixed::to<MixedArray>() const { return this->operator MixedArray(); }
template <>
inline Collection Mixed::to<Collection>() const { return this->operator Collection(); }

/** \brief 创建一个ANSI字符串，并设置type为MT_ANSI */
template <>
inline Mixed & Mixed::createString<char>() { return this->createAnsi(); }
/** \brief 创建一个UNICODE字符串，并设置type为MT_UNICODE */
template <>
inline Mixed & Mixed::createString<wchar>() { return this->createUnicode(); }

/** \brief 创建一个ANSI字符串，并设置type为MT_ANSI */
template <>
inline Mixed & Mixed::createString<char>( XString<char> const & str ) { return this->createAnsi(str); }
/** \brief 创建一个UNICODE字符串，并设置type为MT_UNICODE */
template <>
inline Mixed & Mixed::createString<wchar>( XString<wchar> const & str ) { return this->createUnicode(str); }

/** \brief 创建一个ANSI字符串，并设置type为MT_ANSI */
template <>
inline Mixed & Mixed::createString<char>( XString<char> && str ) { return this->createAnsi( std::move(str) ); }
/** \brief 创建一个UNICODE字符串，并设置type为MT_UNICODE */
template <>
inline Mixed & Mixed::createString<wchar>( XString<wchar> && str ) { return this->createUnicode( std::move(str) ); }

// class Collection inline functions ------------------------------------------------------
template < typename _KTy, typename _VTy, typename _Pr, typename _Alloc >
inline size_t Collection::getMap( std::map< _KTy, _VTy, _Pr, _Alloc > * m ) const
{
    if ( this->_caseInsensitive )
    {
        for ( auto it = this->_pMapI->begin(); it != this->_pMapI->end(); ++it )
            (*m)[(_KTy)it->first] = (_VTy)it->second;
        return m->size();
    }
    else
    {
        for ( auto it = this->_pMap->begin(); it != this->_pMap->end(); ++it )
            (*m)[(_KTy)it->first] = (_VTy)it->second;
        return m->size();
    }
}

inline Mixed const & Collection::get( Mixed const & k, Mixed const & defval = mxNull ) const
{
    if ( this->_caseInsensitive )
    {
        if ( this->_pMapI->find(k) != this->_pMapI->end() )
            return this->_pMapI->at(k);
    }
    else
    {
        if ( this->_pMap->find(k) != this->_pMap->end() )
            return this->_pMap->at(k);
    }
    return defval;
}

inline bool Collection::has( Mixed const & k ) const
{
    return this->_caseInsensitive ? this->_pMapI->find(k) != this->_pMapI->end() : this->_pMap->find(k) != this->_pMap->end();
}

inline bool Collection::_addUniqueKey( Mixed const & k )
{
    if ( this->_caseInsensitive )
    {
        if ( this->_pMapI->find(k) == this->_pMapI->end() )
        {
            this->_pKeysArr->push_back(k);
            return true;
        }
    }
    else
    {
        if ( this->_pMap->find(k) == this->_pMap->end() )
        {
            this->_pKeysArr->push_back(k);
            return true;
        }
    }
    return false;
}

template < typename _MAP >
inline void Collection::_fxC( _MAP * pMap, $c && coll )
{
    for ( auto & pr : coll._list )
    {
        this->_addUniqueKey(pr.first);
        (*pMap)[pr.first] = pr.second;
    }
}

template < typename _Fx1, typename _Fx2, typename _Ty >
inline void Collection::_implAssign( _Fx1 fn, _Fx2 fnI, _Ty && m, bool caseInsensitive )
{
    if ( this->_pKeysArr && ( this->_pMap || this->_pMapI ) ) // 当前已经存在分配的数组和映射表
    {
        this->_pKeysArr->clear();

        if ( this->_caseInsensitive )
        {
            // 当前是大小写无关的，但是目标是大小写相关的，删除当前的创建全新的
            if ( !caseInsensitive )
            {
                this->_caseInsensitive = caseInsensitive;
                delete this->_pMapI;
                this->_pMapI = NULL;
                this->_pMap = new MixedMixedMap();
                (this->*fn)( this->_pMap, std::forward<_Ty>(m) );
            }
            else
            {
                this->_pMapI->clear();
                (this->*fnI)( this->_pMapI, std::forward<_Ty>(m) );
            }
        }
        else
        {
            // 当前是大小写相关的，但是目标是大小写无关的，删除当前的创建全新的
            if ( caseInsensitive )
            {
                this->_caseInsensitive = caseInsensitive;
                delete this->_pMap;
                this->_pMap = NULL;
                this->_pMapI = new MixedMixedMapI();
                (this->*fnI)( this->_pMapI, std::forward<_Ty>(m) );
            }
            else
            {
                this->_pMap->clear();
                (this->*fn)( this->_pMap, std::forward<_Ty>(m) );
            }
        }
    }
    else
    {
        this->destroy();
        this->_caseInsensitive = caseInsensitive;
        this->_pKeysArr = new MixedArray(); // 存放keys
        if ( this->_caseInsensitive )
        {
            this->_pMapI = new MixedMixedMapI();
            (this->*fnI)( this->_pMapI, std::forward<_Ty>(m) );
        }
        else
        {
            this->_pMap = new MixedMixedMap();
            (this->*fn)( this->_pMap, std::forward<_Ty>(m) );
        }
    }
}

/** \brief 扩展iostream的<< */
WINUX_FUNC_DECL(std::ostream &) operator << ( std::ostream & o, Mixed const & m );
WINUX_FUNC_DECL(std::wostream &) operator << ( std::wostream & o, Mixed const & m );

//std::istream & operator >> ( std::istream & o, Mixed const & m );
//std::wistream & operator >> ( std::wistream & o, Mixed const & m );

} // namespace winux

#endif // __UTILITIES_HPP__
