
#include "Cassette.h"
#include "AccountComprehensiveWnd.h"
#include "resource.h"

// AccountComprehensiveWnd

IMPLEMENT_DYNAMIC(AccountComprehensiveWnd, CWnd)

struct AccountComprehensive_Data
{
    // 画布
    winplus::MemDC memCanvas;
    // 窗口大小矩形
    RectF rectWindow;
    // 客户区大小矩形
    RectF rectClient;
    // 右上角关闭按钮区域
    GraphicsPath closeBtnPath;
};

AccountComprehensiveWnd::AccountComprehensiveWnd( CWnd * pParentWnd, winplus::String const & strWindowName, CRect const & rect )
{
    _self.create();

#ifndef _WIN32_WCE
    EnableActiveAccessibility();
#endif

    // 背景图画刷
    //HBRUSH hbrBkgnd = CreatePatternBrush( (HBITMAP)LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_INTEGRATEDBKIMG), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION ) );
    HBRUSH hbrBkgnd = CreatePatternBrush( (HBITMAP)LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_INTEGRATEDBKIMG), IMAGE_BITMAP, 0, 0, 0 ) );
    // 创建窗口
    CWnd::CreateEx(
        WS_EX_LAYERED,
        AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, NULL, hbrBkgnd ),
        strWindowName.c_str(),
        WS_POPUP /*| WS_CAPTION | WS_SYSMENU*/,
        rect,
        pParentWnd,
        0,
        NULL
    );

    std::cout << "AccountComprehensiveWnd()\n";
}

AccountComprehensiveWnd::~AccountComprehensiveWnd()
{
    std::cout << "~AccountComprehensiveWnd()\n";

    _self.destroy();
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
    ON_WM_NCHITTEST()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// AccountComprehensiveWnd 消息处理程序

int AccountComprehensiveWnd::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    if ( CWnd::OnCreate( lpCreateStruct ) == -1 )
        return -1;

    CRect rectWnd;
    GetWindowRect(&rectWnd);
    _self->rectWindow = winplus::RectGdiToGdiplus<RectF>(rectWnd);
    std::cout << rectWnd << "\n";

    // 从图片载入
    Bitmap bmpBkgnd( winplus::Resource( IDR_PNG_ACCOUNT_COMPREHENSIVE_WND, _T("PNG") ).createStream(), FALSE );
    _self->memCanvas.create( NULL, rectWnd.Width(), rectWnd.Height() );
    winplus::Graphics g(_self->memCanvas);

    //g.SetSmoothingMode(SmoothingModeAntiAlias);
    //g.DrawImage( &bmpBkgnd, 0, 0, bmpBkgnd.GetWidth(), bmpBkgnd.GetHeight() );
    constexpr int lBorder = 8;
    constexpr int rBorder = 8;
    constexpr int tBorder = 23;
    constexpr int bBorder = 8;

    _self->rectClient = RectF( lBorder, tBorder, _self->memCanvas.width() - lBorder - rBorder, _self->memCanvas.height() - tBorder - bBorder );
    g.FillRectangle( &SolidBrush( Color( 96, 255, 255, 255 ) ), _self->rectClient );

    TextureBrush ltBrush{ &bmpBkgnd, WrapModeTile, RectF( 0, 0, lBorder, tBorder ) };
    RectF ltCornerRect( 0, 0, lBorder, tBorder );
    ltBrush.TranslateTransform( ltCornerRect.GetLeft(), ltCornerRect.GetTop() );
    g.FillRectangle( &ltBrush, ltCornerRect );

    TextureBrush rtBrush{ &bmpBkgnd, WrapModeTile, RectF( bmpBkgnd.GetWidth() - rBorder, 0, rBorder, tBorder ) };
    RectF rtCornerRect( _self->memCanvas.width() - rBorder, 0, rBorder, tBorder );
    rtBrush.TranslateTransform( rtCornerRect.GetLeft(), rtCornerRect.GetTop() );
    g.FillRectangle( &rtBrush, rtCornerRect );

    TextureBrush rbBrush{ &bmpBkgnd, WrapModeTile, RectF( bmpBkgnd.GetWidth() - rBorder, bmpBkgnd.GetHeight() - bBorder, rBorder, bBorder ) };
    RectF rbCornerRect( _self->memCanvas.width() - rBorder, _self->memCanvas.height() - bBorder, rBorder, bBorder );
    rbBrush.TranslateTransform( rbCornerRect.GetLeft(), rbCornerRect.GetTop() );
    g.FillRectangle( &rbBrush, rbCornerRect );

    TextureBrush lbBrush{ &bmpBkgnd, WrapModeTile, RectF( 0, bmpBkgnd.GetHeight() - bBorder, lBorder, bBorder ) };
    RectF lbCornerRect( 0, _self->memCanvas.height() - bBorder, lBorder, bBorder );
    lbBrush.TranslateTransform( lbCornerRect.GetLeft(), lbCornerRect.GetTop() );
    g.FillRectangle( &lbBrush, lbCornerRect );

    TextureBrush lBrush{ &bmpBkgnd, WrapModeTile, RectF( 0, tBorder, lBorder, 1 ) };
    RectF lEdgeRect( 0, tBorder, lBorder, _self->memCanvas.height() - tBorder - bBorder );
    lBrush.TranslateTransform( lEdgeRect.GetLeft(), lEdgeRect.GetTop() );
    g.FillRectangle( &lBrush, lEdgeRect );

    TextureBrush tBrush{ &bmpBkgnd, WrapModeTile, RectF( lBorder, 0, 1, tBorder ) };
    RectF tEdgeRect( lBorder, 0, _self->memCanvas.width() - lBorder - rBorder, tBorder );
    tBrush.TranslateTransform( tEdgeRect.GetLeft(), tEdgeRect.GetTop() );
    g.FillRectangle( &tBrush, tEdgeRect );

    TextureBrush rBrush{ &bmpBkgnd, WrapModeTile, RectF( bmpBkgnd.GetWidth() - rBorder, tBorder, rBorder, 1 ) };
    RectF rEdgeRect( _self->memCanvas.width() - rBorder, tBorder, rBorder, _self->memCanvas.height() - tBorder - bBorder );
    rBrush.TranslateTransform( rEdgeRect.GetLeft(), rEdgeRect.GetTop() );
    g.FillRectangle( &rBrush, rEdgeRect );

    TextureBrush bBrush{ &bmpBkgnd, WrapModeTile, RectF( lBorder, bmpBkgnd.GetHeight() - bBorder, 1, bBorder ) };
    RectF bEdgeRect( lBorder, _self->memCanvas.height() - bBorder, _self->memCanvas.width() - lBorder - rBorder, bBorder );
    bBrush.TranslateTransform( bEdgeRect.GetLeft(), bEdgeRect.GetTop() );
    g.FillRectangle( &bBrush, bEdgeRect );


    SolidBrush closeBtnBrush( Color( 255, 204, 0 ) );
    RectF closeBtnRect( _self->memCanvas.width() - 8 - 16, 3.5, 16, 16 );
    //g.FillRectangle( &closeBtnBrush, closeBtnRect );

    PointF points[6] = {
        PointF{ closeBtnRect.X + ( closeBtnRect.Width / 2 ), closeBtnRect.Y + 0 },
        PointF{ closeBtnRect.X + ( closeBtnRect.Width / 2 ) + ( closeBtnRect.Width / 4 ) * (REAL)tan( 3.14159265 / 3 ), closeBtnRect.Y + ( closeBtnRect.Height / 4 ) },
        PointF{ closeBtnRect.X + ( closeBtnRect.Width / 2 ) + ( closeBtnRect.Width / 4 ) * (REAL)tan( 3.14159265 / 3 ), closeBtnRect.Y + ( closeBtnRect.Height * 3 / 4 ) },
        PointF{ closeBtnRect.X + ( closeBtnRect.Width / 2 ), closeBtnRect.Y + closeBtnRect.Height },
        PointF{ closeBtnRect.X + ( closeBtnRect.Width / 2 ) - ( closeBtnRect.Width / 4 ) * (REAL)tan( 3.14159265 / 3 ), closeBtnRect.Y + ( closeBtnRect.Height * 3 / 4 ) },
        PointF{ closeBtnRect.X + ( closeBtnRect.Width / 2 ) - ( closeBtnRect.Width / 4 ) * (REAL)tan( 3.14159265 / 3 ), closeBtnRect.Y + ( closeBtnRect.Height / 4 ) }
    };
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    _self->closeBtnPath.AddPolygon( points, countof(points) );
    g.FillPath( &closeBtnBrush, &_self->closeBtnPath );

    //g.FillPolygon( &closeBtnBrush, points, countof(points) );

    /*// 自己绘制窗口的代码
    //CRect rectClient;
    //GetClientRect(&rectClient);
    //_memdc.create( nullptr, rectClient.Width(), rectClient.Height() );

    //winplus::Graphics g(_memdc);
    //g.SetSmoothingMode(SmoothingModeAntiAlias);

    //rectClient.InflateRect( -8, -8 );
 
    //int padding = 16;

    //RectF rect = winplus::RectGdiToGdiplus<RectF>(rectClient);
    //RectF rect1, rect2;
    //rect1 = rect2 = rect;

    //rect1.X += padding;
    //rect1.Y += padding;
    //rect1.Width -= padding;
    //rect1.Height -= padding;
    //g.FillRectangle( &SolidBrush( Color( 16, 0, 0, 0 ) ), rect1 );
    //g.DrawRectangle( &Pen( Color(255, 204, 0), 1 ), rect1 );

    //rect.Width -= padding;
    //rect.Height -= padding;
    //g.FillRectangle( &SolidBrush( Color( 16, 0, 0, 0 ) ), rect );
    //g.DrawRectangle( &Pen( Color(51, 153, 255), 1 ), rect );

    //rect2.Width = padding + 1;
    //rect2.Height = padding + 1;
    //rect2.X = rect1.GetRight() - rect2.Width + 1;
    //rect2.Y = rect.GetTop() - 1;
    //rect2.Inflate( 6, 6 );
    //SolidBrush brush1( Color( 160, 255, 80, 175 ) );
    //SolidBrush brush2( Color( 90, 255, 173, 218 ) );

    //g.SetCompositingMode(CompositingModeSourceCopy);
    //g.FillEllipse( &brush1, rect2 );
    //g.SetCompositingMode(CompositingModeSourceOver);
    //rect2.Inflate( -1, -1 );
    //g.FillEllipse( &brush2, rect2 );


    //g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    //StringFormat sf( StringFormat::GenericTypographic() );
    //sf.SetLineAlignment(StringAlignmentCenter);

    //g.DrawShadowString(
    //    winplus::Window_GetText(GetSafeHwnd()),
    //    Gdiplus::Font( L"微软雅黑", 12 ),
    //    &SolidBrush( Color( 0, 0x99, 0 ) ),
    //    &SolidBrush( Color( 32, 0, 0, 0 ) ),
    //    RectF( 8, 6, 200, 30 ),
    //    &sf,
    //    nullptr
    //);*/


    BLENDFUNCTION blend;
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    POINT ptPos = { 0, 0 };
    SIZE sizeWnd = { _self->memCanvas.width(), _self->memCanvas.height() };
    ::UpdateLayeredWindow( GetSafeHwnd(), NULL, NULL, &sizeWnd, _self->memCanvas, &ptPos, 0, &blend, ULW_ALPHA );

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

LRESULT AccountComprehensiveWnd::OnNcHitTest( CPoint point )
{
    CPoint point2 = point;
    ScreenToClient(&point2);
    //std::cout << point.x << "," << point.y << "\n";
    if ( _self->rectClient.Contains( point2.x, point2.y ) )
    {
        return HTCLIENT;
    }
    else if ( _self->closeBtnPath.IsVisible( point2.x, point2.y ) )
    {
        return HTCLOSE;
    }

    return HTCAPTION;
}

void AccountComprehensiveWnd::OnMouseMove( UINT nFlags, CPoint point )
{

}
