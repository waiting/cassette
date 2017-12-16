//////////////////////////////////////////////////////////////////////
// desc:    控制台相关
// author:  WT
//////////////////////////////////////////////////////////////////////

#if !defined(__WINPLUS_CONSOLE_HPP__)
#define __WINPLUS_CONSOLE_HPP__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace winplus
{

template < typename _VarType > class text_attr_t
{
private:
    WORD _wPrevAttributes;
    WORD _wAttributes;
    HANDLE _hStdHandle;
    _VarType & _v;
public:
    text_attr_t( WORD wAttributes, _VarType const & v ) : _v( const_cast<_VarType &>(v) )
    {
        _hStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        _wAttributes = wAttributes;
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo( _hStdHandle, &csbi );
        _wPrevAttributes = csbi.wAttributes;
    }
    void modify() { SetConsoleTextAttribute( _hStdHandle, _wAttributes | ( _wPrevAttributes & 0x00F0 ) ); }
    void resume() { SetConsoleTextAttribute( _hStdHandle, _wPrevAttributes ); }
    _VarType & val() { return _v; }
    template < typename _ValTy >
    friend std::ostream & operator << ( std::ostream & o, text_attr_t<_ValTy> & tr );
    template < typename _ValTy >
    friend std::istream & operator >> ( std::istream & in, text_attr_t<_ValTy> & tr );
};

template < typename _VarType >
inline std::ostream & operator << ( std::ostream & o, text_attr_t<_VarType> & tr )
{
    tr.modify();
    o << tr.val();
    tr.resume();
    return o;
}

template < typename _VarType >
inline std::istream & operator >> ( std::istream & in, text_attr_t<_VarType> & tr )
{
    tr.modify();
    in >> tr.val();
    tr.resume();
    return in;
}

template < typename _VarType >
inline text_attr_t<_VarType> ConsoleColor( WORD wAttr, _VarType const & v )
{
    return text_attr_t<_VarType>( wAttr, v );
}

/* 确保文本宽度 */
WINPLUS_FUNC_DECL(String) MakesureTextWidth( String const & text, int text_width, bool left_align = false, int ellipsis = 3 );

/* 计算进度块数目 */
inline int CalcBarBlocks( int text_limit, int screen_width )
{
    return ( screen_width - text_limit - 1 - 1 - 1 ) / 2;
}

/* 输出一个横条，左边带一个文字说明 */
WINPLUS_FUNC_DECL(void) PrintBarEx( String const & text, int text_width, int screen_width, WORD clr, int bar_blocks );

/* 在gui模式下开启控制台窗口 */
WINPLUS_FUNC_DECL(void) Win32GUI_ShowConsole();

} // namespace winplus

#endif // !defined(__WINPLUS_CONSOLE_HPP__)
