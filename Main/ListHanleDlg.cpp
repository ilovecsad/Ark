// ListHanleDlg.cpp: 实现文件
//

#include "pch.h"
#include "Main.h"
#include "ListHanleDlg.h"
#include "afxdialogex.h"

extern ULONG_PTR g_ThreadEndFlag;
extern ULONG_PTR g_ThreadCanStart;
// CListHanleDlg 对话框

IMPLEMENT_DYNAMIC(CListHanleDlg, CDialogEx)

CListHanleDlg::CListHanleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HANDLE_DIALOG, pParent)
{
	m_dwPid = 0;
}

CListHanleDlg::~CListHanleDlg()
{
}

void CListHanleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HANLE_LIST, m_list);
}


BEGIN_MESSAGE_MAP(CListHanleDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_RCLICK, IDC_HANLE_LIST, &CListHanleDlg::OnRclickHanleList)
	ON_COMMAND(ID_HANDLEDLG_REFRESH, &CListHanleDlg::OnHandledlgRefresh)
	ON_COMMAND(ID_HANDLEDLG_SHOW, &CListHanleDlg::OnHandledlgShow)
	ON_MESSAGE(WM_ADD_ITEM_HANDLE_DLG, &CListHanleDlg::OnAddItem)

END_MESSAGE_MAP()


// CListHanleDlg 消息处理程序
typedef enum _HANDLE_HEADER_INDEX
{
	eHandleType,
	eHandleName,
	eHandleValue,
	eHandleGranted,
	eHandleObject,
	eHandleRefCount,
	
}HANDLE_HEADER_INDEX;

typedef enum _ADD_TYPE_
{
	eShow_No_NameHanle = 1,
	eAllHanle,

}ADD_TYPE;


BOOL CListHanleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list.InsertColumn(eHandleType, L"Type", LVCFMT_LEFT, 90);
	m_list.InsertColumn(eHandleName, L"Name", LVCFMT_LEFT, 300);
	m_list.InsertColumn(eHandleValue, L"Hanle", LVCFMT_CENTER, 90);
	m_list.InsertColumn(eHandleGranted, L"Granted", LVCFMT_LEFT, 600);
	m_list.InsertColumn(eHandleObject, L"HandleObject", LVCFMT_LEFT, 90);
	m_list.InsertColumn(eHandleRefCount, L"Count", LVCFMT_CENTER, 70);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CListHanleDlg::EnumProcessHandles()
{
	m_list.DeleteAllItems();
	m_vectorSaveAllHandle.clear();
	m_HandleNoNameVector.clear();
	m_nCnt = 0;
	m_nHaveNameCnt = 0;
	m_bStart = FALSE;

	m_clsHandles.GetHandles((ULONG)m_dwPid,m_vectorSaveAllHandle);
	if (m_vectorSaveAllHandle.empty())
	{
		return;
	}

	for (vector <HANDLE_INFO>::iterator Iter = m_vectorSaveAllHandle.begin(); Iter != m_vectorSaveAllHandle.end(); Iter++)
	{

		SendMessage(WM_ADD_ITEM_HANDLE_DLG, eAllHanle, m_nCnt);
		m_nCnt++;
	}
	m_bStart = TRUE;

	
}

VOID CListHanleDlg::AddHandleItem(HANDLE_INFO HandleInfo)
{
	
	CString szHandle, szObject, szReferenceCount,szObjectPath,szPid,szGrantAcess;

	szHandle.Format(L"0x%04x", HandleInfo.Handle);
	szObject.Format(L"0x%016llx", HandleInfo.Object);
	szReferenceCount.Format(L"%d", HandleInfo.ReferenceCount);
	szGrantAcess.Format(L"0x%x", HandleInfo.grantAcess);

	if (HandleInfo.grantAcess) {
		if (wcscmp(L"Thread", HandleInfo.ObjectName) == 0)
		{
			szGrantAcess += L"  (" + AnalysisThreadContext(HandleInfo.grantAcess);
			szGrantAcess = szGrantAcess.Left(szGrantAcess.GetLength() - 2);
			szGrantAcess += L")";
		}
		if (wcscmp(L"Process", HandleInfo.ObjectName) == 0)
		{
			szGrantAcess += L"  (" + AnalysisProcessContext(HandleInfo.grantAcess);
			szGrantAcess = szGrantAcess.Left(szGrantAcess.GetLength() - 2);
			szGrantAcess += L")";
		}
		if (wcscmp(L"Section", HandleInfo.ObjectName) == 0)
		{
			szGrantAcess += L"  (" + AnalysisSectionContext(HandleInfo.grantAcess);
			szGrantAcess = szGrantAcess.Left(szGrantAcess.GetLength() - 2);
			szGrantAcess += L")";
		}
		if (wcscmp(L"Token", HandleInfo.ObjectName) == 0)
		{
			szGrantAcess += L"  (" + AnalysisTokenContext(HandleInfo.grantAcess);
			szGrantAcess = szGrantAcess.Left(szGrantAcess.GetLength() - 2);
			szGrantAcess += L")";
		}
		if (wcscmp(L"File", HandleInfo.ObjectName) == 0)
		{
			szGrantAcess += L"  (" + AnalysisFileContext(HandleInfo.grantAcess);
			szGrantAcess = szGrantAcess.Left(szGrantAcess.GetLength() - 2);
			szGrantAcess += L")";
		}
		if (wcscmp(L"Key", HandleInfo.ObjectName) == 0)
		{
			szGrantAcess += L"  (" + AnalysisKeyContext(HandleInfo.grantAcess);
			szGrantAcess = szGrantAcess.Left(szGrantAcess.GetLength() - 2);
			szGrantAcess += L")";
		}
	}

	HandleInfo.Id ? szPid.Format(L"--->ID:%d", HandleInfo.Id) : szPid = L" ";

	szObjectPath = HandleInfo.u.HandleName + szPid;

	if (wcscmp(L"TmRm", HandleInfo.ObjectName) == 0 || wcscmp(L"TmTm", HandleInfo.ObjectName) == 0)
	{
		//存在GUID
		if (szObjectPath.ReverseFind('\\') == -1)
		{
			szObjectPath = L" ";
			GUID guid = HandleInfo.u.dwGuid;
			szObjectPath.Format(_T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"), guid.Data1, guid.Data2, guid.Data3
				, guid.Data4[0], guid.Data4[1]
				, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
				, guid.Data4[6], guid.Data4[7]);
		}
	}
	if (wcscmp(L"Token", HandleInfo.ObjectName) == 0)
	{
		szObjectPath.Format(L"%s\\%s:0x%x%s", HandleInfo.u.tokenInfo.AuthorityName,
			HandleInfo.u.tokenInfo.AccountName, HandleInfo.u.tokenInfo.TokenId,
			HandleInfo.u.tokenInfo.TokenType);
	}



	int n = m_list.InsertItem(m_list.GetItemCount(), HandleInfo.ObjectName);
	m_list.SetItemText(n, eHandleObject, szObject);
	m_list.SetItemText(n, eHandleValue, szHandle);
	m_list.SetItemText(n, eHandleRefCount, szReferenceCount);
	m_list.SetItemText(n, eHandleName, szObjectPath);
	m_list.SetItemText(n, eHandleGranted, szGrantAcess);

	m_list.SetItemData(n, n);
}

void CListHanleDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	
	if (bShow && nStatus == 0) 
	{
		EnumProcessHandles();

		m_handleInfo.Format(L"句柄总数:%d :有名字的句柄数量:%d", m_nCnt, m_nHaveNameCnt);
		(GetParent()->GetParent())->SetDlgItemText(IDC_PROCESS_TEXT, m_handleInfo);
	}


	if (!bShow) {


	}
}




void CListHanleDlg::OnRclickHanleList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);


	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_HANDLEDLG_REFRESH, L"刷新");
	menu.AppendMenu(MF_SEPARATOR);

	menu.AppendMenu(MF_STRING, ID_HANDLEDLG_SHOW, L"显示所有句柄");
	menu.AppendMenu(MF_SEPARATOR);




	CPoint pt;
	//ClientToScreen(&pt);
	GetCursorPos(&pt);
	menu.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	menu.DestroyMenu();



	*pResult = 0;
}



CString CListHanleDlg::AnalysisProcessContext(ULONG_PTR a)
{
	CString szRet = L" ";
	if (PROCESS_ALL_ACCESS == a)
	{
		szRet = L"full control||";
		return szRet;
	}

	if (a & PROCESS_TERMINATE)
	{
		szRet += L"TERMINATE||";
	}
	if (a & PROCESS_CREATE_THREAD)
	{
		szRet += L"CREATE_THREAD||";
	}
	if (a & PROCESS_SET_SESSIONID)
	{
		szRet += L"SET_SESSIONID||";
	}
	if (a & PROCESS_VM_OPERATION)
	{
		szRet += L"VM_OPERATION||";
	}
	if (a & PROCESS_VM_READ)
	{
		szRet += L"VM_READ||";
	}
	if (a & PROCESS_VM_WRITE)
	{
		szRet += L"VM_WRITE||";
	}
	if (a & PROCESS_DUP_HANDLE)
	{
		szRet += L"DUP_HANDLE||";
	}
	if (a & PROCESS_CREATE_PROCESS)
	{
		szRet += L"CREATE_PROCESS||";
	}
	if (a & PROCESS_SET_QUOTA)
	{
		szRet += L"SET_QUOTA||";
	}

	if (a & PROCESS_SET_INFORMATION)
	{
		szRet += L"SET_INFORMATION||";
	}
	if (a & PROCESS_QUERY_INFORMATION)
	{
		szRet += L"QUERY_INFORMATION||";
	}
	if (a & PROCESS_SUSPEND_RESUME)
	{
		szRet += L"SUSPEND_RESUME||";
	}
	if (a & PROCESS_QUERY_LIMITED_INFORMATION)
	{
		szRet += L"QUERY_LIMITED_INFORMATION||";
	}
	if (a & PROCESS_SET_LIMITED_INFORMATION)
	{
		szRet += L"SET_LIMITED_INFORMATION||";
	}
	szRet.MakeLower();

	return szRet;
}


CString CListHanleDlg::AnalysisThreadContext(ULONG_PTR b)
{
	CString szRet = L" ";
	if (THREAD_ALL_ACCESS == b)
	{
		szRet = L"full control||";
		return szRet;
	}

	if (b& THREAD_TERMINATE)
	{
		szRet += L"TERMINATE||";
	}
	if (b&THREAD_SUSPEND_RESUME)
	{
		szRet += L"SUSPEND_RESUME||";
	}
	if (b&THREAD_GET_CONTEXT)
	{
		szRet += L"GET_CONTEXT||";
	}
	if (b&THREAD_SET_CONTEXT)
	{
		szRet += L"SET_CONTEXT||";
	}
	if (b&THREAD_QUERY_INFORMATION)
	{
		szRet += L"QUERY_INFORMATION||";
	}
	if (b&THREAD_SET_INFORMATION)
	{
		szRet += L"SET_INFORMATION||";
	}
	if (b&THREAD_SET_THREAD_TOKEN)
	{
		szRet += L"SET_THREAD_TOKEN||";
	}
	if (b&THREAD_IMPERSONATE) {
		szRet += L"IMPERSONATE||";

	}
	if (b&THREAD_DIRECT_IMPERSONATION) {
		szRet += L"DIRECT_IMPERSONATION||";

	}
	if (b&THREAD_SET_LIMITED_INFORMATION) {
		szRet += L"SET_LIMITED_INFORMATION||";
	}
	if (b&THREAD_QUERY_LIMITED_INFORMATION)
	{
		szRet += L"QUERY_LIMITED_INFORMATION||";
	}
	if (b&THREAD_RESUME) {
		szRet += L"RESUME||";
	}
	szRet.MakeLower();
	return szRet;
}


CString CListHanleDlg::AnalysisSectionContext(ULONG_PTR b)
{
	
	CString szRet = L" ";
	if (SECTION_ALL_ACCESS == b)
	{
		szRet = L"full control||";
		return szRet;
	}

	if (b & SECTION_QUERY)
	{
		szRet += L"QUERY||";
	}
	if (b & SECTION_MAP_WRITE)
	{
		szRet += L"WRITE||";
	}

	if (b & SECTION_MAP_READ)
	{
		szRet += L"READ||";
	}
	if (b & SECTION_MAP_EXECUTE)
	{
		szRet += L"EXECUTE||";
	}
	if (b & SECTION_EXTEND_SIZE)
	{
		szRet += L"EXTEND_SIZE||";
	}
	if (b&SECTION_MAP_EXECUTE_EXPLICIT)
	{
		szRet += L"EXECUTE_EXPLICIT||";
	}
	szRet.MakeLower();
	return szRet;
}

CString CListHanleDlg::AnalysisTokenContext(ULONG_PTR b)
{
	CString szRet = L" ";
	if (TOKEN_ALL_ACCESS == b)
	{
		szRet = L"full control||";
		return szRet;
	}

	if (b & TOKEN_ASSIGN_PRIMARY)
	{
		szRet += L"ASSIGN_PRIMARY||";
	}
	if (b & TOKEN_DUPLICATE)
	{
		szRet += L"DUPLICATE||";
	}

	if (b & TOKEN_IMPERSONATE)
	{
		szRet += L"IMPERSONATE||";
	}
	if (b & TOKEN_QUERY)
	{
		szRet += L"QUERY||";
	}
	if (b & TOKEN_QUERY_SOURCE)
	{
		szRet += L"QUERY_SOURCE||";
	}
	if (b&TOKEN_ADJUST_PRIVILEGES)
	{
		szRet += L"ADJUST_PRIVILEGES||";
	}

	if (b & TOKEN_ADJUST_GROUPS)
	{
		szRet += L"ADJUST_GROUPS||";
	}
	if (b & TOKEN_ADJUST_DEFAULT)
	{
		szRet += L"ADJUST_DEFAULT||";
	}
	if (b&TOKEN_ADJUST_SESSIONID)
	{
		szRet += L"ADJUST_SESSIONID||";
	}

	szRet.MakeLower();
	return szRet;
}
CString CListHanleDlg::AnalysisFileContext(ULONG_PTR b)
{
	CString szRet = L" ";
	if (FILE_ALL_ACCESS == b)
	{
		szRet = L"full control";
		return szRet;
	}

	if (b& FILE_GENERIC_READ)
	{
		szRet += L"READ||";
	}
	if (b&FILE_GENERIC_WRITE)
	{
		szRet += L"WRITE||";
	}
	if (b&FILE_GENERIC_EXECUTE)
	{
		szRet += L"EXECUTE||";
	}




	szRet.MakeLower();
	return szRet;
}

CString CListHanleDlg::AnalysisKeyContext(ULONG_PTR b)
{
	CString szRet = L" ";
	if (KEY_ALL_ACCESS == b)
	{
		szRet = L"full control";
		return szRet;
	}

	if (b& KEY_QUERY_VALUE)
	{
		szRet += L"QUERY_VALUE||";
	}
	if (b&KEY_SET_VALUE)
	{
		szRet += L"SET_VALUE||";
	}
	if (b&KEY_READ)
	{
		szRet += L"READ||";
	}

	if (b&KEY_WRITE)
	{
		szRet += L"WRITE||";
	}
	if (b&KEY_EXECUTE)
	{
		szRet += L"EXECUTE||";
	}


	szRet.MakeLower();
	return szRet;
}




void CListHanleDlg::OnHandledlgRefresh()
{
	EnumProcessHandles();

	/*

	太难 控制不玩了
	if (m_vectorSaveAllHandle.size() == m_nCnt && g_ThreadCanStart &DLG_HANDLE_THREAD_START)
	{
		InterlockedAnd64((LONG64*)&g_ThreadCanStart, ~DLG_HANDLE_THREAD_START);
		AfxBeginThread(threadWork, this);

	}
	*/
}


void CListHanleDlg::OnHandledlgShow()
{
	// TODO: 在此添加命令处理程序代码
	if (m_bStart) {
		SendMessage(WM_ADD_ITEM_HANDLE_DLG, eShow_No_NameHanle, 0);
	};
}

LRESULT CListHanleDlg::OnAddItem(WPARAM wParam, LPARAM lParam)
{
	if (eShow_No_NameHanle == wParam) {
		if (m_HandleNoNameVector.empty()) {
			return TRUE;
		}

		for (vector <HANDLE_INFO>::iterator Iter = m_HandleNoNameVector.begin(); Iter != m_HandleNoNameVector.end(); Iter++)
		{
			AddHandleItem(*Iter);
		}
		m_HandleNoNameVector.clear();
		m_bStart = FALSE;
		return TRUE;
	}


	if (eAllHanle == wParam)
	{
		CString szTemp;
		szTemp.Empty();
		szTemp = m_vectorSaveAllHandle.at((int)lParam).u.HandleName;
		if (!szTemp.IsEmpty())
		{
			AddHandleItem(m_vectorSaveAllHandle.at((int)lParam));
			m_nHaveNameCnt++;
		}
		else {
			m_HandleNoNameVector.push_back(m_vectorSaveAllHandle.at((int)lParam));
		}
	
	}

	return TRUE;
}


