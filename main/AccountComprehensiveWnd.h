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
    winplus::MemDC _memdc;
public:
    afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
    afx_msg void OnPaint();
};


