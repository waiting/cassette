
#include "Cassette.h"
#include "CassetteApp.h"
#include "Dialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialog dialog

Dialog::Dialog( UINT nIDTemplate, CWnd * pParentWnd )
: CDialog( nIDTemplate, pParentWnd )
{
	//{{AFX_DATA_INIT(Dialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(Dialog, CDialog)
	//{{AFX_MSG_MAP(Dialog)
		// NOTE: the ClassWizard will add message map macros here
		ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipText )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Dialog message handlers
BOOL Dialog::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	TOOLTIPTEXT * pTTT = (TOOLTIPTEXT *)pNMHDR;
	UINT nID = pNMHDR->idFrom;
	if ( pTTT->uFlags & TTF_IDISHWND )
	{
		// idFrom is actually the HWND of the tool
		nID = ::GetDlgCtrlID( reinterpret_cast<HWND>(nID) );
		if ( nID != 0 )
		{
			pTTT->lpszText = MAKEINTRESOURCE(nID); // 利用同ID的字符串资源作提示文本
			pTTT->hinst = AfxGetResourceHandle();
			return TRUE;
		}
	}
	return FALSE;
	
}

BOOL Dialog::PreTranslateMessage( MSG * pMsg ) 
{
	// 让ToolTips Ctrl处理系统发出的ToolTips相关消息
	if ( m_ToolTips )
	{
		m_ToolTips.RelayEvent(pMsg);
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL Dialog::OnInitDialog()
{
	BOOL bRet = CDialog::OnInitDialog();

	//this->EnableToolTips(TRUE); // 不用窗口自带的ToolTips功能

	m_ToolTips.Create( this, TTS_BALLOON | TTS_ALWAYSTIP ); // 气泡提示风格

	//m_ToolTips.SetTipTextColor( RGB( 255, 96, 0 ) ); // 设置提示文本颜色

	// 枚举子窗口,并将其添加ToolTips
	CWnd * pChildWnd;
	if ( pChildWnd = this->GetWindow(GW_CHILD) ) do
	{
		m_ToolTips.AddTool(pChildWnd);
	}
	while ( pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT) );
	
	m_ToolTips.Activate(TRUE);

	return bRet;
}
