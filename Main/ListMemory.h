#pragma once
#include <vector>
#include <algorithm>
#include <afxcmn.h>
#include "Common.h"
#include "CLoadDriver.h"
using namespace std;



class CListMemory
{
public:
	CListMemory();
	~CListMemory();

	BOOL GetMemorys(HANDLE nPid, ULONG_PTR pEprocess, vector<MEMORY_INFO> &vectorMemorys);
	ULONG ChangeMemoryProtection(HANDLE nPid, ULONG_PTR dwBaseAddess, ULONG nNewProtection, ULONG nSize);
	ULONG_PTR AllocateVirtualMemory(ULONG dwPid, ULONG_PTR BaseAddress,SIZE_T RegionSize, ULONG AllocationType);
	ULONG FreeVirtualMemory(ULONG dwPid,ULONG_PTR BaseAddress, SIZE_T RegionSize);
	BOOL RtlSuperCopyMemory(ULONG dwPid, ULONG_PTR BaseAddress, PVOID pBuffer, SIZE_T RegionSize);
};

