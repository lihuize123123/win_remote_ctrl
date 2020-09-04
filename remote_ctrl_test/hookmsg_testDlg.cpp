
// hookmsg_testDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "hookmsg_test.h"
#include "hookmsg_testDlg.h"
#include "afxdialogex.h"
#include "CCtrlPanel.h"
#include "remote_ctrl.h"
#include "udp_ctrl_channel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// ChookmsgtestDlg 对话框



ChookmsgtestDlg::ChookmsgtestDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_HOOKMSG_TEST_DIALOG, pParent), m_connected(false), m_startService(false), m_ioClient(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ChookmsgtestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(ChookmsgtestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_CONNECT, &ChookmsgtestDlg::OnBnClickedConnect)
    ON_BN_CLICKED(IDC_START_SERVICE, &ChookmsgtestDlg::OnBnClickedStartService)
END_MESSAGE_MAP()


// ChookmsgtestDlg 消息处理程序

BOOL ChookmsgtestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void ChookmsgtestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void ChookmsgtestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR ChookmsgtestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void ChookmsgtestDlg::OnDestroy()
{
    // unsetHook();

    CDialog::OnDestroy();
    // TODO: 在此处添加消息处理程序代码
}


void ChookmsgtestDlg::OnBnClickedConnect()
{
    // TODO: 在此添加控件通知处理程序代码
    if (m_connected) {
        stopCtrl1();
        m_ioClient->close();
        delete m_ioClient;
        m_ioClient = NULL;
        m_ctrlPanel->DestroyWindow();
        delete m_ctrlPanel;
        m_ctrlPanel = NULL;
        // update UI
        m_connected = false;
        GetDlgItem(IDC_CONNECT)->SetWindowTextW(_T("connect"));
        GetDlgItem(IDC_START_SERVICE)->EnableWindow(1);
    }
    else {
        // check parameter
        CString ip, port;
        GetDlgItem(IDC_SERVER_IP)->GetWindowTextW(ip);
        GetDlgItem(IDC_SERVER_PORT)->GetWindowTextW(port);
        CT2A ip1(ip), port1(port);
        uint16_t portNum = atoi(LPSTR(port1));

        RECT display;
        display.left = 0;
        display.top = 0;
        display.right = 1080;
        display.bottom = 720;
        setCtrlPanelRect(display);

        m_ctrlPanel = new CCtrlPanel();
        m_ctrlPanel->Create(IDD_CTRL_PANEL);
        m_ctrlPanel->ShowWindow(SW_SHOWNORMAL);
        m_ctrlPanel->MoveWindow(0, 0, 1080, 720);

        UdpConnectInfo info;
        info.remote_ip = LPSTR(ip1);
        info.remote_port = portNum;
        info.send = true;
        info.recv = false;
        m_ioClient = new UdpClient();
        int ret = m_ioClient->connect(&info);
        if (ret < 0) {
            //TODO: pop up error
        }
        // startCtrl(m_ctrlPanel->GetSafeHwnd(), UDP_CLIENT, &info);
        startCtrl1(m_ctrlPanel->GetSafeHwnd(), m_ioClient);
        // update UI
        m_connected = true;
        GetDlgItem(IDC_CONNECT)->SetWindowTextW(_T("disconnect"));
        GetDlgItem(IDC_START_SERVICE)->EnableWindow(0);
    }
}


void ChookmsgtestDlg::OnBnClickedStartService()
{
    // TODO: 在此添加控件通知处理程序代码
    if (m_startService) {
        m_ioClient->close();
        delete m_ioClient;
        m_ioClient = NULL;
        stopCtrled1();
        // update UI
        m_startService = false;
        GetDlgItem(IDC_START_SERVICE)->SetWindowTextW(_T("start service"));
        GetDlgItem(IDC_CONNECT)->EnableWindow(1);
    }
    else {
        CString cstrPort;
        GetDlgItem(IDC_LISTEN_PORT)->GetWindowTextW(cstrPort);
        CT2A astrPort(cstrPort);
        uint16_t port = atoi(LPSTR(astrPort));

        UdpConnectInfo info;
        info.local_ip = "0.0.0.0";
        info.local_port = port;
        info.recv = true;
        info.send = false;
        m_ioClient = new UdpClient();
        int ret = m_ioClient->connect(&info);
        if (ret < 0) {
            //TODO: pop up error
        }
        acceptCtrl1(m_ioClient);
        // update UI
        m_startService = true;
        GetDlgItem(IDC_START_SERVICE)->SetWindowTextW(_T("stop service"));
        GetDlgItem(IDC_CONNECT)->EnableWindow(0);
    }
}
