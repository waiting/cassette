
#include "Cassette.h"
#include "CassetteApp.h"
#include "AboutView.h"
#include "AboutWnd.h"
#include <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AboutWnd

IMPLEMENT_DYNCREATE(AboutWnd, CFrameWnd)

AboutWnd * AboutWnd::ms_pSingleAboutWnd = NULL;

AboutWnd * AboutWnd::GetSingleton( CWnd * pParent /*= NULL */ )
{
	if ( ms_pSingleAboutWnd == NULL )
	{
		ms_pSingleAboutWnd = new AboutWnd(pParent);
		ms_pSingleAboutWnd->UpdateWindow();
	}
	return ms_pSingleAboutWnd;
}

AboutWnd::AboutWnd( CWnd * pParent /*= NULL*/ )
{
	m_pAboutView = new AboutView();
	this->Create(
		NULL,
		(_T("���� - ") + load_string(AFX_IDS_APP_TITLE)).c_str(),
		WS_OVERLAPPEDWINDOW,
		CRect( 0, 0, 480, 340 ),
		pParent
	);

}

AboutWnd::~AboutWnd()
{
	ms_pSingleAboutWnd = NULL;
}

BEGIN_MESSAGE_MAP(AboutWnd, CFrameWnd)
	//{{AFX_MSG_MAP(AboutWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AboutWnd message handlers

BOOL AboutWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	if ( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style &= ~( WS_THICKFRAME | WS_MAXIMIZEBOX | WS_OVERLAPPED | WS_BORDER );
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	//cs.style = WS_POPUP | WS_BORDER;
	//cs.dwExStyle = 0;

	return TRUE;
}

int AboutWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CFrameWnd::OnCreate(lpCreateStruct) == -1 )
		return -1;

	if ( !m_pAboutView->Create(
			NULL,
			NULL,
			AFX_WS_DEFAULT_VIEW,
			CRect( 0, 0, 0, 0 ),
			this,
			AFX_IDW_PANE_FIRST
		)
	)
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	// set icon
	SetIcon( icon( IDR_MAINFRAME, 16, 16 ), TRUE );

	// ���д���
	this->CenterWindow();
	// ��������WM_INITIALUPDATE��Ϣ(ʹ��ͼ��OnInitialUpdate()����)
	this->SendMessageToDescendants( WM_INITIALUPDATE, 0, 0, TRUE, TRUE );

	return 0;
}

void AboutWnd::OnSetFocus(CWnd* pOldWnd) 
{
	CFrameWnd::OnSetFocus(pOldWnd);
	m_pAboutView->SetFocus();
}

BOOL AboutWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// ����ͼ���ȴ���������Ϣ
	if ( m_pAboutView->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) )
		return TRUE;
	
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
