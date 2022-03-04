// ListMemoryDlg.cpp: 实现文件
//

#include "pch.h"
#include "Main.h"
#include "ListMemoryDlg.h"
#include "afxdialogex.h"
#include "ChangeProtectionDlg.h"
#include "CAllocateMemoryDlg.h"
typedef enum _TOKEN_
{
	ENUM_SE_CREATE_TOKEN_NAME,
	ENUM_SE_ASSIGNPRIMARYTOKEN_NAME,
	ENUM_SE_LOCK_MEMORY_NAME,
	ENUM_SE_INCREASE_QUOTA_NAME,
	ENUM_SE_UNSOLICITED_INPUT_NAME,
	ENUM_SE_MACHINE_ACCOUNT_NAME,
	ENUM_SE_TCB_NAME,
	ENUM_SE_SECURITY_NAME,
	ENUM_SE_TAKE_OWNERSHIP_NAME,
	ENUM_SE_LOAD_DRIVER_NAME,
	ENUM_SE_SYSTEM_PROFILE_NAME,
	ENUM_SE_SYSTEMTIME_NAME,
	ENUM_SE_PROF_SINGLE_PROCESS_NAME,
	ENUM_SE_INC_BASE_PRIORITY_NAME,
	ENUM_SE_CREATE_PAGEFILE_NAME,
	ENUM_SE_CREATE_PERMANENT_NAME,
	ENUM_SE_BACKUP_NAME,
	ENUM_SE_RESTORE_NAME,
	ENUM_SE_SHUTDOWN_NAME,
	ENUM_SE_DEBUG_NAME,
	ENUM_SE_AUDIT_NAME,
	ENUM_SE_SYSTEM_ENVIRONMENT_NAME,
	ENUM_SE_CHANGE_NOTIFY_NAME,
	ENUM_SE_REMOTE_SHUTDOWN_NAME,
	ENUM_SE_UNDOCK_NAME,
	ENUM_SE_SYNC_AGENT_NAME,
	ENUM_SE_ENABLE_DELEGATION_NAME,
	ENUM_SE_MANAGE_VOLUME_NAME,
	ENUM_SE_IMPERSONATE_NAME,
	ENUM_SE_CREATE_GLOBAL_NAME,
	ENUM_SE_TRUSTED_CREDMAN_ACCESS_NAME,
	ENUM_SE_RELABEL_NAME,
	ENUM_SE_INC_WORKING_SET_NAME,
	ENUM_SE_TIME_ZONE_NAME,
	ENUM_SE_CREATE_SYMBOLIC_LINK_NAME,
}TOKEN_INFO_EEEE;
// CListMemoryDlg 对话框

IMPLEMENT_DYNAMIC(CListMemoryDlg, CDialogEx)

CListMemoryDlg::CListMemoryDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MEMORY_DIALOG, pParent)
{
	m_dwPid = 0;
	m_pEprocess = 0;
	m_nMemoryCnt = 0;
	m_szInfo = L" ";
}

CListMemoryDlg::~CListMemoryDlg()
{
}

void CListMemoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMORY_LIST, m_list);
}


BEGIN_MESSAGE_MAP(CListMemoryDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_MEMORY_1, &CListMemoryDlg::OnMemoryDlgRefresh)
	ON_NOTIFY(NM_RCLICK, IDC_MEMORY_LIST, &CListMemoryDlg::OnRclickMemoryList)
	ON_MESSAGE(WM_ADD_ITEM_MEMORY_DLG, &CListMemoryDlg::OnAddItem)
	ON_COMMAND(ID_DUMP_MEMORY, &CListMemoryDlg::OnDumpMemory)
	ON_COMMAND(ID_CHANGE_MEMORYPROTECTION, &CListMemoryDlg::OnChangeMemoryprotection)
	ON_COMMAND(ID_ALLOCATE_MEMORY, &CListMemoryDlg::OnAllocateMemory)
	ON_COMMAND(ID_FREE_MEMORY, &CListMemoryDlg::OnFreeMemory)
	ON_COMMAND(ID_MDL_WRITE_MEMORY, &CListMemoryDlg::OnMdlWriteMemory)
END_MESSAGE_MAP()


// CListMemoryDlg 消息处理程序



BOOL CListMemoryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	

	// TODO:  在此添加额外的初始化
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT| LVS_EX_GRIDLINES);
	m_list.InsertColumn(eMemoryBase, L"Base", LVCFMT_LEFT, 130);
	m_list.InsertColumn(eMemorySize, L"Size", LVCFMT_LEFT, 130);
	m_list.InsertColumn(eMemoryProtect, L"Protection", LVCFMT_LEFT, 130);
	m_list.InsertColumn(eMemoryVadFlags, L"VadFlags", LVCFMT_LEFT, 130);
	m_list.InsertColumn(eMemoryState, L"State", LVCFMT_LEFT, 110);
	m_list.InsertColumn(eMemoryType, L"Type", LVCFMT_LEFT, 110);
	m_list.InsertColumn(eMmeoryModuleName, L"ModuleName", LVCFMT_LEFT, 430);

	InitMemoryProtect();




	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CListMemoryDlg::InitSupportThread()
{
	m_list.DeleteAllItems();
	m_nMemoryCnt = 0;
	m_vectorMemory.clear();
	m_vectorModules.clear();
	// 获取进程内存
	m_clsMemory.GetMemorys(m_dwPid, m_pEprocess, m_vectorMemory);
	if (m_vectorMemory.empty())
	{
		return;
	}

	// 获取模块信息
	//m_clsListModules.EnumModulesByPeb((ULONG)m_dwPid, m_pEprocess, m_vectorModules);
	GetModuleNameByObjectPoint();



	for (vector <MEMORY_INFO>::iterator Iter = m_vectorMemory.begin();
		Iter != m_vectorMemory.end();
		Iter++)
	{
		
		SendMessage(WM_ADD_ITEM_MEMORY_DLG, 0x10, m_nMemoryCnt);
		m_nMemoryCnt++;
	}

	
}


VOID CListMemoryDlg::GetModuleNameByObjectPoint()
{
	m_vectorObjectAddress.clear();
	m_vectorFileObject.clear();

	for (vector <MEMORY_INFO>::iterator Iter = m_vectorMemory.begin();
		Iter != m_vectorMemory.end();
		Iter++)
	{
		if (Iter->pFileObject)
		{
			m_vectorObjectAddress.push_back(Iter->pFileObject);
		}
	}
	//除去重复的元素
	sort(m_vectorObjectAddress.begin(), m_vectorObjectAddress.end());
	m_vectorObjectAddress.erase(unique(m_vectorObjectAddress.begin(), m_vectorObjectAddress.end()), m_vectorObjectAddress.end());


	//初始化 m_vectorFileObject
	for (vector <ULONG_PTR>::iterator Iter = m_vectorObjectAddress.begin();
		Iter != m_vectorObjectAddress.end();
		Iter++)
	{
		FILE_OBJECT_INFO info = { 0 };
		CString strImageName = L" ";

		info.pObject = *Iter;
		strImageName = m_clsVad.ObQueryNameString(info.pObject);

		if (!strImageName.IsEmpty()) 
		{
			strImageName = strImageName.Right(strImageName.GetLength() - strImageName.ReverseFind('\\') - 1);
			info.PathName = strImageName;
		}
		m_vectorFileObject.push_back(info);
	}

}



CString CListMemoryDlg::FindModuleName(ULONG_PTR pFileObject)
{
	CString szTemp = L" ";
	if (!m_vectorFileObject.empty())
	{

		for (vector <FILE_OBJECT_INFO>::iterator Iter = m_vectorFileObject.begin();
			Iter != m_vectorFileObject.end();
			Iter++)
		{
			if (Iter->pObject == pFileObject)
			{
				return Iter->PathName;
			}
		}
		
	}
	return  szTemp;
}

void CListMemoryDlg::AddMemoryItem(MEMORY_INFO item)
{
	CString strBase, strSize, strProtect, strType, strImageName, strState,szVadFlags;

	strBase.Format(L"0x%016llx", item.Base);
	strSize.Format(L"0x%016llx", item.Size);
	strProtect = GetMemoryProtect(item.Protect);
	strType = GetMempryType(item.Type);
	strState = GetMempryState(item.State);

	szVadFlags = MemoryProtection(item.VadFlags);
	szVadFlags += MemoryProtectionEx(item.VadFlags);

	if (item.pFileObject)
	{
		//strImageName = GetModuleImageName(item.Base);

		strImageName = FindModuleName(item.pFileObject);
	}

	
	//这个得要处理以下
	int n = m_list.GetItemCount();
	int i = m_list.InsertItem(n, strBase);
	m_list.SetItemText(i, eMemorySize, strSize);
	m_list.SetItemText(i, eMemoryProtect, strProtect);
	m_list.SetItemText(i, eMemoryVadFlags, szVadFlags);
	m_list.SetItemText(i, eMemoryState, strState);
	m_list.SetItemText(i, eMemoryType, strType);
	m_list.SetItemText(i, eMmeoryModuleName, strImageName);

	m_list.SetItemData(i, i);
	
}

void CListMemoryDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow && nStatus == 0) {

		m_szInfo.Format(L"内存总数:%d",m_nMemoryCnt);
		(GetParent()->GetParent())->SetDlgItemText(IDC_PROCESS_TEXT, m_szInfo);
		
	}


	if (!bShow) {


	}
}

CString CListMemoryDlg::MemoryProtection(ULONG_PTR flags)
{
	CString szTemp = L" ";

	MMVAD_FLAGS flag = { 0 };

	RtlCopyMemory(&flag, &flags, sizeof(flag));

	if (flag.Protection == MM_ZERO_ACCESS)
	{
		szTemp = L"NOACESS";
	}
	else if (flag.Protection == MM_READONLY)
	{
		szTemp = L"R";
	}
	else if (flag.Protection == MM_EXECUTE)
	{
		szTemp = L"E";
	}
	else if (flag.Protection == MM_EXECUTE_READ)
	{
		szTemp = L"E-R";
	}
	else if (flag.Protection == MM_READWRITE)
	{
		szTemp = L"R-W";
	}
	else if (flag.Protection == MM_WRITECOPY)
	{
		szTemp = L"WC";
	}
	else if (flag.Protection == MM_EXECUTE_READWRITE)
	{
		szTemp = L"E-R-W";
	}
	else if (flag.Protection == MM_EXECUTE_WRITECOPY)
	{
		szTemp = L"E-WC";
	}
	else if (flag.Protection > MM_EXECUTE_WRITECOPY)
	{
		szTemp = L"NOACESS";
	}
	return szTemp;
}
CString CListMemoryDlg::MemoryProtectionEx(ULONG_PTR flags)
{
	CString szTemp = L" ";
	MM_PRIVATE_VAD_FLAGS flag = { 0 };
	RtlCopyMemory(&flag, &flags, sizeof(flag));

	if (flag.Enclave && flag.NoChange) {
		szTemp = L"-Enclave-NoChange";
	}
	else if (flag.Enclave && flag.NoChange == 0)
	{
		szTemp = L"-Enclave";
	}
	else if (flag.Enclave == 0 && flag.NoChange)
	{
		szTemp = L"-NoChange";
	}


	return szTemp;
}

VOID CListMemoryDlg::InitMemoryProtect()
{
	PROTECT protect;

	memset(&protect, 0, sizeof(PROTECT));
	protect.uType = PAGE_NOACCESS;
	wcsncpy_s(protect.szTypeName, L"No Access", wcslen(L"No Access"));
	m_vectorProtectType.push_back(protect);

	memset(&protect, 0, sizeof(PROTECT));
	protect.uType = PAGE_READONLY;
	wcsncpy_s(protect.szTypeName, L"Read", wcslen(L"Read"));
	m_vectorProtectType.push_back(protect);

	memset(&protect, 0, sizeof(PROTECT));
	protect.uType = PAGE_READWRITE;
	wcsncpy_s(protect.szTypeName, L"ReadWrite", wcslen(L"ReadWrite"));
	m_vectorProtectType.push_back(protect);

	memset(&protect, 0, sizeof(PROTECT));
	protect.uType = PAGE_WRITECOPY;
	wcsncpy_s(protect.szTypeName, L"WriteCopy", wcslen(L"WriteCopy"));
	m_vectorProtectType.push_back(protect);

	memset(&protect, 0, sizeof(PROTECT));
	protect.uType = PAGE_EXECUTE;
	wcsncpy_s(protect.szTypeName, L"Execute", wcslen(L"Execute"));
	m_vectorProtectType.push_back(protect);

	memset(&protect, 0, sizeof(PROTECT));
	protect.uType = PAGE_EXECUTE_READ;
	wcsncpy_s(protect.szTypeName, L"ReadExecute", wcslen(L"ReadExecute"));
	m_vectorProtectType.push_back(protect);

	memset(&protect, 0, sizeof(PROTECT));
	protect.uType = PAGE_EXECUTE_READWRITE;
	wcsncpy_s(protect.szTypeName, L"ReadWriteExecute", wcslen(L"ReadWriteExecute"));
	m_vectorProtectType.push_back(protect);

	memset(&protect, 0, sizeof(PROTECT));
	protect.uType = PAGE_EXECUTE_WRITECOPY;
	wcsncpy_s(protect.szTypeName, L"WriteCopyExecute", wcslen(L"WriteCopyExecute"));
	m_vectorProtectType.push_back(protect);

	memset(&protect, 0, sizeof(PROTECT));
	protect.uType = PAGE_GUARD;
	wcsncpy_s(protect.szTypeName, L"Guard", wcslen(L"Guard"));
	m_vectorProtectType.push_back(protect);

	memset(&protect, 0, sizeof(PROTECT));
	protect.uType = PAGE_NOCACHE;
	wcsncpy_s(protect.szTypeName, L"No Cache", wcslen(L"No Cache"));
	m_vectorProtectType.push_back(protect);

	memset(&protect, 0, sizeof(PROTECT));
	protect.uType = PAGE_WRITECOMBINE;
	wcsncpy_s(protect.szTypeName, L"WriteCombine", wcslen(L"WriteCombine"));
	m_vectorProtectType.push_back(protect);

	memset(&protect, 0, sizeof(PROTECT));
	protect.uType = PAGE_ENCLAVE_THREAD_CONTROL;
	wcsncpy_s(protect.szTypeName, L"PAGE_ENCLAVE_THREAD_CONTROL|PAGE_REVERT_TO_FILE_MAP", wcslen(L"PAGE_ENCLAVE_THREAD_CONTROL|PAGE_REVERT_TO_FILE_MAP"));
	m_vectorProtectType.push_back(protect);


	memset(&protect, 0, sizeof(PROTECT));
	protect.uType = PAGE_TARGETS_NO_UPDATE;
	wcsncpy_s(protect.szTypeName, L"PAGE_REVERT_TO_FILE_MAP|PAGE_TARGETS_INVALID", wcslen(L"PAGE_TARGETS_NO_UPDATE|PAGE_TARGETS_INVALID"));
	m_vectorProtectType.push_back(protect);


	memset(&protect, 0, sizeof(PROTECT));
	protect.uType = PAGE_ENCLAVE_UNVALIDATED;
	wcsncpy_s(protect.szTypeName, L"PAGE_ENCLAVE_UNVALIDATED|PAGE_ENCLAVE_NO_CHANGE", wcslen(L"PAGE_ENCLAVE_UNVALIDATED|PAGE_ENCLAVE_NO_CHANGE"));
	m_vectorProtectType.push_back(protect);

	memset(&protect, 0, sizeof(PROTECT));
	protect.uType = PAGE_ENCLAVE_DECOMMIT;
	wcsncpy_s(protect.szTypeName, L"PAGE_ENCLAVE_DECOMMIT", wcslen(L"PAGE_ENCLAVE_DECOMMIT"));
	m_vectorProtectType.push_back(protect);

}

CString CListMemoryDlg::GetMemoryProtect(ULONG_PTR Protect)
{
	BOOL bFirst = TRUE;
	CString strProtect = L"";

	for (vector <PROTECT>::iterator Iter = m_vectorProtectType.begin();
		Iter != m_vectorProtectType.end();
		Iter++)
	{
		PROTECT item = *Iter;
		if (item.uType & Protect)
		{
			if (bFirst == TRUE)
			{
				strProtect = item.szTypeName;
				bFirst = FALSE;
			}
			else
			{
				strProtect += L" - ";
				strProtect += item.szTypeName;
			}
		}
	}

	return strProtect;
}

//
// 获取内存状态属性
//
CString CListMemoryDlg::GetMempryState(ULONG_PTR State)
{
	CString szState = L"";

	if (State == MEM_COMMIT)
	{
		szState = L"Commit";
	}
	else if (State == MEM_FREE)
	{
		szState = L"Free";
	}
	else if (State == MEM_RESERVE)
	{
		szState = L"Reserve";
	}
	else if (State == MEM_DECOMMIT)
	{
		szState = L"Decommit";
	}
	else if (State == MEM_RELEASE)
	{
		szState = L"Release";
	}

	return szState;
}

//
// 获取内存类型属性
//
CString CListMemoryDlg::GetMempryType(ULONG_PTR Type)
{
	CString szType = L"";

	if (Type == MEM_PRIVATE)
	{
		szType = L"Private";
	}
	else if (Type == MEM_MAPPED)
	{
		szType = L"Map";
	}
	else if (Type == MEM_IMAGE)
	{
		szType = L"Image";
	}
	
	return szType;
}


//
// 根据内存地址获得image name
//
CString CListMemoryDlg::GetModuleImageName(ULONG_PTR dwBase)
{
	CString strImageName = L"";

	for (vector <MODULE_INFO_EX>::iterator Iter = m_vectorModules.begin(); Iter != m_vectorModules.end(); Iter++)
	{
		MODULE_INFO_EX entry = *Iter;

		if (dwBase >= entry.Base && dwBase <= (entry.Base + entry.Size))
		{
			CString strPath = entry.Path;
			strImageName = strPath.Right(strPath.GetLength() - strPath.ReverseFind('\\') - 1);
			break;
		}
	}

	return strImageName;
}

void CListMemoryDlg::OnMemoryDlgRefresh()
{
	
	//EnumMemorys();
	if (m_vectorMemory.size() == m_nMemoryCnt )
	{
		
		AfxBeginThread(threadWork, this);
		
	}
}


void CListMemoryDlg::OnRclickMemoryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_MEMORY_1, L"刷新");
	menu.AppendMenu(MF_SEPARATOR);

	if (m_list.m_hWnd)
	{


		int nItem = m_Functions.GetSelectItem(&m_list);
		if (-1 == nItem)
		{
			return;
		}
		HANDLE dwPid = 0;
		PMEMORY_INFO pInfo = GetMemoryInfoByItem(nItem);
		if (!pInfo)return;
		
		if (pInfo->State == MEM_COMMIT )
		{

			menu.AppendMenu(MF_STRING, ID_DUMP_MEMORY, L"dump内存");
			menu.AppendMenu(MF_SEPARATOR);
		}
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, ID_CHANGE_MEMORYPROTECTION, L"改变内存属性");
		menu.AppendMenu(MF_SEPARATOR);

		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, ID_ALLOCATE_MEMORY, L"分配内存");
		menu.AppendMenu(MF_SEPARATOR);

		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, ID_FREE_MEMORY, L"释放内存");
		menu.AppendMenu(MF_SEPARATOR);


		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, ID_MDL_WRITE_MEMORY, L"MDL写入");
		menu.AppendMenu(MF_SEPARATOR);



	}


	CPoint pt;
	GetCursorPos(&pt);
	menu.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	menu.DestroyMenu();


	*pResult = 0;
}

LRESULT CListMemoryDlg::OnAddItem(WPARAM wParam, LPARAM lParam)
{
	
	if (wParam == 0x10) {

		AddMemoryItem(m_vectorMemory.at((int)lParam));
		if (this->IsWindowVisible()) {
			m_szInfo.Format(L"内存总数:%d", (int)lParam);
			(GetParent()->GetParent())->SetDlgItemText(IDC_PROCESS_TEXT, m_szInfo);
		}
	}

	if (lParam == 0x2021 && wParam == 0x2021)
	{
		CMenu* m_pSysMenu = (GetParent()->GetParent())->GetSystemMenu(FALSE);

		m_pSysMenu->EnableMenuItem(SC_CLOSE, MF_DISABLED);
	}

	if (lParam == 0x2022 && wParam == 0x2022)
	{
		CMenu* m_pSysMenu = (GetParent()->GetParent())->GetSystemMenu(FALSE);
		m_pSysMenu->EnableMenuItem(SC_CLOSE, MF_ENABLED);
	}


	return TRUE;
}



 UINT  CListMemoryDlg:: threadWork(LPVOID lParam)
{

	CListMemoryDlg *pThis = (CListMemoryDlg*)lParam;
	if (pThis) 
	{
		pThis->SendMessage(WM_ADD_ITEM_MEMORY_DLG, 0x2021, 0x2021);

		pThis->InitSupportThread();

		pThis->SendMessage(WM_ADD_ITEM_MEMORY_DLG, 0x2022, 0x2022);
		
	}
	return 0;
}

 void CListMemoryDlg::OnDumpMemory()
 {
	 if (m_list.m_hWnd)
	 {
		 int nItem = m_Functions.GetSelectItem(&m_list);
		 if (-1 == nItem)
		 {
			 return;
		 }
		 HANDLE dwPid = 0;
		 PMEMORY_INFO pInfo = GetMemoryInfoByItem(nItem);
		 if (!pInfo)return;
		 dwPid = m_dwPid;
		 CString szTemp;
		 if (m_clsModules.DumpModuleMemoryToFile((ULONG)dwPid, pInfo->Base, pInfo->Size, szTemp))
		 {
			 AfxMessageBox(L"拷贝成功\n");

		 }

	
	 }
 }


 PMEMORY_INFO CListMemoryDlg::GetMemoryInfoByItem(int nItem)
 {
	 PMEMORY_INFO pInfo = NULL;

	 if (nItem == -1)
	 {
		 return pInfo;
	 }

	 int nData = (int)m_list.GetItemData(nItem);
	 pInfo = &m_vectorMemory.at(nData);

	 return pInfo;
 }


 void CListMemoryDlg::OnChangeMemoryprotection()
 {
	 if (m_list.m_hWnd)
	 {
		 int nItem = m_Functions.GetSelectItem(&m_list);
		 if (-1 == nItem)
		 {
			 return;
		 }
		 HANDLE dwPid = 0;
		 PMEMORY_INFO pInfo = GetMemoryInfoByItem(nItem);
		 if (!pInfo)return;
		 dwPid = m_dwPid;

		 CChangeProtectionDlg changeMemoryDlg;
		 changeMemoryDlg.m_dwPid = (ULONG)dwPid;
		 changeMemoryDlg.m_pMemoryInfo = pInfo;
		
		 changeMemoryDlg.DoModal();

		
	 }
 }


 void CListMemoryDlg::OnAllocateMemory()
 {
	 if (m_list.m_hWnd)
	 {

		 CAllocateMemoryDlg dlg;
		 dlg.m_dwPid = (ULONG)m_dwPid;
		 dlg.DoModal();
	 }
 }


 void CListMemoryDlg::OnFreeMemory()
 {
	 ULONG nStatus = -1;
	 CString szTemp;
	 if (m_list.m_hWnd)
	 {
		 int nItem = m_Functions.GetSelectItem(&m_list);
		 if (-1 == nItem)
		 {
			 return;
		 }
		 HANDLE dwPid = 0;
		 PMEMORY_INFO pInfo = GetMemoryInfoByItem(nItem);
		 if (!pInfo)return;
		 dwPid = m_dwPid;

		 nStatus = m_clsMemory.FreeVirtualMemory((ULONG)dwPid, pInfo->Base, pInfo->Size);
		 if (nStatus == 0) {
			 m_list.DeleteItem(nItem);
		 }
		 else {
			 szTemp.Format(L"释放失败:%x\n", nStatus);
			 AfxMessageBox(szTemp);
		 }
	 }
 }


 void CListMemoryDlg::OnMdlWriteMemory()
 {
	 BOOL bRet = FALSE;
	 CString szTemp;
	 if (m_list.m_hWnd)
	 {
		 int nItem = m_Functions.GetSelectItem(&m_list);
		 if (-1 == nItem)
		 {
			 return;
		 }
		 HANDLE dwPid = 0;
		 PMEMORY_INFO pInfo = GetMemoryInfoByItem(nItem);
		 if (!pInfo)return;
		 dwPid = m_dwPid;

		 if (pInfo->State == MEM_COMMIT)
		 {
			 CString szFilePath;
			 BOOL bCopy = FALSE;
			 PVOID pBuffer = NULL;
			 ULONG nSize = 0;

			 CFileDialog fileDlg(TRUE, 0, NULL, 0, L"All Files (*.*)|*.*||", 0);
			 if (IDOK == fileDlg.DoModal())
			 {

				 szFilePath = fileDlg.GetPathName();
				 if (PathFileExists(szFilePath))
				 {
					 CFile file;
					 TRY
					 {
						 if (file.Open(szFilePath,CFile::modeRead| CFile::typeBinary))
						 {
							 nSize = file.GetLength();

							 pBuffer = malloc(nSize);
							 if (!pBuffer) {
								 AfxMessageBox(L"申请内存失败");
								 return;
							 }
							 RtlZeroMemory(pBuffer, nSize);

							 file.Read(pBuffer, nSize);
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
				 }
				 if (bCopy)
				 {
					 ULONG nFinllySize = 0;

					 nFinllySize = nSize > pInfo->Size ? pInfo->Size : nSize;

					 bRet = m_clsMemory.RtlSuperCopyMemory((ULONG)dwPid, pInfo->Base, pBuffer, nFinllySize);
					 if (pBuffer) {
						 free(pBuffer);
						 pBuffer = NULL;
					 }
				 }
				 
			 }
		 }
		 else
		 {
			 bRet = FALSE;
		 }
		 if (bRet) {
			 AfxMessageBox(L"写入成功");
		 }
		 else
		 {
			 AfxMessageBox(L"写入失败");
		 }
	 }
 }


