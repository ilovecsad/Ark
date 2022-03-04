// CDriverDlg.cpp: 实现文件
//

#include "pch.h"
#include "Main.h"
#include "DriverDlg.h"
#include "afxdialogex.h"


// CDriverDlg 对话框


IMPLEMENT_DYNAMIC(CListDriverDlg, CDialogEx)

CListDriverDlg::CListDriverDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DRIVER_DIALOG, pParent)
{
	m_driverBuffer = NULL;
}

CListDriverDlg::~CListDriverDlg()
{
}

void CListDriverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DRIVER_LIST, m_driverList);
}


BEGIN_MESSAGE_MAP(CListDriverDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_DRIVER_LIST, &CListDriverDlg::OnLvnItemchangedDriverList)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_DRIVER_LIST, &CListDriverDlg::OnCustomdrawDriverList)
	ON_NOTIFY(NM_RCLICK, IDC_DRIVER_LIST, &CListDriverDlg::OnRclickDriverList)
	ON_COMMAND(ID_PROCESSDLG_REFRESH, &CListDriverDlg::OnProcessdlgRefresh)
	ON_COMMAND(ID_UNLOAD_DRIVER, &CListDriverDlg::OnUnloadDriver)
END_MESSAGE_MAP()


// CDriverDlg 消息处理程序


BOOL CListDriverDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_driverList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_driverList.InsertColumn(eDriverName, L"Driver Name", LVCFMT_LEFT, 120);
	m_driverList.InsertColumn(eDriverBase, L"Base", LVCFMT_LEFT, 100);
	m_driverList.InsertColumn(eDriverSize, L"Size", LVCFMT_LEFT, 100);
	m_driverList.InsertColumn(eDriverObject, L"Driver Object", LVCFMT_LEFT, 100);
	m_driverList.InsertColumn(eDriverPath, L"Path", LVCFMT_LEFT, 270);
	m_driverList.InsertColumn(eDriverServiceName, L"Server Name", LVCFMT_LEFT, 110);
	m_driverList.InsertColumn(eDriverStartupType, L"Startup Type", LVCFMT_CENTER, 80);
	m_driverList.InsertColumn(eDriverLoadOrder, L"Load Order", LVCFMT_CENTER, 70);
	m_driverList.InsertColumn(eDriverFileCorporation, L"File Corporation", LVCFMT_LEFT, 140);
	m_driverList.InsertColumn(eDriverFileDescription, L"Description", LVCFMT_LEFT, 200);
	m_driverInfo.Format(L"Drivers: %d, Hidden Drivers: %d, Suspicious PE Image: %d", 0, 0, 0);

	AfxGetMainWnd()->SetDlgItemText(IDC_SHOW_TEXT, m_driverInfo);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CListDriverDlg::ListDrivers()
{
	// TODO: 在此处添加实现代码.
	m_driverInfo.Format(L"Drivers: %d, Hidden Drivers: %d, Suspicious PE Image: %d", 0, 0, 0);
		// 清零
	m_vectorDrivers.clear();
	m_driverList.DeleteAllItems();
	m_vectorColor.clear();
	m_vectorRing3Drivers.clear();
	m_vectorServiceKeys.clear();

	// 内核枚举
	m_clsDriver.ListDrivers(m_vectorDrivers);
	if (m_vectorDrivers.empty())
	{
		return;
	}

	// 插入驱动项
	InsertDriverIntem();

	AfxGetMainWnd()->SetDlgItemText(IDC_SHOW_TEXT, m_driverInfo);

}


void CListDriverDlg::InsertDriverIntem()
{
	
	Ring3GetDriverModules();

	// 枚举services键值,获得系统中全部服务信息
	GetServiceKeys();


	for (vector<DRIVER_INFO>::iterator itor = m_vectorDrivers.begin();
		itor != m_vectorDrivers.end();
		itor++)
	{
		AddItem(*itor);
	}
	m_driverInfo.Format(L"Drivers: %d, Hidden Drivers: %d, Suspicious PE Image: %d", m_driverList.GetItemCount(), nHideDriverCnt, 0);

	AfxGetMainWnd()->SetDlgItemText(IDC_SHOW_TEXT, m_driverInfo);
}

VOID CListDriverDlg::AddItem(DRIVER_INFO itor)
{
	ITEM_COLOR item_clr = enumBlack;

	CString szDesci;
	CString szBase, szSize, szDriverObject, szServiceName, szLoadOrder, szComp, szStartup = L"-";
	CString szDriverPath = itor.szDriverPath;
	CString szDriverName = szDriverPath.Right(szDriverPath.GetLength() - szDriverPath.ReverseFind('\\') - 1);
	szBase.Format(L"0x%llX", itor.nBase);
	szSize.Format(L"0x%08X", itor.nSize);
	szLoadOrder.Format(L"%d", itor.nLodeOrder);
	szServiceName = GetServiceName(szDriverPath);
	szStartup = GetDriverStartupType(szServiceName);
	szDesci = m_Functions.GetFileDescription(szDriverPath);
	if (PathFileExists(szDriverPath))
	{
		szComp = m_Funcionts.GetFileCompanyName(szDriverPath);
	}
	else
	{
		szComp = L"File not found";
	}


	if (itor.nDriverObject)
	{
		szDriverObject.Format(L"0x%llX", itor.nDriverObject);
	}
	else
	{
		szDriverObject = L"-";
	}

	if (itor.DriverType == enumHaveOrder)
	{
		// 是否是隐藏模块
		if (IsHideDriver(itor.nBase))
		{
			nHideDriverCnt++;
			item_clr = enumRed;
		}

		// 如果不是隐藏的模块, 那么判断是否是微软模块
		if (item_clr == enumBlack && !m_Funcionts.IsMicrosoftApp(szComp))
		{
			item_clr = enumBlue;
		}
	}
	else if (itor.DriverType == enumHide)
	{
		szLoadOrder = L"-";
		nHideDriverCnt++;
		item_clr = enumRed;
	}
	int nCnt = m_driverList.GetItemCount();
	int i = m_driverList.InsertItem(nCnt, szDriverName);
	m_driverList.SetItemText(i, eDriverBase, szBase);
	m_driverList.SetItemText(i, eDriverSize, szSize);
	m_driverList.SetItemText(i, eDriverObject, szDriverObject);
	m_driverList.SetItemText(i, eDriverPath, szDriverPath);
	m_driverList.SetItemText(i, eDriverServiceName, szServiceName);
	m_driverList.SetItemText(i, eDriverStartupType, szStartup);
	m_driverList.SetItemText(i, eDriverLoadOrder, szLoadOrder);
	m_driverList.SetItemText(i, eDriverFileCorporation, szComp);
	m_driverList.SetItemText(i, eDriverFileDescription, szDesci);
	
	m_vectorColor.push_back(item_clr);
	

	m_driverList.SetItemData(i, nCnt);
}

//
// 与Ring3枚举的结果对比, 判断驱动模块是否是隐藏的
//
BOOL CListDriverDlg::IsHideDriver(ULONG_PTR Base)
{
	BOOL bHide = TRUE;

	for (vector<ULONG_PTR>::iterator itor = m_vectorRing3Drivers.begin();
		itor != m_vectorRing3Drivers.end();
		itor++)
	{
		if (*itor == Base)
		{
			bHide = FALSE;
			break;
		}
	}

	return bHide;
}


CString CListDriverDlg::GetServiceName(CString szPath)
{
	CString szServiceName = L"";

	if (!szPath.IsEmpty())
	{
		for (list<SERVICE_KEY>::iterator itor = m_vectorServiceKeys.begin(); itor != m_vectorServiceKeys.end(); itor++)
		{
			if (!szPath.CompareNoCase(itor->szPath))
			{
				szServiceName = itor->szKey;
				break;
			}
		}
	}

	return szServiceName;
}
//
// 获取驱动文件的启动类型
//
CString CListDriverDlg::GetDriverStartupType(CString szService)
{
	CString szType = L"-";

	if (!szService.IsEmpty())
	{
		DWORD dwType = 0;
		if (GetStartType(szService, &dwType))
		{
			switch (dwType)
			{
			case SERVICE_AUTO_START:
				szType = L"Automatic";
				break;

			case SERVICE_BOOT_START:
				szType = L"Boot Start";
				break;

			case SERVICE_DEMAND_START:
				szType = L"Manual";
				break;

			case SERVICE_DISABLED:
				szType = L"Disabled";
				break;

			case SERVICE_SYSTEM_START:
				szType = L"System Start";
				break;
			}
		}
	}

	return szType;
}



//
// 获取驱动文件的启动类型
//
BOOL CListDriverDlg::GetStartType(CString szKey, DWORD *dwType)
{
	BOOL bRet = FALSE;

	if (szKey.IsEmpty() || dwType == NULL)
	{
		return bRet;
	}

	szKey = REG_SERVICE + CString(L"\\") + szKey;
	WCHAR szStart[] = { 'S','t','a','r','t','\0' };

	list<VALUE_INFO> valueList;
	m_Registry.EnumValues(szKey, valueList);
	for (list<VALUE_INFO>::iterator ir = valueList.begin();
		ir != valueList.end();
		ir++)
	{
		if (ir->nNameLen == wcslen(szStart) * sizeof(WCHAR) &&
			!ir->szName.CompareNoCase(szStart))
		{
			*dwType = *(PULONG)ir->pData;
			bRet = TRUE;
			break;
		}
	}

	m_Registry.FreeValueList(valueList);
	return bRet;
}


//
// 获取services键值的全部内容
//
void CListDriverDlg::GetServiceKeys()
{
	// TODO: 在此处添加实现代码.

	m_vectorServiceKeys.clear();

	list<KEY_INFO> keyList;
	


	m_Registry.EnumKeys(SERVICE_KEY_NAME, keyList);


	for (list<KEY_INFO>::iterator ir = keyList.begin();
		ir != keyList.end();
		ir++)
	{
		if (ir->nNameLen > 0)
		{
			CString szKey = SERVICE_KEY_NAME + CString(L"\\") + ir->szName;
			CString szImagePathValue = GetServiceKeyImagePathValue(szKey);
			if (!szImagePathValue.IsEmpty())
			{
				SERVICE_KEY sk;
				sk.szKey = ir->szName;
				sk.szPath = szImagePathValue;
				m_vectorServiceKeys.push_back(sk);
			}
		}
	}
	
}

//
// 根据服务键值,获得其ImagePath
//
CString CListDriverDlg::GetServiceKeyImagePathValue(CString szKey)
{
	CString szPath = L"";

	if (szKey.IsEmpty())
	{
		return szPath;
	}

	WCHAR szImagePath[] = { 'I', 'm', 'a', 'g', 'e', 'P', 'a', 't', 'h', '\0' };

	list<VALUE_INFO> valueList;
	m_Registry.EnumValues(szKey, valueList);

	for (list<VALUE_INFO>::iterator ir = valueList.begin();
		ir != valueList.end();
		ir++)
	{
		if (ir->nNameLen == wcslen(szImagePath) * sizeof(WCHAR) &&
			!ir->szName.CompareNoCase(szImagePath))
		{
			szPath = m_Funcionts.RegParsePath((WCHAR*)ir->pData, ir->Type);
			break;
		}
	}

	m_Registry.FreeValueList(valueList);

	return szPath;
}

void CListDriverDlg::Ring3GetDriverModules()
{
	m_vectorRing3Drivers.clear();

	WCHAR szNtdll[] = { 'n', 't', 'd', 'l', 'l', '.', 'd', 'l', 'l', '\0' };
	HINSTANCE ntdll_dll = GetModuleHandle(szNtdll);

	if (ntdll_dll == NULL)
	{
		return;
	}

	pfnZwQuerySystemInformation ZwQuerySystemInformation = NULL;
	CHAR szZwQuerySystemInformation[] =
	{ 'Z', 'w', 'Q', 'u', 'e', 'r', 'y', 'S', 'y', 's', 't', 'e', 'm', 'I', 'n', 'f', 'o', 'r', 'm', 'a', 't', 'i', 'o', 'n', '\0' };

	ZwQuerySystemInformation = (pfnZwQuerySystemInformation)GetProcAddress(ntdll_dll, "ZwQuerySystemInformation");
	if (ZwQuerySystemInformation != NULL)
	{
		DWORD dwNeedSize = 0;
		NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation, NULL, 0, &dwNeedSize);

		if (status == STATUS_INFO_LENGTH_MISMATCH)
		{
			PBYTE pBuffer = new BYTE[dwNeedSize];

			if (pBuffer)
			{
				RtlZeroMemory(pBuffer, dwNeedSize);
				status = ZwQuerySystemInformation(SystemModuleInformation, pBuffer, dwNeedSize, &dwNeedSize);
				if (status == STATUS_SUCCESS)
				{
					ULONG_PTR count = *((ULONG_PTR*)pBuffer);
					PSYSTEM_MODULE_INFORMATION pmi = (PSYSTEM_MODULE_INFORMATION)(pBuffer + sizeof(ULONG_PTR));
					for (ULONG_PTR i = 0; i < count; i++)
					{
						m_vectorRing3Drivers.push_back((ULONG_PTR)(pmi->ImageBase));
						pmi++;
					}
				}

				delete[]pBuffer;
				pBuffer = NULL;
			}
		}
	}
}






void CListDriverDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	// TODO: 在此处添加消息处理程序代码

	if (bShow && nStatus == 0) {

		
		ListDrivers();
	}


}


void CListDriverDlg::OnLvnItemchangedDriverList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}





void CListDriverDlg::OnCustomdrawDriverList(NMHDR *pNMHDR, LRESULT *pResult)
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
		int nData = (int)m_driverList.GetItemData(nItem);
		ITEM_COLOR clr = m_vectorColor.at(nData);

		clrNewTextColor = RGB( 0, 0, 0 );
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


void CListDriverDlg::OnRclickDriverList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_PROCESSDLG_REFRESH, L"刷新");
	menu.AppendMenu(MF_SEPARATOR);


	menu.AppendMenu(MF_STRING, ID_UNLOAD_DRIVER, L"卸载驱动(蓝屏危险)");
	menu.AppendMenu(MF_SEPARATOR);

	CPoint pt;
	GetCursorPos(&pt);
	menu.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	menu.DestroyMenu();
	*pResult = 0;
}


void CListDriverDlg::OnProcessdlgRefresh()
{
	// TODO: 在此添加命令处理程序代码
	ListDrivers();
}


void CListDriverDlg::OnUnloadDriver()
{
	
	int n = m_Funcionts.GetSelectItem(&m_driverList);
	if (n == -1)return;

	ULONG_PTR data = m_driverList.GetItemData(n);

	PDRIVER_INFO pInfo = NULL;

	pInfo = &m_vectorDrivers.at(data);

	if (pInfo)
	{

		if (m_clsDriver.UnLoadDriver(pInfo->nDriverObject))
		{
			m_driverList.DeleteItem(n);
		}

	}

}
