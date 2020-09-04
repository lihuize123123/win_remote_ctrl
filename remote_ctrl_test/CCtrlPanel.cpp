// CCtrlPanel.cpp: 实现文件
//

#include "pch.h"
#include "hookmsg_test.h"
#include "CCtrlPanel.h"
#include "afxdialogex.h"
#include "remote_ctrl.h"

// CCtrlPanel 对话框

IMPLEMENT_DYNAMIC(CCtrlPanel, CDialog)

CCtrlPanel::CCtrlPanel(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CTRL_PANEL, pParent)
{

}

CCtrlPanel::~CCtrlPanel()
{
}

void CCtrlPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCtrlPanel, CDialog)
    ON_WM_MOVE()
    ON_WM_SIZE()
    /*ON_WM_WINDOWPOSCHANGED()*/
END_MESSAGE_MAP()


// CCtrlPanel 消息处理程序


void CCtrlPanel::OnMove(int x, int y)
{
    CDialog::OnMove(x, y);

    // TODO: 在此处添加消息处理程序代码
    UpdateWindowPos();
}


void CCtrlPanel::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    UpdateWindowPos();
}

#if 0
void CCtrlPanel::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
    CDialog::OnWindowPosChanged(lpwndpos);

    // TODO: 在此处添加消息处理程序代码
    UpdateWindowPos();
}
#endif

void CCtrlPanel::UpdateWindowPos()
{
    RECT rect;
    GetWindowRect(&rect);
    setCtrlPanelRect(rect);
}
