#pragma once
#include "CommFunction.h"

#define  DWORD ULONG32
static PVOID PsNtDllBase64 = NULL;
static PVOID fnLdrLoadDll64 = NULL;
static PVOID fnProtectVirtualMemory64 = NULL;
static PVOID fnHookFunc64 = NULL;

static PVOID PsNtDllBase32 = NULL;
static PVOID fnLdrLoadDll32 = NULL;
static PVOID fnProtectVirtualMemory32 = NULL;
static PVOID fnHookFunc32 = NULL;

typedef
NTSTATUS

(*NTPROTECTVIRTUALMEMORY)(IN HANDLE ProcessHandle,
	IN OUT PVOID *UnsafeBaseAddress,
	IN OUT SIZE_T *UnsafeNumberOfBytesToProtect,
	IN ULONG NewAccessProtection,
	OUT PULONG UnsafeOldAccessProtection);

typedef NTSTATUS(*LDRLOADDLL)(
	IN PWCHAR PathToFile OPTIONAL,
	IN ULONG Flags OPTIONAL,
	IN PUNICODE_STRING ModuleFileName,
	OUT PHANDLE ModuleHandle
	);
typedef struct _INJECT_BUFFER
{
	UCHAR code[0x200];
	UCHAR original_code[8];
	PVOID hook_func;
	union
	{
		UNICODE_STRING path;
		UNICODE_STRING32 path32;
	};

	wchar_t buffer[488];
	PVOID module;
} INJECT_BUFFER, *PINJECT_BUFFER;

PINJECT_BUFFER GetInlineHookCode32(IN HANDLE ProcessHandle, IN PUNICODE_STRING pDllPath);
PINJECT_BUFFER GetInlineHookCode64(IN HANDLE ProcessHandle, IN PUNICODE_STRING pDllPath);
PVOID AllocateInjectMemory(IN HANDLE ProcessHandle, IN PVOID DesiredAddress, IN SIZE_T DesiredSize);
NTSTATUS InjectByHook64(HANDLE ProcessId, PVOID ImageBase, PUNICODE_STRING pDllPath/*要注入的dll*/);
NTSTATUS InjectByHook32(HANDLE ProcessId, PVOID ImageBase, PUNICODE_STRING pDllPath/*要注入的dll*/);
VOID LoadImageNotifyCallback(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO pImageInfo);


typedef struct _PARAMX64_
{
	ULONG64 LdrGetProcedureAddress;
	ULONG64 ZwAllocateVirtualMemory;
	ULONG64 LdrLoadDll;
	ULONG64 RtlInitAnsiString;
	ULONG64 RtlAnsiStringToUnicodeString;
	ULONG64 RtlFreeUnicodeString;
	ULONG64 ZwCreateSection;
	ULONG64 ZwMapViewOfSection;
	ULONG64 ZwUnmapViewOfSection;
	ULONG64 ZwCreateFile;
	ULONG64 RtlInitUnicodeString;
	ULONG64 ZwQueryInformationFile;
	ULONG64 ZwClose;
	BOOLEAN bIsAddress;
	WCHAR szDllPath[120];
}PARAMX64, *PPARAMX64;

typedef struct _PARAMX32_
{
	ULONG32 LdrGetProcedureAddress;
	ULONG32 ZwAllocateVirtualMemory;
	ULONG32 LdrLoadDll;
	ULONG32 RtlInitAnsiString;
	ULONG32 RtlAnsiStringToUnicodeString;
	ULONG32 RtlFreeUnicodeString;
	ULONG32 ZwCreateSection;
	ULONG32 ZwMapViewOfSection;
	ULONG32 ZwUnmapViewOfSection;
	ULONG32 ZwCreateFile;
	ULONG32 RtlInitUnicodeString;
	ULONG32 ZwQueryInformationFile;
	ULONG32 ZwClose;
	BOOLEAN bIsAddress;
	WCHAR szDllPath[120];
}PARAMX32, *PPARAMX32;

NTSTATUS HideInjectByHook32(HANDLE ProcessId, PVOID ImageBase, PUNICODE_STRING pDllPath/*要注入的dll*/);
NTSTATUS HideInjectByHook64(HANDLE ProcessId, PVOID ImageBase, PUNICODE_STRING pDllPath/*要注入的dll*/);