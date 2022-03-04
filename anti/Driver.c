#include "heads.h"
#include "emunKernelModule.h"
#include "CommFunction.h"
#include "KernelInject.h"
#include "emunAllKernleCallBack.h"
BOOLEAN g_bInit = FALSE;
BOOLEAN g_bInitPte = FALSE;
_NtSuspendThread NtSuspendThread = NULL;
_NtResumeThread  NtResumeThread = NULL;
_NtTerminateThread NtTerminateThread = NULL;
_ZwCreateThreadEx ZwCreateThreadEx = NULL;
pfnNtAlertResumeThread NtAlertResumeThread = NULL;
pfnNtQueueApcThread NtQueueApcThread = NULL;

ZWPROTECTVIRTUALMEMORY pfnZwProtectVirtualMemory = NULL;
ZWREADVIRTUALMEMORY pfnZwReadVirtualMemory = NULL;
ZWWRITEVIRTUALMEMORY pfnZwWriteVirtualMemory = NULL;
ULONG64 g_PspLoadImageNotifyRoutine = NULL;
ULONG64 g_PspCreateThreadNotifyRoutine = NULL;
ULONG64 g_CmCallbackListHead = NULL;
ULONG64 g_PspCreateProcessNotifyRoutine = NULL;
/*必定蓝屏*/
VOID test_HideDriver(PDRIVER_OBJECT pDriverObject)
{
	if (!pDriverObject) return;
	PLDR_DATA_TABLE_ENTRY entry = (PLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;
	PLDR_DATA_TABLE_ENTRY firstentry;
	UNICODE_STRING uniDriverName;

	firstentry = entry;
	
	RtlInitUnicodeString(&uniDriverName, L"vmhgfs.sys");

	while ((PLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Flink != firstentry) {
		if (entry->FullDllName.Buffer != 0) 
		{

			if (RtlCompareUnicodeString(&uniDriverName, &(entry->BaseDllName), TRUE) == 0) 
			{
			
				*((ULONG_PTR*)entry->InLoadOrderLinks.Blink) = (ULONG_PTR)entry->InLoadOrderLinks.Flink;
				entry->InLoadOrderLinks.Flink->Blink = entry->InLoadOrderLinks.Blink;

				entry->InLoadOrderLinks.Flink = (LIST_ENTRY*)&(entry->InLoadOrderLinks.Flink);
				entry->InLoadOrderLinks.Blink = (LIST_ENTRY*)&(entry->InLoadOrderLinks.Flink);

				break;
			}
		}
	
		entry = (PLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Flink;

	}
}


VOID DriverUnload(PDRIVER_OBJECT pDriverObject)
{
	if (g_bInit) {
		UNICODE_STRING symLinkName;
		RtlInitUnicodeString(&symLinkName, 符号名);
		IoDeleteSymbolicLink(&symLinkName);
		IoDeleteDevice(pDriverObject->DeviceObject);

		Deinitialize();
	}

	if (g_InitLoadImage) {
		PsRemoveLoadImageNotifyRoutine(LoadImageNotifyCallback);
	}

	if (g_pInjectInfo)
	{
		
		RtlFreeMemory(g_pInjectInfo);
		g_pInjectInfo = NULL;
	}

	KdPrint(("驱动卸载成功!"));
	
}







NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryString)
{
	UNREFERENCED_PARAMETER(pRegistryString);

	NTSTATUS status = STATUS_UNSUCCESSFUL;

	pDriverObject->DriverUnload = DriverUnload;
	
	
	//ObRegisterCallbacks()
	if (!initVerSion()) {
		return STATUS_UNSUCCESSFUL;
	}
	
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = &DispatchControlCode;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = &test_DispatchControlCode;
	status = CreateDevice(pDriverObject);

	
	//test_HideDriver(pDriverObject);


	if (NT_SUCCESS(Initialize()))
	{
		 pfnZwProtectVirtualMemory = GetKernelZwFuncByName("ZwProtectVirtualMemory");
		 ZwCreateThreadEx = GetKernelZwFuncByName("ZwCreateThreadEx");

		 pfnZwReadVirtualMemory = GetKernelZwFuncByName("ZwReadVirtualMemory");
		 pfnZwWriteVirtualMemory = GetKernelZwFuncByName("ZwWriteVirtualMemory");

		 if (pfnZwProtectVirtualMemory && ZwCreateThreadEx &&pfnZwReadVirtualMemory&&pfnZwWriteVirtualMemory) {
			 g_bInit = TRUE;
		 }
	}

	if (InitializePteBase())
	{
		g_bInitPte = TRUE;
	}
	
	//
	
	g_PspLoadImageNotifyRoutine = SearchPspLoadImageNotifyRoutine();
	DbgPrint("hzw:g_PspLoadImageNotifyRoutine: %llx", g_PspLoadImageNotifyRoutine);
	g_PspCreateThreadNotifyRoutine = SearchPspCreateThreadNotifyRoutine();
	DbgPrint("hzw:g_PspCreateThreadNotifyRoutine: %llx", g_PspCreateThreadNotifyRoutine);
	g_CmCallbackListHead = SearchCallbackListHead();
	DbgPrint("hzw:g_CmCallbackListHead: %llx", g_CmCallbackListHead);
	g_PspCreateProcessNotifyRoutine = FindPspCreateProcessNotifyRoutine();
	DbgPrint("hzw:g_PspCreateProcessNotifyRoutine: %llx", g_PspCreateProcessNotifyRoutine);
	
	return status;
}


