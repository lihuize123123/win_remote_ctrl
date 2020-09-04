#pragma once


// CCtrlPanel 对话框

class CCtrlPanel : public CDialog
{
	DECLARE_DYNAMIC(CCtrlPanel)

public:
	CCtrlPanel(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCtrlPanel();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CTRL_PANEL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnMove(int x, int y);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);

private:
    void UpdateWindowPos();
};
