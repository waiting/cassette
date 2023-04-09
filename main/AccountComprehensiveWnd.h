#pragma once

// AccountComprehensiveWnd
class AccountComprehensiveWnd : public CWnd
{
    DECLARE_DYNAMIC(AccountComprehensiveWnd)

public:
    AccountComprehensiveWnd( CWnd * pParentWnd, winplus::String const & strWindowName, CRect const & rect );
    virtual ~AccountComprehensiveWnd();

protected:
    virtual BOOL PreCreateWindow( CREATESTRUCT & cs );
    virtual void PostNcDestroy();

    DECLARE_MESSAGE_MAP()

private:
    winplus::MembersWrapper<struct AccountComprehensive_Data> _self;
public:
    afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
    afx_msg void OnPaint();
    afx_msg LRESULT OnNcHitTest( CPoint point );
    afx_msg void OnMouseMove( UINT nFlags, CPoint point );
};


