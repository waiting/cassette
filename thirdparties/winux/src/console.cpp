#include "utilities.hpp"
#include "smartptr.hpp"
#include "system.hpp"
#include "threads.hpp"
#include "console.hpp"

namespace winux
{

static char const * __TerminalFgColorAttrs[] = {
    "30", "34", "32", "36", "31", "35", "33", "37",
    "1;30", "1;34", "1;32", "1;36", "1;31", "1;35", "1;33", "1;37",
};

static char const * __TerminalBgColorAttrs[] = {
    "", "40", "44", "42", "46", "41", "45", "43", "47",
};

// class ConsoleAttr --------------------------------------------------------------------------
ConsoleAttr::ConsoleAttr( winux::ushort attr ) : _attr(attr)
{
#if defined(OS_WIN)
    CONSOLE_SCREEN_BUFFER_INFO csbi = { 0 };
    _hStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo( _hStdHandle, &csbi );
    _wPrevAttributes = csbi.wAttributes;
    _wAttributes = _wPrevAttributes;
    if ( ( _attr & fgIgnore ) != fgIgnore ) // 若不忽略前景色
    {
        _wAttributes = ( _wAttributes & 0xF0 ) | ( _attr & 0x0F );
    }
    if ( ( _attr & bgIgnore ) != bgIgnore ) // 若不忽略背景色
    {
        _wAttributes = ( _wAttributes & 0x0F ) | ( _attr & 0xF0 );
    }
#else
    winux::byte fg = ( _attr & 0x0F );
    winux::byte bg = ( ( _attr >> 4 ) & 0x0F );
    if ( ( _attr & fgIgnore ) != fgIgnore ) // 若不忽略前景色
    {
        this->_strAttr = __TerminalFgColorAttrs[fg];
    }
    if ( ( _attr & bgIgnore ) != bgIgnore ) // 若不忽略背景色
    {
        if ( bg )
        {
            if ( !this->_strAttr.empty() ) this->_strAttr += ";";
            this->_strAttr += __TerminalBgColorAttrs[bg];
        }
    }
#endif
}

void ConsoleAttr::modify() const
{
#if defined(OS_WIN)
    if ( ( _wAttributes & ccaIgnore ) != ccaIgnore )
        SetConsoleTextAttribute( _hStdHandle, _wAttributes );
#else
    if ( !this->_strAttr.empty() ) std::cout << "\033[" << this->_strAttr << "m";
#endif
}

void ConsoleAttr::resume() const
{
#if defined(OS_WIN)
    if ( ( _wAttributes & ccaIgnore ) != ccaIgnore )
        SetConsoleTextAttribute( _hStdHandle, _wPrevAttributes );
#else
    if ( !this->_strAttr.empty() ) std::cout << "\033[0m";
#endif
}

// class ConsoleOuputMutexScopeGuard ----------------------------------------------------------
static winux::MutexNative __mtxConsoleOutput;
ConsoleOuputMutexScopeGuard::ConsoleOuputMutexScopeGuard()
{
    __mtxConsoleOutput.lock();
}

ConsoleOuputMutexScopeGuard::~ConsoleOuputMutexScopeGuard()
{
    __mtxConsoleOutput.unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////

WINUX_FUNC_IMPL(String) ConsoleTextLayout( String const & text, size_t maxLength, ConsoleTextLayoutAlignment align, String::value_type blankCh, bool isTrunc, size_t ellipsis )
{
    if ( text.length() > maxLength )
    {
        return isTrunc ? text.substr( 0, maxLength - ellipsis ) + String( ellipsis, '.' ) : text;
    }
    else
    {
        switch ( align )
        {
        case ctlaLeft:
            return text + String( maxLength - text.length(), blankCh );
            break;
        case ctlaCenter:
            {
                size_t offset = ( maxLength - text.length() ) / 2;
                size_t pad = maxLength - text.length() - offset * 2;
                return String( offset, blankCh ) + text + String( offset + pad, blankCh );
            }
            break;
        case ctlaRight:
            return String( maxLength - text.length(), blankCh ) + text;
            break;
        default:
            return text;
        }
    }
}


} // namespace winux
