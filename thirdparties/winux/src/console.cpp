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
    "", "44", "42", "46", "41", "45", "43", "47", "40", "48",
};

// class ConsoleAttr ----------------------------------------------------------------------
ConsoleAttr::ConsoleAttr( winux::ushort attr, bool isSetBgColor ) : _isSetBgColor(isSetBgColor)
{
#if defined(OS_WIN)
    _wAttributes = attr;

    CONSOLE_SCREEN_BUFFER_INFO csbi = { 0 };
    _hStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo( _hStdHandle, &csbi );
    _wPrevAttributes = csbi.wAttributes;
#else
    if ( ( attr & 0x8000 ) == 0 )
    {
        union
        {
            winux::byte bAttr[2];
            winux::ushort usAttr;
        } tmp;
        tmp.usAttr = attr;

        this->_strAttr = __TerminalFgColorAttrs[tmp.bAttr[0]];
        if ( tmp.bAttr[1] )
        {
            this->_strAttr += ";";
            this->_strAttr += __TerminalBgColorAttrs[tmp.bAttr[1]];
        }
    }
#endif
}

void ConsoleAttr::modify() const
{
#if defined(OS_WIN)
    if ( ( _wAttributes & 0x8000 ) == 0 )
        SetConsoleTextAttribute( _hStdHandle, _wAttributes | ( _wPrevAttributes & ( _isSetBgColor ? 0xFF00 : 0xFFF0 ) ) );
#else
    if ( !this->_strAttr.empty() ) std::cout << "\033[" << this->_strAttr << "m";
#endif
}

void ConsoleAttr::resume() const
{
#if defined(OS_WIN)
    if ( ( _wAttributes & 0x8000 ) == 0 )
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
