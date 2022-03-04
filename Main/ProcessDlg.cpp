// CProcessDlg.cpp: 实现文件
//

#include "pch.h"
#include "Main.h"
#include "ProcessDlg.h"
#include "afxdialogex.h"
#include "MainDlg.h"
#include "Function.h"
#include <TlHelp32.h>
#include <algorithm>
#include "ProcessInfoDlg.h"
// CProcessDlg 对话框

extern CLoadDriver g_connectDriver;
IMPLEMENT_DYNAMIC(CListProcessDlg, CDialogEx)

CListProcessDlg::CListProcessDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROCESS_DIALOG, pParent)
{
	m_nProcessCnt = m_nHideCnt = m_nDenyAccessCnt = 0;
	m_clrHideProc = RGB(255, 0, 0);					    // 隐藏进程默认是 红色
	m_clrMicrosoftProc = RGB(0, 0, 0);					// 微软进程默认是 黑色
	m_clrNormalProc = RGB(0, 0, 255);					// 正常的进程默认是 蓝色
	m_clrMicroHaveOtherModuleProc = RGB(255, 140, 0);	// 微软带有非微软模块默认是 橘黄色
	m_clrNoSigniture = RGB(180, 0, 255);				// 默认是紫色
}

CListProcessDlg::~CListProcessDlg()
{
}

void CListProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROCESS_LIST, m_processlist);
}


BEGIN_MESSAGE_MAP(CListProcessDlg, CDialogEx)
ON_WM_SHOWWINDOW()
ON_NOTIFY(NM_RCLICK, IDC_PROCESS_LIST, &CListProcessDlg::OnRclickProcessList)        //右键 菜单
ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROCESS_LIST, &CListProcessDlg::OnCustomdrawProcessList) //绘制 颜色
ON_COMMAND(ID_PROCESSDLG_REFRESH, &CListProcessDlg::OnHelpRefresh)
ON_COMMAND(ID_PROCESSDLG_SHOW_ALL_INFO, &CListProcessDlg::OnHelpShowAllInfo)
ON_COMMAND(ID_SUSPROCESS, &CListProcessDlg::OnSusprocess)
ON_COMMAND(ID_RESUSEPROCESS, &CListProcessDlg::OnResumeProcess)


ON_COMMAND(ID_LOCATION_EXPLORER, &CListProcessDlg::OnLocationExplorer)
ON_COMMAND(ID_COPY_PROCESS_NAME, &CListProcessDlg::OnCopyProcessName)
ON_COMMAND(ID_COPY_PROCESS_PATH, &CListProcessDlg::OnCopyProcessPath)
ON_COMMAND(ID_KILL_PROCESS, &CListProcessDlg::OnKillProcess)
ON_COMMAND(ID_EXPORT_TEXT, &CListProcessDlg::OnExportText)
ON_COMMAND(ID_EXPORT_EXCEL, &CListProcessDlg::OnExportExcel)
END_MESSAGE_MAP()


// CProcessDlg 消息处理程序


BOOL CListProcessDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
		

	// 初始化默认的进程图标
	////////////////////////////////////////////////////////////
	m_ProImageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 2);
	m_processlist.SetImageList(&m_ProImageList, LVSIL_SMALL);

	m_processlist.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_processlist.InsertColumn(eProcessImageName, L"ImageName", LVCFMT_LEFT, 130);
	m_processlist.InsertColumn(eProcessPid, L"PID", LVCFMT_LEFT, 65);
	m_processlist.InsertColumn(eProcessParentPid, L"Parent Pid", LVCFMT_LEFT, 65);
	m_processlist.InsertColumn(eProcessPath, L"ImagePath", LVCFMT_LEFT, 290);
	m_processlist.InsertColumn(eProcessEprocess, L"Eprocess", LVCFMT_LEFT, 80);
	m_processlist.InsertColumn(eProcessR3Access, L"Ring3 Access", LVCFMT_CENTER, 75);
	m_processlist.InsertColumn(eProcessFileCompany, L"File Corporation", LVCFMT_LEFT, 140);
	m_processlist.InsertColumn(eProcessFileDescription, L"File Description", LVCFMT_LEFT, 190);
	m_processlist.InsertColumn(eProcessCreateTime, L"Creation Time", LVCFMT_LEFT, 140);
	m_processlist.InsertColumn(eProcessCommandLine, L"Command Line", LVCFMT_LEFT, 140);

	m_hProcessIcon = LoadIcon(NULL, IDI_APPLICATION);



	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CListProcessDlg::ListProcess()
{
	ShowProcessAsList();
}

BOOL UDgreaterFileTime(PROCESS_INFO elem1, PROCESS_INFO elem2)
{
	return elem1.ulCreateTime < elem2.ulCreateTime;
}

// 以List的形式展示进程
void CListProcessDlg::ShowProcessAsList()
{
	// 清空image列表、进程列表，初始化默认图标
	m_processlist.DeleteAllItems();
	m_vectorColor.clear();
	m_vectorProcesses.clear();
	

	int nImageCount = m_ProImageList.GetImageCount();
	for (int j = 0; j < nImageCount; j++)
	{
		m_ProImageList.Remove(0);
	}

	// 初始清零
	m_nDenyAccessCnt = m_nProcessCnt = m_nHideCnt = 0;

	//内核获取系统全部进程
	m_clsProcess.EnumProcess(m_Ring0ProcessList);

	// 用CreateToolhelp32Snapshot的方法获取应用层的进程
	GetRing3ProcessList();

	//默认 根据创建时间先后 排序
	sort(m_Ring0ProcessList.begin(), m_Ring0ProcessList.end(), UDgreaterFileTime);


	for (vector <PROCESS_INFO>::iterator Iter = m_Ring0ProcessList.begin();
		Iter != m_Ring0ProcessList.end();
		Iter++)
	{
		
		AddProcessItem(*Iter);
	}

	m_szProcessCnt.Format(L"Processes: %d, Hidden Processes: %d, Ring3 Inaccessible Processes: %d", m_nProcessCnt, m_nHideCnt, m_nDenyAccessCnt);
	
	::SetWindowText(AfxGetMainWnd()->GetDlgItem(IDC_SHOW_TEXT)->m_hWnd, m_szProcessCnt);

}

void CListProcessDlg::AddProcessItem(PROCESS_INFO item)
{
	if (item.ulPid == 0 && item.ulParentPid == 0 && item.ulEprocess == 0)
	{
		return;
	}

	CString strPid, strParentPid, strEprocess, strCompany, strDescription, strR3Access, szDescription, szStartedTime,szCommandLine;
	

	// 处理进程路径，pid
	CString strPath = TrimPath(item.szPath);
	CString strImageName = strPath.Right(strPath.GetLength() - strPath.ReverseFind('\\') - 1);
	item.ulParentPid ? strParentPid.Format(L"%d", item.ulParentPid) : strParentPid.Format(L"-");

	// 处理进程图标
	SHFILEINFO sfi;
	DWORD_PTR nRet;
	HICON hIcon = NULL;

	ZeroMemory(&sfi, sizeof(SHFILEINFO));

	nRet = SHGetFileInfo(strPath,
		FILE_ATTRIBUTE_NORMAL,
		&sfi,
		sizeof(SHFILEINFO),
		SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES
	);
	int nIcon = -1;

	if (nRet)
	{
		nIcon = m_ProImageList.Add(sfi.hIcon);
		DestroyIcon(sfi.hIcon);
	}
	else
	{
		nIcon = m_ProImageList.Add(m_hProcessIcon);
	}

	if (CanOpenProcess(item.ulPid))
	{
		strR3Access = L"-";
	}
	else
	{
		strR3Access = L"拒绝";
		m_nDenyAccessCnt++;
	}

	strEprocess.Format(L"0x%llX", item.ulEprocess);
	strPid.Format(L"%d", item.ulPid);
	strCompany = m_Functions.GetFileCompanyName(strPath);//输入路径 转换成公司名 如: Microsoft Corporation



	szDescription = m_Functions.GetFileDescription(strPath);//如 Windows 会话管理器
	szStartedTime = FileTime2SystemTime(item.ulCreateTime);
	szCommandLine = m_clsProcess.GetProcessCommandLine(item.ulPid);



	int nCnt = m_processlist.GetItemCount();
	int i = m_processlist.InsertItem(nCnt, strImageName, nIcon);
	m_processlist.SetItemText(i, eProcessPid, strPid);
	m_processlist.SetItemText(i, eProcessParentPid, strParentPid);
	m_processlist.SetItemText(i, eProcessPath, strPath);
	m_processlist.SetItemText(i, eProcessEprocess, strEprocess);
	m_processlist.SetItemText(i, eProcessR3Access, strR3Access);
	m_processlist.SetItemText(i, eProcessFileCompany, strCompany);
	m_processlist.SetItemText(i, eProcessFileDescription, szDescription);
	m_processlist.SetItemText(i, eProcessCreateTime, szStartedTime);
	m_processlist.SetItemText(i, eProcessCommandLine, szCommandLine);

	// 默认黑色
	ITEM_COLOR_INFO clrInfo;
	clrInfo.textClr = enumBlack;

	// 如果是隐藏的，那么显示红色
	if (IsProcessHide(item.ulPid))
	{
		m_nHideCnt++;
		clrInfo.textClr = enumRed;
	}

	// 如果不是微软的模块，那么就显示蓝色
	else if (!(m_Functions.IsMicrosoftApp(strCompany) ||
		!strPath.CompareNoCase(L"System") ||
		!strPath.CompareNoCase(L"Idle")))
	{
		clrInfo.textClr = enumBlue;
	}

	// 如果微软进程被注入了其他模块，那么显示土黄色
	//if (clrInfo.textClr == enumBlack && IsHaveNotMicrosoftModule((DWORD)item.ulPid))
	//{
	//	clrInfo.textClr = enumTuhuang;
	//}

	m_vectorColor.push_back(clrInfo);
	m_vectorProcesses.push_back(item);


	m_processlist.SetItemData(i, nCnt);

	m_nProcessCnt++;

}

//
// 采用Snapshot的方法(快速)来判断是否有非微软的模块
//
BOOL CListProcessDlg::IsHaveNotMicrosoftModule(DWORD dwPID)
{
	BOOL bRet = FALSE;
	HANDLE hModuleSnap = NULL;
	MODULEENTRY32 me32 = { 0 };

	hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hModuleSnap == INVALID_HANDLE_VALUE || dwPID == 0)
	{
		return FALSE;
	}

	me32.dwSize = sizeof(MODULEENTRY32);
	if (::Module32First(hModuleSnap, &me32))
	{
		do {

			if (!m_Functions.IsMicrosoftAppByPath(m_Functions.TrimPath(me32.szExePath)))
			{
				bRet = TRUE;
				break;
			}

		} while (::Module32Next(hModuleSnap, &me32));
	}

	CloseHandle(hModuleSnap);
	return bRet;
}


void CListProcessDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码

	if (bShow && nStatus == 0) {

		ListProcess();
	}


	if (!bShow) {


	}

	return;
}
// ring0的pid跟ring3的对比，得出是否是隐藏进程
BOOL CListProcessDlg::IsProcessHide(HANDLE pid)
{
	BOOL bHide = FALSE;

	if (!m_Ring3ProcessList.empty())
	{
		list<HANDLE>::iterator findItr;
		findItr = find(m_Ring3ProcessList.begin(), m_Ring3ProcessList.end(), pid);
		if (findItr == m_Ring3ProcessList.end())
		{
			bHide = TRUE;
		}
	}

	return bHide;
}

CString CListProcessDlg::FileTime2SystemTime(ULONGLONG ulCreateTile)
{
	if (!ulCreateTile)
	{
		return L"-";
	}

	CString szRet;
	SYSTEMTIME SystemTime;
	FILETIME ft, ftLocal;

	ft.dwLowDateTime = (ULONG)ulCreateTile;
	ft.dwHighDateTime = (ULONG)(ulCreateTile >> 32);

	FileTimeToLocalFileTime(&ft, &ftLocal);

	if (FileTimeToSystemTime(&ftLocal, &SystemTime))
	{
		szRet.Format(
			L"%04d/%02d/%02d  %02d:%02d:%02d",
			SystemTime.wYear,
			SystemTime.wMonth,
			SystemTime.wDay,
			SystemTime.wHour,
			SystemTime.wMinute,
			SystemTime.wSecond);
	}

	return szRet;
}

// ring3 使用常规方法枚举进程
BOOL CListProcessDlg::GetRing3ProcessList()
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	BOOL bRet = FALSE;

	// 首先清空下链表
	m_Ring3ProcessList.clear();

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap != INVALID_HANDLE_VALUE)
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hProcessSnap, &pe32))
		{
			do
			{
				
			   m_Ring3ProcessList.push_back((HANDLE)pe32.th32ProcessID);
			
			} while (Process32Next(hProcessSnap, &pe32));

			bRet = TRUE;
		}

		CloseHandle(hProcessSnap);
	}

	return bRet;
}



// list控件的颜色绘制
void CListProcessDlg::OnCustomdrawProcessList(NMHDR *pNMHDR, LRESULT *pResult)
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
		COLORREF clrNewTextColor, clrNewBkColor;
		int nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);

		clrNewTextColor = m_clrMicrosoftProc;
		clrNewBkColor = RGB(255, 255, 255);

		int nData = (int)m_processlist.GetItemData(nItem);
		ITEM_COLOR clrInfo = m_vectorColor.at(nData).textClr;
		if (clrInfo == enumRed)
		{
			clrNewTextColor = m_clrHideProc;
		}
		else if (clrInfo == enumBlue)
		{
			clrNewTextColor = m_clrNormalProc;
		}
		else if (clrInfo == enumTuhuang)
		{
			clrNewTextColor = m_clrMicroHaveOtherModuleProc;
		}
		else if (clrInfo == enumPurple)
		{
			clrNewTextColor = m_clrNoSigniture;
		}
		else if (clrInfo == enumGrey)
		{
			clrNewTextColor = RGB(96,96,96);
		}
		else if (clrInfo == enumGreen)
		{
			clrNewTextColor = RGB(0, 255, 0);
		}



		pLVCD->clrText = clrNewTextColor;   //文字
		pLVCD->clrTextBk = clrNewBkColor;    //背景颜色

		*pResult = CDRF_DODEFAULT;
	}
}
BOOL CListProcessDlg::CanOpenProcess(HANDLE dwPid)
{
	BOOL bOpen = TRUE;

	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_VM_OPERATION, TRUE, (DWORD)dwPid);
	if (hProcess)
	{
		CloseHandle(hProcess);
	}
	else
	{
		bOpen = FALSE;
	}

	return bOpen;
}



// list控件的右键菜单
void CListProcessDlg::OnRclickProcessList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码


	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_PROCESSDLG_REFRESH, L"刷新");
	menu.AppendMenu(MF_SEPARATOR);

	menu.AppendMenu(MF_STRING, ID_PROCESSDLG_SHOW_ALL_INFO, L"进程信息");
	menu.AppendMenu(MF_SEPARATOR);


	menu.AppendMenu(MF_STRING, ID_LOCATION_EXPLORER, L"定位文件");
	menu.AppendMenu(MF_SEPARATOR);

	menu.AppendMenu(MF_STRING, ID_COPY_PROCESS_NAME, L"复制进程名字");
	menu.AppendMenu(MF_STRING, ID_COPY_PROCESS_PATH, L"复制进程路径");
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_KILL_PROCESS, L"结束进程");
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_SEPARATOR);


	menu.AppendMenu(MF_STRING, ID_SUSPROCESS, L"暂停进程");
	menu.AppendMenu(MF_STRING, ID_RESUSEPROCESS, L"恢复进程");
	menu.AppendMenu(MF_SEPARATOR);

	menu.AppendMenu(MF_STRING, ID_EXPORT_TEXT, L"导出");
	menu.AppendMenu(MF_SEPARATOR);
	/*
	SUSPEND_OR_RESUME_TYPE SorType = IsSuspendOrResumeProcessTree();

	switch (SorType)
	{
	case enumAllFalse:
		menu.EnableMenuItem(ID_SUSPROCESS, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
		menu.EnableMenuItem(ID_RESUSEPROCESS, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
		break;

	case enumAllOk:
		menu.EnableMenuItem(ID_SUSPROCESS, MF_BYCOMMAND | MF_ENABLED);
		menu.EnableMenuItem(ID_RESUSEPROCESS, MF_BYCOMMAND | MF_ENABLED);
		break;

	case enumResume:
		menu.EnableMenuItem(ID_SUSPROCESS, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
		menu.EnableMenuItem(ID_RESUSEPROCESS, MF_BYCOMMAND | MF_ENABLED);
		break;

	case enumSuspend:
		menu.EnableMenuItem(ID_SUSPROCESS, MF_BYCOMMAND | MF_ENABLED);
		menu.EnableMenuItem(ID_RESUSEPROCESS, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
		break;
	}
	*/

	CPoint pt;
	//ClientToScreen(&pt);
	GetCursorPos(&pt);
	menu.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	menu.DestroyMenu();


	*pResult = 0;

}


void CListProcessDlg::OnHelpRefresh()
{
	ShowProcessAsList();
}




void CListProcessDlg::ShowProcessListDetailInfo(int nItem)
{
	if (nItem == -1)
	{
		return;
	}

	PPROCESS_INFO pInfo = GetProcessInfoByItem(nItem);
	
	if (!pInfo)
	{
		return;
	}

	CProcessInfoDlg ProcessInfoDlg;

	ProcessInfoDlg.m_nPid = pInfo->ulPid;
	ProcessInfoDlg.m_pEprocess = pInfo->ulEprocess;
	ProcessInfoDlg.m_szImageName = m_processlist.GetItemText(nItem, eProcessImageName);
	ProcessInfoDlg.m_szPath = TrimPath(pInfo->szPath);
	ProcessInfoDlg.m_szParentImage = GetParentProcessInfo(pInfo->ulParentPid);
	ProcessInfoDlg.DoModal();
}


//
// 根据pid获取父进程的image name信息
//
CString CListProcessDlg::GetParentProcessInfo(HANDLE dwParentPid)
{
	CString szRet;
	CString szImage;
	BOOL bFind = FALSE;
	for (vector <PROCESS_INFO>::iterator Iter = m_Ring0ProcessList.begin(); Iter != m_Ring0ProcessList.end(); Iter++)
	{
		if (Iter->ulPid == dwParentPid)
		{
			CString szPath = Iter->szPath;
			szImage = szPath.Right(szPath.GetLength() - szPath.ReverseFind('\\') - 1);
			bFind = TRUE;
			break;
		}
	}

	if (!bFind)
	{
		szImage = L"Non-existent process";
	}

	szRet.Format(L"%s (%d)", szImage, dwParentPid);
	return szRet;
}


//module thread and others
void CListProcessDlg::OnHelpShowAllInfo()
{
	if (m_processlist.m_hWnd)
	{
		int nItem = m_Functions.GetSelectItem(&m_processlist);
	
		if (-1 == nItem)
		{
			return;
		}

		ShowProcessListDetailInfo(nItem);

	}


}

//
// 进程列表模式下，根据item得到相对应PROCESS_INFO的信息
//
PPROCESS_INFO CListProcessDlg::GetProcessInfoByItem(int nItem)
{
	PPROCESS_INFO pInfo = NULL;

	if (nItem == -1)
	{
		return pInfo;
	}

	int nData = (int)m_processlist.GetItemData(nItem);
	pInfo = &m_vectorProcesses.at(nData);

	return pInfo;
}

void CListProcessDlg::OnSusprocess()
{
	if (m_processlist.m_hWnd)
	{
		int nItem = m_Functions.GetSelectItem(&m_processlist);
		if (-1 == nItem)
		{
			return;
		}

		PPROCESS_INFO pInfo = GetProcessInfoByItem(nItem);
		if (!pInfo || pInfo->ulPid == (HANDLE)GetCurrentProcessId())return;

		if (m_clsProcess.SuspendProcess(pInfo->ulPid))
		{
			m_vectorColor.at(nItem).textClr = enumGreen;
			
		}

	}
}


void CListProcessDlg::OnResumeProcess()
{
	if (m_processlist.m_hWnd)
	{
		int nItem = m_Functions.GetSelectItem(&m_processlist);
		if (-1 == nItem)
		{
			return;
		}

		PPROCESS_INFO pInfo = GetProcessInfoByItem(nItem);
		if (!pInfo || pInfo->ulPid == (HANDLE)GetCurrentProcessId())return;

		if (m_clsProcess.ResumeProcess(pInfo->ulPid))
		{
			m_vectorColor.at(nItem).textClr = enumBlack;

		}


	}
}






void CListProcessDlg::OnLocationExplorer()
{
	CString szPath = L"";

	if (m_processlist.m_hWnd)
	{
		POSITION pos = m_processlist.GetFirstSelectedItemPosition();
		if (pos != NULL)
		{
			int nItem = m_processlist.GetNextSelectedItem(pos);
			szPath = m_processlist.GetItemText(nItem, eProcessPath);
		}
	}

	m_Functions.LocationExplorer(szPath);
}


void CListProcessDlg::OnCopyProcessName()
{
	CString szImageName;
	POSITION pos = m_processlist.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_processlist.GetNextSelectedItem(pos);
		szImageName = m_processlist.GetItemText(nItem, eProcessImageName);
	}
	m_Functions.SetStringToClipboard(szImageName);
}


void CListProcessDlg::OnCopyProcessPath()
{
	CString szPath;

	POSITION pos = m_processlist.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_processlist.GetNextSelectedItem(pos);
		szPath = m_processlist.GetItemText(nItem, eProcessPath);
	}

	m_Functions.SetStringToClipboard(szPath);
}


void CListProcessDlg::OnKillProcess()
{
	if (m_processlist.m_hWnd)
	{
		int nItem = m_Functions.GetSelectItem(&m_processlist);
		if (-1 == nItem)
		{
			return;
		}

		PPROCESS_INFO pInfo = GetProcessInfoByItem(nItem);
		if (!pInfo || pInfo->ulPid == (HANDLE)GetCurrentProcessId())return;

		if (m_clsProcess.KillProcess(pInfo->ulPid))
		{
			m_processlist.DeleteItem(nItem);
		}

	}
}


void CListProcessDlg::OnExportText()
{
	m_Functions.ExportListToTxt(&m_processlist,L"");
}


void CListProcessDlg::OnExportExcel()
{
	WCHAR szProcess[] = { 'P','r','o','c','e','s','s','\0' };
	//m_Functions.ExportListToExcel(&m_processlist, szProcess, m_szProcessCnt);
}
