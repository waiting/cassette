#if !defined(AFX_ACCOUNTEDITINGDLG_H__C31AD041_A78D_40D2_8FF2_4F9896B29BF4__INCLUDED_)
#define AFX_ACCOUNTEDITINGDLG_H__C31AD041_A78D_40D2_8FF2_4F9896B29BF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AccountEditingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AccountEditingDlg dialog

class AccountEditingDlg : public CDialog
{
// Construction
public:
	AccountEditingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AccountEditingDlg)
	enum { IDD = IDD_ACCOUNT_EDTING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AccountEditingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AccountEditingDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCOUNTEDITINGDLG_H__C31AD041_A78D_40D2_8FF2_4F9896B29BF4__INCLUDED_)
