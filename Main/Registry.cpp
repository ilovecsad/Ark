#include "Registry.h"
#include "CLoadDriver.h"

extern CLoadDriver g_connectDriver;

CRegistry::CRegistry()
{
}

CRegistry::~CRegistry()
{

}


HANDLE CRegistry::OpenKey(CString szKey)
{
	HANDLE hKey = NULL;

	if (szKey.IsEmpty())
	{
		return hKey;
	}

	UNICODE_STRING unKey;
	if (m_Function.InitUnicodeString(&unKey, szKey.GetBuffer())) //等价于 RtlInitUnicodeString
	{
		OBJECT_ATTRIBUTES oa;
		InitializeObjectAttributes(&oa, &unKey, OBJ_CASE_INSENSITIVE, 0, NULL);

		if (!OpenKey(&hKey, KEY_ALL_ACCESS, &oa))
		{
			hKey = NULL;
		}

		m_Function.FreeUnicodeString(&unKey);
	}
	szKey.ReleaseBuffer();
	


	return hKey;
}






BOOL CRegistry::OpenKey(OUT PHANDLE  KeyHandle,
	IN ACCESS_MASK  DesiredAccess,
	IN POBJECT_ATTRIBUTES  ObjectAttributes
)
{
	BOOL bRet = FALSE;
	static pfnZwOpenKey ZwOpenKey = NULL;
	static HMODULE hNtdll = NULL;
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	HANDLE pTemp = 0;

	if (!hNtdll) {
		hNtdll = GetModuleHandle(L"ntdll.dll");
	}
	if (!ZwOpenKey) {
		ZwOpenKey = (pfnZwOpenKey)GetProcAddress(hNtdll, "ZwOpenKey");
	}
	if (ZwOpenKey)
	{
		status = ZwOpenKey(&pTemp, DesiredAccess, ObjectAttributes);
		if (NT_SUCCESS(status))
		{
			*KeyHandle = pTemp;
			bRet = TRUE;
		}


	}



	return bRet;
}

void CRegistry::EnumKeys(CString szKey, list<KEY_INFO> &keyList)
{
	keyList.clear();

	ULONG i = 0;
	WCHAR keyName[MAX_PATH] = { 0 }; //保存子键的名称 
	DWORD charLength = MAX_PATH;

	HANDLE hKey = OpenKey(szKey);
	if (hKey == NULL)
	{
		return;
	}

	while (RegEnumKeyExW((HKEY)hKey, i, keyName, &charLength, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
	{
		KEY_INFO info;
		info.nNameLen = wcslen(keyName);
		info.szName = keyName;
		keyList.push_back(info);

		RtlZeroMemory(keyName, MAX_PATH);
		charLength = MAX_PATH;
		i++;
	}

	CloseHandle(hKey);
}

void CRegistry::FreeValueList(list<VALUE_INFO> &valueList)
{
	for (list<VALUE_INFO>::iterator ir = valueList.begin();
		ir != valueList.end();
		ir++)
	{
		if (ir->pData)
		{
			free(ir->pData);
			ir->pData = NULL;
		}
	}
}

void CRegistry::EnumValues(CString szKey, list<VALUE_INFO> &valueList)
{
	valueList.clear();

	if (szKey.IsEmpty())
	{
		return;
	}

	
	HANDLE hKey = OpenKey(szKey);
	if (hKey == NULL)
	{
		return;
	}
	///////////////////////////////////////////////////////////////////////

	static pfnZwEnumerateValueKey ZwEnumerateValueKey = NULL;
	static HMODULE hNtdll = NULL;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	if (!hNtdll) {
		hNtdll = GetModuleHandle(L"ntdll.dll");
	}
	if (!ZwEnumerateValueKey) {
		ZwEnumerateValueKey = (pfnZwEnumerateValueKey)GetProcAddress(hNtdll, "ZwEnumerateValueKey");
	}

	if (ZwEnumerateValueKey)
	{
		for (ULONG i = 0; ; i++)
		{
			ULONG nRetLen = 0;
			status = ZwEnumerateValueKey(hKey, i, KeyValueFullInformation, NULL, 0, &nRetLen);
			if (status == STATUS_NO_MORE_ENTRIES) // STATUS_NO_MORE_ENTRIES
			{
				break;
			}
			else if (status == STATUS_INVALID_PARAMETER) 
			{
				break;
			}
			else if (status == STATUS_BUFFER_OVERFLOW || status == STATUS_BUFFER_TOO_SMALL) // STATUS_BUFFER_TOO_SMALL
			{
				PKEY_VALUE_FULL_INFORMATION pBuffer = (PKEY_VALUE_FULL_INFORMATION)malloc(nRetLen + 100);
				if (pBuffer)
				{
					memset(pBuffer, 0, nRetLen + 100);
					status = ZwEnumerateValueKey(hKey, i, KeyValueFullInformation, pBuffer, nRetLen + 100, &nRetLen);

					if (NT_SUCCESS(status))
					{
						VALUE_INFO info;

						CString szName = L"";
						if (pBuffer->NameLength)
						{
							ULONG nLen = pBuffer->NameLength + sizeof(WCHAR);
							WCHAR *szNameTemp = (WCHAR *)malloc(nLen);
							if (szNameTemp)
							{
								memset(szNameTemp, 0, nLen);
								wcsncpy_s(szNameTemp, nLen / sizeof(WCHAR), pBuffer->Name, pBuffer->NameLength / sizeof(WCHAR));
								szName = szNameTemp;

								free(szNameTemp);
							}
						}

						info.nNameLen = pBuffer->NameLength;
						info.szName = szName;
						info.Type = pBuffer->Type;
						info.nDataLen = pBuffer->DataLength;
						info.pData = NULL;

						if (pBuffer->DataLength)
						{
							ULONG nLen = pBuffer->DataLength + sizeof(WCHAR) * 2;
							info.pData = malloc(nLen);
							if (info.pData)
							{
								memset(info.pData, 0, nLen);
								memcpy(info.pData, (PVOID)((PBYTE)pBuffer + pBuffer->DataOffset), pBuffer->DataLength);
							}
						}

						valueList.push_back(info);
					}

					free(pBuffer);
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////////
	CloseHandle(hKey);
}