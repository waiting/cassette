
#include "Cassette.h"
#include "CassetteApp.h"
#include "AccountsView.h"
#include "MainFrame.h"
#include "AboutView.h"
#include "AboutWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MainFrame

IMPLEMENT_DYNAMIC(MainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(MainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(MainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_UPDATE_COMMAND_UI(ID_VIA_AUTOLOGIN, OnUpdateViaAutoLogin)
	ON_COMMAND(ID_VIA_AUTOLOGIN, OnViaAutoLogin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MainFrame construction/destruction

MainFrame::MainFrame() : m_pAccountsView(new AccountsView)
{
	
}

MainFrame::~MainFrame()
{
}

int MainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CFrameWnd::OnCreate(lpCreateStruct) == -1 )
		return -1;
	// create a view to occupy the client area of the frame
	if ( !m_pAccountsView->Create(
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
	
	if ( !m_toolBar.CreateEx(
			this,
			/*TBSTYLE_FLAT*/0,
			WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC
		) || !m_toolBar.LoadToolBar(IDR_MAINFRAME)
	)
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	static UINT indicators[] =
	{
		ID_SEPARATOR,           // status line indicator
		//ID_INDICATOR_CAPS,
		//ID_INDICATOR_NUM,
		//ID_INDICATOR_SCRL,
	};

	if ( !m_statusBar.Create(this) || !m_statusBar.SetIndicators( indicators, countof(indicators) ) )
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_toolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_toolBar);

	// set icon
	SetIcon( icon( IDR_MAINFRAME, 16, 16 ), TRUE );

	g_theApp.GetSharedMemory()->hMainWnd = this->GetSafeHwnd();
	return 0;
}

BOOL MainFrame::PreCreateWindow( CREATESTRUCT & cs )
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style &= ~WS_BORDER;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// MainFrame diagnostics

#ifdef _DEBUG
void MainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void MainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// MainFrame message handlers
void MainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
	m_pAccountsView->SetFocus();
}

BOOL MainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if ( m_pAccountsView->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) )
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

void MainFrame::OnAppAbout()
{
	// 只能打开一个窗口
	AboutWnd * pAbout = AboutWnd::GetAboutWnd(this);
	pAbout->ShowWindow(SW_NORMAL);
	pAbout->SetForegroundWindow();
	pAbout->SetFocus();
	
}

void MainFrame::OnUpdateViaAutoLogin( CCmdUI * pCmdUI ) 
{
	pCmdUI->Enable(g_theApp.m_viaAutoLogin);
	//pCmdUI->SetCheck( g_theApp.m_viaAutoLogin );
	pCmdUI->SetText( _T("不自动登录") );
}

void MainFrame::OnViaAutoLogin() 
{
	g_theApp.m_viaAutoLogin = FALSE;
	g_theApp.m_settings.isAutoLogin = false;
	g_theApp.SaveSettings(CassetteApp::Setting_AutoLogin);
}
