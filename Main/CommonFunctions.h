#pragma once
#include <strsafe.h>
#include <afxcmn.h>
#include "Common.h"
#include "SortListCtrl.h"



class CommonFunctions
{
public:
	CommonFunctions();
	virtual ~CommonFunctions();
	
	CString GetFileCompanyName(CString szPath);
	CString GetFileDescription(CString szPath);
	BOOL IsMicrosoftAppByPath(CString szPath);
	BOOL IsMicrosoftApp(CString szCompany);
	CString TrimPath(WCHAR * szPath);
	CString GetLongPath(CString szPath);
	CString RegParsePath(WCHAR *szData, ULONG dwType);
	CString RegParsePath(CString szPath);
	BOOL InitUnicodeString(UNICODE_STRING *unString, WCHAR *szString);
	void FreeUnicodeString(UNICODE_STRING *unString);
	int GetSelectItem(CSortListCtrl *pList);
	void LocationExplorer(CString szPath);
	BOOL SetStringToClipboard(CString szImageName);
	void ExportListToTxt(CSortListCtrl* pList, CString szStatus);
	BOOL GetDefaultTxtFileName(CString& sExcelFile);
	CString CommonFunctions::ms2ws(LPCSTR szSrc, int cbMultiChar = -1);
};

