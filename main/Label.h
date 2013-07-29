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

	/*Ӧ�øı�,ʹ���������Դ�������ͻ�ˢ*/
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
	CFont m_font;     // ����
	CString m_strFontName; // ��������
	UINT m_uFontSize;   // �ֺ�
	COLORREF m_color; // ������ɫ

	BOOL m_bBrushChanged;
	CBrush m_brush;   // ����
	COLORREF m_bkColor; // ������ɫ

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
