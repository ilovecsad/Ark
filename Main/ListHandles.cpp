#include "ListHandles.h"
extern CLoadDriver g_connectDriver;

CListHandles::CListHandles()
{
}

CListHandles::~CListHandles()
{
}

BOOL CListHandles::GetHandles(ULONG nPid,  vector<HANDLE_INFO>& vectorHandles)
{
	BOOL bRet = FALSE;

	typedef struct _INPUT
	{
		HANDLE nPid;
		ULONG_PTR nSize;
		PALL_HANDLES pBuffer;
	}Input;

	Input inputs = { 0 };

	if (nPid == 0)
	{
		return bRet;
	}
	vectorHandles.clear();

	inputs.nPid = (HANDLE)nPid;

	inputs.nSize = sizeof(HANDLE_INFO) * 0x1000 + sizeof(ULONG);

	inputs.pBuffer = (PALL_HANDLES)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, inputs.nSize);


	if (inputs.pBuffer)
	{

		bRet = g_connectDriver.DeviceControl(IOCTL_ENUM_HANDLES, &inputs, sizeof(Input), 0, 0, 0);


		if (bRet && inputs.pBuffer->nCnt > 0)
		{
			for (ULONG i = 0; i < inputs.pBuffer->nCnt; i++)
			{

				vectorHandles.push_back(inputs.pBuffer->Handles[i]);
			}
		}

		bRet = HeapFree(GetProcessHeap(), 0, inputs.pBuffer);
	}

	return bRet;
}

BOOL CListHandles::CloseHandle(ULONG nPid, BOOL bForce, ULONG hHandle, ULONG pObject)
{
	return 0;
}
