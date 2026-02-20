#include "utilities.hpp"
#include "strings.hpp"
#include "time.hpp"
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <string.h>

#ifdef __GNUC__
#include <sys/time.h>
#endif

#if defined(OS_WIN)
    #include <mbstring.h>
    #include <tchar.h>

#else
    #ifdef UNICODE
        #define _stscanf swscanf
    #else
        #define _stscanf sscanf
    #endif

#endif

namespace winux
{
// class DateTimeL ----------------------------------------------------------------------------
DateTimeL DateTimeL::Current()
{
    return DateTimeL().current();
}

DateTimeL DateTimeL::FromSecond( time_t second )
{
    return DateTimeL( Second(second) );
}

DateTimeL DateTimeL::FromMilliSec( uint64 millisec )
{
    return DateTimeL( MilliSec(millisec) );
}

DateTimeL DateTimeL::FromTm( struct tm const * t )
{
    return DateTimeL().fromTm(t);
}

// Constructors
DateTimeL::DateTimeL() : _millisec(0), _second(0), _minute(0), _hour(0), _day(0), _month(0), _year(0), _wday(0), _yday(0)
{

}

DateTimeL::DateTimeL( short year, short month, short day, short hour, short minute, short second, short millisec ) :
    _millisec(millisec), _second(second), _minute(minute), _hour(hour), _day(day), _month(month), _year(year), _wday(0), _yday(0)
{
    struct tm t = { 0 };
    t.tm_year = _year - 1900;
    t.tm_mon = _month - 1;
    t.tm_mday = _day;
    t.tm_hour = _hour;
    t.tm_min = _minute;
    t.tm_sec = _second;
    mktime(&t); // 本地时间结构体转到UTC时间值，并会填充tm_wday、tm_yday成员

    _wday = t.tm_wday;
    _yday = t.tm_yday + 1;
}

DateTimeL::DateTimeL( Second const & utcSecond ) : _millisec(0), _second(0), _minute(0), _hour(0), _day(0), _month(0), _year(0), _wday(0), _yday(0)
{
    time_t tt = utcSecond.value;
    struct tm * ptm = localtime(&tt); // 获取本地时间结构体

    _yday = ptm->tm_yday + 1;
    _wday = ptm->tm_wday;

    _year = ptm->tm_year + 1900;
    _month = ptm->tm_mon + 1;
    _day = ptm->tm_mday;

    _hour = ptm->tm_hour;
    _minute = ptm->tm_min;
    _second = ptm->tm_sec;
}

DateTimeL::DateTimeL( MilliSec const & utcMillisec ) : _millisec(0), _second(0), _minute(0), _hour(0), _day(0), _month(0), _year(0), _wday(0), _yday(0)
{
    _millisec = utcMillisec.value % 1000;

    time_t tt = utcMillisec.value / 1000;
    struct tm * ptm = localtime(&tt); // 获取本地时间结构体

    _yday = ptm->tm_yday + 1;
    _wday = ptm->tm_wday;

    _year = ptm->tm_year + 1900;
    _month = ptm->tm_mon + 1;
    _day = ptm->tm_mday;

    _hour = ptm->tm_hour;
    _minute = ptm->tm_min;
    _second = ptm->tm_sec;
}

DateTimeL::DateTimeL( String const & dateTimeStr ) : _millisec(0), _second(0), _minute(0), _hour(0), _day(0), _month(0), _year(0), _wday(0), _yday(0)
{
    _stscanf( dateTimeStr.c_str(), $T("%04hu-%02hu-%02huT%02hu:%02hu:%02hu.%03hu"), &_year, &_month, &_day, &_hour, &_minute, &_second, &_millisec );

    struct tm t = { 0 };
    t.tm_year = _year - 1900;
    t.tm_mon = _month - 1;
    t.tm_mday = _day;
    t.tm_hour = _hour;
    t.tm_min = _minute;
    t.tm_sec = _second;
    mktime(&t); // 本地时间结构体转到UTC时间值，并会填充tm_wday、tm_yday成员

    _wday = t.tm_wday;
    _yday = t.tm_yday + 1;
}

time_t DateTimeL::toUtcTime() const
{
    struct tm t = { 0 };
    t.tm_year = _year - 1900;
    t.tm_mon = _month - 1;
    t.tm_mday = _day;
    t.tm_hour = _hour;
    t.tm_min = _minute;
    t.tm_sec = _second;
    return mktime(&t);
}

uint64 DateTimeL::toUtcTimeMs() const
{
    uint64 tt = this->toUtcTime();
    tt = tt * 1000 + _millisec;
    return tt;
}

// 整数输出，若不足位数则用0填充
template < typename _ChTy, typename _IntType >
inline static XString<_ChTy> Impl_ZeroFillIntegerString( _IntType v, ssize_t zeroFill )
{
    _ChTy r[24] = { 0 };
    ssize_t i = countof(r) - 2;
    bool negative = v < 0;
    v = negative ? -v : v;
    if ( !v )
    {
        for ( ; i >= 0 && zeroFill != 0; i--, zeroFill-- )
        {
            r[i] = Literal<_ChTy>::zeroChar;
        }
    }
    else
    {
        while ( v && i >= 0 )
        {
            _IntType iN = v % 10;
            r[i--] = iN + Literal<_ChTy>::zeroChar;
            v /= 10;
        }
        // 填充0
        ssize_t n = countof(r) - 2 - i;
        for ( ; n < zeroFill && i >= 0; n++ )
        {
            r[i--] = Literal<_ChTy>::zeroChar;
        }
        if ( negative && i >= 0 ) r[i--] = Literal<_ChTy>::negativeChar;
    }
    return r + i + 1;
}

// 日期时间格式化输出
template < typename _ChTy >
inline static XString<_ChTy> Impl_DateTimeFormat( DateTimeL const & dtl, XString<_ChTy> const & fmt )
{
    XString<_ChTy> r;
    size_t i = 0;
    while ( i < fmt.length() )
    {
        _ChTy ch = fmt[i];
        if ( ch == (_ChTy)'%' )
        {
            if ( i + 1 < fmt.length() )
            {
                i++; // skip '%'
                switch ( fmt[i] )
                {
                case (_ChTy)'Y':
                    r += Impl_ZeroFillIntegerString<_ChTy>( dtl.getYear(), 4 );
                    i++;
                    break;
                case (_ChTy)'M':
                    r += Impl_ZeroFillIntegerString<_ChTy>( dtl.getMonth(), 2 );
                    i++;
                    break;
                case (_ChTy)'D':
                    r += Impl_ZeroFillIntegerString<_ChTy>( dtl.getDay(), 2 );
                    i++;
                    break;
                case (_ChTy)'h':
                    r += Impl_ZeroFillIntegerString<_ChTy>( dtl.getHour(), 2 );
                    i++;
                    break;
                case (_ChTy)'m':
                    r += Impl_ZeroFillIntegerString<_ChTy>( dtl.getMinute(), 2 );
                    i++;
                    break;
                case (_ChTy)'s':
                    r += Impl_ZeroFillIntegerString<_ChTy>( dtl.getSecond(), 2 );
                    i++;
                    break;
                case (_ChTy)'i':
                    r += Impl_ZeroFillIntegerString<_ChTy>( dtl.getMillisec(), 3 );
                    i++;
                    break;
                case (_ChTy)'%':
                    r += (_ChTy)'%';
                    i++;
                    break;
                }
            }
            else
            {
                r += ch;
                i++;
            }
        }
        else
        {
            r += ch;
            i++;
        }
    }

    return r;
}

template <>
WINUX_FUNC_IMPL(XString<char>) DateTimeL::format( XString<char> const & fmt ) const
{
    return Impl_DateTimeFormat( *this, fmt );
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) DateTimeL::format( XString<wchar> const & fmt ) const
{
    return Impl_DateTimeFormat( *this, fmt );
}

template <>
WINUX_FUNC_IMPL(XString<char>) DateTimeL::toString() const
{
    //return FormatA( "%04hu-%02hu-%02huT%02hu:%02hu:%02hu.%03hu", _year, _month, _day, _hour, _minute, _second, _millisec );
    return this->format<char>("%Y-%M-%DT%h:%m:%s.%i");
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) DateTimeL::toString() const
{
    //return FormatW( L"%04hu-%02hu-%02huT%02hu:%02hu:%02hu.%03hu", _year, _month, _day, _hour, _minute, _second, _millisec );
    return this->format<wchar>(L"%Y-%M-%DT%h:%m:%s.%i");
}

template <>
WINUX_FUNC_IMPL(XString<char>) DateTimeL::toGmtString() const
{
    time_t tt = this->toUtcTime();
    struct tm * t = gmtime(&tt); // 获取格林尼治时间结构体
    char sz[64] = { 0 };
    thread_local char const * months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    thread_local char const * weeks[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    sprintf(
        sz,
        "%s, %02u %s %04u %02u:%02u:%02u GMT",
        weeks[t->tm_wday],
        t->tm_mday,
        months[t->tm_mon],
        t->tm_year + 1900,
        t->tm_hour,
        t->tm_min,
        t->tm_sec
    );
    return sz;
}

template <>
WINUX_FUNC_IMPL(XString<wchar>) DateTimeL::toGmtString() const
{
    time_t tt = this->toUtcTime();
    struct tm * t = gmtime(&tt); // 获取格林尼治时间结构体
    wchar sz[64] = { 0 };
    thread_local wchar const * months[] = { L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun", L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec" };
    thread_local wchar const * weeks[] = { L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat" };
    swprintf(
        sz,
    #if defined(OS_WIN)
    #else
        64,
    #endif
        L"%s, %02u %s %04u %02u:%02u:%02u GMT",
        weeks[t->tm_wday],
        t->tm_mday,
        months[t->tm_mon],
        t->tm_year + 1900,
        t->tm_hour,
        t->tm_min,
        t->tm_sec
    );
    return sz;
}

DateTimeL & DateTimeL::current()
{
    struct timeb tbuf = { 0 };
    ftime(&tbuf);

    struct tm * ptm = localtime(&tbuf.time); // 获取本地时间结构体
    _yday = ptm->tm_yday + 1;
    _wday = ptm->tm_wday;

    _year = ptm->tm_year + 1900;
    _month = ptm->tm_mon + 1;
    _day = ptm->tm_mday;

    _hour = ptm->tm_hour;
    _minute = ptm->tm_min;
    _second = ptm->tm_sec;

    _millisec = tbuf.millitm;
    return *this;
}

DateTimeL & DateTimeL::fromTm( struct tm const * t )
{
    _yday = t->tm_yday + 1;
    _wday = t->tm_wday;

    _year = t->tm_year + 1900;
    _month = t->tm_mon + 1;
    _day = t->tm_mday;

    _hour = t->tm_hour;
    _minute = t->tm_min;
    _second = t->tm_sec;

    _millisec = 0;

    return *this;
}

WINUX_FUNC_IMPL(std::ostream &) operator << ( std::ostream & o, DateTimeL const & dt )
{
    o << dt.toString<char>();
    return o;
}

WINUX_FUNC_IMPL(std::wostream &) operator << ( std::wostream & o, DateTimeL const & dt )
{
    o << dt.toString<wchar>();
    return o;
}

#if defined(OS_WIN)

// 1970-01-01 00:00:00的ULARGE_INTEGER描述
static ULARGE_INTEGER __Time1970( void )
{
    SYSTEMTIME st1970 = {0};
    st1970.wYear = 1970;
    st1970.wMonth = 1;
    st1970.wDay = 1;
    st1970.wHour = 0;
    st1970.wMinute = 0;
    st1970.wSecond = 0;

    FILETIME ft1970;
    ULARGE_INTEGER time1970;
    SystemTimeToFileTime( &st1970, &ft1970 );
    CopyMemory( &time1970, &ft1970, sizeof(time1970) );
    return time1970;
}

WINUX_FUNC_IMPL(uint64) GetUtcTimeMs( void )
{
    FILETIME ft;
    ULARGE_INTEGER time;
    GetSystemTimeAsFileTime(&ft);
    CopyMemory( &time, &ft, sizeof(time) );
    time.QuadPart -= __Time1970().QuadPart;
    return time.QuadPart / 10000;
}

WINUX_FUNC_IMPL(uint64) GetUtcTimeUs( void )
{
    FILETIME ft;
    ULARGE_INTEGER time;
    GetSystemTimeAsFileTime(&ft);
    CopyMemory( &time, &ft, sizeof(time) );
    time.QuadPart -= __Time1970().QuadPart;
    return time.QuadPart / 10;
}

#else

WINUX_FUNC_IMPL(uint64) GetUtcTimeMs( void )
{
    uint64 nMillSec = 0;
    struct timeval tv;
    gettimeofday( &tv, NULL );
    nMillSec = (uint64)tv.tv_sec * 1000;
    nMillSec += tv.tv_usec / 1000;
    return nMillSec;
}

WINUX_FUNC_IMPL(uint64) GetUtcTimeUs( void )
{
    uint64 nMicroSec = 0;
    struct timeval tv;
    gettimeofday( &tv, NULL );
    nMicroSec = (uint64)tv.tv_sec * 1000000;
    nMicroSec += tv.tv_usec;
    return nMicroSec;
}

#endif

WINUX_FUNC_IMPL(time_t) GetUtcTime( void )
{
    return time(NULL);
}


} // namespace winux
