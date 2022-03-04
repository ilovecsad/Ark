#pragma once
#include <ntifs.h>

typedef struct _OB_CALLBACK
{
	LIST_ENTRY        ListEntry;
	ULONG64                Unknown;
	ULONG64                ObHandle;
	ULONG64                ObjTypeAddr;
	ULONG64                PreCall;
	ULONG64                PostCall;
	ULONG64   bType;
} OB_CALLBACK, *POB_CALLBACK;

typedef struct _CALL_BACK_
{
	ULONG64 PspCreateProcessNotifyRoutine[65];
	ULONG64 PspCreateThreadNotifyRoutine[65];
	ULONG64 CmCallbackListHead[65];
	ULONG64 PspLoadImageNotifyRoutine[65];
	OB_CALLBACK obRegister[65];
}CALL_BACK, *PCALL_BACK;

typedef struct _CM_NOTIFY_ENTRY
{
	LIST_ENTRY ListEntryHead;
	ULONG UnKnown1;
	ULONG UnKnown2;
	LARGE_INTEGER Cookie;
	ULONG64 Context;
	ULONG64 Function;
}CM_NOTIFY_ENTRY, *PCM_NOTIFY_ENTRY;



ULONG64 SearchPspLoadImageNotifyRoutine();
ULONG64 SearchPspCreateThreadNotifyRoutine();
ULONG64 SearchCallbackListHead();
BOOLEAN EnumObCallbacks(POB_CALLBACK p,ULONG nSize);
ULONG64 FindPspCreateProcessNotifyRoutine();

void EnumCreateProcessNotify(PVOID* pBuffer, ULONG nSize);
void EnumCreateThreadNotify(PVOID* pBuffer, ULONG nSize);
void EnumLoadImageNotify(PVOID* pBuffer, ULONG nSize);
ULONG CountCmpCallbackAfterXP(PVOID* pBuffer,ULONG nSize);
