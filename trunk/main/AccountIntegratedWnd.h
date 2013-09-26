//////////////////////////////////////////////////////////////////////////
// 账户综合窗口,显示一个账户种类下的账户
// author: WT
//////////////////////////////////////////////////////////////////////////
#if !defined(AFX_ACCOUNTINTEGRATEDWND_H__1FE251B8_B64F_47C2_B695_4BFB32FC6E83__INCLUDED_)
#define AFX_ACCOUNTINTEGRATEDWND_H__1FE251B8_B64F_47C2_B695_4BFB32FC6E83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AccountIntegratedWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AccountIntegratedWnd window

class AccountIntegratedWnd : public CFrameWnd
{
public:
	AccountIntegratedWnd( CWnd * pParentWnd, LPCTSTR lpszWindowName, DWORD dwStyle, DWORD dwExStyle, const RECT& rect );

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AccountIntegratedWnd)
	protected:
	virtual void PostNcDestroy();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

public:
	virtual ~AccountIntegratedWnd();

protected:
	std::auto_ptr<Graphics> m_gDevice;
	std::auto_ptr<Graphics> m_gCached;
	MemImage m_imgCached;
	std::auto_ptr<CachedBitmap> m_CBCached;
	std::auto_ptr<Bitmap> m_loadedBgImg;
	HDC m_hClientDC;
	//重新创建所有相关对象
	void RefreshAllCreate();
	void MakeDraw();
	void Draw();


	WindowTimer m_timer1;
	/* 绘图数据 */
	CRect m_rcClient; // 客户区矩形
	CPoint m_ptCurMouse; // 当前鼠标位置
	int m_radiusMouseCircle; // 鼠标特效圆形半径
	RectF m_captionRect; // 标题矩形

	/* 封装的特效方法 */
	// 绘制一个白色阴影文字
	void DrawShadowString(
		String const & s,
		Font const & font,
		RectF const & layoutRect,
		StringFormat const & fmt,
		RectF * boundingRect
	);
	// 绘制一个白色阴影圆角的框架
	void DrawShadowFrame( RectF const & rect );
	// 绘制一个白色圆角背景
	void DrawBackground( RectF const & rect );

	// 存储已经在当前窗口上显示的本窗口,确保每个当前窗口上只显示一次本窗口
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
	//{{AFX_MSG(AccountIntegratedWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCOUNTINTEGRATEDWND_H__1FE251B8_B64F_47C2_B695_4BFB32FC6E83__INCLUDED_)
