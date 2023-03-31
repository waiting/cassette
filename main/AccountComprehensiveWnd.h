#pragma once


// AccountComprehensiveWnd

class AccountComprehensiveWnd : public CWnd
{
    DECLARE_DYNAMIC(AccountComprehensiveWnd)

public:
    AccountComprehensiveWnd( CWnd * pParentWnd, winplus::String const & strWindowName, CRect const & rect );
    virtual ~AccountComprehensiveWnd();

protected:
    DECLARE_MESSAGE_MAP()
};


