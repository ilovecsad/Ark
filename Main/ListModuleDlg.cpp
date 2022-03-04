// ListModuleDlg.cpp: 实现文件
//

#include "pch.h"
#include "Main.h"
#include "ListModuleDlg.h"
#include "afxdialogex.h"


// CListModuleDlg 对话框
#define SYSTEM_ADDRESS_START32 0x7fffffff
IMPLEMENT_DYNAMIC(CListModuleDlg, CDialogEx)

CListModuleDlg::CListModuleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MODULE_DIALOG, pParent)
{

}

CListModuleDlg::~CListModuleDlg()
{
}

void CListModuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MODULE_LIST, m_listModule);
}


BEGIN_MESSAGE_MAP(CListModuleDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_MODULE_LIST, &CListModuleDlg::OnCustomdrawVadList)
	ON_NOTIFY(NM_RCLICK, IDC_MODULE_LIST, &CListModuleDlg::OnRclickModuleList)
	ON_COMMAND(ID_MEMORY_1, &CListModuleDlg::OnModuleReFresh)
	ON_MESSAGE(WM_ADD_ITEM_MODULE_DLG, &CListModuleDlg::OnAddItem)

	ON_COMMAND(ID_EXPORT_TEXT, &CListModuleDlg::OnExportText)
	ON_COMMAND(ID_COPY_PROCESS_PATH, &CListModuleDlg::OnCopyProcessPath)
	ON_COMMAND(ID_FREE_DLL, &CListModuleDlg::OnFreeDll)
	ON_COMMAND(ID_COPY_MODULE, &CListModuleDlg::OnCopyModule)
	ON_COMMAND(ID_HIDE_MODULE, &CListModuleDlg::OnHideModule)
END_MESSAGE_MAP()


// CListModuleDlg 消息处理程序


BOOL CListModuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	
	m_listModule.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listModule.InsertColumn(eDllPath, L"Path", LVCFMT_LEFT, 340);
	m_listModule.InsertColumn(eDllBase, L"Base", LVCFMT_LEFT, 140);
	m_listModule.InsertColumn(eDllSize, L"Size", LVCFMT_LEFT, 90);
	m_listModule.InsertColumn(eDllFileCompany, L"File Corporation", LVCFMT_LEFT, 150);
	m_listModule.InsertColumn(eDllFileDescription, L"File Description", LVCFMT_LEFT, 180);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CListModuleDlg::EnumModules()
{
	// TODO: 在此处添加实现代码.
	m_listModule.DeleteAllItems();
	m_vectorModules.clear();
	m_vectorVadInfo.clear();
	m_vectorByPeb.clear();
	m_vectorByPebList.clear();
	m_nModuleCnt = 0;
	//开始枚举
	m_clsVadInfo.EnumProcessVad((ULONG)m_dwPid, m_vectorVadInfo);
	if (m_vectorVadInfo.empty())
	{
		return;
	}

	m_clsModules.EnumModulesByPeb((ULONG)m_dwPid, m_pEprocess, m_vectorByPeb);
	for (vector <MODULE_INFO_EX>::iterator Iter = m_vectorByPeb.begin(); Iter != m_vectorByPeb.end(); Iter++)
	{
		m_vectorByPebList.push_back(Iter->Base);
	}



	for (vector <VAD_INFO>::iterator Iter = m_vectorVadInfo.begin(); Iter != m_vectorVadInfo.end(); Iter++)
	{
		if (Iter->pFileObject) {
			AddDllModuleItem(*Iter);
		}
	}
	m_VadInfo.Format(L"模块总数:%d PEB:%d", m_nModuleCnt,m_nModuleCnt- m_nPEBModules);
	(GetParent()->GetParent())->SetDlgItemText(IDC_PROCESS_TEXT, m_VadInfo);
}
void CListModuleDlg::AddDllModuleItem(VAD_INFO item)
{
	CString strBase, strSize, strCompany, strPath, szDescription;
	DLL_INFO dllInfo = { 0 };
	int nSize = 0;
	strPath = m_clsVadInfo.ObQueryNameString(item.pFileObject);

	nSize = (((item.endVpn - item.startVpn) / 0x1000) + 1) * 0x1000;
	strSize.Format(L"0x%08x", nSize);
	strBase.Format(L"0x%016llx", item.startVpn);

	strCompany = m_Functions.GetFileCompanyName(strPath);//输入路径 转换成公司名 如: Microsoft Corporation

	szDescription = m_Functions.GetFileDescription(strPath);//如 Windows 会话管理器

	int n = m_listModule.InsertItem(m_listModule.GetItemCount(), strPath);
	m_listModule.SetItemText(n, eDllBase, strBase);
	m_listModule.SetItemText(n, eDllSize, strSize);
	m_listModule.SetItemText(n, eDllFileCompany, strCompany);
	m_listModule.SetItemText(n, eDllFileDescription, szDescription);
	m_listModule.SetItemData(n, n);

	if (!m_Functions.IsMicrosoftApp(strCompany))
	{
		dllInfo.clrItem = enumBlue;
		m_nNotMicro++;
	}
	if (IsModuleFromPEB(item.startVpn))
	{
		dllInfo.clrItem = enumTuhuang;
		m_nPEBModules++;
	}

	if (item.startVpn > SYSTEM_ADDRESS_START32) {
		if (strPath.Find(L"ntdll.dll") != -1 && !m_ntdllBase) {
			m_ntdllBase = item.startVpn;
		}
	}
	else {
		if (strPath.Find(L"ntdll.dll") != -1 && !m_ntdllBase32) {
			m_ntdllBase32 = (ULONG32)item.startVpn;
		}


		if (!m_kernel32Base)
		{
			strPath.MakeLower();
			if (strPath.Find(L"kernel32.dll") != -1) {
				m_kernel32Base = (ULONG32)item.startVpn;
			}
		}
	}


	dllInfo.Base = item.startVpn;
	dllInfo.DllPath = strPath;
	dllInfo.Size = nSize;
	m_vectorModules.push_back(dllInfo);
	m_nModuleCnt++;
}


void CListModuleDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow && nStatus == 0) {

		EnumModules();
	}


	if (!bShow) 
	{


	}
}

LRESULT CListModuleDlg::OnAddItem(WPARAM wParam, LPARAM lParam)
{

	

	//if (this->IsWindowVisible()) {
	//	m_szInfo.Format(L"内存总数:%d", (int)lParam);
	//	(GetParent()->GetParent())->SetDlgItemText(IDC_PROCESS_TEXT, m_szInfo);
	//}

	return TRUE;
}





void CListModuleDlg::OnCustomdrawVadList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

	*pResult = CDRF_DODEFAULT;

	if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if ((CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage)
	{
		int nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);
		COLORREF clrNewTextColor, clrNewBkColor;

		clrNewTextColor = RGB(0, 0, 0);
		clrNewBkColor = RGB(255, 255, 255);

		if (nItem != -1)
		{
			int nData = (int)m_listModule.GetItemData(nItem);
			ITEM_COLOR clrText = m_vectorModules.at(nData).clrItem;

			//	DebugLog(L"nItem: %d, nData: %d", nItem, nData);

			if (clrText == enumBlue)
			{
				clrNewTextColor = RGB(0, 0, 255);			// 非微软的文件默认显示蓝色
			}
			else if (clrText == enumPurple)
			{
				clrNewTextColor = RGB(180, 0, 255);		// 未签名项默认显示紫色
			}
			else if (clrText == enumTuhuang)
			{
				clrNewTextColor = RGB(255, 140, 0);		// 未签名项默认显示紫色
			}
		}

		pLVCD->clrText = clrNewTextColor;
		pLVCD->clrTextBk = clrNewBkColor;

		*pResult = CDRF_DODEFAULT;
	}
}


// ring0的pid跟ring3的对比，得出是否是隐藏进程
BOOL CListModuleDlg::IsModuleFromPEB(ULONG_PTR Base)
{
	BOOL bHide = FALSE;

	if (!m_vectorByPebList.empty())
	{
		list<ULONG_PTR>::iterator findItr;
		findItr = find(m_vectorByPebList.begin(), m_vectorByPebList.end(), Base);
		if (findItr == m_vectorByPebList.end())
		{
			bHide = TRUE;
		}
	}

	return bHide;
}

void CListModuleDlg::OnRclickModuleList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_MEMORY_1, L"刷新");
	menu.AppendMenu(MF_SEPARATOR);

	menu.AppendMenu(MF_STRING, ID_FREE_DLL, L"卸载模块");
	menu.AppendMenu(MF_SEPARATOR);


	menu.AppendMenu(MF_STRING, ID_COPY_MODULE, L"拷贝模块");
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_COPY_PROCESS_PATH, L"复制模块路径");
	menu.AppendMenu(MF_SEPARATOR);

	menu.AppendMenu(MF_STRING, ID_HIDE_MODULE, L"隐藏模块");
	menu.AppendMenu(MF_SEPARATOR);

	menu.AppendMenu(MF_STRING, ID_EXPORT_TEXT, L"导出");
	menu.AppendMenu(MF_SEPARATOR);

	CPoint pt;
	GetCursorPos(&pt);
	menu.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	menu.DestroyMenu();



	*pResult = 0;
}


void CListModuleDlg::OnModuleReFresh()
{
	EnumModules();
}


void CListModuleDlg::OnExportText()
{
	m_Functions.ExportListToTxt(&m_listModule, L"");
}


void CListModuleDlg::OnCopyProcessPath()
{
	CString szPath;

	POSITION pos = m_listModule.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_listModule.GetNextSelectedItem(pos);
		szPath = m_listModule.GetItemText(nItem, eDllPath);
	}

	m_Functions.SetStringToClipboard(szPath);
}


void CListModuleDlg::OnFreeDll()
{
	if (m_listModule.m_hWnd)
	{
		int nItem = m_Functions.GetSelectItem(&m_listModule);
		if (-1 == nItem)
		{
			return;
		}

		HANDLE dwPid = 0;
		PDLL_INFO pInfo = GetDllInfoByItem(nItem);
		if (!pInfo)return;

		dwPid = m_dwPid;
		ULONG_PTR dwLdrpHashTable = 0;

		if (pInfo->Base > SYSTEM_ADDRESS_START32)
		{
			ULONG_PTR dwLdrpHashTableOffset = 0;
			PVOID pKernel32_BaseThreadInitThunk = NULL;
			HMODULE hKernle32 = (HMODULE)GetModuleHandle(L"kernel32");
			pKernel32_BaseThreadInitThunk = GetProcAddress(hKernle32, "BaseThreadInitThunk");
			if (pKernel32_BaseThreadInitThunk) {
				dwLdrpHashTableOffset = (ULONG_PTR)GetLdrpHashTableAddress(pKernel32_BaseThreadInitThunk);

			}

			dwLdrpHashTable = m_ntdllBase + dwLdrpHashTableOffset;

		}
		else {
			DLL_BASE dllBase = { 0 };
			dllBase.kernel32_base = m_kernel32Base;
			dllBase.ntdll_base = m_ntdllBase32;

			RtlCopyMemory(&dwLdrpHashTable, &dllBase, sizeof(ULONG_PTR));
		}

		if (m_clsModules.FreeLoadLibary((ULONG)dwPid, pInfo->Base, dwLdrpHashTable))
		{
			
			if (m_listModule.DeleteItem(nItem))
			{
				AfxMessageBox(L"卸载成功\n");
			}

		}


	}
}


PDLL_INFO CListModuleDlg::GetDllInfoByItem(int nItem)
{
	PDLL_INFO pInfo = NULL;

	if (nItem == -1)
	{
		return pInfo;
	}

	int nData = (int)m_listModule.GetItemData(nItem);
	pInfo = &m_vectorModules.at(nData);

	return pInfo;
}


void CListModuleDlg::OnCopyModule()
{
	if (m_listModule.m_hWnd)
	{
		int nItem = m_Functions.GetSelectItem(&m_listModule);
		if (-1 == nItem)
		{
			return;
		}

		HANDLE dwPid = 0;
		PDLL_INFO pInfo = GetDllInfoByItem(nItem);
		if (!pInfo)return;

		dwPid = m_dwPid;

		if (m_clsModules.DumpModuleMemoryToFile((ULONG)dwPid, pInfo->Base,pInfo->Size,pInfo->DllPath))
		{
			AfxMessageBox(L"拷贝成功\n");

		}
	}
}


void CListModuleDlg::OnHideModule()
{
	if (m_listModule.m_hWnd)
	{
		int nItem = m_Functions.GetSelectItem(&m_listModule);
		if (-1 == nItem)
		{
			return;
		}

		HANDLE dwPid = 0;
		PDLL_INFO pInfo = GetDllInfoByItem(nItem);
		if (!pInfo)return;

		dwPid = m_dwPid;

		
		ULONG_PTR dwLdrpHashTable = 0;
	
		//默认64位 程序 断 dwLdrpHashTable
		if (pInfo->Base > SYSTEM_ADDRESS_START32)
		{
			ULONG_PTR dwLdrpHashTableOffset = 0;
			PVOID pKernel32_BaseThreadInitThunk = NULL;
			HMODULE hKernle32 = (HMODULE)GetModuleHandle(L"kernel32");
			pKernel32_BaseThreadInitThunk = GetProcAddress(hKernle32, "BaseThreadInitThunk");
			if (pKernel32_BaseThreadInitThunk) {
				dwLdrpHashTableOffset = (ULONG_PTR)GetLdrpHashTableAddress(pKernel32_BaseThreadInitThunk);

			}

			dwLdrpHashTable = m_ntdllBase + dwLdrpHashTableOffset;

		}
		else {
			DLL_BASE dllBase = {0};
			dllBase.kernel32_base = m_kernel32Base;
			dllBase.ntdll_base = m_ntdllBase32;

			RtlCopyMemory(&dwLdrpHashTable, &dllBase, sizeof(ULONG_PTR));
		}
		if (m_clsModules.HideModule((ULONG)dwPid, pInfo->Base, dwLdrpHashTable,pInfo->Size))
		{
			AfxMessageBox(L"隐藏成功\n");

		}
	}
}


PVOID CListModuleDlg::GetLdrpHashTableAddress(PVOID pKernel32_BaseThreadInitThunk)
{
	DWORD i = 0;
	HANDLE hModule = 0;
	PVOID LdrpHashTable = 0;
	PIMAGE_DOS_HEADER DosHeader = NULL;
	PIMAGE_NT_HEADERS NtHeader = NULL;
	PIMAGE_SECTION_HEADER SectionHeader = NULL;
	PVOID* dwSearchStart = 0;
	PVOID* dwSearchEnd = 0;

	//hashtable是保存在ntdll这个模块的.data的一个地址的偏移
	hModule = GetModuleHandle(L"ntdll.dll");

	//得到ntdll的pe文件头结构
	DosHeader = (PIMAGE_DOS_HEADER)hModule;
	NtHeader = (PIMAGE_NT_HEADERS)((PUCHAR)hModule + DosHeader->e_lfanew);
	SectionHeader = IMAGE_FIRST_SECTION(NtHeader);

	//
	// Retrieve .data section
	// 
	//开始搜索section
	for (i = 0; i < NtHeader->FileHeader.NumberOfSections; i++)
	{
		//printf("Section : %s\n", SectionHeader[i].Name);
		if (*(PDWORD)&SectionHeader[i].Name == 'tad.') //.dat
		{
			dwSearchStart = (PVOID*)((ULONG_PTR)hModule + SectionHeader[i].VirtualAddress);
			dwSearchEnd = (PVOID*)((ULONG_PTR)dwSearchStart + SectionHeader[i].Misc.VirtualSize);
			break;
		}
	}

	PVOID* pSearchCurrent = NULL;

	__try {

		for (pSearchCurrent = dwSearchStart; pSearchCurrent < dwSearchEnd; pSearchCurrent++)
		{
			if (*pSearchCurrent == pKernel32_BaseThreadInitThunk)
			{
				PVOID* pSearchCurrent2 = NULL;
				for (pSearchCurrent2 = pSearchCurrent; pSearchCurrent2 < dwSearchEnd; pSearchCurrent2++) {

					//据观测 LdrpHashTable 连续保存32 LIST_ENTRY  如果得不到你就继续增加判断
					if (*pSearchCurrent2 != 0 && *(pSearchCurrent2 + 1) != 0 && *(pSearchCurrent2 + 2) != 0 &&
						*(pSearchCurrent2 + 3) != 0 && *(pSearchCurrent2 + 4) != 0 && *(pSearchCurrent2 + 5) != 0 && *(pSearchCurrent2 + 6) != 0 &&
						*(pSearchCurrent2 + 7) != 0 && *(pSearchCurrent2 + 8) != 0 && *(pSearchCurrent2 + 9) != 0 && *(pSearchCurrent2 + 10) != 0 &&
						*(pSearchCurrent2 + 11) != 0 && *(pSearchCurrent2 + 12) != 0 && *(pSearchCurrent2 + 13) != 0 && *(pSearchCurrent2 + 14) != 0 &&
						*(pSearchCurrent2 + 15) != 0 && *(pSearchCurrent2 + 16) != 0 && *(pSearchCurrent2 + 17) != 0 && *(pSearchCurrent2 + 18) != 0 &&
						*(pSearchCurrent2 + 19) != 0 && *(pSearchCurrent2 + 20) != 0 && *(pSearchCurrent2 + 21) != 0 && *(pSearchCurrent2 + 22) != 0)
					{
						LdrpHashTable = (PVOID)((ULONG_PTR)pSearchCurrent2 -(ULONG_PTR)hModule);
						break;
					}
				}
				break;
			}
		}
	}
	__except (1) {
		LdrpHashTable = NULL;
	}


	return LdrpHashTable;
}
