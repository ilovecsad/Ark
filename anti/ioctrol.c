
#include "ioctrol.h"
#include "emunProcess.h"
#include "CommFunction.h"
#include "emunKernelModule.h"
#include "emunMemory.h"
#include "emunHands.h"
#include "thread.h"
#include "KernelInject.h"
#include "emunAllKernleCallBack.h"

PINJECT_INFO g_pInjectInfo = NULL;
extern BOOLEAN g_bHideInject;

NTSTATUS test_DispatchControlCode(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{

	UNREFERENCED_PARAMETER(pDeviceObject);
	pIrp->IoStatus.Information = 0;//实际输出字节 

	pIrp->IoStatus.Status = STATUS_SUCCESS;//返回成功
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);//指示完成此IRP
	return STATUS_SUCCESS;
}
NTSTATUS CreateDevice(IN PDRIVER_OBJECT pDriverObject)
{
	NTSTATUS status;
	PDEVICE_OBJECT pDevObj;/*用来返回创建设备*/

						   //创建设备名称
	UNICODE_STRING devName;
	UNICODE_STRING symLinkName; // 
	RtlInitUnicodeString(&devName, 设备名);


	//创建设备
	status = IoCreateDevice(pDriverObject, \
		0, \
		& devName, \
		FILE_DEVICE_UNKNOWN, \
		0, TRUE, \
		& pDevObj);
	if (!NT_SUCCESS(status))
	{
		if (status == STATUS_INSUFFICIENT_RESOURCES)
		{
			KdPrint(("资源不足 STATUS_INSUFFICIENT_RESOURCES"));
		}
		if (status == STATUS_OBJECT_NAME_EXISTS)
		{
			KdPrint(("指定对象名存在\n"));
		}
		if (status == STATUS_OBJECT_NAME_COLLISION)
		{
			KdPrint(("对象名有冲突\n"));
		}
		KdPrint(("设备创建失败\n"));
		return status;
	}
	//DbgPrint("设备创建成功\n");



	pDevObj->Flags |= DO_BUFFERED_IO;



	//创建符号链接

	RtlInitUnicodeString(&symLinkName, 符号名);
	status = IoCreateSymbolicLink(&symLinkName, &devName);
	if (!NT_SUCCESS(status)) /*status等于0*/
	{
		IoDeleteDevice(pDevObj);
		return status;
	}
	//KdPrint(("符号链接创建成功\n"));

	return STATUS_SUCCESS;
}


NTSTATUS DispatchControlCode(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;

	PIO_STACK_LOCATION     irpStack = NULL;
	ULONG IoControlCode;

	irpStack = IoGetCurrentIrpStackLocation(Irp);
	IoControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;


	
	switch (IoControlCode)
	{
	case IOCTL_GET_EPROCESS:
	{
		typedef struct _ALL_PROCESSES_
		{
			ULONG_PTR nSize;
			PVOID ProcessInfo;
		}ALL_PROCESSES, *PALL_PROCESSES;

		PALL_PROCESSES pinp = NULL;

		ULONG nCnt = 0;

		__try {

			pinp = Irp->AssociatedIrp.SystemBuffer; //这个地址里保存了 从三环来的内容，同时也是0环 传给三环的内容

		
			ProbeForWrite(pinp->ProcessInfo, pinp->nSize, 1);
			
			nCnt = EnumProcess((PPROCESS_INFO)pinp->ProcessInfo);

			if (nCnt>0) {

				AddIdleProcess(&((PPROCESS_INFO)pinp->ProcessInfo)[nCnt]) ? nCnt++ : nCnt;


				*(PULONG)Irp->AssociatedIrp.SystemBuffer = (ULONG)nCnt;

				ntStatus = STATUS_SUCCESS;

			}
		}
		__except (1)
		{
			
			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_GET_EPROCESS %x\n", ntStatus);
		}
		break;
	}

	case IOCTL_SUSPEND_PROCESS:
	{
		// thread->SuspendCount (0x284) 来判断线程是否被挂起
		struct input
		{
			HANDLE processid;
			
		} *pinp;

		pinp = NULL;
		PEPROCESS pTemp = NULL;
		__try
		{
			
			pinp = Irp->AssociatedIrp.SystemBuffer;
			ntStatus = PsLookupProcessByProcessId(pinp->processid, &pTemp);
			if (NT_SUCCESS(ntStatus))
			{
				
				ntStatus = PsSuspendProcess(pTemp);

				if (!NT_SUCCESS(ntStatus)) 
				{
					DbgPrint("hzw:暂停进程失败 %x \n", ntStatus);

				}

				ObDereferenceObject(pTemp);
			}
		}
		__except (1)
		{

			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_PROCESS_STATUS- %x %d\n", ntStatus, pinp->processid);
		}

		break;
	}
	case IOCTL_RESUME_PROCESS:
	{
		
		struct input
		{
			HANDLE processid;

		} *pinp;

		pinp = NULL;
		PEPROCESS pTemp = NULL;
		__try
		{
			pinp = Irp->AssociatedIrp.SystemBuffer;

			ntStatus = PsLookupProcessByProcessId(pinp->processid, &pTemp);
			if (NT_SUCCESS(ntStatus))
			{
				
				ntStatus = PsResumeProcess(pTemp);

				if (!NT_SUCCESS(ntStatus))
				{
					DbgPrint("hzw:恢复进程失败 %x \n", ntStatus);
				}

				ObDereferenceObject(pTemp);
			}
		}
		__except (1)
		{

			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_PROCESS_STATUS- %x %d\n", ntStatus, pinp->processid);
		}

		break;
	}

	case IOCTL_KISS_PROCESS:
	{

		struct input
		{
			HANDLE processid;

		} *pinp;

		pinp = NULL;
		PEPROCESS pTemp = NULL;
	
		__try
		{
			pinp = Irp->AssociatedIrp.SystemBuffer;

			ntStatus = PsLookupProcessByProcessId(pinp->processid, &pTemp);
			if (NT_SUCCESS(ntStatus))
			{
			
				KAPC_STATE kpc = { 0 };
				__try {
					KeStackAttachProcess(pTemp, &kpc);
					ntStatus = ZwTerminateProcess(NtCurrentProcess(), 0);
				}
				__finally 
				{
					KeUnstackDetachProcess(&kpc);
				}
				if (!NT_SUCCESS(ntStatus))
				{
					DbgPrint("hzw:结束进程失败 %x \n", ntStatus);
				}
				
				ObDereferenceObject(pTemp);
			}
		}
		__except (1)
		{

			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_PROCESS_STATUS- %x %d\n", ntStatus, pinp->processid);
		}

		break;
	}

	case IOCTL_FREE_MODULE:
	{

		struct input
		{
			ULONG dwPid;
			ULONG_PTR dwModuleBase;
			ULONG_PTR dwLdrpHashTable;
		} *pinp;

		pinp = NULL;
		PEPROCESS pCurProcess = NULL;


		KAPC_STATE kpc = { 0 };
		__try
		{
			pinp = Irp->AssociatedIrp.SystemBuffer;

	
			ntStatus = PsLookupProcessByProcessId((HANDLE)pinp->dwPid, &pCurProcess);
			if (NT_SUCCESS(ntStatus) && pinp->dwModuleBase > 0)
			{


				ntStatus = MmUnmapViewOfSection(pCurProcess, (PVOID)pinp->dwModuleBase);
				if (NT_SUCCESS(ntStatus))
				{
					__try {

						KeStackAttachProcess(pCurProcess, &kpc);

						if (PsGetProcessWow64Process(pCurProcess))
						{
							ULONG32 dwTemp = 0;
							RemoveDllModuleInPeb86(pCurProcess, (ULONG)pinp->dwModuleBase);

							dwTemp = GetLdrpHashTable32Ex(pinp->dwLdrpHashTable);
							if (dwTemp) {
								RemoveDllModuleInLdrpHashTable86((LIST_ENTRY32*)(dwTemp), pCurProcess, pinp->dwModuleBase);
							}
						}
						else
						{
							RemoveDllModuleInPeb64(pCurProcess, pinp->dwModuleBase);
							RemoveDllModuleInLdrpHashTable64((LIST_ENTRY *)(pinp->dwLdrpHashTable), pCurProcess, pinp->dwModuleBase);
						}
					}
					__finally {
						KeUnstackDetachProcess(&kpc);
						
					}
				}

				ObDereferenceObject(pCurProcess);

				
			}

			


		}
		__except (1)
		{

			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_FREE_MODULE- %x %d\n", ntStatus, pinp->dwPid);
		}

		break;
	}

	case IOCTL_GET_PROCESS_COMMANDLINE_BY_PEB:
	{
		struct input
		{
			HANDLE processid;
			ULONG_PTR nSize;
			WCHAR* wzCommand;
		} *pinp;

		pinp = NULL;
		PVOID pBuffer = NULL;
		__try 
		{

			pinp = Irp->AssociatedIrp.SystemBuffer;

		

			ProbeForWrite(pinp->wzCommand, pinp->nSize, 1);

			pBuffer = ExAllocatePoolWithTag(PagedPool, pinp->nSize,'hzw');
			if (pBuffer) {

				RtlZeroMemory(pBuffer, pinp->nSize);

				ntStatus =  GetCommandLineByPid(pinp->processid, pBuffer, pinp->nSize);

				if (NT_SUCCESS(ntStatus)) {
					__try {
						RtlCopyMemory(pinp->wzCommand, pBuffer, pinp->nSize);
					}
					__except (1) {

					}
				}

				ExFreePoolWithTag(pBuffer,'hzw');


			}


		}
		__except (1)
		{
			
			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_GET_PROCESS_COMMANDLINE_BY_PEB- %x %d\n", ntStatus, pinp->processid);
		}

		break;
	}

	case IOCTL_CE_ENUMDRIVERS:
	{

		struct input
		{
			ULONG nSize;
			PALL_DRIVERS pBuffer;
		} *pinp;

		pinp = NULL;
		ULONG nCnt = 0;
	
	

		__try
		{
			pinp = Irp->AssociatedIrp.SystemBuffer;

			ProbeForWrite(pinp->pBuffer, pinp->nSize, 1);

			nCnt = pinp->nSize / sizeof(ALL_DRIVERS);

			ntStatus = EnumDriverByLdrDataTableEntry(pinp->pBuffer, nCnt);
			if (NT_SUCCESS(ntStatus) && pinp->pBuffer->nCnt > 0)
			{
				ntStatus = EnumDriversByWalkerDirectoryObject(pinp->pBuffer, nCnt);
				if (!NT_SUCCESS(ntStatus)) {

					DbgPrint("hzw:get DriverObject failed %x\n", ntStatus);
				}
			}
		}
		__except (1)
		{
			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_CE_ENUMDRIVERS- %x\n", ntStatus);
		}

		break;
	}

	case IOCTL_ENUM_THREAD:
	{
		struct input
		{
			ULONG nPid;
			ULONG nSize;
			ULONG_PTR pEprocess;
			PTHREAD_INFO pBuffer;
		} *pinp;

		pinp = NULL;
		ULONG nCnt = 0;

		__try 
		{
			pinp = Irp->AssociatedIrp.SystemBuffer;
			ProbeForWrite(pinp->pBuffer, pinp->nSize, 1);


			nCnt = EnumProcessThreadByPid((HANDLE)pinp->nPid, pinp->pBuffer);

			if (nCnt > 0) 
			{

				*(PULONG)Irp->AssociatedIrp.SystemBuffer = (ULONG)nCnt;
				ntStatus = STATUS_SUCCESS;

			}
		}
		__except (1)
		{
			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_ENUM_THREAD- %x\n", ntStatus);
		}



		break;
	}


	case IOCTL_ENUM_MODULE_BYPEB:
	{
		struct input
		{
			ULONG nPid;
			ULONG nSize;
			ULONG_PTR pEprocess;
			PALL_MODULES pBuffer;
		} *pinp;

		pinp = NULL;
	

	
		__try
		{
			pinp = Irp->AssociatedIrp.SystemBuffer;

			ULONG nCount = (pinp->nSize - sizeof(ULONG)) / sizeof(MODULE_INFO_EX);

			ProbeForWrite(pinp->pBuffer, pinp->nSize, 1);


			ntStatus = EnumProcessModuleByPid((HANDLE)pinp->nPid, pinp->pBuffer,nCount);

			if ( NT_SUCCESS(ntStatus) && pinp->pBuffer->nCnt > 0)
			{
		
				*(PULONG)Irp->AssociatedIrp.SystemBuffer = (ULONG)pinp->pBuffer->nCnt;
				ntStatus = STATUS_SUCCESS;

			}
		}
		__except (1)
		{
			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_ENUM_MODULE_BYPEB- %x\n", ntStatus);
		}



		break;
	}


	case IOCTL_ENUM_VAD:
	{
		struct input
		{
			ULONG nPid;
			ULONG nSize;
			PALL_VADS pBuffer;
		} *pinp;

		pinp = NULL;


		__try
		{
			pinp = Irp->AssociatedIrp.SystemBuffer;

			ULONG nCount = (pinp->nSize - sizeof(ULONG)) / sizeof(VAD_INFO);

			ProbeForWrite(pinp->pBuffer, pinp->nSize, 1);

			if (EnumProcessVad(pinp->nPid, pinp->pBuffer, nCount)) 
			{
				ntStatus = STATUS_SUCCESS;
			}


		}
		__except (1)
		{
			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_ENUM_VAD- %x\n", ntStatus);
		}



		break;
	}

	case IOCTL_QUERYOBJECT_NAME:
	{
		struct input
		{
			ULONG_PTR nSize;
			ULONG_PTR a;
			WCHAR* pBuffer;
		} *pinp;

		pinp = NULL;

	
		__try
		{
			pinp = Irp->AssociatedIrp.SystemBuffer;
			ProbeForWrite(pinp->pBuffer, pinp->nSize, 1);
	
			POBJECT_NAME_INFORMATION szStr = NULL;
			PFILE_OBJECT FileObject = NULL;

			if (MmIsAddressValid((PVOID)pinp->a)) 
			{
				FileObject = (PFILE_OBJECT)pinp->a;

		
				ntStatus = IoQueryFileDosDeviceNameSafeIrql(FileObject, &szStr);

				//ntStatus = IoQueryFileDosDeviceName(FileObject, &szStr);
				if (NT_SUCCESS(ntStatus) && szStr)
				{
					RtlCopyMemory(pinp->pBuffer, szStr->Name.Buffer, szStr->Name.MaximumLength > pinp->nSize ? pinp->nSize : szStr->Name.MaximumLength);
					
				}
				
				if (szStr) {
					ExFreePool(szStr);
				}

			}
			else {
				ntStatus = STATUS_UNSUCCESSFUL;
			}

		
		}
		__except (1)
		{
			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_QUERYOBJECT_NAME- %x\n", ntStatus);
		}

		break;
	}

	case IOCTL_ENUM_MEMORY:
	{
		struct input
		{
			HANDLE nPid;
			ULONG_PTR pEprocess;
			ULONG_PTR nSize;
			PALL_MEMORYS pBuffer;
		} *pinp;

		pinp = NULL;

		
		__try
		{
			pinp = Irp->AssociatedIrp.SystemBuffer;
			ProbeForWrite(pinp->pBuffer, pinp->nSize, 1);
			ULONG nCount = (ULONG)((pinp->nSize - sizeof(ULONG)) / sizeof(MEMORY_INFO));
			ntStatus = GetMemorys(pinp->nPid, pinp->pBuffer, nCount);



		}
		__except (1)
		{
			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_ENUM_MEMORY- %x\n", ntStatus);
		}

		break;
	}



	case IOCTL_ENUM_HANDLES:
	{
		struct input
		{
			HANDLE nPid;
			ULONG_PTR nSize;
			PALL_HANDLES pBuffer;
		} *pinp;

		pinp = NULL;

		
		__try
		{
			pinp = Irp->AssociatedIrp.SystemBuffer;
			ProbeForWrite(pinp->pBuffer, pinp->nSize, 1);
			ULONG nCount = (ULONG)((pinp->nSize - sizeof(ULONG)) / sizeof(HANDLE_INFO));
			
			//ntStatus = EnumHands(pinp->nPid, pinp->pBuffer, nCount);
			ntStatus = EnumHandsByAPI(pinp->nPid, pinp->pBuffer, nCount);


		}
		__except (1)
		{
			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_ENUM_MEMORY- %x\n", ntStatus);
		}

		break;
	}


	case IOCTL_DUMP_BUFFER:
	{

		struct input
		{
			HANDLE dwPid;
			ULONG_PTR dwBaseAddress;
			ULONG nSize;
			PVOID pOutBuffer;
		} *pinp;

		pinp = NULL;

		PEPROCESS pCurProcess = NULL;
	


		__try
		{
			pinp = Irp->AssociatedIrp.SystemBuffer;
			ProbeForWrite(pinp->pOutBuffer, pinp->nSize, 1);
			
			ntStatus = PsLookupProcessByProcessId(pinp->dwPid, &pCurProcess);
			if (NT_SUCCESS(ntStatus)) 
			{
				
				ntStatus = SafeCopyProcessModules(pCurProcess, pinp->dwBaseAddress, pinp->nSize, pinp->pOutBuffer);
				
				ObDereferenceObject(pCurProcess);
			}

		}
		__except (1)
		{
			
			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_PROCESS_STATUS- %x %d\n", ntStatus, pinp->dwPid);
		}

		break;
	}


	case IOCTL_HIDE_MODULE:
	{

		struct input
		{
			ULONG dwPid;
			ULONG nSize;
			ULONG_PTR dwBaseAddress;
			ULONG_PTR dwLdrpHashTable;
		} *pinp;

		pinp = NULL;

		__try
		{
	
			pinp = Irp->AssociatedIrp.SystemBuffer;
		
			ntStatus = HideModule((ULONG)pinp->dwPid, pinp->dwBaseAddress, pinp->dwLdrpHashTable,pinp->nSize);


		}
		__except (1)
		{

			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_HIDE_MODULE- %x %d\n", ntStatus, pinp->dwPid);
		}

		break;
	}


	case IOCTL_CHANGGE_MEMOORY_PROTECTION:
	{

		struct input
		{
			HANDLE dwPid;
			ULONG_PTR dwBaseAddess;
			ULONG Protection;
			ULONG nSize;
		} *pinp;

		pinp = NULL;

		PEPROCESS pTemp = NULL;
		KAPC_STATE kpc = { 0 };
		PVOID pBaseAddress = NULL;
		ULONG nNewAccessProtection = 0;
		ULONG nOldAccessProtection = 0;
		SIZE_T nSize = 0;
		__try
		{
			pinp = Irp->AssociatedIrp.SystemBuffer;

			pBaseAddress = (PVOID)pinp->dwBaseAddess;
			nNewAccessProtection = pinp->Protection;
			nSize = pinp->nSize;

			ntStatus = PsLookupProcessByProcessId(pinp->dwPid, &pTemp);
			if (NT_SUCCESS(ntStatus))
			{
				__try 
				{
					KeStackAttachProcess(pTemp, &kpc);
					if (pfnZwProtectVirtualMemory)
					{
						
						ntStatus = pfnZwProtectVirtualMemory(NtCurrentProcess(), &pBaseAddress, &nSize, nNewAccessProtection, &nOldAccessProtection);
					}
					
					
				}
				__finally
				{
					KeUnstackDetachProcess(&kpc);
					
				}
				ObDereferenceObject(pTemp);
			}

			//返回 结果
			*(PULONG)Irp->AssociatedIrp.SystemBuffer = ntStatus;
			ntStatus = STATUS_SUCCESS;
		}
		__except (1)
		{

			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_HIDE_MODULE- %x %d\n", ntStatus, pinp->dwPid);
		}

		break;
	}


	case IOCTL_ALLOCATE_MEMORY:
	{



		struct input
		{
			HANDLE dwPid;
			ULONG_PTR dwBaseAddess;
			ULONG AllocationType;
			ULONG nSize;
		} *pinp;

		pinp = NULL;

		PEPROCESS pTemp = NULL;
		KAPC_STATE kpc = { 0 };
		PVOID pBaseAddress = NULL;
		SIZE_T AllocateSize = 0;

		ULONG_PTR dwRet = 0;
		

		__try
		{
			pinp = Irp->AssociatedIrp.SystemBuffer;

			pBaseAddress = (PVOID)pinp->dwBaseAddess;
			AllocateSize = pinp->nSize;

			
			ntStatus = PsLookupProcessByProcessId((HANDLE)pinp->dwPid, &pTemp);
			if (NT_SUCCESS(ntStatus))
			{
				__try
				{
					KeStackAttachProcess(pTemp, &kpc);
		
					ntStatus = ZwAllocateVirtualMemory(NtCurrentProcess(), &pBaseAddress, 0, &AllocateSize, pinp->AllocationType, PAGE_EXECUTE_READWRITE);
					if (NT_SUCCESS(ntStatus) && pinp->AllocationType != MEM_RESERVE && pBaseAddress) 
					{
						RtlZeroMemory(pBaseAddress, AllocateSize);
					}
					else
					{
						dwRet = (ULONG)ntStatus;
					}
			
					
				}
				__finally
				{
					KeUnstackDetachProcess(&kpc);
					
				}
				ObDereferenceObject(pTemp);
			}

	

			//返回 结果
		
			*(PULONG_PTR)Irp->AssociatedIrp.SystemBuffer = pBaseAddress >0? pBaseAddress: dwRet;
			ntStatus = STATUS_SUCCESS;
		
		}
		__except (1)
		{

			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_HIDE_MODULE- %x %d\n", ntStatus, pinp->dwPid);
		}

		break;
	}

	case IOCTL_FREE_MEMORY:
	{
		struct input
		{
			HANDLE dwPid;
			ULONG_PTR dwBaseAddess;
			ULONG nSize;
		} *pinp;

		pinp = NULL;

		PEPROCESS pTemp = NULL;
		KAPC_STATE kpc = { 0 };
		PVOID pBaseAddress = NULL;

		SIZE_T AllocateSize = 0;

		
		__try
		{
			pinp = Irp->AssociatedIrp.SystemBuffer;

			pBaseAddress = (PVOID)pinp->dwBaseAddess;
			AllocateSize = pinp->nSize;

			ntStatus = PsLookupProcessByProcessId((HANDLE)pinp->dwPid, &pTemp);
			if (NT_SUCCESS(ntStatus))
			{
				__try
				{
					KeStackAttachProcess(pTemp, &kpc);

					ntStatus = ZwFreeVirtualMemory(NtCurrentProcess(), &pBaseAddress, &AllocateSize, MEM_RELEASE);
				
				}
				__finally
				{
					KeUnstackDetachProcess(&kpc);
					
				}
				ObDereferenceObject(pTemp);
			}

			//返回 结果

			*(PULONG64)Irp->AssociatedIrp.SystemBuffer = ntStatus;
			ntStatus = STATUS_SUCCESS;

		}
		__except (1)
		{

			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_HIDE_MODULE- %x %d\n", ntStatus, pinp->dwPid);
		}

		break;
	}


	case IOCTL_MDL_WRITE_MEMORY:
	{
		struct input
		{
			HANDLE dwPid;
			ULONG_PTR dwBaseAddess;
			PVOID pBuffer;
			ULONG nSize;
		} *pinp;

		pinp = NULL;

		PEPROCESS pTemp = NULL;
		KAPC_STATE kpc = { 0 };
		PVOID pBaseAddress = NULL;
		SIZE_T nSize = 0;
		PVOID pBuffer = NULL;

	
		__try
		{
			pinp = Irp->AssociatedIrp.SystemBuffer;

			pBaseAddress = (PVOID)pinp->dwBaseAddess;
			nSize = pinp->nSize;

			//先判断能不能 读
			ProbeForRead(pinp->pBuffer, nSize, 1);

			pBuffer = RtlAllocateMemory(TRUE, nSize);
			if(!pBuffer) break;

			RtlZeroMemory(pBuffer, nSize);
			__try 
			{
				RtlCopyMemory(pBuffer, pinp->pBuffer, nSize);
			}
			__except (1) 
			{
				if (pBuffer) {
					ExFreePool(pBuffer);
					pBuffer = NULL;
				}
				DbgPrint("hzw:拷贝内存failed- %x %d\n", ntStatus, pinp->dwPid);
			}


			ntStatus = PsLookupProcessByProcessId((HANDLE)pinp->dwPid, &pTemp);
			if (NT_SUCCESS(ntStatus))
			{
				__try
				{
					KeStackAttachProcess(pTemp, &kpc);

					ntStatus = RtlSuperCopyMemory(pBaseAddress, pBuffer, nSize);

				}
				__finally
				{
					KeUnstackDetachProcess(&kpc);
				
				}
				ObDereferenceObject(pTemp);
			}


		}
		__except (1)
		{

			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_HIDE_MODULE- %x %d\n", ntStatus, pinp->dwPid);
		}

		if (pBuffer) {
			RtlFreeMemory(pBuffer);
			pBuffer = NULL;
		}


		break;
	}

	case IOCTL_MDL_CHANGE_VAD_PROTECTION:
	{
		
		struct input
		{
			HANDLE dwPid;
			ULONG_PTR dwBaseAddess;
			ULONG_PTR dwVadProtection;
		} *pinp;

		pinp = NULL;

		PEPROCESS pTemp = NULL;
		

		ULONG_PTR dwBaseAddress = 0;
		ULONG_PTR dwVadProtection = 0;

		__try
		{
			pinp = Irp->AssociatedIrp.SystemBuffer;

			dwBaseAddress = pinp->dwBaseAddess;
			dwVadProtection = pinp->dwVadProtection;

			ntStatus = PsLookupProcessByProcessId((HANDLE)pinp->dwPid, &pTemp);
			if (NT_SUCCESS(ntStatus))
			{

				if (IsRealProcess(pTemp))
				{
					PMMVAD pVad = NULL;
					pVad = MiLocateAddress(pTemp, (PVOID)dwBaseAddress);

					if (MmIsAddressValid(pVad) && MmIsAddressValid(&pVad->Core.u1.Flags.flag)) 
					{
						ntStatus = RtlSuperCopyMemoryEx(&pVad->Core.u1.Flags.flag, &dwVadProtection, sizeof(ULONG_PTR));
					}

				}
				ObDereferenceObject(pTemp);
			}

		}
		__except (1)
		{

			ntStatus = GetExceptionCode();
			DbgPrint("hzw:IOCTL_HIDE_MODULE- %x %d\n", ntStatus, pinp->dwPid);
		}

		break;
	}


	case IOCTL_CREATETHREAD_EX:
	{
		struct input
		{
			HANDLE dwPid;
			PVOID64 addresstoexecute;
			PVOID64 lparam;
		} *pinp;


		pinp = NULL;
		PEPROCESS pCurentProcess = NULL;
		KAPC_STATE kpc = { 0 };
		pinp = Irp->AssociatedIrp.SystemBuffer;

		if (!ZwCreateThreadEx)break;

		ntStatus = PsLookupProcessByProcessId(pinp->dwPid, &pCurentProcess);
		if (NT_SUCCESS(ntStatus))
		{

			__try
			{
				KeStackAttachProcess(pCurentProcess, &kpc);

				HANDLE hThread = NULL;

				ntStatus = ZwCreateThreadEx(&hThread, THREAD_ALL_ACCESS, NULL,
					NtCurrentProcess(), (PTHREAD_START_ROUTINE)pinp->addresstoexecute, pinp->lparam, 0, 0, 0, 0, NULL);

				if (NT_SUCCESS(ntStatus) && hThread) {
					ZwClose(hThread);
				}
				
			}
			__finally
			{
				KeUnstackDetachProcess(&kpc);

			}
			ObDereferenceObject(pCurentProcess);
			
		}

		*(PULONG)Irp->AssociatedIrp.SystemBuffer = ntStatus;
		ntStatus = STATUS_SUCCESS;

		break;
	}


	case IOCTL_OPEN_THREAD:
	{
		struct input
		{
			HANDLE dwThreadId;
		} *pinp;
		pinp = NULL;

		pinp = Irp->AssociatedIrp.SystemBuffer;

		HANDLE hThread = 0;
		PETHREAD pThread = NULL;
		ntStatus = PsLookupThreadByThreadId(pinp->dwThreadId, &pThread);
		if (NT_SUCCESS(ntStatus)) {


			ntStatus = ObOpenObjectByPointer(pThread, 0, NULL, GENERIC_ALL, *PsThreadType, KernelMode, &hThread);

			ObDereferenceObject(pThread);
		}

		*(PHANDLE)Irp->AssociatedIrp.SystemBuffer = hThread;
		
		break;
	}

	case IOCTL_ENABLE_KERNEL_INJECT:
	{
		if (!g_InitLoadImage) 
		{
			ntStatus = PsSetLoadImageNotifyRoutine(LoadImageNotifyCallback);
			if (NT_SUCCESS(ntStatus)) {
				DbgPrint("PsSetLoadImageNotifyRoutine ok!\r\n");
				g_InitLoadImage = TRUE;
			}
		}
	
		break;
	}

	case IOCTL_SET_KERNEL_INJECT:
	{
		struct input
		{
			char *pProcessName;
			WCHAR* pDllPath;
			ULONG dwStats;
		} *pinp;

		pinp = NULL;
		pinp = Irp->AssociatedIrp.SystemBuffer;
	

		if (g_InitLoadImage) 
		{
			
			__try
			{
				ProbeForRead(pinp->pProcessName, 50, 1);
				ProbeForRead(pinp->pDllPath, 256 * 2, 1);
			}
			__except (1) {
				break;
			}

			g_pInjectInfo = (PINJECT_INFO)RtlAllocateMemory(TRUE, sizeof(INJECT_INFO));
			if (!g_pInjectInfo)break;

			RtlCopyMemory(g_pInjectInfo->szProcessName, pinp->pProcessName, 50);
			RtlCopyMemory(g_pInjectInfo->szDllPath, pinp->pDllPath, 256*2);

			RtlInitUnicodeString(&m_GlobalInjectDllPath64, g_pInjectInfo->szDllPath);
			RtlInitUnicodeString(&m_GlobalInjectDllPath32, g_pInjectInfo->szDllPath);
			RtlInitAnsiString(&m_GlobalProcessName, g_pInjectInfo->szProcessName);
		
			g_bHideInject = pinp->dwStats;

			ntStatus = STATUS_SUCCESS;
		}
		
		break;
	}

	case IOCTL_UNLOAD_DRIVER:
	{
		struct input
		{
			PDRIVER_OBJECT pDriverObject;
		} *pinp;

		pinp = NULL;
		pinp = Irp->AssociatedIrp.SystemBuffer;

		if (!pinp->pDriverObject)break;

		ntStatus = UnloadDriver(pinp->pDriverObject);
		break;
	}

	case IOCTL_ENUM_CALLBACK:
	{
		struct input
		{
			PCALL_BACK  pAddress;
		} *pinp;

		pinp = NULL;
		pinp = Irp->AssociatedIrp.SystemBuffer;

		__try 
		{
			ProbeForWrite(pinp->pAddress, sizeof(CALL_BACK),1);

		
			EnumCreateProcessNotify(pinp->pAddress->PspCreateProcessNotifyRoutine, 65);
			EnumCreateThreadNotify(pinp->pAddress->PspCreateThreadNotifyRoutine, 65);
			EnumLoadImageNotify(pinp->pAddress->PspLoadImageNotifyRoutine, 65);
			CountCmpCallbackAfterXP(pinp->pAddress->CmCallbackListHead, 65);
			EnumObCallbacks(pinp->pAddress->obRegister, 65);

			ntStatus = STATUS_SUCCESS;
		}
		__except (1) {
			ntStatus = GetExceptionCode();
		}


		break;
	}



	default:
		break;
	}



	Irp->IoStatus.Status = ntStatus; //三环通过 getlasterror() 得到的就是这个值

	// Set # of bytes to copy back to user-mode...
	if (irpStack) //only NULL when loaded by dbvm
	{
		if (ntStatus == STATUS_SUCCESS)
			Irp->IoStatus.Information = irpStack->Parameters.DeviceIoControl.OutputBufferLength; //返回给3环多少数据
		else
			Irp->IoStatus.Information = 0;

		IoCompleteRequest(Irp, IO_NO_INCREMENT);
	}
	return ntStatus;
}