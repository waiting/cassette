#ifndef __TIME_HPP__
#define __TIME_HPP__
//
// time 提供日期与时间相关的功能
//

namespace winux
{
/** \brief 获取几周是多少秒 */
inline static ulong GetSecondsFromWeeks( int weeks ) { return weeks * 7UL * 86400UL; }
/** \brief 获取几天是多少秒 */
inline static ulong GetSecondsFromDays( int days ) { return days * 86400UL; }
/** \brief 获取几小时是多少秒 */
inline static ulong GetSecondsFromHours( int hours ) { return hours * 3600UL; }
/** \brief 获取几分钟是多少秒 */
inline static ulong GetSecondsFromMinutes( int minutes ) { return minutes * 60UL; }

/** \brief 本地日期时间。L意思'local'，不包含时区信息 */
class WINUX_DLL DateTimeL
{
public:
    /** \brief 当前日期时间 */
    static DateTimeL Current();
    /** \brief 从秒数构建日期时间 */
    static DateTimeL FromSecond( time_t second );
    /** \brief 从毫秒数构建日期时间 */
    static DateTimeL FromMilliSec( uint64 millisec );
    /** \brief 从`struct tm`结构构建日期时间 */
    static DateTimeL FromTm( struct tm const * t );

    struct Second
    {
        time_t value;
        explicit Second( time_t second ) : value(second)
        {
        }
    };
    struct MilliSec
    {
        uint64 value;
        explicit MilliSec( uint64 millisec ) : value(millisec)
        {
        }
    };

    /** \brief 构造函数 */
    DateTimeL();
    /** \brief 构造函数 各参数 */
    DateTimeL( short year, short month, short day, short hour, short minute, short second, short millisec = 0 );
    /** \brief 构造函数 秒数 */
    DateTimeL( Second const & utcSecond );
    /** \brief 构造函数 毫秒数 */
    DateTimeL( MilliSec const & utcMillisec );
    /** \brief 构造函数 格式 xxxx-xx-xxTyy:yy:yy.zzz */
    explicit DateTimeL( String const & dateTimeStr );

    /** \brief 年[1~2038] */
    short getYear() const { return _year; }
    /** \brief 月[1~12] */
    short getMonth() const { return _month; }
    /** \brief 日[1~31] */
    short getDay() const { return _day; }
    /** \brief 时[0~23] */
    short getHour() const { return _hour; }
    /** \brief 分[0~59] */
    short getMinute() const { return _minute; }
    /** \brief 秒[0~59] */
    short getSecond() const { return _second; }
    /** \brief 毫秒[0~999] */
    short getMillisec() const { return _millisec; }
    /** \brief 星期几[0~6, 0=Sunday] */
    short getDayOfWeek() const { return _wday; }
    /** \brief 年第几日[1~366, 1=01-01] */
    short getDayOfYear() const { return _yday; }

    /** \brief 设置年[1~2038] */
    void setYear( short year ) { _year = year; }
    /** \brief 设置月[1~12] */
    void setMonth( short month ) { _month = month; }
    /** \brief 设置日[1~31] */
    void setDay( short day ) { _day = day; }
    /** \brief 设置时[0~23] */
    void setHour( short hour ) { _hour = hour; }
    /** \brief 设置分[0~59] */
    void setMinute( short minute ) { _minute = minute; }
    /** \brief 设置秒[0~59] */
    void setSecond( short second ) { _second = second; }
    /** \brief 设置毫秒[0~999] */
    void setMillisec( short millisec ) { _millisec = millisec; }

    /** \brief 转成UTC秒数 */
    time_t toUtcTime() const;
    /** \brief 转成UTC毫秒数 */
    uint64 toUtcTimeMs() const;

    template < typename _ChTy >
    XString<_ChTy> format( XString<_ChTy> const & fmt ) const;

    /** \brief 格式化转为时间串
     *
     *  \%Y - 年\n\%M - 月\n\%D - 日\n\%h - 时\n\%m - 分\n\%s - 秒\n\%i - 毫秒 */
    #if defined(_UNICODE) || defined(UNICODE)
    XString<wchar> format( XString<wchar> const & fmt ) const;
    #else
    XString<char> format( XString<char> const & fmt ) const;
    #endif

    /** \brief 转为时间串 */
    template <
    #if defined(_UNICODE) || defined(UNICODE)
        typename _ChTy = wchar
    #else
        typename _ChTy = char
    #endif
    >
    XString<_ChTy> toString() const;

    /** \brief 转为GMT时间串，可用在HTTP头里 */
    template <
    #if defined(_UNICODE) || defined(UNICODE)
        typename _ChTy = wchar
    #else
        typename _ChTy = char
    #endif
    >
    XString<_ChTy> toGmtString() const;

    /** \brief 从当前时间构建`DateTimeL` */
    DateTimeL & current();
    /** \brief 从`struct tm`结构构建`DateTimeL` */
    DateTimeL & fromTm( struct tm const * t );

private:
    short _millisec;//!< 毫秒 [0~999]
    short _second;  //!< 秒 [0~59]
    short _minute;  //!< 分 [0~59]
    short _hour;    //!< 时 [0~23]
    short _day;     //!< 日 [1~31]
    short _month;   //!< 月 [1~12]
    short _year;    //!< 年 [1970~2038]
    short _wday;    //!< 星期几[0~6,0=Sun]
    short _yday;    //!< 年第几日[1~366,1=January 1]
};

template <>
WINUX_FUNC_DECL(XString<char>) DateTimeL::format( XString<char> const & fmt ) const;
template <>
WINUX_FUNC_DECL(XString<wchar>) DateTimeL::format( XString<wchar> const & fmt ) const;

#if defined(_UNICODE) || defined(UNICODE)
inline XString<wchar> DateTimeL::format( XString<wchar> const & fmt ) const { return this->format<wchar>(fmt); }
#else
inline XString<char> DateTimeL::format( XString<char> const & fmt ) const { return this->format<char>(fmt); }
#endif

template <>
WINUX_FUNC_DECL(XString<char>) DateTimeL::toString() const;
template <>
WINUX_FUNC_DECL(XString<wchar>) DateTimeL::toString() const;

template <>
WINUX_FUNC_DECL(XString<char>) DateTimeL::toGmtString() const;
template <>
WINUX_FUNC_DECL(XString<wchar>) DateTimeL::toGmtString() const;

WINUX_FUNC_DECL(std::ostream &) operator << ( std::ostream & o, DateTimeL const & dt );
WINUX_FUNC_DECL(std::wostream &) operator << ( std::wostream & o, DateTimeL const & dt );

/** \brief 获取UTC时间毫秒数，UTC秒数可以直接除以1000，或者调用CRT的time(NULL) */
WINUX_FUNC_DECL(uint64) GetUtcTimeMs( void );
/** \brief 获取UTC时间微秒数，UTC秒数可以直接除以1000000，或者调用CRT的time(NULL) */
WINUX_FUNC_DECL(uint64) GetUtcTimeUs( void );
/** \brief 获取UTC时间秒数，或者调用CRT的time(NULL) */
WINUX_FUNC_DECL(time_t) GetUtcTime( void );



} // namespace winux

#endif // __TIME_HPP__
