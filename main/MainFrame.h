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

// 更新所有列表,wParam指示要更新的列表
#define WM_UPDATELIST_ALL ( WM_USER + 1 )
#define WM_TRAY_NOTIFICATION  ( WM_USER + 2 )

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

protected:
    CStatusBar m_statusBar;
    CToolBar m_toolBar;
    class AccountsView * m_pAccountsView;
    AccountCatesDlg m_catesDlg;
    AccountTypesDlg m_typesDlg;
    winplus::Notification m_noti;

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
    // 弹出添加账户的对话框
    void DoAddAccount( CWnd * parent, winplus::Mixed & newAccount );

    // 执行智能热键操作
    void DoIntelligentHotkey();

public:
    // wParam = 1:主界面List更新
    // wParam = 2:TypesDlg界面List更新
    // wParam = 4:CatesDlg界面List更新
    enum // for wParam
    {
        UpdateList_Main = 1,
        UpdateList_TypesDlg = 2,
        UpdateList_CatesDlg = 4,
    };
protected:
    DECLARE_MESSAGE_MAP()
protected:
    //{{AFX_MSG(MainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSetFocus(CWnd *pOldWnd);
    afx_msg void OnAppExit();
    afx_msg void OnAppAbout();
    afx_msg void OnUpdateViaAutoLogin(CCmdUI* pCmdUI);
    afx_msg void OnViaAutoLogin();
    afx_msg void OnUpdateOpenUrl(CCmdUI* pCmdUI);
    afx_msg void OnOpenUrl();
    afx_msg void OnUserSettings();
    afx_msg void OnAppSettings();
    afx_msg void OnClose();
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
    afx_msg void OnMainWndShowHide();
    afx_msg void OnUpdateMainWndShowHide(CCmdUI *pCmdUI);
    //}}AFX_MSG
    afx_msg LRESULT OnHotkey( WPARAM wHotkeyId, LPARAM lParam );
    afx_msg void OnUpdateOperateAccount( CCmdUI * pCmdUI );

    // 调用所有UpdateList();
    afx_msg LRESULT OnUpdateListAll( WPARAM wParam, LPARAM lParam );

    afx_msg LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__079DE26A_055A_40BD_86F1_153BE92E92E8__INCLUDED_)
