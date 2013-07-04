//////////////////////////////////////////////////////////////////////////
// 扩展对话框
// 1 支持工具提示
// author: WT
// date:   2013-07-04
//////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_H__20389200_7D5B_42F5_97C0_248E2F2BA142__INCLUDED_)
#define AFX_DIALOG_H__20389200_7D5B_42F5_97C0_248E2F2BA142__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Dialog dialog

class Dialog : public CDialog
{
// Construction
public:
	Dialog( UINT nIDTemplate, CWnd* pParentWnd = NULL );   // standard constructor

// Dialog Data
	//{{AFX_DATA(Dialog)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
//{{AFX_VIRTUAL(Dialog)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual BOOL OnInitDialog();
//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl m_ToolTips; // 工具提示控件

	// Generated message map functions
	//{{AFX_MSG(Dialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	// 当工具提示需要显示文本时调用
	afx_msg BOOL OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOG_H__20389200_7D5B_42F5_97C0_248E2F2BA142__INCLUDED_)
