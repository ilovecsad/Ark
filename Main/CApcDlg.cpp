// CApcDlg.cpp: 实现文件
//

#include "pch.h"
#include "Main.h"
#include "CApcDlg.h"
#include "afxdialogex.h"
#include <TlHelp32.h>

// CApcDlg 对话框

IMPLEMENT_DYNAMIC(CApcDlg, CDialogEx)

CApcDlg::CApcDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_APC_CALL_DIALOG, pParent)
{

}

CApcDlg::~CApcDlg()
{
}

void CApcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CALL_APC, m_edCall);
	DDX_Control(pDX, IDC_EDIT_ARG_APC, m_edArg);
	DDX_Control(pDX, IDC_CHECK_DLL, m_cbDllInject);
	DDX_Control(pDX, IDC_BUTTON_GETPATH, m_cbGetDllPath);
	DDX_Control(pDX, IDC_CHECK2, m_cbIs64);
}


BEGIN_MESSAGE_MAP(CApcDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CApcDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_GETPATH, &CApcDlg::OnBnClickedButtonGetpath)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CApcDlg 消息处理程序

UINT8    ShellCode64[0x100] = {

	0x48,0x83,0xEC,0x28,   //sub rsp,0x28   4

	0x48,0x8D,0x0d,0x00,0x00,0x00,0x00, //lea rcx,[xxx]  7

	0xff,0x15,0x00,0x00,0x00,0x00, //call qword ptr [xxxx]  6

	0x48,0x83,0xc4,0x28,  //add rsp,0x28  4

	0xff,0x25,0x00,0x00,0x00,0x00,//jmp        6


	0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,

};

UINT8    ShellCode32[0x100] = {
	0x60,
	0x9c,
	0x68,
	0x00,0x00,0x00,0x00,
	0xff,0x15,
	0x00,0x00,0x00,0x00,
	0x9d,
	0x61,
	0xff,0x25,
	0x00,0x00,0x00,0x00,


	0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00
};





BOOL CApcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	
	m_szDllPath.Empty();

	m_cbIs64.SetCheck(TRUE);
	m_cbIs64.EnableWindow(FALSE);

	SetTimer(0x1111, 100,NULL);

	m_edArg.SetWindowText(L"0");




	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CApcDlg::PreTranslateMessage(MSG* pMsg)
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



HANDLE NtOpenThread(DWORD dwProcessId, DWORD dwThreadId)
{

	typedef struct _CLIENT_IDEX_ {
		PVOID UniqueProcess;
		PVOID UniqueThread;
	}CLIENT_IDEX, *PCLIENT_IDEX;
	typedef NTSTATUS(NTAPI *pNtOpenThread)(PHANDLE ThreadHandle, ACCESS_MASK AccessMask, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_IDEX);
	FARPROC fpNtOpenThread = GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtOpenThread");
	if (fpNtOpenThread)
	{
		HANDLE ThreadHandle = 0;
		OBJECT_ATTRIBUTES ObjectAttributes;
		CLIENT_IDEX ClientId;
		InitializeObjectAttributes(&ObjectAttributes, NULL, NULL, NULL, NULL);
		ClientId.UniqueProcess = (PVOID)dwProcessId;
		ClientId.UniqueThread = (PVOID)dwThreadId;
		pNtOpenThread fNtOpenThread = (pNtOpenThread)fpNtOpenThread;
		if (NT_SUCCESS(fNtOpenThread(&ThreadHandle, MAXIMUM_ALLOWED, &ObjectAttributes, &ClientId)))
		{
			if (ThreadHandle)
			{
				return ThreadHandle;
			}
		}
	}
	return 0;
}








void CApcDlg::OnBnClickedOk()
{
	
	CString szTemp = L"";
	if (!m_cbDllInject.GetCheck()) {
		m_edCall.GetWindowText(szTemp);
		ULONG64 pCall64 = (ULONG64)_tcstoull(szTemp, 0, 16);


		if (!pCall64) {

			AfxMessageBox(L"call地址不能为空");
			return;
		}
		m_edArg.EnableWindow(FALSE);
		//m_edArg.GetWindowText(szTemp);
		//ULONG64 pArg = (ULONG64)_tcstoull(szTemp, 0, 16);

		if (AfxMessageBox(L"程序很容易崩溃是否继续", MB_OKCANCEL) == IDOK)
		{
			EipCall_64((ULONG)m_dwPid, (ULONG)m_dwThreadId, pCall64);
		}
		


	}
	else
	{
		char szDllPath[256] = { 0 };
		GetWindowTextA(this->GetSafeHwnd(), szDllPath, 256);
		if (strlen(szDllPath)) 
		{
			BOOL ntStatus = Inject((ULONG)m_dwPid, (ULONG)m_dwThreadId, szDllPath);
			if (ntStatus)
			{
				AfxMessageBox(L"注入成功");
			}
			else {
				AfxMessageBox(L"注入失败");
			}
		}
	}

}



BOOL CApcDlg::Inject(ULONG ProcessId, ULONG ThreadId,char*szDllPath)
{
	if (!m_cbIs64.GetCheck())return FALSE;

	HANDLE    ThreadHandle = m_clsThread.KernelOpenThread(ThreadId);//OpenThread(THREAD_ALL_ACCESS, FALSE, ThreadId);
	//HANDLE    ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);

	// 首先挂起线程

	SuspendThread(ThreadHandle);

	CONTEXT        ThreadContext = { 0 };
	ThreadContext.ContextFlags = CONTEXT_ALL;

	if (GetThreadContext(ThreadHandle, &ThreadContext) == FALSE)
	{
		CloseHandle(ThreadHandle);
		//CloseHandle(ProcessHandle);
		return FALSE;
	}

	PVOID    BufferData = (PVOID)m_clsMemory.AllocateVirtualMemory(ProcessId, NULL, 0x1000, MEM_COMMIT);//VirtualAllocEx(ProcessHandle, NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (BufferData != NULL)
	{

		UINT_PTR    LoadLibraryAddress = (UINT_PTR)GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "LoadLibraryA");
		if (LoadLibraryAddress != NULL)
		{
			
			fixShellcode64(ShellCode64, BufferData, (UINT64)LoadLibraryAddress, ThreadContext.Rip, szDllPath);
			
	
			if (!m_clsMemory.RtlSuperCopyMemory(ProcessId, (ULONG_PTR)BufferData, ShellCode64,sizeof(ShellCode64)))//!WriteProcessMemory(ProcessHandle, BufferData, ShellCode64, 0x100, NULL))
			{
				CloseHandle(ThreadHandle);
				m_clsMemory.FreeVirtualMemory(ProcessId, (ULONG_PTR)BufferData, 0x1000);
				return FALSE;
			}
			ThreadContext.Rip = (UINT64)BufferData;
         
			if (!SetThreadContext(ThreadHandle, &ThreadContext))
			{
				CloseHandle(ThreadHandle);
				m_clsMemory.FreeVirtualMemory(ProcessId, (ULONG_PTR)BufferData, 0x1000);
				return FALSE;
			}
			ResumeThread(ThreadHandle);
		}
	}

	CloseHandle(ThreadHandle);
	//CloseHandle(ProcessHandle);
	return TRUE;
}


VOID CApcDlg::fixShellcode32(UINT8 * pNeedFixShellcode, ULONG pAllocateAddress, ULONG pCallAddress, ULONG jmpRip, char * dllPath)
{
	PUINT8    v1 = pNeedFixShellcode + 29;

	if (dllPath) {
		memcpy((char*)v1, dllPath, strlen(dllPath) + 1);
	}
	*(PUINT32)(pNeedFixShellcode + 3) = (UINT32)pAllocateAddress + 29;

	*(PUINT32)(pNeedFixShellcode + 25) = pCallAddress;
	*(PUINT32)(pNeedFixShellcode + 9) = (UINT32)pAllocateAddress + 25;

	*(PUINT32)(pNeedFixShellcode + 21) = jmpRip;
	*(PUINT32)(pNeedFixShellcode + 17) = (UINT32)pAllocateAddress + 21;

	return;
}
VOID CApcDlg::fixShellcode64(UINT8*pNeedFixShellcode, PVOID pAllocateAddress/*存放shellcode地址*/, ULONG64 pCallAddress, UINT64 jmpRip, char* dllPath /*可空*/)
{
	// ShellCode + 43
	PUINT8    v1 = pNeedFixShellcode + 43;
	if (dllPath)
	{
		memcpy(v1, dllPath, strlen(dllPath) + 1);
	}

	UINT32    DllNameOffset = (UINT32)(((PUINT8)pAllocateAddress + 43) - ((PUINT8)pAllocateAddress + 4) - 7);
	*(PUINT32)(pNeedFixShellcode + 7) = DllNameOffset;

	// ShellCode + 35
	*(PUINT64)(pNeedFixShellcode + 35) = (UINT64)pCallAddress;
	UINT32    LoadLibraryAddressOffset = (UINT32)(((PUINT8)pAllocateAddress + 35) - ((PUINT8)pAllocateAddress + 11) - 6);
	*(PUINT32)(pNeedFixShellcode + 13) = LoadLibraryAddressOffset;

	*(PUINT64)(pNeedFixShellcode + 27) = jmpRip;
}

void CApcDlg::OnBnClickedButtonGetpath()
{
	CString szTemp = L"";

	if (AfxMessageBox(L"只支持64位程序!只支持64位程序!", MB_OKCANCEL) == MB_OK) {

		CFileDialog fileDlg(TRUE, 0, NULL, 0, L"All Files (*.*)|*.*||", 0);
		if (IDOK == fileDlg.DoModal())
		{
			szTemp = fileDlg.GetPathName();
			SetWindowText(szTemp);
		}
	}
}


void CApcDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	KillTimer(0x1111);

	CDialogEx::OnCancel();
}


void CApcDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 0x1111)
	{
		if (m_cbDllInject.GetCheck())
		{
			m_cbGetDllPath.EnableWindow(TRUE);
			m_edCall.EnableWindow(FALSE);
			m_edArg.EnableWindow(FALSE);
		}
		else {
			m_edCall.EnableWindow(TRUE);
			m_edArg.EnableWindow(TRUE);
			m_cbGetDllPath.EnableWindow(FALSE);
		}
	}


	CDialogEx::OnTimer(nIDEvent);
}

BOOL CApcDlg::EipCall_64(ULONG ProcessId, ULONG ThreadId, ULONG64 pCall)
{
	HANDLE    ThreadHandle = m_clsThread.KernelOpenThread(ThreadId);

	SuspendThread(ThreadHandle);

	CONTEXT        ThreadContext = { 0 };
	ThreadContext.ContextFlags = CONTEXT_ALL;

	if (GetThreadContext(ThreadHandle, &ThreadContext) == FALSE)
	{
		CloseHandle(ThreadHandle);
		return FALSE;
	}

	PVOID    BufferData = (PVOID)m_clsMemory.AllocateVirtualMemory(ProcessId, NULL, 0x1000, MEM_COMMIT);
	{
		if (1)
		{

			fixShellcode64(ShellCode64, BufferData, (UINT64)pCall, ThreadContext.Rip, NULL);


			if (!m_clsMemory.RtlSuperCopyMemory(ProcessId, (ULONG_PTR)BufferData, ShellCode64, sizeof(ShellCode64)))
			{
				CloseHandle(ThreadHandle);
				m_clsMemory.FreeVirtualMemory(ProcessId, (ULONG_PTR)BufferData, 0x1000);
				return FALSE;
			}
			ThreadContext.Rip = (UINT64)BufferData;

			if (!SetThreadContext(ThreadHandle, &ThreadContext))
			{
				CloseHandle(ThreadHandle);
				m_clsMemory.FreeVirtualMemory(ProcessId, (ULONG_PTR)BufferData, 0x1000);
				return FALSE;
			}
			ResumeThread(ThreadHandle);
		}
	}

	CloseHandle(ThreadHandle);
	
	return TRUE;
}
