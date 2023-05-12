// BrowserEditingDlg.cpp: ʵ���ļ�
//

#include "Cassette.h"
#include "CassetteApp.h"
#include "BrowserEditingDlg.h"


// BrowserEditingDlg �Ի���

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
    // ���ñ���
    SetWindowText( m_isAdd ? _T("��������ʶ���¼...") : _T("�޸������ʶ���¼...") );


    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // �쳣: OCX ����ҳӦ���� FALSE
}

void BrowserEditingDlg::OnOK()
{
    UpdateData(TRUE);


    EndDialog(IDOK);
}

BEGIN_MESSAGE_MAP(BrowserEditingDlg, CDialog)
END_MESSAGE_MAP()


// BrowserEditingDlg ��Ϣ�������



