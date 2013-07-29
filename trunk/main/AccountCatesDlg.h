#if !defined(AFX_ACCOUNTCATESDLG_H__1B86D5E0_26A4_414F_9137_379A99BE9FB7__INCLUDED_)
#define AFX_ACCOUNTCATESDLG_H__1B86D5E0_26A4_414F_9137_379A99BE9FB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AccountCatesDlg.h : header file
#include "Dialog.h"

/////////////////////////////////////////////////////////////////////////////
// AccountCatesDlg dialog

class AccountCatesDlg : public Dialog, public IUpdateListView
{
// Construction
public:
	AccountCatesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AccountCatesDlg)
	enum { IDD = IDD_ACCOUNT_CATES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	virtual void UpdateList( int flag = UPDATE_LOAD_DATA | UPDATE_LIST_ITEMS, long itemIndex = -1 );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AccountCatesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CUIntArray m_ids;
	CStringArray m_cateNames;
	CStringArray m_cateDescs;
	CStringArray m_typeNames;
	CStringArray m_urls;
	CStringArray m_icoPaths;
	CStringArray m_startups;
	CStringArray m_keywordss;
	CUIntArray m_timeWritens;

	// Generated message map functions
	//{{AFX_MSG(AccountCatesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnListActivated(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAdd();
	afx_msg void OnModify();
	afx_msg void OnDelete();
	//}}AFX_MSG
	afx_msg void OnUpdateModifyDeleteMenu( CCmdUI * pCmdUI );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCOUNTCATESDLG_H__1B86D5E0_26A4_414F_9137_379A99BE9FB7__INCLUDED_)
