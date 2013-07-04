//////////////////////////////////////////////////////////////////////////
// 关于窗口的视图(显示一个HTML文档)
// author: WT
//////////////////////////////////////////////////////////////////////////
#if !defined(AFX_ABOUTVIEW_H__16F2BDAA_4B12_4D0D_8FD5_CAA50258DAB0__INCLUDED_)
#define AFX_ABOUTVIEW_H__16F2BDAA_4B12_4D0D_8FD5_CAA50258DAB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AboutView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AboutView html view

class AboutView : public CHtmlView
{
	DECLARE_DYNCREATE(AboutView)

// html Data
public:
	AboutView();           // protected constructor used by dynamic creation
	virtual ~AboutView();
	
	//{{AFX_DATA(AboutView)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AboutView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generated message map functions
	//{{AFX_MSG(AboutView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUTVIEW_H__16F2BDAA_4B12_4D0D_8FD5_CAA50258DAB0__INCLUDED_)
