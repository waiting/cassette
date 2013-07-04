#if !defined(AFX_USERSETTINGSDLG_H__73C19992_118B_429D_8567_82E9B7F2C475__INCLUDED_)
#define AFX_USERSETTINGSDLG_H__73C19992_118B_429D_8567_82E9B7F2C475__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UserSettingsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// UserSettingsDlg dialog

class UserSettingsDlg : public CDialog
{
// Construction
public:
	UserSettingsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(UserSettingsDlg)
	enum { IDD = IDD_USER_SETTINGS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UserSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(UserSettingsDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERSETTINGSDLG_H__73C19992_118B_429D_8567_82E9B7F2C475__INCLUDED_)
