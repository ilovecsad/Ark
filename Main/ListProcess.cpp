#include "ListProcess.h"
#include <TlHelp32.h>
#include "CLoadDriver.h"

extern CLoadDriver g_connectDriver;

CListProcess::CListProcess()
{
}

CListProcess::~CListProcess()
{
}

CString CListProcess::GetProcessCommandLine(HANDLE dwPid)
{
	CString str = L"";

	PVOID pBuffer = NULL;

	struct input
	{
		HANDLE processid;
		ULONG_PTR nSize;
		PVOID wzCommand;
	};
	input inputs = { 0 };

	pBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 1000);

	if (pBuffer)
	{
		inputs.nSize = 1000;
		inputs.processid = dwPid;
		inputs.wzCommand = pBuffer;

		if (g_connectDriver.DeviceControl(IOCTL_GET_PROCESS_COMMANDLINE_BY_PEB, &inputs, sizeof(input), 0, 0, 0))
		{
			str = (WCHAR*)pBuffer;

			if (!str.IsEmpty())
			{
				int index = 0;
				str.MakeLower();
				str.ReverseFind('"') != -1 ? index = 4 : index = 3;

				str = str.Right(str.GetLength() - str.Find(L"exe") - index);
			}


		}

		HeapFree(GetProcessHeap(), 0, pBuffer);

	}
	return str;
}


BOOL CListProcess::EnumProcess(vector<PROCESS_INFO>& vectorProcess)
{
	BOOL bRet = FALSE;
	ALL_PROCESSES pInput = {0};

	PPROCESS_INFO pProcessInfo = NULL;

	vectorProcess.clear();



	pInput.ProcessInfo = (PVOID)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PROCESS_INFO) * 1000);
	pInput.nSize = sizeof(PROCESS_INFO) * 1000;

	ULONG nRet = 0;

	if (pInput.ProcessInfo) {

		bRet = g_connectDriver.DeviceControl(IOCTL_GET_EPROCESS, &pInput, sizeof(ALL_PROCESSES),&nRet,sizeof(ULONG), 0);
	}

	if (bRet && nRet > 0) {

		pProcessInfo = (PPROCESS_INFO)pInput.ProcessInfo;

		for (ULONG i = 0; i < nRet; i++)
		{
			vectorProcess.push_back(pProcessInfo[i]);
		}

	}


	bRet = HeapFree(GetProcessHeap(), 0, pInput.ProcessInfo);

	return bRet;
}

BOOL CListProcess::EnumProcessByR3(list<ULONG>& vectorProcess)
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	BOOL bRet = FALSE;

	// 首先清空下链表
	vectorProcess.clear();

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap != INVALID_HANDLE_VALUE)
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hProcessSnap, &pe32))
		{
			do
			{
				vectorProcess.push_back(pe32.th32ProcessID);
			} while (Process32Next(hProcessSnap, &pe32));

			bRet = TRUE;
		}

		CloseHandle(hProcessSnap);
	}
	return 0;
}

void CListProcess::SortByProcessCreateTime(vector<PROCESS_INFO>& ProcListEx)
{
}

BOOL CListProcess::KillProcess(HANDLE nPid)
{
	BOOL bRet = FALSE;

	struct input
	{
		HANDLE processid;
	};
	input inputs = { 0 };

	inputs.processid = nPid;

	bRet = g_connectDriver.DeviceControl(IOCTL_KISS_PROCESS, &inputs, sizeof(input), 0, 0, NULL);


	return bRet;
}

BOOL CListProcess::SuspendProcess(HANDLE nPid)
{
	BOOL bRet = FALSE;

	struct input
	{
		
		HANDLE processid;
	};
	input inputs = { 0 };

	inputs.processid = nPid;

	bRet = g_connectDriver.DeviceControl(IOCTL_SUSPEND_PROCESS, &inputs, sizeof(input), 0, 0, NULL);
	

	return bRet;
}

BOOL CListProcess::ResumeProcess(HANDLE nPid)
{
	BOOL bRet = FALSE;

	struct input
	{
		HANDLE processid;
	};
	input inputs = { 0 };

	inputs.processid = nPid;

	bRet = g_connectDriver.DeviceControl(IOCTL_RESUME_PROCESS, &inputs, sizeof(input),0, 0, NULL);


	return bRet;
}

