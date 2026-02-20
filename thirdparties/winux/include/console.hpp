#ifndef __CONSOLE_HPP__
#define __CONSOLE_HPP__
//
// console 提供控制台相关的功能，例如彩色文字输出
//

#include <iostream>

namespace winux
{
/** \brief 颜色属性标记 */
enum ConsoleColorAttrFlags : winux::ushort
{
#if defined(OS_WIN)
    fgBlack = 0,
    bgBlack = 0,

    fgNavy = 0x01,
    fgAtrovirens = 0x02,
        fgTeal = fgNavy | fgAtrovirens,
    fgMaroon = 0x04,
        fgPurple = fgNavy | fgMaroon,
        fgOlive = fgAtrovirens | fgMaroon,
        fgSilver = fgNavy | fgAtrovirens | fgMaroon,

    fgIntensity = 0x08,
    fgGray = fgIntensity,

    fgBlue = fgIntensity | fgNavy,
    fgGreen = fgIntensity | fgAtrovirens,
        fgAqua = fgIntensity | fgNavy | fgAtrovirens,
    fgRed = fgIntensity | fgMaroon,
        fgFuchsia = fgIntensity | fgNavy | fgMaroon,
        fgYellow = fgIntensity | fgAtrovirens | fgMaroon,
        fgWhite = fgIntensity | fgNavy | fgAtrovirens | fgMaroon,

////////////////////////////////////////////////////////////////
    bgNavy = 0x10,
    bgAtrovirens = 0x20,
        bgTeal = bgNavy | bgAtrovirens,
    bgMaroon = 0x40,
        bgPurple = bgNavy | bgMaroon,
        bgOlive = bgAtrovirens | bgMaroon,
        bgSilver = bgNavy | bgAtrovirens | bgMaroon,

    bgIntensity = 0x80,
    bgGray = bgIntensity,

    bgBlue = bgIntensity | bgNavy,
    bgGreen = bgIntensity | bgAtrovirens,
        bgAqua = bgIntensity | bgNavy | bgAtrovirens,
    bgRed = bgIntensity | bgMaroon,
        bgFuchsia = bgIntensity | bgNavy | bgMaroon,
        bgYellow = bgIntensity | bgAtrovirens | bgMaroon,
        bgWhite = bgIntensity | bgNavy | bgAtrovirens | bgMaroon,
#else
    fgBlack = 0,
    fgNavy = 1,
    fgAtrovirens = 2,
    fgTeal = 3,
    fgMaroon = 4,
    fgPurple = 5,
    fgOlive = 6,
    fgSilver = 7,
    fgGray = 8,
    fgIntensity = fgGray,
    fgBlue = 9,
    fgGreen = 10,
    fgAqua = 11,
    fgRed = 12,
    fgFuchsia = 13,
    fgYellow = 14,
    fgWhite = 15,

    bgBlack = 0x10,
    bgNavy = 0x20,
    bgAtrovirens = 0x30,
    bgTeal = 0x40,
    bgMaroon = 0x50,
    bgPurple = 0x60,
    bgOlive = 0x70,
    bgSilver = 0x00,
    bgWhite = 0x80,
    bgGray = bgSilver,
    bgBlue = bgNavy,
    bgGreen = bgAtrovirens,
    bgAqua = bgTeal,
    bgRed = bgMaroon,
    bgFuchsia = bgPurple,
    bgYellow = bgOlive,
#endif
    ccaIgnore = 0x8800,
    bgIgnore = 0x8000,
    fgIgnore = 0x0800
};

/** \brief 控制台属性类 */
class WINUX_DLL ConsoleAttr
{
public:
    ConsoleAttr( winux::ushort attr );

    void modify() const;
    void resume() const;

private:
#if defined(OS_WIN)
    WORD _wPrevAttributes;
    WORD _wAttributes;
    HANDLE _hStdHandle;
#else
    winux::AnsiString _strAttr;
#endif
    winux::ushort _attr;
};

/** \brief 控制台属性类模板 */
template < typename _VarType >
class ConsoleAttrT : public ConsoleAttr
{
public:
    ConsoleAttrT( winux::ushort attr, _VarType const & v ) : ConsoleAttr(attr), _v( const_cast<_VarType&>(v) )
    {
    }

    _VarType & val() const { return _v; }

private:
    _VarType & _v;
};

template < typename _VarType >
inline std::ostream & operator << ( std::ostream & out, ConsoleAttrT<_VarType> const & tr )
{
    tr.modify();
    out << tr.val();
    tr.resume();
    return out;
}

template < typename _VarType >
inline std::istream & operator >> ( std::istream & in, ConsoleAttrT<_VarType> const & tr )
{
    tr.modify();
    in >> tr.val();
    tr.resume();
    return in;
}

template < typename _VarType >
inline std::wostream & operator << ( std::wostream & out, ConsoleAttrT<_VarType> const & tr )
{
    tr.modify();
    out << tr.val();
    tr.resume();
    return out;
}

template < typename _VarType >
inline std::wistream & operator >> ( std::wistream & in, ConsoleAttrT<_VarType> const & tr )
{
    tr.modify();
    in >> tr.val();
    tr.resume();
    return in;
}

/** \brief 控制台颜色函数 */
template < typename _VarType >
inline ConsoleAttrT<_VarType> ConsoleColor( winux::ushort attr, _VarType const & v )
{
    return ConsoleAttrT<_VarType>( attr, v );
}

/** \brief 控制台输出ScopeGuard */
class WINUX_DLL ConsoleOuputMutexScopeGuard
{
public:
    ConsoleOuputMutexScopeGuard();
    ~ConsoleOuputMutexScopeGuard();

private:
    DISABLE_OBJECT_COPY(ConsoleOuputMutexScopeGuard)
};

template < typename _ChTy >
inline static void OutputV( std::basic_ostream<_ChTy> & out )
{
}

template < typename _ChTy, typename _Ty, typename... _ArgType >
inline static void OutputV( std::basic_ostream<_ChTy> & out, _Ty&& a, _ArgType&& ... arg )
{
    out << std::forward<_Ty>(a);
    OutputV<_ChTy>( out, std::forward<_ArgType>(arg)... );
}

template < typename... _ArgType >
inline static void ColorOutputLine( winux::ConsoleAttr const & ca, _ArgType&& ... arg )
{
    ConsoleOuputMutexScopeGuard guard;
    ca.modify();
#if defined(_UNICODE) || defined(UNICODE)
    OutputV( std::wcout, std::forward<_ArgType>(arg)... );
#else
    OutputV( std::cout, std::forward<_ArgType>(arg)... );
#endif
    ca.resume();
#if defined(_UNICODE) || defined(UNICODE)
    std::wcout << std::endl;
#else
    std::cout << std::endl;
#endif
}

template < typename... _ArgType >
inline static void ColorOutput( winux::ConsoleAttr const & ca, _ArgType&& ... arg )
{
    ConsoleOuputMutexScopeGuard guard;
    ca.modify();
#if defined(_UNICODE) || defined(UNICODE)
    OutputV( std::wcout, std::forward<_ArgType>(arg)... );
#else
    OutputV( std::cout, std::forward<_ArgType>(arg)... );
#endif
    ca.resume();
}

// 控制台输出布局控制函数 --------------------------------------------------------------------------------
/** \brief 文本对齐方式 */
enum ConsoleTextLayoutAlignment
{
    ctlaLeft,
    ctlaCenter,
    ctlaRight
};

/** \brief 控制台文本布局，按指定长度输出文本。可以控制对齐方式，是否截断，截断省略号 */
WINUX_FUNC_DECL(String) ConsoleTextLayout( String const & text, size_t maxLength, ConsoleTextLayoutAlignment align = ctlaLeft, String::value_type blankCh = ' ', bool isTrunc = true, size_t ellipsis = 3 );


} // namespace winux

#endif // __CONSOLE_HPP__
