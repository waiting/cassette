// MainFrm.h : interface of the MainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__079DE26A_055A_40BD_86F1_153BE92E92E8__INCLUDED_)
#define AFX_MAINFRM_H__079DE26A_055A_40BD_86F1_153BE92E92E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AccountCatesDlg.h"
#include "AccountTypesDlg.h"

class MainFrame : public CFrameWnd, public IUpdateListView
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
	virtual void UpdateList( int flag = UPDATE_LOAD_DATA | UPDATE_LIST_ITEMS, long itemIndex = -1 );
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
	AccountCatesDlg m_catesDlg;
	AccountTypesDlg m_typesDlg;

	BOOL m_TestFlag;
	// 设置标题显示当前用户名
	void UpdateTitle();
	/* 注册热键的功能
	   当settings为开启热键时:
	     传递的hotkey不为0时,清空先前注册的热键,然后注册hotkey指定的热键
	     传递的hotkey为0时,清空先前注册的热键
	   当settings为关闭热键时:
	     清空先前注册的热键, hotkey忽略 */
	void RefreshHotkey( int hotkey );
// Generated message map functions
protected:
	//{{AFX_MSG(MainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnAppAbout();
	afx_msg void OnUpdateViaAutoLogin(CCmdUI* pCmdUI);
	afx_msg void OnViaAutoLogin();
	afx_msg void OnUpdateOpenUrl(CCmdUI* pCmdUI);
	afx_msg void OnOpenUrl();
	afx_msg void OnUserSettings();
	afx_msg void OnAppSettings();
	afx_msg void OnDestroy();
	afx_msg void OnAddAccount();
	afx_msg void OnModifyAccount();
	afx_msg void OnDelAccount();
	afx_msg void OnUpdateAccountCates(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAccountTypes(CCmdUI* pCmdUI);
	afx_msg void OnAccountCates();
	afx_msg void OnAccountTypes();
	afx_msg void OnBackupData();
	afx_msg void OnResumeData();
	//}}AFX_MSG
	afx_msg LRESULT OnHotkey( WPARAM wParam, LPARAM lParam );
	afx_msg void OnUpdateOperateAccount( CCmdUI * pCmdUI );
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__079DE26A_055A_40BD_86F1_153BE92E92E8__INCLUDED_)
