
#include "Cassette.h"
#include "AccountComprehensiveWnd.h"


// AccountComprehensiveWnd

IMPLEMENT_DYNAMIC(AccountComprehensiveWnd, CWnd)

AccountComprehensiveWnd::AccountComprehensiveWnd( CWnd * pParentWnd, winplus::String const & strWindowName, CRect const & rect )
{
#ifndef _WIN32_WCE
	EnableActiveAccessibility();
#endif

}

AccountComprehensiveWnd::~AccountComprehensiveWnd()
{
}


BEGIN_MESSAGE_MAP(AccountComprehensiveWnd, CWnd)
END_MESSAGE_MAP()



// AccountComprehensiveWnd 消息处理程序


