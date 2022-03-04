// CListProcessHook.cpp: 实现文件
//

#include "pch.h"
#include "Main.h"
#include "CListProcessHook.h"
#include "afxdialogex.h"
#include <TlHelp32.h>

static pfnImageDirectoryEntryToData RtlImageDirectoryEntryToData = NULL;
static pfnRtlFindExportedRoutineByName RtlFindExportedRoutineByName = NULL;
static pfnLdrProcessRelocationBlock LdrProcessRelocationBlock = NULL;
static pfnRtlImageNtHeader RtlImageNtHeader = NULL;
// CListProcessHook 对话框

CListProcessHook *Dlg = NULL;

IMPLEMENT_DYNAMIC(CListProcessHook, CDialogEx)

CListProcessHook::CListProcessHook(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HOOK_DIALOG, pParent)
{

}

CListProcessHook::~CListProcessHook()
{
}

void CListProcessHook::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ENUMPROCESS_LIST, m_listProcess);
	DDX_Control(pDX, IDC_PROCESSHOOK2_LIST, m_listHookInfo);
}


BEGIN_MESSAGE_MAP(CListProcessHook, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_RCLICK, IDC_ENUMPROCESS_LIST, &CListProcessHook::OnRclickEnumprocessList)
	ON_COMMAND(ID_RESFRESH, &CListProcessHook::OnResfresh)
	ON_COMMAND(ID_CHECK_HOOK, &CListProcessHook::OnCheckHook)
	ON_MESSAGE(WM_ADD_ITEM_PROCESSHOOK_DLG, &CListProcessHook::OnAddItemBySendMessage)
	ON_COMMAND(ID_EXPORT_EXCEL, &CListProcessHook::OnExportExcel)
	ON_NOTIFY(NM_RCLICK, IDC_PROCESSHOOK2_LIST, &CListProcessHook::OnRclickProcesshook2List)
END_MESSAGE_MAP()


// CListProcessHook 消息处理程序

DWORD WINAPI EnumHookProc(
	LPVOID lpThreadParameter)
{
	PPROCESS_INFO pInfo = (PPROCESS_INFO)lpThreadParameter;
	if (pInfo && Dlg)
	{
		Dlg->m_bIs64 = FALSE;
		Dlg->m_bIsRun = TRUE;
		::EnableWindow(::GetDlgItem(Dlg->GetSafeHwnd(), IDC_ENUMPROCESS_LIST), FALSE);
		::EnableWindow(::GetDlgItem(Dlg->GetSafeHwnd(), IDC_PROCESSHOOK2_LIST), FALSE);
		Dlg->EnumProcessHooks(pInfo);

		Dlg->m_bIsRun = FALSE;
		::EnableWindow(::GetDlgItem(Dlg->GetSafeHwnd(), IDC_ENUMPROCESS_LIST), TRUE);
		::EnableWindow(::GetDlgItem(Dlg->GetSafeHwnd(), IDC_PROCESSHOOK2_LIST), TRUE);
	}
	
	return 0;
}
BOOL CListProcessHook::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_listProcess.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listProcess.InsertColumn(0, L"PID", LVCFMT_CENTER, 40);
	m_listProcess.InsertColumn(1, L"进程名", LVCFMT_LEFT, 120);

	m_listHookInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listHookInfo.InsertColumn(0, L"Hook Address", LVCFMT_LEFT, 230);
	m_listHookInfo.InsertColumn(1, L"Hook Type", LVCFMT_LEFT, 60);
	m_listHookInfo.InsertColumn(2, L"Current Entry", LVCFMT_LEFT, 150);

	m_listHookInfo.InsertColumn(3, L"挂钩处当前值", LVCFMT_LEFT, 150);
	m_listHookInfo.InsertColumn(4, L"挂钩处原始值", LVCFMT_LEFT, 150);



	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

VOID CListProcessHook::ShowAllProcess()
{
	m_listProcess.DeleteAllItems();
	m_vectorProcess.clear();

	m_clsProcess.EnumProcess(m_vectorProcess);


	for (vector <PROCESS_INFO>::iterator Iter = m_vectorProcess.begin();
		Iter != m_vectorProcess.end();
		Iter++)
	{
		AddProcessItem(*Iter);
	}
}
void CListProcessHook::AddProcessItem(PROCESS_INFO item)
{
	CString szPid ;

	CString szImage = item.szPath;
	szImage = szImage.Right(szImage.GetLength() - szImage.ReverseFind('\\') - 1);

	szPid.Format(L"%d", item.ulPid);

	int nCnt = m_listProcess.GetItemCount();
	m_listProcess.InsertItem(nCnt, szPid);
	m_listProcess.SetItemText(nCnt, 1, szImage);

	m_listProcess.SetItemData(nCnt, nCnt);
}

void CListProcessHook::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	ShowAllProcess();
	Dlg = this;
}


void CListProcessHook::OnRclickEnumprocessList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_RESFRESH, L"刷新");
	menu.AppendMenu(MF_SEPARATOR);


	menu.AppendMenu(MF_STRING, ID_CHECK_HOOK, L"检测");
	menu.AppendMenu(MF_SEPARATOR);

	CPoint pt;
	GetCursorPos(&pt);
	menu.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	menu.DestroyMenu();
	*pResult = 0;
}

void CListProcessHook::OnResfresh()
{
	ShowAllProcess();
}

PPROCESS_INFO CListProcessHook::GetInfoByItem(int nItem)
{
	PPROCESS_INFO pInfo = NULL;

	if (nItem == -1)
	{
		return pInfo;
	}

	int nData = (int)m_listProcess.GetItemData(nItem);
	pInfo = &m_vectorProcess.at(nData);

	return pInfo;
}

PVOID CListProcessHook::DumpDllModuleSection(ULONG dwPid,ULONG_PTR dwBase, DWORD dwSize)
{
	PVOID pSection = NULL;

	if (dwBase <= 0 || dwSize <= 0)
	{
		return pSection;
	}

	return m_clsListModules.DumpModuleMemory(dwPid, dwBase, dwSize);
}


void CListProcessHook::OnCheckHook()
{
	clearWork();
	if (m_listProcess.m_hWnd && m_listHookInfo.m_hWnd && !m_bIsRun) {

		int nItem = m_Functions.GetSelectItem(&m_listProcess);
		if (-1 == nItem)
		{
			return;
		}

		HANDLE dwPid = 0;
		PPROCESS_INFO pInfo = GetInfoByItem(nItem);
		if (!pInfo)return;


		DWORD dwThreadId = 0;
		m_hThread = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)EnumHookProc,
			pInfo,
			0,
			&dwThreadId
		);

	}


}

UINT CListProcessHook::AlignSize(UINT nSize, UINT nAlign)
{
	return ((nSize + nAlign - 1) / nAlign * nAlign);
}
BOOL CListProcessHook::ImageFile(PVOID FileBuffer, PVOID* ImageModuleBase)
{
	PIMAGE_DOS_HEADER ImageDosHeader = NULL;
	PIMAGE_NT_HEADERS ImageNtHeaders = NULL;
	PIMAGE_SECTION_HEADER ImageSectionHeader = NULL;
	DWORD FileAlignment = 0, SectionAlignment = 0, NumberOfSections = 0, SizeOfImage = 0, SizeOfHeaders = 0;
	DWORD Index = 0;
	PVOID ImageBase = NULL;
	DWORD SizeOfNtHeaders = 0;

	if (!FileBuffer || !ImageModuleBase)
	{
		return FALSE;
	}

	__try
	{
		ImageDosHeader = (PIMAGE_DOS_HEADER)FileBuffer;
		if (ImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		{
			return FALSE;
		}

		ImageNtHeaders = (PIMAGE_NT_HEADERS)((ULONG64)FileBuffer + ImageDosHeader->e_lfanew);
		if (ImageNtHeaders->Signature != IMAGE_NT_SIGNATURE)
		{
			return FALSE;
		}

		FileAlignment = ImageNtHeaders->OptionalHeader.FileAlignment;
		SectionAlignment = ImageNtHeaders->OptionalHeader.SectionAlignment;
		NumberOfSections = ImageNtHeaders->FileHeader.NumberOfSections;
		SizeOfImage = ImageNtHeaders->OptionalHeader.SizeOfImage;
		SizeOfHeaders = ImageNtHeaders->OptionalHeader.SizeOfHeaders;
		SizeOfImage = AlignSize(SizeOfImage, SectionAlignment);

		ImageBase = malloc(SizeOfImage);
		if (ImageBase == NULL)
		{
			return FALSE;
		}

		ZeroMemory(ImageBase, SizeOfImage);

		SizeOfNtHeaders = sizeof(ImageNtHeaders->FileHeader) + sizeof(ImageNtHeaders->Signature) + ImageNtHeaders->FileHeader.SizeOfOptionalHeader;
		ImageSectionHeader = (PIMAGE_SECTION_HEADER)((ULONG64)ImageNtHeaders + SizeOfNtHeaders);
		
		for (Index = 0; Index < NumberOfSections; Index++)
		{
			ImageSectionHeader[Index].SizeOfRawData = AlignSize(ImageSectionHeader[Index].SizeOfRawData, FileAlignment);
			ImageSectionHeader[Index].Misc.VirtualSize = AlignSize(ImageSectionHeader[Index].Misc.VirtualSize, SectionAlignment);
		}

		if (ImageSectionHeader[NumberOfSections - 1].VirtualAddress + ImageSectionHeader[NumberOfSections - 1].SizeOfRawData > SizeOfImage)
		{
			ImageSectionHeader[NumberOfSections - 1].SizeOfRawData = SizeOfImage - ImageSectionHeader[NumberOfSections - 1].VirtualAddress;
		}

		CopyMemory(ImageBase, FileBuffer, SizeOfHeaders);

		for (Index = 0; Index < NumberOfSections; Index++)
		{
			DWORD FileOffset = ImageSectionHeader[Index].PointerToRawData;
			DWORD Length = ImageSectionHeader[Index].SizeOfRawData;
			ULONG64 ImageOffset = ImageSectionHeader[Index].VirtualAddress;
			CopyMemory(&((PBYTE)ImageBase)[ImageOffset], &((PBYTE)FileBuffer)[FileOffset], Length);
		}

		*ImageModuleBase = ImageBase;
	}
	__except (1)
	{
		if (ImageBase)
		{
			free(ImageBase);
			ImageBase = NULL;
		}

		*ImageModuleBase = NULL;
		return FALSE;
	}

	return TRUE;
}

PVOID CListProcessHook::MapFileWithoutFix(CString szPath)
{
	if (szPath.IsEmpty() || !PathFileExists(szPath))
	{
		return NULL;
	}

	HANDLE hFile = CreateFile(
		szPath,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	DWORD dwSize = GetFileSize(hFile, NULL);
	if (dwSize == 0)
	{
		CloseHandle(hFile);
		return NULL;
	}

	PVOID pBuffer = malloc(dwSize);
	if (!pBuffer)
	{
		CloseHandle(hFile);
		return NULL;
	}

	DWORD dwRet = 0;
	if (!ReadFile(hFile, pBuffer, dwSize, &dwRet, NULL))
	{
		CloseHandle(hFile);
		free(pBuffer);
		return NULL;
	}

	CloseHandle(hFile);

	
	PVOID ImageBase = NULL;
	
	if (!ImageFile((PBYTE)pBuffer, &ImageBase) || ImageBase == NULL)
	{
		free(pBuffer);
		return NULL;
	}

	//DebugLog(L"New ImageBase: 0x%08X", ImageBase);
	free(pBuffer);
	
	return ImageBase;
}

BOOL CListProcessHook::GetPeImportTable(BYTE *ImageBase, PIMAGE_IMPORT_DESCRIPTOR *ImageImportDescriptor)
{
	BOOL bRet = FALSE;

	if (!ImageBase || !ImageImportDescriptor)
	{
		return bRet;
	}

	*ImageImportDescriptor = NULL;

	__try
	{
		PIMAGE_DOS_HEADER ImageDosHeader = NULL;
		PIMAGE_NT_HEADERS ImageNtHeaders = NULL;

		ImageDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
		if (ImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		{
			return bRet;
		}

		ImageNtHeaders = (PIMAGE_NT_HEADERS)((ULONG64)ImageBase + ImageDosHeader->e_lfanew);
		if (ImageNtHeaders->Signature != IMAGE_NT_SIGNATURE)
		{
			return bRet;
		}
	
		DWORD ImportSize = 0;
		if (!RtlImageDirectoryEntryToData) {
			RtlImageDirectoryEntryToData = (pfnImageDirectoryEntryToData)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlImageDirectoryEntryToData");
		}
		if (!RtlImageDirectoryEntryToData) return FALSE;
		*ImageImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)RtlImageDirectoryEntryToData(ImageBase, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ImportSize);
		bRet = TRUE;
	}
	__except (1)
	{
		*ImageImportDescriptor = NULL;
		bRet = FALSE;
	}

	return bRet;
}

PVOID CListProcessHook::MiFindExportedRoutine(IN PVOID DllBase,BOOL ByName,IN char *RoutineName,DWORD Ordinal)
{
	USHORT OrdinalNumber = 0;
	PULONG NameTableBase = NULL;
	PUSHORT NameOrdinalTableBase = NULL;
	PULONG AddressTableBase = NULL;
	LONG High = 0;
	LONG Low = 0;
	LONG Middle = 0;
	LONG Result = 0;
	ULONG ExportSize = 0;
	PVOID FunctionAddress = NULL;
	PIMAGE_EXPORT_DIRECTORY ExportDirectory = NULL;

	if (!DllBase)
	{
		return NULL;
	}

	PIMAGE_DOS_HEADER ImageDosHeader = NULL;
	PIMAGE_NT_HEADERS ImageNtHeaders = NULL;

	__try
	{
		ImageDosHeader = (PIMAGE_DOS_HEADER)DllBase;
		if (ImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		{
			return NULL;
		}

		ImageNtHeaders = (PIMAGE_NT_HEADERS)((ULONG_PTR)DllBase + ImageDosHeader->e_lfanew);
		if (ImageNtHeaders->Signature != IMAGE_NT_SIGNATURE)
		{
			return NULL;
		}

		ExportDirectory = (PIMAGE_EXPORT_DIRECTORY)RtlImageDirectoryEntryToData(
			DllBase,
			TRUE,
			IMAGE_DIRECTORY_ENTRY_EXPORT,
			&ExportSize);

		if (ExportDirectory == NULL) {
			return NULL;
		}

		//
		// Initialize the pointer to the array of RVA-based ansi export strings.
		//

		NameTableBase = (PULONG)((PCHAR)DllBase + (ULONG)ExportDirectory->AddressOfNames);

		//
		// Initialize the pointer to the array of USHORT ordinal numbers.
		//

		NameOrdinalTableBase = (PUSHORT)((PCHAR)DllBase + (ULONG)ExportDirectory->AddressOfNameOrdinals);

		AddressTableBase = (PULONG)((PCHAR)DllBase + (ULONG)ExportDirectory->AddressOfFunctions);

		if (!ByName)
		{
			return (PVOID)(AddressTableBase[Ordinal] + (ULONG_PTR)DllBase);
		}

		//
		// Lookup the desired name in the name table using a binary search.
		//

		Low = 0;
		Middle = 0;
		High = ExportDirectory->NumberOfNames - 1;

		while (High >= Low) {

			//
			// Compute the next probe index and compare the import name
			// with the export name entry.
			//

			Middle = (Low + High) >> 1;
			// 返回 Name Ordinal 标位
			Result = strcmp(RoutineName,
				(PCHAR)DllBase + NameTableBase[Middle]);

			if (Result < 0) {
				High = Middle - 1;
			}
			else if (Result > 0) {
				Low = Middle + 1;
			}
			else {
				break;
			}
		}
		//
		// If the high index is less than the low index, then a matching
		// table entry was not found. Otherwise, get the ordinal number
		// from the ordinal table.
		//

		if (High < Low) {
			return NULL;
		}

		OrdinalNumber = NameOrdinalTableBase[Middle];

		//
		// If the OrdinalNumber is not within the Export Address Table,
		// then this image does not implement the function.  Return not found.
		//

		if ((ULONG)OrdinalNumber >= ExportDirectory->NumberOfFunctions) {
			return NULL;
		}

		//
		// Index into the array of RVA export addresses by ordinal number.
		//

		FunctionAddress = (PVOID)((PCHAR)DllBase + AddressTableBase[OrdinalNumber]);

		//
		// Forwarders are not used by the kernel and HAL to each other.
		//

		// 	if (FunctionAddress >= (PVOID)ExportDirectory &&
		// 		FunctionAddress <= (PVOID)((PCHAR)ExportDirectory + ExportSize)) 
		// 	{
		// 		return NULL;
		// 	}

		// 	ASSERT ((FunctionAddress <= (PVOID)ExportDirectory) ||
		// 		(FunctionAddress >= (PVOID)((PCHAR)ExportDirectory + ExportSize)));
	}
	__except (1)
	{
		FunctionAddress = NULL;
	}

	return FunctionAddress;
}
void CListProcessHook::GetDllModuleBase(CString szModuleName, ULONG_PTR *dwRet)
{
	if (szModuleName.IsEmpty() || !dwRet)
	{
		return;
	}

	// 	for ( vector <MODULE_INFO>::iterator DllModuleIter = m_DllModuleDlg.m_PebModuleVector.begin(); 
	// 		DllModuleIter != m_DllModuleDlg.m_PebModuleVector.end(); 
	// 		DllModuleIter++)
	// 	{
	// 		CString szPath = DllModuleIter->Path;
	// 		CString szName = szPath.Right(szPath.GetLength() - szPath.ReverseFind('\\') - 1);
	// 
	// 		if (!szName.CompareNoCase(szModuleName))
	// 		{
	// 			*dwRet = DllModuleIter->Base;
	// 			break;
	// 		}
	// 	}

	for (list <DUMP_INFO>::iterator ir = m_DumpList.begin();
		ir != m_DumpList.end();
		ir++)
	{
		CString szPath = ir->szPath;
		CString szName = szPath.Right(szPath.GetLength() - szPath.ReverseFind('\\') - 1);

		if (m_bIs64) {
			if (!szName.CompareNoCase(szModuleName))
			{
				*dwRet = ir->nBase;
				break;
			}
		}
		else {
			if (!szName.CompareNoCase(szModuleName) && ir->nBase < 0x7fffffff)
			{
				*dwRet = ir->nBase;
				break;
			}
		}
	}
}
PVOID CListProcessHook::GetReloadBuffer(ULONG_PTR dwBase)
{
	PVOID pBuffer = NULL;
	if (dwBase)
	{
		for (list<DUMP_INFO>::iterator ir = m_DumpList.begin();
			ir != m_DumpList.end();
			ir++)
		{
			if (ir->nBase == dwBase)
			{
				pBuffer = ir->pReloadBuffer;
				break;
			}
		}
	}

	return pBuffer;
}
BOOL CListProcessHook::FixImportTable(BYTE *ImageBase, ULONG_PTR ExistImageBase)
{
	PIMAGE_IMPORT_DESCRIPTOR ImageImportDescriptor = NULL;
	PIMAGE_THUNK_DATA ImageThunkData = NULL, FirstThunk = NULL;
	PIMAGE_THUNK_DATA32 ImageThunkData32 = NULL, FirstThunk32 = NULL;
	PIMAGE_IMPORT_BY_NAME ImortByName = NULL;
	DWORD ImportSize = 0;
	ULONG_PTR FunctionAddress = 0;
	PIMAGE_DOS_HEADER ImageDosHeader = NULL;
	PIMAGE_NT_HEADERS ImageNtHeaders = NULL;


	if (!GetPeImportTable(ImageBase, &ImageImportDescriptor))//查找PE 文件的 ImageImportDescriptor
	{
		return FALSE;
	}

	if (ImageImportDescriptor == NULL)
	{
		return TRUE;
	}

	while (ImageImportDescriptor->OriginalFirstThunk && ImageImportDescriptor->Name)
	{
		CHAR ModuleName[MAX_PATH] = { 0 };
		strcpy_s(ModuleName, MAX_PATH, (CHAR*)(ImageBase + ImageImportDescriptor->Name));
		CString szModule = m_Functions.ms2ws(ModuleName);
		ULONG_PTR ModuleBase = 0;
		GetDllModuleBase(szModule, &ModuleBase);

		if (ModuleBase == 0)
		{
			// 没能dump下来 就下一个 例如 32位的 kernel32.dll readWPI 就会返回 299错误 奇怪了
			/*
			szModule.MakeUpper();
			if (szModule.Find(L"API-MS-WIN") == -1)
			{
				return FALSE;
			}
			*/
			ImageImportDescriptor++;
			continue;
		}

		PVOID pReloadBuffer = GetReloadBuffer(ModuleBase);// 获取 文件 buffer
		if (pReloadBuffer == NULL)
		{
			return FALSE;
			ImageImportDescriptor++;
			continue;
		}

		if (m_bIs64) {
			ImageThunkData = (PIMAGE_THUNK_DATA)(ImageBase + ImageImportDescriptor->OriginalFirstThunk);//PE文件的 IAT
			FirstThunk = (PIMAGE_THUNK_DATA)(ImageBase + ImageImportDescriptor->FirstThunk);            //PE文件的 IAT

			while (ImageThunkData && FirstThunk && ImageThunkData->u1.Ordinal)
			{
				// 序号导入
				if (IMAGE_SNAP_BY_ORDINAL64(ImageThunkData->u1.Ordinal))
				{
					// 根据PE文件的IAT 信息  获取 -------> 内存中 的IAT 函数地址
					FunctionAddress = (ULONG_PTR)MiFindExportedRoutine((PVOID)pReloadBuffer, FALSE, NULL, ImageThunkData->u1.Ordinal & ~IMAGE_ORDINAL_FLAG64);
					if (FunctionAddress == 0)
					{
						//DebugLog(L"can't find funcion Index %d", ImageThunkData->u1.Ordinal & ~IMAGE_ORDINAL_FLAG32);
						FirstThunk++;
						ImageThunkData++;
						continue;
					}

					FunctionAddress = FunctionAddress - (ULONG_PTR)pReloadBuffer + ModuleBase;
					FirstThunk->u1.Function = FunctionAddress;
				}
				else // 函数名导入
				{
					ImortByName = (PIMAGE_IMPORT_BY_NAME)(ImageBase + ImageThunkData->u1.AddressOfData);
					FunctionAddress = (ULONG_PTR)MiFindExportedRoutine((PVOID)pReloadBuffer, TRUE, (CHAR*)ImortByName->Name, 0);

					if (FunctionAddress == 0)
					{
						//DebugLog(L"can't Funcion Name:%s", m_Functions.ms2ws((CHAR*)ImortByName->Name));
						return FALSE;
					}

					FunctionAddress = FunctionAddress - (ULONG_PTR)pReloadBuffer + ModuleBase;
					FirstThunk->u1.Function = FunctionAddress;
				}

				FirstThunk++;
				ImageThunkData++;
			}

			ImageImportDescriptor++;
		}
		else {
			ImageThunkData32 = ((PIMAGE_THUNK_DATA32)(ImageBase + ImageImportDescriptor->OriginalFirstThunk));
			FirstThunk32 = ((PIMAGE_THUNK_DATA32)(ImageBase + ImageImportDescriptor->FirstThunk));
			while (ImageThunkData32 && FirstThunk32 && ImageThunkData32->u1.Ordinal)
			{
				// 序号导入
				if (IMAGE_SNAP_BY_ORDINAL32(ImageThunkData32->u1.Ordinal))
				{

					FunctionAddress = (ULONG_PTR)MiFindExportedRoutine((PVOID)pReloadBuffer, FALSE, NULL, ImageThunkData32->u1.Ordinal & ~IMAGE_ORDINAL_FLAG32);
					if (FunctionAddress == 0)
					{
						//DebugLog(L"can't find funcion Index %d", ImageThunkData->u1.Ordinal & ~IMAGE_ORDINAL_FLAG32);
						FirstThunk32++;
						ImageThunkData32++;
						continue;
					}

					FunctionAddress = FunctionAddress - (ULONG_PTR)pReloadBuffer + ModuleBase;
					FirstThunk32->u1.Function = FunctionAddress;
				}
				else // 函数名导入
				{

					ImortByName = (PIMAGE_IMPORT_BY_NAME)(ImageBase + ImageThunkData32->u1.AddressOfData);
					FunctionAddress = (ULONG_PTR)MiFindExportedRoutine((PVOID)pReloadBuffer, TRUE, (CHAR*)ImortByName->Name, 0);

					if (FunctionAddress == 0)
					{
						//DebugLog(L"can't Funcion Name:%s", m_Functions.ms2ws((CHAR*)ImortByName->Name));
						return FALSE;
					}

					FunctionAddress = FunctionAddress - (ULONG_PTR)pReloadBuffer + ModuleBase;
					FirstThunk32->u1.Function = FunctionAddress;
				}

				FirstThunk32++;
				ImageThunkData32++;
			}

			ImageImportDescriptor++;
		}

	}
	return TRUE;
}
BOOL CListProcessHook::FixBaseRelocTable(PVOID NewImageBase,ULONG_PTR ExistImageBase
)
/*++

Routine Description:

	This routine relocates an image file that was not loaded into memory
	at the preferred address.

Arguments:

	NewBase - Supplies a pointer to the image base.

	AdditionalBias - An additional quantity to add to all fixups.  The
					 32-bit X86 loader uses this when loading 64-bit images
					 to specify a NewBase that is actually a 64-bit value.

	LoaderName - Indicates which loader routine is being called from.

	Success - Value to return if relocation successful.

	Conflict - Value to return if can't relocate.

	Invalid - Value to return if relocations are invalid.

Return Value:

	Success if image is relocated.
	Conflict if image can't be relocated.
	Invalid if image contains invalid fixups.

--*/

{
	LONGLONG Diff;
	ULONG TotalCountBytes = 0;
	ULONG_PTR VA;
	ULONGLONG OriginalImageBase;
	ULONG SizeOfBlock;
	PUSHORT NextOffset = NULL;
	PIMAGE_BASE_RELOCATION NextBlock;
	PIMAGE_DOS_HEADER ImageDosHeader = NULL;
	PIMAGE_NT_HEADERS ImageNtHeaders = NULL;

	__try
	{
		ImageDosHeader = (PIMAGE_DOS_HEADER)NewImageBase;
		if (ImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		{
			return FALSE;
		}

		ImageNtHeaders = (PIMAGE_NT_HEADERS)((ULONG_PTR)NewImageBase + ImageDosHeader->e_lfanew);
		if (ImageNtHeaders->Signature != IMAGE_NT_SIGNATURE)
		{
			return FALSE;
		}

		switch (ImageNtHeaders->OptionalHeader.Magic) {

		case IMAGE_NT_OPTIONAL_HDR32_MAGIC:

			OriginalImageBase =
				((PIMAGE_NT_HEADERS32)ImageNtHeaders)->OptionalHeader.ImageBase;
			break;

		case IMAGE_NT_OPTIONAL_HDR64_MAGIC:

			OriginalImageBase =
				((PIMAGE_NT_HEADERS64)ImageNtHeaders)->OptionalHeader.ImageBase;
			break;

		default:
			return FALSE;
		}

		//
		// Locate the relocation section.
		//

		NextBlock = (PIMAGE_BASE_RELOCATION)RtlImageDirectoryEntryToData(
			NewImageBase, TRUE, IMAGE_DIRECTORY_ENTRY_BASERELOC, &TotalCountBytes);

		// 如果没有导入表，直接返回TRUE
		if (!NextBlock || !TotalCountBytes)
		{
			return TRUE;
		}

		Diff = (ULONG_PTR)ExistImageBase - OriginalImageBase;
		while (TotalCountBytes)
		{
			SizeOfBlock = NextBlock->SizeOfBlock;
			TotalCountBytes -= SizeOfBlock;
			SizeOfBlock -= sizeof(IMAGE_BASE_RELOCATION);
			SizeOfBlock /= sizeof(USHORT);
			NextOffset = (PUSHORT)((PCHAR)NextBlock + sizeof(IMAGE_BASE_RELOCATION));

			VA = (ULONG_PTR)NewImageBase + NextBlock->VirtualAddress;
			if (!LdrProcessRelocationBlock) {
				LdrProcessRelocationBlock = (pfnLdrProcessRelocationBlock)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "LdrProcessRelocationBlock");
			}
			if (!LdrProcessRelocationBlock) return FALSE;
			if (!(NextBlock = LdrProcessRelocationBlock(VA,
				SizeOfBlock,
				NextOffset,
				Diff)))
			{
				return FALSE;
			}
		}
	}
	__except (1)
	{
		return FALSE;
	}

	return TRUE;
}
void CListProcessHook::FixImprtAndReacDir()
{
	for (list<DUMP_INFO>::iterator ir = m_DumpList.begin();
		m_bIsRun && ir != m_DumpList.end();
		ir++)
	{
		if (ir->pReloadBuffer && ir->nBase)
		{
			if (ir->nBase > 0x7fffffff)
			{
				m_bIs64 = TRUE;
				ir->bIs64 = TRUE;
			}
			else {
				m_bIs64 = FALSE;
				ir->bIs64 = FALSE;
			}
			if (FixImportTable((PBYTE)ir->pReloadBuffer, ir->nBase) &&
				FixBaseRelocTable((PBYTE)ir->pReloadBuffer, ir->nBase))
			{
				ir->bFixed = TRUE;
			}
		}
	}
}
void CListProcessHook::GetDllModuleBaseAndSize(CString szModuleName, ULONG_PTR *dwBase, ULONG_PTR *dwSize)
{
	if (szModuleName.IsEmpty() || !dwBase || !dwSize)
	{
		return;
	}

	for (vector <MODULE_INFO_EX>::iterator DllModuleIter = m_vectorModules.begin();
		DllModuleIter != m_vectorModules.end();
		DllModuleIter++)
	{
		CString szPath = DllModuleIter->Path;
		CString szName = szPath.Right(szPath.GetLength() - szPath.ReverseFind('\\') - 1);

		if (!szName.CompareNoCase(szModuleName))
		{
			*dwBase = DllModuleIter->Base;
			*dwSize = DllModuleIter->Size;
			break;
		}
	}
}

VOID CListProcessHook::GetImageBoundImport(PIMAGE_BOUND_IMPORT_DESCRIPTOR ImageBoundImportDescriptor, list<BOUND_IMPORT_INFO> &m_BoundList)
{
	if (ImageBoundImportDescriptor)
	{
		for (int i = 0; i < 0x20; i++)
		{
			if (IsBadReadPtr((PBYTE)ImageBoundImportDescriptor + i * sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR), sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR)) ||
				ImageBoundImportDescriptor[i].TimeDateStamp == 0 ||
				ImageBoundImportDescriptor[i].OffsetModuleName == 0)
			{
				break;
			}

			if (IsBadStringPtrA((PCHAR)ImageBoundImportDescriptor + ImageBoundImportDescriptor[i].OffsetModuleName, 1))
			{
				break;
			}

			CString szModule = m_Functions.ms2ws((PCHAR)ImageBoundImportDescriptor + ImageBoundImportDescriptor[i].OffsetModuleName);

			BOUND_IMPORT_INFO info;
			info.dwRefBase = info.dwRefSize = 0;
			info.szModule = szModule;
			info.szRefModule = szModule;
			info.dwRefTimeDateStamp = ImageBoundImportDescriptor[i].TimeDateStamp;
			GetDllModuleBaseAndSize(info.szRefModule, &info.dwRefBase, &info.dwRefSize);
			m_BoundList.push_back(info);

			if (ImageBoundImportDescriptor[i].NumberOfModuleForwarderRefs > 0)
			{
				int nRefs = ImageBoundImportDescriptor[i].NumberOfModuleForwarderRefs;

				for (int j = 1; j <= nRefs; j++)
				{
					i += j;

					BOUND_IMPORT_INFO info;
					info.dwRefBase = info.dwRefSize = 0;
					info.szModule = szModule;

					if (IsBadReadPtr((PBYTE)ImageBoundImportDescriptor + i * sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR), sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR)) ||
						ImageBoundImportDescriptor[i].TimeDateStamp == 0 ||
						ImageBoundImportDescriptor[i].OffsetModuleName == 0)
					{
						break;
					}

					if (IsBadStringPtrA((PCHAR)ImageBoundImportDescriptor + ImageBoundImportDescriptor[i].OffsetModuleName, 1))
					{
						break;
					}

					info.szRefModule = m_Functions.ms2ws((PCHAR)ImageBoundImportDescriptor + ImageBoundImportDescriptor[i].OffsetModuleName);
					info.dwRefTimeDateStamp = ImageBoundImportDescriptor[i].TimeDateStamp;
					GetDllModuleBaseAndSize(info.szRefModule, &info.dwRefBase, &info.dwRefSize);
					m_BoundList.push_back(info);
				}
			}
		}
	}
}

ULONG_PTR CListProcessHook::FindExportedRoutineInReloadModule(IN ULONG_PTR DllBase, IN char *RoutineName)
{
	USHORT OrdinalNumber;
	PULONG NameTableBase;
	PUSHORT NameOrdinalTableBase;
	PULONG AddressTableBase;
	LONG High;
	LONG Low;
	LONG Middle;
	LONG Result;
	ULONG ExportSize;
	PIMAGE_EXPORT_DIRECTORY ExportDirectory;

	if (!DllBase || !RoutineName)
	{
		return 0;
	}

	PIMAGE_DOS_HEADER ImageDosHeader = NULL;
	PIMAGE_NT_HEADERS ImageNtHeaders = NULL;
	ULONG_PTR FunctionAddress = 0;

	__try
	{
		ImageDosHeader = (PIMAGE_DOS_HEADER)DllBase;
		if (ImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		{
			return 0;
		}

		ImageNtHeaders = (PIMAGE_NT_HEADERS)((ULONG_PTR)DllBase + ImageDosHeader->e_lfanew);
		if (ImageNtHeaders->Signature != IMAGE_NT_SIGNATURE)
		{
			return 0;
		}

		ExportDirectory = (PIMAGE_EXPORT_DIRECTORY)RtlImageDirectoryEntryToData(
			(PVOID)DllBase,
			TRUE,
			IMAGE_DIRECTORY_ENTRY_EXPORT,
			&ExportSize);

		if (ExportDirectory == NULL) {
			return 0;
		}

		//
		// Initialize the pointer to the array of RVA-based ansi export strings.
		//

		NameTableBase = (PULONG)(DllBase + (ULONG_PTR)ExportDirectory->AddressOfNames);

		//
		// Initialize the pointer to the array of USHORT ordinal numbers.
		//

		NameOrdinalTableBase = (PUSHORT)(DllBase + (ULONG_PTR)ExportDirectory->AddressOfNameOrdinals);

		AddressTableBase = (PULONG)(DllBase + (ULONG_PTR)ExportDirectory->AddressOfFunctions);

		//
		// Lookup the desired name in the name table using a binary search.
		//

		Low = 0;
		Middle = 0;
		High = ExportDirectory->NumberOfNames - 1;

		while (High >= Low) {

			//
			// Compute the next probe index and compare the import name
			// with the export name entry.
			//

			Middle = (Low + High) >> 1;

			char *zx = (PCHAR)(DllBase + NameTableBase[Middle]);
			Result = strcmp(RoutineName, zx);

			if (Result < 0) {
				High = Middle - 1;
			}
			else if (Result > 0) {
				Low = Middle + 1;
			}
			else {
				break;
			}
		}
		//
		// If the high index is less than the low index, then a matching
		// table entry was not found. Otherwise, get the ordinal number
		// from the ordinal table.
		//

		if (High < Low) {
			return 0;
		}

		OrdinalNumber = NameOrdinalTableBase[Middle];

		//
		// If the OrdinalNumber is not within the Export Address Table,
		// then this image does not implement the function.  Return not found.
		//

		if ((ULONG_PTR)OrdinalNumber >= ExportDirectory->NumberOfFunctions)
		{
			return 0;
		}

		//
		// Index into the array of RVA export addresses by ordinal number.
		//


		FunctionAddress = (ULONG_PTR)((PCHAR)DllBase + AddressTableBase[OrdinalNumber]);

		//
		// Forwarders are not used by the kernel and HAL to each other.
		//

		if (FunctionAddress >= (ULONG_PTR)ExportDirectory && FunctionAddress <= (ULONG_PTR)((PCHAR)ExportDirectory + ExportSize))
		{
			return 0;
		}

		// 	ASSERT ((FunctionAddress <= (PVOID)ExportDirectory) ||
		// 		(FunctionAddress >= (PVOID)((PCHAR)ExportDirectory + ExportSize)));

		FunctionAddress = FunctionAddress - (ULONG_PTR)DllBase;
	}
	__except (1)
	{
		FunctionAddress = 0;
	}

	return FunctionAddress;
}
ULONG_PTR CListProcessHook::GetImportFuncOffset(PVOID ImageBase, CHAR *szFunc)
{
	ULONG_PTR dwFunc = 0;
	if (!ImageBase || !szFunc)
	{
		return 0;
	}

	dwFunc = FindExportedRoutineInReloadModule((ULONG_PTR)ImageBase, szFunc);

	return dwFunc;
}

BOOL CListProcessHook::IsBoundImport(CString szModule, ULONG_PTR dwFunction, list<BOUND_IMPORT_INFO> BoundList)
{
	BOOL bRet = FALSE;
	if (BoundList.size() == 0 || szModule.IsEmpty() || dwFunction <= 0)
	{
		return bRet;
	}

	for (list<BOUND_IMPORT_INFO>::iterator itor = BoundList.begin();
		itor != BoundList.end();
		itor++)
	{
		if (!szModule.CompareNoCase(itor->szModule) &&
			dwFunction >= itor->dwRefBase &&
			dwFunction <= itor->dwRefBase + itor->dwRefSize)
		{
			bRet = TRUE;
			break;
		}
	}

	return bRet;
}

void CListProcessHook::EnumIatHook(PVOID pSection, CString szPath, ULONG_PTR nBase)
{
	if (pSection == NULL || szPath.IsEmpty() || !nBase)
	{
		return;
	}

	PIMAGE_IMPORT_DESCRIPTOR ImageImportDescriptor = NULL;
	PIMAGE_BOUND_IMPORT_DESCRIPTOR ImageBoundImportDescriptor = NULL;
	PIMAGE_THUNK_DATA ImageThunkData = NULL, FirstThunk = NULL;
	PIMAGE_THUNK_DATA32 ImageThunkData32 = NULL, FirstThunk32 = NULL;
	PIMAGE_IMPORT_BY_NAME ImortByName = NULL;
	ULONG ImportSize = 0;
	ULONG_PTR dwImportFunctionAddress = 0;
	CString szHookedModule = szPath.Right(szPath.GetLength() - szPath.ReverseFind('\\') - 1);
	szHookedModule.MakeLower();

	ImageImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)RtlImageDirectoryEntryToData(pSection, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ImportSize);
	if (ImageImportDescriptor == NULL || ImportSize == 0)
	{
		return;
	}

	
	ImageBoundImportDescriptor = (PIMAGE_BOUND_IMPORT_DESCRIPTOR)RtlImageDirectoryEntryToData(pSection, TRUE, IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT, &ImportSize);
	if (ImageBoundImportDescriptor && ImportSize)
	{
		GetImageBoundImport(ImageBoundImportDescriptor, m_BoundList);
	}

	while (ImageImportDescriptor->OriginalFirstThunk && ImageImportDescriptor->Name)
	{
		CHAR szImportModule[MAX_PATH] = { 0 };
		strcpy_s(szImportModule, MAX_PATH, (CHAR*)((ULONG_PTR)pSection + ImageImportDescriptor->Name));
		CString wsImportModule = m_Functions.ms2ws(szImportModule);

		ULONG_PTR dwImportModuleBase = 0;
		GetDllModuleBase(wsImportModule, &dwImportModuleBase);

		if (dwImportModuleBase == 0)
		{
			ImageImportDescriptor++;
			continue;
		}

		PVOID pBuffer = GetReloadBuffer(dwImportModuleBase);
		if (pBuffer == NULL)
		{
			ImageImportDescriptor++;
			continue;
		}

		ImageBoundImportDescriptor = (PIMAGE_BOUND_IMPORT_DESCRIPTOR)RtlImageDirectoryEntryToData(pBuffer, TRUE, IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT, &ImportSize);
		if (ImageBoundImportDescriptor && ImportSize)
		{
			GetImageBoundImport(ImageBoundImportDescriptor, m_BoundList);
		}
		if (m_bIs64)
		{
			ImageThunkData = (PIMAGE_THUNK_DATA)((ULONG_PTR)pSection + ImageImportDescriptor->OriginalFirstThunk);
			FirstThunk = (PIMAGE_THUNK_DATA)((ULONG_PTR)pSection + ImageImportDescriptor->FirstThunk);
			while (!IsBadReadPtr(ImageThunkData, sizeof(IMAGE_THUNK_DATA)) &&
				!IsBadReadPtr(FirstThunk, sizeof(IMAGE_THUNK_DATA)) &&
				ImageThunkData->u1.Ordinal)
			{
				// import by name
				if (!IMAGE_SNAP_BY_ORDINAL64(ImageThunkData->u1.Ordinal))
				{
					ImortByName = (PIMAGE_IMPORT_BY_NAME)((ULONG_PTR)pSection + ImageThunkData->u1.AddressOfData);

					if (IsBadStringPtrA((CHAR*)ImortByName->Name, 1))
					{
						break;
					}

					dwImportFunctionAddress = FirstThunk->u1.Function;
					//从PE 文件 获取 函数偏移
					ULONG_PTR dwImportFuncOffset = GetImportFuncOffset(pBuffer, (CHAR*)ImortByName->Name);
					if (dwImportFunctionAddress && dwImportFuncOffset)
					{
						ULONG_PTR dwOriginAddress = dwImportModuleBase + dwImportFuncOffset;
						if (dwImportFunctionAddress != dwOriginAddress)
						{
							// is bound import
							if (!IsBoundImport(wsImportModule, dwImportFunctionAddress, m_BoundList))
							{
								IAT_HOOK_INFO iat;
								iat.szHookedModule = szHookedModule;
								iat.szExpModule = wsImportModule.MakeLower();
								iat.szFunction = m_Functions.ms2ws((CHAR*)ImortByName->Name);
								iat.dwHookAddress = dwImportFunctionAddress;
								iat.dwOriginAddress = dwOriginAddress;
								iat.dwIatAddress = (ULONG)FirstThunk - (ULONG)pSection + nBase;
								m_IatHookList.push_back(iat);
							}
						}
					}
				}

				FirstThunk++;
				ImageThunkData++;
			}

			ImageImportDescriptor++;
		}
		else {
			//32 位
			ImageThunkData32 = (PIMAGE_THUNK_DATA32)((ULONG_PTR)pSection + ImageImportDescriptor->OriginalFirstThunk);
			FirstThunk32 = (PIMAGE_THUNK_DATA32)((ULONG_PTR)pSection + ImageImportDescriptor->FirstThunk);
			while (!IsBadReadPtr(ImageThunkData32, sizeof(IMAGE_THUNK_DATA32)) &&
				!IsBadReadPtr(FirstThunk32, sizeof(IMAGE_THUNK_DATA32)) &&
				ImageThunkData32->u1.Ordinal)
			{
				// import by name
				if (!IMAGE_SNAP_BY_ORDINAL32(ImageThunkData32->u1.Ordinal))
				{
					ImortByName = (PIMAGE_IMPORT_BY_NAME)((ULONG_PTR)pSection + ImageThunkData32->u1.AddressOfData);



					if (IsBadStringPtrA((CHAR*)ImortByName->Name, 1))
					{
						break;
					}

					dwImportFunctionAddress = FirstThunk32->u1.Function;
					//从PE 文件 获取 函数偏移
					ULONG_PTR dwImportFuncOffset = GetImportFuncOffset(pBuffer, (CHAR*)ImortByName->Name);
					if (dwImportFunctionAddress && dwImportFuncOffset)
					{
						ULONG_PTR dwOriginAddress = dwImportModuleBase + dwImportFuncOffset;
						if (dwImportFunctionAddress != dwOriginAddress)
						{
							// is bound import
							if (!IsBoundImport(wsImportModule, dwImportFunctionAddress, m_BoundList))
							{
								IAT_HOOK_INFO iat;
								iat.szHookedModule = szHookedModule;
								iat.szExpModule = wsImportModule.MakeLower();
								iat.szFunction = m_Functions.ms2ws((CHAR*)ImortByName->Name);
								iat.dwHookAddress = dwImportFunctionAddress;
								iat.dwOriginAddress = dwOriginAddress;
								iat.dwIatAddress = (ULONG)FirstThunk32 - (ULONG)pSection + nBase;
								m_IatHookList.push_back(iat);
							}
						}
					}
				}

				FirstThunk32++;
				ImageThunkData32++;
			}

			ImageImportDescriptor++;
		}
	}

}

void CListProcessHook::EnumProcessHooks(PPROCESS_INFO pInfo)
{
	m_listHookInfo.DeleteAllItems();
	m_vectorModules.clear();
	m_IatHookList.clear();
	m_InlineHookInfo.clear();
	m_DumpList.clear();
	m_InlineHookCnt = 0;
	m_clsListModules.EnumModulesByPeb((ULONG)pInfo->ulPid, (ULONG_PTR)pInfo->ulEprocess, m_vectorModules);
	//ListProcessModules((ULONG)pInfo->ulPid);


	for (vector <MODULE_INFO_EX>::iterator DllIter = m_vectorModules.begin();
		m_bIsRun && DllIter != m_vectorModules.end();
		DllIter++)
	{
		MODULE_INFO_EX ModuleItem = *DllIter;
		CString strPath = ModuleItem.Path;
		if (strPath.IsEmpty() || !PathFileExists(strPath))
		{
			continue;
		}
		
		//重定位 dll路径
		strPath.MakeLower();
		if (ModuleItem.Sign == 0x32) 
		{
			strPath.Replace(L"system32", L"syswow64");
		}


		PVOID pSectionBuffer = DumpDllModuleSection((ULONG)pInfo->ulPid,ModuleItem.Base, ModuleItem.Size);
		if (!pSectionBuffer)
		{
			continue;
		}
		PVOID pReloadBuffer = MapFileWithoutFix(strPath);//文件来的
		if (!pReloadBuffer)
		{
			free(pSectionBuffer);
			continue;
		}

		DUMP_INFO Dump;
		Dump.nBase = ModuleItem.Base;
		Dump.nSize = ModuleItem.Size;
		Dump.szPath = strPath;
		Dump.pSectionBuffer = pSectionBuffer;
		Dump.pReloadBuffer = pReloadBuffer;
		Dump.bFixed = FALSE;
		Dump.bIs64 = FALSE;
		m_DumpList.push_back(Dump);
	}
	FixImprtAndReacDir();  //这个事 修复 PE文件的

	BOOL bFirst = FALSE;
	for (list<DUMP_INFO>::iterator ir = m_DumpList.begin();
		m_bIsRun && ir != m_DumpList.end();
		ir++)
	{
		m_szStatus.Format(L"%s: %s", L"[Scanning...]", ir->szPath);
		SendMessage(WM_ADD_ITEM_PROCESSHOOK_DLG, 0x2021, 0x2021);

		if (ir->bFixed)
		{
			if (!bFirst)
			{
				if (ir->szPath.Find(L".exe")) {
					EnumIatHook(ir->pSectionBuffer, ir->szPath, ir->nBase);  //内存的
				}
				bFirst = TRUE;
			}
			if (ir->bIs64)
			{
				EnumInlineHook(ir->pSectionBuffer, ir->pReloadBuffer, ir->nBase);
			}
			else
			{
				EnumInlineHook32(ir->pSectionBuffer, ir->pReloadBuffer, ir->nBase);
			}

		}
	}

	SendMessage(WM_ADD_ITEM_PROCESSHOOK_DLG, 0x2022, 0x2022);



	m_szStatus.Format(L"一共有%d个InlineHook", m_InlineHookCnt);
	SendMessage(WM_ADD_ITEM_PROCESSHOOK_DLG, 0x2021, 0x2021);
}




void CListProcessHook::EnumInlineHookEx(PVOID pSection, PVOID pReload, ULONG_PTR dwBase)
{
	if (pSection == NULL || pReload == NULL || dwBase == 0)
	{
		return;
	}

	PIMAGE_EXPORT_DIRECTORY ReloadImageExportDirectory = NULL;
	PIMAGE_DOS_HEADER ImageDosHeader = NULL;
	PIMAGE_NT_HEADERS ImageNtHeaders = NULL;
	ULONG ExportSize = 0;
	PULONG_PTR pReloadAddressOfNames = NULL;
	PUSHORT pReloadAddressOfNameOrdinals = NULL;
	PULONG_PTR ReloadAddressOfFunctionsRVA = 0;
	ULONG i = 0, iCnt = 0;
	PIMAGE_SECTION_HEADER NtSection = NULL;

	ImageDosHeader = (PIMAGE_DOS_HEADER)pReload;
	if (ImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return;
	}

	ImageNtHeaders = (PIMAGE_NT_HEADERS)((ULONG_PTR)pReload + ImageDosHeader->e_lfanew);
	if (ImageNtHeaders->Signature != IMAGE_NT_SIGNATURE)
	{
		return;
	}


	CHAR szText[] = { '.','t','e','x','t','\0' };
	NtSection = IMAGE_FIRST_SECTION(ImageNtHeaders);
	for (i = 0; i < ImageNtHeaders->FileHeader.NumberOfSections; i++)
	{


		if (dwBase == 0x0007FFAE3320000)
		{
			if ((NtSection[i].Characteristics & IMAGE_SCN_CNT_CODE) &&
				!strcmp((CHAR*)NtSection->Name, szText)) // 是代码段
			{
				ULONG_PTR nOffset = PassIATAddress(pReload, NtSection[i].VirtualAddress, NtSection[i].Misc.VirtualSize);
				if (nOffset == (ULONG_PTR)-1)
				{
					continue;
				}

				ULONG Size = NtSection[i].Misc.VirtualSize - nOffset;


				COMPARE_DATA cd_from_memory = { 0 };
				cd_from_memory.startAddress = NtSection[i].VirtualAddress + (ULONG_PTR)pSection + nOffset;
				cd_from_memory.EndAddress = cd_from_memory.startAddress + Size;
				cd_from_memory.Size = Size;
				cd_from_memory.dwFixBase = (cd_from_memory.startAddress - (ULONG_PTR)pSection) + dwBase;
				cd_from_memory.type = 0;

				COMPARE_DATA cd_from_pe = {0};
				cd_from_pe.startAddress = NtSection[i].VirtualAddress + (ULONG_PTR)pReload + nOffset;
				cd_from_pe.EndAddress = cd_from_pe.startAddress + Size;
				cd_from_pe.Size = Size;
				cd_from_pe.dwFixBase= (cd_from_pe.startAddress - (ULONG_PTR)pReload) + dwBase;
				cd_from_pe.type = 1;

				DisasmCode(&cd_from_memory);
				DisasmCode(&cd_from_pe);
	
				
			}

			int c = 0;
		}
	}
}



VOID CListProcessHook::DisasmCode(PCOMPARE_DATA pData)
{
	size_t j;
	cs_insn* pTemp = NULL;
	csh handle = 0;
	cs_insn* insn = NULL;
	size_t count = 0;


	if (!pData)return;

	cs_open(CS_ARCH_X86, CS_MODE_64, &handle);
	while ((pData->startAddress < pData->EndAddress) && handle)
	{
		count = cs_disasm(handle, (unsigned char*)pData->startAddress, pData->Size, pData->dwFixBase, pData->Size, &insn);

		for (j = 0; j < count; j++)
		{

			pTemp = &insn[j];
			if (pData->type==0) 
			{
				
				SendMessageW(WM_ADD_ITEM_PROCESSHOOK_DLG, 0x20222, (LPARAM)&insn[j]);
			}
			else {
				SendMessageW(WM_ADD_ITEM_PROCESSHOOK_DLG, 0x202222, (LPARAM)&insn[j]);
			}
		}
		if (count < pData->Size)
		{
			if (pTemp)
			{
				pData->startAddress += pTemp->address - pData->dwFixBase + pTemp->size;
				pData->dwFixBase = pTemp->address + pTemp->size;
			}
			if (insn) {
				cs_free(insn, count);
				insn = NULL;
			}
			pData->startAddress += 1;
			pData->dwFixBase += 1;
			pTemp = NULL;

			count = TRUE;
		}

	}
	cs_close(&handle);
}

void CListProcessHook::EnumInlineHook(PVOID pSectionFromMemory, PVOID pReloadFromPe, ULONG_PTR dwBase)
{
	if (pSectionFromMemory == NULL || pReloadFromPe == NULL || dwBase == 0)
	{
		return;
	}

	PIMAGE_EXPORT_DIRECTORY ReloadImageExportDirectory = NULL;
	PIMAGE_DOS_HEADER ImageDosHeader = NULL;
	PIMAGE_NT_HEADERS ImageNtHeaders = NULL;
	ULONG ExportSize = 0;
	PULONG_PTR pReloadAddressOfNames = NULL;
	PUSHORT pReloadAddressOfNameOrdinals = NULL;
	PULONG_PTR ReloadAddressOfFunctionsRVA = 0;
	ULONG i = 0, iCnt = 0;
	PIMAGE_SECTION_HEADER NtSection = NULL;

	ImageDosHeader = (PIMAGE_DOS_HEADER)pReloadFromPe;
	if (ImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return;
	}

	ImageNtHeaders = (PIMAGE_NT_HEADERS)((ULONG_PTR)pReloadFromPe + ImageDosHeader->e_lfanew);
	if (ImageNtHeaders->Signature != IMAGE_NT_SIGNATURE)
	{
		return;
	}


	DISASM dm_org;

	CHAR szText[] = { '.','t','e','x','t','\0' };
	NtSection = IMAGE_FIRST_SECTION(ImageNtHeaders);
	for (i = 0; i < ImageNtHeaders->FileHeader.NumberOfSections; i++)
	{
		if ((NtSection[i].Characteristics & IMAGE_SCN_CNT_CODE) &&
			!strcmp((CHAR*)NtSection[i].Name, szText)) // 是代码段
		{
			ULONG_PTR nOffset = PassIATAddress(pReloadFromPe, NtSection[i].VirtualAddress, NtSection[i].Misc.VirtualSize);
			if (nOffset == (ULONG_PTR)-1)
			{
				break;
			}

			ULONG_PTR ReloadAddress = NtSection[i].VirtualAddress + (ULONG_PTR)pReloadFromPe + nOffset;   //pe
			ULONG_PTR OriginAddress = NtSection[i].VirtualAddress + (ULONG_PTR)pSectionFromMemory + nOffset; //内存
			ULONG Size = NtSection[i].Misc.VirtualSize - nOffset;

			int  len = 0;
			BOOL bSign = FALSE;
			ULONG_PTR dwEntry = 0;

			if (Size > 0 && ReloadAddress > 0 && OriginAddress > 0)
			{
				ULONG_PTR n = 0, nDifCnt = 0, pDifAddress = 0;;


				for (n = 0;
					n < Size && !IsBadReadPtr((PVOID)(ReloadAddress + n), sizeof(PVOID)) && !IsBadReadPtr((PVOID)(OriginAddress + n), sizeof(PVOID));
					n++)
				{
					if (*((PBYTE)ReloadAddress + n) != *((PBYTE)OriginAddress + n) &&
						*(PULONG_PTR)(ReloadAddress + n) != 0 &&
						*(PULONG_PTR)(ReloadAddress + sizeof(ULONG_PTR) + n) != 0)
					{
						if (!nDifCnt)
						{
							pDifAddress = (ULONG_PTR)OriginAddress + n;
						}


						RtlZeroMemory(&dm_org, 0);
						dm_org.Archi = 0;
						dm_org.EIP = OriginAddress + n;
						dm_org.Options = MasmSyntax;
						dm_org.VirtualAddr = OriginAddress + n - (ULONG_PTR)pSectionFromMemory + dwBase;


						len = Disasm(&dm_org);

						if (len < 0) {
							nDifCnt += len;
						}
						if (len == 1)
						{
							nDifCnt += len;
						}
						else if (len > 1)
						{
							n--;
							n += len;
							nDifCnt += len;
						}
						if (!bSign)
						{

							if (IsHookCode(CString(dm_org.CompleteInstr))) {
								bSign = TRUE;
								dwEntry = (ULONG_PTR)dm_org.VirtualAddr;
							}
						}

						dm_org.EIP += len;
						dm_org.VirtualAddr += len;

					}
					else
					{
						if (nDifCnt > 2 && bSign)
						{
							INLINE_HOOK_INFO info;
							info.dwHookOffset = pDifAddress - (ULONG_PTR)pSectionFromMemory;
							info.dwLen = nDifCnt;
							info.dwBase = dwBase;
							info.dwReload = (ULONG_PTR)pReloadFromPe;

							m_InlineHookInfo.push_back(info);

						}

						nDifCnt = 0;
						bSign = FALSE;
						dwEntry = 0;
					}
				}

			}

			break;
		}

		++NtSection;
	}
}

void CListProcessHook::EnumInlineHook32(PVOID pSectionFromMemory, PVOID pReloadFromPe, ULONG_PTR dwBase)
{
	if (pSectionFromMemory == NULL || pReloadFromPe == NULL || dwBase == 0)
	{
		return;
	}

	PIMAGE_EXPORT_DIRECTORY ReloadImageExportDirectory = NULL;
	PIMAGE_DOS_HEADER ImageDosHeader = NULL;
	PIMAGE_NT_HEADERS32 ImageNtHeaders = NULL;
	ULONG ExportSize = 0;
	PULONG_PTR pReloadAddressOfNames = NULL;
	PUSHORT pReloadAddressOfNameOrdinals = NULL;
	PULONG_PTR ReloadAddressOfFunctionsRVA = 0;
	ULONG i = 0, iCnt = 0;
	PIMAGE_SECTION_HEADER NtSection = NULL;

	ImageDosHeader = (PIMAGE_DOS_HEADER)pReloadFromPe;
	if (ImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return;
	}

	ImageNtHeaders = (PIMAGE_NT_HEADERS32)((ULONG_PTR)pReloadFromPe + ImageDosHeader->e_lfanew);
	if (ImageNtHeaders->Signature != IMAGE_NT_SIGNATURE)
	{
		return;
	}


	DISASM dm_org;

	CHAR szText[] = { '.','t','e','x','t','\0' };
	NtSection = IMAGE_FIRST_SECTION(ImageNtHeaders);
	for (i = 0; i < ImageNtHeaders->FileHeader.NumberOfSections; i++)
	{
		if ((NtSection[i].Characteristics & IMAGE_SCN_CNT_CODE) &&
			!strcmp((CHAR*)NtSection[i].Name, szText)) // 是代码段
		{
			ULONG_PTR nOffset = PassIATAddress(pReloadFromPe, NtSection[i].VirtualAddress, NtSection[i].Misc.VirtualSize);
			if (nOffset == (ULONG_PTR)-1)
			{
				break;
			}

			ULONG_PTR ReloadAddress = NtSection[i].VirtualAddress + (ULONG_PTR)pReloadFromPe + nOffset;   //pe
			ULONG_PTR OriginAddress = NtSection[i].VirtualAddress + (ULONG_PTR)pSectionFromMemory + nOffset; //内存
			ULONG Size = NtSection[i].Misc.VirtualSize - nOffset;

			int  len = 0;
			BOOL bSign = FALSE;


			if (Size > 0 && ReloadAddress > 0 && OriginAddress > 0)
			{
				ULONG_PTR n = 0, nDifCnt = 0, pDifAddress = 0;;


				for (n = 0;
					n < Size && !IsBadReadPtr((PVOID)(ReloadAddress + n), sizeof(PVOID)) && !IsBadReadPtr((PVOID)(OriginAddress + n), sizeof(PVOID));
					n++)
				{
					if (*((PBYTE)ReloadAddress + n) != *((PBYTE)OriginAddress + n) &&
						*(PULONG_PTR)(ReloadAddress + n) != 0 &&
						*(PULONG_PTR)(ReloadAddress + sizeof(ULONG_PTR) + n) != 0)
					{
						if (!nDifCnt)
						{
							pDifAddress = (ULONG_PTR)OriginAddress + n;
						}


						RtlZeroMemory(&dm_org, 0);
						dm_org.Archi = 1;
						dm_org.EIP = OriginAddress + n;
						dm_org.Options = MasmSyntax;
						dm_org.VirtualAddr = OriginAddress + n - (ULONG_PTR)pSectionFromMemory + dwBase;


						len = Disasm(&dm_org);

						if (len < 0) {
							nDifCnt += len;
						}
						if (len == 1)
						{
							nDifCnt += len;
						}
						else if (len > 1)
						{
							n--;
							n += len;
							nDifCnt += len;
						}
						if (!bSign)
						{

							if (IsHookCode(CString(dm_org.CompleteInstr))) {
								bSign = TRUE;

							}
						}

						dm_org.EIP += len;
						dm_org.VirtualAddr += len;

					}
					else
					{
						if (nDifCnt > 2 && bSign)
						{
							INLINE_HOOK_INFO info;
							info.dwHookOffset = pDifAddress - (ULONG_PTR)pSectionFromMemory;
							info.dwLen = nDifCnt;
							info.dwBase = dwBase;
							info.dwReload = (ULONG_PTR)pReloadFromPe;

							m_InlineHookInfo.push_back(info);

						}

						nDifCnt = 0;
						bSign = FALSE;

					}
				}

			}

			break;
		}

		++NtSection;
	}
}

BOOLEAN CListProcessHook::IsHookCode(CString szTemp)
{
	if (!szTemp.GetLength())return FALSE;
	BOOLEAN bOk = FALSE;
	if (szTemp.Find('j') != -1) // 跳转
	{
		bOk = TRUE;
	}
	else if (szTemp.Find(L"mov") != -1) // mov 
	{
		bOk = TRUE;
	}
	else if (szTemp.Find(L"call") != -1)
	{
		bOk = TRUE;
	}
	else if (szTemp.Find(L"push") != -1)
	{
		bOk = TRUE;
	}
	else if (szTemp.Find(L"push") != -1)
	{
		bOk = TRUE;
	}
	else if (szTemp.Find(L"nop") != -1)
	{
		bOk = TRUE;
	}

	return bOk;
}

// 也不只是IAT了, 凡是section表中在text段中的都排除, eg. idata段等.
ULONG_PTR CListProcessHook::PassIATAddress(PVOID pSection, ULONG_PTR pVA, ULONG nSize)
{
	if (pSection == NULL || !pVA || !nSize)
	{
		return 0;
	}

	ULONG_PTR dwTemp1 = 0, dwTemp2 = 0;
	ULONG_PTR dwRet = 0;
	ULONG ImportSize = 0;
	PVOID pRet = NULL;
	pRet = RtlImageDirectoryEntryToData(pSection, TRUE, IMAGE_DIRECTORY_ENTRY_IAT, &ImportSize);
	if (pRet && ImportSize)
	{
		if ((ULONG_PTR)pRet == (ULONG_PTR)pSection + pVA &&
			(ULONG_PTR)pRet + ImportSize <= (ULONG_PTR)pSection + pVA + nSize)
		{
			dwRet += ImportSize;
		}
	}

	pRet = RtlImageDirectoryEntryToData(pSection, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &ImportSize);
	if (pRet && ImportSize)
	{
		if ((ULONG_PTR)pRet == (ULONG_PTR)pSection + pVA + dwRet &&
			(ULONG_PTR)pRet <= (ULONG_PTR)pSection + pVA + nSize)
		{
			dwRet += ImportSize;
		}
	}

	return dwRet;
}


PVOID CListProcessHook::GetOriginBuffer(ULONG_PTR dwBase)
{
	PVOID pBuffer = NULL;
	if (dwBase)
	{
		for (list<DUMP_INFO>::iterator ir = m_DumpList.begin();
			ir != m_DumpList.end();
			ir++)
		{
			if (ir->nBase == dwBase)
			{
				pBuffer = ir->pSectionBuffer;
				break;
			}
		}
	}

	return pBuffer;
}


CString CListProcessHook::GetModuleImageName(ULONG_PTR dwBase)
{
	CString szImage;

	if (dwBase)
	{
		for (list<DUMP_INFO>::iterator ir = m_DumpList.begin();
			ir != m_DumpList.end();
			ir++)
		{
			if (ir->nBase == dwBase)
			{
				CString szPath = ir->szPath;
				szImage = szPath.Right(szPath.GetLength() - szPath.ReverseFind('\\') - 1);
				szImage.MakeLower();
				break;
			}
		}
	}

	return szImage;
}

CString CListProcessHook::GetFunctionName(PVOID pSection, DWORD dwOffset)
{
	if (!pSection || !dwOffset)
	{
		return NULL;
	}

	CString szName;
	PULONG NameTableBase;
	PUSHORT NameOrdinalTableBase;
	PULONG AddressTableBase;
	LONG High;
	LONG Low;
	ULONG ExportSize;
	PIMAGE_EXPORT_DIRECTORY ExportDirectory = (PIMAGE_EXPORT_DIRECTORY)RtlImageDirectoryEntryToData((PVOID)pSection, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &ExportSize);
	if (ExportDirectory == NULL)
	{
		return NULL;
	}

	NameTableBase = (PULONG)((ULONG_PTR)pSection + (ULONG)ExportDirectory->AddressOfNames);
	NameOrdinalTableBase = (PUSHORT)((ULONG_PTR)pSection + (ULONG)ExportDirectory->AddressOfNameOrdinals);
	AddressTableBase = (PULONG)((ULONG_PTR)pSection + (ULONG)ExportDirectory->AddressOfFunctions);

	if (IsBadReadPtr((PVOID)NameTableBase, sizeof(ULONG)) ||
		IsBadReadPtr((PVOID)NameOrdinalTableBase, sizeof(USHORT)) ||
		IsBadReadPtr((PVOID)AddressTableBase, sizeof(ULONG)))
	{
		return NULL;
	}

	High = ExportDirectory->NumberOfNames - 1;

	for (Low = 0; Low < High; Low++)
	{
		if (IsBadReadPtr((PVOID)(NameOrdinalTableBase + Low), sizeof(USHORT)))
		{
			return NULL;
		}

		USHORT OrdinalNumber = NameOrdinalTableBase[Low];
		if ((ULONG)OrdinalNumber <= ExportDirectory->NumberOfFunctions)
		{
			if (IsBadReadPtr((PVOID)(AddressTableBase + OrdinalNumber), sizeof(ULONG)))
			{
				return NULL;
			}

			DWORD nOfffset = AddressTableBase[OrdinalNumber];
			if (nOfffset == dwOffset)
			{
				szName = m_Functions.ms2ws((PCHAR)((ULONG_PTR)pSection + NameTableBase[Low]));
				break;
			}
		}
	}

	return szName;
}

ULONG_PTR CListProcessHook::GetInlineHookAddress(PVOID pBuffer, ULONG_PTR dwBase, ULONG dwHookOffset, ULONG nCodeLen)
{
	ULONG_PTR nRet = 0;
	DISASM Instruction;
	ULONG len = 0;

	memset(&Instruction, 0, sizeof(DISASM));
	Instruction.VirtualAddr = dwBase + dwHookOffset;
	Instruction.EIP = (UIntPtr)((PBYTE)pBuffer + (ULONG)dwHookOffset);
	Instruction.Archi = !m_bIs64;
	CString szAddress;
	for (ULONG i = 0; i < nCodeLen; i += len)
	{
		len = Disasm(&Instruction);
		if (len == 0)
		{
			break;
		}

		CHAR *aasm = Instruction.CompleteInstr;
		CString szAsm = m_Functions.ms2ws(aasm);
		BOOL bOk = FALSE;

		if (szAsm.Find('j') != -1) // 跳转
		{
			bOk = TRUE;
		}
		else if (szAsm.Find(L"mov") != -1) // mov 
		{
			bOk = TRUE;
		}
		else if (szAsm.Find(L"call") != -1)
		{
			bOk = TRUE;
		}
		else if (szAsm.Find(L"push") != -1)
		{
			bOk = TRUE;
		}

		if (bOk && szAsm.Find(' ') != -1)
		{
			int n = szAsm.Find(' ');
			szAddress = szAsm.Right(szAsm.GetLength() - n - 1);
		}
	}

	if (!szAddress.IsEmpty())
	{
		swscanf_s(szAddress, L"%x", &nRet);
	}
	else
	{
		if (nCodeLen < 5)
		{
			nRet = GetInlineHookAddress(pBuffer, dwBase, dwHookOffset - (5 - nCodeLen), 5);
		}
	}

	//if (nRet)
	//{
	//	if (!IsExecuteSection(nRet))
	//	{
	//		nRet = 0;
	//	}
	//}

	return nRet;
}


CString CListProcessHook::GetDllModulePath(ULONG_PTR dwRet)
{
	CString strPath;

	if (!dwRet)
	{
		return NULL;
	}

	for (vector <MODULE_INFO_EX>::iterator itor = m_vectorModules.begin();
		itor != m_vectorModules.end();
		itor++)
	{
		if (dwRet >= itor->Base && dwRet <= itor->Base + itor->Size)
		{
			MODULE_INFO_EX info = *itor;
			strPath = info.Path;
			if (strPath.IsEmpty())
			{
				strPath = m_Functions.TrimPath(itor->Path);
			}

			break;
		}
	}

	return strPath;
}


void CListProcessHook::ShowScanResult()
{

	if (m_listHookInfo.m_hWnd)
	{


		for (list<IAT_HOOK_INFO>::iterator itor = m_IatHookList.begin();
			itor != m_IatHookList.end();
			itor++)
		{

			CString szFunction = itor->szHookedModule;
			szFunction += L"->";
			szFunction += itor->szExpModule;
			szFunction += L" [";
			szFunction += itor->szFunction;
			szFunction += L"]";


			CString szOrgAddress;
			szOrgAddress.Format(L"0x%llX --> 0x%llX", itor->dwOriginAddress, itor->dwHookAddress);



			int n = m_listHookInfo.InsertItem(m_listHookInfo.GetItemCount(), szFunction);
			m_listHookInfo.SetItemText(n, 1, L"Iat");
			m_listHookInfo.SetItemText(n, 2, szOrgAddress);
			m_listHookInfo.SetItemData(n, n);
			m_InlineHookCnt++;
		}





		// inline hook
		for (list<INLINE_HOOK_INFO>::iterator itor = m_InlineHookInfo.begin();
			itor != m_InlineHookInfo.end();
			itor++)
		{
			PVOID pBuffer = GetOriginBuffer(itor->dwBase);
			CString szModule = GetModuleImageName(itor->dwBase);
			CString szFunc = GetFunctionName(pBuffer, itor->dwHookOffset);
			//again
			if (szFunc.IsEmpty())
			{
				szFunc = GetFunctionName(pBuffer, itor->dwHookOffset &(~0xf));
				if (!szFunc.IsEmpty())
				{
					itor->dwHookOffset = itor->dwHookOffset &(~0xf);
				}
			}
			if (!szFunc.IsEmpty())
			{
				szModule += L": ";
				szModule += szFunc;
			}
			else
			{
				CString szTemp;
				szTemp.Format(L"%s+0x%X", szModule, itor->dwHookOffset);
				szModule = szTemp;
			}


			ULONG_PTR dwCurrent = 0;//GetInlineHookAddress(pBuffer, itor->dwBase, itor->dwHookOffset, itor->dwLen);
			CString szCurrent;
			szCurrent.Format(L"0x%llX -> ", itor->dwBase + (ULONG)itor->dwHookOffset);

			CString szHookModule;

			//szHookModule = GetPreDisCode(itor->dwReload + itor->dwHookOffset, itor->dwLen);

			int n = m_listHookInfo.InsertItem(m_listHookInfo.GetItemCount(), szModule);
			m_listHookInfo.SetItemText(n, 1, L"inline");
			m_listHookInfo.SetItemText(n, 2, szCurrent);
			m_listHookInfo.SetItemData(n, n);
			m_InlineHookCnt++;

		}
	
	}
	
}



LRESULT CListProcessHook::OnAddItemBySendMessage(WPARAM wParam, LPARAM lParam)
{

	if (wParam == 0x2021 && lParam == 0x2021) {
		::SetWindowText(AfxGetMainWnd()->GetDlgItem(IDC_SHOW_TEXT)->m_hWnd, m_szStatus);
	}

	if (wParam == 0x2022 && lParam == 0x2022) {

		ShowScanResult();
	}



	return LRESULT();
}


VOID CListProcessHook::clearWork()
{
	for (list<DUMP_INFO>::iterator itor = m_DumpList.begin();
		itor != m_DumpList.end();
		itor++)
	{
		if (itor->pSectionBuffer)
		{
			free(itor->pSectionBuffer);
			itor->pSectionBuffer = NULL;
		}
		if (itor->pReloadBuffer)
		{
			free(itor->pReloadBuffer);
			itor->pReloadBuffer = NULL;
		}

	}
	m_BoundList.clear();
	m_IatHookList.clear();
	m_InlineHookInfo.clear();
	m_DumpList.clear();
	m_vectorModules.clear();
	m_listHookInfo.DeleteAllItems();
	SetWindowText(L" ");

}


void CListProcessHook::OnExportExcel()
{
	m_Functions.ExportListToTxt(&m_listHookInfo, L"");
}


void CListProcessHook::OnRclickProcesshook2List(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CMenu menu;
	menu.CreatePopupMenu();
	//menu.AppendMenu(MF_STRING, ID_RESFRESH, L"恢复");
	//menu.AppendMenu(MF_SEPARATOR);


	menu.AppendMenu(MF_STRING, ID_EXPORT_EXCEL, L"导出");
	menu.AppendMenu(MF_SEPARATOR);

	CPoint pt;
	GetCursorPos(&pt);
	menu.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	menu.DestroyMenu();
	*pResult = 0;
}



BOOL CListProcessHook::ListProcessModules(DWORD dwPID)
{
	m_vectorModules.clear();
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;
	MODULE_INFO_EX info = { 0 };
	//  Take a snapshot of all modules in the specified process. 
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwPID);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		return(FALSE);
	}

	//  Set the size of the structure before using it. 
	me32.dwSize = sizeof(MODULEENTRY32);

	//  Retrieve information about the first module, 
	//  and exit if unsuccessful 
	if (!Module32First(hModuleSnap, &me32))
	{
		CloseHandle(hModuleSnap);     // Must clean up the snapshot object! 
		return(FALSE);
	}
	do
	{
		info.Base = (ULONG_PTR)me32.hModule;
		info.Size = me32.modBaseSize;
		RtlCopyMemory(info.Path, me32.szExePath, MAX_PATH * sizeof(WCHAR));
		m_vectorModules.push_back(info);
	} while (Module32Next(hModuleSnap, &me32));



	//  Do not forget to clean up the snapshot object. 
	CloseHandle(hModuleSnap);
	return(TRUE);
}
