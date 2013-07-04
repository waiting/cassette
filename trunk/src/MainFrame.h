// MainFrm.h : interface of the MainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__079DE26A_055A_40BD_86F1_153BE92E92E8__INCLUDED_)
#define AFX_MAINFRM_H__079DE26A_055A_40BD_86F1_153BE92E92E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class MainFrame : public CFrameWnd
{
	
public:
	MainFrame();
	virtual ~MainFrame();
protected: 
	DECLARE_DYNAMIC(MainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	// 让视图优先响应命令
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar m_statusBar;
	CToolBar m_toolBar;
	AccountsView * m_pAccountsView;

// Generated message map functions
protected:
	//{{AFX_MSG(MainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnAppAbout();
	afx_msg void OnUpdateViaAutoLogin(CCmdUI* pCmdUI);
	afx_msg void OnViaAutoLogin();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__079DE26A_055A_40BD_86F1_153BE92E92E8__INCLUDED_)
