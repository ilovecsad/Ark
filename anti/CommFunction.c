#include "CommFunction.h"
#include "struct.h"

ULONG_PTR g_pxe_base = 0;
ULONG_PTR g_pde_base = 0;
ULONG_PTR g_ppe_base = 0;
ULONG_PTR g_pte_base = 0;
extern BOOLEAN g_bInitPte;
//CHAR*转 WCHAR*
//输入窄字符串首地址，输出宽字符串，BUFFER 需要已经分配好空间

VOID CharToWchar(PCHAR src, PWCHAR dst)
{
	UNICODE_STRING uString;
	ANSI_STRING aString;
	RtlInitAnsiString(&aString, src);
	RtlAnsiStringToUnicodeString(&uString, &aString, TRUE);
	__try {
		wcscpy(dst, uString.Buffer);
	}
	__except (1) {
		DbgPrint("hzw:%x\n", GetExceptionCode());
	}
	RtlFreeUnicodeString(&uString);
}

//WCHAR*转换为 CHAR*
//输入宽字符串首地址，输出窄字符串，BUFFER 需要已经分配好空间
VOID WcharToChar(PWCHAR src, PCHAR dst)
{
	UNICODE_STRING uString;
	ANSI_STRING aString;
	RtlInitUnicodeString(&uString, src);
	RtlUnicodeStringToAnsiString(&aString, &uString, TRUE);
	__try {
		strcpy(dst, aString.Buffer);
	}
	__except (1) {
		DbgPrint("hzw:%x\n", GetExceptionCode());
	}
	
	RtlFreeAnsiString(&aString);
}

//UNICODE_STRINGz 转换为 CHAR*
//输入 UNICODE_STRING 的指针，输出窄字符串，BUFFER 需要已经分配好空间
VOID UnicodeToChar(PUNICODE_STRING dst, char *src)
{
	ANSI_STRING string;
	RtlUnicodeStringToAnsiString(&string, dst, TRUE);
	__try {
		strcpy(src, string.Buffer);
	}
	__except (1) {
		DbgPrint("hzw:%x\n", GetExceptionCode());
	}
	
	RtlFreeAnsiString(&string);
}

//7.强制重启计算机。在内核里直接使用 OUT 指令就能强制重启计算机而不可能被任何钩子
//拦截。此代码可以用在反调试里。
VOID ForceReboot()
{
	typedef void(__fastcall *FCRB)(void);

	FCRB fcrb = NULL;
	UCHAR shellcode[6] = "\xB0\xFE\xE6\x64\xC3";
	fcrb = ExAllocatePool(NonPagedPool, 5);
	memcpy(fcrb, shellcode, 5);
	fcrb();
}

//8.强制关闭计算机。在内核里直接使用 OUT 指令就能强制关闭计算机而不可能被任何钩子
//拦截。此代码可以用在反调试里。
VOID ForceShutdown()
{
	typedef void(__fastcall *FCRB)(void);

	FCRB fcrb = NULL;
	UCHAR shellcode[12] = "\x66\xB8\x01\x20\x66\xBA\x04\x10\x66\xEF\xC3";
	fcrb = ExAllocatePool(NonPagedPool, 11);
	memcpy(fcrb, shellcode, 11);
	fcrb();
}




PETHREAD HandleToThread(IN HANDLE hThreadHanle)
{
	NTSTATUS status;
	PETHREAD Thread = NULL;

	status = ObReferenceObjectByHandle(
		hThreadHanle,
		0,
		*PsThreadType,
		KernelMode,
		&Thread,
		NULL);

	if (NT_SUCCESS(status))
	{
		ObDereferenceObject(Thread);
		return Thread;
	}
	else
	{
		return NULL;
	}
}


PEPROCESS HandleToProcess(IN HANDLE hProcessHanle)
{
	NTSTATUS status;
	PEPROCESS Process = NULL;

	status = ObReferenceObjectByHandle(
		hProcessHanle,
		0,
		*PsProcessType,
		KernelMode,
		&Process,
		NULL);

	if (NT_SUCCESS(status))
	{
		return Process;
	}
	else
	{
		return NULL;
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////////

//https://bbs.pediy.com/thread-224747.htm 说明原因

typedef struct tag_FyWorkQueueItem
{
	WORK_QUEUE_ITEM WorkQueueItem;
	PFILE_OBJECT  arg1;
	POBJECT_NAME_INFORMATION  arg2;
	KEVENT CompleteEvent;
	NTSTATUS   bStatus;
} FyWorkQueueItem, *PFyWorkQueueItem;


VOID IoQueryFileDosDeviceNameWorkItem(IN PFyWorkQueueItem lpFyWorkQueueItem)
{

	__try {
		lpFyWorkQueueItem->bStatus = IoQueryFileDosDeviceName((PFILE_OBJECT)lpFyWorkQueueItem->arg1,&lpFyWorkQueueItem->arg2);
		
	}
	__except (1) {
		
	}
	KeSetEvent(&lpFyWorkQueueItem->CompleteEvent, IO_NO_INCREMENT, FALSE);
}


//需要释放内存 类似于 IoQueryFileDosDeviceName 使用
NTSTATUS IoQueryFileDosDeviceNameSafeIrql(PFILE_OBJECT FileObject, POBJECT_NAME_INFORMATION* ObjectNameInformation)
{
	
	NTSTATUS            bStatus = STATUS_UNSUCCESSFUL;
	FyWorkQueueItem* WorkItem = NULL;

	WorkItem = (FyWorkQueueItem*)ExAllocatePoolWithTag(NonPagedPool, sizeof(FyWorkQueueItem),'hzw');
	if (!WorkItem)return STATUS_UNSUCCESSFUL;

	RtlZeroMemory(WorkItem, sizeof(FyWorkQueueItem));
	if (KeGetCurrentIrql() <= APC_LEVEL)
	{
		if (KeAreApcsDisabled() || KeGetCurrentIrql() == APC_LEVEL)
		{
		
			/*ExInitializeWorkItem()
				Item->WorkerRoutine = Routine;
	            Item->Parameter = Context;
	            Item->List.Flink = NULL;
			*/
			KeInitializeEvent(&WorkItem->CompleteEvent, NotificationEvent, FALSE);

			WorkItem->bStatus = STATUS_UNSUCCESSFUL;
			WorkItem->WorkQueueItem.List.Flink = NULL;
			WorkItem->WorkQueueItem.WorkerRoutine = (PWORKER_THREAD_ROUTINE)IoQueryFileDosDeviceNameWorkItem;
			WorkItem->arg1 = FileObject;
			WorkItem->WorkQueueItem.Parameter = WorkItem;
			ExQueueWorkItem(&WorkItem->WorkQueueItem, DelayedWorkQueue);

			KeWaitForSingleObject(&WorkItem->CompleteEvent, Executive, KernelMode, FALSE, NULL);
			__try {
				
				bStatus = WorkItem->bStatus;
				*ObjectNameInformation = WorkItem->arg2;
			}
			__except (1) {

			}
		}
		else {
			DbgPrint("hzw: IoQueryFileDosDeviceNameSafeIrql failed %d %x\n", KeGetCurrentIrql());
		}
	}
	else
	{
		DbgPrint("hzw: IRQL太高了 %d %x\n", KeGetCurrentIrql());
		bStatus = STATUS_UNSUCCESSFUL;
	}

	if (WorkItem) {
		ExFreePoolWithTag(WorkItem,'hzw');
	}


	return bStatus;
}

/*
SrcAddr r3地址要复制
DstAddr R0 申请的地址
*/

NTSTATUS SafeCopyMemory_R3_to_R0(ULONG_PTR SrcAddr/*R3 要复制的地址*/, ULONG_PTR DstAddr/*在R0申请的地址*/, ULONG Size)
{
	
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	ULONG nRemainSize = PAGE_SIZE - (SrcAddr & 0xFFF);
	ULONG nCopyedSize = 0;

	if (!SrcAddr ||
		!DstAddr ||
		!Size)
	{
		return status;
	}

	while (nCopyedSize < Size)
	{
		PMDL pSrcMdl = NULL;
		PVOID pMappedSrc = NULL;

		if (Size - nCopyedSize < nRemainSize)
		{
			nRemainSize = Size - nCopyedSize;
		}

		pSrcMdl = IoAllocateMdl((PVOID)(SrcAddr & 0xFFFFFFFFFFFFF000), PAGE_SIZE, FALSE, FALSE, NULL);
		if (pSrcMdl)
		{
			__try
			{
				MmProbeAndLockPages(pSrcMdl, UserMode, IoReadAccess);
				pMappedSrc = MmGetSystemAddressForMdlSafe(pSrcMdl, NormalPagePriority);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
			}
		}

		if (pMappedSrc)
		{
			__try {
				RtlCopyMemory((PVOID)DstAddr, (PVOID)((ULONG_PTR)pMappedSrc + (SrcAddr & 0xFFF)), nRemainSize);
			}
			__except(1)
			{
				DbgPrint("hzw:拷贝内存异常%x\n", GetExceptionCode());
			}
			MmUnlockPages(pSrcMdl);
		}

		if (pSrcMdl)
		{
			IoFreeMdl(pSrcMdl);
		}

		if (nCopyedSize)
		{
			nRemainSize = PAGE_SIZE;
		}

		nCopyedSize += nRemainSize;
		SrcAddr += nRemainSize;
		DstAddr += nRemainSize;
	}

	status = STATUS_SUCCESS;

	return status;
}



NTSTATUS SafeCopyMemory_R0_to_R3(PVOID SrcAddr/*R0要复制的地址*/, PVOID DstAddr/*返回 R3 的地址*/, ULONG Size)
{
	PMDL  pSrcMdl = NULL, pDstMdl = NULL;
	PUCHAR pSrcAddress = NULL, pDstAddress = NULL;
	NTSTATUS st = STATUS_UNSUCCESSFUL;

	pSrcMdl = IoAllocateMdl(SrcAddr, Size, FALSE, FALSE, NULL);
	if (!pSrcMdl)
	{
		return st;
	}

	MmBuildMdlForNonPagedPool(pSrcMdl);

	pSrcAddress = MmGetSystemAddressForMdlSafe(pSrcMdl, NormalPagePriority);


	if (!pSrcAddress)
	{
		IoFreeMdl(pSrcMdl);
		return st;
	}

	pDstMdl = IoAllocateMdl(DstAddr, Size, FALSE, FALSE, NULL);
	if (!pDstMdl)
	{
		IoFreeMdl(pSrcMdl);
		return st;
	}

	__try
	{
		MmProbeAndLockPages(pDstMdl, UserMode, IoWriteAccess);
		pDstAddress = MmGetSystemAddressForMdlSafe(pDstMdl, NormalPagePriority);

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}

	if (pDstAddress)
	{
		__try {
			RtlCopyMemory(pDstAddress, pSrcAddress, Size);
		}
		__except (1) {
			DbgPrint("hzw:拷贝内存异常%x\n", GetExceptionCode());
		}
		MmUnlockPages(pDstMdl);
		st = STATUS_SUCCESS;
	}

	IoFreeMdl(pDstMdl);
	IoFreeMdl(pSrcMdl);

	return st;
}

ULONG64 getPteBase()
{
	return g_pte_base;
}

ULONG64 getPte(ULONG64 VirtualAddress)
{
	ULONG64 pteBase = getPteBase();
	return ((VirtualAddress >> 9) & 0x7FFFFFFFF8) + pteBase;
}

ULONG64 getPde(ULONG64 VirtualAddress)
{
	ULONG64 pteBase = getPteBase();
	ULONG64 pte = getPte(VirtualAddress);
	return ((pte >> 9) & 0x7FFFFFFFF8) + pteBase;
}

ULONG64 getPdpte(ULONG64 VirtualAddress)
{
	ULONG64 pteBase = getPteBase();
	ULONG64 pde = getPde(VirtualAddress);
	return ((pde >> 9) & 0x7FFFFFFFF8) + pteBase;
}

ULONG64 getPml4e(ULONG64 VirtualAddress)
{
	ULONG64 pteBase = getPteBase();
	ULONG64 ppe = getPdpte(VirtualAddress);
	return ((ppe >> 9) & 0x7FFFFFFFF8) + pteBase;
}



// 用于 穿透 应用层的虚拟内存属性
NTSTATUS RtlSuperCopyMemory(IN VOID UNALIGNED* Destination, IN CONST VOID UNALIGNED* Source, IN ULONG Length)
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	if (!Destination || !Source || Length == 0)
	{
		return ntStatus;
	}

	PVOID pDstAddress = NULL;
	
	PMDL Mdl = IoAllocateMdl(Destination, Length, 0, 0, NULL);
	if (Mdl == NULL)
	{
		return STATUS_NO_MEMORY;
	}

	__try
	{
		MmProbeAndLockPages(Mdl, UserMode, IoReadAccess);
		
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
	pDstAddress = MmGetSystemAddressForMdlSafe(Mdl, NormalPagePriority);
	if (pDstAddress)
	{
		__try 
		{
			RtlCopyMemory(pDstAddress, Source, Length);
			ntStatus = STATUS_SUCCESS;
		}
		__except (1) {
			ntStatus = GetExceptionCode();
		}

		MmUnlockPages(Mdl);
	}


	IoFreeMdl(Mdl);

	return ntStatus;
}



BOOLEAN InitializePteBase()
{
	
	BOOLEAN bRet = FALSE;
	ULONG_PTR PTEBase = 0;
	ULONG_PTR PXEPA = __readcr3() & 0xFFFFFFFFF000;
	PHYSICAL_ADDRESS PXEPAParam;
	PXEPAParam.QuadPart = (LONGLONG)PXEPA;
	ULONG_PTR PXEVA = (ULONG_PTR)MmGetVirtualForPhysical(PXEPAParam); // _MMPTE_HARDWARE 0xffffaed7`6bb5d000
	ULONG_PTR PXEOffset = 0;
	ULONG_PTR slot = 0;
	if (PXEVA)
	{
		
		do
		{
			if ((*(PULONGLONG)(PXEVA + PXEOffset) & 0xFFFFFFFFF000) == PXEPA)
			{
				PTEBase = (PXEOffset + 0xFFFF000) << 36;
				bRet = TRUE;
				break;
			}
			PXEOffset += 8;
			slot++;
		} while (PXEOffset < PAGE_SIZE);
	}


	if (PTEBase) {
		g_pte_base = PTEBase;
		g_pde_base = (ULONG_PTR)g_pte_base + ((__int64)slot << 30);
		g_ppe_base = (ULONG_PTR)g_pte_base + ((__int64)slot << 30) + ((__int64)slot << 21);
		g_pxe_base = ((ULONG_PTR)g_ppe_base + ((__int64)slot << 12));
	}


	return bRet;
}


// 用于 穿透 内核层的虚拟内存属性
NTSTATUS RtlSuperCopyMemoryEx(IN VOID UNALIGNED* Destination, IN CONST VOID UNALIGNED* Source, IN ULONG Length)
{

	if (!Destination || !Source || Length == 0)
	{
		return STATUS_UNSUCCESSFUL;
	}

	const KIRQL Irql = KeRaiseIrqlToDpcLevel();

	PMDL Mdl = IoAllocateMdl(Destination, Length, 0, 0, NULL);
	if (Mdl == NULL)
	{
		KeLowerIrql(Irql);
		return STATUS_NO_MEMORY;
	}

	MmBuildMdlForNonPagedPool(Mdl);

	// Hack: prevent bugcheck from Driver Verifier and possible future versions of Windows
#pragma prefast(push)
#pragma prefast(disable:__WARNING_MODIFYING_MDL, "Trust me I'm a scientist")
	const CSHORT OriginalMdlFlags = Mdl->MdlFlags;
	Mdl->MdlFlags |= MDL_PAGES_LOCKED;
	Mdl->MdlFlags &= ~MDL_SOURCE_IS_NONPAGED_POOL;

	// Map pages and do the copy
	const PVOID Mapped = MmMapLockedPagesSpecifyCache(Mdl, KernelMode, MmCached, NULL, FALSE, HighPagePriority);
	if (Mapped == NULL)
	{
		Mdl->MdlFlags = OriginalMdlFlags;
		IoFreeMdl(Mdl);
		KeLowerIrql(Irql);
		return STATUS_NONE_MAPPED;
	}

	RtlCopyMemory(Mapped, Source, Length);

	MmUnmapLockedPages(Mapped, Mdl);
	Mdl->MdlFlags = OriginalMdlFlags;
#pragma prefast(pop)
	IoFreeMdl(Mdl);
	KeLowerIrql(Irql);

	return STATUS_SUCCESS;
}



char* GetPathA(const char *szCurFile, char a, ULONG nCount)
{
	ULONG index = 0;
	for (SIZE_T i = strlen(szCurFile) - 1; i > 0; i--)
	{
		if (szCurFile[i] == a)
		{
			index++;
			if (index == nCount)
			{
				return (char*)&szCurFile[i];
			}
		}
	}
	return NULL;
}

wchar_t* GetPathW(const wchar_t *szCurFile, wchar_t a, ULONG nCount)
{
	ULONG index = 0;
	for (SIZE_T i = wcslen(szCurFile) - 1; i > 0; i--)
	{

		if (szCurFile[i] == a)
		{
			index++;
			if (index == nCount)
			{
				return (wchar_t*)&szCurFile[i];
			}
		}
	}
	return NULL;
}


