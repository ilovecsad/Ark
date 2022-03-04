#pragma once
#include <ntifs.h>

typedef struct _MEMORY_INFO_
{
	ULONG_PTR Base;
	ULONG_PTR Size;
	ULONG_PTR Protect;
	ULONG_PTR VadFlags;
	ULONG_PTR State;
	ULONG_PTR Type;
	ULONG_PTR pFileObject;
}MEMORY_INFO, *PMEMORY_INFO;

typedef struct _ALL_MEMORYS_
{
	ULONG nCnt;
	MEMORY_INFO Memorys[1];
}ALL_MEMORYS, *PALL_MEMORYS;

NTSTATUS GetMemorys(HANDLE dwPid, PALL_MEMORYS pMemorys, ULONG nCnt);
NTSTATUS SafeCopyProcessModules(PEPROCESS pEprocess, ULONG_PTR nBase, ULONG nSize, PVOID pOutBuffer);
NTSTATUS RemoveDllModuleInPeb64(PEPROCESS pEprocess, ULONG_PTR Base);
NTSTATUS RemoveDllModuleInPeb86(PEPROCESS pEprocess, ULONG Base);
NTSTATUS RemoveDllModuleInLdrpHashTable64(LIST_ENTRY * LdrpHashTable,PEPROCESS pEprocess, ULONG_PTR Base);
NTSTATUS RemoveDllModuleInLdrpHashTable86(LIST_ENTRY32* LdrpHashTable,PEPROCESS pEprocess, ULONG Base);
