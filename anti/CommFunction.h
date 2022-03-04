#pragma once
#include <ntifs.h>

typedef struct _MMPTE_HARDWARE            // 18 elements, 0x8 bytes (sizeof) 
{
	/*0x000*/     UINT64       Valid : 1;               // 0 BitPosition                   
	/*0x000*/     UINT64       Dirty1 : 1;              // 1 BitPosition                   
	/*0x000*/     UINT64       Owner : 1;               // 2 BitPosition                   
	/*0x000*/     UINT64       WriteThrough : 1;        // 3 BitPosition                   
	/*0x000*/     UINT64       CacheDisable : 1;        // 4 BitPosition                   
	/*0x000*/     UINT64       Accessed : 1;            // 5 BitPosition                   
	/*0x000*/     UINT64       Dirty : 1;               // 6 BitPosition                   
	/*0x000*/     UINT64       LargePage : 1;           // 7 BitPosition                   
	/*0x000*/     UINT64       Global : 1;              // 8 BitPosition                   
	/*0x000*/     UINT64       CopyOnWrite : 1;         // 9 BitPosition                   
	/*0x000*/     UINT64       Unused : 1;              // 10 BitPosition                  
	/*0x000*/     UINT64       Write : 1;               // 11 BitPosition                  
	/*0x000*/     UINT64       PageFrameNumber : 36;    // 12 BitPosition                  
	/*0x000*/     UINT64       ReservedForHardware : 4; // 48 BitPosition                  
	/*0x000*/     UINT64       ReservedForSoftware : 4; // 52 BitPosition                  
	/*0x000*/     UINT64       WsleAge : 4;             // 56 BitPosition                  
	/*0x000*/     UINT64       WsleProtection : 3;      // 60 BitPosition                  
	/*0x000*/     UINT64       NoExecute : 1;           // 63 BitPosition                  
}MMPTE_HARDWARE, *PMMPTE_HARDWARE;


#define SYSTEM_ADDRESS_START 0x00007ffffffeffff

#define SYSTEM_ADDRESS_START32 0x7fffffff

VOID CharToWchar(PCHAR src, PWCHAR dst);
VOID UnicodeToChar(PUNICODE_STRING dst, char *src);
VOID WcharToChar(PWCHAR src, PCHAR dst);
VOID ForceShutdown();
VOID ForceReboot();

PEPROCESS HandleToProcess(IN HANDLE hProcessHanle);

PETHREAD HandleToThread(IN HANDLE hThreadHanle);

NTSTATUS IoQueryFileDosDeviceNameSafeIrql(PFILE_OBJECT FileObject, POBJECT_NAME_INFORMATION *ObjectNameInformation);
NTSTATUS SafeCopyMemory_R3_to_R0(ULONG_PTR SrcAddr/*R3 要复制的地址*/, ULONG_PTR DstAddr/*在R0申请的地址*/, ULONG Size);
NTSTATUS SafeCopyMemory_R0_to_R3(PVOID SrcAddr/*R0要复制的地址*/, PVOID DstAddr/*返回 R3 的地址*/, ULONG Size);


NTSTATUS RtlSuperCopyMemory(IN VOID UNALIGNED* Destination, IN CONST VOID UNALIGNED* Source, IN ULONG Length);
NTSTATUS RtlSuperCopyMemoryEx(IN VOID UNALIGNED* Destination, IN CONST VOID UNALIGNED* Source, IN ULONG Length);

BOOLEAN InitializePteBase();
ULONG64 getPte(ULONG64 VirtualAddress);
ULONG64 getPteBase();
ULONG64 getPde(ULONG64 VirtualAddress);
ULONG64 getPdpte(ULONG64 VirtualAddress);
ULONG64 getPml4e(ULONG64 VirtualAddress);

char* GetPathA(const char *szCurFile, char a, ULONG nCount);
wchar_t* GetPathW(const wchar_t *szCurFile, wchar_t a, ULONG nCount);


typedef struct _INJECT_INFO_
{
	CHAR szProcessName[50];
	WCHAR szDllPath[256];

}INJECT_INFO,*PINJECT_INFO;