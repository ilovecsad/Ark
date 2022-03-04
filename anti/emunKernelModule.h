#pragma once
#include <ntifs.h>
#define  MAX_PATH 260


#define NUMBER_HASH_BUCKETS 37
typedef struct _OBJECT_DIRECTORY_ENTRY
{
	struct _OBJECT_DIRECTORY_ENTRY *ChainLink;
	PVOID Object;
	ULONG HashValue;
} OBJECT_DIRECTORY_ENTRY, *POBJECT_DIRECTORY_ENTRY;

typedef struct _OBJECT_DIRECTORY
{
	struct _OBJECT_DIRECTORY_ENTRY *HashBuckets[NUMBER_HASH_BUCKETS];
} OBJECT_DIRECTORY, *POBJECT_DIRECTORY;

typedef enum _DRIVER_TYPE_
{
	enumHide,
	enumHaveOrder,
	enumSuspiciousPe,
}DRIVER_TYPE;
typedef struct _DRIVER_INFO_
{
	DRIVER_TYPE DriverType;
	ULONG_PTR nLodeOrder;
	ULONG_PTR nBase;
	ULONG_PTR nSize;
	ULONG_PTR nDriverObject;
	WCHAR szDriverPath[MAX_PATH];
}DRIVER_INFO, *PDRIVER_INFO;

typedef struct _ALL_DRIVERS_
{
	ULONG nCnt;
	DRIVER_INFO Drivers[1];
}ALL_DRIVERS, *PALL_DRIVERS;



typedef enum _LDR_DLL_LOAD_REASON  // 10 elements, 0x4 bytes
{
	LoadReasonStaticDependency = 0 /*0x0*/,
	LoadReasonStaticForwarderDependency = 1 /*0x1*/,
	LoadReasonDynamicForwarderDependency = 2 /*0x2*/,
	LoadReasonDelayloadDependency = 3 /*0x3*/,
	LoadReasonDynamicLoad = 4 /*0x4*/,
	LoadReasonAsImageLoad = 5 /*0x5*/,
	LoadReasonAsDataLoad = 6 /*0x6*/,
	LoadReasonEnclavePrimary = 7 /*0x7*/,
	LoadReasonEnclaveDependency = 8 /*0x8*/,
	LoadReasonUnknown = -1 /*0xFF*/
}LDR_DLL_LOAD_REASON, *PLDR_DLL_LOAD_REASON;

typedef struct _LDR_DATA_TABLE_ENTRY                         // 59 elements, 0x120 bytes (sizeof) 
{
	/*0x000*/     struct _LIST_ENTRY InLoadOrderLinks;                     // 2 elements, 0x10 bytes (sizeof)   
	/*0x010*/     struct _LIST_ENTRY InMemoryOrderLinks;                   // 2 elements, 0x10 bytes (sizeof)   
	/*0x020*/     struct _LIST_ENTRY InInitializationOrderLinks;           // 2 elements, 0x10 bytes (sizeof)   
	/*0x030*/     VOID*        DllBase;
	/*0x038*/     VOID*        EntryPoint;
	/*0x040*/     ULONG32      SizeOfImage;
	/*0x044*/     UINT8        _PADDING0_[0x4];
	/*0x048*/     struct _UNICODE_STRING FullDllName;                      // 3 elements, 0x10 bytes (sizeof)   
	/*0x058*/     struct _UNICODE_STRING BaseDllName;                      // 3 elements, 0x10 bytes (sizeof)   
	union                                                    // 3 elements, 0x4 bytes (sizeof)    
	{
		/*0x068*/         UINT8        FlagGroup[4];
		struct                                               // 28 elements, 0x4 bytes (sizeof)   
		{
			/*0x068*/             ULONG32      PackagedBinary : 1;                 // 0 BitPosition                     
			/*0x068*/             ULONG32      MarkedForRemoval : 1;               // 1 BitPosition                     
			/*0x068*/             ULONG32      ImageDll : 1;                       // 2 BitPosition                     
			/*0x068*/             ULONG32      LoadNotificationsSent : 1;          // 3 BitPosition                     
			/*0x068*/             ULONG32      TelemetryEntryProcessed : 1;        // 4 BitPosition                     
			/*0x068*/             ULONG32      ProcessStaticImport : 1;            // 5 BitPosition                     
			/*0x068*/             ULONG32      InLegacyLists : 1;                  // 6 BitPosition                     
			/*0x068*/             ULONG32      InIndexes : 1;                      // 7 BitPosition                     
			/*0x068*/             ULONG32      ShimDll : 1;                        // 8 BitPosition                     
			/*0x068*/             ULONG32      InExceptionTable : 1;               // 9 BitPosition                     
			/*0x068*/             ULONG32      ReservedFlags1 : 2;                 // 10 BitPosition                    
			/*0x068*/             ULONG32      LoadInProgress : 1;                 // 12 BitPosition                    
			/*0x068*/             ULONG32      LoadConfigProcessed : 1;            // 13 BitPosition                    
			/*0x068*/             ULONG32      EntryProcessed : 1;                 // 14 BitPosition                    
			/*0x068*/             ULONG32      ProtectDelayLoad : 1;               // 15 BitPosition                    
			/*0x068*/             ULONG32      ReservedFlags3 : 2;                 // 16 BitPosition                    
			/*0x068*/             ULONG32      DontCallForThreads : 1;             // 18 BitPosition                    
			/*0x068*/             ULONG32      ProcessAttachCalled : 1;            // 19 BitPosition                    
			/*0x068*/             ULONG32      ProcessAttachFailed : 1;            // 20 BitPosition                    
			/*0x068*/             ULONG32      CorDeferredValidate : 1;            // 21 BitPosition                    
			/*0x068*/             ULONG32      CorImage : 1;                       // 22 BitPosition                    
			/*0x068*/             ULONG32      DontRelocate : 1;                   // 23 BitPosition                    
			/*0x068*/             ULONG32      CorILOnly : 1;                      // 24 BitPosition                    
			/*0x068*/             ULONG32      ChpeImage : 1;                      // 25 BitPosition                    
			/*0x068*/             ULONG32      ReservedFlags5 : 2;                 // 26 BitPosition                    
			/*0x068*/             ULONG32      Redirected : 1;                     // 28 BitPosition                    
			/*0x068*/             ULONG32      ReservedFlags6 : 2;                 // 29 BitPosition                    
			/*0x068*/             ULONG32      CompatDatabaseProcessed : 1;        // 31 BitPosition                    
		}Flags;
	}u;
	/*0x06C*/     UINT16       ObsoleteLoadCount;
	/*0x06E*/     UINT16       TlsIndex;
	/*0x070*/     struct _LIST_ENTRY HashLinks;                            // 2 elements, 0x10 bytes (sizeof)   
	/*0x080*/     ULONG32      TimeDateStamp;
	/*0x084*/     UINT8        _PADDING1_[0x4];
	/*0x088*/     PVOID EntryPointActivationContext;
	/*0x090*/     VOID*        Lock;
	/*0x098*/     PVOID DdagNode;
	/*0x0A0*/     struct _LIST_ENTRY NodeModuleLink;                       // 2 elements, 0x10 bytes (sizeof)   
	/*0x0B0*/     PVOID LoadContext;
	/*0x0B8*/     VOID*        ParentDllBase;
	/*0x0C0*/     VOID*        SwitchBackContext;
	/*0x0C8*/     struct _RTL_BALANCED_NODE BaseAddressIndexNode;          // 6 elements, 0x18 bytes (sizeof)   
	/*0x0E0*/     struct _RTL_BALANCED_NODE MappingInfoIndexNode;          // 6 elements, 0x18 bytes (sizeof)   
	/*0x0F8*/     UINT64       OriginalBase;
	/*0x100*/     union _LARGE_INTEGER LoadTime;                           // 4 elements, 0x8 bytes (sizeof)    
	/*0x108*/     ULONG32      BaseNameHashValue;
	/*0x10C*/     enum _LDR_DLL_LOAD_REASON LoadReason;
	/*0x110*/     ULONG32      ImplicitPathOptions;
	/*0x114*/     ULONG32      ReferenceCount;
	/*0x118*/     ULONG32      DependentLoadFlags;
	/*0x11C*/     UINT8        SigningLevel;
	/*0x11D*/     UINT8        _PADDING2_[0x3];
}LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;





BOOLEAN IsUnicodeStringValid(PUNICODE_STRING unString);
NTSTATUS EnumDriverByLdrDataTableEntry(PALL_DRIVERS pDriversInfo, ULONG nCnt);
VOID WalkerDirectoryObject(PALL_DRIVERS pDriversInfo, PVOID pDirectoryObject, ULONG nCnt);
void InsertDriver(PALL_DRIVERS pDriversInfo, PDRIVER_OBJECT pDriverObject, ULONG nCnt);
BOOLEAN AddDriverObjectToDriverInList(PALL_DRIVERS pDriversInfo, PDRIVER_OBJECT pDriverObject, ULONG nCnt);
BOOLEAN IsAddressValid(PVOID pAddress, ULONG nLen);
NTSTATUS EnumDriversByWalkerDirectoryObject(PALL_DRIVERS pDriversInfo, ULONG nCnt);
BOOLEAN IsRealDriverObject(PDRIVER_OBJECT DriverObject);
NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject);