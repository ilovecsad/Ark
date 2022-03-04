#pragma once
#include <vector>
#include <algorithm>
#include <afxcmn.h>
#include "Common.h"
#include "CLoadDriver.h"
using namespace std;


class CListHandles
{
public:
	CListHandles();
	~CListHandles();
	BOOL GetHandles(ULONG nPid,  vector<HANDLE_INFO> &vectorHandles);
	BOOL CloseHandle(ULONG nPid, BOOL bForce, ULONG hHandle, ULONG pObject);
};
