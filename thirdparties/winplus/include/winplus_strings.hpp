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
/* UTF8与兼容串之间的转换 */
WINPLUS_FUNC_DECL(String) Utf8ToString( AnsiString const & utf8 );
WINPLUS_FUNC_DECL(AnsiString) StringToUtf8( String const & str );

/* 使用逗号分割字符串 */
StringArray StrSplitComma( String const & str );
/* 使用分号分割字符串 */
StringArray StrSplitSemicolon( String const & str );

/* 载入字符串资源 */
WINPLUS_FUNC_DECL(UnicodeString) LoadStringExW( HMODULE module, UINT id );
WINPLUS_FUNC_DECL(String) LoadStringEx( HMODULE module, UINT id );
/* 采用GetModuleHandle(NULL)指定模块 */
WINPLUS_FUNC_DECL(String) LoadString( UINT id );

} // namespace winplus

#endif // !defined(__WINPLUS_STRINGS_HPP__)
