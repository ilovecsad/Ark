#include "emunHands.h"
#include "CommFunction.h"
#include "emunProcess.h"
#include "exapi.h"

PHANDLE_TABLE_ENTRY ExpLookupHandleTableEntry(
	IN PHANDLE_TABLE HandleTable,
	IN EXHANDLE tHandle
)
{
	ULONG_PTR i, j, k;
	ULONG_PTR CapturedTable;
	ULONG TableLevel;
	PHANDLE_TABLE_ENTRY Entry = NULL;
	EXHANDLE Handle;

	PUCHAR TableLevel1;
	PUCHAR TableLevel2;
	PUCHAR TableLevel3;

	ULONG_PTR MaxHandle;

	PAGED_CODE();

	Handle = tHandle;
	Handle.TagBits = 0;

	MaxHandle = *(volatile ULONG *)&HandleTable->NextHandleNeedingPool;
	if (Handle.Value >= MaxHandle)
	{
		return NULL;
	}

	CapturedTable = *(volatile ULONG_PTR *)&HandleTable->TableCode;
	TableLevel = (ULONG)(CapturedTable & LEVEL_CODE_MASK);
	CapturedTable = CapturedTable - TableLevel;

	switch (TableLevel)
	{
	case 0:
	{
		TableLevel1 = (PUCHAR)CapturedTable;

		Entry = (PHANDLE_TABLE_ENTRY)&TableLevel1[Handle.Value *
			(sizeof(HANDLE_TABLE_ENTRY) / HANDLE_VALUE_INC)];

		break;
	}

	case 1:
	{
		TableLevel2 = (PUCHAR)CapturedTable;

		i = Handle.Value % (LOWLEVEL_COUNT * HANDLE_VALUE_INC);
		Handle.Value -= i;
		j = Handle.Value / ((LOWLEVEL_COUNT * HANDLE_VALUE_INC) / sizeof(PHANDLE_TABLE_ENTRY));

		TableLevel1 = (PUCHAR)*(PHANDLE_TABLE_ENTRY*)&TableLevel2[j];
		Entry = (PHANDLE_TABLE_ENTRY)&TableLevel1[i * (sizeof(HANDLE_TABLE_ENTRY) / HANDLE_VALUE_INC)];

		break;
	}

	case 2:
	{
		TableLevel3 = (PUCHAR)CapturedTable;

		i = Handle.Value % (LOWLEVEL_COUNT * HANDLE_VALUE_INC);
		Handle.Value -= i;
		k = Handle.Value / ((LOWLEVEL_COUNT * HANDLE_VALUE_INC) / sizeof(PHANDLE_TABLE_ENTRY));
		j = k % (MIDLEVEL_COUNT * sizeof(PHANDLE_TABLE_ENTRY));
		k -= j;
		k /= MIDLEVEL_COUNT;

		TableLevel2 = (PUCHAR)*(PHANDLE_TABLE_ENTRY*)&TableLevel3[k];
		TableLevel1 = (PUCHAR)*(PHANDLE_TABLE_ENTRY*)&TableLevel2[j];
		Entry = (PHANDLE_TABLE_ENTRY)&TableLevel1[i * (sizeof(HANDLE_TABLE_ENTRY) / HANDLE_VALUE_INC)];

		break;
	}

	default: _assume(0);
	}

	return Entry;
}




NTSTATUS EnumHandsByAPI(HANDLE dwPid, PALL_HANDLES pBuffer, ULONG nCnt)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	PEPROCESS EProcess = NULL;
	ULONG nRetLength = 0;

	SYSTEM_HANDLE_INFORMATION dwInfo = { 0 };

	if (!NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)dwPid, &EProcess)))
	{
		return Status;
	}


	Status = ZwQuerySystemInformation(SystemHandleInformation, &dwInfo, sizeof(SYSTEM_HANDLE_INFORMATION), &nRetLength);
	if (STATUS_INFO_LENGTH_MISMATCH == Status && nRetLength>0)
	{
		PSYSTEM_HANDLE_INFORMATION pHandles = NULL;

		nRetLength += PAGE_SIZE;
		pHandles = ExAllocatePool(NonPagedPool, nRetLength);
		if (pHandles)
		{
			RtlZeroMemory(pHandles, nRetLength);

			Status = ZwQuerySystemInformation(SystemHandleInformation, pHandles, nRetLength, &nRetLength);
			if (NT_SUCCESS(Status))
			{
				ULONG i = 0;
				for (i = 0; i < pHandles->NumberOfHandles; i++)
				{
				
					if (dwPid == (HANDLE)pHandles->Handles[i].ProcessId)
					{
						if (nCnt > pBuffer->nCnt)
						{
							InsertHandleToList(EProcess, &pHandles->Handles[i], pBuffer);
						}

						pBuffer->nCnt++;
					}
				}
			}


			ExFreePool(pHandles);
		}

	}

	ObDereferenceObject(EProcess);

	return Status;
}



NTSTATUS EnumHands(HANDLE dwPid,PALL_HANDLES pBuffer,ULONG nCnt)
{

	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	PEPROCESS EProcess = NULL;

	PHANDLE_TABLE handleTable = NULL;
	UNICODE_STRING strProcess = RTL_CONSTANT_STRING(L"Process");
	UNICODE_STRING strThread = RTL_CONSTANT_STRING(L"Thread");

	if (!NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)dwPid, &EProcess)))
	{
		return Status;
	}

	handleTable = *(PHANDLE_TABLE*)((PUCHAR)EProcess + enumObjectTableOffset_EPROCESS);

	if (MmIsAddressValid(handleTable))
	{

		ULONG_PTR Handle = 0;
		PHANDLE_TABLE_ENTRY Entry = NULL;
		PVOID Object = NULL;
		PMY_OBJECT_TYPE ObjectType = NULL;

		for (Handle = 0;; Handle += HANDLE_VALUE_INC)
		{

			Entry = ExpLookupHandleTableEntry(handleTable, *(PEXHANDLE)&Handle);
			if (Entry == NULL || !MmIsAddressValid(Entry))
			{
				break;
			}

			*(ULONG_PTR*)&Object = Entry->ObjectPointerBits;
			*(ULONG_PTR*)&Object <<= 4;
			if (Object == NULL)
			{
				continue;
			}

			*(ULONG_PTR*)&Object |= 0xFFFF000000000000;
			*(ULONG_PTR*)&Object += 0x30;
			ObjectType = ObGetObjectType(Object);
			if (ObjectType == NULL)
			{
				continue;
			}

			//////////////////////////////////////////////////////////////////////////////////////////////
			// 干活
			if ((nCnt > pBuffer->nCnt) && MmIsAddressValid(ObjectType) && ExGetPreviousMode() == UserMode)
			{
			
				__try {
					GetNameByObjectType(ObjectType, Object, &pBuffer->Handles[pBuffer->nCnt].u, 300);
					pBuffer->Handles[pBuffer->nCnt].Object = (ULONG_PTR)Object;
					pBuffer->Handles[pBuffer->nCnt].Handle = (ULONG)Handle;
					pBuffer->Handles[pBuffer->nCnt].grantAcess = Entry->GrantedAccessBits;
					pBuffer->Handles[pBuffer->nCnt].ReferenceCount = ObjectType->TotalNumberOfHandles;

					if (MmIsAddressValid(&ObjectType->Name) && MmIsAddressValid(ObjectType->Name.Buffer) && ObjectType->Name.Length > 0) {
						RtlCopyMemory(&pBuffer->Handles[pBuffer->nCnt].ObjectName, ObjectType->Name.Buffer,
							ObjectType->Name.MaximumLength > 50 ? 50 : ObjectType->Name.MaximumLength);
					}

					if (RtlEqualUnicodeString(&ObjectType->Name, &strProcess, TRUE))
					{
						pBuffer->Handles[pBuffer->nCnt].Id = (ULONG)PsGetProcessId(Object);
					}
					else if (RtlEqualUnicodeString(&ObjectType->Name, &strThread, TRUE))
					{
						pBuffer->Handles[pBuffer->nCnt].Id = (ULONG)PsGetThreadId(Object);
					}

					pBuffer->nCnt++;
				}
				__except (1) 
				{
					break;
				}
			}

			if (pBuffer->nCnt > nCnt) {
				break;
			}
			//////////////////////////////////////////////////////////////////////////////////////////////
		}

	}

	ObDereferenceObject(EProcess);
	Status = STATUS_SUCCESS;

	return Status;
}
VOID InsertHandleToList(PEPROCESS pEprocess, PSYSTEM_HANDLE_TABLE_ENTRY_INFO p, PALL_HANDLES pHandles)
{
	if (!MmIsAddressValid(pEprocess) || !MmIsAddressValid(p) || !pHandles) {
		return;
	}
	UNICODE_STRING strProcess = RTL_CONSTANT_STRING(L"Process");
	UNICODE_STRING strThread = RTL_CONSTANT_STRING(L"Thread");
	PMY_OBJECT_TYPE ObjectType = NULL;
	ObjectType = ObGetObjectType(p->Object);
	if (!ObjectType)return;

	if (ExGetPreviousMode() == UserMode && MmIsAddressValid(ObjectType)) {
		__try
		{
			if (MmIsAddressValid(&ObjectType->Name) && MmIsAddressValid(ObjectType->Name.Buffer) && ObjectType->Name.Length > 0) {
				// 经验一 ObGetObjectType 出来的ObjectType  访问 很容易蓝屏
				RtlCopyMemory(&pHandles->Handles[pHandles->nCnt].ObjectName, ObjectType->Name.Buffer, ObjectType->Name.MaximumLength > 50 ? 50 : ObjectType->Name.MaximumLength);
			}
			pHandles->Handles[pHandles->nCnt].Object = (ULONG_PTR)p->Object;
			pHandles->Handles[pHandles->nCnt].Handle = p->HandleValue;
			pHandles->Handles[pHandles->nCnt].Handle = (ULONG)p->HandleValue;
			pHandles->Handles[pHandles->nCnt].grantAcess = p->GrantedAccess;
			pHandles->Handles[pHandles->nCnt].ReferenceCount = ObjectType->TotalNumberOfHandles;
			GetNameByObjectType(ObjectType, p->Object, &pHandles->Handles[pHandles->nCnt].u, 300);

			if (RtlEqualUnicodeString(&ObjectType->Name, &strProcess, TRUE))
			{
				pHandles->Handles[pHandles->nCnt].Id = (ULONG)PsGetProcessId(p->Object);
			}
			else if (RtlEqualUnicodeString(&ObjectType->Name, &strThread, TRUE))
			{
				pHandles->Handles[pHandles->nCnt].Id = (ULONG)PsGetThreadId(p->Object);
			}
		}
		__except (1)
		{

		}
	}
}





VOID GetNameByObjectType(PMY_OBJECT_TYPE pObjectType,PVOID pObject,IN PVOID pBuffer, ULONG nSize)
{
	if (!MmIsAddressValid(pObjectType) || !MmIsAddressValid(pObject))return;

	UNICODE_STRING strProcess = RTL_CONSTANT_STRING(L"Process");
	UNICODE_STRING strThread = RTL_CONSTANT_STRING(L"Thread");
	UNICODE_STRING strFile = RTL_CONSTANT_STRING(L"File");
	UNICODE_STRING strToken = RTL_CONSTANT_STRING(L"Token"); //dt _token
	UNICODE_STRING strSection = RTL_CONSTANT_STRING(L"Section");//dt _section

	/*
Pool tag	Object type
TmTx	_KTRANSACTION
TmRm	_KRESOURCEMANAGER
TmEn	_KENLISTMENT
TmTm	_KTM
	*/
	UNICODE_STRING strSTmTm = RTL_CONSTANT_STRING(L"TmTm");//TmTm https://bbs.pediy.com/thread-259664.htm
	UNICODE_STRING strTmRm = RTL_CONSTANT_STRING(L"TmRm");//dt _KRESOURCEMANAGER->RmId  https://blog.csdn.net/xiangbaohui/article/details/109476238
	if (RtlEqualUnicodeString(&pObjectType->Name, &strProcess, TRUE))
	{
		
		GetProcessPath(pObject, pBuffer, nSize);

		return;
	}
	else if (RtlEqualUnicodeString(&pObjectType->Name, &strThread, TRUE))
	{
		
		GetProcessPath(IoThreadToProcess((PETHREAD)pObject), pBuffer, nSize);

		return;

	}
	else if (RtlEqualUnicodeString(&pObjectType->Name, &strSTmTm, TRUE))
	{
		NTSTATUS stats = STATUS_UNSUCCESSFUL;
		stats = AnalysisTmTm((PKTM)pObject, pBuffer, nSize);
		if (!NT_SUCCESS(stats)) {
			DbgPrint("hzw:get TmTm failed\n");
		}
		return;
	}

	else if (RtlEqualUnicodeString(&pObjectType->Name, &strTmRm, TRUE))
	{
		NTSTATUS stats = STATUS_UNSUCCESSFUL;
		GUID guid = { 0 };
		
		stats = GetGuidByTmRmObject((PKRESOURCEMANAGER)pObject, &guid);
		if (NT_SUCCESS(stats)) {
			
			RtlCopyMemory(pBuffer, &guid, (sizeof(guid) > nSize ? nSize : sizeof(guid)));
			
		}
		else {
			DbgPrint("hzw:get TmRm failed\n");
		}

		return;
	}

	else if (RtlEqualUnicodeString(&pObjectType->Name, &strFile, TRUE))
	{
		
		AnalysisFile((PFILE_OBJECT)pObject, pBuffer, nSize);
		return;

	}

	else if (RtlEqualUnicodeString(&pObjectType->Name, &strSection, TRUE))
	{
		
		AnalysisSection((PSECTION)pObject, pBuffer, nSize);
		return;
	}
	else if (RtlEqualUnicodeString(&pObjectType->Name, &strToken, TRUE))
	{
		AnalysisTokenObject((PTOKEN)pObject, pBuffer, nSize);
		return;
	}

	else
	{
		GetHandleObjectName(pObject, pBuffer, nSize);

		return;
	}

	return;
}


VOID  AnalysisTokenObject(PTOKEN pToken, PVOID pBuffer, ULONG nSize)
{
	TOKEN_INFO tokenInfo = { 0 };
	UNICODE_STRING  strPrimary = RTL_CONSTANT_STRING(L"(Primary)"); 
	UNICODE_STRING strImpersonation = RTL_CONSTANT_STRING(L"(Impersonation)");
	BOOLEAN bOK = FALSE;

	if (MmIsAddressValid(pToken))
	{
		if (pToken->TokenType == 1) {
			RtlCopyMemory(tokenInfo.TokenType, strPrimary.Buffer, strPrimary.MaximumLength);
		}
		else {
			RtlCopyMemory(tokenInfo.TokenType, strImpersonation.Buffer, strImpersonation.MaximumLength);
		}
		tokenInfo.TokenId = pToken->AuthenticationId.LowPart;
		
		if (MmIsAddressValid(pToken->LogonSession) && MmIsAddressValid(pToken->LogonSession->AccountName.Buffer)&& MmIsAddressValid(pToken->LogonSession->AuthorityName.Buffer))
		{
			RtlCopyMemory(tokenInfo.AccountName, pToken->LogonSession->AccountName.Buffer, pToken->LogonSession->AccountName.MaximumLength);
			RtlCopyMemory(tokenInfo.AuthorityName, pToken->LogonSession->AuthorityName.Buffer, pToken->LogonSession->AuthorityName.MaximumLength);
			bOK = TRUE;
		}
	}

	if (bOK) {
		__try 
		{
			ProbeForWrite(pBuffer, nSize,1);
			RtlCopyMemory(pBuffer, &tokenInfo, sizeof(tokenInfo));
		}
		__except (1) {
			DbgPrint("hzw:AnalysisTokenObject failed\n");
			return;
		}
	}

}


VOID  AnalysisFile(PFILE_OBJECT pFileObject, PVOID pBuffer, ULONG nSize)
{
	NTSTATUS stats = STATUS_UNSUCCESSFUL;
	

	UNICODE_STRING strTemp = RTL_CONSTANT_STRING(L"\\");
	POBJECT_NAME_INFORMATION pObjectNameInfo = NULL;
	if (MmIsAddressValid(pFileObject))
	{

		stats = IoQueryFileDosDeviceNameSafeIrql(pFileObject, &pObjectNameInfo);
		//stats = IoQueryFileDosDeviceName(pFileObject, &pObjectNameInfo);
		if (NT_SUCCESS(stats) && pObjectNameInfo)
		{

			if (RtlEqualUnicodeString(&pObjectNameInfo->Name, &strTemp, TRUE))
			{
				//如果 得到的符号是 \\ 就给另一个函数解析
				stats = GetHandleObjectName((PVOID)pFileObject, pBuffer, nSize);
			}
			else
			{
				__try {
					ProbeForWrite(pBuffer, nSize, 1);
					RtlCopyMemory(pBuffer, pObjectNameInfo->Name.Buffer, pObjectNameInfo->Name.MaximumLength > nSize ? nSize : pObjectNameInfo->Name.MaximumLength);
				}
				__except (1)
				{
					DbgPrint("hzw:AnalysisFile %x\n", GetExceptionCode());
				}

			}
			if (pObjectNameInfo) {
				ExFreePool(pObjectNameInfo);
			}
		}

	}
}

VOID  AnalysisSection(PSECTION pSection, PVOID pBuffer, ULONG nSize)
{
	POBJECT_NAME_INFORMATION pObjectNameInfo = NULL;
	NTSTATUS stats = STATUS_UNSUCCESSFUL;
	if (MmIsAddressValid(pSection->u1.ControlArea))
	{
		
		PFILE_OBJECT pFileObject = NULL;
		if (MmIsAddressValid((PVOID)pSection->u1.ControlArea) && MmIsAddressValid(pSection->u1.ControlArea->FilePointer.Value))
		{
			pFileObject = (PFILE_OBJECT)((pSection->u1.ControlArea->FilePointer.Value >> 4) << 4);

			stats = IoQueryFileDosDeviceNameSafeIrql(pFileObject, &pObjectNameInfo);
		//	stats = IoQueryFileDosDeviceName(pFileObject, &pObjectNameInfo);
			if (NT_SUCCESS(stats))
			{

				__try {
					ProbeForWrite(pBuffer, nSize, 1);
					RtlCopyMemory(pBuffer, pObjectNameInfo->Name.Buffer, pObjectNameInfo->Name.MaximumLength > nSize ? nSize : pObjectNameInfo->Name.MaximumLength);
				}
				__except (1) {
					DbgPrint("hzw:AnalysisSection failed %x\n",GetExceptionCode());
				}

			}

			if (pObjectNameInfo) {
				ExFreePool(pObjectNameInfo);
			}
		}
		else
		{
			
			stats = GetHandleObjectName(pSection, pBuffer, nSize);
			if (stats == 2 && MmIsAddressValid(pSection) && MmIsAddressValid(&pSection->u))
			{
				if (pSection->u.Flags.Commit)
				{
					UNICODE_STRING strCommit = RTL_CONSTANT_STRING(L"Commit");//dt _section
					__try {
						ProbeForWrite(pBuffer, nSize, 1);
						RtlCopyMemory(pBuffer, strCommit.Buffer, strCommit.MaximumLength);
					}
					__except (1) {

					}

				}
			}
		}

	}

	return;
}





NTSTATUS  AnalysisTmTm(PKTM pKtm,PVOID pBuffer,ULONG nSize)
{
	NTSTATUS nStatus = STATUS_UNSUCCESSFUL;
	
	if (MmIsAddressValid(pKtm))
	{
		//路径
		if ( MmIsAddressValid(&pKtm->LogFileName)&&MmIsAddressValid(pKtm->LogFileName.Buffer) && pKtm->LogFileName.Length > 0)
		{
			__try {
				ProbeForWrite(pBuffer, nSize,1);
				RtlCopyMemory(pBuffer, pKtm->LogFileName.Buffer, pKtm->LogFileName.MaximumLength > nSize ? nSize : pKtm->LogFileName.MaximumLength);
				nStatus = STATUS_SUCCESS;
			}
			__except (1)
			{
				return GetExceptionCode();
			}
		}
		else
		{
			//guid
			if (MmIsAddressValid(pKtm) && MmIsAddressValid(&pKtm->TmIdentity))
			{
				__try {
					ProbeForWrite(pBuffer, nSize, 1);
					RtlCopyMemory(pBuffer, &pKtm->TmIdentity, sizeof(GUID) > nSize ? nSize : sizeof(GUID));
					nStatus = STATUS_SUCCESS;
				}
				__except (1)
				{
					return GetExceptionCode();
				}
			}
		}
	}

	return nStatus;
}

NTSTATUS GetGuidByTmRmObject(PKRESOURCEMANAGER pTmRm,OUT GUID* pGuid)
{
	NTSTATUS nStatus = STATUS_UNSUCCESSFUL;

	if (MmIsAddressValid(pGuid) && MmIsAddressValid(pTmRm) && MmIsAddressValid(&pTmRm->RmId))
	{
		__try {
			*pGuid = pTmRm->RmId;
			nStatus = STATUS_SUCCESS;
		}
		__except (1)
		{
			return GetExceptionCode();
		}
	}

	return nStatus;
}




VOID GetProcessPath(PEPROCESS a, PVOID szObjectName,ULONG nSize)
{
	PVOID pFilePoint = NULL;
	POBJECT_NAME_INFORMATION pObjectNameInfo = NULL;

	NTSTATUS stats = PsReferenceProcessFilePointer(a, &pFilePoint);
	if (NT_SUCCESS(stats))
	{
		
		stats = IoQueryFileDosDeviceNameSafeIrql(pFilePoint, &pObjectNameInfo);
		//stats = IoQueryFileDosDeviceName(pFilePoint, &pObjectNameInfo);
		if (NT_SUCCESS(stats) && pObjectNameInfo)
		{
			__try {
				ProbeForWrite(szObjectName, nSize, 1);
				RtlCopyMemory(szObjectName, pObjectNameInfo->Name.Buffer, (pObjectNameInfo->Name.MaximumLength > nSize ? nSize : pObjectNameInfo->Name.MaximumLength));
			}
			__except (1) {
				DbgPrint("hzw:GetProcessPath %x\n", GetExceptionCode());
			}

			if (pObjectNameInfo) {
				ExFreePool(pObjectNameInfo);
			}
		}
		ObDereferenceObject(pFilePoint);
	}
	else
	{
		char* szTemp = NULL;
		szTemp = PsGetProcessImageFileName(a);
		if (szTemp) 
		{
			CharToWchar(szTemp, szObjectName);
		}
	}
}



NTSTATUS GetHandleObjectName(IN PVOID Object, IN PVOID szObjectName,ULONG nSize)
{
	if (!MmIsAddressValid(Object))return STATUS_UNSUCCESSFUL;

	NTSTATUS status = STATUS_UNSUCCESSFUL;

	POBJECT_NAME_INFORMATION pStr = NULL;
	ULONG nRetLength = 0;

	
	status = ObQueryNameString(Object, NULL, 0, &nRetLength);
	if (STATUS_INFO_LENGTH_MISMATCH == status)
	{

		pStr = ExAllocatePool(PagedPool, nRetLength);
		if (pStr)
		{
			RtlZeroMemory(pStr, nRetLength);
			status = ObQueryNameString(Object, pStr, nRetLength, &nRetLength);
			if (NT_SUCCESS(status) && pStr->Name.Length>0)
			{
				__try {
					ProbeForWrite(szObjectName, nSize, 1);
					RtlCopyMemory(szObjectName, pStr->Name.Buffer, nRetLength > nSize ? nSize : nRetLength);
				}
				__except (1)
				{
					DbgPrint("hzw:GetHandleObjectName %x\n",GetExceptionCode());
				}
			}


			ExFreePool(pStr);
		}

	}

	if (NT_SUCCESS(status) && pStr->Name.Length == 0) {
		
		status = 2;
	}


	return status;
}



