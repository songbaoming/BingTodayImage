
// BingTodayImageDlg.h : ͷ�ļ�
//

#pragma once


// CBingTodayImageDlg �Ի���
class CBingTodayImageDlg : public CDialogEx
{
// ����
public:
	CBingTodayImageDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BINGTODAYIMAGE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	CWinThread *m_pImageThread;

	// ���ɵ���Ϣӳ�亯��
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
