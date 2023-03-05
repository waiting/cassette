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

// ���������б�,wParamָʾҪ���µ��б�
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
    // ����ͼ������Ӧ����
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
    // ���ñ�����ʾ��ǰ�û���
    void UpdateTitle();
    /* ע���ȼ��Ĺ���
       ��settingsΪ�����ȼ�ʱ:
         ���ݵ�hotkey��Ϊ0ʱ,�����ǰע����ȼ�,Ȼ��ע��hotkeyָ�����ȼ�
         ���ݵ�hotkeyΪ0ʱ,�����ǰע����ȼ�
       ��settingsΪ�ر��ȼ�ʱ:
         �����ǰע����ȼ�, hotkey���� */
    void RefreshHotkey( int hotkey );
    // ��������˻��ĶԻ���
    void DoAddAccount( CWnd * parent, winplus::Mixed & newAccount );

    // ִ�������ȼ�����
    void DoIntelligentHotkey();

public:
    // wParam = 1:������List����
    // wParam = 2:TypesDlg����List����
    // wParam = 4:CatesDlg����List����
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

    // ��������UpdateList();
    afx_msg LRESULT OnUpdateListAll( WPARAM wParam, LPARAM lParam );

    afx_msg LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__079DE26A_055A_40BD_86F1_153BE92E92E8__INCLUDED_)
