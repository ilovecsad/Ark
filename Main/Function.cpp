#include "Function.h"


CString GetLongPath(CString szPath)
{
	CString strPath;

	if (szPath.Find(L'~') != -1)
	{
		WCHAR szLongPath[MAX_PATH] = { 0 };
		DWORD nRet = GetLongPathName(szPath, szLongPath, MAX_PATH);
		if (nRet >= MAX_PATH || nRet == 0)
		{
			strPath = szPath;
		}
		else
		{
			strPath = szLongPath;
		}
	}
	else
	{
		strPath = szPath;
	}

	return strPath;
}

CString TrimPath(WCHAR * szPath)
{
	CString strPath;

	if (szPath[1] == ':' && szPath[2] == '\\')
	{
		strPath = szPath;
	}
	else if (wcslen(szPath) > wcslen(L"\\SystemRoot\\") &&
		!_wcsnicmp(szPath, L"\\SystemRoot\\", wcslen(L"\\SystemRoot\\")))
	{
		WCHAR szSystemDir[MAX_PATH] = { 0 };
		GetWindowsDirectory(szSystemDir, MAX_PATH);
		strPath.Format(L"%s\\%s", szSystemDir, szPath + wcslen(L"\\SystemRoot\\"));
	}
	else if (wcslen(szPath) > wcslen(L"system32\\") &&
		!_wcsnicmp(szPath, L"system32\\", wcslen(L"system32\\")))
	{
		WCHAR szSystemDir[MAX_PATH] = { 0 };
		GetWindowsDirectory(szSystemDir, MAX_PATH);
		strPath.Format(L"%s\\%s", szSystemDir, szPath/* + wcslen(L"system32\\")*/);
	}
	else if (wcslen(szPath) > wcslen(L"\\??\\") &&
		!_wcsnicmp(szPath, L"\\??\\", wcslen(L"\\??\\")))
	{
		strPath = szPath + wcslen(L"\\??\\");
	}
	else if (wcslen(szPath) > wcslen(L"%ProgramFiles%") &&
		!_wcsnicmp(szPath, L"%ProgramFiles%", wcslen(L"%ProgramFiles%")))
	{
		WCHAR szSystemDir[MAX_PATH] = { 0 };
		if (GetWindowsDirectory(szSystemDir, MAX_PATH) != 0)
		{
			CString szTemp = szSystemDir;
			szTemp = szTemp.Left(szTemp.Find('\\'));
			szTemp += L"\\Program Files";
			szTemp += szPath + wcslen(L"%ProgramFiles%");
			strPath = szTemp;
		}
	}
	else
	{
		strPath = szPath;
	}

	strPath = GetLongPath(strPath);

	return strPath;
}


void FixPeDump86(PVOID pBuffer)
{
	if (!pBuffer)
	{
		return;
	}

	PIMAGE_DOS_HEADER       PEDosHeader;
	PIMAGE_FILE_HEADER      PEFileHeader;
	PIMAGE_OPTIONAL_HEADER  PEOptionalHeader;
	PIMAGE_SECTION_HEADER   PESectionHeaders;
	PULONG                  PEMagic;

	__try
	{
		PEDosHeader = (PIMAGE_DOS_HEADER)pBuffer;
		PEMagic = (PULONG)((ULONG_PTR)pBuffer + PEDosHeader->e_lfanew);
		PEFileHeader = (PIMAGE_FILE_HEADER)((ULONG_PTR)pBuffer + PEDosHeader->e_lfanew + sizeof(ULONG));
		PEOptionalHeader = (PIMAGE_OPTIONAL_HEADER)((ULONG_PTR)pBuffer + PEDosHeader->e_lfanew + sizeof(ULONG) + sizeof(IMAGE_FILE_HEADER));
		PESectionHeaders = (PIMAGE_SECTION_HEADER)((ULONG_PTR)pBuffer + PEDosHeader->e_lfanew + sizeof(ULONG) + sizeof(IMAGE_FILE_HEADER) + sizeof(IMAGE_OPTIONAL_HEADER));

		if (PEFileHeader->NumberOfSections > 0)
		{
			ULONG_PTR SectionAlignment = PEOptionalHeader->SectionAlignment;
			ULONG_PTR VirtualAddress = PESectionHeaders->VirtualAddress;

			for (ULONG i = 0; i < PEFileHeader->NumberOfSections; i++)
			{
				ULONG_PTR VirtualSize = PESectionHeaders[i].Misc.VirtualSize;
				ULONG_PTR DivVirtualSize = VirtualSize % SectionAlignment;

				if (DivVirtualSize)
				{
					VirtualSize = SectionAlignment + VirtualSize - DivVirtualSize;
				}

				PESectionHeaders[i].SizeOfRawData = VirtualSize;
				PESectionHeaders[i].PointerToRawData = VirtualAddress;

				VirtualAddress += VirtualSize;
			}
		}
	}
	__except (1)
	{

	}
}