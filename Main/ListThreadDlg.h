#pragma once
#include "SortListCtrl.h"
#include "ListThreads.h"
#include "ListModules.h"
#include "CommonFunctions.h"
// CListThreadDlg 对话框
typedef enum _KTHREAD_STATE
{
	Initialized,
	Ready,
	Running,
	Standby,
	Terminated,
	Waiting,
	Transition,
	DeferredReady,
	GateWait
} KTHREAD_STATE, *PKTHREAD_STATE;
typedef enum _THREAD_HEADER_INDEX_
{
	eThreadId,
	eThreadObject,
	eThreadTeb,
	eThreadPriority,
	eThreadStartAddress,
	eThreadSwitchTimes,
	eThreadStatus,
	eThreadFlag,
	eThreadStartModule,
}THREAD_HEADER_INDEX;

class CListThreadDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CListThreadDlg)

public:
	CListThreadDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CListThreadDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_THREAD_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
public:
 
	HANDLE m_nPid;                   //保存进程PID
	PVOID m_pEprocess;               //保存进程 EPROCESS
	CString m_szImageName;
	CommonFunctions m_Functions;
	CSortListCtrl m_threadList;
	CString m_threadInfo;
	ULONG m_nThreadsCnt;
	ULONG m_hideThreadCount;
	vector<THREAD_INFO> m_vectorThreads;
	vector<ITEM_COLOR> m_vectorColor;
	vector<THREAD_INFO> m_vectorThreadsAll;
	CListThreads m_clsThreads;
	CListModules m_clsModules;
	vector<MODULE_INFO_EX> m_vectorModules;
	vector<DRIVER_INFO> m_vectorDrivers;
	CListDrivers m_clsDrivers;
private:
	void EnumThreads();
	void AddThreadItem(THREAD_INFO ThreadInfo);
	CString GetModulePathByThreadStartAddress(ULONG_PTR dwBase);
	CString GetDriverPath(ULONG_PTR pCallback);

private:

public:

	afx_msg void OnCustomdrawThreadList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRclickThreadList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRefreshThread();
	afx_msg void OnSuspendThread();
	afx_msg void OnResumeThread();
	afx_msg void OnKillThread();
	PTHREAD_INFO GetThreadInfoByItem(int nItem);
	afx_msg void OnExportText();
	afx_msg void OnRemoteCall();
	afx_msg void OnApcCall();
};
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)