
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
			pTTT->lpszText = MAKEINTRESOURCE(nID); // ����ͬID���ַ�����Դ����ʾ�ı�
			pTTT->hinst = AfxGetResourceHandle();
			return TRUE;
		}
	}
	return FALSE;
	
}

BOOL Dialog::PreTranslateMessage( MSG * pMsg ) 
{
	// ��ToolTips Ctrl����ϵͳ������ToolTips�����Ϣ
	if ( m_ToolTips )
	{
		m_ToolTips.RelayEvent(pMsg);
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL Dialog::OnInitDialog()
{
	BOOL bRet = CDialog::OnInitDialog();

	//this->EnableToolTips(TRUE); // ���ô����Դ���ToolTips����

	m_ToolTips.Create( this, TTS_BALLOON | TTS_ALWAYSTIP ); // ������ʾ���

	//m_ToolTips.SetTipTextColor( RGB( 255, 96, 0 ) ); // ������ʾ�ı���ɫ

	// ö���Ӵ���,���������ToolTips
	CWnd * pChildWnd;
	if ( pChildWnd = this->GetWindow(GW_CHILD) ) do
	{
		m_ToolTips.AddTool(pChildWnd);
	}
	while ( pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT) );
	
	m_ToolTips.Activate(TRUE);

	return bRet;
}
