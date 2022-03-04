#include "peb.h"

ULONG_PTR Kthread_offset_ContextSwitches = 0;
ULONG_PTR Kthread_offset_state = 0;
ULONG_PTR Kthread_offset_Win32StartAddress = 0;
ULONG_PTR Kthread_offset_SuspendCount = 0;

ULONG_PTR enumObjectTableOffset_EPROCESS = 0;
ULONG_PTR eprocess_offset_VadHint = 0;
ULONG_PTR eprocess_offset_VadRoot = 0;
ULONG g_ObjectCallbackListOffset;
extern PSHORT NtBuildNumber;

//支持系统
BOOLEAN initVerSion()
{
	
	BOOLEAN bRet = FALSE;

	SHORT dwVersion = 0;
	__try {
		dwVersion = *NtBuildNumber;
	}
	__except (1)
	{
		return FALSE;
	}

	switch (dwVersion)
	{
	case 0x47BA:
	case 0x47BB:
	{
		Kthread_offset_ContextSwitches = 0x154;
		Kthread_offset_state = 0x184;
		Kthread_offset_Win32StartAddress = 0x6a0;
		Kthread_offset_SuspendCount = 0x284;
		enumObjectTableOffset_EPROCESS = 0x418; /*句柄表偏移 位置*/
		eprocess_offset_VadRoot = 0x658;
		eprocess_offset_VadHint = 0x660;
		g_ObjectCallbackListOffset = 0xc8;
		bRet = TRUE;
	}
		break;

	default:
		break;
	}




	return bRet;
}
