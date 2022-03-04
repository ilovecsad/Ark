#pragma once
#include <vector>
#include <algorithm>
#include <afxcmn.h>
#include "Common.h"
#include "CLoadDriver.h"
#include "CommonFunctions.h"
using namespace std;

class CListVadInfo
{
public:
	CListVadInfo();
	~CListVadInfo();

	BOOL EnumProcessVad(ULONG nPid, vector<VAD_INFO>& vectorVads);
	CString ObQueryNameString(ULONG_PTR a);
	CommonFunctions m_commonFun;
	BOOL changeVadProtect(ULONG dwPid, ULONG_PTR dwBaseAddress, ULONG_PTR dwVadProtection);
};