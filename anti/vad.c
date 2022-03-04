#include "vad.h"
#include "exapi.h"


PMMVAD MiLocateAddress( IN PEPROCESS pEprocess, IN PVOID VirtualAddress)
{

	if (!eprocess_offset_VadHint || !eprocess_offset_VadRoot || !MmIsAddressValid(pEprocess)) {
		return NULL;
	}

	PMMVAD VadHint = (PMMVAD)(*(PUINT64)((UINT64)pEprocess + eprocess_offset_VadHint));
	PMMVAD VadRoot = (PMMVAD)(*(PUINT64)((UINT64)pEprocess + eprocess_offset_VadRoot));
	if (!MmIsAddressValid(VadHint) || !MmIsAddressValid(VadRoot))
	{
		return NULL;
	}

	ULONG64 Vpn = (ULONG64)((UINT64)VirtualAddress >> PAGE_SHIFT);
	//如果VPN在VADHINT范围内，直接返回VadHint

	ULONG64 x = VadHint->Core.StartingVpnHigh;
	x = x << 32;
	ULONG64 y = VadHint->Core.EndingVpnHigh;
	 y = y << 32;


	if ((Vpn >= (x | (ULONG64)VadHint->Core.StartingVpn)) && (Vpn <= (y | (ULONG64)VadHint->Core.EndingVpn)))
		return VadHint;



	while (TRUE)
	{

		if (!MmIsAddressValid(VadRoot)) return 0;

		ULONG64 a = VadRoot->Core.EndingVpnHigh;
		a = a << 32;

		ULONG64 c = VadRoot->Core.StartingVpnHigh;
		c = c << 32;



		if (Vpn > (a | (ULONG64)VadRoot->Core.EndingVpn))
		{
			VadRoot = (PMMVAD)VadRoot->Core.VadNode.Right;
		}
		else
		{
			if (Vpn >= (c | (ULONG64)VadRoot->Core.StartingVpn))
			{

				return VadRoot;
			}
			VadRoot = (PMMVAD)VadRoot->Core.VadNode.Left;
		}

	}
	return NULL;
}




VOID EnumVad(PMMVAD Root, PALL_VADS pBuffer, ULONG nCnt)
{

	

	if (!Root || !pBuffer || !nCnt) {
		return;
	}

	__try
	{
		if (nCnt > pBuffer->nCnt) 
		{
			ULONG64 a = (ULONG64)Root->Core.EndingVpnHigh;
			a = a << 32;
			ULONG64 b = (ULONG64)Root->Core.StartingVpnHigh;
			b = b << 32;

			pBuffer->VadInfos[pBuffer->nCnt].pVad = (ULONG_PTR)Root;



			pBuffer->VadInfos[pBuffer->nCnt].startVpn = (b | Root->Core.StartingVpn) << PAGE_SHIFT;
			pBuffer->VadInfos[pBuffer->nCnt].endVpn = ((a|Root->Core.EndingVpn) << PAGE_SHIFT)+0xfff;
			pBuffer->VadInfos[pBuffer->nCnt].flags = Root->Core.u1.Flags.flag;

			if (MmIsAddressValid(Root->Subsection) && MmIsAddressValid(Root->Subsection->ControlArea)) 
			{
				if (MmIsAddressValid((PVOID)((Root->Subsection->ControlArea->FilePointer.Value >> 4) << 4))) 
				{
					pBuffer->VadInfos[pBuffer->nCnt].pFileObject = ((Root->Subsection->ControlArea->FilePointer.Value >> 4) << 4);
				}
			}
			pBuffer->nCnt++;
		}
	
		if (MmIsAddressValid(Root->Core.VadNode.Left)) 
		{
			EnumVad((PMMVAD)Root->Core.VadNode.Left,pBuffer,nCnt);
		}

		if (MmIsAddressValid(Root->Core.VadNode.Right)) 
		{
			EnumVad((PMMVAD)Root->Core.VadNode.Right,pBuffer,nCnt);
		}
	}
	__except (1)
	{
		DbgPrint("hzw:EnumVad异常！！");
	}
}



BOOLEAN EnumProcessVad(ULONG Pid, PALL_VADS pBuffer,ULONG nCnt)
{
	PEPROCESS Peprocess = 0;
	PRTL_AVL_TREE Table = NULL;
	PMMVAD Root = NULL;
	if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)Pid, &Peprocess)))
	{
		Table = (PRTL_AVL_TREE)((UCHAR*)Peprocess + eprocess_offset_VadRoot);
		if (!MmIsAddressValid(Table) || !eprocess_offset_VadRoot) {
			return FALSE;
		}

		__try {

			//第一个节点 就是这个地址
			Root = (PMMVAD)Table->Root;
			if (nCnt > pBuffer->nCnt)
			{
				ULONG64 a = (ULONG64)Root->Core.EndingVpnHigh;
				a = a << 32;
				ULONG64 b = (ULONG64)Root->Core.StartingVpnHigh;
				b = b << 32;

				pBuffer->VadInfos[pBuffer->nCnt].pVad = (ULONG_PTR)Root;
				pBuffer->VadInfos[pBuffer->nCnt].startVpn = (b | Root->Core.StartingVpn) << PAGE_SHIFT;
				pBuffer->VadInfos[pBuffer->nCnt].endVpn = (a | Root->Core.EndingVpn) << PAGE_SHIFT;
				pBuffer->VadInfos[pBuffer->nCnt].flags = Root->Core.u1.Flags.flag;

				if (MmIsAddressValid(Root->Subsection) && MmIsAddressValid(Root->Subsection->ControlArea))
				{
					if (MmIsAddressValid((PVOID)((Root->Subsection->ControlArea->FilePointer.Value >> 4) << 4)))
					{
						pBuffer->VadInfos[pBuffer->nCnt].pFileObject = ((Root->Subsection->ControlArea->FilePointer.Value >> 4) << 4);
					}
				}
				pBuffer->nCnt++;
			}

			
			if (Table->Root->Left)
				EnumVad((MMVAD*)Table->Root->Left, pBuffer,nCnt);

			if (Table->Root->Right)
				EnumVad((MMVAD*)Table->Root->Right,pBuffer,nCnt);
		}
		__finally 
		{
			ObDereferenceObject(Peprocess);
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}
