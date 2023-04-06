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
    // �Զ�ɾ������,��new����ʱ�ֲ�������ڴ�,��������ΪTRUE
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
    // ��ȡָ��HWND�����ϵı��ര��
    static CWnd * GetWnd( HWND hWnd );

    // ����ָ��HWND�����ϵı��ര��
    static void SetWnd( HWND hWnd, CWnd * pWnd );

    // ɾ��ָ��HWND�����ϵı��ര��
    static void DelWnd( CWnd * pWnd );

    // Constructors
    SpwWindow( CWnd * pWndParent );

protected:
    virtual void PostNcDestroy();

protected:
    // �洢�Ѿ���HWND��������ʾ�ı��ര�ڣ�ȷ��ÿ��HWND������ֻ��ʾһ�����ര��
    static std::map<HWND, CWnd *> _hasDisplayed;

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDOW_H__E8E25A6B_5104_4140_89A1_D7F7DC3CA079__INCLUDED_)
