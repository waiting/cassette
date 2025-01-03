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
// html Data
public:
    AboutView( UINT uHtmlResId );           // protected constructor used by dynamic creation
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
protected:
    virtual void OnInitialUpdate();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnBeforeNavigate2( LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel );
    //}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    UINT _uHtmlResId;

    // Generated message map functions
    //{{AFX_MSG(AboutView)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUTVIEW_H__16F2BDAA_4B12_4D0D_8FD5_CAA50258DAB0__INCLUDED_)
