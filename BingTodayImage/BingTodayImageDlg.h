
// BingTodayImageDlg.h : 头文件
//

#pragma once


// CBingTodayImageDlg 对话框
class CBingTodayImageDlg : public CDialogEx
{
// 构造
public:
	CBingTodayImageDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BINGTODAYIMAGE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CWinThread *m_pImageThread;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	static UINT s_msgTaskBarRestart;
	NOTIFYICONDATA m_nid;
	CString m_strLastChangeData;
	CString m_strImageDesc;

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void CheckAutoRun();
	afx_msg LRESULT OnTaskBarRestart(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNotifyicon(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetBingImage(WPARAM wParam, LPARAM lParam);
	void SetChangeImageTimer(SYSTEMTIME &stmNow);
	afx_msg void OnNotifyAutorun();
	afx_msg void OnNotifyExit();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
