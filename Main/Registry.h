#pragma once
#include <afxcmn.h>
#include "Common.h"
#include <list>
#include "CommonFunctions.h"

using namespace std;


#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
typedef LONG NTSTATUS;
#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L)    // ntsubauth
#define STATUS_UNSUCCESSFUL              ((NTSTATUS)0xC0000001L)
#define STATUS_NO_MORE_ENTRIES           ((NTSTATUS)0x8000001AL)
#define STATUS_INVALID_PARAMETER         ((NTSTATUS)0xC000000DL)    // winnt
#define STATUS_BUFFER_TOO_SMALL          ((NTSTATUS)0xC0000023L)
#define STATUS_BUFFER_OVERFLOW           ((NTSTATUS)0x80000005L)

typedef enum _KEY_VALUE_INFORMATION_CLASS {
	KeyValueBasicInformation,
	KeyValueFullInformation,
	KeyValuePartialInformation,
	KeyValueFullInformationAlign64,
	KeyValuePartialInformationAlign64,
	KeyValueLayerInformation,
	MaxKeyValueInfoClass
} KEY_VALUE_INFORMATION_CLASS;


typedef struct _KEY_VALUE_FULL_INFORMATION {
	ULONG TitleIndex;
	ULONG Type;
	ULONG DataOffset;
	ULONG DataLength;
	ULONG NameLength;
	WCHAR Name[1];
} KEY_VALUE_FULL_INFORMATION, *PKEY_VALUE_FULL_INFORMATION;

typedef struct _KEY_INFO {
	ULONG   nNameLen;
	CString szName;
} KEY_INFO, *PKEY_INFO;

typedef struct _VALUE_INFO {
	ULONG   Type;
	ULONG   nDataLen;
	ULONG   nNameLen;
	CString szName;
	PVOID pData;
} VALUE_INFO, *PVALUE_INFO;


class CRegistry
{
public:
	CRegistry();
	virtual ~CRegistry();
	void EnumKeys(CString szKey, list<KEY_INFO> &keyList);
	void EnumValues(CString szKey, list<VALUE_INFO> &valueList);
	HANDLE OpenKey(CString szKey);
	void FreeValueList(list<VALUE_INFO> &valueList);
	BOOL OpenKey(OUT PHANDLE  KeyHandle,IN ACCESS_MASK  DesiredAccess,IN POBJECT_ATTRIBUTES  ObjectAttributes);
	BOOL CRegistry::EnumerateKey(IN HANDLE  KeyHandle,IN ULONG  Index,IN ULONG  KeyInformationClass,OUT PVOID  KeyValueInformation,IN ULONG  Length,OUT PULONG  ResultLength);


private:
	CommonFunctions m_Function;
};



typedef  NTSTATUS(_stdcall *pfnZwOpenKey)(
	_Out_ PHANDLE KeyHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes
);


typedef NTSTATUS(_stdcall *pfnZwEnumerateValueKey)(
	_In_ HANDLE KeyHandle,
	_In_ ULONG Index,
	_In_ KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
	_Out_writes_bytes_to_opt_(Length, *ResultLength) PVOID KeyValueInformation,
	_In_ ULONG Length,
	_Out_ PULONG ResultLength
	);