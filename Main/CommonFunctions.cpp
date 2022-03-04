#include "CommonFunctions.h"
#include <set>
#include <afxdlgs.h>
using namespace std;

#pragma comment(lib, "Version.lib")




CommonFunctions::CommonFunctions()
{
}

CommonFunctions::~CommonFunctions()
{
}


CString CommonFunctions::GetFileCompanyName(CString szPath)
{
	CString strCompanyName = L"";

	if (szPath.IsEmpty())
	{
		return strCompanyName;
	}

	if (!szPath.CompareNoCase(L"Idle") || !szPath.CompareNoCase(L"System"))
	{
		return strCompanyName;
	}

	if (!PathFileExists(szPath))
	{
		strCompanyName = L"File not found";
		return strCompanyName;
	}

	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;

	LPCWSTR lpstrFilename = (LPCWSTR)szPath;
	DWORD dwHandle = 0;
	DWORD dwVerInfoSize = ::GetFileVersionInfoSize(lpstrFilename, &dwHandle);

	if (dwVerInfoSize)
	{
		LPVOID lpbuffer = LocalAlloc(LMEM_ZEROINIT, dwVerInfoSize);

		if (lpbuffer)
		{
			if (GetFileVersionInfo(lpstrFilename, dwHandle, dwVerInfoSize, lpbuffer))
			{
				UINT cbTranslate = 0;

				if (VerQueryValue(lpbuffer, L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &cbTranslate))
				{
					LPCWSTR lpwszBlock = 0;
					UINT    cbSizeBuf = 0;
					WCHAR   szSubBlock[MAX_PATH] = { 0 };

					if ((cbTranslate / sizeof(struct LANGANDCODEPAGE)) > 0)
					{
						StringCchPrintf(szSubBlock, sizeof(szSubBlock) / sizeof(WCHAR),
							L"\\StringFileInfo\\%04x%04x\\CompanyName", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
					}

					if (VerQueryValue(lpbuffer, szSubBlock, (LPVOID*)&lpwszBlock, &cbSizeBuf))
					{
						WCHAR lpoutCompanyName[MAX_PATH] = { 0 };

						StringCchCopy(lpoutCompanyName, MAX_PATH / sizeof(WCHAR), (LPCWSTR)lpwszBlock);
						strCompanyName = lpoutCompanyName;
					}
				}
			}

			LocalFree(lpbuffer);
		}
	}

	return strCompanyName;
}

CString CommonFunctions::GetFileDescription(CString szPath)
{
	CString strDescription;

	if (!szPath.IsEmpty() && PathFileExists(szPath))
	{
		LPWSTR lpstrFilename = (LPWSTR)(LPCWSTR)szPath;
		DWORD dwSize = GetFileVersionInfoSize(lpstrFilename, NULL);

		if (dwSize)
		{
			LPVOID pBlock = malloc(dwSize);
			if (pBlock)
			{
				if (GetFileVersionInfo(lpstrFilename, 0, dwSize, pBlock))
				{
					char* pVerValue = NULL;
					UINT nSize = 0;
					if (VerQueryValue(pBlock, TEXT("\\VarFileInfo\\Translation"), (LPVOID*)&pVerValue, &nSize))
					{
						CString strSubBlock, strTranslation;
						strDescription.Format(L"000%x", *((unsigned short int *)pVerValue));
						strTranslation = strDescription.Right(4);
						strDescription.Format(L"000%x", *((unsigned short int *)&pVerValue[2]));
						strTranslation += strDescription.Right(4);

						strSubBlock.Format(L"\\StringFileInfo\\%s\\FileDescription", strTranslation);
						if (VerQueryValue(pBlock, strSubBlock.GetBufferSetLength(256), (LPVOID*)&pVerValue, &nSize))
						{
							strSubBlock.ReleaseBuffer();
							strDescription.Format(L"%s", pVerValue);
						}
					}
				}

				free(pBlock);
			}
		}
	}

	return strDescription;
}
BOOL CommonFunctions::IsMicrosoftAppByPath(CString szPath)
{
	CString szCompany = GetFileCompanyName(szPath);
	return IsMicrosoftApp(szCompany);
}

BOOL CommonFunctions::IsMicrosoftApp(CString szCompany)
{
	BOOL bRet = FALSE;

	if (szCompany.IsEmpty())
	{
		return bRet;
	}

	if (!szCompany.CompareNoCase(L"Microsoft Corporation") ||
		!szCompany.CompareNoCase(L"Microsoft Corp.") ||
		!szCompany.CompareNoCase(L"Microsoft Corp., Veritas Software") ||
		!szCompany.CompareNoCase(L"Microsoft Corp., Veritas Software.")
		)
	{
		bRet = TRUE;
	}

	return bRet;
}


/*
"\SystemRoot\system32\ntoskrnl.exe" 转成 "C:\system32\ntoskrnl.exe"
*/
CString CommonFunctions::TrimPath(WCHAR * szPath)
{
	CString strPath;

	// 	if (wcschr(szPath, L'~'))
	// 	{
	// 		WCHAR szLongPath[MAX_PATH] = {0};
	// 		GetLongPathName(szPath, szLongPath, MAX_PATH);
	// 		wcsncpy_s(szPath, MAX_PATH, szLongPath, wcslen(szLongPath) + 1);
	// 	}

	if (wcslen(szPath) > wcslen(L"\\SystemRoot\\") &&
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
	 else if (wcslen(szPath) > wcslen(L"\\Windows") &&
		 !_wcsnicmp(szPath, L"\\Windows", wcslen(L"\\Windows")))
	 {
		WCHAR szSystemDir[MAX_PATH] = { 0 };
		if (GetSystemDirectory(szSystemDir, MAX_PATH) != 0)
		{
			CString szTemp = szSystemDir;
			szTemp = szTemp.Left(2);
			szTemp += L"\\";
			szTemp += szPath;
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


CString CommonFunctions::GetLongPath(CString szPath)
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


CString CommonFunctions::RegParsePath(WCHAR *szData, ULONG dwType)
{
	CString szFilePath;


	if (szData != NULL && wcslen((PWCHAR)szData) > 0)
	{

		if (szFilePath.IsEmpty())
		{
			szFilePath = TrimPath(szData);
		}
	}

	return szFilePath;
}


/* 类似于 RtlInitUnicodeString   但要释放内存 FreeUnicodeString */
BOOL CommonFunctions::InitUnicodeString(UNICODE_STRING *unString, WCHAR *szString)
{
	BOOL bRet = FALSE;
	if (!szString)
	{
		unString->Buffer = NULL;
		unString->Length = 0;
		unString->MaximumLength = 2;
		bRet = TRUE;
	}
	else
	{
		ULONG nLen = wcslen(szString);
		if (unString && nLen > 0)
		{
			PWCHAR pBuffer = (PWCHAR)malloc((nLen + 1) * sizeof(WCHAR));
			if (pBuffer)
			{
				memset(pBuffer, 0, (nLen + 1) * sizeof(WCHAR));
				wcscpy_s(pBuffer, nLen + 1, szString);
				unString->Buffer = pBuffer;
				unString->Length = (USHORT)(nLen * sizeof(WCHAR));
				unString->MaximumLength = (USHORT)((nLen + 1) * sizeof(WCHAR));
				bRet = TRUE;
			}
		}
	}

	return bRet;
}

void CommonFunctions::FreeUnicodeString(UNICODE_STRING *unString)
{
	if (unString && unString->Buffer && unString->Length > 0)
	{
		unString->Length = 0;
		unString->MaximumLength = 0;
		free(unString->Buffer);
		unString->Buffer = NULL;
	}
}



CString CommonFunctions::RegParsePath(CString szPath)
{
	if (!szPath.IsEmpty())
	{
		CString szTemp = szPath;
		szTemp.MakeUpper();

		int nExe = szTemp.Find(L"RUNDLL32.EXE ");
		if (nExe != -1)
		{
			szPath = szPath.Right(szPath.GetLength() - nExe - wcslen(L"RUNDLL32.EXE "));
		}

		if (szPath.Find('/') != -1)
		{
			szPath = szPath.Left(szPath.Find('/') - 1);
		}

		if (szPath.Find(L" -") != -1)
		{
			szPath = szPath.Left(szPath.Find(L" -"));
		}

		if (szPath.Find(',') != -1)
		{
			szPath = szPath.Left(szPath.Find(','));
		}

		szPath.TrimLeft(' ');
		szPath.TrimRight(' ');

		szPath.TrimLeft('"');
		szPath.TrimRight('"');

		WCHAR* szRootTemp = szPath.GetBuffer();
		if (!_wcsnicmp(szRootTemp, L"%SystemRoot%", wcslen(L"%SystemRoot%")))
		{
			WCHAR szSystemDir[MAX_PATH] = { 0 };
			if (GetWindowsDirectory(szSystemDir, MAX_PATH) != 0)
			{
				CString szTemp = szSystemDir;
				szTemp += (szRootTemp + wcslen(L"%SystemRoot%"));
				szPath = szTemp;
			}
		}
		else if (!_wcsnicmp(szRootTemp, L"%WinDir%", wcslen(L"%WinDir%")))
		{
			WCHAR szSystemDir[MAX_PATH] = { 0 };
			if (GetWindowsDirectory(szSystemDir, MAX_PATH) != 0)
			{
				CString szTemp = szSystemDir;
				szTemp += (szRootTemp + wcslen(L"%WinDir%"));
				szPath = szTemp;
			}
		}
		else if (!_wcsnicmp(szPath.GetBuffer(), L"%ProgramFiles%", wcslen(L"%ProgramFiles%")))
		{
			WCHAR szSystemDir[MAX_PATH] = { 0 };
			if (GetWindowsDirectory(szSystemDir, MAX_PATH) != 0)
			{
				CString szTemp = szSystemDir;
				szTemp = szTemp.Left(szTemp.Find('\\'));
				szTemp += L"\\Program Files";
				szTemp += (szPath.GetBuffer() + wcslen(L"%ProgramFiles%"));
				szPath = szTemp;
			}
		}
		// 		else if (szPath.Find('\\') == -1)
		// 		{
		// 			WCHAR szSystemDir[MAX_PATH] = {0};
		// 			if (GetSystemDirectory(szSystemDir, MAX_PATH) != 0)
		// 			{
		// 				CString szTemp = szSystemDir;
		// 				szTemp += L"\\";
		// 				szTemp += szPath;
		// 				szPath = szTemp;
		// 			}
		// 		}

		//	szPath.ReleaseBuffer();

		int nTemp = szPath.ReverseFind('"');
		if (nTemp > 0)
		{
			szPath = szPath.Left(nTemp);
		}

		if (szPath.Find('\\') == -1)
		{
			BOOL bFind = FALSE;
			WCHAR szSystemDir[MAX_PATH] = { 0 };
			if (GetWindowsDirectory(szSystemDir, MAX_PATH) != 0)
			{
				CString szTemp = szSystemDir;
				szTemp += L"\\" + szPath;
				if (PathFileExists(szTemp))
				{
					szPath = szTemp;
					bFind = TRUE;
				}
			}

			memset(szSystemDir, 0, MAX_PATH * sizeof(WCHAR));

			if (!bFind && GetSystemDirectory(szSystemDir, MAX_PATH))
			{
				CString szTemp = szSystemDir;
				szTemp += L"\\" + szPath;
				if (PathFileExists(szTemp))
				{
					szPath = szTemp;
					bFind = TRUE;
				}
			}
		}
	}

	szPath = GetLongPath(szPath);
	return szPath;
}


int CommonFunctions::GetSelectItem(CSortListCtrl *pList)
{
	int nRet = -1;

	if (pList)
	{
		POSITION pos = pList->GetFirstSelectedItemPosition();
		if (pos != NULL)
		{
			nRet = pList->GetNextSelectedItem(pos);
		}
	}

	return nRet;
}

void CommonFunctions::LocationExplorer(CString szPath)
{
	if (!szPath.IsEmpty() && PathFileExists(szPath))
	{
		CString strCmd;
		strCmd.Format(L"/select,%s", szPath);
		ShellExecuteW(NULL, L"open", L"explorer.exe", strCmd, NULL, SW_SHOW);
	}
	else
	{
		MessageBox(NULL, L"Cannot find the file!", NULL, MB_OK | MB_ICONWARNING);
	}
}


BOOL CommonFunctions::SetStringToClipboard(CString szImageName)
{
	if (szImageName.IsEmpty())
	{
		return TRUE;
	}

	BOOL bRet = FALSE;

	if (OpenClipboard(NULL))
	{
		HGLOBAL clipbuffer = 0;
		WCHAR* buffer = NULL;

		EmptyClipboard();
		clipbuffer = LocalAlloc(GMEM_ZEROINIT, (szImageName.GetLength() + 1) * sizeof(WCHAR));
		if (clipbuffer)
		{
			buffer = (WCHAR*)GlobalLock(clipbuffer);
			if (buffer)
			{
				wcsncpy_s(buffer, szImageName.GetLength() + 1, szImageName.GetBuffer(), szImageName.GetLength());
				szImageName.ReleaseBuffer();
				SetClipboardData(CF_UNICODETEXT, clipbuffer);
				GlobalUnlock(clipbuffer);

				bRet = TRUE;
			}
		}

		CloseClipboard();
	}

	return bRet;
}





void CommonFunctions::ExportListToTxt(CSortListCtrl* pList, CString szStatus)
{
	if (pList->GetItemCount() > 0)
	{
		CString sExcelFile;

		if (!GetDefaultTxtFileName(sExcelFile))
			return;

		CFile file;

		TRY
		{
			if (file.Open(sExcelFile, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate, NULL))
			{
				int i = 0;
				LVCOLUMN columnData;
				CString columnName;
				int columnNum = 0;
				CString strCloumn;
				CHAR szColumnA[0x4000] = {0};
				WCHAR szColumnW[0x4000] = {0};
				CHAR szStatusA[0x4000] = {0};
				WCHAR szStatusW[0x4000] = {0};

				int nLenTemp = szStatus.GetLength();
				wcsncpy_s(szStatusW, 0x4000, szStatus.GetBuffer(), nLenTemp);
				szStatus.ReleaseBuffer();

				WideCharToMultiByte(CP_ACP, 0, szStatusW, -1, szStatusA, 0x4000, NULL, NULL);
				file.Write(szStatusA, (UINT)strlen(szStatusA));
				file.Write("\r\n\r\n", (UINT)strlen("\r\n\r\n"));

				columnData.mask = LVCF_TEXT;
				columnData.cchTextMax = 100;
				columnData.pszText = columnName.GetBuffer(100);

				for (i = 0; pList->GetColumn(i, &columnData); i++)
				{
					strCloumn = strCloumn + columnData.pszText + L"\t\t";
				}

				strCloumn += "\r\n";
				nLenTemp = strCloumn.GetLength();
				wcsncpy_s(szColumnW, 0x4000, strCloumn.GetBuffer(), nLenTemp);
				strCloumn.ReleaseBuffer();
				WideCharToMultiByte(CP_ACP, 0, szColumnW, -1, szColumnA, 0x4000, NULL, NULL);
				file.Write(szColumnA, (UINT)strlen(szColumnA));

				columnName.ReleaseBuffer();
				columnNum = i;

				for (int nItemIndex = 0; nItemIndex < pList->GetItemCount(); nItemIndex++)
				{
					CHAR szColumnA[0x4000] = {0};
					WCHAR szColumnW[0x4000] = {0};
					CString strItem;

					for (i = 0; i < columnNum; i++)
					{
						strItem = strItem + pList->GetItemText(nItemIndex, i) + L"\t\t";
					}

					strItem += "\r\n";
					nLenTemp = strItem.GetLength();
					wcsncpy_s(szColumnW, 0x4000, strItem.GetBuffer(), nLenTemp);
					strItem.ReleaseBuffer();
					WideCharToMultiByte(CP_ACP, 0, szColumnW, -1, szColumnA, 0x4000, NULL, NULL);
					file.Write(szColumnA, (UINT)strlen(szColumnA));
				}

				file.Close();
			}
		}
			CATCH_ALL(e)
		{
			file.Abort();   // close file safely and quietly
		//	THROW_LAST();
		}
		END_CATCH_ALL

			if (PathFileExists(sExcelFile))
			{
				ShellExecuteW(NULL, L"open", sExcelFile, NULL, NULL, SW_SHOW);
			}
			else
			{
				MessageBox(NULL, L"导出到文本文件失败!", L"导出", MB_OK | MB_ICONERROR);
			}
	}
}




BOOL CommonFunctions::GetDefaultTxtFileName(CString& sExcelFile)
{
	CString timeStr;
	CTime day = CTime::GetCurrentTime();
	int filenameday, filenamemonth, filenameyear, filehour, filemin, filesec;

	filenameday = day.GetDay();
	filenamemonth = day.GetMonth();
	filenameyear = day.GetYear();
	filehour = day.GetHour();
	filemin = day.GetMinute();
	filesec = day.GetSecond();
	timeStr.Format(L"%04d%02d%02d%02d%02d%02d", filenameyear, filenamemonth, filenameday, filehour, filemin, filesec);

	sExcelFile = timeStr + L".txt";

	CFileDialog fileDlg(
		FALSE,
		0,
		sExcelFile,
		0,
		L"Text Files (*.txt)|*.txt|All Files (*.*)|*.*||",
		0
	);

	if (IDOK == fileDlg.DoModal())
	{
		sExcelFile = fileDlg.GetFileName();

		CString szXls = sExcelFile.Right((int)wcslen(L".txt"));

		if (szXls.CompareNoCase(L".txt") != 0)
		{
			sExcelFile += L".txt";
		}

		if (!PathFileExists(sExcelFile))
		{
			return TRUE;
		}
		else if (PathFileExists(sExcelFile) && MessageBox(NULL, L"文件已经存在，是否覆盖？", L"导出", MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			if (!DeleteFile(sExcelFile))
			{
				MessageBox(NULL, L"覆盖文件失败！", L"导出", MB_OK | MB_ICONERROR);
				return FALSE;
			}

			return TRUE;
		}
	}

	return FALSE;
}

/*
CString CommonFunctions::GetExcelDriver()
{
	WCHAR szBuf[2001];
	WORD cbBufMax = 2000;
	WORD cbBufOut;
	WCHAR *pszBuf = szBuf;
	CString sDriver;

	if (!SQLGetInstalledDrivers(szBuf, cbBufMax, &cbBufOut))
		return NULL;

	do
	{
		if (wcsstr(pszBuf, L"Excel") != 0)
		{
			sDriver = CString(pszBuf);
			break;
		}

		pszBuf = wcschr(pszBuf, L'\0') + 1;
	} while (pszBuf[1] != L'\0');

	return sDriver;
}
void CommonFunctions::ExportListToExcel(CSortListCtrl* pList, CString strTitle, CString szStatus)
{
	BOOL bOk = FALSE;

	if (pList->GetItemCount() > 0)
	{
		CDatabase database;
		CString sDriver;
		CString sExcelFile;
		CString sSql;
		CString tableName = strTitle;

		sDriver = GetExcelDriver();
		if (sDriver.IsEmpty())
		{
			MessageBox(NULL, L"没有安装Excel!\n请先安装Excel软件才能使用导出功能!", L"导出", MB_OK | MB_ICONERROR);
			return;
		}

		if (!GetDefaultXlsFileName(sExcelFile))
			return;

		sSql.Format(L"DRIVER={%s};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s", sDriver, sExcelFile, sExcelFile);

		if (database.OpenEx(sSql, CDatabase::noOdbcDialog))
		{
			int i;
			LVCOLUMN columnData;
			CString columnName;
			int columnNum = 0;
			CString strH;
			CString strV;

			sSql = L"";
			strH = L"";
			columnData.mask = LVCF_TEXT;
			columnData.cchTextMax = 100;
			columnData.pszText = columnName.GetBuffer(100);

			for (i = 0; pList->GetColumn(i, &columnData); i++)
			{
				if (i != 0)
				{
					sSql = sSql + L", ";
					strH = strH + L", ";
				}

				sSql = sSql + L" " + L"[" + columnData.pszText + L"]" + L" TEXT";
				strH = strH + L" " + L"[" + columnData.pszText + L"]" + L" ";
			}

			columnName.ReleaseBuffer();
			columnNum = i;

			sSql = L"CREATE TABLE " + tableName + L" ( " + sSql + L" ) ";
			database.ExecuteSQL(sSql);

			for (int nItemIndex = 0; nItemIndex < pList->GetItemCount(); nItemIndex++)
			{
				strV = L"";
				for (i = 0; i < columnNum; i++)
				{
					if (i != 0)
					{
						strV = strV + L", ";
					}

					strV = strV + L" '" + pList->GetItemText(nItemIndex, i) + L"' ";
				}

				sSql = L"INSERT INTO " + tableName + L" (" + strH + L")" + L" VALUES(" + strV + L")";
				database.ExecuteSQL(sSql);
			}

			strV = L"";
			for (i = 0; i < columnNum; i++)
			{
				if (i != 0)
				{
					strV = strV + L", ";
				}

				strV = strV + L" '" + L" " + L"' ";
			}

			sSql = L"INSERT INTO " + tableName + L" (" + strH + L")" + L" VALUES(" + strV + L")";
			database.ExecuteSQL(sSql);

			strV = L"";
			strV = strV + L" '" + szStatus + L"' ";
			for (i = 1; i < columnNum; i++)
			{
				strV = strV + L", ";
				strV = strV + L" '" + L" " + L"' ";
			}

			sSql = L"INSERT INTO " + tableName + L" (" + strH + L")" + L" VALUES(" + strV + L")";
			database.ExecuteSQL(sSql);

			bOk = TRUE;
		}

		database.Close();

		if (bOk && PathFileExists(sExcelFile))
		{
			ShellExecuteW(NULL, L"open", sExcelFile, NULL, NULL, SW_SHOW);
		}
		else
		{
			MessageBox(NULL, L"导出到Excel文件失败!", L"导出", MB_OK | MB_ICONERROR);
		}
	}
}
*/


//多字节字符串转WCHAR字符串
CString CommonFunctions::ms2ws(LPCSTR szSrc, int cbMultiChar)
{
	CString strDst;
	if (szSrc == NULL || cbMultiChar == 0) {
		return strDst;
	}

	WCHAR*pBuff = NULL;
	int nLen = MultiByteToWideChar(CP_ACP, 0, szSrc, cbMultiChar, NULL, 0);
	if (nLen > 0) {
		pBuff = new WCHAR[nLen + 1];
		memset(pBuff, 0, (nLen + 1) * sizeof(WCHAR));
		if (pBuff) {
			MultiByteToWideChar(CP_ACP, 0, szSrc, cbMultiChar, pBuff, nLen);
			pBuff[nLen] = 0;
			strDst = pBuff;
			delete[] pBuff;
		}
	}

	return strDst;
}