
// hookmsg_testDlg.h: 头文件
//

#pragma once
#include "CCtrlPanel.h"
#include "ctrl_channel.h"

// ChookmsgtestDlg 对话框
class ChookmsgtestDlg : public CDialog
{
// 构造
public:
	ChookmsgtestDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HOOKMSG_TEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
    bool m_connected;
    bool m_startService;
    CCtrlPanel *m_ctrlPanel;
    IIOClient *m_ioClient;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedConnect();
    afx_msg void OnBnClickedStartService();
};
