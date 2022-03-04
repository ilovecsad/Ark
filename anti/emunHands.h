#pragma once
#include <ntifs.h>
#include "peb.h"
#include "exapi.h"
#include "struct.h"
#include "vad.h"
#define HANDLE_VALUE_INC 4

#define TABLE_PAGE_SIZE	PAGE_SIZE
#define LOWLEVEL_COUNT (TABLE_PAGE_SIZE / sizeof(HANDLE_TABLE_ENTRY))
#define MIDLEVEL_COUNT (PAGE_SIZE / sizeof(PHANDLE_TABLE_ENTRY))

#define LEVEL_CODE_MASK 3


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




typedef struct _EXHANDLE
{
	union
	{
		struct
		{
			ULONG32 TagBits : 2;
			ULONG32 Index : 30;
		};
		HANDLE GenericHandleOverlay;
		ULONG_PTR Value;
	};
} EXHANDLE, *PEXHANDLE;

typedef struct _HANDLE_TABLE_ENTRY
{
	union
	{
		LONG_PTR VolatileLowValue;
		LONG_PTR LowValue;
		PVOID InfoTable;
		LONG_PTR RefCountField;
		struct
		{
			ULONG_PTR Unlocked : 1;
			ULONG_PTR RefCnt : 16;
			ULONG_PTR Attributes : 3;
			ULONG_PTR ObjectPointerBits : 44;
		};
	};
	union
	{
		LONG_PTR HighValue;
		struct _HANDLE_TABLE_ENTRY *NextFreeHandleEntry;
		EXHANDLE LeafHandleValue;
		struct
		{
			ULONG32 GrantedAccessBits : 25;
			ULONG32 NoRightsUpgrade : 1;
			ULONG32 Spare1 : 6;
		};
		ULONG32 Spare2;
	};
} HANDLE_TABLE_ENTRY, *PHANDLE_TABLE_ENTRY;

typedef struct _HANDLE_TABLE_FREE_LIST
{
	ULONG_PTR FreeListLock;
	PHANDLE_TABLE_ENTRY FirstFreeHandleEntry;
	PHANDLE_TABLE_ENTRY lastFreeHandleEntry;
	LONG32 HandleCount;
	ULONG32 HighWaterMark;
	ULONG32 Reserved[8];
} HANDLE_TABLE_FREE_LIST, *PHANDLE_TABLE_FREE_LIST;
typedef struct _HANDLE_TABLE
{
	ULONG32 NextHandleNeedingPool;
	LONG32 ExtraInfoPages;
	ULONG_PTR TableCode;
	PEPROCESS QuotaProcess;
	LIST_ENTRY HandleTableList;
	ULONG32 UniqueProcessId;
	union
	{
		ULONG32 Flags;
		struct
		{
			BOOLEAN StrictFIFO : 1;
			BOOLEAN EnableHandleExceptions : 1;
			BOOLEAN Rundown : 1;
			BOOLEAN Duplicated : 1;
			BOOLEAN RaiseUMExceptionOnInvalidHandleClose : 1;
		};
	};
	ULONG_PTR HandleContentionEvent;
	ULONG_PTR HandleTableLock;
	union
	{
		HANDLE_TABLE_FREE_LIST FreeLists[1];
		BOOLEAN ActualEntry[32];
	};
	PVOID DebugInfo;
} HANDLE_TABLE, *PHANDLE_TABLE;














PHANDLE_TABLE_ENTRY ExpLookupHandleTableEntry(
	IN PHANDLE_TABLE HandleTable,
	IN EXHANDLE tHandle);


NTSTATUS GetHandleObjectName(IN PVOID Object, IN PVOID szObjectName, ULONG nSize);
NTSTATUS EnumHands(HANDLE dwPid, PALL_HANDLES pBuffer, ULONG nCnt);

NTSTATUS EnumHandsByAPI(HANDLE dwPid, PALL_HANDLES pBuffer, ULONG nCnt);
VOID InsertHandleToList(PEPROCESS pEprocess, PSYSTEM_HANDLE_TABLE_ENTRY_INFO p, PALL_HANDLES pHandles);
VOID GetNameByObjectType(PMY_OBJECT_TYPE pObjectType, PVOID pObject, IN PVOID szObjectName, ULONG nSize);
VOID GetProcessPath(PEPROCESS a, PVOID szObjectName, ULONG nSize);

NTSTATUS GetGuidByTmRmObject(PKRESOURCEMANAGER pTmRm, OUT GUID* pGuid);
NTSTATUS  AnalysisTmTm(PKTM pKtm, PVOID strLogFileName, ULONG nSize);
VOID  AnalysisFile(PFILE_OBJECT pFileObject, PVOID szObjectName, ULONG nSize);
VOID  AnalysisSection(PSECTION pSection, PVOID strLogFileName, ULONG nSize);
VOID  AnalysisTokenObject(PTOKEN pToken, PVOID szObjectName, ULONG nSize);
