// BrowserEditingDlg.cpp: 实现文件
//

#include "Cassette.h"
#include "CassetteApp.h"
#include "BrowserEditingDlg.h"


// BrowserEditingDlg 对话框

BrowserEditingDlg::BrowserEditingDlg( CWnd* pParent ) : Dialog(BrowserEditingDlg::IDD, pParent)
{

}

BrowserEditingDlg::~BrowserEditingDlg()
{
}

void BrowserEditingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL BrowserEditingDlg::OnInitDialog()
{
    Dialog::OnInitDialog();

    ES_CENTER;

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

BEGIN_MESSAGE_MAP(BrowserEditingDlg, CDialog)
END_MESSAGE_MAP()


// BrowserEditingDlg 消息处理程序

