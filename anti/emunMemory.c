#include "emunMemory.h"
#include "vad.h"
#include "CommFunction.h"
#include "exapi.h"
#include "emunKernelModule.h"
#include "peb.h"
#include "emunProcess.h"
NTSTATUS GetMemorys(HANDLE dwPid, PALL_MEMORYS pMemorys, ULONG nCnt)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	ULONG_PTR Base = 0;
	PEPROCESS pTemp = NULL;
	KAPC_STATE kpc = { 0 };
	status = PsLookupProcessByProcessId(dwPid, &pTemp);

	if (NT_SUCCESS(status))
	{

		while (Base < (ULONG_PTR)MM_HIGHEST_USER_ADDRESS)
		{
			MEMORY_BASIC_INFORMATION mbi = { 0 };
			size_t ret = 0;

			__try {
				KeStackAttachProcess(pTemp, &kpc);
				status = ZwQueryVirtualMemory(NtCurrentProcess(),
					(PVOID)Base,
					MemoryBasicInformation,
					&mbi,
					sizeof(MEMORY_BASIC_INFORMATION),
					&ret);
			}
			__finally {
				KeUnstackDetachProcess(&kpc);
			}

			if (NT_SUCCESS(status))
			{
				PMMVAD pMmvad = NULL;
				ULONG nCurCnt = pMemorys->nCnt;
				if (nCnt > pMemorys->nCnt)
				{
					pMemorys->Memorys[nCurCnt].Base = Base;
					pMemorys->Memorys[nCurCnt].Size = mbi.RegionSize;
					pMemorys->Memorys[nCurCnt].Protect = mbi.Protect;
					pMemorys->Memorys[nCurCnt].State = mbi.State;
					pMemorys->Memorys[nCurCnt].Type = mbi.Type;

					__try {
						pMmvad = MiLocateAddress(pTemp, (PVOID)Base);
					}
					__except (1) 
					{
						pMmvad = NULL;

					}
					if (MmIsAddressValid(pMmvad) && pMmvad)
					{
						__try {
							pMemorys->Memorys[nCurCnt].VadFlags = pMmvad->Core.u1.Flags.flag;
						}
						__except (1)
						{
							pMemorys->Memorys[nCurCnt].VadFlags = 0;
						}

						if (MmIsAddressValid(pMmvad->Subsection) && MmIsAddressValid(pMmvad->Subsection->ControlArea))
						{
							if (MmIsAddressValid((PVOID)((pMmvad->Subsection->ControlArea->FilePointer.Value >> 4) << 4)))
							{
								__try {
									pMemorys->Memorys[nCurCnt].pFileObject = ((pMmvad->Subsection->ControlArea->FilePointer.Value >> 4) << 4);
								}
								__except (1)
								{
									pMemorys->Memorys[nCurCnt].pFileObject = 0;
								}
							}
						}

					}
					

					pMemorys->nCnt++;

				}
				Base += mbi.RegionSize;

			}
			else
			{
				Base += PAGE_SIZE;
			}

			if (nCnt < pMemorys->nCnt) {
				break;
			}

		}

		ObDereferenceObject(pTemp);
	}

	return status;
}




NTSTATUS SafeCopyProcessModules(PEPROCESS pEprocess, ULONG_PTR nBase, ULONG nSize, PVOID pOutBuffer)
{
	BOOLEAN bAttach = FALSE;
	KAPC_STATE ks = {0};
	PVOID pBuffer = NULL;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	if (nSize == 0 || pOutBuffer == NULL || pEprocess == NULL)
	{
		return status;
	}



	pBuffer = ExAllocatePoolWithTag(PagedPool, nSize,'hzw');
	if (!pBuffer)
	{
		return status;
	}

	memset(pBuffer, 0, nSize);

	if (pEprocess != IoGetCurrentProcess())
	{
		KeStackAttachProcess(pEprocess, &ks);
		bAttach = TRUE;
	}

	status = SafeCopyMemory_R3_to_R0(nBase,(ULONG_PTR)pBuffer, nSize);

	if (bAttach)
	{
		KeUnstackDetachProcess(&ks);
		bAttach = FALSE;
	}

	if (NT_SUCCESS(status))
	{
		status = SafeCopyMemory_R0_to_R3(pBuffer, pOutBuffer, nSize);
	}

	if (pBuffer)
	{
		ExFreePoolWithTag(pBuffer,'hzw');
		pBuffer = NULL;
	}

	return status;
}





NTSTATUS HideModule(ULONG dwPid,ULONG_PTR dwModuleBase,ULONG_PTR dwLdrpHashTable,ULONG nSize)
{
	
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	PEPROCESS pCurProcess = NULL;
	PVOID pTempBuffer = NULL;
	BOOLEAN bAttach = FALSE;
	KAPC_STATE ks = { 0 };
	BOOLEAN isCopy = FALSE;
	BOOLEAN bSuspendProcess = FALSE;
	pTempBuffer = RtlAllocateMemory(TRUE, nSize);
	if (pTempBuffer)
	{
	
		ntStatus = PsLookupProcessByProcessId(dwPid, &pCurProcess);
		if (NT_SUCCESS(ntStatus))
		{

			__try {
				if (pCurProcess != IoGetCurrentProcess())
				{
					KeStackAttachProcess(pCurProcess, &ks);
					bAttach = TRUE;
				}

				ntStatus = SafeCopyMemory_R3_to_R0(dwModuleBase, (ULONG_PTR)pTempBuffer, nSize);
				if (NT_SUCCESS(ntStatus) && pTempBuffer)
				{
					ntStatus = PsSuspendProcess(pCurProcess);
					if (!NT_SUCCESS(ntStatus)) {
						DbgPrint("hzw:暂停进程失败 %x\n", ntStatus);
					}
					bSuspendProcess = TRUE;
					ntStatus = MmUnmapViewOfSection(pCurProcess, dwModuleBase);
					if (NT_SUCCESS(ntStatus))
					{
						PVOID pBaseAddresss = dwModuleBase;
						SIZE_T AllocateSize =nSize;
					
						ntStatus = ZwAllocateVirtualMemory(NtCurrentProcess(), &pBaseAddresss, 0, &AllocateSize, MEM_RESERVE, PAGE_EXECUTE_READWRITE);
						if (NT_SUCCESS(ntStatus))
						{
							ntStatus = ZwAllocateVirtualMemory(NtCurrentProcess(), &pBaseAddresss, 0, &AllocateSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
							if (NT_SUCCESS(ntStatus))
							{
								
								__try
								{
								
									//抹去PE信息
									RtlFillMemory(pTempBuffer, 0x100, 0xcd);

									ntStatus = SafeCopyMemory_R0_to_R3(pTempBuffer, pBaseAddresss,AllocateSize);
						
									if (NT_SUCCESS(ntStatus)) {
										isCopy = TRUE;
									}

								}
								__except (1) {

								}
								if (isCopy)
								{
									if (PsGetProcessWow64Process(pCurProcess)) 
									{
										ULONG32 dwTemp = 0;
										 RemoveDllModuleInPeb86(pCurProcess, (ULONG)pBaseAddresss);
										 dwTemp = GetLdrpHashTable32Ex(dwLdrpHashTable);

										 if (dwTemp) {
											 RemoveDllModuleInLdrpHashTable86((LIST_ENTRY32*)(dwTemp), pCurProcess, pBaseAddresss);
										 }
									}
									else 
									{
										RemoveDllModuleInPeb64(pCurProcess, pBaseAddresss);
										RemoveDllModuleInLdrpHashTable64((LIST_ENTRY *)(dwLdrpHashTable), pCurProcess, pBaseAddresss);
									}
								
				
								}

							}

						}
						else {
							DbgPrint("hzw:申请内存失败 %x\n", ntStatus);
						}
					}
					else {
						DbgPrint("hzw:卸载模块section失败 %x\n", ntStatus);
					}
				}
				else {
					DbgPrint("hzw:拷贝模块内存失败 %x\n", ntStatus);
				}


			}
			__finally
			{
				if (bAttach)
				{
					KeUnstackDetachProcess(&ks);
					bAttach = FALSE;
				}
			}
			if (bSuspendProcess) 
			{
				ntStatus = PsResumeProcess(pCurProcess);
				if (!NT_SUCCESS(ntStatus)) {
					DbgPrint("hzw:恢复进程失败 %x\n", ntStatus);
				}
			}
			if (pTempBuffer) 
			{
				RtlFreeMemory(pTempBuffer);
				pTempBuffer = NULL;
			}
			ObDereferenceObject(pCurProcess);
			if (!isCopy) 
			{
				ntStatus = STATUS_UNSUCCESSFUL;
				DbgPrint("hzw:HideModule-->拷贝失败 %x\n");
			}
			else {
				ntStatus = STATUS_SUCCESS;
			}
		}
	}
	return ntStatus;
}



//
// 移除PEB链表中的模块
//
VOID WalkerModuleListToRemoveDllModule64(PLIST_ENTRY pList, ULONG nType, ULONG_PTR Base)
{
	PLIST_ENTRY entry = pList->Flink;


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

				if ((ULONG_PTR)pLdrEntry->DllBase == Base &&
					(ULONG_PTR)entry->Blink < SYSTEM_ADDRESS_START &&
					(ULONG_PTR)entry->Flink < SYSTEM_ADDRESS_START)
				{
					RemoveEntryList(entry);
					break;
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
NTSTATUS RemoveDllModuleInPeb64(PEPROCESS pEprocess, ULONG_PTR Base)
{
	
	NTSTATUS status = STATUS_UNSUCCESSFUL;


	if (pEprocess && KeGetCurrentIrql() < DISPATCH_LEVEL)
	{
		PPEB64 peb = PsGetProcessPeb(pEprocess);

		__try
		{
			if ((ULONG_PTR)peb > 0 && (ULONG_PTR)peb < SYSTEM_ADDRESS_START)
			{
				PPEB_LDR_DATA pLdr = NULL;

				ProbeForRead(peb, sizeof(PEB64), 1);
				ProbeForRead(peb->Ldr, sizeof(PEB_LDR_DATA), 1);

				pLdr = (PPEB_LDR_DATA)(peb->Ldr);
				if ((ULONG_PTR)pLdr > 0 && (ULONG_PTR)pLdr < SYSTEM_ADDRESS_START)
				{
					WalkerModuleListToRemoveDllModule64(&pLdr->InLoadOrderModuleList, 1, Base);
					WalkerModuleListToRemoveDllModule64(&pLdr->InMemoryOrderModuleList, 2, Base);
					WalkerModuleListToRemoveDllModule64(&pLdr->InInitializationOrderModuleList, 3, Base);

					status = STATUS_SUCCESS;
				}
			}
		}
		__except (1)
		{
			KdPrint(("hzw:EnumDllModuleByPeb catch __except\n"));
		}

	}

	return status;
}







//
// 移除PEB链表中的模块
//

BOOLEAN
RemoveEntryList86(
	_In_ PLIST_ENTRY32 Entry
)

{
	
	PLIST_ENTRY32 PrevEntry;
	PLIST_ENTRY32 NextEntry;

	NextEntry = Entry->Flink;
	PrevEntry = Entry->Blink;
	if ((NextEntry->Blink != Entry) || (PrevEntry->Flink != Entry)) 
	{
		FatalListEntryError((PVOID)PrevEntry,
			(PVOID)Entry,
			(PVOID)NextEntry);
	}

	PrevEntry->Flink = NextEntry;
	NextEntry->Blink = PrevEntry;
	return (BOOLEAN)(PrevEntry == NextEntry);
}


VOID WalkerModuleListToRemoveDllModule86(PLIST_ENTRY32 pList, ULONG nType, ULONG32 Base)
{
	PLIST_ENTRY32 entry = pList->Flink;


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

				if ((ULONG32)pLdrEntry->DllBase == Base &&
					(ULONG32)entry->Blink < SYSTEM_ADDRESS_START &&
					(ULONG32)entry->Flink < SYSTEM_ADDRESS_START)
				{
					RemoveEntryList86(entry);
					break;
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
NTSTATUS RemoveDllModuleInPeb86(PEPROCESS pEprocess, ULONG Base)
{

	NTSTATUS status = STATUS_UNSUCCESSFUL;


	if (pEprocess && KeGetCurrentIrql() < DISPATCH_LEVEL)
	{
		PPEB32 peb = PsGetProcessWow64Process(pEprocess);

		__try
		{
			if ((ULONG)peb > 0 && (ULONG)peb < SYSTEM_ADDRESS_START32)
			{
				PPEB_LDR_DATA32 pLdr = NULL;

				ProbeForRead(peb, sizeof(PEB32), 1);
				ProbeForRead(peb->Ldr, sizeof(PEB_LDR_DATA32), 1);

				pLdr = (PPEB_LDR_DATA32)(peb->Ldr);
				if ((ULONG)pLdr > 0 && (ULONG)pLdr < SYSTEM_ADDRESS_START32)
				{
					WalkerModuleListToRemoveDllModule86(&pLdr->InLoadOrderModuleList, 1, Base);
					WalkerModuleListToRemoveDllModule86(&pLdr->InMemoryOrderModuleList, 2, Base);
					WalkerModuleListToRemoveDllModule86(&pLdr->InInitializationOrderModuleList, 3, Base);

					status = STATUS_SUCCESS;
				}
			}
		}
		__except (1)
		{
			KdPrint(("hzw:EnumDllModuleByPeb catch __except\n"));
		}

	}

	return status;
}













//
// 移除LdrpHashTable链表中的进程模块项
//
NTSTATUS RemoveDllModuleInLdrpHashTable64(LIST_ENTRY * LdrpHashTable,PEPROCESS pEprocess, ULONG_PTR Base)
{



	NTSTATUS status = STATUS_UNSUCCESSFUL;


	if (!LdrpHashTable ||
		!pEprocess || !Base)
	{
		return status;
	}
	

	__try
	{
		LIST_ENTRY *pListEntry = NULL, *pListHead = NULL;
		DWORD size = sizeof(LIST_ENTRY) * 32;
		BOOL bRemove = FALSE;
		int i = 0;

		// 读Hash表
		ProbeForRead((PVOID)LdrpHashTable, size, 1);

		for (i = 0; !bRemove && i < 32; i++)
		{
			pListHead = LdrpHashTable + i;	//指向某hash节点头
			pListEntry = pListHead->Flink;

			while (pListEntry != pListHead)
			{
				
				PLDR_DATA_TABLE_ENTRY pLdrEntry = (PLDR_DATA_TABLE_ENTRY)((BYTE *)pListEntry - 0x70/*定位 PLDR_DATA_TABLE_ENTRY->HashLinks*/);

				if ((ULONG_PTR)pLdrEntry > 0 && (ULONG_PTR)pLdrEntry < SYSTEM_ADDRESS_START)
				{
					ProbeForRead(pLdrEntry, sizeof(LDR_DATA_TABLE_ENTRY), 1);

					if ((ULONG_PTR)(pLdrEntry->DllBase) == Base)
					{

						KdPrint(("hzw:find dll \n"));
						RemoveEntryList(pListEntry);
						bRemove = TRUE;
						status = STATUS_SUCCESS;
						break;
					}


				}

				pListEntry = pListEntry->Flink; // 通过节点的hashlinks来递进
			}
		}
	}
	__except (1)
	{
		KdPrint(("hzw:RemoveDllModuleInLdrpHashTable catch __except\n"));
	}


	return status;
}




NTSTATUS RemoveDllModuleInLdrpHashTable86(LIST_ENTRY32 *LdrpHashTable,PEPROCESS pEprocess, ULONG Base)
{


	NTSTATUS status = STATUS_UNSUCCESSFUL;


	if (!LdrpHashTable ||
		!pEprocess ||!Base)
	{
		return status;
	}


	__try
	{
		LIST_ENTRY32 *pListEntry = NULL, *pListHead = NULL;
		DWORD size = sizeof(LIST_ENTRY32) * 32;
		BOOL bRemove = FALSE;
		int i = 0;

		// 读Hash表
		ProbeForRead((ULONG32)LdrpHashTable, size, 1);

		for (i = 0; !bRemove && i < 32; i++)
		{
			pListHead = LdrpHashTable + i;	//指向某hash节点头
			pListEntry = pListHead->Flink;

			while (pListEntry != pListHead)
			{

				PLDR_DATA_TABLE_ENTRY32 pLdrEntry = (PLDR_DATA_TABLE_ENTRY32)((BYTE *)pListEntry - 0x3c/*定位 PLDR_DATA_TABLE_ENTRY->HashLinks*/);

				if ((ULONG32)pLdrEntry > 0 && (ULONG32)pLdrEntry < SYSTEM_ADDRESS_START32)
				{
					ProbeForRead(pLdrEntry, sizeof(LDR_DATA_TABLE_ENTRY32), 1);

					if ((ULONG32)(pLdrEntry->DllBase) == Base)
					{
						
						RemoveEntryList86(pListEntry);
						bRemove = TRUE;
						status = STATUS_SUCCESS;
						break;
					}

				}

				pListEntry = pListEntry->Flink; // 通过节点的hashlinks来递进
			}
		}
	}
	__except (1)
	{
		KdPrint(("hzw:RemoveDllModuleInLdrpHashTable catch __except\n"));
	}


	return status;
}