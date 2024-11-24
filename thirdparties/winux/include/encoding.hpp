#ifndef __ENCODING_HPP__
#define __ENCODING_HPP__
//
// encoding 提供编码与解码相关的功能
//

namespace winux
{
/** \brief URL编码
 *
 * 除了-_.之外的所有非字母数字字符都将被替换成百分号（%）后跟两位十六进制数，空格则编码为加号（+）。
 * 此编码与WWW表单POST数据的编码方式是一样的，同时与application/x-www-form-urlencoded的媒体类型编码方式一样。
 * 由于历史原因，此编码在将空格编码为加号（+）方面与RFC1738编码不同。
 * 此函数便于将字符串编码并将其用于URL的请求部分，同时它还便于将变量传递给下一页。 */
template < typename _ChTy >
XString<_ChTy> UrlEncode( void const * buf, size_t size );
template <>
WINUX_FUNC_DECL(XString<char>) UrlEncode( void const * buf, size_t size );
template <>
WINUX_FUNC_DECL(XString<wchar>) UrlEncode( void const * buf, size_t size );
template <>
WINUX_FUNC_DECL(XString<char16>) UrlEncode( void const * buf, size_t size );
template <>
WINUX_FUNC_DECL(XString<char32>) UrlEncode( void const * buf, size_t size );

template < typename _ChTy >
inline XString<_ChTy> UrlEncode( AnsiString const & data ) { return UrlEncode<_ChTy>( data.c_str(), data.length() ); }
template < typename _ChTy >
inline XString<_ChTy> UrlEncodeBuffer( Buffer const & buf ) { return UrlEncode<_ChTy>( buf.getBuf(), buf.getSize() ); }

#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString UrlEncode( void const * buf, size_t size ) { return UrlEncode<wchar>( buf, size ); }
inline UnicodeString UrlEncode( AnsiString const & data ) { return UrlEncode<wchar>( data.c_str(), data.length() ); }
inline UnicodeString UrlEncodeBuffer( Buffer const & buf ) { return UrlEncode<wchar>( buf.getBuf(), buf.getSize() ); }
#else
inline AnsiString UrlEncode( void const * buf, size_t size ) { return UrlEncode<char>( buf, size ); }
inline AnsiString UrlEncode( AnsiString const & data ) { return UrlEncode<char>( data.c_str(), data.length() ); }
inline AnsiString UrlEncodeBuffer( Buffer const & buf ) { return UrlEncode<char>( buf.getBuf(), buf.getSize() ); }
#endif


/** \brief URL编码，与UrlEncode()配套 */
template < typename _ChTy >
AnsiString UrlDecode( XString<_ChTy> const & encodedStr );
template <>
WINUX_FUNC_DECL(AnsiString) UrlDecode( XString<char> const & encodedStr );
template <>
WINUX_FUNC_DECL(AnsiString) UrlDecode( XString<wchar> const & encodedStr );
template <>
WINUX_FUNC_DECL(AnsiString) UrlDecode( XString<char16> const & encodedStr );
template <>
WINUX_FUNC_DECL(AnsiString) UrlDecode( XString<char32> const & encodedStr );

/** \brief URL编码，与UrlEncodeBuffer()配套 */
template < typename _ChTy >
Buffer UrlDecodeBuffer( XString<_ChTy> const & encodedStr );
template <>
WINUX_FUNC_DECL(Buffer) UrlDecodeBuffer( XString<char> const & encodedStr );
template <>
WINUX_FUNC_DECL(Buffer) UrlDecodeBuffer( XString<wchar> const & encodedStr );
template <>
WINUX_FUNC_DECL(Buffer) UrlDecodeBuffer( XString<char16> const & encodedStr );
template <>
WINUX_FUNC_DECL(Buffer) UrlDecodeBuffer( XString<char32> const & encodedStr );

#if defined(_UNICODE) || defined(UNICODE)
inline AnsiString UrlDecode( UnicodeString const & encodedStr ) { return UrlDecode<wchar>(encodedStr); }
inline Buffer UrlDecodeBuffer( UnicodeString const & encodedStr ) { return UrlDecodeBuffer<wchar>(encodedStr); }
#else
inline AnsiString UrlDecode( AnsiString const & encodedStr ) { return UrlDecode<char>(encodedStr); }
inline Buffer UrlDecodeBuffer( AnsiString const & encodedStr ) { return UrlDecodeBuffer<char>(encodedStr); }
#endif


/** \brief URI组成部分编码，遵循RFC1738
 *
 * 除了-_.~之外的所有非字母数字字符都将被替换成百分号（%）后跟两位十六进制数。
 * 这是在RFC1738中描述的编码。 */
template < typename _ChTy >
XString<_ChTy> UriComponentEncode( void const * buf, size_t size );
template <>
WINUX_FUNC_DECL(XString<char>) UriComponentEncode( void const * buf, size_t size );
template <>
WINUX_FUNC_DECL(XString<wchar>) UriComponentEncode( void const * buf, size_t size );
template <>
WINUX_FUNC_DECL(XString<char16>) UriComponentEncode( void const * buf, size_t size );
template <>
WINUX_FUNC_DECL(XString<char32>) UriComponentEncode( void const * buf, size_t size );

template < typename _ChTy >
inline XString<_ChTy> UriComponentEncode( AnsiString const & data ) { return UriComponentEncode<_ChTy>( data.c_str(), data.length() ); }
template < typename _ChTy >
inline XString<_ChTy> UriComponentEncodeBuffer( Buffer const & buf ) { return UriComponentEncode<_ChTy>( buf.getBuf(), buf.getSize() ); }

#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString UriComponentEncode( void const * buf, size_t size ) { return UriComponentEncode<wchar>( buf, size ); }
inline UnicodeString UriComponentEncode( AnsiString const & data ) { return UriComponentEncode<wchar>( data.c_str(), data.length() ); }
inline UnicodeString UriComponentEncodeBuffer( Buffer const & buf ) { return UriComponentEncode<wchar>( buf.getBuf(), buf.getSize() ); }
#else
inline AnsiString UriComponentEncode( void const * buf, size_t size ) { return UriComponentEncode<char>( buf, size ); }
inline AnsiString UriComponentEncode( AnsiString const & data ) { return UriComponentEncode<char>( data.c_str(), data.length() ); }
inline AnsiString UriComponentEncodeBuffer( Buffer const & buf ) { return UriComponentEncode<char>( buf.getBuf(), buf.getSize() ); }
#endif


/** \brief URI组成部分编码，与UriComponentEncode()配套 */
template < typename _ChTy >
AnsiString UriComponentDecode( XString<_ChTy> const & encodedStr );
template <>
WINUX_FUNC_DECL(AnsiString) UriComponentDecode( XString<char> const & encodedStr );
template <>
WINUX_FUNC_DECL(AnsiString) UriComponentDecode( XString<wchar> const & encodedStr );
template <>
WINUX_FUNC_DECL(AnsiString) UriComponentDecode( XString<char16> const & encodedStr );
template <>
WINUX_FUNC_DECL(AnsiString) UriComponentDecode( XString<char32> const & encodedStr );

/** \brief URI组成部分编码，与UriComponentEncodeBuffer()配套 */
template < typename _ChTy >
Buffer UriComponentDecodeBuffer( XString<_ChTy> const & encodedStr );
template <>
WINUX_FUNC_DECL(Buffer) UriComponentDecodeBuffer( XString<char> const & encodedStr );
template <>
WINUX_FUNC_DECL(Buffer) UriComponentDecodeBuffer( XString<wchar> const & encodedStr );
template <>
WINUX_FUNC_DECL(Buffer) UriComponentDecodeBuffer( XString<char16> const & encodedStr );
template <>
WINUX_FUNC_DECL(Buffer) UriComponentDecodeBuffer( XString<char32> const & encodedStr );

#if defined(_UNICODE) || defined(UNICODE)
inline AnsiString UriComponentDecode( UnicodeString const & encodedStr ) { return UriComponentDecode<wchar>(encodedStr); }
inline Buffer UriComponentDecodeBuffer( UnicodeString const & encodedStr ) { return UriComponentDecodeBuffer<wchar>(encodedStr); }
#else
inline AnsiString UriComponentDecode( AnsiString const & encodedStr ) { return UriComponentDecode<char>(encodedStr); }
inline Buffer UriComponentDecodeBuffer( AnsiString const & encodedStr ) { return UriComponentDecodeBuffer<char>(encodedStr); }
#endif


/** \brief URL编码 */
WINUX_FUNC_DECL(String) UrlEncodeString( String const & str, bool asUtf8 = sizeof(String::value_type) == sizeof(wchar) );

/** \brief URL解码，与UrlEncodeString()配套 */
WINUX_FUNC_DECL(String) UrlDecodeString( String const & encodedStr, bool isUtf8 = sizeof(String::value_type) == sizeof(wchar) );


/** \brief URI组成部分编码，遵循RFC1738 */
WINUX_FUNC_DECL(String) UriComponentEncodeString( String const & str, bool asUtf8 = sizeof(String::value_type) == sizeof(wchar) );

/** \brief URI组成部分解码，与UriComponentEncodeString()配套 */
WINUX_FUNC_DECL(String) UriComponentDecodeString( String const & encodedStr, bool isUtf8 = sizeof(String::value_type) == sizeof(wchar) );


/** \brief HTML编码。作用字符:'&', '<', '>', ' ' */
WINUX_FUNC_DECL(String) HtmlEncode( String const & str );

/** \brief HTML解码。与HtmlEncode()配套 */
WINUX_FUNC_DECL(String) HtmlDecode( String const & str );


/** \brief Base64编码 */
template < typename _ChTy >
XString<_ChTy> Base64Encode( void const * buf, size_t size );
template <>
WINUX_FUNC_DECL(XString<char>) Base64Encode( void const * buf, size_t size );
template <>
WINUX_FUNC_DECL(XString<wchar>) Base64Encode( void const * buf, size_t size );
template <>
WINUX_FUNC_DECL(XString<char16>) Base64Encode( void const * buf, size_t size );
template <>
WINUX_FUNC_DECL(XString<char32>) Base64Encode( void const * buf, size_t size );

/** \brief Base64编码 */
template < typename _ChTy >
inline XString<_ChTy> Base64Encode( AnsiString const & data ) { return Base64Encode<_ChTy>( data.c_str(), data.length() ); }

/** \brief Base64编码 */
template < typename _ChTy >
inline XString<_ChTy> Base64EncodeBuffer( Buffer const & buf ) { return Base64Encode<_ChTy>( buf.getBuf(), buf.getSize() ); }

/** \brief Base64编码 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString Base64Encode( void const * buf, size_t size ) { return Base64Encode<wchar>( buf, size ); }
inline UnicodeString Base64Encode( AnsiString const & data ) { return Base64Encode<wchar>( data.c_str(), data.length() ); }
inline UnicodeString Base64EncodeBuffer( Buffer const & buf ) { return Base64Encode<wchar>( buf.getBuf(), buf.getSize() ); }
#else
inline AnsiString Base64Encode( void const * buf, size_t size ) { return Base64Encode<char>( buf, size ); }
inline AnsiString Base64Encode( AnsiString const & data ) { return Base64Encode<char>( data.c_str(), data.length() ); }
inline AnsiString Base64EncodeBuffer( Buffer const & buf ) { return Base64Encode<char>( buf.getBuf(), buf.getSize() ); }
#endif


/** \brief Base64解码，与Base64Encode()配套 */
template < typename _ChTy >
AnsiString Base64Decode( XString<_ChTy> const & base64Str );
template <>
WINUX_FUNC_DECL(AnsiString) Base64Decode( XString<char> const & base64Str );
template <>
WINUX_FUNC_DECL(AnsiString) Base64Decode( XString<wchar> const & base64Str );
template <>
WINUX_FUNC_DECL(AnsiString) Base64Decode( XString<char16> const & base64Str );
template <>
WINUX_FUNC_DECL(AnsiString) Base64Decode( XString<char32> const & base64Str );


/** \brief Base64解码，与Base64Encode()配套 */
template < typename _ChTy >
Buffer Base64DecodeBuffer( XString<_ChTy> const & base64Str );
template <>
WINUX_FUNC_DECL(Buffer) Base64DecodeBuffer( XString<char> const & base64Str );
template <>
WINUX_FUNC_DECL(Buffer) Base64DecodeBuffer( XString<wchar> const & base64Str );
template <>
WINUX_FUNC_DECL(Buffer) Base64DecodeBuffer( XString<char16> const & base64Str );
template <>
WINUX_FUNC_DECL(Buffer) Base64DecodeBuffer( XString<char32> const & base64Str );

/** \brief Base64解码，与Base64Encode()配套 */
#if defined(_UNICODE) || defined(UNICODE)
inline AnsiString Base64Decode( UnicodeString const & base64Str ) { return Base64Decode<wchar>(base64Str); }
inline Buffer Base64DecodeBuffer( UnicodeString const & base64Str ) { return Base64DecodeBuffer<wchar>(base64Str); }
#else
inline AnsiString Base64Decode( AnsiString const & base64Str ) { return Base64Decode<char>(base64Str); }
inline Buffer Base64DecodeBuffer( AnsiString const & base64Str ) { return Base64DecodeBuffer<char>(base64Str); }
#endif


/** \brief 二进制数据转成16进制串 */
template < typename _ChTy >
XString<_ChTy> BufferToHex( Buffer const & buf );
template <>
WINUX_FUNC_DECL(XString<char>) BufferToHex( Buffer const & buf );
template <>
WINUX_FUNC_DECL(XString<wchar>) BufferToHex( Buffer const & buf );
template <>
WINUX_FUNC_DECL(XString<char16>) BufferToHex( Buffer const & buf );
template <>
WINUX_FUNC_DECL(XString<char32>) BufferToHex( Buffer const & buf );

/** \brief 二进制数据转成16进制串 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString BufferToHex( Buffer const & buf ) { return BufferToHex<wchar>(buf); }
#else
inline AnsiString BufferToHex( Buffer const & buf ) { return BufferToHex<char>(buf); }
#endif


/** \brief 16进制串转成二进制数据 */
template < typename _ChTy >
Buffer HexToBuffer( XString<_ChTy> const & hexStr );
template <>
WINUX_FUNC_DECL(Buffer) HexToBuffer( XString<char> const & hexStr );
template <>
WINUX_FUNC_DECL(Buffer) HexToBuffer( XString<wchar> const & hexStr );
template <>
WINUX_FUNC_DECL(Buffer) HexToBuffer( XString<char16> const & hexStr );
template <>
WINUX_FUNC_DECL(Buffer) HexToBuffer( XString<char32> const & hexStr );

/** \brief 16进制串转成二进制数据 */
#if defined(_UNICODE) || defined(UNICODE)
inline Buffer HexToBuffer( UnicodeString const & hexStr ) { return HexToBuffer<wchar>(hexStr); }
#else
inline Buffer HexToBuffer( AnsiString const & hexStr ) { return HexToBuffer<char>(hexStr); }
#endif


/** \brief 将数据进行md5编码，返回二进制数据 */
WINUX_FUNC_DECL(Buffer) Md5( void const * buf, size_t size );
/** \brief 将数据进行md5编码，返回二进制数据 */
inline Buffer Md5( Buffer const & buf ) { return Md5( buf.getBuf(), buf.getSize() ); }

/** \brief 将数据进行md5编码，返回16进制数字符串 */
template < typename _ChTy >
inline XString<_ChTy> Md5Hex( void const * buf, size_t size ) { return BufferToHex<_ChTy>( Md5( buf, size ) ); }
/** \brief 将数据进行md5编码，返回16进制数字符串 */
template < typename _ChTy >
inline XString<_ChTy> Md5Hex( Buffer const & buf ) { return Md5Hex<_ChTy>( buf.getBuf(), buf.getSize() ); }

/** \brief 将数据进行md5编码，返回16进制数字符串 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString Md5Hex( void const * buf, size_t size ) { return BufferToHex<wchar>( Md5( buf, size ) ); }
inline UnicodeString Md5Hex( Buffer const & buf ) { return Md5Hex<wchar>( buf.getBuf(), buf.getSize() ); }
#else
inline AnsiString Md5Hex( void const * buf, size_t size ) { return BufferToHex<char>( Md5( buf, size ) ); }
inline AnsiString Md5Hex( Buffer const & buf ) { return Md5Hex<char>( buf.getBuf(), buf.getSize() ); }
#endif


/** \brief 将数据进行sha1编码，返回二进制数据 */
WINUX_FUNC_DECL(Buffer) Sha1( void const * buf, size_t size );
/** \brief 将数据进行sha1编码，返回二进制数据 */
inline Buffer Sha1( Buffer const & buf ) { return Sha1( buf.getBuf(), buf.getSize() ); }

/** \brief 将数据进行sha1编码，返回16进制数字符串 */
template < typename _ChTy >
inline XString<_ChTy> Sha1Hex( void const * buf, size_t size ) { return BufferToHex<_ChTy>( Sha1( buf, size ) ); }
/** \brief 将数据进行sha1编码，返回16进制数字符串 */
template < typename _ChTy >
inline XString<_ChTy> Sha1Hex( Buffer const & buf ) { return Sha1Hex<_ChTy>( buf.getBuf(), buf.getSize() ); }

/** \brief 将数据进行sha1编码，返回16进制数字符串 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString Sha1Hex( void const * buf, size_t size ) { return BufferToHex<wchar>( Sha1( buf, size ) ); }
inline UnicodeString Sha1Hex( Buffer const & buf ) { return Sha1Hex<wchar>( buf.getBuf(), buf.getSize() ); }
#else
inline AnsiString Sha1Hex( void const * buf, size_t size ) { return BufferToHex<char>( Sha1( buf, size ) ); }
inline AnsiString Sha1Hex( Buffer const & buf ) { return Sha1Hex<char>( buf.getBuf(), buf.getSize() ); }
#endif


/** \brief 将数据进行sha224编码，返回二进制数据 */
WINUX_FUNC_DECL(Buffer) Sha224( void const * buf, size_t size );
/** \brief 将数据进行sha224编码，返回二进制数据 */
inline Buffer Sha224( Buffer const & buf ) { return Sha224( buf.getBuf(), buf.getSize() ); }

/** \brief 将数据进行sha224编码，返回16进制数字符串 */
template < typename _ChTy >
inline XString<_ChTy> Sha224Hex( void const * buf, size_t size ) { return BufferToHex<_ChTy>( Sha224( buf, size ) ); }
/** \brief 将数据进行sha224编码，返回16进制数字符串 */
template < typename _ChTy >
inline XString<_ChTy> Sha224Hex( Buffer const & buf ) { return Sha224Hex<_ChTy>( buf.getBuf(), buf.getSize() ); }

/** \brief 将数据进行sha224编码，返回16进制数字符串 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString Sha224Hex( void const * buf, size_t size ) { return BufferToHex<wchar>( Sha224( buf, size ) ); }
inline UnicodeString Sha224Hex( Buffer const & buf ) { return Sha224Hex<wchar>( buf.getBuf(), buf.getSize() ); }
#else
inline AnsiString Sha224Hex( void const * buf, size_t size ) { return BufferToHex<char>( Sha224( buf, size ) ); }
inline AnsiString Sha224Hex( Buffer const & buf ) { return Sha224Hex<char>( buf.getBuf(), buf.getSize() ); }
#endif


/** \brief 将数据进行sha256编码，返回二进制数据 */
WINUX_FUNC_DECL(Buffer) Sha256( void const * buf, size_t size );
/** \brief 将数据进行sha256编码，返回二进制数据 */
inline Buffer Sha256( Buffer const & buf ) { return Sha256( buf.getBuf(), buf.getSize() ); }

/** \brief 将数据进行sha256编码，返回16进制数字符串 */
template < typename _ChTy >
inline XString<_ChTy> Sha256Hex( void const * buf, size_t size ) { return BufferToHex<_ChTy>( Sha256( buf, size ) ); }
/** \brief 将数据进行sha256编码，返回16进制数字符串 */
template < typename _ChTy >
inline XString<_ChTy> Sha256Hex( Buffer const & buf ) { return Sha256Hex<_ChTy>( buf.getBuf(), buf.getSize() ); }

/** \brief 将数据进行sha256编码，返回16进制数字符串 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString Sha256Hex( void const * buf, size_t size ) { return BufferToHex<wchar>( Sha256( buf, size ) ); }
inline UnicodeString Sha256Hex( Buffer const & buf ) { return Sha256Hex<wchar>( buf.getBuf(), buf.getSize() ); }
#else
inline AnsiString Sha256Hex( void const * buf, size_t size ) { return BufferToHex<char>( Sha256( buf, size ) ); }
inline AnsiString Sha256Hex( Buffer const & buf ) { return Sha256Hex<char>( buf.getBuf(), buf.getSize() ); }
#endif


/** \brief 将数据进行sha384编码，返回二进制数据 */
WINUX_FUNC_DECL(Buffer) Sha384( void const * buf, size_t size );
/** \brief 将数据进行sha384编码，返回二进制数据 */
inline Buffer Sha384( Buffer const & buf ) { return Sha384( buf.getBuf(), buf.getSize() ); }

/** \brief 将数据进行sha384编码，返回16进制数字符串 */
template < typename _ChTy >
inline XString<_ChTy> Sha384Hex( void const * buf, size_t size ) { return BufferToHex<_ChTy>( Sha384( buf, size ) ); }
/** \brief 将数据进行sha384编码，返回16进制数字符串 */
template < typename _ChTy >
inline XString<_ChTy> Sha384Hex( Buffer const & buf ) { return Sha384Hex<_ChTy>( buf.getBuf(), buf.getSize() ); }

/** \brief 将数据进行sha384编码，返回16进制数字符串 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString Sha384Hex( void const * buf, size_t size ) { return BufferToHex<wchar>( Sha384( buf, size ) ); }
inline UnicodeString Sha384Hex( Buffer const & buf ) { return Sha384Hex<wchar>( buf.getBuf(), buf.getSize() ); }
#else
inline AnsiString Sha384Hex( void const * buf, size_t size ) { return BufferToHex<char>( Sha384( buf, size ) ); }
inline AnsiString Sha384Hex( Buffer const & buf ) { return Sha384Hex<char>( buf.getBuf(), buf.getSize() ); }
#endif


/** \brief 将数据进行sha512编码，返回二进制数据 */
WINUX_FUNC_DECL(Buffer) Sha512( void const * buf, size_t size );
/** \brief 将数据进行sha512编码，返回二进制数据 */
inline Buffer Sha512( Buffer const & buf ) { return Sha512( buf.getBuf(), buf.getSize() ); }

/** \brief 将数据进行sha512编码，返回16进制数字符串 */
template < typename _ChTy >
inline XString<_ChTy> Sha512Hex( void const * buf, size_t size ) { return BufferToHex<_ChTy>( Sha512( buf, size ) ); }
/** \brief 将数据进行sha512编码，返回16进制数字符串 */
template < typename _ChTy >
inline XString<_ChTy> Sha512Hex( Buffer const & buf ) { return Sha512Hex<_ChTy>( buf.getBuf(), buf.getSize() ); }

/** \brief 将数据进行sha512编码，返回16进制数字符串 */
#if defined(_UNICODE) || defined(UNICODE)
inline UnicodeString Sha512Hex( void const * buf, size_t size ) { return BufferToHex<wchar>( Sha512( buf, size ) ); }
inline UnicodeString Sha512Hex( Buffer const & buf ) { return Sha512Hex<wchar>( buf.getBuf(), buf.getSize() ); }
#else
inline AnsiString Sha512Hex( void const * buf, size_t size ) { return BufferToHex<char>( Sha512( buf, size ) ); }
inline AnsiString Sha512Hex( Buffer const & buf ) { return Sha512Hex<char>( buf.getBuf(), buf.getSize() ); }
#endif


/** \brief 内容加密错误代码 */
enum EncryptErrorCode
{
    ERR_SUCCEED,
    ERR_DATASIZE,
    ERR_BUFFSIZE,
};

/** \brief 未进行C++包装的内容加密 */
WINUX_FUNC_DECL(int) RawEncrypt( byte const * data, size_t dataSize, byte * buf, size_t bufSize, size_t * outSize );
/** \brief 未进行C++包装的内容解密，与RawEncrypt()配套 */
WINUX_FUNC_DECL(int) RawDecrypt( byte const * encryptData, size_t dataSize, byte * buf, size_t bufSize, size_t * outSize );

/** \brief 内容加密 */
WINUX_FUNC_DECL(AnsiString) EncryptContent( AnsiString const & content );
/** \brief 内容加密 */
WINUX_FUNC_DECL(Buffer) EncryptContent( Buffer const & content );
/** \brief 内容解密，与EncryptContent()配套 */
WINUX_FUNC_DECL(AnsiString) DecryptContent( AnsiString const & encryptContent );
/** \brief 内容解密，与EncryptContent()配套 */
WINUX_FUNC_DECL(Buffer) DecryptContent( Buffer const & encryptContent );

#define EncryptCArray(data) EncryptContent( CBufferToAnsiString( data, sizeof(data) ) )
#define EncryptCBuffer( buf, size ) EncryptContent( CBufferToAnsiString( buf, size ) )
#define DecryptCArray(data) DecryptContent( CBufferToAnsiString( data, sizeof(data) ) )
#define DecryptCBuffer( buf, size ) DecryptContent( CBufferToAnsiString( buf, size ) )


} // namespace winux

#endif // __ENCODING_HPP__
