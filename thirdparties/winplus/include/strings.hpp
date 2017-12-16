//////////////////////////////////////////////////////////////////////////
// purpose: 字串相关
// author:  WT
//////////////////////////////////////////////////////////////////////////

#if !defined(__WINPLUS_STRINGS_HPP__)
#define __WINPLUS_STRINGS_HPP__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace winplus
{

/* 返回一个本地字符串里有多少个实际的字符(by local CodePage),用于mbstowcs */
WINPLUS_FUNC_DECL(UINT) LocalCharsCount( LocalString const & local );
/* 返回一个unicode字符串最少长度(by local CodePage),用于wcstombs */
WINPLUS_FUNC_DECL(UINT) UnicodeMinLength( UnicodeString const & unicode );
/* Unicode转换到本地Ansi */
WINPLUS_FUNC_DECL(LocalString) UnicodeToLocal( UnicodeString const & unicode );
/* 本地ANSI转到Unicode */
WINPLUS_FUNC_DECL(UnicodeString) LocalToUnicode( LocalString const & local );

/* 兼容字符串与Unicode,Local字符串相互转换 */
WINPLUS_FUNC_DECL(String) LocalToString( LocalString const & local );
WINPLUS_FUNC_DECL(String) UnicodeToString( UnicodeString const & unicode );
WINPLUS_FUNC_DECL(LocalString) StringToLocal( String const & str );
WINPLUS_FUNC_DECL(UnicodeString) StringToUnicode( String const & str );

/* UTF8与兼容串之间的转换 */
WINPLUS_FUNC_DECL(String) Utf8ToString( AnsiString const & utf8 );
WINPLUS_FUNC_DECL(AnsiString) StringToUtf8( String const & str );

/* 分割字符串 */
WINPLUS_FUNC_DECL(INT) StrSplit( String const & str, String const & delim, StringArray * arr );
WINPLUS_FUNC_DECL(INT) StrSplit( AnsiString const & str, AnsiString const & delim, AnsiStringArray * arr );
WINPLUS_FUNC_DECL(INT) StrSplit( UnicodeString const & str, UnicodeString const & delim, UnicodeStringArray * arr );

WINPLUS_FUNC_DECL(StringArray) StrSplit( String const & str, String const & delim );
/* 使用逗号分割字符串 */
inline StringArray StrSplitComma( String const & str )
{
    return StrSplit( str, TEXT(",") );
}
/* 使用分号分割字符串 */
inline StringArray StrSplitSemicolon( String const & str )
{
    return StrSplit( str, TEXT(";") );
}
WINPLUS_FUNC_DECL(String) StrJoin( String const & delim, StringArray const & arr );
WINPLUS_FUNC_DECL(String) StrInsert( String const & str, INT start, INT end, String const & insert );
WINPLUS_DLL String FormatExV( INT cch, LPCTSTR fmt, va_list args );
WINPLUS_DLL String FormatEx( INT cch, LPCTSTR fmt, ... );
WINPLUS_DLL String Format( LPCTSTR fmt, ... );
// 去掉左右两端的空白字符
WINPLUS_FUNC_DECL(AnsiString) _StrTrim( AnsiString const & str );
WINPLUS_FUNC_DECL(UnicodeString) _StrTrim( UnicodeString const & str );
WINPLUS_FUNC_DECL(String) _StrTrim( String const & str );

/* 载入字符串资源 */
WINPLUS_FUNC_DECL(UnicodeString) LoadStringExW( HMODULE module, UINT id );
WINPLUS_FUNC_DECL(String) LoadStringEx( HMODULE module, UINT id );
/* 采用GetModuleHandle(NULL)指定模块 */
WINPLUS_FUNC_DECL(String) LoadString( UINT id );

/* 反斜杠操作 */
WINPLUS_FUNC_DECL(AnsiString) AddSlashes( AnsiString const & str, AnsiString const & charlist );
WINPLUS_FUNC_DECL(AnsiString) StripSlashes( AnsiString const & str, AnsiString const & charlist );

/* KMP多项匹配替换 */
class WINPLUS_DLL MultiMatch
{
public:
    typedef std::map<String, String> StringMap;
    typedef std::vector<String> StringArray;
    #define KmpCalcNext _Templ_KmpCalcNext<short>
    typedef std::vector<short> KmpNextValueArray;

    struct MatchState
    {
        int j; //下一次从子串开始的位置
        int markpos;// 标记，表示进行到这个位置了，该从这个位置开始
    };

    typedef std::vector<MatchState> MatchStates;

    struct MatchResult
    {
        int pos;
        int item;   // 匹配项的索引
    };

    typedef String (* ReplaceFuncType)( MultiMatch const * matchObj, int item, void * extra );

    MultiMatch( String matches[], uint m, String replaces[], uint n );
    MultiMatch( String matches[], uint count, ReplaceFuncType replaceFunc, void * extra = NULL );
    MultiMatch( StringArray const & matches, StringArray const & replaces );
    MultiMatch( StringArray const & matches, ReplaceFuncType replaceFunc, void * extra = NULL );

    MultiMatch();
    void init( StringArray const & matches, StringArray const & replaces );
    void init( StringArray const & matches, ReplaceFuncType replaceFunc, void * extra = NULL );

    /** 添加一对匹配替换项,返回要匹配的项数 */
    int addMatchReplacePair( String const & match, String const & replace );
    /** 添加要匹配项 */
    int addMatch( String const & match );
    /** 设置新的替换回调函数，返回旧的替换回调函数 */
    ReplaceFuncType setReplaceFunc( ReplaceFuncType newReplaceFunc );

    /** 搜索任意一项匹配 str:字符串, offset:偏移,表示从哪个位置开始搜
     * 返回的位置包含offset值 */
    MatchResult search( String const & str, int offset = 0 ) const;
    /* 贪婪模式 */
    //MatchResult greedSearch( String const & str, int offset = 0 ) const;
    /** 替换 */
    String replace( String const & str) const;

    String getMatchItem( int item ) const;
    String getReplaceItem( int item ) const;
protected:
    void _construct();

private:
    std::vector<KmpNextValueArray> _nextVals;
    StringArray _matchItems;
    StringArray _replaceItems;
    ReplaceFuncType _replaceFunc;
    void * _extra;
};

///////////////////////////////////////////////////////////////////////////
/* 填充based-zero字符串缓冲区包装类 */
class SZInput
{
    union
    {
        char * _psz;
        wchar_t * _pwsz;
    };
    enum { szCharInput, szWCharInput } _type;
    size_t _count;
public:
    SZInput( char * psz, size_t count ) : _psz(psz), _type(szCharInput), _count(count) { }
    SZInput( wchar_t * pwsz, size_t count ) : _pwsz(pwsz), _type(szWCharInput), _count(count) { }

    SZInput & operator = ( char const * pstr )
    {
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

    SZInput & operator = ( wchar_t const * pwstr )
    {
        switch ( _type )
        {
        case szCharInput:
            {
                LocalString str = UnicodeToLocal(pwstr);
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

};

#if _MSC_VER >= 1600
/* VC2010以上支持模板取数组大小 */
template < typename _CHAR, DWORD _N >
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

} // namespace winplus

#endif // !defined(__WINPLUS_STRINGS_HPP__)
