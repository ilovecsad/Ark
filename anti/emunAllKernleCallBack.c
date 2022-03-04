#include "emunAllKernleCallBack.h"

extern ULONG g_ObjectCallbackListOffset;
ULONG64 SearchPspLoadImageNotifyRoutine()
{
	ULONG64 pTemp = 0;
	ULONG64 pCheckArea = NULL;
	ULONG64 i = 0;
	UNICODE_STRING szPsRemoveLoadImageNotifyRoutine = RTL_CONSTANT_STRING(L"PsRemoveLoadImageNotifyRoutine");
	pCheckArea = (ULONG64)MmGetSystemRoutineAddress(&szPsRemoveLoadImageNotifyRoutine);
	if (pCheckArea && MmIsAddressValid((PVOID)pCheckArea) && MmIsAddressValid((PVOID)(pCheckArea + 0xff)))
	{
		for (i = pCheckArea; i < pCheckArea + 0xff; i++)
		{
			__try
			{

				if ((*(PUCHAR)i == 0x48) && (*(PUCHAR)(i + 1) == 0x8d) && (*(PUCHAR)(i + 2) == 0x0d))
				{
					LONG OffsetAddr = 0;
					RtlCopyMemory(&OffsetAddr, (PUCHAR)(i + 3), 4);
					pTemp = OffsetAddr + 7 + i;
					return pTemp;
				}
			}
			__except (1)
			{
				pTemp = NULL;
				break;
			}
		}
	}

	return pTemp;
}


ULONG64 SearchPspCreateThreadNotifyRoutine()
{

	ULONG64 pTemp = 0;
	ULONG64 pCheckArea = NULL;
	ULONG64 i = 0;
	UNICODE_STRING szPsRemoveLoadImageNotifyRoutine = RTL_CONSTANT_STRING(L"PsRemoveCreateThreadNotifyRoutine");
	pCheckArea = (ULONG64)MmGetSystemRoutineAddress(&szPsRemoveLoadImageNotifyRoutine);
	if (pCheckArea && MmIsAddressValid((PVOID)pCheckArea) && MmIsAddressValid((PVOID)(pCheckArea + 0xff)))
	{
		for (i = pCheckArea; i < pCheckArea + 0xff; i++)
		{
			__try
			{

				if ((*(PUCHAR)i == 0x48) && (*(PUCHAR)(i + 1) == 0x8d) && (*(PUCHAR)(i + 2) == 0x0d))
				{
					LONG OffsetAddr = 0;
					RtlCopyMemory(&OffsetAddr, (PUCHAR)(i + 3), 4);
					pTemp = OffsetAddr + 7 + i;
					return pTemp;
				}
			}
			__except (1)
			{
				pTemp = NULL;
				break;
			}
		}
	}

	return pTemp;
}





//CmUnRegisterCallback
ULONG64 SearchCallbackListHead()
{
	ULONG64 pTemp = 0;
	ULONG64 pCheckArea = NULL;
	ULONG64 i = 0;
	UNICODE_STRING szPsRemoveLoadImageNotifyRoutine = RTL_CONSTANT_STRING(L"CmUnRegisterCallback");
	pCheckArea = (ULONG64)MmGetSystemRoutineAddress(&szPsRemoveLoadImageNotifyRoutine);
	if (pCheckArea && MmIsAddressValid((PVOID)pCheckArea) && MmIsAddressValid((PVOID)(pCheckArea + 0xff)))
	{
		for (i = pCheckArea; i < pCheckArea + 0xff; i++)
		{
			__try
			{

				if ((*(PUCHAR)i == 0x48) && (*(PUCHAR)(i + 1) == 0x8d) && (*(PUCHAR)(i + 2) == 0x0d))
				{
					LONG OffsetAddr = 0;
					RtlCopyMemory(&OffsetAddr, (PUCHAR)(i + 3), 4);
					pTemp = OffsetAddr + 7 + i;
					return pTemp;
				}
			}
			__except (1)
			{
				pTemp = NULL;
				break;
			}
		}
	}

	return pTemp;
}



BOOLEAN EnumObCallbacks(POB_CALLBACK p, ULONG nSize)
{
	ULONG i = 0;
	PLIST_ENTRY CurrEntry = NULL;
	POB_CALLBACK pObCallback = NULL;
	ULONG64 ObProcessCallbackListHead = *(ULONG64*)PsProcessType + g_ObjectCallbackListOffset;
	ULONG64 ObThreadCallbackListHead = *(ULONG64*)PsThreadType + g_ObjectCallbackListOffset;
	//
	if (!MmIsAddressValid((PVOID)ObProcessCallbackListHead) && !MmIsAddressValid((PVOID)ObThreadCallbackListHead)
		&& !MmIsAddressValid((*(PULONG64)(ObProcessCallbackListHead)) && !MmIsAddressValid((PVOID)(*(PULONG64)(ObThreadCallbackListHead)))))
	{
		return NULL;
	}

	DbgPrint("hzw:ObProcessCallbackListHead: %p\n", ObProcessCallbackListHead);
	CurrEntry = ((PLIST_ENTRY)ObProcessCallbackListHead)->Flink;	//list_head的数据是垃圾数据，忽略


	do
	{

		pObCallback = (POB_CALLBACK)CurrEntry;
		if (pObCallback->ObHandle != 0)
		{
			DbgPrint("hzw:ObHandle: %p\n", pObCallback->ObHandle);
			DbgPrint("hzw:PreCall: %p\n", pObCallback->PreCall);
			DbgPrint("hzw:PostCall: %p\n", pObCallback->PostCall);

			if (i > nSize)break;

			if (MmIsAddressValid(pObCallback->ObjTypeAddr)) 
			{
				p[i].ObHandle = pObCallback->ObHandle;
				p[i].PreCall = pObCallback->PreCall;
				p[i].PostCall = pObCallback->PostCall;
				p[i].ObjTypeAddr = pObCallback->ObjTypeAddr;
				p[i].Unknown = pObCallback->Unknown;
				p[i].bType = TRUE;
				i++;
			}
		}
		CurrEntry = CurrEntry->Flink;
	} while (CurrEntry != (PLIST_ENTRY)ObProcessCallbackListHead);
	//
	DbgPrint("hzw:ObThreadCallbackListHead: %p\n", ObThreadCallbackListHead);
	
	CurrEntry = ((PLIST_ENTRY)ObThreadCallbackListHead)->Flink;	//list_head的数据是垃圾数据，忽略
	do
	{
		pObCallback = (POB_CALLBACK)CurrEntry;
		if (pObCallback->ObHandle != 0)
		{
			DbgPrint("hzw:ObHandle: %p\n", pObCallback->ObHandle);
			DbgPrint("hzw:PreCall: %p\n", pObCallback->PreCall);
			DbgPrint("hzw:PostCall: %p\n", pObCallback->PostCall);

			if (i > nSize)break;
			if (MmIsAddressValid(pObCallback->ObjTypeAddr)) 
			{
				p[i].ObHandle = pObCallback->ObHandle;
				p[i].PreCall = pObCallback->PreCall;
				p[i].PostCall = pObCallback->PostCall;
				p[i].ObjTypeAddr = pObCallback->ObjTypeAddr;
				p[i].Unknown = pObCallback->Unknown;
				p[i].bType = FALSE;
				i++;
			}

		}
		CurrEntry = CurrEntry->Flink;
	} while (CurrEntry != (PLIST_ENTRY)ObThreadCallbackListHead);

	return TRUE;
}


ULONG64 FindPspSetCreateProcessNotifyRoutine(ULONG64 pCheckArea)
{
	ULONG64 i = 0;
	if (MmIsAddressValid(pCheckArea) && MmIsAddressValid(pCheckArea + 0xff))
	{
		for (i = pCheckArea; i < pCheckArea + 0xff; i++)
		{
			if (*(PUCHAR)i == 0x4c && *(PUCHAR)(i + 1) == 0x8d && *(PUCHAR)(i + 2) == 0x2d)	//lea     r13,[nt!PspCreateProcessNotifyRoutine 
			{
				LONG OffsetAddr = 0;
				memcpy(&OffsetAddr, (PUCHAR)(i + 3), 4);
				return OffsetAddr + 7 + i;
			}
		}
	}
	return NULL;
}


ULONG64 FindPspCreateProcessNotifyRoutine()
{
	LONG	OffsetAddr = 0;
	ULONG64	i = 0, pCheckArea = 0;
	UNICODE_STRING	unstrFunc;
	PVOID pTemp = NULL;
	//获得PsSetCreateProcessNotifyRoutine的地址
	RtlInitUnicodeString(&unstrFunc, L"PsSetCreateProcessNotifyRoutine");
	pCheckArea = (ULONG64)MmGetSystemRoutineAddress(&unstrFunc);
	if (pCheckArea)
	{

		//获得PspCreateProcessNotifyRoutine的地址
		if (MmIsAddressValid(pCheckArea) && MmIsAddressValid(pCheckArea+0xff))
		{
			for (i = pCheckArea; i < pCheckArea + 0xff; i++)
			{
				if (*(PUCHAR)i == 0xe8)	//lea r14,xxxx
				{
					LONG OffsetAddr = 0;
					memcpy(&OffsetAddr, (PUCHAR)(i + 1), 4);
					
					pTemp = OffsetAddr + 5 + i;
					DbgPrint("hzw:PspSetCreateProcessNotifyRoutine %llx", pTemp);

					return FindPspSetCreateProcessNotifyRoutine(pTemp);
				}
			}
		}
	}
	return 0;
}


void EnumCreateProcessNotify(PVOID* pBuffer,ULONG nSize)
{
	int i = 0;
	BOOLEAN b;
	ULONG64 NotifyAddr = 0, MagicPtr = 0;
	ULONG64 PspCreateProcessNotifyRoutine = FindPspCreateProcessNotifyRoutine();
	DbgPrint("hzw:PspCreateProcessNotifyRoutine: %llx", PspCreateProcessNotifyRoutine);
	if (!PspCreateProcessNotifyRoutine)
		return;
	for (i = 0; i < 64; i++)
	{
		MagicPtr = PspCreateProcessNotifyRoutine + i * 8;
		NotifyAddr = *(PULONG64)(MagicPtr);
		if (MmIsAddressValid((PVOID)NotifyAddr) && NotifyAddr != 0)
		{
			
			NotifyAddr = *(PULONG64)(NotifyAddr & 0xfffffffffffffff8);

			if (MmIsAddressValid(NotifyAddr)) {
				pBuffer[i] = NotifyAddr;
			}
		}

		if (i > nSize)break;
	}
}



void EnumCreateThreadNotify(PVOID* pBuffer, ULONG nSize)
{
	int i = 0;
	BOOLEAN b;
	ULONG64 NotifyAddr = 0, MagicPtr = 0;
	ULONG64 PspCreateThreadNotifyRoutine = SearchPspCreateThreadNotifyRoutine();
	DbgPrint("hzw:PspCreateThreadNotifyRoutine: %llx", PspCreateThreadNotifyRoutine);
	if (!PspCreateThreadNotifyRoutine)
		return;
	for (i = 0; i < 64; i++)
	{
		MagicPtr = PspCreateThreadNotifyRoutine + i * 8;
		NotifyAddr = *(PULONG64)(MagicPtr);
		if (MmIsAddressValid((PVOID)NotifyAddr) && NotifyAddr != 0)
		{

			NotifyAddr = *(PULONG64)(NotifyAddr & 0xfffffffffffffff8);

			if (MmIsAddressValid(NotifyAddr)) {
				pBuffer[i] = NotifyAddr;
			}
		}

		if (i > nSize)break;
	}
}


void EnumLoadImageNotify(PVOID* pBuffer, ULONG nSize)
{
	int i = 0;
	BOOLEAN b;
	ULONG64 NotifyAddr = 0, MagicPtr = 0;
	ULONG64 PspLoadImageNotifyRoutine = SearchPspLoadImageNotifyRoutine();
	DbgPrint("hzw:PspLoadImageNotifyRoutine: %llx", PspLoadImageNotifyRoutine);
	if (!PspLoadImageNotifyRoutine)
		return;

	for (i = 0; i < 64; i++)
	{
		MagicPtr = PspLoadImageNotifyRoutine + i * 8;
		NotifyAddr = *(PULONG64)(MagicPtr);
		if ( MmIsAddressValid((PVOID)NotifyAddr) && NotifyAddr != 0)
		{

			NotifyAddr = *(PULONG64)(NotifyAddr & 0xfffffffffffffff8);

			if (MmIsAddressValid(NotifyAddr)) {
				pBuffer[i] = NotifyAddr;
			}
		}

		if (i > nSize)break;
	}
}


ULONG CountCmpCallbackAfterXP(PVOID* pBuffer,ULONG nSize)
{
	ULONG sum = 0;
	ULONG64 dwNotifyItemAddr = 0;;
	ULONG64* pNotifyFun = NULL;
	ULONG64* baseNotifyAddr = NULL;
	ULONG64 dwNotifyFun = 0;
	LARGE_INTEGER cmpCookie = {0};
	PLIST_ENTRY notifyList = NULL;
	PCM_NOTIFY_ENTRY notify = NULL;
	dwNotifyItemAddr = SearchCallbackListHead();
	notifyList = (LIST_ENTRY *)dwNotifyItemAddr;

	if (!MmIsAddressValid(notifyList)) return NULL;

	do
	{
		notify = (CM_NOTIFY_ENTRY *)notifyList;
		if (MmIsAddressValid(notify))
		{
			if (MmIsAddressValid((PVOID)(notify->Function)) && notify->Function >
				0x8000000000000000)
			{
				DbgPrint("hzw:[CmCallback]Function=%p\tCookie=%p",
					(PVOID)(notify->Function), (PVOID)(notify->Cookie.QuadPart));
			
				if (sum > 0) {
					if (MmIsAddressValid((PVOID)(notify->Function))) {
						pBuffer[sum] = (PVOID)(notify->Function);
					}
					if (nSize < sum)break;
				}

				sum++;
			}
		}
			notifyList = notifyList->Flink;
	} while (notifyList != ((LIST_ENTRY*)(dwNotifyItemAddr)));
	return sum;
}



