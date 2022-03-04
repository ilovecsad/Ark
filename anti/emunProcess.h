#pragma once
#include <ntifs.h>
#include "exapi.h"
#include <intrin.h>

#define  IA32_GS_BASE  0xC0000101
#define  IdleThread_offset 0x18

#define  MAX_PATHEX 300

typedef struct _PROCESS_INFO_
{
	HANDLE ulPid;
	HANDLE ulParentPid;
	PVOID ulEprocess;
	ULONGLONG ulCreateTime;
	WCHAR szPath[MAX_PATHEX];
}PROCESS_INFO, *PPROCESS_INFO;


typedef struct _THREAD_INFO_
{
	ULONG Tid;
	ULONG Priority;
	ULONG ContextSwitches;
	ULONG State;
	ULONG_PTR Teb;
	ULONG_PTR Win32StartAddress;
	ULONG_PTR pThread;
	ULONG_PTR ThreadFlag;
}THREAD_INFO, *PTHREAD_INFO;


typedef struct _MODULE_INFO_EX
{
	ULONG Size;
	ULONG Sign;    
	ULONG_PTR Base;
	WCHAR Path[260];
}MODULE_INFO_EX, *PMODULE_INFO_EX;

typedef struct _ALL_MODULES_
{
	ULONG nCnt;
	MODULE_INFO_EX Modules[1];
}ALL_MODULES, *PALL_MODULES;

ULONG EnumProcess(PPROCESS_INFO pBuffer);

PEPROCESS GetIdleProcess();

BOOLEAN AddIdleProcess(PPROCESS_INFO p);

NTSTATUS GetCommandLineByPid(HANDLE dwPid, PVOID pBuffer, ULONG_PTR nSize);

ULONG EnumProcessThreadByPid(HANDLE dwPid, PTHREAD_INFO pBuffer);

NTSTATUS EnumProcessModuleByPid(HANDLE dwPid, PALL_MODULES pBuffer, ULONG nCnt);
BOOLEAN IsRealProcess(PEPROCESS pEprocess);
BOOLEAN IsProcessDie(PEPROCESS pEprocess);
VOID WalkerModuleList(PLIST_ENTRY pList, ULONG nType, PALL_MODULES pPmi, ULONG nCnt);
BOOLEAN IsModuleInList(ULONG_PTR Base, ULONG Size, PALL_MODULES pPmi, ULONG nCnt);
void WalkerModuleList32(PLIST_ENTRY32 pList, ULONG nType, PALL_MODULES pPmi, ULONG nCnt);
UCHAR PspGetThreadSuspendCount(PETHREAD pThread);
NTSTATUS HideModule(ULONG dwPid, ULONG_PTR dwModuleBase, ULONG_PTR dwLdrpHashTable,ULONG nSize);

VOID WalkerModuleListToRemoveDllModule64(PLIST_ENTRY pList, ULONG nType, ULONG_PTR Base);
NTSTATUS RemoveDllModuleInPeb64(PEPROCESS pEprocess, ULONG_PTR Base);

ULONG32 GetLdrpHashTable32(ULONG32 pKernel32_BaseThreadInitThunk, ULONG32 hModule);
ULONG_PTR GetProcAddress(PVOID BaseAddress, char* lpFunctionName);

ULONG32 GetLdrpHashTable32Ex(ULONG_PTR arg);