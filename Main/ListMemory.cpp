#include "ListMemory.h"

extern CLoadDriver g_connectDriver;
CListMemory::CListMemory()
{
}

CListMemory::~CListMemory()
{
}

BOOL CListMemory::GetMemorys(HANDLE nPid, ULONG_PTR pEprocess, vector<MEMORY_INFO>& vectorMemorys)
{

	BOOL bRet = FALSE;

	typedef struct _INPUT
	{
		HANDLE nPid;
		ULONG_PTR pEprocess;
		ULONG_PTR nSize;
		PALL_MEMORYS pBuffer;
	}Input;

	Input inputs = { 0 };

	if (nPid == 0)
	{
		return bRet;
	}
	vectorMemorys.clear();

	inputs.nPid = nPid;

	inputs.nSize = sizeof(MEMORY_INFO) * 0x5000 + sizeof(ULONG); 

	inputs.pBuffer = (PALL_MEMORYS)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, inputs.nSize);
	inputs.pEprocess = pEprocess;

	if (inputs.pBuffer)
	{

		bRet = g_connectDriver.DeviceControl(IOCTL_ENUM_MEMORY, &inputs, sizeof(Input), 0, 0, 0);


		if (bRet && inputs.pBuffer->nCnt > 0)
		{
			for (ULONG i = 0; i < inputs.pBuffer->nCnt; i++)
			{
		
				vectorMemorys.push_back(inputs.pBuffer->Memorys[i]);
			}
		}

		bRet = HeapFree(GetProcessHeap(), 0, inputs.pBuffer);
	}

	return bRet;

}

ULONG CListMemory::ChangeMemoryProtection(HANDLE nPid, ULONG_PTR dwBaseAddess,ULONG nNewProtection ,ULONG nSize)
{

	ULONG nStatus = -1;

	if (!nPid || !dwBaseAddess || !nSize)return nStatus;
	typedef struct _INPUT
	{
		HANDLE dwPid;
		ULONG_PTR dwBaseAddess;
		ULONG Protection;
		ULONG nSize;
	}Input;

	Input inputs = { 0 };

	inputs.dwBaseAddess = dwBaseAddess;
	inputs.dwPid = nPid;
	inputs.nSize = nSize;
	inputs.Protection = nNewProtection;
	BOOL bRet = FALSE;

	bRet = g_connectDriver.DeviceControl(IOCTL_CHANGGE_MEMOORY_PROTECTION, &inputs, sizeof(Input), &nStatus, sizeof(ULONG), 0);

	return nStatus;
}

ULONG_PTR CListMemory::AllocateVirtualMemory(ULONG dwPid, ULONG_PTR  BaseAddress, SIZE_T RegionSize, ULONG AllocationType)
{
	ULONG_PTR nStatus =0;

	if (!dwPid || !RegionSize || !AllocationType)return nStatus;
	typedef struct _INPUT
	{
		HANDLE dwPid;
		ULONG_PTR dwBaseAddess;
		ULONG AllocationType;
		ULONG nSize;
	}Input;

	Input inputs = { 0 };

	
	inputs.dwPid = (HANDLE)dwPid;
	inputs.nSize = RegionSize;
	inputs.AllocationType = AllocationType;
	inputs.dwBaseAddess = BaseAddress;
	BOOL bRet = FALSE;

	bRet = g_connectDriver.DeviceControl(IOCTL_ALLOCATE_MEMORY, &inputs, sizeof(Input), &nStatus, sizeof(ULONG_PTR), 0);
	

	return nStatus;
}

ULONG CListMemory::FreeVirtualMemory(ULONG dwPid, ULONG_PTR BaseAddress, SIZE_T RegionSize)
{
	ULONG nStatus = 0xC0000001;

	if (!dwPid || !RegionSize )return nStatus;
	typedef struct _INPUT
	{
		HANDLE dwPid;
		ULONG_PTR dwBaseAddess;
		ULONG nSize;
	}Input;

	Input inputs = { 0 };


	inputs.dwPid = (HANDLE)dwPid;
	inputs.nSize = RegionSize;
	
	inputs.dwBaseAddess = BaseAddress;
	BOOL bRet = FALSE;

	bRet = g_connectDriver.DeviceControl(IOCTL_FREE_MEMORY, &inputs, sizeof(Input), &nStatus, sizeof(ULONG), 0);
	


	return nStatus;
}

BOOL CListMemory::RtlSuperCopyMemory(ULONG dwPid, ULONG_PTR BaseAddress, PVOID pBuffer, SIZE_T RegionSize)
{
	BOOL bRet = FALSE;

	if (!dwPid || !RegionSize ||!pBuffer || !BaseAddress)return bRet;
	typedef struct _INPUT
	{
		HANDLE dwPid;
		ULONG_PTR dwBaseAddess;
		PVOID pBuffer;
		ULONG nSize;
	}Input;

	Input inputs = { 0 };


	inputs.dwPid = (HANDLE)dwPid;
	inputs.nSize = RegionSize;
	inputs.dwBaseAddess = BaseAddress;
	inputs.pBuffer = pBuffer;

	bRet = g_connectDriver.DeviceControl(IOCTL_MDL_WRITE_MEMORY, &inputs, sizeof(Input),NULL, 0, 0);

	return bRet;
}


