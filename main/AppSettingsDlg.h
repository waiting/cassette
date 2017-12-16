#if !defined(AFX_APPSETTINGSDLG_H__9A1ADEF2_A70D_4B93_ACA5_C665AF0151BF__INCLUDED_)
#define AFX_APPSETTINGSDLG_H__9A1ADEF2_A70D_4B93_ACA5_C665AF0151BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AppSettingsDlg.h : header file
#include "Dialog.h"

/////////////////////////////////////////////////////////////////////////////
// AppSettingsDlg dialog

class AppSettingsDlg : public Dialog
{
// Construction
public:
    AppSettingsDlg( CWnd * pParent, CassetteApp::SettingsParameters * pSettings );

// Dialog Data
    //{{AFX_DATA(AppSettingsDlg)
    enum { IDD = IDD_APP_SETTINGS };
    BOOL    m_IsEnabledAutoRun;
    BOOL    m_IsEnabledHotkey;
    BOOL    m_IsEnabledHttpSrv;
    BOOL    m_IsEnabledScheme;
    CString m_BackupPath;
    CString m_DatabasePath;
    CString m_WordslibPath;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(AppSettingsDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    CassetteApp::SettingsParameters * m_pSettings;
    // Generated message map functions
    //{{AFX_MSG(AppSettingsDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPSETTINGSDLG_H__9A1ADEF2_A70D_4B93_ACA5_C665AF0151BF__INCLUDED_)
