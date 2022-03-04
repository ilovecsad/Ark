#pragma once
#include <ntifs.h>
#include <windef.h>

int GetExportSsdtIndex(const char* ExportName);
NTSTATUS Initialize();
void* RtlAllocateMemory(BOOLEAN InZeroMemory, SIZE_T InSize);
void RtlFreeMemory(void* InPointer);
void Deinitialize();
PVOID GetKernelZwFuncByName(const char* ExportName);
PVOID GetSSDTFucnByNTDLL_Name(const char* apiname);


typedef DWORD(NTAPI  *PTHREAD_START_ROUTINE)(
	PVOID lpThreadParameter
	);
typedef struct _PROC_THREAD_ATTRIBUTE_LIST *PPROC_THREAD_ATTRIBUTE_LIST, *LPPROC_THREAD_ATTRIBUTE_LIST;
typedef NTSTATUS(NTAPI *_ZwCreateThreadEx)(
	OUT PHANDLE ThreadHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN HANDLE ProcessHandle,
	IN PTHREAD_START_ROUTINE StartRoutine,
	IN PVOID StartContext,
	IN ULONG CreateThreadFlags,
	IN SIZE_T ZeroBits OPTIONAL,
	IN SIZE_T StackSize OPTIONAL,
	IN SIZE_T MaximumStackSize OPTIONAL,
	IN PPROC_THREAD_ATTRIBUTE_LIST AttributeList
	);


typedef NTSTATUS (NTAPI*_ZwSuspendThread)(IN HANDLE ThreadHandle,OUT PULONG PreviousSuspendCount OPTIONAL);

typedef NTSTATUS(NTAPI*_ZwResumeThread)(
	IN HANDLE ThreadHandle,
	OUT PULONG PreviousSuspendCount OPTIONAL
	);

typedef NTSTATUS(NTAPI*_NtSuspendThread)(
	IN HANDLE ThreadHandle,
	OUT PULONG PreviousSuspendCount OPTIONAL
	);

typedef NTSTATUS(NTAPI*_NtResumeThread)(
	IN HANDLE ThreadHandle,
	OUT PULONG PreviousSuspendCount OPTIONAL
	);

typedef NTSTATUS(NTAPI *_NtTerminateThread)(HANDLE hThread, PULONG uExitCode);

typedef NTSYSAPI NTSTATUS(*ZWPROTECTVIRTUALMEMORY)(IN HANDLE     ProcessHandle,
	IN PVOID *     BaseAddress,
	IN SIZE_T *     NumberOfBytesToProtect,
	IN ULONG     NewAccessProtection,
	OUT PULONG     OldAccessProtection
	);

typedef NTSTATUS(NTAPI *_ZwGetContextThread)(

	IN HANDLE               ThreadHandle,
	OUT PCONTEXT            pContext);

typedef NTSTATUS(NTAPI *_ZwSetContextThread)(

	IN HANDLE               ThreadHandle,
	IN PCONTEXT             Context);


typedef  NTSTATUS(NTAPI *ZWPROTECTVIRTUALMEMORY)(IN HANDLE     ProcessHandle,
	IN PVOID *     BaseAddress,
	IN SIZE_T *     NumberOfBytesToProtect,
	IN ULONG     NewAccessProtection,
	OUT PULONG     OldAccessProtection
	);
typedef NTSTATUS(NTAPI *pfnNtAlertResumeThread)(HANDLE ThreadHandle, PULONG SuspendCount);
typedef NTSTATUS(NTAPI *pfnNtQueueApcThread)(HANDLE ThreadHandle, PIO_APC_ROUTINE ApcRoutine,
	PVOID ApcRoutineContext OPTIONAL, PIO_STATUS_BLOCK ApcStatusBlock OPTIONAL, 
	ULONG ApcReserved OPTIONAL);



typedef NTSYSAPI NTSTATUS(*ZWREADVIRTUALMEMORY)(
	IN HANDLE ProcessHandle,
	IN PVOID BaseAddress,
	OUT PVOID Buffer,
	IN ULONG BufferLength,
	OUT PULONG ReturnLength OPTIONAL
	);

typedef NTSYSAPI NTSTATUS(*ZWWRITEVIRTUALMEMORY)(
	IN HANDLE ProcessHandle,
	IN PVOID BaseAddress,
	OUT PVOID Buffer,
	IN ULONG BufferLength,
	OUT PULONG ReturnLength OPTIONAL
	);