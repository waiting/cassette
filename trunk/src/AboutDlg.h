// AboutDlg.h: interface for the AboutDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ABOUTDLG_H__FF4280D1_4662_4F8F_9712_55AC8F652D26__INCLUDED_)
#define AFX_ABOUTDLG_H__FF4280D1_4662_4F8F_9712_55AC8F652D26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// AboutDlg dialog used for App About

class AboutDlg : public CDialog
{
public:
	AboutDlg();
	
	// Dialog Data
	//{{AFX_DATA(AboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(AboutDlg)
	// No message handlers
	afx_msg void OnSysLink1( NMHDR * pNmHdr, LRESULT * pResult );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_ABOUTDLG_H__FF4280D1_4662_4F8F_9712_55AC8F652D26__INCLUDED_)
