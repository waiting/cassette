#if !defined(AFX_LABEL_H__98C02A72_555B_48A6_A995_C8839CB1E7B3__INCLUDED_)
#define AFX_LABEL_H__98C02A72_555B_48A6_A995_C8839CB1E7B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Label.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Label window

class Label : public CStatic
{
// Construction
public:
	Label();
	virtual ~Label();

// Attributes
public:
	void SetFontName( LPCTSTR lpszFontName )
	{
		m_bFontChanged = TRUE;
		m_strFontName = lpszFontName;
	}
	void SetFontSize( UINT uFontSize )
	{
		m_bFontChanged = TRUE;
		m_uFontSize = uFontSize;
	}
	void SetColor( COLORREF color )
	{
		m_color = color;
	}
	void SetBackground( COLORREF bkColor )
	{
		m_bBrushChanged = TRUE;
		m_bkColor = bkColor;
	}

	/*应用改变,使用最新属性创建字体和画刷*/
	void ApplyChange()
	{
		if ( m_bFontChanged )
		{
			m_font.DeleteObject();
			m_font.CreatePointFont( m_uFontSize, m_strFontName );
			m_bFontChanged = FALSE;
		}

		if ( m_bBrushChanged )
		{
			m_brush.DeleteObject();
			m_brush.CreateSolidBrush(m_bkColor);
			m_bBrushChanged = FALSE;
		}
	}
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Label)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	BOOL m_bFontChanged;
	CFont m_font;     // 字体
	CString m_strFontName; // 字体名称
	UINT m_uFontSize;   // 字号
	COLORREF m_color; // 文字颜色

	BOOL m_bBrushChanged;
	CBrush m_brush;   // 背景
	COLORREF m_bkColor; // 背景颜色

	//{{AFX_MSG(Label)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LABEL_H__98C02A72_555B_48A6_A995_C8839CB1E7B3__INCLUDED_)
