// CListVadDlg.cpp: 实现文件
//

#include "pch.h"
#include "Main.h"
#include "CListVadDlg.h"
#include "afxdialogex.h"
#include "ChangeVadProtectionDlg.h"

// CListVadDlg 对话框

IMPLEMENT_DYNAMIC(CListVadDlg, CDialogEx)

CListVadDlg::CListVadDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VAD_DIALOG, pParent)
{

}

CListVadDlg::~CListVadDlg()
{
}

void CListVadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VAD_LIST, m_listVad);
}


BEGIN_MESSAGE_MAP(CListVadDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_RCLICK, IDC_VAD_LIST, &CListVadDlg::OnRclickVadList)
	ON_COMMAND(ID_PROCESSDLG_REFRESH, &CListVadDlg::OnProcessdlgRefresh)
	ON_COMMAND(ID_CHANGE_VAD_PROTECTION, &CListVadDlg::OnChangeVadProtection)
END_MESSAGE_MAP()


// CListVadDlg 消息处理程序


BOOL CListVadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	
	m_listVad.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_listVad.InsertColumn(eVAD, L"Vad", LVCFMT_LEFT, 100);
	m_listVad.InsertColumn(eStart, L"Start", LVCFMT_LEFT, 125);
	m_listVad.InsertColumn(eEnd, L"End", LVCFMT_LEFT, 125);
	m_listVad.InsertColumn(eType, L"Type", LVCFMT_LEFT, 70);
	m_listVad.InsertColumn(eProtect, L"Protection", LVCFMT_LEFT, 150);
	m_listVad.InsertColumn(eSection, L"Section", LVCFMT_LEFT, 500);




	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}



void CListVadDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow && nStatus == 0) {

		EnumVadInfo();
	}


	if (!bShow) {


	}

	return;
}

void CListVadDlg::EnumVadInfo()
{
	// TODO: 在此处添加实现代码.
	m_listVad.DeleteAllItems();
	m_vectorVad.clear();
	m_nCnt = 0;
	m_clsVad.EnumProcessVad(m_dwPid, m_vectorVad);

	if (m_vectorVad.empty())return;

	for (vector <VAD_INFO>::iterator Iter = m_vectorVad.begin();
		Iter != m_vectorVad.end();
		Iter++)
	{
		AddVadInfoItem(*Iter);
		
	}
	CString szTemp;
	szTemp.Format(L"总数:%d", m_nCnt);

	(GetParent()->GetParent())->SetDlgItemText(IDC_PROCESS_TEXT, szTemp);
}

void CListVadDlg::AddVadInfoItem(VAD_INFO item)
{
	
	CString szVadAddress,szStart,szEnd,szMemoryProtection,szType,szFileName,szTemp;

	szMemoryProtection = MemoryProtection(item.flags);
	szTemp = MemoryProtectionEx(item.flags);
	szMemoryProtection += szTemp;

	szType = MemoryType(item.flags,item.pFileObject);

	if (item.pFileObject) {
		szFileName = m_clsVad.ObQueryNameString(item.pFileObject);
		
	}
	else {
		szFileName = L" ";
	}

	szVadAddress.Format(L"0x%016llx", item.pVad);
	szStart.Format(L"0x%016llx", item.startVpn);
	szEnd.Format(L"0x%016llx", item.endVpn);


	int n = m_listVad.InsertItem(m_listVad.GetItemCount(), szVadAddress);
	m_listVad.SetItemText(n, eStart, szStart);
	m_listVad.SetItemText(n, eEnd, szEnd);
	m_listVad.SetItemText(n, eProtect, szMemoryProtection);
	m_listVad.SetItemText(n, eType, szType);
	m_listVad.SetItemText(n, eSection, szFileName);

	m_listVad.SetItemData(n, n);
	m_nCnt++;
}

CString CListVadDlg::MemoryProtection(ULONG_PTR flags)
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

CString CListVadDlg::MemoryProtectionEx(ULONG_PTR flags)
{
	CString szTemp = L" ";
	MM_PRIVATE_VAD_FLAGS flag = { 0 };
	RtlCopyMemory(&flag, &flags, sizeof(flag));

	if (flag.Enclave && flag.NoChange) {
		szTemp = L"-Enclave-NoChange";
	}
	else if(flag.Enclave && flag.NoChange == 0)
	{
		szTemp = L"-Enclave";
	}
	else if (flag.Enclave==0 && flag.NoChange)
	{
		szTemp = L"-NoChange";
	}


	return szTemp;
}

CString CListVadDlg::MemoryType(ULONG_PTR flags,ULONG_PTR a)
{
	CString szTemp = L" ";

	MMVAD_FLAGS flag = { 0 };

	RtlCopyMemory(&flag, &flags, sizeof(flag));

	if (flag.PrivateMemory)
	{
		szTemp = L"Private";
	}
	else if(!flag.PrivateMemory)
	{
		szTemp = L"Mapped";
	}


	return szTemp;
}


void CListVadDlg::OnRclickVadList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_PROCESSDLG_REFRESH, L"刷新");
	menu.AppendMenu(MF_SEPARATOR);


	menu.AppendMenu(MF_STRING, ID_CHANGE_VAD_PROTECTION, L"更改VAD属性(有蓝屏危险)");
	menu.AppendMenu(MF_SEPARATOR);

	CPoint pt;
	GetCursorPos(&pt);
	menu.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	menu.DestroyMenu();
	*pResult = 0;
}


void CListVadDlg::OnProcessdlgRefresh()
{
	// TODO: 在此添加命令处理程序代码
	EnumVadInfo();
}


void CListVadDlg::OnChangeVadProtection()
{
	if (MessageBox(L"对于系统DLL不要乱改(如ntdll,kernle32),除非你真的理解-windows内存管理机制", L"注意",MB_OKCANCEL) == IDOK)
	{
		
		if (m_listVad.m_hWnd)
		{
			int nItem = m_Functions.GetSelectItem(&m_listVad);
			if (-1 == nItem)
			{
				return;
			}

			ULONG dwPid = 0;
			PVAD_INFO pInfo = GetVadInfoByItem(nItem);
			if (!pInfo)return;
			dwPid = m_dwPid;


			CChangeVadProtectionDlg m_changeVadProtectionDlg;
			m_changeVadProtectionDlg.m_dwPid = m_dwPid;
			m_changeVadProtectionDlg.m_oldVadFlags = pInfo->flags;
			m_changeVadProtectionDlg.m_dwBaseAddress = pInfo->startVpn;

			m_changeVadProtectionDlg.DoModal();
		}
		
	}
}


PVAD_INFO CListVadDlg::GetVadInfoByItem(int nItem)
{
	PVAD_INFO pInfo = NULL;

	if (nItem == -1)
	{
		return pInfo;
	}

	int nData = (int)m_listVad.GetItemData(nItem);
	pInfo = &m_vectorVad.at(nData);

	return pInfo;
}
