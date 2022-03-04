// ListThreadDlg.cpp: 实现文件
//

#include "pch.h"
#include "Main.h"
#include "ListThreadDlg.h"
#include "afxdialogex.h"
#include "CallDlg.h"
#include "CApcDlg.h"

#define SYSTEM_ADDRESS_START 0x00007ffffffeffff
// CListThreadDlg 对话框

IMPLEMENT_DYNAMIC(CListThreadDlg, CDialogEx)

CListThreadDlg::CListThreadDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_THREAD_DIALOG, pParent)
{
	m_nPid = 0;
	m_szImageName = L"";
	m_pEprocess = 0;
}

CListThreadDlg::~CListThreadDlg()
{
}

void CListThreadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_THREAD_LIST, m_threadList);
}


BEGIN_MESSAGE_MAP(CListThreadDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_THREAD_LIST, &CListThreadDlg::OnCustomdrawThreadList)
	ON_NOTIFY(NM_RCLICK, IDC_THREAD_LIST, &CListThreadDlg::OnRclickThreadList)
	ON_COMMAND(ID_MEMORY_1, &CListThreadDlg::OnRefreshThread)
	ON_COMMAND(ID_SUSPEND_THREAD, &CListThreadDlg::OnSuspendThread)
	ON_COMMAND(ID_RESUME_THREAD, &CListThreadDlg::OnResumeThread)
	ON_COMMAND(ID_KILL_THREAD, &CListThreadDlg::OnKillThread)
	ON_COMMAND(ID_EXPORT_TEXT, &CListThreadDlg::OnExportText)
	ON_COMMAND(ID_REMOTE_CALL, &CListThreadDlg::OnRemoteCall)
	ON_COMMAND(ID_APC_CALL, &CListThreadDlg::OnApcCall)
END_MESSAGE_MAP()


// CListThreadDlg 消息处理程序


BOOL CListThreadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化


	m_nThreadsCnt = m_hideThreadCount = 0;

	m_threadList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_threadList.InsertColumn(eThreadId, L"Tid", LVCFMT_LEFT, 50);
	m_threadList.InsertColumn(eThreadObject, L"Thread Object", LVCFMT_LEFT, 85);
	m_threadList.InsertColumn(eThreadTeb, L"Teb", LVCFMT_LEFT, 85);
	m_threadList.InsertColumn(eThreadPriority, L"Priority", LVCFMT_LEFT, 85);
	m_threadList.InsertColumn(eThreadStartAddress, L"Start Address", LVCFMT_LEFT, 85);
	m_threadList.InsertColumn(eThreadSwitchTimes, L"Switch Times", LVCFMT_LEFT, 85);
	m_threadList.InsertColumn(eThreadStatus, L"Status", LVCFMT_LEFT, 85);
	m_threadList.InsertColumn(eThreadFlag, L"SuspendCount", LVCFMT_CENTER, 100);
	m_threadList.InsertColumn(eThreadStartModule, L"Start Module", LVCFMT_LEFT, 520);


	m_threadInfo.Format(L"ThreadCount:%d Hide:%d", m_nThreadsCnt, m_hideThreadCount);
	(GetParent()->GetParent())->SetDlgItemText(IDC_PROCESS_TEXT, m_threadInfo);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CListThreadDlg::EnumThreads()
{
	// TODO: 在此处添加实现代码.

	m_threadList.DeleteAllItems();
	m_nThreadsCnt = 0;
	m_vectorThreads.clear();
	m_vectorColor.clear();

	if (!m_nPid || !m_pEprocess)return;

	// 枚举线程
	m_clsThreads.ListThreads((ULONG)m_nPid,(ULONG_PTR)m_pEprocess, m_vectorThreadsAll);


	// 枚举模块
	m_clsModules.EnumModulesByPeb((ULONG)m_nPid, (ULONG_PTR)m_pEprocess, m_vectorModules);

	if (m_vectorThreadsAll.empty())
	{
		return;
	}

	for (vector <THREAD_INFO>::iterator Iter = m_vectorThreadsAll.begin();
		Iter != m_vectorThreadsAll.end();
		Iter++)
	{
		AddThreadItem(*Iter);
	}
	m_threadInfo.Format(L"ThreadCount:%d Hide:%d", m_nThreadsCnt, m_hideThreadCount);
	(GetParent()->GetParent())->SetDlgItemText(IDC_PROCESS_TEXT, m_threadInfo);

	return;
}

void CListThreadDlg::AddThreadItem(THREAD_INFO ThreadInfo)
{
	if (ThreadInfo.State == Terminated)
	{
		return;
	}

	CString szTid, szEthread, szTeb, szPriority, szWin32StartAddress, szContextSwitches, szState, szModule,szSuspendCount;

	szTid.Format(L"%d", ThreadInfo.Tid);
	szEthread.Format(L"0x%016llX", ThreadInfo.pThread);
	if (ThreadInfo.Teb == 0)
	{
		szTeb = L"-";
	}
	else
	{
		szTeb.Format(L"0x%016llX", ThreadInfo.Teb);
	}

	szPriority.Format(L"%d", ThreadInfo.Priority);
	szWin32StartAddress.Format(L"0x%016llX", ThreadInfo.Win32StartAddress);
	szContextSwitches.Format(L"%d", ThreadInfo.ContextSwitches);
	szModule = GetModulePathByThreadStartAddress(ThreadInfo.Win32StartAddress);
	szSuspendCount.Format(L"%d", ThreadInfo.ThreadFlag);
	switch (ThreadInfo.State)
	{
	case Initialized:
		szState = L"Initialized";
		break;

	case Ready:
		szState = L"Ready";
		break;

	case Running:
		szState = L"Running";
		break;

	case Standby:
		szState = L"Standby";
		break;

	case Terminated:
		szState = L"Terminated";
		break;

	case Waiting:
		szState = L"Waiting";
		break;

	case Transition:
		szState = L"Transition";
		break;

	case DeferredReady:
		szState = L"Deferred Ready";
		break;

	case GateWait:
		szState = L"Gate Wait";
		break;

	default:
		szState = L"UnKnown";
		break;
	}
	

	int n = m_threadList.InsertItem(m_threadList.GetItemCount(), szTid);
	m_threadList.SetItemText(n, eThreadObject, szEthread);
	m_threadList.SetItemText(n, eThreadTeb, szTeb);
	m_threadList.SetItemText(n, eThreadPriority, szPriority);
	m_threadList.SetItemText(n, eThreadStartAddress, szWin32StartAddress);
	m_threadList.SetItemText(n, eThreadSwitchTimes, szContextSwitches);
	m_threadList.SetItemText(n, eThreadStatus, szState);
	m_threadList.SetItemText(n, eThreadFlag, szSuspendCount);
	m_threadList.SetItemText(n, eThreadStartModule, szModule);

	ITEM_COLOR clr = enumBlack;
	if (szModule.IsEmpty())
	{
		clr = enumRed;
	}

	m_vectorColor.push_back(clr);
	m_vectorThreads.push_back(ThreadInfo);
	m_threadList.SetItemData(n, m_nThreadsCnt);
	m_nThreadsCnt++;

}


void CListThreadDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码

	if (bShow && nStatus == 0) {

		EnumThreads();
	}


	if (!bShow) {


	}

	return;
}



//
// 根据线程的起始地址找到模块路径
//
CString CListThreadDlg::GetModulePathByThreadStartAddress(ULONG_PTR dwBase)
{
	CString szRet = L"";

	if (dwBase < SYSTEM_ADDRESS_START)
	{
		for (vector <MODULE_INFO_EX>::iterator Iter = m_vectorModules.begin();
			Iter != m_vectorModules.end();
			Iter++)
		{
			MODULE_INFO_EX entry = *Iter;
			if (dwBase >= entry.Base && dwBase <= (entry.Base + entry.Size))
			{
				szRet = entry.Path;

				szRet = szRet.Right(szRet.GetLength() - szRet.ReverseFind('\\') - 1);

				if (entry.Sign == 0x32 && szRet.Find(L".dll") != -1)
				{
					szRet += L"*32";
				}
				else if(entry.Sign == 0x64 && szRet.Find(L".dll") != -1)
				{
					szRet += L"*64";
				}


			}
		}
	}
	else
	{
		if (m_vectorDrivers.empty())
		{
			m_clsDrivers.ListDrivers(m_vectorDrivers);
		}

		szRet = GetDriverPath(dwBase);
	}

	return szRet;
}


// 根据起始地址获取驱动的路径
//
CString CListThreadDlg::GetDriverPath(ULONG_PTR pCallback)
{
	CString szPath = L"";

	for (vector<DRIVER_INFO>::iterator itor = m_vectorDrivers.begin(); itor != m_vectorDrivers.end(); itor++)
	{
		ULONG_PTR nBase = itor->nBase;
		ULONG_PTR nEnd = itor->nBase + itor->nSize;

		if (pCallback >= nBase && pCallback <= nEnd)
		{
			szPath = itor->szDriverPath;

			szPath = szPath.Right(szPath.GetLength() - szPath.ReverseFind('\\')-1);
			
			break;
		}
	}

	return szPath;
}

void CListThreadDlg::OnCustomdrawThreadList(NMHDR *pNMHDR, LRESULT *pResult)
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
		if (nItem != -1)
		{
			COLORREF clrNewTextColor, clrNewBkColor;
			clrNewTextColor = RGB(0, 0, 0);
			clrNewBkColor = RGB(255, 255, 255);

			int nData = (int)m_threadList.GetItemData(nItem);
			ITEM_COLOR clr = m_vectorColor.at(nData);
			if (clr == enumRed)
			{
				clrNewTextColor = RGB(255, 0, 0);
			}

			pLVCD->clrText = clrNewTextColor;
			pLVCD->clrTextBk = clrNewBkColor;
		}

		*pResult = CDRF_DODEFAULT;
	}
}


void CListThreadDlg::OnRclickThreadList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_MEMORY_1, L"刷新");
	menu.AppendMenu(MF_SEPARATOR);

	
	if (m_threadList.m_hWnd)
	{
		int nItem = m_Functions.GetSelectItem(&m_threadList);
		if (-1 == nItem)
		{
			return;
		}

		PTHREAD_INFO pInfo = GetThreadInfoByItem(nItem);
		if (!pInfo || pInfo->Tid == (ULONG)GetCurrentThreadId())return;

		if (pInfo->ThreadFlag > 0) {
			menu.AppendMenu(MF_STRING, ID_RESUME_THREAD, L"恢复线程");
		}
		else {
			menu.AppendMenu(MF_STRING, ID_SUSPEND_THREAD, L"暂停线程");
		}
	}
	menu.AppendMenu(MF_SEPARATOR);

	menu.AppendMenu(MF_STRING, ID_KILL_THREAD, L"结束线程");
	menu.AppendMenu(MF_SEPARATOR);

	menu.AppendMenu(MF_STRING, ID_EXPORT_TEXT, L"导出");
	menu.AppendMenu(MF_SEPARATOR);

	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_REMOTE_CALL, L"远程Call");
	menu.AppendMenu(MF_SEPARATOR);


	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, ID_APC_CALL, L"EIP注入");
	menu.AppendMenu(MF_SEPARATOR);


	CPoint pt;
	GetCursorPos(&pt);
	menu.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	menu.DestroyMenu();


	*pResult = 0;
}


void CListThreadDlg::OnRefreshThread()
{
	// TODO: 在此添加命令处理程序代码
	EnumThreads();
}


void CListThreadDlg::OnSuspendThread()
{
	if (m_threadList.m_hWnd)
	{
		int nItem = m_Functions.GetSelectItem(&m_threadList);
		if (-1 == nItem)
		{
			return;
		}

		PTHREAD_INFO pInfo = GetThreadInfoByItem(nItem);
		if (!pInfo || pInfo->Tid == (ULONG)GetCurrentThreadId())return;

		ULONG ntStaus = STATUS_UNSUCCESSFUL;
		HANDLE hThreadHanle = m_clsThreads.KernelOpenThread(pInfo->Tid);
		if (hThreadHanle)
		{
			ntStaus = m_clsThreads.SuspendThread(hThreadHanle);

			CloseHandle(hThreadHanle);
		}
		CString szTemp = L"";
		szTemp.Format(L"暂停结果:%x", ntStaus);
		AfxMessageBox(szTemp);

	}
}


void CListThreadDlg::OnResumeThread()
{
	if (m_threadList.m_hWnd)
	{
		int nItem = m_Functions.GetSelectItem(&m_threadList);
		if (-1 == nItem)
		{
			return;
		}

		PTHREAD_INFO pInfo = GetThreadInfoByItem(nItem);
		if (!pInfo || pInfo->Tid == (ULONG)GetCurrentThreadId())return;

		ULONG ntStaus = STATUS_UNSUCCESSFUL;
		HANDLE hThreadHanle = m_clsThreads.KernelOpenThread(pInfo->Tid);
		if (hThreadHanle)
		{
			ntStaus = m_clsThreads.ResumeThread(hThreadHanle);
			CloseHandle(hThreadHanle);
		}
		CString szTemp = L"";
		szTemp.Format(L"恢复结果:%x", ntStaus);
		AfxMessageBox(szTemp);

	}
}


void CListThreadDlg::OnKillThread()
{
	if (m_threadList.m_hWnd)
	{
		int nItem = m_Functions.GetSelectItem(&m_threadList);
		if (-1 == nItem)
		{
			return;
		}

		PTHREAD_INFO pInfo = GetThreadInfoByItem(nItem);
		if (!pInfo || pInfo->Tid == (ULONG)GetCurrentThreadId())return;

	
		ULONG ntStaus = STATUS_UNSUCCESSFUL;
		HANDLE hThreadHanle = m_clsThreads.KernelOpenThread(pInfo->Tid);
		if (hThreadHanle)
		{
			ntStaus = m_clsThreads.KillThread(hThreadHanle);
			CloseHandle(hThreadHanle);
		}
		CString szTemp = L"";
		szTemp.Format(L"结束结果:%x", ntStaus);
		AfxMessageBox(szTemp);
	}
}


PTHREAD_INFO CListThreadDlg::GetThreadInfoByItem(int nItem)
{
	PTHREAD_INFO pInfo = NULL;

	if (nItem == -1)
	{
		return pInfo;
	}

	int nData = (int)m_threadList.GetItemData(nItem);
	pInfo = &m_vectorThreads.at(nData);

	return pInfo;
}


void CListThreadDlg::OnExportText()
{
	m_Functions.ExportListToTxt(&m_threadList, L"");
}


void CListThreadDlg::OnRemoteCall()
{
	if (m_threadList.m_hWnd)
	{
		

		CCallDlg callDlg;
		callDlg.m_dwPid = m_nPid;

		callDlg.DoModal();
	}
}


void CListThreadDlg::OnApcCall()
{
	if (m_threadList.m_hWnd)
	{
		int nItem = m_Functions.GetSelectItem(&m_threadList);
		if (-1 == nItem)
		{
			return;
		}

		PTHREAD_INFO pInfo = GetThreadInfoByItem(nItem);
		if (!pInfo || pInfo->Tid == (ULONG)GetCurrentThreadId())return;

		CApcDlg eipDlg;

		eipDlg.m_dwPid = m_nPid;
		eipDlg.m_dwThreadId = (HANDLE)pInfo->Tid;
		eipDlg.DoModal();
	}
}



