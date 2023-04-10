// Window.cpp : implementation file
//

#include "Cassette.h"
#include "Window.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// class Window ---------------------------------------------------------------------------
Window::Window() : _bAutoDelete(FALSE)
{
}

Window::~Window()
{
}

void Window::PostNcDestroy()
{
	if ( _bAutoDelete )
	{
		delete this;
	}
}

BEGIN_MESSAGE_MAP(Window, CWnd)
    //{{AFX_MSG_MAP(Window)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Window message handlers



// class SpwWindow ------------------------------------------------------------------------
std::map<HWND, CWnd *> SpwWindow::_hasDisplayed;

CWnd * SpwWindow::GetWnd( HWND hWnd )
{
    if ( _hasDisplayed.find( hWnd ) != _hasDisplayed.end() )
        return _hasDisplayed[hWnd];
    return NULL;
}

void SpwWindow::SetWnd( HWND hWnd, CWnd * pWnd )
{
    _hasDisplayed[hWnd] = pWnd;
}

void SpwWindow::DelWnd( CWnd * pWnd )
{
    for ( auto it = _hasDisplayed.begin(); it != _hasDisplayed.end(); ++it )
    {
        if ( it->second == pWnd )
        {
            _hasDisplayed.erase(it->first);
            break;
        }
    }
}

SpwWindow::SpwWindow( CWnd * pWndParent )
{
    // 加入维护表
    SetWnd( pWndParent->GetSafeHwnd(), this );
}

SpwWindow::~SpwWindow()
{
    DelWnd(this);
}

BEGIN_MESSAGE_MAP(SpwWindow, Window)

END_MESSAGE_MAP()

