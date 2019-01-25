
// BingTodayImageDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BingTodayImageApp.h"
#include "BingTodayImageDlg.h"
#include "afxdialogex.h"
#include "BingTodayImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_GETBINGIMAGE				(WM_USER + 1024)


void IncDay(SYSTEMTIME &stm)
{
	if (stm.wMonth == 2) {
		if (stm.wDay == 29 || (stm.wDay == 28 && (stm.wDay % 400 && (stm.wYear % 4 || !(stm.wYear % 100))))) {
			stm.wDay = 1;
			stm.wMonth += 1;
		}
		else {
			stm.wDay += 1;
		}
	}
	else if (stm.wDay == 30 && (stm.wMonth == 4 || stm.wMonth == 6 || stm.wMonth == 9 || stm.wMonth == 11)) {
		stm.wDay = 1;
		stm.wMonth += 1;
	}
	else if (stm.wDay == 31 && (stm.wMonth == 1 || stm.wMonth == 3 || stm.wMonth == 5 || stm.wMonth == 7 ||
		stm.wMonth == 8 || stm.wMonth == 10 || stm.wMonth == 12)) {
		stm.wDay = 1;
		if (stm.wMonth == 12) {
			stm.wMonth = 1;
			stm.wYear += 1;
		}
		else
			stm.wMonth += 1;

	}
	else {
		stm.wDay += 1;
	}
}

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

			pdlg->SendMessage(WM_GETBINGIMAGE, (WPARAM)(LPCTSTR)strTemp);
			
			break;
		}
		if (WAIT_IO_COMPLETION == SleepEx(3000, TRUE))
			break;
	}

	return 0;
}
// CBingTodayImageDlg 对话框

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
	ON_MESSAGE(WM_GETBINGIMAGE, &CBingTodayImageDlg::OnGetBingImage)
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CBingTodayImageDlg 消息处理程序

BOOL CBingTodayImageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_nid.cbSize = sizeof(m_nid);
	m_nid.uID = 20;
	m_nid.hWnd = GetSafeHwnd();
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_nid.hIcon = (HICON)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINFRAME));
	m_nid.uCallbackMessage = WM_NOTIFYICON;
	_tcscpy_s(m_nid.szTip, TEXT("BingTodayImage"));
	Shell_NotifyIcon(NIM_ADD, &m_nid);

	CheckAutoRun();

	m_strLastChangeData = theApp.GetProfileString(TEXT("setting"), TEXT("BingTodayImage"));
	SetTimer(0, 1000, nullptr);
	
	ShowWindow(SW_HIDE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CBingTodayImageDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
	LPCTSTR lpszName = TEXT("BingTodayImage");
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
	m_nid.uFlags &= ~NIF_INFO;
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


LRESULT CBingTodayImageDlg::OnGetBingImage(WPARAM wParam, LPARAM lParam)
{
	ASSERT(wParam);

	m_strImageDesc = (LPCTSTR)wParam;
	m_nid.uFlags |= NIF_INFO;
	m_nid.dwInfoFlags = NIIF_INFO;
	m_nid.uTimeout = 1000;
	_tcscpy_s(m_nid.szInfoTitle, TEXT("必应今日美图"));
	_tcscpy_s(m_nid.szInfo, m_strImageDesc);
	Shell_NotifyIcon(NIM_MODIFY, &m_nid);

	SYSTEMTIME stm;
	GetLocalTime(&stm);
#ifndef DEBUG
	m_strLastChangeData.Format(TEXT("%u%02u%02u"), stm.wYear, stm.wMonth, stm.wDay);
	theApp.WriteProfileString(TEXT("setting"), TEXT("BingTodayImage"), m_strLastChangeData);
#endif // !DEBUG

	SetChangeImageTimer(stm);
	return 0;
}

void CBingTodayImageDlg::SetChangeImageTimer(SYSTEMTIME &stmNow)
{
	DWORD dwElapse;
	LARGE_INTEGER liNow, liNext;

	SystemTimeToFileTime(&stmNow, (PFILETIME)&liNow);
	stmNow.wHour = 8;
	stmNow.wMinute = stmNow.wSecond = stmNow.wMilliseconds = 0;
	IncDay(stmNow);
	SystemTimeToFileTime(&stmNow, (PFILETIME)&liNext);
	dwElapse = (liNext.QuadPart - liNow.QuadPart) / 10000;
	SetTimer(0, dwElapse, nullptr);
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


void CBingTodayImageDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 0) {
		KillTimer(nIDEvent);
		SYSTEMTIME stm;
		TCHAR szText[12];
		GetLocalTime(&stm);
		_stprintf_s(szText, TEXT("%u%02u%02u"), stm.wYear, stm.wMonth, stm.wDay);
		if (m_strLastChangeData != szText) {
			if (m_pImageThread)
				delete m_pImageThread;
			m_pImageThread = AfxBeginThread(GetBingTodayImage, this, 0, 0, CREATE_SUSPENDED);
			if (m_pImageThread) {
				m_pImageThread->m_bAutoDelete = FALSE;
				m_pImageThread->ResumeThread();
			}
		}
		else
			SetChangeImageTimer(stm);
	}
	CDialogEx::OnTimer(nIDEvent);
}
