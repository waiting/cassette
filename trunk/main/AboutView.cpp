
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

BOOL AboutView::PreTranslateMessage(MSG* pMsg) 
{
	//½ûÖ¹Êó±êÏÔÊ¾IÐÍÖ¸Õë
	HCURSOR curIbeam = LoadCursor( NULL, IDC_IBEAM );
	if ( GetCursor() == curIbeam )
	{
		HCURSOR curArrow = LoadCursor( NULL, IDC_ARROW );
		SetCursor( curArrow );
	}


	//ÆÁ±ÎÓÒ¼ü²Ëµ¥,×ó¼üË«»÷
	switch ( pMsg->message )
	{
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
		return TRUE;
	}

	//½ûÖ¹Ñ¡Ôñ
	if ( pMsg->message == WM_MOUSEMOVE )
	{
		int nVirtKey = GetKeyState( VK_LBUTTON ); 
		if ( nVirtKey & 0x8000 )
		{
			return TRUE;
		}
	}

	return CHtmlView::PreTranslateMessage(pMsg);
}
