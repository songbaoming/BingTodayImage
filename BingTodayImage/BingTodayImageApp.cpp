
// BingTodayImage.cpp : ����Ӧ�ó��������Ϊ��
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


// CBingTodayImageApp ����

CBingTodayImageApp::CBingTodayImageApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CBingTodayImageApp ����

CBingTodayImageApp theApp;


// CBingTodayImageApp ��ʼ��

BOOL CBingTodayImageApp::InitInstance()
{
	m_hMutex = CreateMutex(NULL, FALSE, TEXT("{D5426629-9583-4231-B439-394DA68B2F28}"));
	if (ERROR_ALREADY_EXISTS == GetLastError())
		return FALSE;
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	//��ʼ��Winsock��
	if (!AfxSocketInit()) {
		AfxMessageBox(TEXT("Winsock���ʼ��ʧ�ܣ�"));
		return FALSE;
	}

	AfxEnableControlContainer();

	// ���Windows Native���Ӿ����������Ա��� MFC �ؼ�����������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
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

// ��ʾ������Ϣ
// ������
//	BOOL bUseMsgBox���Ƿ���MessageBox�ķ�ʽ��ʾ����falseʱ�Ե�����Ϣ��ʽ��ʾ
//	LONG *lpErrCode����Ŵ������ı���ָ�룬Ϊ��ʱ����ʹ��GetLastError()��ȡ
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
