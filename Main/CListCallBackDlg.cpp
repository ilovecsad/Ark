// CListCallBackDlg.cpp: 实现文件
//

#include "pch.h"
#include "Main.h"
#include "CListCallBackDlg.h"
#include "afxdialogex.h"


// CListCallBackDlg 对话框

IMPLEMENT_DYNAMIC(CListCallBackDlg, CDialogEx)

CListCallBackDlg::CListCallBackDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CALLBACK_DIALOG, pParent)
{

}

CListCallBackDlg::~CListCallBackDlg()
{
}

void CListCallBackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CALLBACK_LIST, m_list);
}


BEGIN_MESSAGE_MAP(CListCallBackDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_CALLBACK_LIST, &CListCallBackDlg::OnCustomdrawCallbackList)
END_MESSAGE_MAP()


// CListCallBackDlg 消息处理程序


BOOL CListCallBackDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, L"回调入口", LVCFMT_LEFT, 130);
	m_list.InsertColumn(1, L"类型", LVCFMT_LEFT, 130);
	m_list.InsertColumn(2, L"当前函数所在模块", LVCFMT_LEFT, 300);



	return FALSE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CListCallBackDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow && nStatus == 0) 
	{
		AfxGetMainWnd()->SetDlgItemText(IDC_SHOW_TEXT, L"");
		showAllReusult();

	}

	if (!bShow) {


	}
}

void CListCallBackDlg::showAllReusult()
{
	m_list.DeleteAllItems();
	GetData();


	if (!m_data.empty())
	{
		ITEM_COLOR item_clr = enumBlack;
		CString szFun, szType, szComp;
		for (vector<CALLBACK_DATA>::iterator itor = m_data.begin();
			itor != m_data.end();
			itor++)
		{
			szFun.Empty();
			szType.Empty();

			if (itor->Type == eObRegister) {
				szFun.Format(L"0x%llX", itor->info.PreCall);
			}
			else {
				szFun.Format(L"0x%llX", itor->funcAddress);
			}

			switch (itor->Type)
			{
			case eCreateProcess:
				szType = L"CreateProcess";
				break;

			case eCreateThread:
				szType = L"CreateThread";
				break;

			case eCmCallback:
				szType = L"CmpCallBack";
				break;

			case eLoadImage:
				szType = L"LoadImage";
				break;
			case eObRegister:

				if (itor->info.bType) {
					szType = L"ObRegister-PsProcess";
				}
				else {
					szType = L"ObRegister-PsThread";
				}
				
				break;

			default:
				break;
			}

			if (PathFileExists(itor->szPath))
			{
				szComp = m_Funcionts.GetFileCompanyName(itor->szPath);
			}
			else
			{
				szComp = L"File not found";
			}

			if (!m_Funcionts.IsMicrosoftApp(szComp))
			{
				item_clr = enumBlue;			// 其他的非微软模块;
			}


			int i = m_list.InsertItem(m_list.GetItemCount(), szFun);
			m_list.SetItemText(i, 1, szType);
			m_list.SetItemText(i, 2, itor->szPath);
			m_list.SetItemData(i, i);
			m_vectorColor.push_back(item_clr);
		
		}

	}

}

VOID CListCallBackDlg::GetData()
{
	m_data.clear();
	
	PCALL_BACK pTemp = NULL;

	CALLBACK_DATA info = { 0 };

	if (m_clsFunc.EnumAllCallBack(&pTemp))
	{


		for (int i = 0; i < 64; i++)
		{
			if (pTemp->PspCreateProcessNotifyRoutine[i] != 0)
			{
				info = { 0 };
				info.funcAddress = pTemp->PspCreateProcessNotifyRoutine[i];
				info.Type = eCreateProcess;
				info.szPath = GetPathByAddress(info.funcAddress);
				m_data.push_back(info);
			}
		}


		for (int i = 0; i < 64; i++)
		{
			if (pTemp->PspCreateThreadNotifyRoutine[i] != 0)
			{
				info = { 0 };
				info.funcAddress = pTemp->PspCreateThreadNotifyRoutine[i];
				info.Type = eCreateThread;
				info.szPath = GetPathByAddress(info.funcAddress);
				m_data.push_back(info);
			}
		}



		for (int i = 0; i < 64; i++)
		{
			if (pTemp->CmCallbackListHead[i] != 0)
			{

				info = { 0 };
				info.funcAddress = pTemp->CmCallbackListHead[i];
				info.Type = eCmCallback;
				info.szPath = GetPathByAddress(info.funcAddress);
				m_data.push_back(info);
			}
		}

		for (int i = 0; i < 64; i++)
		{
			if (pTemp->PspLoadImageNotifyRoutine[i] != 0)
			{

				info = { 0 };
				info.funcAddress = pTemp->PspLoadImageNotifyRoutine[i];
				info.Type = eLoadImage;
				info.szPath = GetPathByAddress(info.funcAddress);
				m_data.push_back(info);
			}
		}

		for (int i = 0; i < 64; i++)
		{
			if (pTemp->obRegister[i].ObjTypeAddr != 0)
			{
				info = { 0 };

				info.info = pTemp->obRegister[i];
				info.Type = eObRegister;
				info.szPath = GetPathByAddress(info.info.PreCall);
				m_data.push_back(info);
			}
		}



		if (pTemp) {
			free(pTemp);
			pTemp = NULL;
		}

	}
}

CString CListCallBackDlg::GetPathByAddress(ULONG64 dwAddress)
{
	m_vectorDrivers.clear();
	// 内核枚举
	m_clsDriver.ListDrivers(m_vectorDrivers);
	if (m_vectorDrivers.empty())
	{
		return L"";
	}

	for (vector<DRIVER_INFO>::iterator itor = m_vectorDrivers.begin();
		itor != m_vectorDrivers.end();
		itor++)
	{
		
		if ((itor->nBase < dwAddress) && ((itor->nBase + itor->nSize) > dwAddress))
		{
			return itor->szDriverPath;
		}

	}


	return L"";
}


void CListCallBackDlg::OnCustomdrawCallbackList(NMHDR *pNMHDR, LRESULT *pResult)
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
		int nData = (int)m_list.GetItemData(nItem);
		ITEM_COLOR clr = m_vectorColor.at(nData);

		clrNewTextColor = RGB(0, 0, 0);
		clrNewBkColor = RGB(255, 255, 255);

		if (clr == enumBlue)
		{
			clrNewTextColor = RGB(0, 0, 255);
		}
		else if (clr == enumRed)
		{
			clrNewTextColor = RGB(255, 0, 0);
		}
		else if (clr == enumPurple)
		{
			clrNewTextColor = RGB(180, 0, 255);
		}

		pLVCD->clrText = clrNewTextColor;
		pLVCD->clrTextBk = clrNewBkColor;

		*pResult = CDRF_DODEFAULT;
	}
}
