#include "zwfun.h"
#include "pe.h"
#include "exapi.h"
unsigned char* FileData = 0;
ULONG FileSize = 0;

void RtlFreeMemory(void* InPointer)
{
	ExFreePool(InPointer);
}
void* RtlAllocateMemory(BOOLEAN InZeroMemory, SIZE_T InSize)
{
	void* Result = ExAllocatePoolWithTag(NonPagedPool, InSize, 'hzw');
	if (InZeroMemory && (Result != NULL))
		RtlZeroMemory(Result, InSize);
	return Result;
}
void Deinitialize()
{
	RtlFreeMemory(FileData);
}
NTSTATUS Initialize()
{
	UNICODE_STRING FileName;
	OBJECT_ATTRIBUTES ObjectAttributes;
	RtlInitUnicodeString(&FileName, L"\\SystemRoot\\system32\\ntdll.dll");
	InitializeObjectAttributes(&ObjectAttributes, &FileName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);

	
	if (KeGetCurrentIrql() != PASSIVE_LEVEL)
	{
#ifdef _DEBUG
		DbgPrint("[DeugMessage] KeGetCurrentIrql != PASSIVE_LEVEL!\n");
#endif
		return STATUS_UNSUCCESSFUL;
	}

	HANDLE FileHandle;
	IO_STATUS_BLOCK IoStatusBlock;
	NTSTATUS NtStatus = ZwCreateFile(&FileHandle,
		GENERIC_READ,
		&ObjectAttributes,
		&IoStatusBlock, NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ,
		FILE_OPEN,
		FILE_SYNCHRONOUS_IO_NONALERT,
		NULL, 0);
	if (NT_SUCCESS(NtStatus))
	{

		FILE_STANDARD_INFORMATION StandardInformation = { 0 };
		NtStatus = ZwQueryInformationFile(FileHandle, &IoStatusBlock, &StandardInformation, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
		if (NT_SUCCESS(NtStatus))
		{
			FileSize = StandardInformation.EndOfFile.LowPart;
			DbgPrint("[DeugMessage] FileSize of ntdll.dll is %08X!\r\n", StandardInformation.EndOfFile.LowPart);
			FileData = (unsigned char*)RtlAllocateMemory(TRUE, FileSize);

			LARGE_INTEGER ByteOffset;
			ByteOffset.LowPart = ByteOffset.HighPart = 0;
			NtStatus = ZwReadFile(FileHandle,
				NULL, NULL, NULL,
				&IoStatusBlock,
				FileData,
				FileSize,
				&ByteOffset, NULL);

			if (!NT_SUCCESS(NtStatus))
			{
				RtlFreeMemory(FileData);
				DbgPrint("[DeugMessage] ZwReadFile failed with status %08X...\r\n", NtStatus);
			}
		}
		else
			DbgPrint("[DeugMessage] ZwQueryInformationFile failed with status %08X...\r\n", NtStatus);
		ZwClose(FileHandle);
	}
	else
		DbgPrint("[DeugMessage] ZwCreateFile failed with status %08X...\r\n", NtStatus);
	return NtStatus;
}


int GetExportSsdtIndex(const char* ExportName)
{
	ULONG_PTR ExportOffset = GetExportOffset(FileData, FileSize, ExportName);
	if (ExportOffset == PE_ERROR_VALUE)
		return -1;

	int SsdtOffset = -1;
	unsigned char* ExportData = FileData + ExportOffset;
	for (int i = 0; i < 32 && ExportOffset + i < FileSize; i++)
	{
		if (ExportData[i] == 0xC2 || ExportData[i] == 0xC3)  //RET
			break;
		if (ExportData[i] == 0xB8)  //mov eax,X
		{
			SsdtOffset = *(int*)(ExportData + i + 1);
			break;
		}
	}

	if (SsdtOffset == -1)
	{
		DbgPrint("[DeugMessage] SSDT Offset for %s not found...\r\n", ExportName);
	}

	return SsdtOffset;
}


PVOID GetKernelZwFuncByName(const char* ExportName)
{
	UNICODE_STRING FunctionName;
	ULONG_PTR DllIndex = (ULONG_PTR)-1;
	ULONG_PTR scale = 0;

	ULONG ZwReadFileId, ZwCreateFileId;

	RtlInitUnicodeString(&FunctionName, L"ZwReadFile");
	PVOID pTempZwReadFile = MmGetSystemRoutineAddress(&FunctionName);

	RtlInitUnicodeString(&FunctionName, L"ZwCreateFile");
	PVOID pTempZwCreateFile = MmGetSystemRoutineAddress(&FunctionName);
	if (!pTempZwCreateFile || !pTempZwReadFile) {

		//DPRINT("pTempZwCreateFile or pTempZwReadFile is zero!\r\n");
		return 0;
	}

	ZwReadFileId = GetExportSsdtIndex("ZwReadFile");
	ZwCreateFileId =GetExportSsdtIndex("ZwCreateFile");
	if (ZwCreateFileId == 0 || ZwReadFileId == 0) {
		return 0;
	}

	DllIndex = GetExportSsdtIndex(ExportName);
	if (DllIndex != (ULONG_PTR)-1)
	{

		scale = (LONG)(((LONG)((DWORD_PTR)pTempZwCreateFile - (DWORD_PTR)pTempZwReadFile)) / (ZwCreateFileId - ZwReadFileId));
		if (scale == 0) {
			return 0;
		}
		else {
			return (PVOID)((DllIndex - ZwReadFileId)*scale + (DWORD_PTR)pTempZwReadFile);
		}
	}

	return 0;
}


//structures
typedef struct _SSDTStruct_INFO
{
	LONG* pServiceTable;
	PVOID pCounterTable;
#ifdef _WIN64
	ULONGLONG NumberOfServices;
#else
	ULONG NumberOfServices;
#endif
	PCHAR pArgumentTable;
}SSDTStruct;



PVOID GetKernelBase(PULONG pImageSize)
{
	typedef struct _SYSTEM_MODULE_ENTRY
	{
		HANDLE Section;
		PVOID MappedBase;
		PVOID ImageBase;
		ULONG ImageSize;
		ULONG Flags;
		USHORT LoadOrderIndex;
		USHORT InitOrderIndex;
		USHORT LoadCount;
		USHORT OffsetToFileName;
		UCHAR FullPathName[256];
	} SYSTEM_MODULE_ENTRY, *PSYSTEM_MODULE_ENTRY;

#pragma warning(disable:4200)
	typedef struct _SYSTEM_MODULE_INFORMATION
	{
		ULONG Count;
		SYSTEM_MODULE_ENTRY Module[0];
	} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

	PVOID pModuleBase = NULL;
	PSYSTEM_MODULE_INFORMATION pSystemInfoBuffer = NULL;

	ULONG SystemInfoBufferSize = 0;

	NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation,
		&SystemInfoBufferSize,
		0,
		&SystemInfoBufferSize);

	if (!SystemInfoBufferSize)
	{
		DbgPrint("[DeugMessage] ZwQuerySystemInformation (1) failed...\r\n");
		return NULL;
	}

	pSystemInfoBuffer = (PSYSTEM_MODULE_INFORMATION)ExAllocatePool(NonPagedPool, SystemInfoBufferSize * 2);

	if (!pSystemInfoBuffer)
	{
		DbgPrint("[DeugMessage] ExAllocatePool failed...\r\n");
		return NULL;
	}

	memset(pSystemInfoBuffer, 0, SystemInfoBufferSize * 2);

	status = ZwQuerySystemInformation(SystemModuleInformation,
		pSystemInfoBuffer,
		SystemInfoBufferSize * 2,
		&SystemInfoBufferSize);

	if (NT_SUCCESS(status))
	{
		pModuleBase = pSystemInfoBuffer->Module[0].ImageBase;
		if (pImageSize)
			*pImageSize = pSystemInfoBuffer->Module[0].ImageSize;
	}
	else
		DbgPrint("[DeugMessage] ZwQuerySystemInformation (2) failed...\r\n");

	ExFreePool(pSystemInfoBuffer);

	return pModuleBase;
}




//Based on: https://github.com/hfiref0x/WinObjEx64
static SSDTStruct* SSDTfind()
{
	static SSDTStruct* SSDT = 0;
	if (!SSDT)
	{
#ifndef _WIN64
		//x86 code
		UNICODE_STRING routineName;
		RtlInitUnicodeString(&routineName, L"KeServiceDescriptorTable");
		SSDT = (SSDTStruct*)MmGetSystemRoutineAddress(&routineName);
#else
		//x64 code
		ULONG kernelSize;
		ULONG_PTR kernelBase = (ULONG_PTR)GetKernelBase(&kernelSize);
		if (kernelBase == 0 || kernelSize == 0)
			return NULL;

		// Find KiSystemServiceStart
		const unsigned char KiSystemServiceStartPattern[] = { 0x8B, 0xF8, 0xC1, 0xEF, 0x07, 0x83, 0xE7, 0x20, 0x25, 0xFF, 0x0F, 0x00, 0x00 };
		const ULONG signatureSize = sizeof(KiSystemServiceStartPattern);
		BOOLEAN found = FALSE;
		ULONG KiSSSOffset;
		for (KiSSSOffset = 0; KiSSSOffset < kernelSize - signatureSize; KiSSSOffset++)
		{
			if (RtlCompareMemory(((unsigned char*)kernelBase + KiSSSOffset), KiSystemServiceStartPattern, signatureSize) == signatureSize)
			{
				found = TRUE;
				break;
			}
		}
		if (!found)
			return NULL;

		// lea r10, KeServiceDescriptorTable
		ULONG_PTR address = kernelBase + KiSSSOffset + signatureSize;
		LONG relativeOffset = 0;
		if ((*(unsigned char*)address == 0x4c) &&
			(*(unsigned char*)(address + 1) == 0x8d) &&
			(*(unsigned char*)(address + 2) == 0x15))
		{
			relativeOffset = *(LONG*)(address + 3);
		}
		if (relativeOffset == 0)
			return NULL;

		SSDT = (SSDTStruct*)(address + relativeOffset + 7);
#endif
	}
	return SSDT;
}



PVOID GetFunctionAddress(const char* apiname)
{
	//read address from SSDT
	SSDTStruct* SSDT = SSDTfind();
	if (!SSDT)
	{
		DbgPrint("[DeugMessage] SSDT not found...\r\n");
		return 0;
	}
	ULONG_PTR SSDTbase = (ULONG_PTR)SSDT->pServiceTable;
	if (!SSDTbase)
	{
		DbgPrint("[DeugMessage] ServiceTable not found...\r\n");
		return 0;
	}
	ULONG readOffset = GetExportSsdtIndex(apiname);
	if (readOffset == -1)
		return 0;
	if (readOffset >= SSDT->NumberOfServices)
	{
		DbgPrint("[DeugMessage] Invalid read offset...\r\n");
		return 0;
	}
#ifdef _WIN64
	return (PVOID)((SSDT->pServiceTable[readOffset] >> 4) + SSDTbase);
#else
	return (PVOID)SSDT->pServiceTable[readOffset];
#endif
}




PVOID GetSSDTFucnByNTDLL_Name(const char* apiname)
{
	return GetFunctionAddress(apiname);
}