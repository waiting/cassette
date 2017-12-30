#if !defined(AFX_ACCOUNTCATEEDITINGDLG_H__EFEA4C61_47F6_476A_8425_6C47342A336F__INCLUDED_)
#define AFX_ACCOUNTCATEEDITINGDLG_H__EFEA4C61_47F6_476A_8425_6C47342A336F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AccountCateEditingDlg.h : header file
#include "Dialog.h"

/////////////////////////////////////////////////////////////////////////////
// AccountCateEditingDlg dialog

class AccountCateEditingDlg : public Dialog
{
// Construction
public:
    AccountCateEditingDlg( CWnd * parent, bool isAdd, winux::Mixed * cateFields );

// Dialog Data
    //{{AFX_DATA(AccountCateEditingDlg)
    enum { IDD = IDD_ACCOUNT_CATE_EDITING };
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(AccountCateEditingDlg)
    protected:
    virtual void DoDataExchange( CDataExchange * pDX );    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    int     m_typeIndex;
    int     m_startupIndex;

    bool m_isAdd;
    winux::Mixed * m_cate;
    // Generated message map functions
    //{{AFX_MSG(AccountCateEditingDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCOUNTCATEEDITINGDLG_H__EFEA4C61_47F6_476A_8425_6C47342A336F__INCLUDED_)
