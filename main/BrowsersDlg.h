#pragma once
#include "Dialog.h"

// BrowsersDlg 对话框

class BrowsersDlg : public Dialog, public IUpdateListView
{
public:
    BrowsersDlg( CWnd * pParent = nullptr );   // 标准构造函数
    virtual ~BrowsersDlg();

// 对话框数据
    enum { IDD = IDD_BROWSERS };

    // 更新列表
    virtual void UpdateList( int flag = UPDATE_LOAD_DATA | UPDATE_LIST_ITEMS, long itemIndex = -1 ) override;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();

    BrowserArray m_browsers;

    afx_msg void OnListActivated( NMHDR *pNMHDR, LRESULT *pResult );
    afx_msg void OnListRClick( NMHDR *pNMHDR, LRESULT *pResult );
    afx_msg void OnBrowserAdd();
    afx_msg void OnBrowserModify();
    afx_msg void OnBrowserDelete();
    afx_msg void OnUpdateBrowserMenu( CCmdUI * pCmdUI );

    DECLARE_MESSAGE_MAP()
    friend class MainFrame;
};
