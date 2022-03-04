#pragma once
#include "ListThreads.h"
#include "ListMemory.h"
// CApcDlg 对话框

class CApcDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CApcDlg)

public:
	CApcDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CApcDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APC_CALL_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_szDllPath;
	HANDLE m_dwPid;
	HANDLE m_dwThreadId;
	CEdit m_edCall;
	CEdit m_edArg;
	CListThreads m_clsThread;
	CListMemory m_clsMemory;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	BOOL Inject(ULONG ProcessId, ULONG ThreadId, char*szDllPath);
	VOID fixShellcode64(UINT8*pNeedFixShellcode, PVOID pAllocateAddress/*存放shellcode地址*/, ULONG64 pCallAddress, UINT64 jmpRip, char* dllPath /*可空*/);
	VOID fixShellcode32(UINT8 * pNeedFixShellcode, ULONG pAllocateAddress, ULONG pCallAddress, ULONG jmpRip, char * dllPath);
	CButton m_cbDllInject;
	afx_msg void OnBnClickedButtonGetpath();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	BOOL EipCall_64(ULONG ProcessId, ULONG ThreadId, ULONG64 pCall);


	CButton m_cbGetDllPath;
	CButton m_cbIs64;
};

