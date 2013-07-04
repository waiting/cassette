// ChildView.cpp : implementation of the AccountsView class
//

#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AccountsView

AccountsView::AccountsView()
{
}

AccountsView::~AccountsView()
{
}


BEGIN_MESSAGE_MAP(AccountsView, CListView)
	//{{AFX_MSG_MAP(AccountsView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// AccountsView message handlers

BOOL AccountsView::PreCreateWindow( CREATESTRUCT & cs ) 
{
	if ( !CListView::PreCreateWindow(cs) )
		return FALSE;

	cs.style |= LVS_REPORT | WS_TABSTOP;// | WS_BORDER;
	//cs.dwExStyle |= WS_EX_CLIENTEDGE;
	//cs.style &= ~WS_BORDER;
	/*cs.lpszClass = AfxRegisterWndClass(
		CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)( COLOR_WINDOW + 1 ),
		NULL
	);*/

	return TRUE;
}

int AccountsView::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if ( CListView::OnCreate(lpCreateStruct) == -1 )
		return -1;
	
	// TODO: Add your specialized creation code here
	CListCtrl & lst = this->GetListCtrl();
	lst.InsertColumn( 0, _T("账户名称"), LVCFMT_LEFT, 100 );
	lst.InsertColumn( 1, _T("账号"), LVCFMT_LEFT, 100 );
	lst.InsertColumn( 2, _T("类别"), LVCFMT_LEFT, 48 );
	lst.InsertColumn( 3, _T("安全级别"), LVCFMT_LEFT, 80 );
	lst.InsertColumn( 4, _T("录入时刻"), LVCFMT_LEFT, 120 );
	lst.InsertColumn( 5, _T("备注"), LVCFMT_LEFT, 160 );

	return 0;
}

void AccountsView::OnSize(UINT nType, int cx, int cy) 
{
	CListView::OnSize(nType, cx, cy);
	if ( nType != 0 || ( cx == 0 && cy == 0 ) )
	{
		return;
	}
	CRect rc;
	GetClientRect(&rc);
	//msgbox( format("Type=%u,{%d,%d,%d,%d}",nType, rc) );
	int widths[] = { 100, 100, 48, 80, 120, 160 };
	double sumWidths = 0;
	int i;
	for ( i = 0; i < countof(widths); ++i )
		sumWidths += widths[i];

	CListCtrl & lst = this->GetListCtrl();
	for ( i = 0; i < countof(widths); ++i )
	{
		int newWidth = widths[i] / sumWidths * ( rc.Width() - GetSystemMetrics(SM_CXVSCROLL) );
		lst.SetColumnWidth( i, widths[i] > newWidth ? widths[i] : newWidth  );
	}

}
