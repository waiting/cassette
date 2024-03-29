//////////////////////////////////////////////////////////////////////////
// 扩展对话框
// 1 支持工具提示
// author: WT
// date:   2013-07-04
//////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_H__20389200_7D5B_42F5_97C0_248E2F2BA142__INCLUDED_)
#define AFX_DIALOG_H__20389200_7D5B_42F5_97C0_248E2F2BA142__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dialog.h : header file
#include "Label.h"

/////////////////////////////////////////////////////////////////////////////
// Dialog dialog
// 自定义对话框，支持工具提示，菜单状态更新
class Dialog : public CDialog
{
// Construction
public:
    Dialog( UINT nIDTemplate, CWnd* pParentWnd = NULL );   // standard constructor

// Dialog Data
    //{{AFX_DATA(Dialog)
        // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    BOOL Create( CWnd * pParentWnd )
    {
        return CDialog::Create( m_nIDTemplate, pParentWnd );
    }

// Overrides
    // ClassWizard generated virtual function overrides
//{{AFX_VIRTUAL(Dialog)
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    virtual BOOL OnInitDialog();
//}}AFX_VIRTUAL

// Implementation
protected:
    UINT m_nIDTemplate; // 对话框模板资源ID
    CToolTipCtrl m_ToolTips; // 工具提示控件
    //std::vector< winplus::SimplePointer<Label> > m_allLabels; // 标签，用于子类化Static，自绘

    // Generated message map functions
    //{{AFX_MSG(Dialog)
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    //}}AFX_MSG
    afx_msg BOOL OnEraseBkgnd( CDC * pDC );

    // 当弹出菜单时调用，更新菜单项的状态
    afx_msg void OnInitMenuPopup( CMenu * pMenu, UINT nIndex, BOOL bSysMenu );

    // 当工具提示需要显示文本时调用
    afx_msg BOOL OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );

    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOG_H__20389200_7D5B_42F5_97C0_248E2F2BA142__INCLUDED_)
