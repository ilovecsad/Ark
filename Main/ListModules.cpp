#include "ListModules.h"
#include <TlHelp32.h>
#include <afxdlgs.h>
#include "Function.h"
extern CLoadDriver g_connectDriver;

CListModules::CListModules()
{

}

CListModules::~CListModules()
{

}


BOOL CListModules::EnumModulesByPeb(ULONG nPid, ULONG_PTR pEprocess, vector<MODULE_INFO_EX>& vectorModules)
{

	BOOL bRet = FALSE;

	typedef struct _INPUT
	{
		ULONG nPid;
		ULONG nSize;
		ULONG_PTR pEprocess;
		PALL_MODULES pBuffer;
	}Input;

	Input inputs = { 0 };

	if (nPid == 0 && pEprocess == 0)
	{
		return bRet;
	}
	vectorModules.clear();

	inputs.nPid = nPid;
	inputs.pEprocess = pEprocess;
	inputs.nSize = sizeof(MODULE_INFO_EX) * 1000 + sizeof(ULONG); //默认有1000个线程

	inputs.pBuffer = (PALL_MODULES)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, inputs.nSize);

	if (inputs.pBuffer)
	{
		
		bRet = g_connectDriver.DeviceControl(IOCTL_ENUM_MODULE_BYPEB, &inputs, sizeof(Input), 0, 0, 0);


		if (bRet && inputs.pBuffer->nCnt > 0)
		{
			for (ULONG i = 0; i < inputs.pBuffer->nCnt; i++)
			{
				vectorModules.push_back(inputs.pBuffer->Modules[i]);
			}
		}

		bRet = HeapFree(GetProcessHeap(), 0, inputs.pBuffer);
	}



	return bRet;
}

// 
// 根据进程的快照取进程模块
//
BOOL CListModules::EnumModulesBySnapshot(ULONG nPid, vector<MODULE_INFO_EX> &vectorModules)
{
	BOOL bRet = FALSE;

	vectorModules.clear();

	MODULEENTRY32 me32 = { 0 };
	me32.dwSize = sizeof(MODULEENTRY32);

	HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, nPid);

	if (hModuleSnap != INVALID_HANDLE_VALUE)
	{
		if (Module32First(hModuleSnap, &me32))
		{
			do
			{
				MODULE_INFO_EX info;

				info.Base = (ULONG_PTR)me32.modBaseAddr;
				info.Size = me32.modBaseSize;
				wcsncpy_s(info.Path, MAX_PATH - 1, me32.szModule, wcslen(me32.szModule));

				vectorModules.push_back(info);

			} while (Module32Next(hModuleSnap, &me32));
		}

		CloseHandle(hModuleSnap);
		bRet = TRUE;
	}

	return bRet;
}

BOOL CListModules::FreeLoadLibary(ULONG nPid, ULONG_PTR dwModuleBase,ULONG_PTR dwLdrpHashTable)
{
	BOOL bRet = FALSE;

	struct input
	{
		ULONG threadId;
		ULONG_PTR dwModuleBase;
		ULONG_PTR dwLdrpHashTable;
	};
	input inputs = { 0 };

	inputs.threadId = nPid;
	inputs.dwModuleBase = dwModuleBase;
	inputs.dwLdrpHashTable = dwLdrpHashTable;
	bRet = g_connectDriver.DeviceControl(IOCTL_FREE_MODULE, &inputs, sizeof(input), 0, 0, NULL);


	return bRet;
}

PVOID CListModules::DumpModuleMemory(ULONG nPid, ULONG_PTR dwModuleBase, ULONG nSize)
{
	BOOL bRet = FALSE;

	struct input
	{
		ULONG threadId;
		ULONG_PTR dwModuleBase;
		ULONG nSize;
		PVOID pOutBuffer;
	};
	input inputs = { 0 };

	PVOID pTemp = NULL;

	inputs.threadId = nPid;
	inputs.dwModuleBase = dwModuleBase;
	inputs.nSize = nSize;

	pTemp = malloc(nSize);
	if (!pTemp) return NULL;
	RtlZeroMemory(pTemp, inputs.nSize);
	inputs.pOutBuffer = pTemp;

	bRet = g_connectDriver.DeviceControl(IOCTL_DUMP_BUFFER, &inputs, sizeof(input), 0, 0, NULL);

	if (!bRet) {
		free(pTemp);
		pTemp = NULL;
	}

	return pTemp;
}

BOOL CListModules::DumpModuleMemoryToFile(ULONG nPid, ULONG_PTR nBase, ULONG nSize, CString szModule)
{
	if ((nPid == 0) || nSize == 0)
	{
		return FALSE;
	}

	BOOL bCopy = FALSE;
	WCHAR szFile[MAX_PATH] = { 0 };
	wcsncpy_s(szFile, MAX_PATH, szModule.GetBuffer(), szModule.GetLength());
	szModule.ReleaseBuffer();

	CFileDialog fileDlg(
		FALSE,
		0,
		(LPWSTR)szFile,
		0,
		L"(*.exe; *.dll; *.sys; *.oct)|*.exe;*.dll;*.sys;*.oct|All Files (*.*)|*.*||",
		0
	);

	if (IDOK != fileDlg.DoModal())
	{
		return FALSE;
	}

	CString szFilePath = fileDlg.GetFileName();
	if (!PathFileExists(szFilePath) ||
		(PathFileExists(szFilePath) && ::MessageBox(NULL, L"文件已经存在，是否覆盖?", L"antiSpy", MB_YESNO | MB_ICONQUESTION) == IDYES))
	{
		PVOID pBuffer =DumpModuleMemory(nPid,nBase, nSize);
		if (!pBuffer)
		{
			return FALSE;
		}


		CFile file;
		TRY
		{
			if (file.Open(szFilePath, CFile::modeCreate | CFile::modeWrite))
			{
				file.Write(pBuffer, nSize);
				file.Close();
				bCopy = TRUE;
			}
		}
			CATCH_ALL(e)
		{
			file.Abort();   // close file safely and quietly
			//THROW_LAST();
		}
		END_CATCH_ALL

			if (bCopy)
			{
				MessageBox(NULL, L"Dump OK!", L"antiSpy", MB_OK);
			}
			else
			{
				MessageBox(NULL, L"Dump Failed!", L"antiSpy", MB_OK);
			}

		free(pBuffer);
		pBuffer = NULL;
	}


	return bCopy;
}


BOOL CListModules::HideModule(ULONG nPid, ULONG_PTR dwModuleBase, ULONG_PTR dwLdrpHashTable,ULONG nSize)
{
	BOOL bRet = FALSE;
	if (!nPid || !dwModuleBase || !nSize)return bRet;

	struct input
	{
		ULONG threadId;
		ULONG nSize;
		ULONG_PTR dwModuleBase;
		ULONG_PTR dwLdrpHashTable;
		
	};
	input inputs = { 0 };



	inputs.threadId = nPid;
	inputs.dwModuleBase = dwModuleBase;
	inputs.nSize = nSize;
	inputs.dwLdrpHashTable = dwLdrpHashTable;

	bRet = g_connectDriver.DeviceControl(IOCTL_HIDE_MODULE, &inputs, sizeof(input), 0, 0, NULL);


	return bRet;
}
