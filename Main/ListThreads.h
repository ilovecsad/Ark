#pragma once
#include "CLoadDriver.h"
#include <vector>
#include "Common.h"

using namespace std;

typedef _Return_type_success_(return >= 0) LONG NTSTATUS;


typedef struct _IO_STATUS_BLOCK {
	union {
		NTSTATUS Status;
		PVOID Pointer;
	} DUMMYUNIONNAME;

	ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;



typedef struct _IO_STATUS_BLOCK32 {
	NTSTATUS Status;
	ULONG Information;
} IO_STATUS_BLOCK32, *PIO_STATUS_BLOCK32;
typedef
VOID
(NTAPI *PIO_APC_ROUTINE) (
	_In_ PVOID ApcContext,
	_In_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_ ULONG Reserved
	);

typedef NTSTATUS(NTAPI*_NtResumeThread)(
	IN HANDLE ThreadHandle,
	OUT PULONG PreviousSuspendCount OPTIONAL
	);

typedef NTSTATUS(NTAPI *_NtTerminateThread)(HANDLE hThread, PULONG uExitCode);

typedef NTSTATUS(NTAPI *pNtSuspendThread)(HANDLE ThreadHandle, PULONG SuspendCount);
typedef NTSTATUS(NTAPI *pNtAlertResumeThread)(HANDLE ThreadHandle, PULONG SuspendCount);
typedef NTSTATUS(NTAPI *pNtAllocateVirtualMemory)(HANDLE ProcessHandle, PVOID *BaseAddress, ULONG_PTR ZeroBits, PSIZE_T RegionSize, ULONG AllocationType, ULONG Protect);
typedef NTSTATUS(NTAPI *pNtWriteVirtualMemory)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToWrite, PULONG NumberOfBytesWritten);
typedef NTSTATUS(NTAPI *pNtQueueApcThread)(HANDLE ThreadHandle, PIO_APC_ROUTINE ApcRoutine, PVOID ApcRoutineContext OPTIONAL, PIO_STATUS_BLOCK ApcStatusBlock OPTIONAL, ULONG ApcReserved OPTIONAL);



class CListThreads
{
public:
	CListThreads();
	~CListThreads();
	BOOL ListThreads(ULONG nPid, ULONG_PTR pEprocess, vector<THREAD_INFO>& vectorThreads);
	NTSTATUS KillThread(HANDLE hThreadHanle);
	NTSTATUS ResumeThread(HANDLE hThreadHanle);
	NTSTATUS SuspendThread(HANDLE hThreadHanle);
	HANDLE KernelOpenThread(DWORD dwThreadId);
	NTSTATUS AlertResumeThread(HANDLE ThreadHandle);
	NTSTATUS QueueApcThread(HANDLE ThreadHandle, PVOID ApcRoutine, PVOID ApcRoutineContext);
	ULONG hzw_CreateRemoteThread(HANDLE dwPid, PVOID64 paddresstoexecute, PVOID64 lparam);
};