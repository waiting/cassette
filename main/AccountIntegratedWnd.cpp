
#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountIntegratedWnd.h"
#include "AccountEditingDlg.h"

#include <playsoundapi.h>

std::map<HWND, AccountIntegratedWnd *> AccountIntegratedWnd::m_hasDisplayed;

// AccountIntegratedWnd -------------------------------------------------------------------
AccountIntegratedWnd::AccountIntegratedWnd( CWnd * pParentWnd, LPCTSTR lpszWindowName, const RECT& rect ) :
    m_primaryFont( L"宋体", 24, 0, UnitPixel ),
    m_penBlack( Color( 0, 0, 0 ) ),
    m_penHalfWhite( Color( 128, 255, 255, 255 ), 0.5f ),
    m_penWhite( Color( 255, 255, 255 ) ),
    m_brushHalfWhite( Color( 64, 255, 255, 255 ) ),
    m_brushBlack( Color( 0, 0, 0 ) ),
    m_brushWhite( Color( 255, 255, 255 ) )
{
    m_ptCurMouse = CPoint( -100, -100 );
    m_radiusMouseCircle = 8;
    m_sfHVCenter.SetAlignment(StringAlignmentCenter);
    m_sfHVCenter.SetLineAlignment(StringAlignmentCenter);

    CWnd::CreateEx( 0, AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW ), lpszWindowName, WS_POPUP | WS_CAPTION | WS_SYSMENU, rect, pParentWnd, 0, NULL );
    AddDisplayedWnd( pParentWnd->GetSafeHwnd(), this );
}

AccountIntegratedWnd::~AccountIntegratedWnd()
{
}

void AccountIntegratedWnd::SetAccountsInfo( AccountCate const & cate, AccountArray const & accounts )
{
    m_cate = cate;
    m_accounts.Copy(accounts);
}

void AccountIntegratedWnd::RefreshAllCreate()
{
    // 创建相关对象
    GetClientRect(&m_rcClient);
    // 载入背景图
    m_loadedBgImg.attachNew( new Bitmap( AfxGetApp()->m_hInstance, MAKEINTRESOURCEW(IDR_INTEGRATEDBKIMG) ) );
    m_timer1.create( *this, 100 );
    // 创建缓冲图
    //m_memCanvas.create( NULL, m_rcClient.Width(), m_rcClient.Height() );
    m_memCanvas.create( m_rcClient.Width(), m_rcClient.Height() );
    m_gCanvas.attachNew( new Graphics(m_memCanvas) );
    m_gCanvas->SetSmoothingMode(SmoothingModeAntiAlias);
    m_gCanvas->SetTextRenderingHint(TextRenderingHintAntiAlias);

    // 作图
    MakeDraw();
}

// 作画逻辑
void AccountIntegratedWnd::MakeDraw()
{
    // 画背景
    m_gCanvas->DrawImage( m_loadedBgImg.get(), 0, 0, 0, 0, m_loadedBgImg->GetWidth(), m_loadedBgImg->GetHeight(), UnitPixel );

    // 画鼠标特效
    //int x, y;
    //x = m_ptCurMouse.x - m_radiusMouseCircle / 2;
    //y = m_ptCurMouse.y - m_radiusMouseCircle / 2;
    //m_gCanvas->FillEllipse( &m_brushHalfWhite, x, y, m_radiusMouseCircle, m_radiusMouseCircle );
    //m_gCanvas->DrawEllipse( &m_penHalfWhite, x, y, m_radiusMouseCircle, m_radiusMouseCircle );

    // 画主标题
    DrawShadowString( _T("账户信息查看"), m_primaryFont, RectF( 0, 0, m_rcClient.Width(), 40 ), m_sfHVCenter, &m_captionRect );

    // 画内容背景
    m_contentRect = RectF( 10, 40, m_rcClient.Width() - 20, m_rcClient.Height() - 40 - 10 );
    //DrawBackground(m_contentRect);
    //DrawShadowFrame(m_contentRect);

    // 画账户信息
    // std::cout << m_contentRect.Height / 5 << std::endl;
    int nAcountsCount = (int)m_accounts.GetCount();
    int i = 0;
    // 边距
    constexpr int padding = 4;
    // 行高
    constexpr int lineHeight = 68;
    Pen *penWhite = m_penHalfWhite.Clone();
    penWhite->SetDashStyle(DashStyleDot);
    for ( int height = lineHeight; height < m_contentRect.Height && i < nAcountsCount; height += lineHeight, i++ )
    {
        m_gCanvas->DrawLine( penWhite, m_contentRect.GetLeft() + padding, m_contentRect.GetTop() + height, m_contentRect.GetRight() - 1 - padding, m_contentRect.GetTop() + height );

        RectF accountInfoRect( m_contentRect.X + padding, m_contentRect.Y + height - (lineHeight - padding), m_contentRect.Width - 1 - padding - padding, (lineHeight - padding) );

        SolidBrush brushHalfBlack( Color( 32, 0, 0, 0 ) );
        m_gCanvas->FillRectangle( &brushHalfBlack, accountInfoRect );
        StringFormat fmt;
        fmt.SetLineAlignment(StringAlignmentNear);
        Gdiplus::Font font( L"宋体", 10 );

        RectF targetRect = accountInfoRect;
        targetRect.Inflate( -padding, -padding );
        RectF outRect;
        DrawShadowString( CStringToString(m_accounts[i].m_myName), font, targetRect, fmt, &outRect );

        RectF targetRect1 = targetRect;
        targetRect1.Offset( 0, outRect.Height + padding );
        DrawShadowString( "" + CStringToString(m_accounts[i].m_accountName), font, targetRect1, fmt, &outRect );

        RectF targetRect2 = targetRect1;
        targetRect2.Offset( 0, outRect.Height + padding );
        DrawShadowString( "" + CStringToString(m_accounts[i].m_accountPwd), font, targetRect2, fmt, &outRect );

        //winplus::UnicodeString strMyName = winplus::StringToUnicode( CStringToString(m_accounts[i].m_myName) );
        //m_gCanvas->DrawString( strMyName.c_str(), strMyName.length(), &font, rect, &fmt, &m_brushBlack );
    }
    delete penWhite;
}

void AccountIntegratedWnd::DrawShadowString( winplus::String const & s, Gdiplus::Font const & font, RectF const & layoutRect, StringFormat const & fmt, RectF * boundingRect )
{
    winplus::UnicodeString sU = winplus::StringToUnicode(s);
    if ( boundingRect )
    {
        m_gCanvas->MeasureString(
            sU.c_str(),
            (INT)sU.length(),
            &font,
            layoutRect,
            &fmt,
            boundingRect
        );
    }
    
    m_gCanvas->DrawString(
        sU.c_str(),
        (INT)sU.length(),
        &font,
        RectF( layoutRect.X + 1, layoutRect.Y + 1, layoutRect.Width, layoutRect.Height ),
        &fmt,
        &m_brushBlack
    );
    
    m_gCanvas->DrawString(
        sU.c_str(),
        (INT)sU.length(),
        &font,
        layoutRect,
        &fmt,
        &m_brushWhite
    );
}

void AccountIntegratedWnd::DrawShadowFrame( RectF const & rect )
{
    winplus::DrawRoundRectangle( *m_gCanvas.get(), m_penBlack, RectF( rect.X + 1, rect.Y + 1, rect.Width, rect.Height ), 10.0 );
    winplus::DrawRoundRectangle( *m_gCanvas.get(), m_penWhite, rect, 10.0 );
}

void AccountIntegratedWnd::DrawBackground( RectF const & rect )
{
    winplus::FillRoundRectangle( *m_gCanvas.get(), m_brushHalfWhite, rect, 10.0 );
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

    // 设置透明度
    //SetLayeredWindowAttributes( 0, 255, LWA_ALPHA );

    this->RefreshAllCreate();

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

void AccountIntegratedWnd::OnTimer( UINT_PTR nIDEvent )
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
    std::cout << "AccountIntegratedWnd::OnLButtonUp(" << point.x << ", " << point.y << ")\n";

    if ( m_contentRect.Contains( point.x, point.y ) )
    {
        //PlaySound( MAKEINTRESOURCE(IDR_WAVE_SELECTED), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC );
    }
    //CWnd::OnLButtonUp( nFlags, point );
}

void AccountIntegratedWnd::OnLButtonDown( UINT nFlags, CPoint point )
{
    std::cout << "AccountIntegratedWnd::OnLButtonDown()\n";
    //PlaySound( MAKEINTRESOURCE(IDR_WAVE_SELECTED), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC );
    //CWnd::OnLButtonDown( nFlags, point );
}
