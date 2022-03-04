#pragma once
#include "SortListCtrl.h"
#include "ListModules.h"
#include "CListVadInfo.h"
typedef enum _DLL_HEADER_INDEX_
{
	eDllPath,
	eDllBase,
	eDllSize,
	eDllFileCompany,
	eDllFileDescription,
	eDllCreateTime,
}DLL_HEADER_INDEX;

// CListModuleDlg 对话框

typedef struct _DLL_INFO_
{
	ULONG_PTR Base;
	ULONG_PTR Size;
	CString DllPath;
	ITEM_COLOR clrItem;
}DLL_INFO, *PDLL_INFO;


typedef struct _DLL_BASE_
{
	ULONG32 ntdll_base;
	ULONG32 kernel32_base;
	
}DLL_BASE;

class CListModuleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CListModuleDlg)

public:
	CListModuleDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CListModuleDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MODULE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCustomdrawVadList(NMHDR *pNMHDR, LRESULT *pResult);
public:
	
	ULONG_PTR m_ntdllBase;
	ULONG32 m_ntdllBase32;
	ULONG32 m_kernel32Base;
	ULONG m_nNotMicro;								// 非微软的模块数量
	ULONG m_nPEBModules;								// 非PEB模块的数量
	CommonFunctions m_Functions;
	CString m_VadInfo;
	ULONG m_nModuleCnt;								// 模块总的数量
	HANDLE m_dwPid;
	ULONG_PTR m_pEprocess;
	CSortListCtrl m_listModule;
	CListModules m_clsModules;
	CListVadInfo m_clsVadInfo;
	vector<DLL_INFO> m_vectorModules;			// 得到的模块列表
	vector<VAD_INFO> m_vectorVadInfo;
	vector<MODULE_INFO_EX>m_vectorByPeb;
	list<ULONG_PTR>m_vectorByPebList;
public:
	void EnumModules();
	void AddDllModuleItem(VAD_INFO item);
	BOOL IsModuleFromPEB(ULONG_PTR dwBase);

	afx_msg void OnRclickModuleList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnModuleReFresh();
	afx_msg LRESULT OnAddItem(WPARAM wParam, LPARAM lParam);
	afx_msg void OnExportText();
	afx_msg void OnCopyProcessPath();
	afx_msg void OnFreeDll();
	PDLL_INFO GetDllInfoByItem(int nItem);
	afx_msg void OnCopyModule();
	afx_msg void OnHideModule();
	PVOID GetLdrpHashTableAddress(PVOID pKernel32_BaseThreadInitThunk);
};
