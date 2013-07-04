#if !defined(AFX_ACCOUNTCATESDLG_H__1B86D5E0_26A4_414F_9137_379A99BE9FB7__INCLUDED_)
#define AFX_ACCOUNTCATESDLG_H__1B86D5E0_26A4_414F_9137_379A99BE9FB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AccountCatesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AccountCatesDlg dialog

class AccountCatesDlg : public CDialog
{
// Construction
public:
	AccountCatesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AccountCatesDlg)
	enum { IDD = IDD_ACCOUNT_CATES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AccountCatesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AccountCatesDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCOUNTCATESDLG_H__1B86D5E0_26A4_414F_9137_379A99BE9FB7__INCLUDED_)
