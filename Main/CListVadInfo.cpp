#include "CListVadInfo.h"
extern CLoadDriver g_connectDriver;

CListVadInfo::CListVadInfo()
{
}

CListVadInfo::~CListVadInfo()
{
}

BOOL CListVadInfo::EnumProcessVad(ULONG nPid, vector<VAD_INFO>& vectorVads)
{
	BOOL bRet = FALSE;

	typedef struct _INPUT
	{
		ULONG nPid;
		ULONG nSize;
		PALL_VADS pBuffer;
	}Input;

	Input inputs = { 0 };

	if (nPid == 0 )
	{
		return bRet;
	}
	vectorVads.clear();

	inputs.nPid = nPid;

	inputs.nSize = sizeof(VAD_INFO) * 0x5000 + sizeof(ULONG); //默认有1000个线程

	inputs.pBuffer = (PALL_VADS)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, inputs.nSize);

	if (inputs.pBuffer)
	{

		bRet = g_connectDriver.DeviceControl(IOCTL_ENUM_VAD, &inputs, sizeof(Input), 0, 0, 0);


		if (bRet && inputs.pBuffer->nCnt > 0)
		{
			for (ULONG i = 0; i < inputs.pBuffer->nCnt; i++)
			{
				vectorVads.push_back(inputs.pBuffer->VadInfos[i]);
			}
		}

		bRet = HeapFree(GetProcessHeap(), 0, inputs.pBuffer);
	}



	return bRet;
}

CString CListVadInfo::ObQueryNameString(ULONG_PTR a)
{
	CString szTemp = L" ";
	BOOL bRet = FALSE;

	if (!a)return szTemp;

	typedef struct _INPUT
	{
		ULONG_PTR nSize;
		ULONG_PTR a;
		WCHAR* pBuffer;
	}Input;

	Input inputs = { 0 };

	inputs.a = a;

	inputs.nSize = 0x1000; 

	inputs.pBuffer = (WCHAR*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, inputs.nSize);

	if (inputs.pBuffer)
	{

		bRet = g_connectDriver.DeviceControl(IOCTL_QUERYOBJECT_NAME, &inputs, sizeof(Input), 0, 0, 0);

		if (bRet)
		{
			szTemp = inputs.pBuffer;
		}

		bRet = HeapFree(GetProcessHeap(), 0, inputs.pBuffer);
	}


	return szTemp;
}

BOOL CListVadInfo::changeVadProtect(ULONG dwPid, ULONG_PTR dwBaseAddress, ULONG_PTR dwVadProtection)
{
	BOOL bRet = FALSE;

	if (!dwPid || !dwBaseAddress || !dwVadProtection)return bRet;
	typedef struct _INPUT
	{
		HANDLE dwPid;
		ULONG_PTR dwBaseAddess;
		ULONG_PTR dwVadProtection;
	}Input;

	Input inputs = { 0 };


	inputs.dwPid = (HANDLE)dwPid;
	inputs.dwVadProtection = dwVadProtection;
	inputs.dwBaseAddess = dwBaseAddress;
	

	bRet = g_connectDriver.DeviceControl(IOCTL_MDL_CHANGE_VAD_PROTECTION, &inputs, sizeof(Input), NULL, 0, 0);

	return bRet;
}
