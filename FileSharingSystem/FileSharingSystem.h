
// FileSharingSystem.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFileSharingSystemApp:
// �йش����ʵ�֣������ FileSharingSystem.cpp
//

class CFileSharingSystemApp : public CWinApp
{
public:
	CFileSharingSystemApp();
	~CFileSharingSystemApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFileSharingSystemApp theApp;