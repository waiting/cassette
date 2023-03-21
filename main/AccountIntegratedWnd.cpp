
#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountIntegratedWnd.h"

#include <playsoundapi.h>

std::map<HWND, AccountIntegratedWnd *> AccountIntegratedWnd::m_hasDisplayed;

// AccountIntegratedWnd -------------------------------------------------------------------
AccountIntegratedWnd::AccountIntegratedWnd( CWnd * pParentWnd, LPCTSTR lpszWindowName, const RECT& rect )
{
    m_ptCurMouse = CPoint( -100, -100 );
    m_radiusMouseCircle = 8;

    CWnd::CreateEx( 0, AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW ), lpszWindowName, WS_POPUP | WS_CAPTION | WS_SYSMENU, rect, pParentWnd, 0, NULL );
    AddDisplayedWnd( pParentWnd->GetSafeHwnd(), this );
}

AccountIntegratedWnd::~AccountIntegratedWnd()
{
}

void AccountIntegratedWnd::RefreshAllCreate()
{
    // 创建相关对象
    GetClientRect(&m_rcClient);
    
    // 创建缓冲图
    //m_memCanvas.create( NULL, m_rcClient.Width(), m_rcClient.Height() );
    m_memCanvas.create( m_rcClient.Width(), m_rcClient.Height() );
    m_gCanvas = std::auto_ptr<Graphics>( new Graphics(m_memCanvas) );
    m_gCanvas->SetSmoothingMode(SmoothingModeAntiAlias);
    m_gCanvas->SetTextRenderingHint(TextRenderingHintAntiAlias);

    //作图
    MakeDraw();
}

void AccountIntegratedWnd::SetAccountsInfo( AccountCate const & cate, AccountArray const & accounts )
{
    m_cate = cate;
    m_accounts.Copy(accounts);
}

// 作画逻辑
void AccountIntegratedWnd::MakeDraw()
{
    // 画背景
    m_gCanvas->DrawImage( m_loadedBgImg.get(), 0, 0, 0, 0, m_loadedBgImg->GetWidth(), m_loadedBgImg->GetHeight(), UnitPixel );

    // 画鼠标特效
    int x, y;
    x = m_ptCurMouse.x - m_radiusMouseCircle / 2;
    y = m_ptCurMouse.y - m_radiusMouseCircle / 2;
    m_gCanvas->FillEllipse( &SolidBrush( Color(64,255,255,255) ), x, y, m_radiusMouseCircle, m_radiusMouseCircle );
    m_gCanvas->DrawEllipse( &Pen( Color(128,255,255,255), 0.5f ), x, y, m_radiusMouseCircle, m_radiusMouseCircle );

    Gdiplus::Font font( L"宋体", 24, 0, UnitPixel );
    StringFormat fmt;
    fmt.SetAlignment(StringAlignmentCenter);
    fmt.SetLineAlignment(StringAlignmentCenter);

    RectF outRect( 0, 0, m_rcClient.Width(), 40 );
    DrawShadowString( _T("账户信息查看"), font, outRect, fmt, &m_captionRect );

    // 画内容背景
    RectF rcContentBg( 10, 40, m_rcClient.Width() - 20, m_rcClient.Height() - 40 - 10 );
    DrawBackground(rcContentBg);
    DrawShadowFrame(rcContentBg);

    // 画账户信息

}

void AccountIntegratedWnd::DrawShadowString( winplus::String const & s, Gdiplus::Font const & font, RectF const & layoutRect, StringFormat const & fmt, RectF * boundingRect )
{
    winplus::UnicodeString sU = winplus::StringToUnicode(s);
    if ( boundingRect )
    {
        m_gCanvas->MeasureString(
            sU.c_str(),
            sU.length(),
            &font,
            layoutRect,
            &fmt,
            boundingRect
        );
    }
    
    m_gCanvas->DrawString(
        sU.c_str(),
        sU.length(),
        &font,
        RectF( layoutRect.X + 1, layoutRect.Y + 1, layoutRect.Width, layoutRect.Height ),
        &fmt,
        &SolidBrush( Color( 0, 0, 0 ) )
    );
    
    m_gCanvas->DrawString(
        sU.c_str(),
        sU.length(),
        &font,
        layoutRect,
        &fmt,
        &SolidBrush( Color( 255, 255, 255 ) )
    );
}

void AccountIntegratedWnd::DrawShadowFrame( RectF const & rect )
{
    //创建一个的画笔
    Pen pen1( Color( 0, 0, 0 ) );
    winplus::DrawRoundRectangle( *m_gCanvas.get(), pen1, RectF( rect.X+1,rect.Y+1,rect.Width,rect.Height ), 10 );
    Pen pen2( Color( 255, 255, 255 ) ); 
    winplus::DrawRoundRectangle( *m_gCanvas.get(), pen2, rect, 10 );
}

void AccountIntegratedWnd::DrawBackground( RectF const & rect )
{
    winplus::FillRoundRectangle( *m_gCanvas.get(), SolidBrush( Color( 64, 255, 255, 255 ) ), rect, 10 );
}

void AccountIntegratedWnd::PostNcDestroy()
{
    std::cout << "AccountIntegratedWnd::PostNcDestroy()\n";
    DelDisplayedWnd(this);
    delete this;
}

BOOL AccountIntegratedWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
    if ( !CWnd::PreCreateWindow(cs) ) return FALSE;
    //cs.style &= ~( WS_THICKFRAME | WS_MAXIMIZEBOX | WS_OVERLAPPED | WS_BORDER );
    //cs.style = WS_POPUP | WS_CAPTION | WS_SYSMENU;
    cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
    cs.dwExStyle |= WS_EX_TOOLWINDOW;
    //cs.lpszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS/* | CS_SAVEBITS*/ );
    
    return TRUE;
}

BEGIN_MESSAGE_MAP(AccountIntegratedWnd, CWnd)
    //{{AFX_MSG_MAP(AccountIntegratedWnd)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    //ON_WM_NCHITTEST()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AccountIntegratedWnd message handlers

int AccountIntegratedWnd::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    if ( CWnd::OnCreate(lpCreateStruct) == -1 )
        return -1;

    //设置透明度
    //SetLayeredWindowAttributes( 0, 255, LWA_ALPHA );
    //载入背景图
    m_loadedBgImg = std::auto_ptr<Bitmap>( new Bitmap( AfxGetApp()->m_hInstance, MAKEINTRESOURCEW(IDR_INTEGRATEDBKIMG) ) );

    RefreshAllCreate();

    m_timer1.create( *this, 100 );
    return 0;
}

void AccountIntegratedWnd::OnDestroy()
{
    std::cout << "AccountIntegratedWnd::OnDestroy()\n";

    CWnd::OnDestroy();
}

void AccountIntegratedWnd::OnPaint()
{
    CPaintDC dc(this);
    Graphics g(dc);
    m_memCanvas.outputEntire( g, 0, 0 );
    //m_memCanvas.copyEntireTo( dc, 0, 0 );
}

LRESULT AccountIntegratedWnd::OnNcHitTest( CPoint point )
{
    LRESULT ret = CWnd::OnNcHitTest(point);
    //std::cout << "AccountIntegratedWnd::OnNcHitTest( CPoint point )" << ret << "\n";

    // 伪造点击标题栏，使得可以点击任意位置移动窗口
    if ( HTCLIENT == ret && GetAsyncKeyState(MK_LBUTTON) < 0 )
    {
        return HTCAPTION;
    }
    return ret;
}

BOOL AccountIntegratedWnd::OnEraseBkgnd( CDC * pDC )
{
    return TRUE; // CFrameWnd::OnEraseBkgnd(pDC);
}

void AccountIntegratedWnd::OnTimer( UINT nIDEvent )
{
    if ( nIDEvent == m_timer1.getId() )
    {
        static int incDelta = 1;
        m_radiusMouseCircle += ++incDelta;
        if ( m_radiusMouseCircle > 60 )
        {
            m_radiusMouseCircle = 10;
            incDelta = 1;
        }

        MakeDraw();
        Invalidate(FALSE);
    }
}

void AccountIntegratedWnd::OnMouseMove( UINT nFlags, CPoint point )
{
    //std::cout << "AccountIntegratedWnd::OnMouseMove()\n";
    m_ptCurMouse = point;
    MakeDraw();
    Invalidate(FALSE);

    //CWnd::OnMouseMove(nFlags, point);
}


void AccountIntegratedWnd::OnLButtonUp( UINT nFlags, CPoint point )
{
    std::cout << "AccountIntegratedWnd::OnLButtonUp()\n";
    PlaySound( MAKEINTRESOURCE(IDR_WAVE_SELECTED), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC );
    //CWnd::OnLButtonUp( nFlags, point );
}


void AccountIntegratedWnd::OnLButtonDown( UINT nFlags, CPoint point )
{
    std::cout << "AccountIntegratedWnd::OnLButtonDown()\n";
    //PlaySound( MAKEINTRESOURCE(IDR_WAVE_SELECTED), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC );
    //CWnd::OnLButtonDown( nFlags, point );
}
