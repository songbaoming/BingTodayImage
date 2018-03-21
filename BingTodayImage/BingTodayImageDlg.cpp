
// BingTodayImageDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BingTodayImageApp.h"
#include "BingTodayImageDlg.h"
#include "afxdialogex.h"
#include "BingTodayImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



void WINAPI APCFunc(ULONG_PTR dwParam)
{

}

UINT GetBingTodayImage(void *pParam)
{
	auto pdlg = (CBingTodayImageDlg*)pParam;

	auto strTemp = theApp.m_strModuleDir + TEXT("\\1.jpg");
	for (;;) {
		CBingTodayImage bti;
		if (bti.GetTodayImage(strTemp)) {
			strTemp = bti.GetDesc();
			int nIndex = strTemp.Find(TEXT('('));
			if (nIndex > 0)
				strTemp = strTemp.Left(nIndex);

			pdlg->m_nid.uFlags |= NIF_INFO;
			pdlg->m_nid.dwInfoFlags = NIIF_INFO;
			pdlg->m_nid.uTimeout = 1000;
			_tcscpy_s(pdlg->m_nid.szInfoTitle, TEXT("��Ӧ������ͼ"));
			_tcscpy_s(pdlg->m_nid.szInfo, strTemp);
			Shell_NotifyIcon(NIM_MODIFY, &pdlg->m_nid);
			SYSTEMTIME stm;
			GetLocalTime(&stm);
			strTemp.Format(TEXT("%u%02u%02u"), stm.wYear, stm.wMonth, stm.wDay);
			theApp.WriteProfileString(TEXT("setting"), TEXT("BingTodayImage"), strTemp);
			break;
		}
		if (WAIT_IO_COMPLETION == SleepEx(3000, TRUE))
			break;
	}

	return 0;
}
// CBingTodayImageDlg �Ի���

UINT CBingTodayImageDlg::s_msgTaskBarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));

CBingTodayImageDlg::CBingTodayImageDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_BINGTODAYIMAGE_DIALOG, pParent)
	, m_pImageThread(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBingTodayImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBingTodayImageDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_NOTIFYICON, &CBingTodayImageDlg::OnNotifyicon)
	ON_REGISTERED_MESSAGE(s_msgTaskBarRestart, &CBingTodayImageDlg::OnTaskBarRestart)
	ON_COMMAND(ID_NOTIFY_AUTORUN, &CBingTodayImageDlg::OnNotifyAutorun)
	ON_COMMAND(ID_NOTIFY_EXIT, &CBingTodayImageDlg::OnNotifyExit)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CBingTodayImageDlg ��Ϣ��������

BOOL CBingTodayImageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵������ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	m_nid.cbSize = sizeof(m_nid);
	m_nid.uID = 20;
	m_nid.hWnd = GetSafeHwnd();
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_nid.hIcon = (HICON)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINFRAME));
	m_nid.uCallbackMessage = WM_NOTIFYICON;
	_tcscpy_s(m_nid.szTip, TEXT("BingTodayImage"));
	Shell_NotifyIcon(NIM_ADD, &m_nid);

	CheckAutoRun();

	if (theApp.m_bGetBingTodayImage) {
		m_pImageThread = AfxBeginThread(GetBingTodayImage, this, 0, 0, CREATE_SUSPENDED);
		if (m_pImageThread) {
			m_pImageThread->m_bAutoDelete = FALSE;
			m_pImageThread->ResumeThread();
		}
	}
	ShowWindow(SW_HIDE);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}


// �����Ի���������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CBingTodayImageDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CBingTodayImageDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CBingTodayImageDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CBingTodayImageDlg::CheckAutoRun()
{
	HKEY hKey;
	TCHAR szPath[MAX_PATH] = {};
	DWORD dwLen = MAX_PATH * sizeof(TCHAR);
	LPCTSTR lpszName = TEXT("QuickExec");
	auto nRes = RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
		0, KEY_ALL_ACCESS, &hKey);
	if (nRes != ERROR_SUCCESS) {
		ShowErrMsg(TRUE);
		return;
	}
	nRes = RegQueryValueEx(hKey, lpszName, NULL, NULL, (LPBYTE)szPath, &dwLen);
	if (theApp.m_bAotuRun && nRes != ERROR_SUCCESS) {
		GetModuleFileName(NULL, szPath, MAX_PATH);
		RegSetValueEx(hKey, lpszName, 0, REG_SZ, (LPBYTE)szPath, (_tcslen(szPath) + 1) * sizeof(TCHAR));
	}
	else if (!theApp.m_bAotuRun && nRes == ERROR_SUCCESS) {
		RegDeleteValue(hKey, lpszName);
	}
	RegCloseKey(hKey);
}

LRESULT CBingTodayImageDlg::OnTaskBarRestart(WPARAM wParam, LPARAM lParam)
{
	Shell_NotifyIcon(NIM_DELETE, &m_nid);
	Shell_NotifyIcon(NIM_ADD, &m_nid);
	return LRESULT();
}

LRESULT CBingTodayImageDlg::OnNotifyicon(WPARAM wParam, LPARAM lParam)
{
	switch (lParam) {
	case WM_RBUTTONUP:
	{
		CPoint pt;
		GetCursorPos(&pt);
		SetForegroundWindow(); //

		CMenu menu;
		menu.LoadMenu(IDR_NOTIFY);
		auto pSubMenu = menu.GetSubMenu(0);
		pSubMenu->CheckMenuItem(ID_NOTIFY_AUTORUN, theApp.m_bAotuRun ? MF_CHECKED : MF_UNCHECKED);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);
		menu.DestroyMenu();
	}
	break;
	default:
		break;
	}
	return 0;
	return LRESULT();
}


void CBingTodayImageDlg::OnNotifyAutorun()
{
	theApp.m_bAotuRun = !theApp.m_bAotuRun;
	theApp.WriteProfileInt(TEXT("setting"), TEXT("AutoRun"), theApp.m_bAotuRun);
	CheckAutoRun();
}


void CBingTodayImageDlg::OnNotifyExit()
{
	SendMessage(WM_CLOSE);
}


void CBingTodayImageDlg::OnOK()
{
}


void CBingTodayImageDlg::OnCancel()
{
	DestroyWindow();
}


void CBingTodayImageDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	if (m_pImageThread) {
		QueueUserAPC(APCFunc, m_pImageThread->m_hThread, 0);
		WaitForSingleObject(m_pImageThread->m_hThread, INFINITE);
		delete m_pImageThread;
	}
	Shell_NotifyIcon(NIM_DELETE, &m_nid);
}