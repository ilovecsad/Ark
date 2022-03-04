#pragma once
#include "SortListCtrl.h"
#include "Common.h"
#include "ListMemory.h"
#include "ListModules.h"
#include "CListVadInfo.h"
typedef enum _MEMORY_HEADER_INDEX
{
	eMemoryBase,
	eMemorySize,
	eMemoryProtect,
	eMemoryVadFlags,
	eMemoryState,
	eMemoryType,
	eMmeoryModuleName,
}MEMORY_HEADER_INDEX;
// CListMemoryDlg 对话框


typedef struct _FILE_OBJECT_INFO_
{
	ULONG_PTR pObject;
	CString PathName;
}FILE_OBJECT_INFO, *PFILE_OBJECT_INFO;

typedef struct _PROTECT_
{
	ULONG uType;
	WCHAR szTypeName[100];
}PROTECT, *PPROTECT;




class CListMemoryDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CListMemoryDlg)

public:
	CListMemoryDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CListMemoryDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MEMORY_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
public:
	CListModules m_clsModules;
	CommonFunctions m_Functions;
	CString m_szInfo;
	CSortListCtrl m_list;
	HANDLE m_dwPid;
	ULONG m_nMemoryCnt;
	ULONG_PTR m_pEprocess;
	vector<MEMORY_INFO> m_vectorMemory;
	CListMemory m_clsMemory;
	vector<PROTECT> m_vectorProtectType;
	CListModules m_clsListModules;
	vector<MODULE_INFO_EX> m_vectorModules;
	CListVadInfo m_clsVad;
	vector<FILE_OBJECT_INFO> m_vectorFileObject;
	vector<ULONG_PTR> m_vectorObjectAddress;     //保持 Object指针
public:
	void AddMemoryItem(MEMORY_INFO item);
	VOID InitMemoryProtect();
	CString GetMemoryProtect(ULONG_PTR Protect);
	CString GetMempryState(ULONG_PTR State);
	CString GetMempryType(ULONG_PTR Type);
	CString GetModuleImageName(ULONG_PTR dwBase);
	CString MemoryProtectionEx(ULONG_PTR flags);
	CString MemoryProtection(ULONG_PTR flags);
	VOID GetModuleNameByObjectPoint();
	CString FindModuleName(ULONG_PTR pFileObject);
	afx_msg void OnMemoryDlgRefresh();
	afx_msg void OnRclickMemoryList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnAddItem(WPARAM wParam, LPARAM lParam);
	void InitSupportThread();
	static UINT threadWork(LPVOID lParam);
	afx_msg void OnDumpMemory();
	PMEMORY_INFO GetMemoryInfoByItem(int nItem);
	afx_msg void OnChangeMemoryprotection();
	afx_msg void OnAllocateMemory();
	afx_msg void OnFreeMemory();
	afx_msg void OnMdlWriteMemory();
	
};
