#pragma once
#include "Dialog.h"

// BrowserEditingDlg �Ի���

class BrowserEditingDlg : public Dialog
{
public:
    BrowserEditingDlg(CWnd* pParent = nullptr);   // ��׼���캯��
    virtual ~BrowserEditingDlg();

    // �Ի�������
    enum { IDD = IDD_BROWSER_EDITING };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()
};
