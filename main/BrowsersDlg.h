#pragma once
#include "Dialog.h"

// BrowsersDlg 对话框

class BrowsersDlg : public Dialog
{
public:
    BrowsersDlg( CWnd * pParent = nullptr );   // 标准构造函数
    virtual ~BrowsersDlg();

// 对话框数据
    enum { IDD = IDD_BROWSERS };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();

    BrowserArray m_browsers;

    DECLARE_MESSAGE_MAP()

};
