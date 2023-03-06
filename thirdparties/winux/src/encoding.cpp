
#include "utilities.hpp"
#include "encoding.hpp"
#include "strings.hpp"
#include "md5.hpp"
#include "mystdint.h"
#include "sha1.h"

namespace winux
{

#include "is_x_funcs.inl"

static String const __UrlEncodeTable[] = {
    "%00", "%01", "%02", "%03", "%04", "%05", "%06", "%07", "%08", "%09", "%0A", "%0B", "%0C", "%0D", "%0E", "%0F",
    "%10", "%11", "%12", "%13", "%14", "%15", "%16", "%17", "%18", "%19", "%1A", "%1B", "%1C", "%1D", "%1E", "%1F",
    /* */"+", /*!*/"%21", /*"*/"%22", /*#*/"%23", /*$*/"%24", /*%*/"%25", /*&*/"%26", /*'*/"%27", /*(*/"%28", /*)*/"%29", /***/"%2A", /*+*/"%2B", /*,*/"%2C", "-", ".", /* / */"%2F",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", /*:*/"%3A", /*;*/"%3B", /*<*/"%3C", /*=*/"%3D", /*>*/"%3E", /*?*/"%3F",
    /*@*/"%40", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", /*[*/"%5B", /* \ */"%5C", /*]*/"%5D", /*^*/"%5E", "_",
    /*`*/ "%60", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", /*{*/"%7B", /*|*/"%7C", /*}*/"%7D", /*~*/"%7E", "%7F",
    "%80", "%81", "%82", "%83", "%84", "%85", "%86", "%87", "%88", "%89", "%8A", "%8B", "%8C", "%8D", "%8E", "%8F",
    "%90", "%91", "%92", "%93", "%94", "%95", "%96", "%97", "%98", "%99", "%9A", "%9B", "%9C", "%9D", "%9E", "%9F",
    "%A0", "%A1", "%A2", "%A3", "%A4", "%A5", "%A6", "%A7", "%A8", "%A9", "%AA", "%AB", "%AC", "%AD", "%AE", "%AF",
    "%B0", "%B1", "%B2", "%B3", "%B4", "%B5", "%B6", "%B7", "%B8", "%B9", "%BA", "%BB", "%BC", "%BD", "%BE", "%BF",
    "%C0", "%C1", "%C2", "%C3", "%C4", "%C5", "%C6", "%C7", "%C8", "%C9", "%CA", "%CB", "%CC", "%CD", "%CE", "%CF",
    "%D0", "%D1", "%D2", "%D3", "%D4", "%D5", "%D6", "%D7", "%D8", "%D9", "%DA", "%DB", "%DC", "%DD", "%DE", "%DF",
    "%E0", "%E1", "%E2", "%E3", "%E4", "%E5", "%E6", "%E7", "%E8", "%E9", "%EA", "%EB", "%EC", "%ED", "%EE", "%EF",
    "%F0", "%F1", "%F2", "%F3", "%F4", "%F5", "%F6", "%F7", "%F8", "%F9", "%FA", "%FB", "%FC", "%FD", "%FE", "%FF",
};

static String const __UriComponentEncodeTable[] = {
    "%00", "%01", "%02", "%03", "%04", "%05", "%06", "%07", "%08", "%09", "%0A", "%0B", "%0C", "%0D", "%0E", "%0F",
    "%10", "%11", "%12", "%13", "%14", "%15", "%16", "%17", "%18", "%19", "%1A", "%1B", "%1C", "%1D", "%1E", "%1F",
    /* */"%20", /*!*/"%21", /*"*/"%22", /*#*/"%23", /*$*/"%24", /*%*/"%25", /*&*/"%26", /*'*/"%27", /*(*/"%28", /*)*/"%29", /***/"%2A", /*+*/"%2B", /*,*/"%2C", "-", ".", /* / */"%2F",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", /*:*/"%3A", /*;*/"%3B", /*<*/"%3C", /*=*/"%3D", /*>*/"%3E", /*?*/"%3F",
    /*@*/"%40", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", /*[*/"%5B", /* \ */"%5C", /*]*/"%5D", /*^*/"%5E", "_",
    /*`*/ "%60", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", /*{*/"%7B", /*|*/"%7C", /*}*/"%7D", "~", "%7F",
    "%80", "%81", "%82", "%83", "%84", "%85", "%86", "%87", "%88", "%89", "%8A", "%8B", "%8C", "%8D", "%8E", "%8F",
    "%90", "%91", "%92", "%93", "%94", "%95", "%96", "%97", "%98", "%99", "%9A", "%9B", "%9C", "%9D", "%9E", "%9F",
    "%A0", "%A1", "%A2", "%A3", "%A4", "%A5", "%A6", "%A7", "%A8", "%A9", "%AA", "%AB", "%AC", "%AD", "%AE", "%AF",
    "%B0", "%B1", "%B2", "%B3", "%B4", "%B5", "%B6", "%B7", "%B8", "%B9", "%BA", "%BB", "%BC", "%BD", "%BE", "%BF",
    "%C0", "%C1", "%C2", "%C3", "%C4", "%C5", "%C6", "%C7", "%C8", "%C9", "%CA", "%CB", "%CC", "%CD", "%CE", "%CF",
    "%D0", "%D1", "%D2", "%D3", "%D4", "%D5", "%D6", "%D7", "%D8", "%D9", "%DA", "%DB", "%DC", "%DD", "%DE", "%DF",
    "%E0", "%E1", "%E2", "%E3", "%E4", "%E5", "%E6", "%E7", "%E8", "%E9", "%EA", "%EB", "%EC", "%ED", "%EE", "%EF",
    "%F0", "%F1", "%F2", "%F3", "%F4", "%F5", "%F6", "%F7", "%F8", "%F9", "%FA", "%FB", "%FC", "%FD", "%FE", "%FF",
};

WINUX_FUNC_IMPL(String) UrlEncode( String const & str )
{
    String s;
    s.reserve( str.length() * 3 );
    for ( String::const_iterator it = str.begin(); it != str.end(); ++it )
    {
        unsigned char const c = *it;
        if (
            c == '-' || c == '_' || c == '.' ||
            ( c >= '0' && c <= '9' ) ||
            ( c >= 'A' && c <= 'Z' ) ||
            ( c >= 'a' && c <= 'z' )
        )
        {
            s += c;
        }
        else if ( c == ' ' )
        {
            s += '+';
        }
        else
        {
            s += __UrlEncodeTable[c];
        }
    }//*/

    /*char sz[4] = { '%' };
    for ( String::const_iterator it = str.begin(); it != str.end(); ++it )
    {
        unsigned char const c = *it;
        if (
            c == '-' || c == '_' || c == '.' ||
            ( c >= '0' && c <= '9' ) ||
            ( c >= 'A' && c <= 'Z' ) ||
            ( c >= 'a' && c <= 'z' )
        )
        {
            s += c;
        }
        else if ( c == ' ' )
        {
            s += '+';
        }
        else
        {
            unsigned char tmp;
            tmp = ( c & 0xF );
            sz[2] = tmp < 10 ? tmp + '0' : tmp - 10 + 'A';
            tmp = ( ( c >> 4 ) & 0xF );
            sz[1] = tmp < 10 ? tmp + '0' : tmp - 10 + 'A';
            s += sz;
        }
    }//*/

    return s;
}

WINUX_FUNC_IMPL(String) UrlDecode( String const & str )
{
    String s;
    s.reserve( str.length() );
    String::const_iterator it;
    String::value_type low, hig;
    for ( it = str.begin(); it != str.end(); )
    {
        if ( *it == '+' )
        {
            s += ' ';
            ++it;
        }
        else if ( *it != '%' )
        {
            s += *it;
            ++it;
        }
        else // *it == '%'
        {
            ++it; // skip '%'
            if ( it != str.end() )
            {
                hig = *it;
                bool isHex = IsHex(hig);
                if ( isHex )
                {
                    hig = hig > '9' ? hig - 'A' + 10 : hig - '0';
                }
                else // is not a hex value.
                {
                    s += '%'; // 把跳过的'%'也加上
                    s += hig;
                }

                ++it;
                if (  it != str.end() )
                {
                    low = *it;

                    if ( isHex )
                    {
                        low = low > '9' ? low - 'A' + 10 : low - '0' ;
                        s += (String::value_type)( low | ( hig << 4 ) );
                    }
                    else // not hex
                    {
                        s += low;
                    }

                    ++it;
                }
            }
            else // 说明已经是最后一个字符
            {
                s += '%';
            }
        }
    }
    return s;
}

WINUX_FUNC_IMPL(String) UriComponentEncode( String const & str )
{
    String s;
    s.reserve( str.length() * 3 );
    for ( String::const_iterator it = str.begin(); it != str.end(); ++it )
    {
        unsigned char const c = *it;
        if (
            c == '-' || c == '_' || c == '.' || c == '~' ||
            ( c >= '0' && c <= '9' ) ||
            ( c >= 'A' && c <= 'Z' ) ||
            ( c >= 'a' && c <= 'z' )
        )
        {
            s += c;
        }
        else
        {
            s += __UriComponentEncodeTable[c];
        }
    }//*/

    /*char sz[4] = { '%' };
    for ( String::const_iterator it = str.begin(); it != str.end(); ++it )
    {
        unsigned char const c = *it;
        if (
            c == '-' || c == '_' || c == '.' || c == '~' ||
            ( c >= '0' && c <= '9' ) ||
            ( c >= 'A' && c <= 'Z' ) ||
            ( c >= 'a' && c <= 'z' )
        )
        {
            s += c;
        }
        else
        {
            unsigned char tmp;
            tmp = ( c & 0xF );
            sz[2] = tmp < 10 ? tmp + '0' : tmp - 10 + 'A';
            tmp = ( ( c >> 4 ) & 0xF );
            sz[1] = tmp < 10 ? tmp + '0' : tmp - 10 + 'A';
            s += sz;
        }
    }//*/

    return s;
}

WINUX_FUNC_IMPL(String) UriComponentDecode( String const & str )
{
    String s;
    s.reserve( str.length() );
    String::const_iterator it;
    String::value_type low, hig;
    for ( it = str.begin(); it != str.end(); )
    {
        if ( *it != '%' )
        {
            s += *it;
            ++it;
        }
        else
        {
            ++it; // skip '%'
            if ( it != str.end() )
            {
                hig = *it;
                bool isHex = IsHex(hig);
                if ( isHex )
                {
                    hig = hig > '9' ? hig - 'A' + 10 : hig - '0';
                }
                else // is not a hex value.
                {
                    s += '%'; // 把跳过的'%'也加上
                    s += hig;
                }

                ++it;
                if (  it != str.end() )
                {
                    low = *it;

                    if ( isHex )
                    {
                        low = low > '9' ? low - 'A' + 10 : low - '0' ;
                        s += (String::value_type)( low | ( hig << 4 ) );
                    }
                    else // not hex
                    {
                        s += low;
                    }

                    ++it;
                }
            }
            else // 说明已经是最后一个字符
            {
                s += '%';
            }
        }
    }
    return s;
}

WINUX_FUNC_IMPL(String) HtmlEncode( String const & str )
{
    thread_local MultiMatch mm( { "&amp;", "&lt;", "&gt;", "&nbsp;" }, { "&", "<", ">", " " } );
    return mm.replace(str);
}

WINUX_FUNC_IMPL(String) HtmlDecode( String const & str )
{
    thread_local MultiMatch mm( { "&amp;", "&lt;", "&gt;", "&nbsp;" }, { "&", "<", ">", " " } );
    return mm.replace(str);
}


static AnsiString const __Base64EncodeChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char const __Base64DecodeChars[] = 
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
};

WINUX_FUNC_IMPL(String) Base64Encode( void const * buf, size_t size )
{
    String outStr;
    unsigned char c1, c2, c3;
    size_t i = 0;
    char const * data = (char const *)buf;

    while ( i < size )
    {
        // read the first unsigned char
        c1 = data[i++];
        if ( i==size )       // pad with "="
        {
            outStr += __Base64EncodeChars[ c1>>2 ];
            outStr += __Base64EncodeChars[ (c1&0x3)<<4 ];
            outStr += "==";
            break;
        }

        // read the second unsigned char
        c2 = data[i++];
        if ( i==size )       // pad with "="
        {
            outStr += __Base64EncodeChars[ c1>>2 ];
            outStr += __Base64EncodeChars[ ((c1&0x3)<<4) | ((c2&0xF0)>>4) ];
            outStr += __Base64EncodeChars[ (c2&0xF)<<2 ];
            outStr += "=";
            break;
        }

        // read the third unsigned char
        c3 = data[i++];
        // convert into four bytes String
        outStr += __Base64EncodeChars[ c1>>2 ];
        outStr += __Base64EncodeChars[ ((c1&0x3)<<4) | ((c2&0xF0)>>4) ];
        outStr += __Base64EncodeChars[ ((c2&0xF)<<2) | ((c3&0xC0)>>6) ];
        outStr += __Base64EncodeChars[ c3&0x3F ];
    }

    return outStr;
}

WINUX_FUNC_IMPL(String) Base64Encode( AnsiString const & data )
{
    return Base64Encode( data.c_str(), data.length() );
}

WINUX_FUNC_IMPL(AnsiString) Base64Decode( String const & base64Str )
{
    AnsiString outData;
    char c1, c2, c3, c4;
    size_t i = 0;
    size_t len = base64Str.length();

    while ( i < len )
    {
        // read the first unsigned char
        do {
            c1 = __Base64DecodeChars[ base64Str[i++] ];
        } while ( i<len && c1==-1);

        if ( c1==-1)
            break;

        // read the second unsigned char
        do {
            c2 = __Base64DecodeChars[ base64Str[i++] ];
        } while ( i<len && c2==-1);

        if ( c2==-1 )
            break;

        // assamble the first unsigned char
        outData += char( (c1<<2) | ((c2&0x30)>>4) );

        // read the third unsigned char
        do {
            c3 = base64Str[i++];
            if ( c3==61 )       // meet with "=", break
                return outData;
            c3 = __Base64DecodeChars[ c3 ];
        } while ( i<len && c3==-1);

        if ( c3==-1 )
            break;

        // assamble the second unsigned char
        outData += char( ((c2&0XF)<<4) | ((c3&0x3C)>>2) );

        // read the fourth unsigned char
        do {
            c4 = base64Str[i++];
            if ( c4==61 )       // meet with "=", break
                return outData;
            c4 = __Base64DecodeChars[ c4 ];
        } while ( i<len && c4==-1 );

        if ( c4==-1 )
            break;

        // assamble the third unsigned char
        outData += char( ((c3&0x03)<<6) | c4 );
    }

    return outData;
}

WINUX_FUNC_IMPL(String) Md5Hex( void const * buf, size_t size )
{
    MD5 md5;
    md5.update( (char const *)buf, (unsigned int)size );
    md5.finalize();
    return md5.hexdigest();
}

WINUX_FUNC_IMPL(String) Sha1Hex( void const * buf, size_t size )
{
    Buffer result = Sha1( buf, size );
    winux::byte * digest = (winux::byte *)result.getBuf();

    char sz[SHA1_DIGEST_SIZE * 2 + 1];
    for ( int i = 0; i < SHA1_DIGEST_SIZE; i++ )
        sprintf( sz + i * 2, "%02x", digest[i] );
    sz[SHA1_DIGEST_SIZE * 2] = 0;

    return String(sz);
}

WINUX_FUNC_IMPL(Buffer) Sha1( void const * buf, size_t size )
{
    Buffer res;
    res.alloc(SHA1_DIGEST_SIZE);
    sha1_ctx ctx;
    sha1_init(&ctx);
    sha1_update( &ctx, buf, size );
    sha1_final( &ctx, (uint8_t *)res.getBuf() );
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

}// namespace winux
