
#include "Cassette.h"
#include "CassetteApp.h"
#include "AboutView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AboutView

IMPLEMENT_DYNCREATE(AboutView, CHtmlView)

AboutView::AboutView()
{
    //{{AFX_DATA_INIT(AboutView)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT

}

AboutView::~AboutView()
{
}

void AboutView::DoDataExchange( CDataExchange * pDX )
{
    CHtmlView::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(AboutView)
    // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AboutView, CHtmlView)
    //{{AFX_MSG_MAP(AboutView)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AboutView diagnostics

#ifdef _DEBUG
void AboutView::AssertValid() const
{
    CHtmlView::AssertValid();
}

void AboutView::Dump(CDumpContext& dc) const
{
    CHtmlView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// AboutView message handlers

BOOL AboutView::PreCreateWindow(CREATESTRUCT& cs) 
{
    if ( !CHtmlView::PreCreateWindow(cs) )
        return FALSE;

    cs.dwExStyle &= ~WS_EX_CLIENTEDGE;

    return TRUE;
}

void AboutView::OnInitialUpdate() 
{
    CHtmlView::OnInitialUpdate();

    this->LoadFromResource(IDR_HTML_ABOUT);
}

BOOL AboutView::PreTranslateMessage( MSG * pMsg ) 
{
    // 禁止鼠标显示I型指针，在HTML代码里设置body{ cursor: default; }的CSS即可
    //static HCURSOR curIbeam = LoadCursor( NULL, IDC_IBEAM );
    //if ( GetCursor() == curIbeam )
    //{
    //    static HCURSOR curArrow = LoadCursor( NULL, IDC_ARROW );
    //    SetCursor(curArrow);
    //}

    // 屏蔽右键菜单，左键双击
    switch ( pMsg->message )
    {
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_LBUTTONDBLCLK:
        return TRUE;
    }
 
    // 禁止选择
    if ( pMsg->message == WM_MOUSEMOVE )
    {
        if ( GetKeyState(VK_LBUTTON) & 0x8000 )
            return TRUE;
    }
    else if ( pMsg->message == WM_LBUTTONDOWN )
    {
        if ( GetAsyncKeyState(VK_CONTROL) & 0x8000 )
            return TRUE;
    }

    return CHtmlView::PreTranslateMessage(pMsg);
}

void AboutView::OnBeforeNavigate2( LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel )
{
    winplus::String strUrl = lpszURL;
    // 如果是外部网站则用shellexec打开
    if ( strUrl.length() > 3 && winplus::StrLower( strUrl.substr( 0, 4 ) ) == "http" )
    {
        ShellExecute( NULL, _T("open"), strUrl.c_str(), NULL, NULL, SW_NORMAL );

        *pbCancel = TRUE;
        return;
    }

    CHtmlView::OnBeforeNavigate2( lpszURL, nFlags, lpszTargetFrameName, baPostedData, lpszHeaders, pbCancel );
}
