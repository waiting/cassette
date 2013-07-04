#if !defined(AFX_USERLOGINDLG_H__3F7B129B_3422_4FD7_975B_D7B35FEFAF22__INCLUDED_)
#define AFX_USERLOGINDLG_H__3F7B129B_3422_4FD7_975B_D7B35FEFAF22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UserLoginDlg.h : header file
//
#include "Dialog.h"
#include "Label.h"
/////////////////////////////////////////////////////////////////////////////
// UserLoginDlg dialog

class UserLoginDlg : public Dialog
{
// Construction
public:
	UserLoginDlg(
		CWnd * pParent,
		CString * username,
		CString * password,
		BOOL * autoLogin,
		BOOL * savePwd
	);

// Dialog Data
	//{{AFX_DATA(UserLoginDlg)
	enum { IDD = IDD_USER_LOGIN };
	Label	m_lblLogin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UserLoginDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString * m_username;
	CString * m_password;
	BOOL * m_autoLogin;
	BOOL * m_savePwd;

	// Generated message map functions
	//{{AFX_MSG(UserLoginDlg)
	afx_msg void OnSysLinkRegister( NMHDR * pNmHdr, LRESULT * pResult );
	afx_msg void OnSysLinkForgetPwd( NMHDR * pNmHdr, LRESULT * pResult );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERLOGINDLG_H__3F7B129B_3422_4FD7_975B_D7B35FEFAF22__INCLUDED_)
