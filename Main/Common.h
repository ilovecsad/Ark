#pragma once
#include <windows.h>


#define IOCTL_UNKNOWN_BASE					            FILE_DEVICE_UNKNOWN
#define IOCTL_GET_EPROCESS					            CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0800, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_GET_PROCESS_COMMANDLINE_BY_PEB		    CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0801, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_CE_QUERYINFORMATIONPROCESS		        CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0802, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_CE_ENUMDRIVERS		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0803, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_ENUM_THREAD		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0804, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_ENUM_VAD		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0805, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_ENUM_MODULE_BYPEB		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0806, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_QUERYOBJECT_NAME		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0807, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_ENUM_MEMORY		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0808, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_ENUM_HANDLES		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0809, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_SUSPEND_PROCESS		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0810, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_RESUME_PROCESS		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0811, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS) 
#define IOCTL_KISS_PROCESS		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0812, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS) 
#define IOCTL_SUSPEND_THREAD		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0813, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_RESUME_THREAD		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0814, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS) 
#define IOCTL_KISS_THREAD		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0815, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS) 
#define IOCTL_FREE_MODULE		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0816, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS) 
#define IOCTL_DUMP_BUFFER		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0817, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS) 
#define IOCTL_HIDE_MODULE		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0818, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS) 
#define IOCTL_CHANGGE_MEMOORY_PROTECTION		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0819, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS) 
#define IOCTL_ALLOCATE_MEMORY		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0820, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS) 
#define IOCTL_FREE_MEMORY		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0821, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_MDL_WRITE_MEMORY		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0822, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_MDL_CHANGE_VAD_PROTECTION		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0823, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_CE_CREATETHREAD_EX		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0824, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_OPEN_THREAD		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0825, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_ENABLE_KERNEL_INJECT		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0826, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_SET_KERNEL_INJECT		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0827, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_UNLOAD_DRIVER		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0828, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_ENUM_CALLBACK		CTL_CODE(IOCTL_UNKNOWN_BASE, 0x0829, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
typedef struct _PROCESS_INFO_
{
	HANDLE ulPid;
	HANDLE ulParentPid;
	PVOID ulEprocess;
	ULONGLONG ulCreateTime;
	WCHAR szPath[300];
}PROCESS_INFO, *PPROCESS_INFO;

typedef struct _ALL_PROCESSES_
{
	ULONG_PTR nSize;
	PVOID ProcessInfo;
}ALL_PROCESSES, *PALL_PROCESSES;

typedef enum _ITEM_COLOR_
{
	enumBlack = 0,		// 微软的模块
	enumRed,			// 危险的项
	enumBlue,			// 其他的非微软模块
	enumPurple,			// 未带签名的项
	enumTuhuang,		// 微软的进程中带有非微软的模块
	enumGrey,
	enumGreen,
}ITEM_COLOR;

typedef struct _ITEM_COLOR_INFO_
{
	ITEM_COLOR textClr;  //文字颜色
	ITEM_COLOR bgdClr;   //背景颜色
}ITEM_COLOR_INFO, *PITEM_COLOR_INFO;


////////// 以下结构用来枚举内核模块 /////////////
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


#define OBJ_INHERIT             0x00000002L
#define OBJ_PERMANENT           0x00000010L
#define OBJ_EXCLUSIVE           0x00000020L
#define OBJ_CASE_INSENSITIVE    0x00000040L
#define OBJ_OPENIF              0x00000080L
#define OBJ_OPENLINK            0x00000100L
#define OBJ_KERNEL_HANDLE       0x00000200L
#define OBJ_FORCE_ACCESS_CHECK  0x00000400L
#define OBJ_VALID_ATTRIBUTES    0x000007F2L

#define InitializeObjectAttributes( p, n, a, r, s ) {		\
	(p)->Length = sizeof(OBJECT_ATTRIBUTES );      \
	(p)->RootDirectory = r;                             \
	(p)->Attributes = a;                                \
	(p)->ObjectName = n;                                \
	(p)->SecurityDescriptor = s;                        \
	(p)->SecurityQualityOfService = NULL;               \
}


////////////////////////////////////////////////////////////////////////

#define	CLASSES_ROOT		L"\\Registry\\Machine\\SOFTWARE\\Classes"
#define	LOCAL_MACHINE		L"\\Registry\\Machine"
#define USERS				L"\\Registry\\User"
#define CURRENT_CONFIGL     L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Hardware Profiles\\Current"

#define	REG_SERVICE			L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\"
#define SERVICE_KEY_NAME	L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services"

////////////////////////////////////////////////////////////////////////
typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _MZF_OBJECT_ATTRIBUTES {
	ULONG Length;
	HANDLE RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
	PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef struct _MZF_CLIENT_ID {
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} MZF_CLIENT_ID;
typedef MZF_CLIENT_ID *PMZF_CLIENT_ID;


typedef struct _ENUM_VALUE_
{
	PULONG RetLength;
	PVOID pValueInfo;
}ENUM_VALUE, *PENUM_VALUE;

typedef struct _CREATE_KEY_RETURN_
{
	PHANDLE pKeyHandle;
	PULONG  Disposition;
}CREATE_KEY_RETURN, *PCREATE_KEY_RETURN;

typedef struct _COMMUNICATE_KEY_
{
	//OPERATE_TYPE OpType;

	union {
		struct {
			ACCESS_MASK DesiredAccess;
			POBJECT_ATTRIBUTES ObjectAttributes;
		} Open;

		struct {
			HANDLE hKey;
			ULONG Index;
			ULONG InformationClass;
			ULONG Length;
		} Enum;

		struct {
			HANDLE hKey;
		} Delete;

		struct {
			HANDLE hKey;
			PUNICODE_STRING ValueName;
			ULONG Type;
			PVOID Data;
			ULONG DataSize;
		} SetValue;

		struct {
			HANDLE hKey;
			PUNICODE_STRING ValueName;
		} DeleteValue;

		struct {
			HANDLE hKey;
			PUNICODE_STRING NewName;
		} RenameKey;
	} op;

}COMMUNICATE_KEY, *PCOMMUNICATE_KEY;


typedef struct _KEY_BASIC_INFORMATION {
	LARGE_INTEGER LastWriteTime;
	ULONG   TitleIndex;
	ULONG   NameLength;
	WCHAR   Name[1];            // Variable length string
} KEY_BASIC_INFORMATION, *PKEY_BASIC_INFORMATION;


/////////////////////////////////////Thread///////////////////////////////////////


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

typedef struct _ALL_THREADS_
{
	ULONG nCnt;
	THREAD_INFO Threads[1];
}ALL_THREADS, *PALL_THREADS;


typedef struct _MODULE_INFO_EX
{
	ULONG Size;
	ULONG Sign;
	ULONG_PTR Base;
	WCHAR Path[MAX_PATH];
}MODULE_INFO_EX, *PMODULE_INFO_EX;

typedef struct _ALL_MODULES_
{
	ULONG nCnt;
	MODULE_INFO_EX Modules[1];
}ALL_MODULES, *PALL_MODULES;



typedef struct _MM_GRAPHICS_VAD_FLAGS        // 15 elements, 0x4 bytes (sizeof) 
{
	/*0x000*/     ULONG32      Lock : 1;                   // 0 BitPosition                   
	/*0x000*/     ULONG32      LockContended : 1;          // 1 BitPosition                   
	/*0x000*/     ULONG32      DeleteInProgress : 1;       // 2 BitPosition                   
	/*0x000*/     ULONG32      NoChange : 1;               // 3 BitPosition                   
	/*0x000*/     ULONG32      VadType : 3;                // 4 BitPosition                   
	/*0x000*/     ULONG32      Protection : 5;             // 7 BitPosition                   
	/*0x000*/     ULONG32      PreferredNode : 6;          // 12 BitPosition                  
	/*0x000*/     ULONG32      PageSize : 2;               // 18 BitPosition                  
	/*0x000*/     ULONG32      PrivateMemoryAlwaysSet : 1; // 20 BitPosition                  
	/*0x000*/     ULONG32      WriteWatch : 1;             // 21 BitPosition                  
	/*0x000*/     ULONG32      FixedLargePageSize : 1;     // 22 BitPosition                  
	/*0x000*/     ULONG32      ZeroFillPagesOptional : 1;  // 23 BitPosition                  
	/*0x000*/     ULONG32      GraphicsAlwaysSet : 1;      // 24 BitPosition                  
	/*0x000*/     ULONG32      GraphicsUseCoherentBus : 1; // 25 BitPosition                  
	/*0x000*/     ULONG32      GraphicsPageProtection : 3; // 26 BitPosition                  
}MM_GRAPHICS_VAD_FLAGS, *PMM_GRAPHICS_VAD_FLAGS;
typedef struct _MM_PRIVATE_VAD_FLAGS         // 15 elements, 0x4 bytes (sizeof) 
{
	/*0x000*/     ULONG32      Lock : 1;                   // 0 BitPosition                   
	/*0x000*/     ULONG32      LockContended : 1;          // 1 BitPosition                   
	/*0x000*/     ULONG32      DeleteInProgress : 1;       // 2 BitPosition                   
	/*0x000*/     ULONG32      NoChange : 1;               // 3 BitPosition                   
	/*0x000*/     ULONG32      VadType : 3;                // 4 BitPosition                   
	/*0x000*/     ULONG32      Protection : 5;             // 7 BitPosition                   
	/*0x000*/     ULONG32      PreferredNode : 6;          // 12 BitPosition                  
	/*0x000*/     ULONG32      PageSize : 2;               // 18 BitPosition                  
	/*0x000*/     ULONG32      PrivateMemoryAlwaysSet : 1; // 20 BitPosition                  
	/*0x000*/     ULONG32      WriteWatch : 1;             // 21 BitPosition                  
	/*0x000*/     ULONG32      FixedLargePageSize : 1;     // 22 BitPosition                  
	/*0x000*/     ULONG32      ZeroFillPagesOptional : 1;  // 23 BitPosition                  
	/*0x000*/     ULONG32      Graphics : 1;               // 24 BitPosition                  
	/*0x000*/     ULONG32      Enclave : 1;                // 25 BitPosition                  
	/*0x000*/     ULONG32      ShadowStack : 1;            // 26 BitPosition                  
}MM_PRIVATE_VAD_FLAGS, *PMM_PRIVATE_VAD_FLAGS;


typedef struct _MMVAD_FLAGS            // 9 elements, 0x4 bytes (sizeof) 
{
	/*0x000*/     ULONG32      Lock : 1;             // 0 BitPosition                  
	/*0x000*/     ULONG32      LockContended : 1;    // 1 BitPosition                  
	/*0x000*/     ULONG32      DeleteInProgress : 1; // 2 BitPosition                  
	/*0x000*/     ULONG32      NoChange : 1;         // 3 BitPosition                  
	/*0x000*/     ULONG32      VadType : 3;          // 4 BitPosition                  
	/*0x000*/     ULONG32      Protection : 5;       // 7 BitPosition                  
	/*0x000*/     ULONG32      PreferredNode : 6;    // 12 BitPosition                 
	/*0x000*/     ULONG32      PageSize : 2;         // 18 BitPosition                 
	/*0x000*/     ULONG32      PrivateMemory : 1;    // 20 BitPosition                 
}MMVAD_FLAGS, *PMMVAD_FLAGS;

typedef struct _MM_SHARED_VAD_FLAGS            // 11 elements, 0x4 bytes (sizeof) 
{
	/*0x000*/     ULONG32      Lock : 1;                     // 0 BitPosition                   
	/*0x000*/     ULONG32      LockContended : 1;            // 1 BitPosition                   
	/*0x000*/     ULONG32      DeleteInProgress : 1;         // 2 BitPosition                   
	/*0x000*/     ULONG32      NoChange : 1;                 // 3 BitPosition                   
	/*0x000*/     ULONG32      VadType : 3;                  // 4 BitPosition                   
	/*0x000*/     ULONG32      Protection : 5;               // 7 BitPosition                   
	/*0x000*/     ULONG32      PreferredNode : 6;            // 12 BitPosition                  
	/*0x000*/     ULONG32      PageSize : 2;                 // 18 BitPosition                  
	/*0x000*/     ULONG32      PrivateMemoryAlwaysClear : 1; // 20 BitPosition                  
	/*0x000*/     ULONG32      PrivateFixup : 1;             // 21 BitPosition                  
	/*0x000*/     ULONG32      HotPatchAllowed : 1;          // 22 BitPosition                  
}MM_SHARED_VAD_FLAGS, *PMM_SHARED_VAD_FLAGS;

typedef struct _VAD_INFO_
{
	ULONG_PTR pVad;
	ULONG_PTR startVpn;
	ULONG_PTR endVpn;
	ULONG_PTR pFileObject;
	ULONG_PTR flags;
}VAD_INFO, *PVAD_INFO;

typedef struct _ALL_VADS_
{
	ULONG nCnt;
	VAD_INFO VadInfos[1];
}ALL_VADS, *PALL_VADS;

#define MM_ZERO_ACCESS         0  // this value is not used.
#define MM_READONLY            1
#define MM_EXECUTE             2
#define MM_EXECUTE_READ        3
#define MM_READWRITE           4  // bit 2 is set if this is writable.
#define MM_WRITECOPY           5
#define MM_EXECUTE_READWRITE   6
#define MM_EXECUTE_WRITECOPY   7




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



typedef struct _TOKEN_INFO
{
	ULONG TokenId;
	WCHAR  AccountName[100];
	WCHAR AuthorityName[100];
	WCHAR TokenType[50];

}TOKEN_INFO, *PTOKEN_INFO;

typedef struct _HANDLE_INFO_
{
	ULONG Handle;
	ULONG ReferenceCount;
	ULONG Id;
	ULONG grantAcess;
	ULONG_PTR Object;
	WCHAR ObjectName[50];
	union
	{
		GUID dwGuid;
		TOKEN_INFO tokenInfo;
		WCHAR HandleName[300];
	}u;
}HANDLE_INFO, *PHANDLE_INFO;

typedef struct _PROCESS_HANDLES_
{
	ULONG nCnt;
	HANDLE_INFO Handles[1];
}ALL_HANDLES, *PALL_HANDLES;


typedef enum _SUSPEND_OR_RESUME_TYPE_
{
	enumAllFalse,
	enumAllOk,
	enumSuspend,
	enumResume,
}SUSPEND_OR_RESUME_TYPE;
typedef struct _RESUME_OR_SUSPEND_
{
	ULONG bSuspend;
	ULONG bResume;
}RESUME_OR_SUSPEND, *PRESUME_OR_SUSPEND;



typedef struct _OB_CALLBACK
{
	LIST_ENTRY             ListEntry;
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