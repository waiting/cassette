#if !defined(AFX_USERSETTINGSDLG_H__73C19992_118B_429D_8567_82E9B7F2C475__INCLUDED_)
#define AFX_USERSETTINGSDLG_H__73C19992_118B_429D_8567_82E9B7F2C475__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UserSettingsDlg.h : header file
#include "Dialog.h"

/////////////////////////////////////////////////////////////////////////////
// UserSettingsDlg dialog

class UserSettingsDlg : public Dialog
{
// Construction
public:
	UserSettingsDlg(
		CWnd * pParent,
		CString * username,
		CString * password,
		int * protectLevel,
		int * condone,
		int * hotkey
	);

	// «∑Ò–ﬁ∏ƒ√‹¬Î
	bool IsModifyPassword() const;

// Dialog Data
	//{{AFX_DATA(UserSettingsDlg)
	enum { IDD = IDD_USER_SETTINGS };
	CString	m_oldPassword;
	CString	m_newPassword;
	CString	m_cfmPassword;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UserSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString * m_username;
	CString * m_password;
	int * m_protectLevel;
	int * m_condone;
	int * m_hotkey;

	// Generated message map functions
	//{{AFX_MSG(UserSettingsDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERSETTINGSDLG_H__73C19992_118B_429D_8567_82E9B7F2C475__INCLUDED_)
