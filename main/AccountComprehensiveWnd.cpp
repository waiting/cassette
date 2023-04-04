
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
    CWnd::CreateEx(
        WS_EX_LAYERED,
        AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, NULL, hbrBkgnd ), strWindowName.c_str(), WS_POPUP | WS_CAPTION | WS_SYSMENU, rect, pParentWnd, 0, NULL );

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
    //g.SetSmoothingMode(SmoothingModeAntiAlias);

    rectClient.InflateRect(-8, -8);
    //g.DrawBackground( winplus::RectGdiToGdiplus<RectF>(rectClient), &SolidBrush( Color( 120, 0,0,0) ), 10 );
    int padding = 16;

    RectF rect = winplus::RectGdiToGdiplus<RectF>(rectClient);
    RectF rect1, rect2;
    rect1 = rect2 = rect;

    rect1.X += padding;
    rect1.Y += padding;
    rect1.Width -= padding;
    rect1.Height -= padding;
    //g.DrawBackground( rect1, &SolidBrush( Color( 16, 0, 0, 0 ) ), 1 );
    g.FillRectangle( &SolidBrush( Color( 16, 0, 0, 0 ) ), rect1 );
    //g.DrawShadowFrame( rect1, &Pen( Color(255, 204, 0), 4 ), nullptr, 1 );
    g.DrawRectangle( &Pen( Color(255, 204, 0), 1 ), rect1 );

    rect.Width -= padding;
    rect.Height -= padding;
    //g.DrawBackground( rect, &SolidBrush( Color( 16, 0, 0, 0 ) ), 1 );
    g.FillRectangle( &SolidBrush( Color( 16, 0, 0, 0 ) ), rect );
    //g.DrawShadowFrame( rect, &Pen( Color(51, 153, 255), 4 ), nullptr, 1 );
    g.DrawRectangle( &Pen( Color(51, 153, 255), 1 ), rect );

    rect2.Width = padding + 1;
    rect2.Height = padding + 1;
    rect2.X = rect1.GetRight() - rect2.Width + 1;
    rect2.Y = rect.GetTop();

    g.FillRectangle( &SolidBrush( Color( 255, 80, 175 ) ), rect2 );
    rect2.Inflate( -1, -1 );
    g.FillRectangle( &SolidBrush( Color( 255, 173, 218 ) ), rect2 );

    g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    StringFormat sf( StringFormat::GenericTypographic() );
    sf.SetLineAlignment(StringAlignmentCenter);

    g.DrawShadowString(
        winplus::Window_GetText(GetSafeHwnd()),
        Gdiplus::Font( L"微软雅黑", 12 ),
        &SolidBrush( Color( 0, 0x99, 0 ) ),
        &SolidBrush( Color( 32, 0, 0, 0 ) ),
        RectF( 8, 6, 200, 30 ),
        &sf,
        nullptr
    );


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
    //_memdc.copyEntireTo( dc, 0, 0 );

    //BLENDFUNCTION blend;
    //blend.BlendOp = AC_SRC_OVER;
    //blend.SourceConstantAlpha = 255;
    //blend.AlphaFormat = AC_SRC_ALPHA;

    //POINT ptPos = { 0, 0 };
    //SIZE sizeWnd = { _memdc.width(), _memdc.height() };

    //::UpdateLayeredWindow( GetSafeHwnd(), NULL, NULL, &sizeWnd, _memdc, &ptPos, 0, &blend, ULW_ALPHA );
}
