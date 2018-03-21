
// BingTodayImage.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "BingTodayImageApp.h"
#include "BingTodayImageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBingTodayImageApp

BEGIN_MESSAGE_MAP(CBingTodayImageApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CBingTodayImageApp 构造

CBingTodayImageApp::CBingTodayImageApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CBingTodayImageApp 对象

CBingTodayImageApp theApp;


// CBingTodayImageApp 初始化

BOOL CBingTodayImageApp::InitInstance()
{
	m_hMutex = CreateMutex(NULL, FALSE, TEXT("{D5426629-9583-4231-B439-394DA68B2F28}"));
	if (ERROR_ALREADY_EXISTS == GetLastError())
		return FALSE;
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	//初始化Winsock库
	if (!AfxSocketInit()) {
		AfxMessageBox(TEXT("Winsock库初始化失败！"));
		return FALSE;
	}

	AfxEnableControlContainer();

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("Soon"));

	TCHAR szText[MAX_PATH];
	GetModuleFileName(NULL, szText, MAX_PATH);
	auto p = _tcsrchr(szText, TEXT('\\'));
	if (p)
		*p = 0;
	m_strModuleDir = szText;

	SYSTEMTIME stm;
	GetLocalTime(&stm);
	_stprintf_s(szText, TEXT("%u%02u%02u"), stm.wYear, stm.wMonth, stm.wDay);
	auto strDay = GetProfileString(TEXT("setting"), TEXT("BingTodayImage"));
	m_bGetBingTodayImage = strDay != szText;
	m_bAotuRun = GetProfileInt(TEXT("setting"), TEXT("AutoRun"), 0);

	auto pDlg = new CBingTodayImageDlg;
	m_pMainWnd = pDlg;
	pDlg->Create(IDD_BINGTODAYIMAGE_DIALOG);

	return TRUE;
}



int CBingTodayImageApp::ExitInstance()
{
	if (m_hMutex)
		CloseHandle(m_hMutex);
	return CWinApp::ExitInstance();
}

// 显示错误信息
// 参数：
//	BOOL bUseMsgBox：是否以MessageBox的方式显示错误，false时以调试信息方式显示
//	LONG *lpErrCode：存放错误代码的变量指针，为空时函数使用GetLastError()获取
void ShowErrMsg(BOOL bUseMsgBox, CString strPreMsg/* = TEXT("")*/, LONG *lpErrCode/* = NULL*/)
{
	LONG lErrCode;
	LPVOID lpMsgBuf;
	if (lpErrCode)
		lErrCode = *lpErrCode;
	else
		lErrCode = GetLastError();
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		lErrCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	strPreMsg += (LPTSTR)lpMsgBuf;
	if (bUseMsgBox)
		AfxMessageBox(strPreMsg);
	else
		TRACE(strPreMsg);
	LocalFree(lpMsgBuf);
}
