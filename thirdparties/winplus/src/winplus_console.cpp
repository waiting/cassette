#include "winplus_definitions.hpp"
#include "winplus_console.hpp"
#include "console.hpp"
#include <io.h>
#include <fcntl.h>

namespace winplus
{

WINPLUS_FUNC_IMPL(String) MakesureTextWidth( String const & text, int text_width, bool left_align, int ellipsis )
{
    if ( text.length() > text_width )
    {
        return text.substr( 0, text_width - ellipsis ) + String( ellipsis, '.' );
    }
    else
    {
        if ( left_align )
        {
            return text + String( text_width - text.length(), ' ' );
        }
        else
        {
            return String( text_width - text.length(), ' ' ) + text;
        }
    }
}

WINPLUS_FUNC_IMPL(void) PrintBarEx( String const & text, int text_width, int screen_width, WORD clr, int bar_blocks )
{
    std::cout << " ";
    std::cout << MakesureTextWidth( text, text_width, false, 2 );
    std::cout << " ";
    int i;
    int bar_width = screen_width - text_width - 1 - 1 - 1;
    int real_blocks = bar_width / 2;
    String tmp;
    for ( i = 0; i < real_blocks && i < bar_blocks; ++i )
    {
        tmp += "■";
    }

    std::cout << ConsoleColor( clr, tmp );
    std::cout << std::endl;
}

WINPLUS_FUNC_IMPL(void) Win32GUI_ShowConsole()
{
    AllocConsole(); // 分配一个控制台资源
    // 标准设备指针重定向
    freopen( "CONOUT$", "w+", stdout );
    freopen( "CONIN$", "r+", stdin );
    freopen( "CONERR$", "r+", stderr );
    //freopen( "CONOUT$", "wt+", stdout );
    //freopen( "CONIN$", "rt+", stdin );
    //freopen( "CONERR$", "rt+", stderr );


    //static const WORD MAX_CONSOLE_LINES = 1000;
    //int hConHandle;
    //intptr_t lStdHandle;
    //CONSOLE_SCREEN_BUFFER_INFO coninfo;
    //FILE *fp;

    //// allocate a console for this app
    //AllocConsole();

    //// set the screen buffer to be big enough to let us scroll text
    //GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    //coninfo.dwSize.Y = MAX_CONSOLE_LINES;
    //SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

    //// redirect unbuffered STDOUT to the console
    //lStdHandle = (intptr_t)GetStdHandle(STD_OUTPUT_HANDLE);
    //hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    //fp = _fdopen( hConHandle, "w" );
    //*stdout = *fp;
    //setvbuf( stdout, NULL, _IONBF, 0 );

    //// redirect unbuffered STDIN to the console
    //lStdHandle = (intptr_t)GetStdHandle(STD_INPUT_HANDLE);
    //hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    //fp = _fdopen( hConHandle, "r" );
    //*stdin = *fp;
    //setvbuf( stdin, NULL, _IONBF, 0 );

    //// redirect unbuffered STDERR to the console
    //lStdHandle = (intptr_t)GetStdHandle(STD_ERROR_HANDLE);
    //hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    //fp = _fdopen( hConHandle, "w" );
    //*stderr = *fp;
    //setvbuf( stderr, NULL, _IONBF, 0 );

    //// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
    //// point to console as well
    //std::ios::sync_with_stdio();
}

} // namespace winplus
