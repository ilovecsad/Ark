// ChangeProtectionDlg.cpp: 实现文件
//

#include "pch.h"
#include "Main.h"
#include "ChangeProtectionDlg.h"
#include "afxdialogex.h"


// CChangeProtectionDlg 对话框

IMPLEMENT_DYNAMIC(CChangeProtectionDlg, CDialogEx)

CChangeProtectionDlg::CChangeProtectionDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHANGE_MEMORY_DIALOG, pParent)
{
	m_pMemoryInfo = NULL;
	m_dwPid = 0;
}

CChangeProtectionDlg::~CChangeProtectionDlg()
{
}

void CChangeProtectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROTECTION_COMBO, m_cbProtection);

}


BEGIN_MESSAGE_MAP(CChangeProtectionDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_PROTECTION_COMBO, &CChangeProtectionDlg::OnSelchangeProtectionCombo)
	ON_BN_CLICKED(IDOK, &CChangeProtectionDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CChangeProtectionDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CChangeProtectionDlg 消息处理程序

/*
#define PAGE_NOACCESS           0x01
#define PAGE_READONLY           0x02
#define PAGE_READWRITE          0x04
#define PAGE_WRITECOPY          0x08

#define PAGE_EXECUTE            0x10
#define PAGE_EXECUTE_READ       0x20
#define PAGE_EXECUTE_READWRITE  0x40

#define PAGE_EXECUTE_WRITECOPY  0x80

#define PAGE_GUARD             0x100
#define PAGE_NOCACHE           0x200

#define PAGE_WRITECOMBINE      0x400

#define PAGE_ENCLAVE_THREAD_CONTROL 0x80000000
#define PAGE_REVERT_TO_FILE_MAP     0x80000000
#define PAGE_TARGETS_NO_UPDATE      0x40000000
#define PAGE_TARGETS_INVALID        0x40000000
#define PAGE_ENCLAVE_UNVALIDATED    0x20000000
#define PAGE_ENCLAVE_DECOMMIT       0x10000000

*/
BOOL CChangeProtectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	m_cbProtection.ResetContent();
	// TODO:  在此添加额外的初始化
	//m_cbProtection.AddString(L"PAGE_NOACCESS");
	m_cbProtection.InsertString(0, L"PAGE_NOACCESS");
	m_seletProtection.push_back(PAGE_NOACCESS);

	//m_cbProtection.AddString(L"PAGE_READONLY");
	m_cbProtection.InsertString(1, L"PAGE_READONLY");
	m_seletProtection.push_back(PAGE_READONLY);

	//m_cbProtection.AddString(L"PAGE_READWRITE");
	m_cbProtection.InsertString(2, L"PAGE_READWRITE");
	m_seletProtection.push_back(PAGE_READWRITE);

	//m_cbProtection.AddString(L"PAGE_WRITECOPY");
	m_cbProtection.InsertString(3,L"PAGE_WRITECOPY");
	m_seletProtection.push_back(PAGE_WRITECOPY);

	//m_cbProtection.AddString(L"PAGE_EXECUTE");
	m_cbProtection.InsertString(4, L"PAGE_EXECUTE");
	m_seletProtection.push_back(PAGE_EXECUTE);

	//m_cbProtection.AddString(L"PAGE_EXECUTE_READ");
	m_cbProtection.InsertString(5, L"PAGE_EXECUTE_READ");
	m_seletProtection.push_back(PAGE_EXECUTE_READ);

	//m_cbProtection.AddString(L"PAGE_EXECUTE_READWRITE");
	m_cbProtection.InsertString(6, L"PAGE_EXECUTE_READWRITE");
	m_seletProtection.push_back(PAGE_EXECUTE_READWRITE);

	//m_cbProtection.AddString(L"PAGE_EXECUTE_WRITECOPY");
	m_cbProtection.InsertString(7, L"PAGE_EXECUTE_WRITECOPY");
	m_seletProtection.push_back(PAGE_EXECUTE_WRITECOPY);

	//m_cbProtection.AddString(L"PAGE_GUARD");
	m_cbProtection.InsertString(8, L"PAGE_GUARD");
	m_seletProtection.push_back(PAGE_GUARD);

	//m_cbProtection.AddString(L"PAGE_NOCACHE");
	m_cbProtection.InsertString(9, L"PAGE_NOCACHE");
	m_seletProtection.push_back(PAGE_NOCACHE);

	//m_cbProtection.AddString(L"PAGE_WRITECOMBINE");
	m_cbProtection.InsertString(10, L"PAGE_WRITECOMBINE");
	m_seletProtection.push_back(PAGE_WRITECOMBINE);


	return FALSE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CChangeProtectionDlg::OnSelchangeProtectionCombo()
{
	GetDlgItem(IDOK)->EnableWindow(TRUE);

}


void CChangeProtectionDlg::OnBnClickedOk()
{
	int n = m_cbProtection.GetCurSel();
	CString szTemp;
	ULONG_PTR nNewProtection = m_seletProtection.at(n);
	ULONG nRet = m_clsMemory.ChangeMemoryProtection((HANDLE)m_dwPid, m_pMemoryInfo->Base, (ULONG)nNewProtection,m_pMemoryInfo->Size);

	szTemp.Format(L"返回的结果: 0x%X", nRet);
	SetWindowText(szTemp);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
}


void CChangeProtectionDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}
