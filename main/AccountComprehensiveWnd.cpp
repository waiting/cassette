
#include "Cassette.h"
#include "AccountComprehensiveWnd.h"
#include "resource.h"

// AccountComprehensiveWnd

IMPLEMENT_DYNAMIC(AccountComprehensiveWnd, CWnd)

AccountComprehensiveWnd::AccountComprehensiveWnd( CWnd * pParentWnd, winplus::String const & strWindowName, CRect const & rect )
{
#ifndef _WIN32_WCE
    EnableActiveAccessibility();
#endif

    // 背景图画刷
    //HBRUSH hbrBkgnd = CreatePatternBrush( (HBITMAP)LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_INTEGRATEDBKIMG), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION ) );
    HBRUSH hbrBkgnd = CreatePatternBrush( (HBITMAP)LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_INTEGRATEDBKIMG), IMAGE_BITMAP, 0, 0, 0 ) );
    // 创建窗口
    CWnd::CreateEx( WS_EX_LAYERED, AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, NULL, 0 ), strWindowName.c_str(), WS_POPUP | WS_CAPTION | WS_SYSMENU, rect, pParentWnd, 0, NULL );

    std::cout << "AccountComprehensiveWnd()\n";
}

AccountComprehensiveWnd::~AccountComprehensiveWnd()
{

    std::cout << "~AccountComprehensiveWnd()\n";
}

BOOL AccountComprehensiveWnd::PreCreateWindow( CREATESTRUCT & cs )
{
    if ( !CWnd::PreCreateWindow(cs) ) return FALSE;
    cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
    cs.dwExStyle |= WS_EX_TOOLWINDOW;
    return TRUE;
}

void AccountComprehensiveWnd::PostNcDestroy()
{
    delete this;
}

BEGIN_MESSAGE_MAP(AccountComprehensiveWnd, CWnd)
    ON_WM_CREATE()
    ON_WM_PAINT()
END_MESSAGE_MAP()

// AccountComprehensiveWnd 消息处理程序

int AccountComprehensiveWnd::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    if ( CWnd::OnCreate( lpCreateStruct ) == -1 )
        return -1;

    // TODO:  在此添加您专用的创建代码
    CRect rectClient;
    GetClientRect(&rectClient);
    _memdc.create( nullptr, rectClient.Width(), rectClient.Height() );

    winplus::Graphics g(_memdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    rectClient.InflateRect(-8,-8);
    //g.DrawBackground( winplus::RectGdiToGdiplus<RectF>(rectClient), &SolidBrush( Color( 120, 0,0,0) ), 10 );
    RectF rect = winplus::RectGdiToGdiplus<RectF>(rectClient);
    RectF rect1, rect2;
    rect1 = rect2 = rect;

    rect1.X += 10;
    rect1.Y += 10;
    rect1.Width -= 10;
    rect1.Height -= 10;
    g.DrawBackground( rect1, &SolidBrush( Color( 16, 0, 0, 0 ) ), 1 );
    g.DrawShadowFrame( rect1, &Pen( Color(255, 204, 0), 4 ), nullptr, 1 );

    rect.Width -= 10;
    rect.Height -= 10;
    g.DrawBackground( rect, &SolidBrush( Color( 16, 0, 0, 0 ) ), 1 );
    g.DrawShadowFrame( rect, &Pen( Color(51, 153, 255), 4 ), nullptr, 1 );

    rect2.Width = 14;
    rect2.Height = 14;
    rect2.X = rect1.GetRight() - rect2.Width + 2;
    rect2.Y = rect.GetTop() - 2;

    g.DrawBackground(rect2, &SolidBrush( Color( 255, 255, 80, 175 ) ), 1);


    BLENDFUNCTION blend;
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    POINT ptPos = { 0, 0 };
    SIZE sizeWnd = { _memdc.width(), _memdc.height() };
    ::UpdateLayeredWindow( GetSafeHwnd(), NULL, NULL, &sizeWnd, _memdc, &ptPos, 0, &blend, ULW_ALPHA );

    return 0;
}

void AccountComprehensiveWnd::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    //_memdc.alphaEntireTo( dc, 0, 0, _memdc.width(), _memdc.height() );

    BLENDFUNCTION blend;
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    POINT ptPos = { 0, 0 };
    SIZE sizeWnd = { _memdc.width(), _memdc.height() };

    //HDC hScreenDC = GetDC(hWnd);
    //UpdateLayeredWindow( hWnd, hScreenDC, NULL, &sizeWnd, g_memdc, &ptPos, 0, &blend, ULW_ALPHA );
    ::UpdateLayeredWindow( GetSafeHwnd(), NULL, NULL, &sizeWnd, _memdc, &ptPos, 0, &blend, ULW_ALPHA );
}
