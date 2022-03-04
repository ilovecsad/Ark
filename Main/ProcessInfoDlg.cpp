// ProcessInfoDlg.cpp: 实现文件
//

#include "pch.h"
#include "Main.h"
#include "ProcessInfoDlg.h"
#include "afxdialogex.h"


// CProcessInfoDlg 对话框

IMPLEMENT_DYNAMIC(CProcessInfoDlg, CDialogEx)

CProcessInfoDlg::CProcessInfoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROCESS_INFO_DIALOG, pParent)
{

	m_nPid = 0;
	m_pEprocess = 0;
	m_szImageName = L"";
}

CProcessInfoDlg::~CProcessInfoDlg()
{
}

void CProcessInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROCESS_INFO_TAB, m_tab);
}


BEGIN_MESSAGE_MAP(CProcessInfoDlg, CDialogEx)

END_MESSAGE_MAP()






BOOL CProcessInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 初始化窗口标题（进程名 | pid | pEprocess）
	CString szImage;
	szImage.Format(L"%s ( %s | PID:%d | 0x%llX )", L"Process Information", m_szImageName, m_nPid, m_pEprocess);
	SetWindowText(szImage);

	//设置 threadDlg 所需要的 传递的参数
	m_threadDlg.m_nPid = m_nPid;
	m_threadDlg.m_pEprocess = m_pEprocess;
	m_threadDlg.m_szImageName = m_szImageName;
	m_tab.AddPage(L"Thread", &m_threadDlg, IDD_THREAD_DIALOG);


	m_vadDlg.m_dwPid = (ULONG)m_nPid;
	m_tab.AddPage(L"VAD_INFO", &m_vadDlg, IDD_VAD_DIALOG);

	m_moduleDlg.m_dwPid = m_nPid;
	m_moduleDlg.m_pEprocess = (ULONG_PTR)m_pEprocess;
	m_tab.AddPage(L"Modules", &m_moduleDlg, IDD_MODULE_DIALOG);

	m_memoryDlg.m_dwPid = m_nPid;
	m_memoryDlg.m_pEprocess = (ULONG_PTR)m_pEprocess;
	m_tab.AddPage(L"Memory", &m_memoryDlg, IDD_MEMORY_DIALOG);


	m_handleDlg.m_dwPid = m_nPid;
	m_tab.AddPage(L"Hanles", &m_handleDlg, IDD_HANDLE_DIALOG);


	m_wndDlg.m_dwPid = (ULONG)m_nPid;
	m_tab.AddPage(L"Windows", &m_wndDlg, IDD_HWND_DIALOG);

	m_tokenDlg.m_dwPid = (ULONG)m_nPid;
	m_tab.AddPage(L"Token", &m_tokenDlg, IDD_TOKEN_DIALOG);

	m_tab.Show();

	
	AfxBeginThread(m_memoryDlg.threadWork, &m_memoryDlg);
	


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}





