#include "thread.h"
#include "exapi.h"
#include "emunProcess.h"


extern pfnNtAlertResumeThread NtAlertResumeThread;
extern pfnNtQueueApcThread NtQueueApcThread;

VOID NtSuspendThreadWork(IN PVOID context)
{
	typedef struct _INPUT_DATA_
	{
		ULONG PreviousSuspendCount;
		HANDLE dwThreadId;
		NTSTATUS ntStatus;
		KEVENT kEvent;
	}INPUT_DATA;

	INPUT_DATA* arg = NULL;
	PETHREAD pThread = NULL;
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	ULONG uLength = 0;


	arg = (INPUT_DATA*)context;
	if (MmIsAddressValid(arg)) 
	{
		HANDLE hThread = 0;
		status = PsLookupThreadByThreadId(arg->dwThreadId, &pThread);
		if (NT_SUCCESS(status)) {

	
			status = ObOpenObjectByPointer(pThread, OBJ_KERNEL_HANDLE, NULL, GENERIC_ALL, *PsThreadType, KernelMode, &hThread);

			if (NT_SUCCESS(status) && hThread && MmIsAddressValid(NtSuspendThread))
			{

				status = NtSuspendThread(hThread, &uLength);
			}

			ObDereferenceObject(pThread);
		}

	}

	__try {
		arg->ntStatus = status;
		arg->PreviousSuspendCount = uLength;
	}
	__except (1) {

	}

	KeSetEvent(&arg->kEvent, 0, TRUE);
	PsTerminateSystemThread(STATUS_SUCCESS);
}


//创建线程的函数
NTSTATUS NtSuspendThreadBySystemThread(IN HANDLE dwThreadId, OUT PULONG PreviousSuspendCount OPTIONAL)
{
	HANDLE hThread = NULL;
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	
	typedef struct _INPUT_DATA_
	{
		ULONG PreviousSuspendCount;
		HANDLE dwThreadId;
		NTSTATUS ntStatus;
		KEVENT kEvent;
	}INPUT_DATA;
	


	INPUT_DATA* pArg = NULL;

	pArg = (INPUT_DATA*)ExAllocatePool(NonPagedPool, sizeof(INPUT_DATA));

	if (!pArg) return status;

	RtlZeroMemory(pArg, sizeof(INPUT_DATA));

	pArg->dwThreadId = dwThreadId;
	KeInitializeEvent(&pArg->kEvent, SynchronizationEvent, FALSE);

	status = PsCreateSystemThread(&hThread, 0, NULL, NULL, NULL, (PKSTART_ROUTINE)NtSuspendThreadWork,(PVOID)pArg);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("hzw:NtSuspendThreadBySystemThread failed!");
		if (pArg) {
			ExFreePool(pArg);
		}
		return status;
	}


	ZwClose(hThread);
	KeWaitForSingleObject(&pArg->kEvent, Executive, KernelMode, FALSE, NULL);

	__try {
		status = pArg->ntStatus;
		*PreviousSuspendCount = pArg->PreviousSuspendCount;
	}
	__except (1) {

	}


	if (pArg) 
	{
		ExFreePool(pArg);
	}

	return status;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////


VOID NtResumeThreadWork(IN PVOID context)
{
	typedef struct _INPUT_DATA_
	{
		ULONG PreviousSuspendCount;
		HANDLE dwThreadId;
		NTSTATUS ntStatus;
		KEVENT kEvent;
	}INPUT_DATA;

	INPUT_DATA* arg = NULL;
	PETHREAD pThread = NULL;
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	ULONG uLength = 0;


	arg = (INPUT_DATA*)context;
	if (MmIsAddressValid(arg))
	{
		HANDLE hThread = 0;
		status = PsLookupThreadByThreadId(arg->dwThreadId, &pThread);
		if (NT_SUCCESS(status)) {

			
			status = ObOpenObjectByPointer(pThread, OBJ_KERNEL_HANDLE, NULL, GENERIC_ALL, *PsThreadType, KernelMode, &hThread);

			if (NT_SUCCESS(status) && hThread && MmIsAddressValid(NtResumeThread))
			{

				status = NtResumeThread(hThread, &uLength);
				ZwClose(hThread);
			}
		
			ObDereferenceObject(pThread);
		}

	}

	__try {
		arg->ntStatus = status;
		arg->PreviousSuspendCount = uLength;
	}
	__except (1) {

	}

	KeSetEvent(&arg->kEvent, 0, TRUE);
	PsTerminateSystemThread(STATUS_SUCCESS);
}


//创建线程的函数
NTSTATUS NtResumeThreadBySystemThread(IN HANDLE dwThreadId, OUT PULONG PreviousSuspendCount OPTIONAL)
{
	HANDLE hThread = NULL;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	typedef struct _INPUT_DATA_
	{
		ULONG PreviousSuspendCount;
		HANDLE dwThreadId;
		NTSTATUS ntStatus;
		KEVENT kEvent;
	}INPUT_DATA;



	INPUT_DATA* pArg = NULL;

	pArg = (INPUT_DATA*)ExAllocatePool(NonPagedPool, sizeof(INPUT_DATA));

	if (!pArg) return status;

	RtlZeroMemory(pArg, sizeof(INPUT_DATA));

	pArg->dwThreadId = dwThreadId;
	KeInitializeEvent(&pArg->kEvent, SynchronizationEvent, FALSE);

	status = PsCreateSystemThread(&hThread, 0, NULL, NULL, NULL, (PKSTART_ROUTINE)NtResumeThreadWork, (PVOID)pArg);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("hzw:NtResumeThreadBySystemThread failed!");
		if (pArg) {
			ExFreePool(pArg);
		}
		return status;
	}


	ZwClose(hThread);
	KeWaitForSingleObject(&pArg->kEvent, Executive, KernelMode, FALSE, NULL);

	__try {
		status = pArg->ntStatus;
		*PreviousSuspendCount = pArg->PreviousSuspendCount;
	}
	__except (1) {

	}


	if (pArg)
	{
		ExFreePool(pArg);
	}

	return status;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////


VOID NtTerminateThreadWork(IN PVOID context)
{
	typedef struct _INPUT_DATA_
	{
		ULONG PreviousSuspendCount;
		HANDLE dwThreadId;
		NTSTATUS ntStatus;
		KEVENT kEvent;
	}INPUT_DATA;

	INPUT_DATA* arg = NULL;
	PETHREAD pThread = NULL;
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	ULONG uLength = 0;

	arg = (INPUT_DATA*)context;
	if (MmIsAddressValid(arg))
	{
		HANDLE hThread = 0;
		status = PsLookupThreadByThreadId(arg->dwThreadId, &pThread);
		if (NT_SUCCESS(status)) {

			status = ObOpenObjectByPointer(pThread, OBJ_KERNEL_HANDLE, NULL, GENERIC_ALL, *PsThreadType, KernelMode, &hThread);

			if (NT_SUCCESS(status) && hThread && MmIsAddressValid(NtTerminateThread))
			{

				status = NtTerminateThread(hThread, &uLength);

				ZwClose(hThread);
			}

			ObDereferenceObject(pThread);
		}

	}

	__try {
		arg->ntStatus = status;
		arg->PreviousSuspendCount = uLength;
	}
	__except (1) {

	}

	KeSetEvent(&arg->kEvent, 0, TRUE);
	PsTerminateSystemThread(STATUS_SUCCESS);
}


//创建线程的函数
NTSTATUS NtTerminateThreadBySystemThread(IN HANDLE dwThreadId, OUT PULONG PreviousSuspendCount OPTIONAL)
{
	HANDLE hThread = NULL;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	typedef struct _INPUT_DATA_
	{
		ULONG PreviousSuspendCount;
		HANDLE dwThreadId;
		NTSTATUS ntStatus;
		KEVENT kEvent;
	}INPUT_DATA;



	INPUT_DATA* pArg = NULL;

	pArg = (INPUT_DATA*)ExAllocatePool(NonPagedPool, sizeof(INPUT_DATA));

	if (!pArg) return status;

	RtlZeroMemory(pArg, sizeof(INPUT_DATA));

	pArg->dwThreadId = dwThreadId;
	KeInitializeEvent(&pArg->kEvent, SynchronizationEvent, FALSE);

	status = PsCreateSystemThread(&hThread, 0, NULL, NULL, NULL, (PKSTART_ROUTINE)NtTerminateThreadWork, (PVOID)pArg);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("hzw:NtResumeThreadBySystemThread failed!");
		if (pArg) {
			ExFreePool(pArg);
		}
		return status;
	}


	ZwClose(hThread);
	KeWaitForSingleObject(&pArg->kEvent, Executive, KernelMode, FALSE, NULL);

	__try {
		status = pArg->ntStatus;
		*PreviousSuspendCount = pArg->PreviousSuspendCount;
	}
	__except (1) {

	}


	if (pArg)
	{
		ExFreePool(pArg);
	}

	return status;
}


