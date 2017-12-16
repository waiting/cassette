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
    AccountCatesDlg( CWnd * parent = NULL );   // standard constructor

// Dialog Data
    //{{AFX_DATA(AccountCatesDlg)
    enum { IDD = IDD_ACCOUNT_CATES };
        // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    virtual void UpdateList( int flag = UPDATE_LOAD_DATA | UPDATE_LIST_ITEMS, long itemIndex = -1 );
    // 弹出添加对话框
    void DoAdd( CWnd * parent, AccountCate * cate );
    // 根据窗口标题和关键字匹配以及是否为浏览器判断是哪个种类
    // 返回种类在数组中的索引
    int GetCateIndexMatchWndTitle( CString const & wndTitle, bool isBrowser ) const;

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(AccountCatesDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    AccountCateArray m_cates;// 存储种类信息的数组

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
    friend class MainFrame;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCOUNTCATESDLG_H__1B86D5E0_26A4_414F_9137_379A99BE9FB7__INCLUDED_)
