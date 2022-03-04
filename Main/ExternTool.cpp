#include "ExternTool.h"
#include "CLoadDriver.h"
extern CLoadDriver g_connectDriver;

BOOL CExternTool::SetLoadImageNotifyRoutine()
{
	BOOL bRet = FALSE;

	bRet = g_connectDriver.DeviceControl(IOCTL_ENABLE_KERNEL_INJECT, 0,0, NULL, 0, 0);

	return bRet;
}

BOOL CExternTool::StatrInject(PVOID pProcessName,PVOID pDllPath,ULONG dwStats)
{
	BOOL bRet = FALSE;

	typedef struct _INPUT
	{
		char* arg1;
		WCHAR*arg2;
		ULONG dwStats;
	}Input;

	Input inputs = { 0 };
	inputs.arg1 = (char*)pProcessName;
	inputs.arg2 = (WCHAR*)pDllPath;
	inputs.dwStats = dwStats;

	bRet = g_connectDriver.DeviceControl(IOCTL_SET_KERNEL_INJECT, &inputs, sizeof(Input), NULL, 0, 0);

	return bRet;
}
