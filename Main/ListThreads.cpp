#include "ListThreads.h"

#include <TlHelp32.h>
extern CLoadDriver g_connectDriver;
CListThreads::CListThreads()
{
}

CListThreads::~CListThreads()
{

}




//
// 枚举线程
//
BOOL CListThreads::ListThreads(ULONG nPid, ULONG_PTR pEprocess, vector<THREAD_INFO>& vectorThreads)
{
	BOOL bRet = FALSE;

	typedef struct _INPUT
	{
		ULONG nPid;
		ULONG nSize;
		ULONG_PTR pEprocess;
		PTHREAD_INFO pBuffer;
	}Input;

	Input inputs = { 0 };

	if (nPid == 0 && pEprocess == 0)
	{
		return bRet;
	}
	vectorThreads.clear();

	ULONG nRetLength = 0;
	inputs.nPid = nPid;
	inputs.pEprocess = pEprocess;
	inputs.nSize = sizeof(THREAD_INFO) * 1000; //默认有1000个线程

	inputs.pBuffer = (THREAD_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, inputs.nSize);

	if (inputs.pBuffer)
	{

		bRet = g_connectDriver.DeviceControl(IOCTL_ENUM_THREAD, &inputs, sizeof(Input), &nRetLength, sizeof(ULONG), 0);


		if (bRet && nRetLength > 0)
		{
			for (ULONG i = 0; i < nRetLength; i++)
			{
				vectorThreads.push_back(inputs.pBuffer[i]);
			}
		}

		bRet = HeapFree(GetProcessHeap(), 0, inputs.pBuffer);
	}


	return bRet;
}

NTSTATUS CListThreads::KillThread(HANDLE hThreadHanle)
{
	NTSTATUS ntStatus = 0xC0000001L;
	_NtTerminateThread NtTerminateThread = NULL;

	NtTerminateThread = (_NtTerminateThread)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtTerminateThread");
	if (NtTerminateThread && hThreadHanle)
	{
		ULONG uExitCode = 0;
		ntStatus = NtTerminateThread(hThreadHanle, &uExitCode);
	}

	return ntStatus;
}




NTSTATUS CListThreads::ResumeThread(HANDLE hThreadHanle)
{
	NTSTATUS ntStatus = 0xC0000001L;
	_NtResumeThread NtResumeThread = NULL;

	NtResumeThread = (_NtResumeThread)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtResumeThread");
	if (NtResumeThread && hThreadHanle)
	{
		ULONG SuspendCount = 0;
		ntStatus = NtResumeThread(hThreadHanle, &SuspendCount);
	}

	return ntStatus;
}

NTSTATUS CListThreads::SuspendThread(HANDLE hThreadHanle)
{
	NTSTATUS ntStatus = 0xC0000001L;
	pNtSuspendThread NtSuspendThread = NULL;

	NtSuspendThread = (pNtSuspendThread)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtSuspendThread");
	if (NtSuspendThread && hThreadHanle)
	{
		ULONG SuspendCount = 0;
		ntStatus = NtSuspendThread(hThreadHanle, &SuspendCount);
	}

	return ntStatus;
}

ULONG CListThreads::hzw_CreateRemoteThread(HANDLE dwPid, PVOID64 paddresstoexecute, PVOID64 lparam)
{
	BOOL bRet = FALSE;

	ULONG ntStatus = 0;

	struct input
	{
		HANDLE dwPid;
		PVOID64 addresstoexecute;
		PVOID64 lparam;
	};
	input inputs = { 0 };

	inputs.dwPid = dwPid;
	inputs.addresstoexecute = paddresstoexecute;
	inputs.lparam = lparam;

	bRet = g_connectDriver.DeviceControl(IOCTL_CE_CREATETHREAD_EX, &inputs, sizeof(input), &ntStatus, sizeof(ULONG), NULL);


	return ntStatus;
}




HANDLE  CListThreads::KernelOpenThread(DWORD dwThreadId)
{
	struct input
	{
		HANDLE dwThreadId;
	};
	input inputs = { 0 };

	HANDLE ntStatus = 0;
	BOOL bRet = FALSE;
	inputs.dwThreadId = (HANDLE)dwThreadId;

	bRet = g_connectDriver.DeviceControl(IOCTL_OPEN_THREAD, &inputs, sizeof(input), &ntStatus, sizeof(HANDLE), NULL);

	return ntStatus;
}

NTSTATUS CListThreads::AlertResumeThread(HANDLE ThreadHandle)
{
	NTSTATUS ntStatus = 0xC0000001L;
	pNtAlertResumeThread NtAlertResumeThread = NULL;

	NtAlertResumeThread = (pNtAlertResumeThread)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtAlertResumeThread");
	if (NtAlertResumeThread && ThreadHandle)
	{
		ULONG SuspendCount = 0;
		ntStatus = NtAlertResumeThread(ThreadHandle, &SuspendCount);
	}

	return ntStatus;
}

NTSTATUS CListThreads::QueueApcThread(HANDLE ThreadHandle, PVOID ApcRoutine, PVOID ApcRoutineContext)
{
	NTSTATUS ntStatus = 0xC0000001L;
	pNtQueueApcThread NtQueueApcThread = NULL;

	NtQueueApcThread = (pNtQueueApcThread)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQueueApcThread");
	if (NtQueueApcThread && ThreadHandle)
	{
		ntStatus = NtQueueApcThread(ThreadHandle, (PIO_APC_ROUTINE)ApcRoutine, ApcRoutineContext, NULL, NULL);
	}

	return ntStatus;
}

