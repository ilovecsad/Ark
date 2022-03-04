#pragma once
#include <ntifs.h>
#include "zwfun.h"
#include "CommFunction.h"
NTKERNELAPI PVOID NTAPI ObGetObjectType(IN PVOID pObject);
NTKERNELAPI PVOID NTAPI PsGetProcessPeb(
	_In_ PEPROCESS Process
);


NTKERNELAPI CHAR* PsGetProcessImageFileName(
	__in PEPROCESS Process
);

NTKERNELAPI LONGLONG PsGetProcessCreateTimeQuadPart( PEPROCESS Process);

NTKERNELAPI HANDLE PsGetProcessInheritedFromUniqueProcessId(IN PEPROCESS Process);


//获取 进程文件对象
NTKERNELAPI NTSTATUS
PsReferenceProcessFilePointer(
	IN PEPROCESS Process,
	OUT PVOID *pFilePointer
);
NTKERNELAPI NTSTATUS
IoQueryFileDosDeviceName(
	IN PFILE_OBJECT FileObject,
	OUT POBJECT_NAME_INFORMATION *ObjectNameInformation
);

NTKERNELAPI PVOID NTAPI PsGetThreadWin32Thread(PETHREAD pEthread);
NTKERNELAPI PVOID NTAPI PsGetProcessWow64Process(PEPROCESS pProcess);

NTKERNELAPI PVOID NTAPI PsGetThreadTeb(PETHREAD pEthread);
NTKERNELAPI NTSTATUS NTAPI ZwQueryInformationProcess(IN HANDLE ProcessHandle, IN PROCESSINFOCLASS ProcessInformationClass, OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT PULONG ReturnLength OPTIONAL);


NTKERNELAPI
NTSTATUS
NTAPI
PsAcquireProcessExitSynchronization(
	_In_ PEPROCESS Process
);

NTKERNELAPI
VOID
NTAPI
PsReleaseProcessExitSynchronization(
	_In_ PEPROCESS Process
);


typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBasicInformation,
	SystemProcessorInformation,
	SystemPerformanceInformation,
	SystemTimeOfDayInformation,
	SystemNotImplemented1,
	SystemProcessesInformation,
	SystemCallCounts,
	SystemConfigurationInformation,
	SystemProcessorTimes,
	SystemGlobalFlag,
	SystemNotImplemented2,
	SystemModuleInformation,
	SystemLockInformation,
	SystemNotImplemented3,
	SystemNotImplemented4,
	SystemNotImplemented5,
	SystemHandleInformation, //枚举系统中的全部句柄 
	SystemObjectInformation,
	SystemPagefileInformation,
	SystemInstructionEmulationCounts,
	SystemInvalidInfoClass1,
	SystemCacheInformation,
	SystemPoolTagInformation,
	SystemProcessorStatistics,
	SystemDpcInformation,
	SystemNotImplemented6,
	SystemLoadImage,
	SystemUnloadImage,
	SystemTimeAdjustment,
	SystemNotImplemented7,
	SystemNotImplemented8,
	SystemNotImplemented9,
	SystemCrashDumpInformation,
	SystemExceptionInformation,
	SystemCrashDumpStateInformation,
	SystemKernelDebuggerInformation,
	SystemContextSwitchInformation,
	SystemRegistryQuotaInformation,
	SystemLoadAndCallImage,
	SystemPrioritySeparation,
	SystemNotImplemented10,
	SystemNotImplemented11,
	SystemInvalidInfoClass2,
	SystemInvalidInfoClass3,
	SystemTimeZoneInformation,
	SystemLookasideInformation,
	SystemSetTimeSlipEvent,
	SystemCreateSession,
	SystemDeleteSession,
	SystemInvalidInfoClass4,
	SystemRangeStartInformation,
	SystemVerifierInformation,
	SystemAddVerifier,
	SystemSessionProcessesInformation
} SYSTEM_INFORMATION_CLASS, *PSYSTEM_INFORMATION_CLASS;

NTKERNELAPI NTSTATUS NTAPI ZwQuerySystemInformation(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	ULONG *ReturnLength);

NTKERNELAPI //声明要使用此函数
NTSTATUS //返回类型
PsSuspendProcess(PEPROCESS Process);


NTKERNELAPI //声明要使用此函数
NTSTATUS //返回类型
PsResumeProcess(PEPROCESS Process);


NTKERNELAPI NTSTATUS MmUnmapViewOfSection
(IN PEPROCESS Process, //进程的 EPROCESS
	IN PVOID BaseAddress); //DLL 模块基址

NTKERNELAPI ULONG NTAPI KeCapturePersistentThreadState(PCONTEXT Context, PKTHREAD Thread, ULONG BugCheckCode, ULONG_PTR BugCheckParameter1, ULONG_PTR BugCheckParameter2, ULONG_PTR BugCheckParameter3, ULONG_PTR BugCheckParameter4, PVOID VirtualAddress);
extern ULONG_PTR Kthread_offset_ContextSwitches;
extern ULONG_PTR Kthread_offset_state;
extern ULONG_PTR Kthread_offset_Win32StartAddress;
extern ULONG_PTR Kthread_offset_SuspendCount;
extern ULONG_PTR enumObjectTableOffset_EPROCESS;
extern ULONG_PTR eprocess_offset_VadHint;
extern ULONG_PTR eprocess_offset_VadRoot;

extern _NtSuspendThread NtSuspendThread;
extern _NtResumeThread  NtResumeThread;
extern _NtTerminateThread NtTerminateThread;
extern ZWPROTECTVIRTUALMEMORY pfnZwProtectVirtualMemory;
extern ZWREADVIRTUALMEMORY pfnZwReadVirtualMemory;
extern ZWWRITEVIRTUALMEMORY pfnZwWriteVirtualMemory;
extern _ZwCreateThreadEx ZwCreateThreadEx;

extern BOOLEAN g_InitLoadImage;
extern UNICODE_STRING m_GlobalInjectDllPath64;
extern UNICODE_STRING m_GlobalInjectDllPath32;
extern ANSI_STRING    m_GlobalProcessName;

extern PINJECT_INFO g_pInjectInfo;