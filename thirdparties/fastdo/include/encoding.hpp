﻿#ifndef __ENCODING_HPP__
#define __ENCODING_HPP__

namespace winux
{

/** \brief URL编码
 *
 * 除了-_.之外的所有非字母数字字符都将被替换成百分号（%）后跟两位十六进制数，空格则编码为加号（+）。
 * 此编码与WWW表单POST数据的编码方式是一样的，同时与application/x-www-form-urlencoded的媒体类型编码方式一样。
 * 由于历史原因，此编码在将空格编码为加号（+）方面与RFC1738编码不同。
 * 此函数便于将字符串编码并将其用于URL的请求部分，同时它还便于将变量传递给下一页。 */
WINUX_FUNC_DECL(String) UrlEncode( String const & str );

/** \brief URL解码，与UrlEncode()配套 */
WINUX_FUNC_DECL(String) UrlDecode( String const & str );

/** \brief URI组成部分编码，遵循RFC1738
 *
 * 除了-_.~之外的所有非字母数字字符都将被替换成百分号（%）后跟两位十六进制数。
 * 这是在RFC1738中描述的编码。 */
WINUX_FUNC_DECL(String) UriComponentEncode( String const & str );

/** \brief URI组成部分解码，与UriComponentEncode()配套 */
WINUX_FUNC_DECL(String) UriComponentDecode( String const & str );

/** \brief HTML编码 作用字符:'&', '<', '>', ' ' */
WINUX_FUNC_DECL(String) HtmlEncode( String const & str );

/** \brief HTML解码，与HtmlEncode()配套 */
WINUX_FUNC_DECL(String) HtmlDecode( String const & str );


/** \brief Base64编码 */
WINUX_FUNC_DECL(String) Base64Encode( AnsiString const & data );
/** \brief Base64解码，与Base64Encode()配套 */
WINUX_FUNC_DECL(AnsiString) Base64Decode( String const & base64Str );

/// \brief 内容加密错误代码
enum EncryptErrorCode
{
    ERR_SUCCEED,
    ERR_DATASIZE,
    ERR_BUFFSIZE,
};

/** \brief 未进行C++包装的内容加密 */
WINUX_FUNC_DECL(int) RawEncrypt( unsigned char const * data, int data_size, unsigned char * buf, int buf_size, int * out_size );
/** \brief 未进行C++包装的内容解密，与RawEncrypt()配套 */
WINUX_FUNC_DECL(int) RawDecrypt( unsigned char const * encrypt_data, int data_size, unsigned char * buf, int buf_size, int * out_size );

/** \brief 内容加密 */
WINUX_FUNC_DECL(AnsiString) EncryptContent( AnsiString const & content );
/** \brief 内容解密，与EncryptContent()配套 */
WINUX_FUNC_DECL(AnsiString) DecryptContent( AnsiString const & encryptContent );

#define EncryptCArray(data) EncryptContent( BufferToAnsiString( data, sizeof(data) ) )
#define EncryptBuffer( buf, size ) EncryptContent( BufferToAnsiString( buf, size ) )
#define DecryptCArray(data) DecryptContent( BufferToAnsiString( data, sizeof(data) ) )
#define DecryptBuffer( buf, size ) DecryptContent( BufferToAnsiString( buf, size ) )


} // namespace winux

#endif // __ENCODING_HPP__
