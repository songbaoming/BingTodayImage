
// BingTodayImage.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CBingTodayImageApp: 
// �йش����ʵ�֣������ BingTodayImage.cpp
//

class CBingTodayImageApp : public CWinApp
{
public:
	CBingTodayImageApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
public:
	bool m_bGetBingTodayImage;
	bool m_bAotuRun;
	HANDLE m_hMutex;
	CString m_strModuleDir;
	virtual int ExitInstance();
};

extern CBingTodayImageApp theApp;