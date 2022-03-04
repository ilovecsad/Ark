// CExternDlg.cpp: 实现文件
//

#include "pch.h"
#include "Main.h"
#include "CExternDlg.h"
#include "afxdialogex.h"

BOOL m_bOnce =FALSE;
// CExternDlg 对话框

IMPLEMENT_DYNAMIC(CExternDlg, CDialogEx)

CExternDlg::CExternDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EXTERN_DIALOG, pParent)
{

}

CExternDlg::~CExternDlg()
{
}

void CExternDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_ENABLE, m_cbEnableInject);
	DDX_Control(pDX, IDC_EDIT_PROCESS, m_edProcessName);
	DDX_Control(pDX, IDC_EDIT_DLL, m_edDllPath);
	DDX_Control(pDX, IDC_BUTTON_PROCESS, m_cbSetProcessName);
	DDX_Control(pDX, IDC_BUTTON_DLL, m_cbSetDllPath);
	DDX_Control(pDX, IDC_BUTTON_START, m_cbStartBtn);
	DDX_Control(pDX, IDC_CHECK_HIDE, m_cbMemory);
}


BEGIN_MESSAGE_MAP(CExternDlg, CDialogEx)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	
	ON_BN_CLICKED(IDC_BUTTON_PROCESS, &CExternDlg::OnClickedButtonProcess)
	ON_BN_CLICKED(IDC_BUTTON_DLL, &CExternDlg::OnClickedButtonDll)
	ON_BN_CLICKED(IDC_BUTTON_START, &CExternDlg::OnBnClickedButtonStart)
END_MESSAGE_MAP()


// CExternDlg 消息处理程序


void CExternDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (0x2222)
	{
		if (m_cbEnableInject.GetCheck())
		{
			m_cbSetProcessName.EnableWindow(TRUE);
			m_cbSetDllPath.EnableWindow(TRUE);
			m_cbMemory.EnableWindow(TRUE);
		}
		else {
			m_cbSetProcessName.EnableWindow(FALSE);
			m_cbSetDllPath.EnableWindow(FALSE);
			m_cbMemory.EnableWindow(FALSE);
		}


	}


	CDialogEx::OnTimer(nIDEvent);
}


BOOL CExternDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	
	SetTimer(0x2222, 100, NULL);

	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CExternDlg::OnClose()
{
	
	KillTimer(0x2222);

	CDialogEx::OnClose();
}




void CExternDlg::OnClickedButtonProcess()
{
	
	CFileDialog fileDlg(TRUE, 0, NULL, 0, L"All Files (*.*)|*.*||", 0);
	
	
	if (IDOK == fileDlg.DoModal()) {

		m_szProcessName = fileDlg.GetFileName();
		if (!m_szProcessName.IsEmpty())
		{
			SetDlgItemText(IDC_EDIT_PROCESS, m_szProcessName);
		}
	}

}


void CExternDlg::OnClickedButtonDll()
{

	CFileDialog fileDlg(TRUE, 0, NULL, 0, L"All Files (*.*)|*.*||", 0);

	if (IDOK == fileDlg.DoModal()) {
		m_szDllPath = fileDlg.GetPathName();
		if (!m_szDllPath.IsEmpty())
		{
			SetDlgItemText(IDC_EDIT_DLL, m_szDllPath);
		}
	}
}

typedef struct _INJECT_INFO_
{
	CHAR szProcessName[50];
	WCHAR szDllPath[256];
	ULONG nState;
}INJECT_INFO, *PINJECT_INFO;
void CExternDlg::OnBnClickedButtonStart()
{
	BOOL bRet = FALSE;
	if (m_cbEnableInject.GetCheck())
	{
		if (!m_bOnce) {
			bRet = m_clsExtern.SetLoadImageNotifyRoutine();
			m_bOnce = TRUE;
		}
	}

	if (m_bOnce && bRet)
	{
		PINJECT_INFO pTemp = (PINJECT_INFO)malloc(sizeof(INJECT_INFO) * 2);
		if (!pTemp)return;

		RtlZeroMemory(pTemp, sizeof(INJECT_INFO) * 2);
		::GetDlgItemTextA(this->GetSafeHwnd(), IDC_EDIT_PROCESS, pTemp->szProcessName, 50);
		if (!m_cbMemory.GetCheck())
		{
			::GetDlgItemTextW(this->GetSafeHwnd(), IDC_EDIT_DLL, pTemp->szDllPath, 256 * sizeof(WCHAR));
			pTemp->nState = 0;
		}
		else
		{
			pTemp->nState = 1;
			
			WCHAR szTemp[MAX_PATH] = { 0 };
			CString szPath;
			CString szPath2;
			DWORD dwSize = MAX_PATH;
			if (QueryFullProcessImageNameW(GetCurrentProcess(), 1, szTemp, &dwSize))
			{
				szPath = szTemp;
				szPath = szPath.Left(szPath.ReverseFind('\\') + 1);

				m_edDllPath.GetWindowText(szPath2);
				szPath2 = szPath2.Right(szPath2.GetLength() - szPath2.ReverseFind('\\') - 1);

				szPath += szPath2;

				RtlCopyMemory(pTemp->szDllPath, szPath.GetBuffer(), 256 * sizeof(WCHAR));
				szPath.ReleaseBuffer();
	
			}
		}

		bRet = m_clsExtern.StatrInject(pTemp->szProcessName, pTemp->szDllPath, pTemp->nState);

		if (pTemp)
		{
			free(pTemp);
			pTemp = NULL;
		}
		if (bRet) {
			AfxMessageBox(L"设置成功");
		}

	}

}
