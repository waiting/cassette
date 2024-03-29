
#include "Cassette.h"
#include "CassetteApp.h"
#include "Label.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Label
Label::Label()
{
    m_bFontChanged = FALSE;
    m_strFontName = _T("΢���ź�");
    m_uFontSize = 12;
    m_color = RGB( 0, 0, 0 );


    m_bBrushChanged = FALSE;
    m_bkColor = GetSysColor(COLOR_3DFACE);
}

Label::~Label()
{

}


BEGIN_MESSAGE_MAP(Label, CStatic)
    //{{AFX_MSG_MAP(Label)
    ON_WM_CTLCOLOR_REFLECT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Label message handlers

HBRUSH Label::CtlColor(CDC* pDC, UINT nCtlColor) 
{
    pDC->SetTextColor(m_color);
    if ( m_font.GetSafeHandle() )
    {
        pDC->SelectObject(&m_font);
    }

    pDC->SetBkMode(TRANSPARENT);
    // TODO: Return a non-NULL brush if the parent's handler should not be called
    if ( m_brush.GetSafeHandle() )
    {
        return m_brush;
    }
    else
    {
        return (HBRUSH)GetStockObject(NULL_BRUSH);
    }
}
