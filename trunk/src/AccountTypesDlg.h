#if !defined(AFX_ACCOUNTTYPESDLG_H__BFB9B0BC_90D5_4205_98DE_84C757C2713A__INCLUDED_)
#define AFX_ACCOUNTTYPESDLG_H__BFB9B0BC_90D5_4205_98DE_84C757C2713A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AccountTypesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AccountTypesDlg dialog

class AccountTypesDlg : public CDialog
{
// Construction
public:
	AccountTypesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AccountTypesDlg)
	enum { IDD = IDD_ACCOUNT_TYPES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AccountTypesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AccountTypesDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCOUNTTYPESDLG_H__BFB9B0BC_90D5_4205_98DE_84C757C2713A__INCLUDED_)
