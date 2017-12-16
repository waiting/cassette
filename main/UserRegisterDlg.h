#if !defined(AFX_USERREGISTERDLG_H__A5977CE7_20F2_4F99_9651_D6E0502A561A__INCLUDED_)
#define AFX_USERREGISTERDLG_H__A5977CE7_20F2_4F99_9651_D6E0502A561A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UserRegisterDlg.h : header file
#include "Dialog.h"

/////////////////////////////////////////////////////////////////////////////
// UserRegisterDlg dialog

class UserRegisterDlg : public Dialog
{
// Construction
public:
    UserRegisterDlg( CWnd * parent, User * user );

// Dialog Data
    //{{AFX_DATA(UserRegisterDlg)
    enum { IDD = IDD_USER_REGISTER };
    CString m_cfmPassword;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(UserRegisterDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    User * m_user;

    // Generated message map functions
    //{{AFX_MSG(UserRegisterDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERREGISTERDLG_H__A5977CE7_20F2_4F99_9651_D6E0502A561A__INCLUDED_)
