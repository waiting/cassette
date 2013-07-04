// AboutDlg.cpp: implementation of the AboutDlg class.
//
//////////////////////////////////////////////////////////////////////
#include "Cassette.h"
#include "CassetteApp.h"
#include "AboutDlg.h"
#include <commctrl.h>

AboutDlg::AboutDlg() : CDialog(AboutDlg::IDD)
{
	//{{AFX_DATA_INIT(AboutDlg)
	//}}AFX_DATA_INIT
}

void AboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(AboutDlg, CDialog)
//{{AFX_MSG_MAP(AboutDlg)
	// No message handlers
	ON_NOTIFY( NM_CLICK, IDC_SYSLINK1, OnSysLink1 )
	ON_NOTIFY( NM_RETURN, IDC_SYSLINK1, OnSysLink1 )
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void AboutDlg::OnSysLink1( NMHDR * pNmHdr, LRESULT * pResult )
{
	PNMLINK pNmLink = reinterpret_cast<PNMLINK>(pNmHdr);
	//msgbox( unicode_to_string(pNmLink->item.szID) );
	ShellExecute( NULL, TEXT("open"), unicode_to_string(pNmLink->item.szUrl).c_str(), NULL, NULL, SW_NORMAL );
	GetDlgItem(IDOK)->SetFocus();
	*pResult = 0;
}
