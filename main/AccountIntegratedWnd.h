
#if !defined(AFX_ACCOUNTINTEGRATEDWND_H__1FE251B8_B64F_47C2_B695_4BFB32FC6E83__INCLUDED_)
#define AFX_ACCOUNTINTEGRATEDWND_H__1FE251B8_B64F_47C2_B695_4BFB32FC6E83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Window.h"

// 账户信息窗口，显示一个账户种类下的账户
class AccountIntegratedWnd : public SpwWindow
{
public:
    AccountIntegratedWnd( CWnd * pParentWnd, LPCTSTR lpszWindowName, const RECT& rect );
    virtual ~AccountIntegratedWnd();

    // 重新显示账户信息
    void SetAccountsInfo( AccountCate const & cate, AccountArray const & accounts );

protected:
    // 重新创建所有相关对象
    void RefreshAllCreate();
    // 作画
    void MakeDraw();

    winplus::WindowTimer m_timer1; // 定时器

    /* 账户信息 */
    AccountCate m_cate;
    struct AccountContext
    {
        // 账户
        Account account;
        // 账户绘画区域
        RectF accountRect;
        // 账户字段显示区域
        RectF accountField0Rect;
        RectF accountField1Rect;
        // 是否选中
        //bool isSelected;
        // 是否显示密码
        bool isPwdShown;
        AccountContext()
        {
            this->_zeroInit();
        }
        AccountContext( Account const & account ) : account(account)
        {
            this->_zeroInit();
        }
        void _zeroInit()
        {
            using MyRect = decltype(accountRect);
            ZeroMemory( &accountRect, sizeof(MyRect) );
            ZeroMemory( &accountField0Rect, sizeof(MyRect) );
            ZeroMemory( &accountField1Rect, sizeof(MyRect) );
            //isSelected = false;
            isPwdShown = false;
        }
    };
    CArray<AccountContext> m_accounts;

    /* 绘图数据 */
    winplus::SimplePointer<Bitmap> m_loadedBgImg; // 背景图
    winplus::SimplePointer<winplus::Graphics> m_gCanvas; // 绘图对象
    winplus::MemImage m_memCanvas; // 内存画布

    CRect m_rcClient; // 客户区矩形
    CPoint m_ptCurMouse; // 当前鼠标位置
    int m_radiusMouseCircle; // 鼠标特效圆形半径
    RectF m_captionRect; // 标题矩形
    RectF m_contentRect; // 内容矩形

    Gdiplus::Font m_primaryFont; // 主字体
    StringFormat m_sfHVCenter; // 字符串输出对齐格式
    Pen m_penBlack;
    Pen m_penHalfWhite;
    Pen m_penWhite;
    SolidBrush m_brushHalfWhite;
    SolidBrush m_brushBlack;
    SolidBrush m_brushWhite;

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(AccountIntegratedWnd)
protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    //}}AFX_VIRTUAL

protected:
    //{{AFX_MSG(AccountIntegratedWnd)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
    afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
    afx_msg void OnRButtonDown( UINT nFlags, CPoint point );
    afx_msg void OnRButtonUp( UINT nFlags, CPoint point );
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCOUNTINTEGRATEDWND_H__1FE251B8_B64F_47C2_B695_4BFB32FC6E83__INCLUDED_)
