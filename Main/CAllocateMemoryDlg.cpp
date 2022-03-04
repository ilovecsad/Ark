// CAllocateMemoryDlg.cpp: 实现文件
//

#include "pch.h"
#include "Main.h"
#include "CAllocateMemoryDlg.h"
#include "afxdialogex.h"


// CAllocateMemoryDlg 对话框

IMPLEMENT_DYNAMIC(CAllocateMemoryDlg, CDialogEx)

CAllocateMemoryDlg::CAllocateMemoryDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CAllocateMemoryDlg::~CAllocateMemoryDlg()
{
}

void CAllocateMemoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ADDRESS, m_edAddress);
	DDX_Control(pDX, IDC_EDIT_SIZE, m_edSize);
	DDX_Control(pDX, IDC_COMBO_PROTECTION, m_cbProtection);
}


BEGIN_MESSAGE_MAP(CAllocateMemoryDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAllocateMemoryDlg::OnBnClickedOk)
END_MESSAGE_MAP()





//处理 编辑框 只能输入16进制
BOOL CAllocateMemoryDlg::PreTranslateMessage(MSG* pMsg)
{
	if ((WM_CHAR == pMsg->message) && (::GetDlgCtrlID(pMsg->hwnd) == IDC_EDIT_ADDRESS)) {
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

BOOL CAllocateMemoryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();



	m_cbProtection.ResetContent();
	m_cbProtection.InsertString(0, _TEXT("MEM_RESERVE"));
	m_cbProtection.SetItemData(0, MEM_RESERVE);

	m_cbProtection.InsertString(1, _TEXT("MEM_COMMIT"));
	m_cbProtection.SetItemData(1, MEM_COMMIT);

	m_cbProtection.SetCurSel(1);
	m_edAddress.SetWindowText(_TEXT("0"));
	m_edSize.SetWindowText(_TEXT("1000"));


	return FALSE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CAllocateMemoryDlg::OnBnClickedOk()
{
	ULONG_PTR ntStatus = 0;
	ULONG nSize = 0;
	ULONG_PTR dwBaseAddress = 0;
	PVOID pRetAddress = NULL;
	CString szBuff = L"";
	m_edSize.GetWindowText(szBuff);
	nSize = _tcstoul(szBuff, 0, 16);
	if (nSize > 0xffffffff) {
		MessageBox(L"申请空间太大", L"提示");
		return;
	}

	if (nSize < 0x1000) {
		nSize = 0x1000;
	}
	m_edAddress.GetWindowText(szBuff);
	dwBaseAddress = _tcstoull(szBuff, 0, 16);

	int dwType = m_cbProtection.GetItemData(m_cbProtection.GetCurSel());
	
	ntStatus = m_clsMemory.AllocateVirtualMemory(m_dwPid, dwBaseAddress, nSize, dwType);

	szBuff.Empty();

	szBuff.Format(L"返回的结果:0x%llX ",ntStatus);

	SetWindowText(szBuff);
	
}


