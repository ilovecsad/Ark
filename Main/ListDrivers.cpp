#include "ListDrivers.h"
#include "CLoadDriver.h"
extern CLoadDriver g_connectDriver;
#pragma warning(disable:4996)
CListDrivers::CListDrivers()
{

}

CListDrivers::~CListDrivers()
{

}

//
// 枚举驱动
//
BOOL CListDrivers::ListDrivers(vector<DRIVER_INFO> &vectorDrivers)
{
	BOOL bRet = FALSE;
	ULONG nCnt = 1000;
	PALL_DRIVERS pDriverInfo = NULL;

	vectorDrivers.clear();

	 struct input {
		ULONG nSize;
		PALL_DRIVERS pBuffer;
	};

	 input inputs = { 0 };

	 inputs.nSize = nCnt * sizeof(ALL_DRIVERS);
	 inputs.pBuffer = (PALL_DRIVERS)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nCnt * sizeof(ALL_DRIVERS));
	 if (inputs.pBuffer)
	 {
		 pDriverInfo = inputs.pBuffer;

		 bRet = g_connectDriver.DeviceControl(IOCTL_CE_ENUMDRIVERS, &inputs, sizeof(input), 0, 0, 0);

		 if (bRet && pDriverInfo->nCnt > 0)
		 {

			 for (ULONG i = 0; i < pDriverInfo->nCnt; i++)
			 {
				 FixDriverPath(&pDriverInfo->Drivers[i]);
				 vectorDrivers.push_back(pDriverInfo->Drivers[i]);
			 }
		 }

		 bRet = HeapFree(GetProcessHeap(), 0, inputs.pBuffer);
	 }



	return bRet;
}

BOOL CListDrivers::UnLoadDriver(ULONG_PTR DriverObject)
{
	BOOL bRet = FALSE;
	struct input {
		ULONG_PTR DriverObject;
	};

	input inputs = { 0 };

	if (DriverObject) 
	{
		inputs.DriverObject = DriverObject;
		bRet = g_connectDriver.DeviceControl(IOCTL_UNLOAD_DRIVER, &inputs, sizeof(input), 0, 0, 0);
	}
	return bRet;
}





 //修复驱动的路径

void CListDrivers::FixDriverPath(PDRIVER_INFO pDriverInfo)
{
	if (!pDriverInfo || wcslen(pDriverInfo->szDriverPath) == 0)
	{
		return;
	}

	WCHAR szWindowsDirectory[MAX_PATH] = { 0 };
	WCHAR szDriverDirectory[MAX_PATH] = { 0 };
	WCHAR szDriver[] = { '\\', 's', 'y', 's', 't', 'e', 'm', '3', '2', '\\', 'd', 'r', 'i', 'v', 'e', 'r', 's', '\\', '\0' };

	GetWindowsDirectory(szWindowsDirectory, MAX_PATH - 1);
	wcscpy(szDriverDirectory, szWindowsDirectory);
	wcscat(szDriverDirectory, szDriver);

	WCHAR *szOriginPath = pDriverInfo->szDriverPath;
	WCHAR szPath[MAX_PATH] = { 0 };
	WCHAR *szTemp = wcschr(szOriginPath, L'\\');

	// 没有目录信息，只有一个驱动名字的，直接拼接Driver目录。
	if (!szTemp)
	{
		wcscpy(szPath, szDriverDirectory);
		wcscat(szPath, szOriginPath);
		wcscpy(szOriginPath, szPath);
		szOriginPath[wcslen(szPath)] = L'\0';
	}
	else
	{
		WCHAR szFuck[] = { '\\', '?', '?', '\\', '\0' };
		WCHAR szSystemRoot[] = { '\\', 'S', 'y', 's', 't', 'e', 'm', 'R', 'o', 'o', 't', '\0' };
		WCHAR szWindows[] = { '\\', 'W', 'i', 'n', 'd', 'o', 'w', 's', '\0' };
		WCHAR szWinnt[] = { '\\', 'W', 'i', 'n', 'n', 't', '\0' };
		size_t nOrigin = wcslen(szOriginPath);

		if (nOrigin >= wcslen(szFuck) && !_wcsnicmp(szOriginPath, szFuck, wcslen(szFuck)))
		{
			wcscpy(szPath, szOriginPath + wcslen(szFuck));
			wcscpy(szOriginPath, szPath);
			szOriginPath[wcslen(szPath)] = L'\0';
		}
		else if (nOrigin >= wcslen(szSystemRoot) && !_wcsnicmp(szOriginPath, szSystemRoot, wcslen(szSystemRoot)))
		{
			wcscpy(szPath, szWindowsDirectory);
			wcscat(szPath, szOriginPath + wcslen(szSystemRoot));
			wcscpy(szOriginPath, szPath);
			szOriginPath[wcslen(szPath)] = L'\0';
		}
		else if (nOrigin >= wcslen(szWindows) && !_wcsnicmp(szOriginPath, szWindows, wcslen(szWindows)))
		{
			wcscpy(szPath, szWindowsDirectory);
			wcscat(szPath, szOriginPath + wcslen(szWindows));
			wcscpy(szOriginPath, szPath);
			szOriginPath[wcslen(szPath)] = L'\0';
		}
		else if (nOrigin >= wcslen(szWinnt) && !_wcsnicmp(szOriginPath, szWinnt, wcslen(szWinnt)))
		{
			wcscpy(szPath, szWindowsDirectory);
			wcscat(szPath, szOriginPath + wcslen(szWinnt));
			wcscpy(szOriginPath, szPath);
			szOriginPath[wcslen(szPath)] = L'\0';
		}
	}

	// 如果是短文件名
	if (wcschr(szOriginPath, '~'))
	{
		WCHAR szLongPath[MAX_PATH] = { 0 };
		DWORD nRet = GetLongPathName(szOriginPath, szLongPath, MAX_PATH);
		if (!(nRet >= MAX_PATH || nRet == 0))
		{
			wcscpy(szOriginPath, szLongPath);
			szOriginPath[wcslen(szLongPath)] = L'\0';
		}
	}
}

