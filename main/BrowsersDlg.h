#pragma once
#include "Dialog.h"

// BrowsersDlg �Ի���

class BrowsersDlg : public Dialog
{
public:
    BrowsersDlg( CWnd * pParent = nullptr );   // ��׼���캯��
    virtual ~BrowsersDlg();

// �Ի�������
    enum { IDD = IDD_BROWSERS };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    virtual BOOL OnInitDialog();

    BrowserArray m_browsers;

    DECLARE_MESSAGE_MAP()

};
