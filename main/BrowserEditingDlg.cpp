// BrowserEditingDlg.cpp: ʵ���ļ�
//

#include "Cassette.h"
#include "CassetteApp.h"
#include "BrowserEditingDlg.h"


// BrowserEditingDlg �Ի���

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
                  // �쳣: OCX ����ҳӦ���� FALSE
}

BEGIN_MESSAGE_MAP(BrowserEditingDlg, CDialog)
END_MESSAGE_MAP()


// BrowserEditingDlg ��Ϣ�������

