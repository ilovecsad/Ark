// ListWindowDlg.cpp: 实现文件
//

#include "pch.h"
#include "Main.h"
#include "ListWindowDlg.h"
#include "afxdialogex.h"


BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
BOOL CALLBACK EnumChildWindowsProc(HWND hwnd, LPARAM lParam);
// CListWindowDlg 对话框

IMPLEMENT_DYNAMIC(CListWindowDlg, CDialogEx)

CListWindowDlg::CListWindowDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HWND_DIALOG, pParent)
{

}

CListWindowDlg::~CListWindowDlg()
{
}

void CListWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_WND_LIST, m_list);
}


BEGIN_MESSAGE_MAP(CListWindowDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_RESFRESH, &CListWindowDlg::OnResfresh)
	ON_NOTIFY(NM_RCLICK, IDC_WND_LIST, &CListWindowDlg::OnRclickWndList)
END_MESSAGE_MAP()


// CListWindowDlg 消息处理程序


BOOL CListWindowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list.InsertColumn(eWndHand, L"窗口句柄", LVCFMT_LEFT, 130);
	m_list.InsertColumn(eWndTitle, L"窗口标题", LVCFMT_LEFT, 260);
	m_list.InsertColumn(eWndClass, L"窗口类名", LVCFMT_LEFT, 260);
	m_list.InsertColumn(eWndEnable, L"窗口可见性", LVCFMT_CENTER, 80);
	m_list.InsertColumn(eWndThread, L"所属线程ID", LVCFMT_LEFT, 300);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

VOID CListWindowDlg::ListAllWindows(ULONG dwPid)
{
	m_vecWndInfo.clear();
	m_list.DeleteAllItems();
	PWINDOWS_INFO_ALL pBuffer = NULL;

	ULONG nSize = sizeof(WINDOWS_INFO_ALL) + sizeof(WINDOWS_INFO) * 200;
	pBuffer = (PWINDOWS_INFO_ALL)malloc(nSize);
	if (!pBuffer)return;
	RtlZeroMemory(pBuffer, nSize);
	pBuffer->dwProcessPid = m_dwPid;

	EnumWindows(EnumWindowsProc,(LPARAM)pBuffer);

	for (ULONG i = 0; i < pBuffer->nCnt; i++)
	{
		m_vecWndInfo.push_back(pBuffer->pWndInfo[i]);
	}


	for (vector <WINDOWS_INFO>::iterator Iter = m_vecWndInfo.begin(); Iter != m_vecWndInfo.end(); Iter++)
	{
		addItem(*Iter);
	}

}

BOOL CALLBACK EnumChildWindowsProc(HWND hwnd, LPARAM lParam)
{
	PWINDOWS_INFO_ALL pBuffer = (PWINDOWS_INFO_ALL)lParam;
	if (!pBuffer)return FALSE;
	if (pBuffer->nCnt < 200)
	{
		pBuffer->pWndInfo[pBuffer->nCnt].dwWndHand = hwnd;
		pBuffer->pWndInfo[pBuffer->nCnt].dwWndThread = GetWindowThreadProcessId(hwnd, NULL);
		pBuffer->pWndInfo[pBuffer->nCnt].bMain = FALSE;
		::GetWindowText(hwnd, pBuffer->pWndInfo[pBuffer->nCnt].szWndTitle, 100);
		::GetClassName(hwnd, pBuffer->pWndInfo[pBuffer->nCnt].szWndClass, 50);
		pBuffer->pWndInfo[pBuffer->nCnt].dwWndEnable = ::IsWindowVisible(hwnd);
		pBuffer->nCnt++;
	}


	return true;
}




BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	PWINDOWS_INFO_ALL pBuffer = (PWINDOWS_INFO_ALL)lParam;
	if (!pBuffer)return FALSE;

	DWORD dwPid = 0;
	GetWindowThreadProcessId(hwnd, &dwPid);
	if (dwPid == (DWORD)pBuffer->dwProcessPid)
	{
		if (pBuffer->nCnt < 200)
		{
			pBuffer->pWndInfo[pBuffer->nCnt].dwWndHand = hwnd;
			pBuffer->pWndInfo[pBuffer->nCnt].dwWndThread = GetWindowThreadProcessId(hwnd, &dwPid);
			pBuffer->pWndInfo[pBuffer->nCnt].bMain = TRUE;
			::GetWindowText(hwnd, pBuffer->pWndInfo[pBuffer->nCnt].szWndTitle, 100);
			::GetClassName(hwnd, pBuffer->pWndInfo[pBuffer->nCnt].szWndClass, 50);
			pBuffer->pWndInfo[pBuffer->nCnt].dwWndEnable = ::IsWindowVisible(hwnd);
			pBuffer->nCnt++;
		}


		EnumChildWindows(hwnd, EnumChildWindowsProc, (LPARAM)pBuffer);
	}
	return true;
}

void CListWindowDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	ListAllWindows(m_dwPid);
}

VOID CListWindowDlg::addItem(WINDOWS_INFO dwItem)
{
	
	CString szMain = L"主窗口：";
	CString szMain2 = L"子窗口：";
	CString szWnd,szWndEnable,szWndThread;

	szWnd.Format(L"0x%04x", dwItem.dwWndHand);
	szWndEnable.Format(L"%x", dwItem.dwWndEnable);
	szWndThread.Format(L"%d", dwItem.dwWndThread);

	int n = m_list.InsertItem(m_list.GetItemCount(), szWnd);


	if (dwItem.bMain)
	{
		szMain += dwItem.szWndTitle;
		m_list.SetItemText(n, eWndTitle, szMain);
	}
	else {
		szMain2 += dwItem.szWndTitle;
		m_list.SetItemText(n, eWndTitle, szMain2);
	}
	
	m_list.SetItemText(n, eWndClass, dwItem.szWndClass);
	m_list.SetItemText(n, eWndEnable, szWndEnable);
	m_list.SetItemText(n, eWndThread, szWndThread);

	m_list.SetItemData(n, n);
}



void CListWindowDlg::OnResfresh()
{
	ListAllWindows(m_dwPid);
}


void CListWindowDlg::OnRclickWndList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_RESFRESH, L"刷新");
	menu.AppendMenu(MF_SEPARATOR);

	CPoint pt;
	GetCursorPos(&pt);
	menu.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	menu.DestroyMenu();

	*pResult = 0;
}
