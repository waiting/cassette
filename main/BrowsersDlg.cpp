// BrowsersDlg.cpp: ʵ���ļ�
//

#include "Cassette.h"
#include "CassetteApp.h"
#include "BrowsersDlg.h"

// BrowsersDlg �Ի���

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

    // ����ListView��ListCtrl����չ���
    lst.SetExtendedStyle(LVS_EX_FULLROWSELECT);

    CRect rectClient;
    GetClientRect(&rectClient);
    int width = rectClient.Width() - 1 - GetSystemMetrics(SM_CXVSCROLL);

    // ��ʼ���б�ͷ
    lst.InsertColumn( 0, _T("���"), LVCFMT_LEFT, 40 );
    lst.InsertColumn( 1, _T("��д"), LVCFMT_LEFT, 50 );
    lst.InsertColumn( 3, _T("EXE�ļ���"), LVCFMT_LEFT, 80 );
    lst.InsertColumn( 2, _T("����"), LVCFMT_LEFT, width - 170 );

    LoadBrowsers( g_theApp.GetDatabase(), &m_browsers );

    return TRUE;  // return TRUE unless you set the focus to a control
                  // �쳣: OCX ����ҳӦ���� FALSE
}


BEGIN_MESSAGE_MAP(BrowsersDlg, Dialog)
END_MESSAGE_MAP()


// BrowsersDlg ��Ϣ�������
