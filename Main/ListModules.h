#pragma once
#include <vector>
#include <algorithm>
#include <afxcmn.h>
#include "Common.h"
#include "CLoadDriver.h"
using namespace std;


class CListModules
{
public:
	CListModules();
	~CListModules();

	BOOL EnumModulesByPeb(ULONG nPid, ULONG_PTR pEprocess, vector<MODULE_INFO_EX> &vectorModules);
	BOOL EnumModulesBySnapshot(ULONG nPid, vector<MODULE_INFO_EX> &vectorModules);
	BOOL FreeLoadLibary(ULONG nPid,ULONG_PTR dwModuleBase, ULONG_PTR dwLdrpHashTable);
	PVOID DumpModuleMemory(ULONG nPid, ULONG_PTR dwModuleBase,ULONG nSize);
	BOOL DumpModuleMemoryToFile(ULONG nPid, ULONG_PTR nBase, ULONG nSize, CString szModule);
	BOOL HideModule(ULONG nPid, ULONG_PTR dwModuleBase,ULONG_PTR dwLdrpHashTable,ULONG nSize);
};

