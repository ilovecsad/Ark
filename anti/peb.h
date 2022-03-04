#pragma once
#include <ntifs.h>

typedef struct _CURDIR              // 2 elements, 0x18 bytes (sizeof) 
{
	/*0x000*/     struct _UNICODE_STRING DosPath; // 3 elements, 0x10 bytes (sizeof) 
	/*0x010*/     VOID*        Handle;
}CURDIR, *PCURDIR;
typedef struct _RTL_DRIVE_LETTER_CURDIR // 4 elements, 0x18 bytes (sizeof) 
{
	/*0x000*/     UINT16       Flags;
	/*0x002*/     UINT16       Length;
	/*0x004*/     ULONG32      TimeStamp;
	/*0x008*/     struct _STRING DosPath;             // 3 elements, 0x10 bytes (sizeof) 
}RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;


typedef struct _RTL_USER_PROCESS_PARAMETERS                // 37 elements, 0x440 bytes (sizeof) 
{
	/*0x000*/     ULONG32      MaximumLength;
	/*0x004*/     ULONG32      Length;
	/*0x008*/     ULONG32      Flags;
	/*0x00C*/     ULONG32      DebugFlags;
	/*0x010*/     VOID*        ConsoleHandle;
	/*0x018*/     ULONG32      ConsoleFlags;
	/*0x01C*/     UINT8        _PADDING0_[0x4];
	/*0x020*/     VOID*        StandardInput;
	/*0x028*/     VOID*        StandardOutput;
	/*0x030*/     VOID*        StandardError;
	/*0x038*/     struct _CURDIR CurrentDirectory;                       // 2 elements, 0x18 bytes (sizeof)   
	/*0x050*/     struct _UNICODE_STRING DllPath;                        // 3 elements, 0x10 bytes (sizeof)   
	/*0x060*/     struct _UNICODE_STRING ImagePathName;                  // 3 elements, 0x10 bytes (sizeof)   
	/*0x070*/     struct _UNICODE_STRING CommandLine;                    // 3 elements, 0x10 bytes (sizeof)   
	/*0x080*/     VOID*        Environment;
	/*0x088*/     ULONG32      StartingX;
	/*0x08C*/     ULONG32      StartingY;
	/*0x090*/     ULONG32      CountX;
	/*0x094*/     ULONG32      CountY;
	/*0x098*/     ULONG32      CountCharsX;
	/*0x09C*/     ULONG32      CountCharsY;
	/*0x0A0*/     ULONG32      FillAttribute;
	/*0x0A4*/     ULONG32      WindowFlags;
	/*0x0A8*/     ULONG32      ShowWindowFlags;
	/*0x0AC*/     UINT8        _PADDING1_[0x4];
	/*0x0B0*/     struct _UNICODE_STRING WindowTitle;                    // 3 elements, 0x10 bytes (sizeof)   
	/*0x0C0*/     struct _UNICODE_STRING DesktopInfo;                    // 3 elements, 0x10 bytes (sizeof)   
	/*0x0D0*/     struct _UNICODE_STRING ShellInfo;                      // 3 elements, 0x10 bytes (sizeof)   
	/*0x0E0*/     struct _UNICODE_STRING RuntimeData;                    // 3 elements, 0x10 bytes (sizeof)   
	/*0x0F0*/     struct _RTL_DRIVE_LETTER_CURDIR CurrentDirectores[32];
	/*0x3F0*/     UINT64       EnvironmentSize;
	/*0x3F8*/     UINT64       EnvironmentVersion;
	/*0x400*/     VOID*        PackageDependencyData;
	/*0x408*/     ULONG32      ProcessGroupId;
	/*0x40C*/     ULONG32      LoaderThreads;
	/*0x410*/     struct _UNICODE_STRING RedirectionDllName;             // 3 elements, 0x10 bytes (sizeof)   
	/*0x420*/     struct _UNICODE_STRING HeapPartitionName;              // 3 elements, 0x10 bytes (sizeof)   
	/*0x430*/     UINT64*      DefaultThreadpoolCpuSetMasks;
	/*0x438*/     ULONG32      DefaultThreadpoolCpuSetMaskCount;
	/*0x43C*/     UINT8        _PADDING2_[0x4];
}RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;


typedef struct _PEB_LDR_DATA                            // 9 elements, 0x58 bytes (sizeof) 
{
	/*0x000*/     ULONG32      Length;
	/*0x004*/     UINT8        Initialized;
	/*0x005*/     UINT8        _PADDING0_[0x3];
	/*0x008*/     VOID*        SsHandle;
	/*0x010*/     struct _LIST_ENTRY InLoadOrderModuleList;           // 2 elements, 0x10 bytes (sizeof) 
	/*0x020*/     struct _LIST_ENTRY InMemoryOrderModuleList;         // 2 elements, 0x10 bytes (sizeof) 
	/*0x030*/     struct _LIST_ENTRY InInitializationOrderModuleList; // 2 elements, 0x10 bytes (sizeof) 
	/*0x040*/     VOID*        EntryInProgress;
	/*0x048*/     UINT8        ShutdownInProgress;
	/*0x049*/     UINT8        _PADDING1_[0x7];
	/*0x050*/     VOID*        ShutdownThreadId;
}PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _PEB64
{
	UCHAR InheritedAddressSpace;
	UCHAR ReadImageFileExecOptions;
	UCHAR BeingDebugged;
	UCHAR BitField;
	ULONG64 Mutant;
	ULONG64 ImageBaseAddress;
	PPEB_LDR_DATA Ldr;
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
	ULONG64 SubSystemData;
	ULONG64 ProcessHeap;
	ULONG64 FastPebLock;
	ULONG64 AtlThunkSListPtr;
	ULONG64 IFEOKey;
	ULONG64 CrossProcessFlags;
	ULONG64 UserSharedInfoPtr;
	ULONG SystemReserved;
	ULONG AtlThunkSListPtr32;
	ULONG64 ApiSetMap;
} PEB64, *PPEB64;







#pragma pack(4)


typedef struct _PEB32
{
	UCHAR InheritedAddressSpace;
	UCHAR ReadImageFileExecOptions;
	UCHAR BeingDebugged;
	UCHAR BitField;
	ULONG Mutant;
	ULONG ImageBaseAddress;
	ULONG Ldr;
	ULONG ProcessParameters;
	ULONG SubSystemData;
	ULONG ProcessHeap;
	ULONG FastPebLock;
	ULONG AtlThunkSListPtr;
	ULONG IFEOKey;
	ULONG CrossProcessFlags;
	ULONG UserSharedInfoPtr;
	ULONG SystemReserved;
	ULONG AtlThunkSListPtr32;
	ULONG ApiSetMap;
} PEB32, *PPEB32;

typedef struct _PEB_LDR_DATA32
{
	ULONG Length;
	BOOLEAN Initialized;
	ULONG SsHandle;
	LIST_ENTRY32 InLoadOrderModuleList;
	LIST_ENTRY32 InMemoryOrderModuleList;
	LIST_ENTRY32 InInitializationOrderModuleList;
	ULONG EntryInProgress;
} PEB_LDR_DATA32, *PPEB_LDR_DATA32;

typedef struct _LDR_DATA_TABLE_ENTRY32
{
	LIST_ENTRY32 InLoadOrderLinks;
	LIST_ENTRY32 InMemoryOrderModuleList;
	LIST_ENTRY32 InInitializationOrderModuleList;
	ULONG DllBase;
	ULONG EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING32 FullDllName;
	UNICODE_STRING32 BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union
	{
		LIST_ENTRY32 HashLinks;
		ULONG SectionPointer;
	}u1;
	ULONG CheckSum;
	union
	{
		ULONG TimeDateStamp;
		ULONG LoadedImports;
	}u2;
	ULONG EntryPointActivationContext;
	ULONG PatchInformation;
} LDR_DATA_TABLE_ENTRY32, *PLDR_DATA_TABLE_ENTRY32;

#pragma pack()


typedef enum _SYSTEM_DLL_TYPE  // 7 elements, 0x4 bytes
{
	PsNativeSystemDll = 0 /*0x0*/,
	PsWowX86SystemDll = 1 /*0x1*/,
	PsWowArm32SystemDll = 2 /*0x2*/,
	PsWowAmd64SystemDll = 3 /*0x3*/,
	PsWowChpeX86SystemDll = 4 /*0x4*/,
	PsVsmEnclaveRuntimeDll = 5 /*0x5*/,
	PsSystemDllTotalTypes = 6 /*0x6*/
}SYSTEM_DLL_TYPE, *PSYSTEM_DLL_TYPE;

typedef struct _EWOW64PROCESS        // 3 elements, 0x10 bytes (sizeof) 
{
	/*0x000*/     VOID*        Peb;
	/*0x008*/     UINT16       Machine;
	/*0x00A*/     UINT8        _PADDING0_[0x2];
	/*0x00C*/     enum _SYSTEM_DLL_TYPE NtdllType;
}EWOW64PROCESS, *PEWOW64PROCESS;


BOOLEAN initVerSion();


