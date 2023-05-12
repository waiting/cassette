#pragma once
#include "Dialog.h"

// BrowserEditingDlg 对话框

class BrowserEditingDlg : public Dialog
{
public:
    BrowserEditingDlg( CWnd * pParent, bool isAdd, Mixed * browserFields );   // 标准构造函数
    virtual ~BrowserEditingDlg();

    // 对话框数据
    enum { IDD = IDD_BROWSER_EDITING };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();
    virtual void OnOK();

    CWnd * m_pWndParent;
    bool m_isAdd;
    Mixed * m_browser;

    DECLARE_MESSAGE_MAP()

};
