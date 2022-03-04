// ChangeVadProtectionDlg.cpp: 实现文件
//

#include "pch.h"
#include "Main.h"
#include "ChangeVadProtectionDlg.h"
#include "afxdialogex.h"


// CChangeVadProtectionDlg 对话框

IMPLEMENT_DYNAMIC(CChangeVadProtectionDlg, CDialogEx)

CChangeVadProtectionDlg::CChangeVadProtectionDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VAD_PROTECTION_DLG, pParent)
{
	m_dwPid = 0;
	m_oldVadFlags = { 0 };
	m_dwBaseAddress = 0;
}

CChangeVadProtectionDlg::~CChangeVadProtectionDlg()
{
}

void CChangeVadProtectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_VAD, m_cbView);
	DDX_Control(pDX, IDC_CHECK_CLEAR1, m_clearNoChange);
	DDX_Control(pDX, IDC_CHECK_ENCLAVE, m_clearEnclave);
}


BEGIN_MESSAGE_MAP(CChangeVadProtectionDlg, CDialogEx)
	ON_BN_CLICKED(IDOK_OK, &CChangeVadProtectionDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_CB_VAD, &CChangeVadProtectionDlg::OnSelendcancelCbVad)
	ON_BN_CLICKED(ID_QUXIAO, &CChangeVadProtectionDlg::OnBnClickedQuxiao)
END_MESSAGE_MAP()


// CChangeVadProtectionDlg 消息处理程序



BOOL CChangeVadProtectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDOK_OK)->EnableWindow(FALSE);
	m_cbView.ResetContent();

	m_cbView.InsertString(0, L"MM_READONLY(1)");
	m_cbView.SetItemData(0, MM_READONLY);

	m_cbView.InsertString(1, L"MM_EXECUTE_READ(3)");
	m_cbView.SetItemData(1, MM_EXECUTE_READ);

	m_cbView.InsertString(2, L"MM_READWRITE(4)");
	m_cbView.SetItemData(2, MM_READWRITE);

	m_cbView.InsertString(3, L"MM_WRITECOPY(5)");
	m_cbView.SetItemData(3, MM_WRITECOPY);

	m_cbView.InsertString(4, L"MM_EXECUTE_READWRITE(6)");
	m_cbView.SetItemData(4, MM_EXECUTE_READWRITE);

	m_cbView.InsertString(5, L"MM_EXECUTE_WRITECOPY(7)");
	m_cbView.SetItemData(5, MM_EXECUTE_WRITECOPY);

	m_cbView.InsertString(6, L"MM_EXECUTE(2)");
	m_cbView.SetItemData(6, MM_EXECUTE);

	m_cbView.SetCurSel(4);
	GetDlgItem(IDOK_OK)->EnableWindow(TRUE);

	return FALSE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CChangeVadProtectionDlg::OnSelendcancelCbVad()
{
	GetDlgItem(IDOK_OK)->EnableWindow(TRUE);
}

void CChangeVadProtectionDlg::OnBnClickedOk()
{

	
	int n = m_cbView.GetCurSel();

	ULONG dwType = m_cbView.GetItemData(n);

	MM_PRIVATE_VAD_FLAGS pTemp = { 0 };

	RtlCopyMemory(&pTemp, &m_oldVadFlags, sizeof(MM_PRIVATE_VAD_FLAGS));
	
	pTemp.Protection = dwType;
	if (m_clearNoChange.GetCheck())
	{
		pTemp.NoChange = 0;
	}
	if (m_clearEnclave.GetCheck())
	{
		pTemp.Enclave = 0;
	}

	ULONG_PTR nResult = 0;
	RtlCopyMemory(&nResult, &pTemp, sizeof(MM_PRIVATE_VAD_FLAGS));
	
	if (m_clsVad.changeVadProtect(m_dwPid, m_dwBaseAddress, nResult))
	{
		AfxMessageBox(L"更改VAD属性成功");
	}
	else
	{
		AfxMessageBox(L"更改VAD属性失败");
	}

	GetDlgItem(IDOK_OK)->EnableWindow(FALSE);
	
}



void CChangeVadProtectionDlg::OnBnClickedQuxiao()
{
	CDialogEx::OnCancel();
}
