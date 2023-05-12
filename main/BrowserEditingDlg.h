#pragma once
#include "Dialog.h"

// BrowserEditingDlg �Ի���

class BrowserEditingDlg : public Dialog
{
public:
    BrowserEditingDlg( CWnd * pParent, bool isAdd, Mixed * browserFields );   // ��׼���캯��
    virtual ~BrowserEditingDlg();

    // �Ի�������
    enum { IDD = IDD_BROWSER_EDITING };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    virtual BOOL OnInitDialog();
    virtual void OnOK();

    CWnd * m_pWndParent;
    bool m_isAdd;
    Mixed * m_browser;

    DECLARE_MESSAGE_MAP()

};
