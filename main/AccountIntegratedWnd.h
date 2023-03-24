
#if !defined(AFX_ACCOUNTINTEGRATEDWND_H__1FE251B8_B64F_47C2_B695_4BFB32FC6E83__INCLUDED_)
#define AFX_ACCOUNTINTEGRATEDWND_H__1FE251B8_B64F_47C2_B695_4BFB32FC6E83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// �˻��ۺϴ��ڣ���ʾһ���˻������µ��˻�
// AccountIntegratedWnd window
class AccountIntegratedWnd : public CWnd
{
protected:
    // �洢�Ѿ��ڵ�ǰ��������ʾ�ı����ڣ�ȷ��ÿ����ǰ������ֻ��ʾһ�α�����
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
    // ���´���������ض���
    void RefreshAllCreate();
    // ������ʾ�˻���Ϣ
    void SetAccountsInfo( AccountCate const & cate, AccountArray const & accounts );
    // ����
    void MakeDraw();

    winplus::WindowTimer m_timer1; // ��ʱ��

    /* �˻���Ϣ */
    AccountCate m_cate;
    AccountArray m_accounts;

    /* ��ͼ���� */
    winplus::SimplePointer<Bitmap> m_loadedBgImg; // ����ͼ
    winplus::SimplePointer<Graphics> m_gCanvas; // ��ͼ����
    winplus::MemImage m_memCanvas; // �ڴ滭��
    //winplus::MemDC m_memCanvas; // �ڴ滭��

    CRect m_rcClient; // �ͻ�������
    CPoint m_ptCurMouse; // ��ǰ���λ��
    int m_radiusMouseCircle; // �����ЧԲ�ΰ뾶
    RectF m_captionRect; // �������
    RectF m_contentRect; // ���ݾ���

    Gdiplus::Font m_primaryFont; // ������
    StringFormat m_sfHVCenter; // �ַ�����������ʽ
    Pen m_penBlack;
    Pen m_penHalfWhite;
    Pen m_penWhite;
    SolidBrush m_brushHalfWhite;
    SolidBrush m_brushBlack;
    SolidBrush m_brushWhite;

    /* ��װ����Ч���� */
    // ����һ����ɫ��Ӱ����
    void DrawShadowString(
        winplus::String const & s,
        Gdiplus::Font const & font,
        RectF const & layoutRect,
        StringFormat const & fmt,
        RectF * boundingRect
    );
    // ����һ����ɫ��ӰԲ�ǵĿ��
    void DrawShadowFrame( RectF const & rect );
    // ����һ����ɫԲ�Ǳ���
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
