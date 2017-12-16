//////////////////////////////////////////////////////////////////////////
// purpose: 时间相关
// author:  WT
//////////////////////////////////////////////////////////////////////////

#if !defined(__WINPLUS_TIME_HPP__)
#define __WINPLUS_TIME_HPP__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace winplus
{

/* 获得[1970-01-01 0:0:0]到当前的UTC时间,精确到毫秒 */
WINPLUS_FUNC_DECL(ULONGLONG) GetUtcTimeMs( void );
/* 获得[1970-01-01 0:0:0]到当前的UTC时间,精确到秒 */
WINPLUS_FUNC_DECL(UINT) GetUtcTime( void );
/* 通过一个[1970-1-1 0:0:0]至今的UTC毫秒数获取一个UTC SYSTEMTIME */
WINPLUS_FUNC_DECL(SYSTEMTIME) GetUtcSysTime( ULONGLONG utcTimeMs );
/* 通过一个[1970-1-1 0:0:0]至今的UTC毫秒数获取一个Local SYSTEMTIME */
WINPLUS_FUNC_DECL(SYSTEMTIME) GetLocalSysTime( ULONGLONG utcTimeMs );
/* 格式化时间,<yr><mo><dy><h><m><s><ms> */
WINPLUS_FUNC_DECL(String) FormatTime( String const & fmt, SYSTEMTIME const & time );

} // namespace winplus

#endif // !defined(__WINPLUS_TIME_HPP__)
