// CallDlg.cpp: 实现文件
//

#include "pch.h"
#include "Main.h"
#include "CallDlg.h"
#include "afxdialogex.h"


// CCallDlg 对话框

IMPLEMENT_DYNAMIC(CCallDlg, CDialogEx)

CCallDlg::CCallDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CALL_DIALOG, pParent)
{

}

CCallDlg::~CCallDlg()
{
}

void CCallDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CALL, m_edCall);
	DDX_Control(pDX, IDC_EDIT_ARG, m_edArg);
}


BEGIN_MESSAGE_MAP(CCallDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CCallDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCallDlg 消息处理程序


BOOL CCallDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_edArg.SetWindowText(L"0");

	CString szTemp = L"";
	szTemp.Format(L"%d", m_dwPid);
	SetWindowText(szTemp);

	return FALSE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}




BOOL CCallDlg::PreTranslateMessage(MSG* pMsg)
{
	if ((WM_CHAR == pMsg->message) && ((::GetDlgCtrlID(pMsg->hwnd) == IDC_EDIT_CALL))) {
		if (pMsg->wParam == VK_BACK)//相应back键
			return CDialog::PreTranslateMessage(pMsg);
		TCHAR ch = (TCHAR)pMsg->wParam;
		if ((ch >= '0' && ch <= '9'))
			return CDialog::PreTranslateMessage(pMsg);
		if ((ch >= 'A' && ch <= 'F'))
			return CDialog::PreTranslateMessage(pMsg);
		if ((ch >= 'a' && ch <= 'f'))
			return CDialog::PreTranslateMessage(pMsg);
		return TRUE;
	}
	if ((WM_CHAR == pMsg->message) && (::GetDlgCtrlID(pMsg->hwnd) == IDC_EDIT_ARG)) {
		if (pMsg->wParam == VK_BACK)//相应back键
			return CDialog::PreTranslateMessage(pMsg);
		TCHAR ch = (TCHAR)pMsg->wParam;
		if ((ch >= '0' && ch <= '9'))
			return CDialog::PreTranslateMessage(pMsg);
		if ((ch >= 'A' && ch <= 'F'))
			return CDialog::PreTranslateMessage(pMsg);
		if ((ch >= 'a' && ch <= 'f'))
			return CDialog::PreTranslateMessage(pMsg);
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CCallDlg::OnBnClickedOk()
{

	CString szTemp= L"";
	m_edCall.GetWindowText(szTemp);
	ULONG64 pCall64 = (ULONG64)_tcstoull(szTemp, 0, 16);
	HANDLE dwPid = m_dwPid;

	if (!pCall64 || !m_dwPid) {

		AfxMessageBox(L"call地址不能为空");
		return;
	}

	m_edArg.GetWindowText(szTemp);
	ULONG64 pArg= (ULONG64)_tcstoull(szTemp, 0, 16);

	ULONG ntStatus = m_clsThread.hzw_CreateRemoteThread(dwPid, (PVOID64)pCall64, (PVOID64)pArg);

	szTemp.Empty();
	szTemp.Format(L"执行结果返回:0x%x", ntStatus);
	SetWindowText(szTemp);

	return;
}
