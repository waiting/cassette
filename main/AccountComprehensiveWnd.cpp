
#include "Cassette.h"
#include "AccountComprehensiveWnd.h"
#include "resource.h"

// AccountComprehensiveWnd

IMPLEMENT_DYNAMIC(AccountComprehensiveWnd, CWnd)

#define ID_TIMER_RENDER 1
struct AccountComprehensive_Data
{
    // 画布
    winplus::MemDC memCanvas;
    // 背景框架图
    winplus::SimplePointer<Bitmap> imgBgFrame;
    // 边框宽度
    int lBorder;
    int rBorder;
    int tBorder;
    int bBorder;

    // 窗口大小矩形
    RectF rectWindow;
    // 客户区大小矩形
    RectF rectClient;
    // 右上角关闭按钮的路径区域
    GraphicsPath closeBtnPath;
    // 当前鼠标位置
    Point ptCurMouse;
};

AccountComprehensiveWnd::AccountComprehensiveWnd( CWnd * pParentWnd, winplus::String const & strWindowName, CRect const & rect ) : SpwWindow(pParentWnd)
{
    _self.create();

#ifndef _WIN32_WCE
    EnableActiveAccessibility();
#endif

    // 背景图画刷
    //HBRUSH hbrBkgnd = CreatePatternBrush( (HBITMAP)LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_INTEGRATEDBKIMG), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION ) );
    //HBRUSH hbrBkgnd = CreatePatternBrush( (HBITMAP)LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_INTEGRATEDBKIMG), IMAGE_BITMAP, 0, 0, 0 ) );

    // 创建窗口
    __super::CreateEx(
        WS_EX_LAYERED,
        AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, NULL, NULL ),
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

void AccountComprehensiveWnd::RefreshUi()
{
    // 获取窗口矩形
    CRect rc;
    GetWindowRect(&rc);
    _self->rectWindow = winplus::RectGdiToGdiplus<RectF>(rc);
    std::cout << "Window " << _self->rectWindow << "\n";

    // 客户区
    _self->rectClient = RectF( _self->lBorder, _self->tBorder, _self->rectWindow.Width - _self->lBorder - _self->rBorder, _self->rectWindow.Height - _self->tBorder - _self->bBorder );

    // 创建画布
    _self->memCanvas.create( NULL, _self->rectWindow.Width, _self->rectWindow.Height );
}

void AccountComprehensiveWnd::Draw()
{
    Bitmap & imgBgFrame = *_self->imgBgFrame.get();
    winplus::Graphics g(_self->memCanvas);

    int lBorder = _self->lBorder;
    int rBorder = _self->rBorder;
    int tBorder = _self->tBorder;
    int bBorder = _self->bBorder;

    // 清空画布内容
    g.SetCompositingMode(CompositingModeSourceCopy);
    g.FillRectangle( &SolidBrush( Color( 0, 0, 0, 0 ) ), _self->rectWindow );
    g.SetCompositingMode(CompositingModeSourceOver);

    // 画客户区背景
    g.FillRectangle( &SolidBrush( Color( 96, 255, 255, 255 ) ), _self->rectClient );

    TextureBrush ltBrush{ &imgBgFrame, WrapModeTile, RectF( 0, 0, lBorder, tBorder ) };
    RectF ltCornerRect( 0, 0, lBorder, tBorder );
    ltBrush.TranslateTransform( ltCornerRect.GetLeft(), ltCornerRect.GetTop() );
    g.FillRectangle( &ltBrush, ltCornerRect );

    TextureBrush rtBrush{ &imgBgFrame, WrapModeTile, RectF( imgBgFrame.GetWidth() - rBorder, 0, rBorder, tBorder ) };
    RectF rtCornerRect( _self->memCanvas.width() - rBorder, 0, rBorder, tBorder );
    rtBrush.TranslateTransform( rtCornerRect.GetLeft(), rtCornerRect.GetTop() );
    g.FillRectangle( &rtBrush, rtCornerRect );

    TextureBrush rbBrush{ &imgBgFrame, WrapModeTile, RectF( imgBgFrame.GetWidth() - rBorder, imgBgFrame.GetHeight() - bBorder, rBorder, bBorder ) };
    RectF rbCornerRect( _self->memCanvas.width() - rBorder, _self->memCanvas.height() - bBorder, rBorder, bBorder );
    rbBrush.TranslateTransform( rbCornerRect.GetLeft(), rbCornerRect.GetTop() );
    g.FillRectangle( &rbBrush, rbCornerRect );

    TextureBrush lbBrush{ &imgBgFrame, WrapModeTile, RectF( 0, imgBgFrame.GetHeight() - bBorder, lBorder, bBorder ) };
    RectF lbCornerRect( 0, _self->memCanvas.height() - bBorder, lBorder, bBorder );
    lbBrush.TranslateTransform( lbCornerRect.GetLeft(), lbCornerRect.GetTop() );
    g.FillRectangle( &lbBrush, lbCornerRect );

    TextureBrush lBrush{ &imgBgFrame, WrapModeTile, RectF( 0, tBorder, lBorder, 1 ) };
    RectF lEdgeRect( 0, tBorder, lBorder, _self->memCanvas.height() - tBorder - bBorder );
    lBrush.TranslateTransform( lEdgeRect.GetLeft(), lEdgeRect.GetTop() );
    g.FillRectangle( &lBrush, lEdgeRect );

    TextureBrush tBrush{ &imgBgFrame, WrapModeTile, RectF( lBorder, 0, 1, tBorder ) };
    RectF tEdgeRect( lBorder, 0, _self->memCanvas.width() - lBorder - rBorder, tBorder );
    tBrush.TranslateTransform( tEdgeRect.GetLeft(), tEdgeRect.GetTop() );
    g.FillRectangle( &tBrush, tEdgeRect );

    TextureBrush rBrush{ &imgBgFrame, WrapModeTile, RectF( imgBgFrame.GetWidth() - rBorder, tBorder, rBorder, 1 ) };
    RectF rEdgeRect( _self->memCanvas.width() - rBorder, tBorder, rBorder, _self->memCanvas.height() - tBorder - bBorder );
    rBrush.TranslateTransform( rEdgeRect.GetLeft(), rEdgeRect.GetTop() );
    g.FillRectangle( &rBrush, rEdgeRect );

    TextureBrush bBrush{ &imgBgFrame, WrapModeTile, RectF( lBorder, imgBgFrame.GetHeight() - bBorder, 1, bBorder ) };
    RectF bEdgeRect( lBorder, _self->memCanvas.height() - bBorder, _self->memCanvas.width() - lBorder - rBorder, bBorder );
    bBrush.TranslateTransform( bEdgeRect.GetLeft(), bEdgeRect.GetTop() );
    g.FillRectangle( &bBrush, bEdgeRect );

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

    _self->closeBtnPath.Reset();
    _self->closeBtnPath.AddPolygon( points, countof(points) );

    winplus::SimplePointer<SolidBrush> pCloseBtnBrush;
    if ( _self->closeBtnPath.IsVisible(_self->ptCurMouse) )
    {
        pCloseBtnBrush.attachNew( new SolidBrush( Color( 255, 123, 215 ) ) );
    }
    else
    {
        pCloseBtnBrush.attachNew( new SolidBrush( Color( 255, 204, 0 ) ) );
    }

    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.FillPath( pCloseBtnBrush.get(), &_self->closeBtnPath );
    //g.FillPolygon( &closeBtnBrush, points, countof(points) );
    g.SetSmoothingMode(SmoothingModeDefault);

    // 标题
    g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    RectF rectTitle( 8, 2, _self->rectWindow.Width - 16, _self->tBorder - 4 );
    StringFormat sf( StringFormat::GenericTypographic() );
    sf.SetTrimming(StringTrimming::StringTrimmingEllipsisCharacter);
    //sf.SetLineAlignment(StringAlignment::StringAlignmentNear);

    UnicodeString strTitle = StringToUnicode( Window_GetText(*this) );
    g.DrawString( strTitle.c_str(), strTitle.length(), &Gdiplus::Font( L"微软雅黑", 10 ), rectTitle, &sf, &SolidBrush( Color( 255, 255, 255 ) ) );
    g.SetTextRenderingHint(TextRenderingHintSystemDefault);

    Pen pen( Color( 96, 96, 96 ) );
    pen.SetDashStyle(DashStyle::DashStyleDot);
    auto line = _self->rectClient.Height / 10;
    for ( float off = line; off < _self->rectClient.Height-1; off += line )
    {
        g.DrawLine( &pen, _self->rectClient.GetLeft() + 4, _self->rectClient.GetTop() + off, _self->rectClient.GetRight() - 4, _self->rectClient.GetTop() + off );
    }
}

void AccountComprehensiveWnd::Render()
{
    BLENDFUNCTION blend;
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    POINT ptPos = { 0, 0 };
    SIZE sizeWnd = { _self->memCanvas.width(), _self->memCanvas.height() };
    ::UpdateLayeredWindow( GetSafeHwnd(), NULL, NULL, &sizeWnd, _self->memCanvas, &ptPos, 0, &blend, ULW_ALPHA );
}

BOOL AccountComprehensiveWnd::PreCreateWindow( CREATESTRUCT & cs )
{
    if ( !__super::PreCreateWindow(cs) ) return FALSE;
    cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
    cs.dwExStyle |= WS_EX_TOOLWINDOW;
    return TRUE;
}

BEGIN_MESSAGE_MAP(AccountComprehensiveWnd, SpwWindow)
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_NCHITTEST()
    ON_WM_MOUSEMOVE()
    ON_WM_TIMER()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_RBUTTONDBLCLK()
END_MESSAGE_MAP()

// AccountComprehensiveWnd 消息处理程序

int AccountComprehensiveWnd::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    if ( __super::OnCreate( lpCreateStruct ) == -1 )
        return -1;

    // 载入背景框架图
    _self->imgBgFrame.attachNew( new Bitmap( winplus::CreateStreamFromResource( IDR_PNG_ACCOUNT_COMPREHENSIVE_WND, _T("PNG") ), FALSE ) );
    _self->lBorder = 8;
    _self->rBorder = 8;
    _self->tBorder = 23;
    _self->bBorder = 8;


    this->RefreshUi();
    this->Draw();
    this->Render();

    return 0;
}

void AccountComprehensiveWnd::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    //_memdc.alphaEntireTo( dc, 0, 0, _memdc.width(), _memdc.height() );
    //_memdc.copyEntireTo( dc, 0, 0 );

}

LRESULT AccountComprehensiveWnd::OnNcHitTest( CPoint point )
{
    LRESULT lResult = HTCAPTION;
    CPoint point2 = point;
    ScreenToClient(&point2);
    _self->ptCurMouse.X = point2.x;
    _self->ptCurMouse.Y = point2.y;

    //std::cout << "NcHitTest" << " " << point2 << "\n";
    if ( _self->rectClient.Contains( point2.x, point2.y ) )
    {
        if ( ( GetAsyncKeyState(VK_LBUTTON) & 0x8001 ) == 0x8000 )
        {
            lResult = HTCAPTION;
        }
        else
        {
            lResult = HTCLIENT;
        }
    }
    else if ( _self->closeBtnPath.IsVisible( point2.x, point2.y ) )
    {
        lResult = HTCLIENT;
        /*if ( ( GetAsyncKeyState(VK_LBUTTON) & 0x8000 ) == 0x8000 )
        {
            lResult = HTCLIENT;
        }
        else
        {
            lResult = HTCLOSE;
        }*/
    }

    this->Draw();
    this->Render();

    return lResult;
}

void AccountComprehensiveWnd::OnMouseMove( UINT nFlags, CPoint point )
{
    //std::cout << "MouseMove: nFlags=" << nFlags << ", " << point << "\n";
}

void AccountComprehensiveWnd::OnTimer( UINT_PTR nIDEvent )
{
    switch ( nIDEvent )
    {
    case ID_TIMER_RENDER:
        this->Render();
        break;
    }
}

void AccountComprehensiveWnd::OnLButtonDown( UINT nFlags, CPoint point )
{
    std::cout << "LButtonDown: nFlags=" << nFlags << ", " << point << "\n";
}

void AccountComprehensiveWnd::OnLButtonUp( UINT nFlags, CPoint point )
{
    std::cout << "LButtonUp: nFlags=" << nFlags << ", " << point << "\n";

    if ( _self->closeBtnPath.IsVisible( point.x, point.y ) )
    {
        SendMessage(WM_CLOSE);
    }
}

void AccountComprehensiveWnd::OnLButtonDblClk( UINT nFlags, CPoint point )
{
    std::cout << "LButtonDblClk: nFlags=" << nFlags << ", " << point << "\n";
}

void AccountComprehensiveWnd::OnRButtonDown( UINT nFlags, CPoint point )
{
    std::cout << "RButtonDown: nFlags=" << nFlags << ", " << point << "\n";
}

void AccountComprehensiveWnd::OnRButtonUp( UINT nFlags, CPoint point )
{
    std::cout << "RButtonUp: nFlags=" << nFlags << ", " << point << "\n";
}

void AccountComprehensiveWnd::OnRButtonDblClk( UINT nFlags, CPoint point )
{
    std::cout << "RButtonDblClk: nFlags=" << nFlags << ", " << point << "\n";
}


