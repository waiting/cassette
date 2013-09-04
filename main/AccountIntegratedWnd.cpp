
#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountIntegratedWnd.h"

std::map<HWND, AccountIntegratedWnd *> AccountIntegratedWnd::m_hasDisplayed;
/////////////////////////////////////////////////////////////////////////////
// AccountIntegratedWnd

AccountIntegratedWnd::AccountIntegratedWnd( CWnd * pParentWnd, LPCTSTR lpszWindowName, DWORD dwStyle, DWORD dwExStyle, const RECT& rect )
{
	m_hClientDC = NULL;
	m_ptCurMouse = CPoint( -100, -100 );
	m_radiusMouseCircle = 10;

	CFrameWnd::Create( NULL, lpszWindowName, dwStyle, rect, pParentWnd, NULL, dwExStyle, NULL );
	AddDisplayedWnd( *pParentWnd, this );
}

AccountIntegratedWnd::~AccountIntegratedWnd()
{
}

void AccountIntegratedWnd::RefreshAllCreate()
{
	// 创建相关对象
	GetClientRect(&m_rcClient);
	
	// 创建关联HDC的绘图对象
	m_gDevice = std::auto_ptr<Graphics>( new Graphics(m_hClientDC) );
	// 创建缓冲图
	m_imgCached.create( m_rcClient.Width(), m_rcClient.Height() );
	m_gCached = std::auto_ptr<Graphics>( new Graphics(m_imgCached) );
	m_gCached->SetSmoothingMode(SmoothingModeAntiAlias);
	m_gCached->SetTextRenderingHint(TextRenderingHintAntiAlias);

	//作图
	MakeDraw();

}

// 作画逻辑
void AccountIntegratedWnd::MakeDraw()
{
	// 画背景
	m_gCached->DrawImage( m_loadedBgImg.get(), 0, 0, 0, 0, m_loadedBgImg->GetWidth(), m_loadedBgImg->GetHeight(), UnitPixel );
	/*m_gCached->DrawImage( 
		m_loadedBgImg.get(),
		Rect( 0, 0, m_rcClient.Width(), m_rcClient.Height() ),
		0, 0, m_loadedBgImg->GetWidth(), m_loadedBgImg->GetHeight(),
		UnitPixel
	);*/
	// 画鼠标特效
	/*int x, y;
	x = m_ptCurMouse.x - m_radiusMouseCircle / 2;
	y = m_ptCurMouse.y - m_radiusMouseCircle / 2;
	m_gCached->FillEllipse( &SolidBrush( Color(64,255,255,255) ), x, y, m_radiusMouseCircle, m_radiusMouseCircle );
	//m_gCached->DrawEllipse( &Pen( Color(128,255,255,255), 0.5f ), x, y, m_radiusMouseCircle, m_radiusMouseCircle );//*/

	Font font( L"宋体", 24, 0, UnitPixel );
	StringFormat fmt;
	fmt.SetAlignment(StringAlignmentCenter);
	fmt.SetLineAlignment(StringAlignmentCenter);

	RectF outRect( 0, 0, m_rcClient.Width(), 40 );
	DrawShadowString( _T("账户信息查看"), font, outRect, fmt, &m_captionRect );

	//DrawShadowFrame( RectF( 10, 40, m_rcClient.Width() - 20, m_rcClient.Height() - 40 - 10 ) );
	DrawBackground( RectF( 10, 40, m_rcClient.Width() - 20, m_rcClient.Height() - 40 - 10 ) );

	// 建立在gDevice上的缓冲位图
	m_CBCached = std::auto_ptr<CachedBitmap>( new CachedBitmap( m_imgCached, m_gDevice.get() ) );
}

void AccountIntegratedWnd::Draw()
{
	//if ( m_CBCached.get() )
	//{
		m_gDevice->DrawCachedBitmap( m_CBCached.get(), 0, 0 );
	//}
	//else
	//{
	//	m_imgCached.output_entire( *m_gDevice.get(), 0, 0 );
	//}
}

void AccountIntegratedWnd::DrawShadowString( string const & s, Font const & font, RectF const & layoutRect, StringFormat const & fmt, RectF * boundingRect )
{
	unicode_string sU = string_to_unicode(s);
	if ( boundingRect )
	{
		m_gCached->MeasureString(
			sU.c_str(),
			sU.length(),
			&font,
			layoutRect,
			&fmt,
			boundingRect
		);
	}
	
	m_gCached->DrawString(
		sU.c_str(),
		sU.length(),
		&font,
		RectF( layoutRect.X + 1, layoutRect.Y + 1, layoutRect.Width, layoutRect.Height ),
		&fmt,
		&SolidBrush( Color( 0, 0, 0 ) )
	);
	
	m_gCached->DrawString(
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
	DrawRoundRectangle( *m_gCached.get(), pen1, RectF( rect.X+1,rect.Y+1,rect.Width,rect.Height ), 10 );
	Pen pen2( Color( 255, 255, 255 ) ); 
	DrawRoundRectangle( *m_gCached.get(), pen2, rect, 10 );
}

void AccountIntegratedWnd::DrawBackground( RectF const & rect )
{
	FillRoundRectangle( *m_gCached.get(), SolidBrush( Color( 64, 255, 255, 255 ) ), rect, 10 );
}

BEGIN_MESSAGE_MAP(AccountIntegratedWnd, CFrameWnd)
	//{{AFX_MSG_MAP(AccountIntegratedWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void AccountIntegratedWnd::PostNcDestroy() 
{
	DelDisplayedWnd(this);
	delete this;
}

BOOL AccountIntegratedWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	if ( !CFrameWnd::PreCreateWindow(cs) ) return FALSE;
	//cs.style &= ~( WS_THICKFRAME | WS_MAXIMIZEBOX | WS_OVERLAPPED | WS_BORDER );
	cs.style = WS_POPUP | WS_CAPTION | WS_SYSMENU;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.dwExStyle |= WS_EX_TOOLWINDOW;
	//cs.lpszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS/* | CS_SAVEBITS*/ );
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// AccountIntegratedWnd message handlers

int AccountIntegratedWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	//设置透明度
	SetLayeredWindowAttributes( *this, 0, 255, LWA_ALPHA );
	//载入背景图
	HBITMAP hBmpBgImage = LoadBitmap( g_theApp.m_hInstance, MAKEINTRESOURCE(IDR_INTEGRATEDBKIMG) );
	m_loadedBgImg = std::auto_ptr<Bitmap>( new Bitmap( hBmpBgImage, NULL ) );
	DeleteObject(hBmpBgImage);

	m_hClientDC = ::GetDC( GetSafeHwnd() );

	RefreshAllCreate();

	//m_timer1.create( *this, 100 );
	return 0;
}

void AccountIntegratedWnd::OnDestroy() 
{
	if ( m_hClientDC )
	{
		::ReleaseDC( GetSafeHwnd(), m_hClientDC );
		m_hClientDC = NULL;
	}
	CFrameWnd::OnDestroy();
}

void AccountIntegratedWnd::OnPaint()
{
	CPaintDC dc(this);
	Draw();
}

void AccountIntegratedWnd::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	if ( cx < 1 || cy < 1 )
		return;

	//Invalidate(FALSE);
}

UINT AccountIntegratedWnd::OnNcHitTest(CPoint point) 
{
	//伪造点击标题栏，使得可以点击任意位置移动窗口
	if ( HTCLIENT == CFrameWnd::OnNcHitTest(point) && GetAsyncKeyState(MK_LBUTTON) < 0 )
	{
		return HTCAPTION;
	}
	return CFrameWnd::OnNcHitTest(point);
}

BOOL AccountIntegratedWnd::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;//CFrameWnd::OnEraseBkgnd(pDC);
}

void AccountIntegratedWnd::OnTimer(UINT nIDEvent) 
{
	if ( nIDEvent == m_timer1.get_id() )
	{
		static int incDelta = 3;
		m_radiusMouseCircle += ++incDelta;
		if ( m_radiusMouseCircle > 60 )
		{
			m_radiusMouseCircle = 10;
			incDelta = 3;
		}

		MakeDraw();
		Invalidate(FALSE);
	}

}

void AccountIntegratedWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	CFrameWnd::OnMouseMove(nFlags, point);
	m_ptCurMouse = point;
	//MakeDraw();
	//Invalidate(FALSE);
}
