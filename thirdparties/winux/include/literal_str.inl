#pragma once
// 字符/字符串字面量
#define LITERAL_LIST( sc, s ) \
    sc( A, "A" ) sc( B, "B" ) sc( C, "C" ) sc( D, "D" ) sc( E, "E" ) sc( F, "F" ) sc( N, "N" ) sc( R, "R" ) sc( T, "T" ) sc( V, "V" ) sc( X, "X" ) sc( Z, "Z" ) \
    sc( a, "a" ) sc( b, "b" ) sc( c, "c" ) sc( d, "d" ) sc( e, "e" ) sc( f, "f" ) sc( n, "n" ) sc( r, "r" ) sc( t, "t" ) sc( v, "v" ) sc( x, "x" ) sc( z, "z" ) \
    sc( zero, "0" ) sc( one, "1" ) sc( two, "2" ) sc( three, "3" ) sc( four, "4" ) sc( five, "5" ) sc( six, "6" ) sc( seven, "7" ) sc( eight, "8" ) sc( nine, "9" ) \
    sc( nul, "" ) sc( empty, "" ) sc( bel, "\a" ) sc( bs, "\b" ) sc( ht, "\t" ) sc( lf, "\n" ) sc( vt, "\v" ) sc( ff, "\f" ) sc( cr, "\r" ) \
    sc( space, " " ) \
    sc( under, "_" ) \
    sc( dollar, "$" ) \
    sc( slash, "\\" ) \
    sc( divide, "/" ) \
    sc( positive, "+" ) \
    sc( negative, "-" ) \
    sc( quote, "\"" ) \
    sc( apos, "\'" ) \
    sc( sharp, "#" ) \
    sc( caret, "^" ) \
    sc( period, "." ) \
    sc( comma, "," ) \
    sc( colon, ":" ) \
    sc( semicolon, ";" ) \
    sc( pipe, "|" ) \
    sc( amp, "&" ) \
    sc( lt, "<" ) \
    sc( gt, ">" ) \
    sc( eq, "=" ) \
    sc( LBRK, "(" ) sc( RBRK, ")" ) \
    sc( LSquareBRK, "[" ) sc( RSquareBRK, "]" ) \
    sc( LCurlyBRK, "{" ) sc( RCurlyBRK, "}" ) \
    s( dbldivide, "//" ) \
    s( crlf, "\r\n" ) \
    s( slash_a, "\\a" ) \
    s( slash_b, "\\b" ) \
    s( slash_t, "\\t" ) \
    s( slash_n, "\\n" ) \
    s( slash_v, "\\v" ) \
    s( slash_f, "\\f" ) \
    s( slash_r, "\\r" ) \
    s( slash_x, "\\x" ) \
    s( cslashes, "\n\r\t\v\a\\\'\"" ) \
    s( hexadecUpper, "0123456789ABCDEF" ) \
    s( hexadecLower, "0123456789abcdef" ) \
    s( alphabetUpper, "ABCDEFGHIJKLMNOPQRSTUVWXYZ" ) \
    s( alphabetLower, "abcdefghijklmnopqrstuvwxyz" ) \
    s( boolFalse, "false" ) s( boolTrue, "true" ) s( boolNo, "no" ) s( boolYes, "yes" ) s( boolOff, "off" ) s( boolOn, "on" ) \
    s( nullValue, "null" )

// 生成不同类型的字符/字符串字面量
#define LITERAL_CHAR_SC( name, content ) static constexpr char const *name##Str = content; static constexpr char const name##Char = name##Str[0];
#define LITERAL_CHAR_S( name, content ) static constexpr char const *name##Str = content;

#define LITERAL_WCHAR_SC( name, content ) static constexpr wchar const *name##Str = L##content; static constexpr wchar const name##Char = name##Str[0];
#define LITERAL_WCHAR_S( name, content ) static constexpr wchar const *name##Str = L##content;

#define LITERAL_CHAR16_SC( name, content ) static constexpr char16 const *name##Str = u##content; static constexpr char16 const name##Char = name##Str[0];
#define LITERAL_CHAR16_S( name, content ) static constexpr char16 const *name##Str = u##content;

#define LITERAL_CHAR32_SC( name, content ) static constexpr char32 const *name##Str = U##content; static constexpr char32 const name##Char = name##Str[0];
#define LITERAL_CHAR32_S( name, content ) static constexpr char32 const *name##Str = U##content;

template < typename _ChTy >
struct Literal
{
    LITERAL_LIST( LITERAL_CHAR_SC, LITERAL_CHAR_S )
};

template <>
struct Literal<char>
{
    LITERAL_LIST( LITERAL_CHAR_SC, LITERAL_CHAR_S )
};

template <>
struct Literal<wchar>
{
    LITERAL_LIST( LITERAL_WCHAR_SC, LITERAL_WCHAR_S )
};

template <>
struct Literal<char16>
{
    LITERAL_LIST( LITERAL_CHAR16_SC, LITERAL_CHAR16_S )
};

template <>
struct Literal<char32>
{
    LITERAL_LIST( LITERAL_CHAR32_SC, LITERAL_CHAR32_S )
};
