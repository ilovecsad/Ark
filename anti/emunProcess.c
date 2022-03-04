#include "emunProcess.h"
#include "CommFunction.h"
#include "peb.h"
#include "emunKernelModule.h"
#include <intrin.h>
#include "exapi.h"
#include <ntimage.h>

ULONG32 g_dwLdrpHashTable = 0;


BOOLEAN AddIdleProcess(PPROCESS_INFO p)
{
	PEPROCESS pIdle = NULL;
	BOOLEAN bRet = FALSE;

	char* pProcessName = NULL;

	pIdle = GetIdleProcess();

	if (pIdle) {


		p->ulEprocess = pIdle;
		p->ulCreateTime = PsGetProcessCreateTimeQuadPart(pIdle);
		p->ulPid = 0;
		pProcessName = PsGetProcessImageFileName(pIdle);
		if (pProcessName)
		{
			CharToWchar(pProcessName, p->szPath);
		}

		bRet = TRUE;
	}

	return bRet;
}

PEPROCESS GetIdleProcess()
{

	PKPCR pKpcr = NULL;
	PETHREAD pIdle = NULL;
	PEPROCESS pTemp = NULL;

	pKpcr =  (PKPCR)__readmsr(IA32_GS_BASE);
	
	__try {
		pIdle = (PETHREAD)(*(PULONG_PTR)((ULONG_PTR)pKpcr->CurrentPrcb + IdleThread_offset));
	}
	__except (1) {
		DbgPrint("hzw: GetIdleProcess %x\n", GetExceptionCode());
		return NULL;
	}

	pTemp = IoThreadToProcess(pIdle);
	
	return pTemp;
}


ULONG EnumProcess(PPROCESS_INFO pBuffer)
{
	NTSTATUS stats = STATUS_UNSUCCESSFUL;
	ULONG i = 0;
	PEPROCESS eproc = NULL;
	ULONG nCount = 0;

	for (i = 4; i < 262144; i = i + 4)

	{
		stats = PsLookupProcessByProcessId((HANDLE)i, &eproc);

		if (NT_SUCCESS(stats))
		{
			if (IsRealProcess(eproc)) 
			{

				POBJECT_NAME_INFORMATION pObjectNameInfo = NULL;
				PVOID pFilePoint = NULL;
				char* pProcessName = NULL;

				pBuffer[nCount].ulEprocess = eproc;
				pBuffer[nCount].ulPid = (HANDLE)i;
				pBuffer[nCount].ulParentPid = PsGetProcessInheritedFromUniqueProcessId(eproc);
				pBuffer[nCount].ulCreateTime = PsGetProcessCreateTimeQuadPart(eproc);


				stats = PsReferenceProcessFilePointer(eproc, &pFilePoint);
				if (NT_SUCCESS(stats))
				{
					
					stats = IoQueryFileDosDeviceNameSafeIrql(pFilePoint, &pObjectNameInfo);
					//stats = IoQueryFileDosDeviceName(pFilePoint, &pObjectNameInfo);
					if (NT_SUCCESS(stats)&& pObjectNameInfo)
					{
						__try {
							RtlCopyMemory(pBuffer[nCount].szPath, pObjectNameInfo->Name.Buffer, (pObjectNameInfo->Name.MaximumLength > MAX_PATHEX ? MAX_PATHEX : pObjectNameInfo->Name.MaximumLength));
						}
						__except (1)
						{

						}
						if (pObjectNameInfo) {
							ExFreePool(pObjectNameInfo);
						}
					
					}
					ObDereferenceObject(pFilePoint);
				}
				else {
					pProcessName = PsGetProcessImageFileName(eproc);
					if (pProcessName)
					{
						CharToWchar(pProcessName, pBuffer[nCount].szPath);
					}

				}


				nCount++;
				ObDereferenceObject(eproc);
			}
		}

		//应该不会有超过 990个进程的吧
		if (nCount > 990) {
			return 0;
		}

	}



	return nCount;
}

NTSTATUS GetCommandLineByPid(HANDLE dwPid, PVOID pBuffer, ULONG_PTR nSize)
{
	KAPC_STATE kpc = { 0 };

	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	PEPROCESS pTemp = NULL;



	ntStatus = PsLookupProcessByProcessId(dwPid, &pTemp);
	if (NT_SUCCESS(ntStatus))
	{
		PPEB64 peb = PsGetProcessPeb(pTemp);
		if (peb)
		{

			__try {
				KeStackAttachProcess(pTemp, &kpc);

				if (peb->ProcessParameters)
				{
			
					ULONG nLength = 0;
					nLength = peb->ProcessParameters->CommandLine.MaximumLength;
					RtlCopyMemory(pBuffer, peb->ProcessParameters->CommandLine.Buffer, nLength > nSize ? nSize : nLength);
				}

			}
			__finally
			{

				KeUnstackDetachProcess(&kpc);
			}
			ObDereferenceObject(pTemp);

		}
		
	}

	return ntStatus;
}

ULONG EnumProcessThreadByPid(HANDLE dwPid, PTHREAD_INFO pBuffer)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	ULONG dwCnt = 0;

	PEPROCESS eproc = NULL;
	PETHREAD pTempThread = NULL;
	status  = PsLookupProcessByProcessId(dwPid, &eproc);
	if (!NT_SUCCESS(status)){
		return dwCnt;
	}

	for (ULONG i = 4; i < 262144; i = i + 4)
	{

		status = PsLookupThreadByThreadId((HANDLE)i, &pTempThread);
		if (NT_SUCCESS(status))
		{
			if ((IoThreadToProcess(pTempThread) == eproc) && (0 != Kthread_offset_Win32StartAddress) 
				&&(0 != Kthread_offset_ContextSwitches) && (0 != Kthread_offset_state) && !PsIsThreadTerminating(pTempThread))
			{
				__try {
					pBuffer[dwCnt].pThread = (ULONG_PTR)pTempThread;
					pBuffer[dwCnt].Tid = (ULONG)PsGetThreadId(pTempThread);
					pBuffer[dwCnt].Teb = (ULONG_PTR)PsGetThreadTeb(pTempThread);
					pBuffer[dwCnt].Win32StartAddress = *(ULONG_PTR*)((ULONG_PTR)pTempThread + Kthread_offset_Win32StartAddress);
					pBuffer[dwCnt].Priority = (ULONG_PTR)KeQueryPriorityThread(pTempThread);
					pBuffer[dwCnt].ContextSwitches = *(ULONG*)((ULONG_PTR)pTempThread + Kthread_offset_ContextSwitches);
					pBuffer[dwCnt].State = *(CHAR*)((ULONG_PTR)pTempThread + Kthread_offset_state);
					pBuffer[dwCnt].ThreadFlag = PspGetThreadSuspendCount(pTempThread);
				}
				__except (1) {
					DbgPrint("hzw: EnumProcessThreadByPid %x\n", GetExceptionCode());
				}
				
	
				dwCnt++;
			}
			ObDereferenceObject(pTempThread);
			pTempThread = NULL;
		}

	}

	ObDereferenceObject(eproc);

	return dwCnt;
}

NTSTATUS EnumProcessModuleByPid(HANDLE dwPid, PALL_MODULES pBuffer,ULONG nCnt)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	ULONG dwCnt = 0;
	PEPROCESS eproc = NULL;
	
	PALL_MODULES pTemp = NULL;

	status = PsLookupProcessByProcessId(dwPid, &eproc);

	if (!NT_SUCCESS(status)) {
		return status;
	}

	pTemp = (PALL_MODULES)ExAllocatePoolWithTag(PagedPool, sizeof(ULONG)+sizeof(MODULE_INFO_EX)*nCnt, 'Mzf');
	if (!pTemp) {
		DbgPrint("hzw:allocateMemory for module failed\n");
		dwCnt = 0;
		goto _clear;
	}

	RtlZeroMemory(pTemp, sizeof(ULONG) + sizeof(MODULE_INFO_EX)*nCnt);

	KAPC_STATE kpc = { 0 };
	PPEB64 pPeb = NULL;
	PPEB32 pPeb32 = NULL;

	pPeb32 = (PPEB32)PsGetProcessWow64Process(eproc);
	

	pPeb = PsGetProcessPeb(eproc);

	if (pPeb) 
	{
		__try
		{
			KeStackAttachProcess(eproc, &kpc);

			PPEB_LDR_DATA pLdr = NULL;
			pLdr =	pPeb->Ldr;
			if (pLdr && pPeb)
			{
				ProbeForRead(pPeb, sizeof(PEB64), 1);
				ProbeForRead(pLdr, sizeof(PEB_LDR_DATA), 1);

				if ((ULONG_PTR)pLdr > 0 && (ULONG_PTR)pLdr < SYSTEM_ADDRESS_START)
				{
					WalkerModuleList(&pLdr->InLoadOrderModuleList, 1, pTemp, nCnt);
					WalkerModuleList(&pLdr->InMemoryOrderModuleList, 2, pTemp, nCnt);
					WalkerModuleList(&pLdr->InInitializationOrderModuleList, 3, pTemp, nCnt);

				}
			}

			if (pPeb32)
			{

				PPEB_LDR_DATA32 pLdr32 = NULL;
				pLdr32 = (PPEB_LDR_DATA32)pPeb32->Ldr;
				ProbeForRead(pPeb32, sizeof(PEB32), 1);
				ProbeForRead(pLdr32, sizeof(PEB_LDR_DATA32), 1);

				if (pLdr32)
				{
					WalkerModuleList32(&pLdr32->InLoadOrderModuleList, 1, pTemp, nCnt);
					WalkerModuleList32(&pLdr32->InMemoryOrderModuleList, 2, pTemp, nCnt);
					WalkerModuleList32(&pLdr32->InInitializationOrderModuleList, 3, pTemp, nCnt);

				}

			}
			


		}
		__finally
		{
			KeUnstackDetachProcess(&kpc);
		}
		if (pTemp->nCnt > 0)
		{
			RtlCopyMemory(pBuffer, pTemp, sizeof(ULONG) + sizeof(MODULE_INFO_EX)*nCnt);
			status = STATUS_SUCCESS;
		}
		else 
		{
			status = STATUS_UNSUCCESSFUL;
		}

	}
	else
	{
		status = STATUS_UNSUCCESSFUL;
	}




	ExFreePoolWithTag(pTemp, 'Mzf');

_clear:
	ObDereferenceObject(eproc);


	return status;
}
	


BOOLEAN IsModuleInList(ULONG_PTR Base, ULONG Size, PALL_MODULES pPmi, ULONG nCnt)
{
	BOOLEAN bIn = FALSE;
	ULONG i = 0;
	ULONG nTempCnt = pPmi->nCnt > nCnt ? nCnt : pPmi->nCnt;

	for (i = 0; i < nTempCnt; i++)
	{
		if (Base == pPmi->Modules[i].Base && Size == pPmi->Modules[i].Size)
		{
			bIn = TRUE;
			break;
		}
	}

	return bIn;
}

BOOLEAN IsProcessDie(PEPROCESS pEprocess)
{
	BOOLEAN bDie = FALSE;
	ULONG_PTR ObjectTableOffset = enumObjectTableOffset_EPROCESS;


	if (pEprocess &&MmIsAddressValid(pEprocess) &&MmIsAddressValid((PVOID)((ULONG_PTR)pEprocess + ObjectTableOffset)))
	{
		PVOID pObjectTable = *(PVOID*)((ULONG_PTR)pEprocess + ObjectTableOffset);

		if (!pObjectTable || !MmIsAddressValid(pObjectTable))
		{
			KdPrint(("hzw:process is die\n"));
			bDie = TRUE;
		}
	}
	else
	{
		bDie = TRUE;
	}

	return bDie;
}





VOID WalkerModuleList(PLIST_ENTRY pList, ULONG nType, PALL_MODULES pPmi, ULONG nCnt)
{
	PLIST_ENTRY entry = NULL;

	if (!pList || !pPmi)
	{
		return;
	}

	entry = pList->Flink;

	while ((ULONG_PTR)entry > 0 && (ULONG_PTR)entry < SYSTEM_ADDRESS_START && entry != pList)
	{
		PLDR_DATA_TABLE_ENTRY pLdrEntry = NULL;

		switch (nType)
		{
		case 1:
			pLdrEntry = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
			break;

		case 2:
			pLdrEntry = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
			break;

		case 3:
			pLdrEntry = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY, InInitializationOrderLinks);
			break;
		}

		if ((ULONG_PTR)pLdrEntry > 0 && (ULONG_PTR)pLdrEntry < SYSTEM_ADDRESS_START)
		{
			__try
			{
				ProbeForRead(pLdrEntry, sizeof(LDR_DATA_TABLE_ENTRY), 1);

				if (!IsModuleInList((ULONG_PTR)pLdrEntry->DllBase, pLdrEntry->SizeOfImage, pPmi, nCnt))
				{
					if (nCnt > pPmi->nCnt)
					{

						pPmi->Modules[pPmi->nCnt].Base = (ULONG_PTR)pLdrEntry->DllBase;
						pPmi->Modules[pPmi->nCnt].Size = pLdrEntry->SizeOfImage;
						pPmi->Modules[pPmi->nCnt].Sign = 0x64;
						ProbeForRead(pLdrEntry->FullDllName.Buffer, pLdrEntry->FullDllName.MaximumLength, 1);

						wcsncpy(pPmi->Modules[pPmi->nCnt].Path, pLdrEntry->FullDllName.Buffer, pLdrEntry->FullDllName.MaximumLength > MAX_PATH ? MAX_PATH : pLdrEntry->FullDllName.MaximumLength);
					}
				
					pPmi->nCnt++;
				}
			}
			__except (1)
			{
				KdPrint(("hzw:WalkerModuleList __except(1)\n"));
			}
		}

		entry = entry->Flink;
	}
}




void WalkerModuleList32(PLIST_ENTRY32 pList, ULONG nType, PALL_MODULES pPmi, ULONG nCnt)
{
	PLIST_ENTRY32 entry = NULL;
	

	if (!pList || !pPmi)
	{
		return;
	}

	entry = (PLIST_ENTRY32)pList->Flink;

	while ((ULONG32)entry > 0 && (ULONG32)entry < SYSTEM_ADDRESS_START32 && entry != pList)
	{
		PLDR_DATA_TABLE_ENTRY32 pLdrEntry = NULL;

		switch (nType)
		{
		case 1:
			pLdrEntry = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY32, InLoadOrderLinks);
			break;

		case 2:
			pLdrEntry = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY32, InMemoryOrderModuleList);
			break;

		case 3:
			pLdrEntry = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY32, InInitializationOrderModuleList);
			break;
		}

		if ((ULONG32)pLdrEntry > 0 && (ULONG32)pLdrEntry < SYSTEM_ADDRESS_START32)
		{
			__try
			{
				ProbeForRead(pLdrEntry, sizeof(LDR_DATA_TABLE_ENTRY32), 1);

				if (!IsModuleInList((ULONG_PTR)pLdrEntry->DllBase, pLdrEntry->SizeOfImage, pPmi, nCnt))
				{
					if (nCnt > pPmi->nCnt)
					{
						pPmi->Modules[pPmi->nCnt].Base = (ULONG_PTR)pLdrEntry->DllBase;
						pPmi->Modules[pPmi->nCnt].Size = pLdrEntry->SizeOfImage;
						pPmi->Modules[pPmi->nCnt].Sign = 0x32;
						ProbeForRead((PVOID)pLdrEntry->FullDllName.Buffer, pLdrEntry->FullDllName.MaximumLength, 1);
						wcsncpy(pPmi->Modules[pPmi->nCnt].Path, (PVOID)pLdrEntry->FullDllName.Buffer, pLdrEntry->FullDllName.MaximumLength > MAX_PATH ? MAX_PATH : pLdrEntry->FullDllName.MaximumLength);

					}
					
					pPmi->nCnt++;
				}
			}
			__except (1)
			{
				KdPrint(("hze:WalkerModuleList32 __except(1)\n"));
			}
		}

		entry = entry->Flink;
	}
}


BOOLEAN IsRealProcess(PEPROCESS pEprocess)
{
	POBJECT_TYPE pObjectType;
	BOOLEAN bRet = FALSE;
	
	
	if (MmIsAddressValid((PVOID)(pEprocess)))
	{
		pObjectType = (POBJECT_TYPE)ObGetObjectType((PVOID)pEprocess);
		if (pObjectType &&
			*PsProcessType == pObjectType &&
			!IsProcessDie(pEprocess))
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

UCHAR PspGetThreadSuspendCount(PETHREAD pThread)
{
	UCHAR szTemp = 0;

	if (Kthread_offset_SuspendCount)
	{
		if (MmIsAddressValid((PVOID)((ULONG_PTR)pThread + Kthread_offset_SuspendCount)))
		{
			__try 
			{
				
				szTemp = *(UCHAR*)((ULONG_PTR)pThread + Kthread_offset_SuspendCount);
				
			}
			__except (1) {
				DbgPrint("hzw:PspGetThreadSuspendCount failed\n");
			}
		}

	}

	return szTemp;
}


#define IMAGE_FIRST_SECTION32( ntheader ) ((PIMAGE_SECTION_HEADER)        \
    ((ULONG32)(ntheader) +                                            \
     FIELD_OFFSET( IMAGE_NT_HEADERS32, OptionalHeader ) +                 \
     ((ntheader))->FileHeader.SizeOfOptionalHeader   \
    ))
ULONG32 GetLdrpHashTable32(ULONG32 pKernel32_BaseThreadInitThunk,ULONG32 hModule)
{
	if (!pKernel32_BaseThreadInitThunk || !hModule) {
		return 0;
	}

	ULONG32 i = 0;
	ULONG32 LdrpHashTable = 0;
	PIMAGE_DOS_HEADER DosHeader = NULL;
	PIMAGE_NT_HEADERS32 NtHeader = NULL;
	PIMAGE_SECTION_HEADER SectionHeader = NULL;
	ULONG32* dwSearchStart = 0;
	ULONG32* dwSearchEnd = 0;

	//hashtable是保存在ntdll这个模块的.data的一个地址的偏移


	__try 
	{
		ProbeForRead((PULONG32)hModule, sizeof(IMAGE_DOS_HEADER), 1);
		//得到ntdll的pe文件头结构
		DosHeader = (PIMAGE_DOS_HEADER)hModule;
		if (DosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
			return 0;
		}

		ProbeForRead((PULONG32)((PUCHAR)hModule + DosHeader->e_lfanew), sizeof(IMAGE_NT_HEADERS32), 1);
		NtHeader = (PIMAGE_NT_HEADERS32)((PUCHAR)hModule + DosHeader->e_lfanew);
		if (NtHeader->Signature != IMAGE_NT_SIGNATURE) {
			return 0;
		}

		SectionHeader = IMAGE_FIRST_SECTION32(NtHeader);

		//
		// Retrieve .data section
		// 
		//开始搜索section
		for (i = 0; i < NtHeader->FileHeader.NumberOfSections; i++)
		{
			//printf("Section : %s\n", SectionHeader[i].Name);
			if (*(PULONG32)&SectionHeader[i].Name == 'tad.') //.dat
			{
				dwSearchStart = (PULONG32)((ULONG32)hModule + SectionHeader[i].VirtualAddress);
				dwSearchEnd = (PULONG32)((ULONG32)dwSearchStart + SectionHeader[i].Misc.VirtualSize);
				break;
			}
		}

		ProbeForRead(dwSearchStart, dwSearchEnd - dwSearchStart, 1);

		PULONG32 pSearchCurrent = NULL;

		for (pSearchCurrent = dwSearchStart; pSearchCurrent < dwSearchEnd; pSearchCurrent++)
		{
			if (*pSearchCurrent == pKernel32_BaseThreadInitThunk)
			{
				PULONG32 pSearchCurrent2 = NULL;
				for (pSearchCurrent2 = pSearchCurrent; pSearchCurrent2 < dwSearchEnd; pSearchCurrent2++) {

					//据观测 LdrpHashTable 连续保存32 LIST_ENTRY  如果得不到你就继续增加判断
					if (*pSearchCurrent2 != 0 && *(pSearchCurrent2 + 1) != 0 && *(pSearchCurrent2 + 2) != 0 &&
						*(pSearchCurrent2 + 3) != 0 && *(pSearchCurrent2 + 4) != 0 && *(pSearchCurrent2 + 5) != 0 && *(pSearchCurrent2 + 6) != 0 &&
						*(pSearchCurrent2 + 7) != 0 && *(pSearchCurrent2 + 8) != 0 && *(pSearchCurrent2 + 9) != 0 && *(pSearchCurrent2 + 10) != 0 &&
						*(pSearchCurrent2 + 11) != 0 && *(pSearchCurrent2 + 12) != 0 && *(pSearchCurrent2 + 13) != 0 && *(pSearchCurrent2 + 14) != 0 &&
						*(pSearchCurrent2 + 15) != 0 && *(pSearchCurrent2 + 16) != 0 && *(pSearchCurrent2 + 17) != 0 && *(pSearchCurrent2 + 18) != 0 &&
						*(pSearchCurrent2 + 19) != 0 && *(pSearchCurrent2 + 20) != 0 && *(pSearchCurrent2 + 21) != 0 && *(pSearchCurrent2 + 22) != 0)
					{
						LdrpHashTable = pSearchCurrent2;
						break;
					}
				}
				break;
			}
		}
	}
	__except (1) 
	{
		LdrpHashTable = 0;
	}


	return LdrpHashTable;
}




ULONG_PTR GetProcAddress( PVOID BaseAddress, char* lpFunctionName)
{
	
	PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)BaseAddress;
	PIMAGE_NT_HEADERS32 pNtHdr32 = NULL;
	PIMAGE_NT_HEADERS64 pNtHdr64 = NULL;
	PIMAGE_EXPORT_DIRECTORY pExport = NULL;
	ULONG expSize = 0;
	ULONG_PTR pAddress = 0;
	PUSHORT pAddressOfOrds;
	PULONG  pAddressOfNames;
	PULONG  pAddressOfFuncs;
	ULONG i;

	ASSERT(BaseAddress != NULL);
	if (BaseAddress == NULL)
		return 0;

	/// Not a PE file
	if (pDosHdr->e_magic != IMAGE_DOS_SIGNATURE)
		return 0;

	pNtHdr32 = (PIMAGE_NT_HEADERS32)((PUCHAR)BaseAddress + pDosHdr->e_lfanew);
	pNtHdr64 = (PIMAGE_NT_HEADERS64)((PUCHAR)BaseAddress + pDosHdr->e_lfanew);

	// Not a PE file
	if (pNtHdr32->Signature != IMAGE_NT_SIGNATURE)
		return 0;

	// 64 bit image
	if (pNtHdr32->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
		pExport = (PIMAGE_EXPORT_DIRECTORY)(pNtHdr64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + (ULONG_PTR)BaseAddress);
		expSize = pNtHdr64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
	}
	// 32 bit image
	else
	{
		pExport = (PIMAGE_EXPORT_DIRECTORY)(pNtHdr32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + (ULONG_PTR)BaseAddress);
		expSize = pNtHdr32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
	}

	pAddressOfOrds = (PUSHORT)(pExport->AddressOfNameOrdinals + (ULONG_PTR)BaseAddress);
	pAddressOfNames = (PULONG)(pExport->AddressOfNames + (ULONG_PTR)BaseAddress);
	pAddressOfFuncs = (PULONG)(pExport->AddressOfFunctions + (ULONG_PTR)BaseAddress);

	for (i = 0; i < pExport->NumberOfFunctions; ++i)
	{
		USHORT OrdIndex = 0xFFFF;
		PCHAR  pName = NULL;

		// Find by index
		if ((ULONG_PTR)lpFunctionName <= 0xFFFF)
		{
			OrdIndex = (USHORT)i;
		}
		// Find by name
		else if ((ULONG_PTR)lpFunctionName > 0xFFFF && i < pExport->NumberOfNames)
		{
			pName = (PCHAR)(pAddressOfNames[i] + (ULONG_PTR)BaseAddress);
			OrdIndex = pAddressOfOrds[i];
		}
		// Weird params
		else
			return 0;

		if (((ULONG_PTR)lpFunctionName <= 0xFFFF && (USHORT)((ULONG_PTR)lpFunctionName) == OrdIndex + pExport->Base) ||
			((ULONG_PTR)lpFunctionName > 0xFFFF && strcmp(pName, (PTSTR)(PCTSTR)lpFunctionName) == 0))
		{
			pAddress = pAddressOfFuncs[OrdIndex] + (ULONG_PTR)BaseAddress;

			// Check forwarded export
			if (pAddress >= (ULONG_PTR)pExport && pAddress <= (ULONG_PTR)pExport + expSize)
			{
				return 0;
			}

			break;
		}
	}
	return (ULONG_PTR)pAddress;
}

ULONG32 GetLdrpHashTable32Ex(ULONG_PTR a)
{
	if (!a)return 0;

	ULONG32 dwLdrpHashTable = 0;
	typedef struct _DLL_BASE_
	{
		ULONG32 ntdll_base;
		ULONG32 kernel32_base;

	}DLL_BASE;

	DLL_BASE dwDllBase = { 0 };
	RtlCopyMemory(&dwDllBase, &a, sizeof(ULONG_PTR));
	
	__try {
		ULONG32  dwBaseThreadInitThunk = (ULONG32)GetProcAddress((PVOID)dwDllBase.kernel32_base, "BaseThreadInitThunk");
		if (dwBaseThreadInitThunk) {
			
			dwLdrpHashTable = GetLdrpHashTable32(dwBaseThreadInitThunk, dwDllBase.ntdll_base);

		}
	}
	__except (1)
	{
		dwLdrpHashTable = 0;
	}

	return dwLdrpHashTable;
}
