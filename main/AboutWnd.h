//////////////////////////////////////////////////////////////////////////
// 关于窗口
// author: WT
//////////////////////////////////////////////////////////////////////////
#if !defined(AFX_ABOUTWND_H__A30B4135_4A86_4F2E_8E2B_31F6D3FD6B1B__INCLUDED_)
#define AFX_ABOUTWND_H__A30B4135_4A86_4F2E_8E2B_31F6D3FD6B1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AboutWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AboutWnd frame

class AboutWnd : public CFrameWnd
{
private:
    static AboutWnd * ms_pSingleAboutWnd;
protected:
    AboutWnd( CWnd * parent = NULL );
    virtual ~AboutWnd();
public:
    // 获取关于对话框的实例
    static AboutWnd * GetSingleton( CWnd * pParent = NULL );

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(AboutWnd)
protected:
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    //}}AFX_VIRTUAL

// Implementation
protected:
    AboutView * m_pAboutView;

    // Generated message map functions
    //{{AFX_MSG(AboutWnd)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUTWND_H__A30B4135_4A86_4F2E_8E2B_31F6D3FD6B1B__INCLUDED_)
