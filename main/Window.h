#if !defined(AFX_WINDOW_H__E8E25A6B_5104_4140_89A1_D7F7DC3CA079__INCLUDED_)
#define AFX_WINDOW_H__E8E25A6B_5104_4140_89A1_D7F7DC3CA079__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Window.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Window window has auto delete setting
class Window : public CWnd
{
// Construction
public:
	Window();
	virtual ~Window();

// Attributes
public:
    // 自动删除对象,当new分配时又不想管理内存,请设置其为TRUE
	void AutoDelete( BOOL bAutoDel ) { _bAutoDelete = bAutoDel; }
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Window)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation

	// Generated message map functions
protected:
	BOOL _bAutoDelete;
	//{{AFX_MSG(Window)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

// Single window per Window
class SpwWindow : public Window
{
public:
    // 获取指定HWND窗口上的本类窗口
    static CWnd * GetWnd( HWND hWnd );

    // 设置指定HWND窗口上的本类窗口
    static void SetWnd( HWND hWnd, CWnd * pWnd );

    // 删除指定HWND窗口上的本类窗口
    static void DelWnd( CWnd * pWnd );

    // Constructors
    SpwWindow( CWnd * pWndParent );

protected:
    virtual void PostNcDestroy();

protected:
    // 存储已经在HWND窗口上显示的本类窗口，确保每个HWND窗口上只显示一个本类窗口
    static std::map<HWND, CWnd *> _hasDisplayed;

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDOW_H__E8E25A6B_5104_4140_89A1_D7F7DC3CA079__INCLUDED_)
