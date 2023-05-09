// BrowsersDlg.cpp: 实现文件
//

#include "Cassette.h"
#include "CassetteApp.h"
#include "BrowsersDlg.h"

// BrowsersDlg 对话框

BrowsersDlg::BrowsersDlg( CWnd * pParent ) : Dialog( BrowsersDlg::IDD, pParent )
{

}

BrowsersDlg::~BrowsersDlg()
{
}

void BrowsersDlg::DoDataExchange(CDataExchange* pDX)
{
	Dialog::DoDataExchange(pDX);
}

BOOL BrowsersDlg::OnInitDialog()
{
    Dialog::OnInitDialog();

    CListCtrl & lst = *(CListCtrl *)GetDlgItem(IDC_LIST_BROWSERS);

    // 设置ListView中ListCtrl的扩展风格
    lst.SetExtendedStyle(LVS_EX_FULLROWSELECT);

    CRect rectClient;
    GetClientRect(&rectClient);
    int width = rectClient.Width() - 1 - GetSystemMetrics(SM_CXVSCROLL);

    // 初始化列表头
    lst.InsertColumn( 0, _T("编号"), LVCFMT_LEFT, 40 );
    lst.InsertColumn( 1, _T("缩写"), LVCFMT_LEFT, 50 );
    lst.InsertColumn( 3, _T("EXE文件名"), LVCFMT_LEFT, 80 );
    lst.InsertColumn( 2, _T("标题"), LVCFMT_LEFT, width - 170 );

    LoadBrowsers( g_theApp.GetDatabase(), &m_browsers );

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


BEGIN_MESSAGE_MAP(BrowsersDlg, Dialog)
END_MESSAGE_MAP()


// BrowsersDlg 消息处理程序
