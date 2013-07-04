#if !defined(AFX_APPSETTINGSDLG_H__9A1ADEF2_A70D_4B93_ACA5_C665AF0151BF__INCLUDED_)
#define AFX_APPSETTINGSDLG_H__9A1ADEF2_A70D_4B93_ACA5_C665AF0151BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AppSettingsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AppSettingsDlg dialog

class AppSettingsDlg : public CDialog
{
// Construction
public:
	AppSettingsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AppSettingsDlg)
	enum { IDD = IDD_APP_SETTINGS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AppSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AppSettingsDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPSETTINGSDLG_H__9A1ADEF2_A70D_4B93_ACA5_C665AF0151BF__INCLUDED_)
