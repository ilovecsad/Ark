// CListTokenDlg.cpp: 实现文件
//

#include "pch.h"
#include "Main.h"
#include "CListTokenDlg.h"
#include "afxdialogex.h"


// CListTokenDlg 对话框

IMPLEMENT_DYNAMIC(CListTokenDlg, CDialogEx)

CListTokenDlg::CListTokenDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TOKEN_DIALOG, pParent)
{
	m_dwPid = 0;
}

CListTokenDlg::~CListTokenDlg()
{
}

void CListTokenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TOKEN_LIST, m_list);
}

const WCHAR* szName[]{
TEXT("SeCreateTokenPrivilege"),
 TEXT("SeAssignPrimaryTokenPrivilege"),
 TEXT("SeLockMemoryPrivilege"),
 TEXT("SeIncreaseQuotaPrivilege"),
 TEXT("SeUnsolicitedInputPrivilege"),
 TEXT("SeMachineAccountPrivilege"),
 TEXT("SeTcbPrivilege"),
 TEXT("SeSecurityPrivilege"),
 TEXT("SeTakeOwnershipPrivilege"),
 TEXT("SeLoadDriverPrivilege"),
 TEXT("SeSystemProfilePrivilege"),
 TEXT("SeSystemtimePrivilege"),
 TEXT("SeProfileSingleProcessPrivilege"),
 TEXT("SeIncreaseBasePriorityPrivilege"),
 TEXT("SeCreatePagefilePrivilege"),
 TEXT("SeCreatePermanentPrivilege"),
 TEXT("SeBackupPrivilege"),
 TEXT("SeRestorePrivilege"),
 TEXT("SeShutdownPrivilege"),
 TEXT("SeDebugPrivilege"),
 TEXT("SeAuditPrivilege"),
 TEXT("SeSystemEnvironmentPrivilege"),
 TEXT("SeChangeNotifyPrivilege"),
 TEXT("SeRemoteShutdownPrivilege"),
 TEXT("SeUndockPrivilege"),
 TEXT("SeSyncAgentPrivilege"),
 TEXT("SeEnableDelegationPrivilege"),
 TEXT("SeManageVolumePrivilege"),
 TEXT("SeImpersonatePrivilege"),
 TEXT("SeCreateGlobalPrivilege"),
 TEXT("SeTrustedCredManAccessPrivilege"),
 TEXT("SeRelabelPrivilege"),
 TEXT("SeIncreaseWorkingSetPrivilege"),
 TEXT("SeTimeZonePrivilege"),
 TEXT("SeCreateSymbolicLinkPrivilege"),
};



ULONG_PTR token_Index[]{
  (ULONG_PTR)1<<1,
  (ULONG_PTR)1<<2,
  (ULONG_PTR)1<<3,
  (ULONG_PTR)1<<4,
  (ULONG_PTR)1<<5,
  (ULONG_PTR)1<<6,
  (ULONG_PTR)1<<7,
  (ULONG_PTR)1<<8,
  (ULONG_PTR)1<<9,
  (ULONG_PTR)1<<10,
  (ULONG_PTR)1<<11,
  (ULONG_PTR)1<<12,
  (ULONG_PTR)1<<13,
  (ULONG_PTR)1<<14,
  (ULONG_PTR)1<<15,
  (ULONG_PTR)1<<16,
  (ULONG_PTR)1<<17,
  (ULONG_PTR)1<<18,
  (ULONG_PTR)1<<19,
  (ULONG_PTR)1<<20,
  (ULONG_PTR)1<<21,
  (ULONG_PTR)1<<22,
  (ULONG_PTR)1<<23,     
  (ULONG_PTR)1<<24,
  (ULONG_PTR)1<<25,
  (ULONG_PTR)1<<26,
  (ULONG_PTR)1<<27,
  (ULONG_PTR)1<<28,
  (ULONG_PTR)1<<29,
  (ULONG_PTR)1<<30,
  (ULONG_PTR)1<<31,
  (ULONG_PTR)1<<32,
  (ULONG_PTR)1<<33,
  (ULONG_PTR)1<<34,
  (ULONG_PTR)1<<35,
};


BEGIN_MESSAGE_MAP(CListTokenDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_TOKEN_LIST, &CListTokenDlg::OnCustomdrawTokenList)
END_MESSAGE_MAP()


// CListTokenDlg 消息处理程序


BOOL CListTokenDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, L"Name", LVCFMT_CENTER, 300);
	m_list.InsertColumn(1, L"Status", LVCFMT_CENTER, 150);
	m_list.InsertColumn(2, L"Description", LVCFMT_LEFT, 300);

	InitFunc();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CListTokenDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow && nStatus == 0) 
	{
		m_dwToken = 0x4800;

		ShowProcessToken(m_dwToken);
	}


	if (!bShow) {


	}
}

//ITEM_COLOR clrItem;
void CListTokenDlg::InitFunc()
{
	m_list.DeleteAllItems();
	m_vectorToken.clear();
	m_dwToken = 0;
	for (int i = 0; i < 35; i++)
	{
		int n=  m_list.InsertItem(m_list.GetItemCount(), szName[i]);
		m_list.SetItemText(i, 1, L"Disbale");
		TOKEN_EXEX info = { 0 };
		info.dwIndex = token_Index[i];
		m_vectorToken.push_back(info);

		m_list.SetItemData(n, n);
	}

}




void CListTokenDlg::OnCustomdrawTokenList(NMHDR *pNMHDR, LRESULT *pResult)
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
			int nData = (int)m_list.GetItemData(nItem);
			ITEM_COLOR clrText = m_vectorToken.at(nData).clrItem;


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


void CListTokenDlg::ShowProcessToken(ULONG_PTR dwToken)
{
	int i = 0;

	for (vector <TOKEN_EXEX>::iterator Iter = m_vectorToken.begin(); Iter != m_vectorToken.end(); Iter++)
	{
		if (Iter->dwIndex & dwToken)
		{
			m_list.SetItemText(i, 1, L"Enbale");
			Iter->clrItem = enumBlue;
		}
		i++;
	}
}
