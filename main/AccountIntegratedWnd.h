//////////////////////////////////////////////////////////////////////////
// �˻��ۺϴ���,��ʾһ���˻������µ��˻�
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
	//���´���������ض���
	void RefreshAllCreate();
	void MakeDraw();
	void Draw();


	WindowTimer m_timer1;
	/* ��ͼ���� */
	CRect m_rcClient; // �ͻ�������
	CPoint m_ptCurMouse; // ��ǰ���λ��
	int m_radiusMouseCircle; // �����ЧԲ�ΰ뾶
	RectF m_captionRect; // �������

	/* ��װ����Ч���� */
	// ����һ����ɫ��Ӱ����
	void DrawShadowString(
		String const & s,
		Font const & font,
		RectF const & layoutRect,
		StringFormat const & fmt,
		RectF * boundingRect
	);
	// ����һ����ɫ��ӰԲ�ǵĿ��
	void DrawShadowFrame( RectF const & rect );
	// ����һ����ɫԲ�Ǳ���
	void DrawBackground( RectF const & rect );

	// �洢�Ѿ��ڵ�ǰ��������ʾ�ı�����,ȷ��ÿ����ǰ������ֻ��ʾһ�α�����
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
