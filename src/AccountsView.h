// ChildView.h : interface of the AccountsView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDVIEW_H__7450EEF0_270E_450D_B442_628F9161B81A__INCLUDED_)
#define AFX_CHILDVIEW_H__7450EEF0_270E_450D_B442_628F9161B81A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// AccountsView window

class AccountsView : public CListView
{
// Construction
public:
	AccountsView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AccountsView)
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~AccountsView();

	// Generated message map functions
protected:
	//{{AFX_MSG(AccountsView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__7450EEF0_270E_450D_B442_628F9161B81A__INCLUDED_)
