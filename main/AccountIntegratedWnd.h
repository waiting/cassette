
#if !defined(AFX_ACCOUNTINTEGRATEDWND_H__1FE251B8_B64F_47C2_B695_4BFB32FC6E83__INCLUDED_)
#define AFX_ACCOUNTINTEGRATEDWND_H__1FE251B8_B64F_47C2_B695_4BFB32FC6E83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// 账户综合窗口，显示一个账户种类下的账户
// AccountIntegratedWnd window
class AccountIntegratedWnd : public CWnd
{
protected:
    // 存储已经在当前窗口上显示的本窗口，确保每个当前窗口上只显示一次本窗口
    static std::map<HWND, AccountIntegratedWnd *> m_hasDisplayed;

public:
    static AccountIntegratedWnd * GetDisplayedWnd( HWND hWnd )
    {
        if ( m_hasDisplayed.find(hWnd) != m_hasDisplayed.end() )
            return m_hasDisplayed[hWnd];
        return NULL;
    }

    static void AddDisplayedWnd( HWND hWnd, AccountIntegratedWnd * pIntegratedWnd )
    {
        m_hasDisplayed[hWnd] = pIntegratedWnd;
    }

    static void DelDisplayedWnd( AccountIntegratedWnd * pIntegratedWnd )
    {
        std::map<HWND, AccountIntegratedWnd *>::iterator it;
        for ( it = m_hasDisplayed.begin(); it != m_hasDisplayed.end(); ++it )
        {
            if ( it->second == pIntegratedWnd )
            {
                m_hasDisplayed.erase(it->first);
                break;
            }
        }
    }

public:
    AccountIntegratedWnd( CWnd * pParentWnd, LPCTSTR lpszWindowName, const RECT& rect );
    virtual ~AccountIntegratedWnd();

protected:
    // 重新创建所有相关对象
    void RefreshAllCreate();
    // 重新显示账户信息
    void SetAccountsInfo( AccountCate const & cate, AccountArray const & accounts );
    // 作画
    void MakeDraw();

    winplus::WindowTimer m_timer1; // 定时器

    /* 账户信息 */
    AccountCate m_cate;
    AccountArray m_accounts;

    /* 绘图数据 */
    winplus::SimplePointer<Bitmap> m_loadedBgImg; // 背景图
    winplus::SimplePointer<Graphics> m_gCanvas; // 绘图对象
    winplus::MemImage m_memCanvas; // 内存画布
    //winplus::MemDC m_memCanvas; // 内存画布

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

    /* 封装的特效方法 */
    // 绘制一个白色阴影文字
    void DrawShadowString(
        winplus::String const & s,
        Gdiplus::Font const & font,
        RectF const & layoutRect,
        StringFormat const & fmt,
        RectF * boundingRect
    );
    // 绘制一个白色阴影圆角的框架
    void DrawShadowFrame( RectF const & rect );
    // 绘制一个白色圆角背景
    void DrawBackground( RectF const & rect );


    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(AccountIntegratedWnd)
    protected:
    virtual void PostNcDestroy();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    //}}AFX_VIRTUAL

protected:
    //{{AFX_MSG(AccountIntegratedWnd)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
    afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCOUNTINTEGRATEDWND_H__1FE251B8_B64F_47C2_B695_4BFB32FC6E83__INCLUDED_)
