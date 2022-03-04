
// Main.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// 主符号

#include "SortListCtrl.h"
// CMainApp:
// 有关此类的实现，请参阅 Main.cpp
//

class CMainApp : public CWinApp
{
public:
	CMainApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
//	afx_msg void OnHelpRefresh();
};

extern CMainApp theApp;


#define WM_ADD_ITEM_HANDLE_DLG       WM_CUSTOMER_MY+4
#define WM_ADD_ITEM_MEMORY_DLG       WM_CUSTOMER_MY+5
#define WM_ADD_ITEM_MODULE_DLG       WM_CUSTOMER_MY+6
#define WM_ADD_ITEM_PROCESSHOOK_DLG       WM_CUSTOMER_MY+7


