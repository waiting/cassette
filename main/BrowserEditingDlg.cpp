// BrowserEditingDlg.cpp: 实现文件
//

#include "Cassette.h"
#include "CassetteApp.h"
#include "BrowserEditingDlg.h"


// BrowserEditingDlg 对话框

BrowserEditingDlg::BrowserEditingDlg( CWnd * pParent, bool isAdd, Mixed * browserFields ) :
    Dialog(BrowserEditingDlg::IDD, pParent),
    m_pWndParent(pParent),
    m_isAdd(isAdd),
    m_browser(browserFields)
{

}

BrowserEditingDlg::~BrowserEditingDlg()
{
}

void BrowserEditingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

    Browser browser;
    browser = *m_browser;
    DDX_Text(pDX, IDC_EDIT_BROWSERNAME, browser.m_browserName);
    DDX_Text(pDX, IDC_EDIT_BROWSERTITLE, browser.m_browserTitle);
    DDX_Text(pDX, IDC_EDIT_BROWSEREXE, browser.m_exeName);

    browser.assignTo( m_browser, "name,title,exe_name" );
}

BOOL BrowserEditingDlg::OnInitDialog()
{
    Dialog::OnInitDialog();
    // 设置标题
    SetWindowText( m_isAdd ? _T("添加浏览器识别记录...") : _T("修改浏览器识别记录...") );


    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

void BrowserEditingDlg::OnOK()
{
    UpdateData(TRUE);


    EndDialog(IDOK);
}

BEGIN_MESSAGE_MAP(BrowserEditingDlg, CDialog)
END_MESSAGE_MAP()


// BrowserEditingDlg 消息处理程序



