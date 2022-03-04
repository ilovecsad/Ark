#pragma once
#include "SortListCtrl.h"
#include "Common.h"
#include "ListHandles.h"
// CListHanleDlg 对话框

class CListHanleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CListHanleDlg)

public:
	CListHanleDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CListHanleDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HANDLE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CSortListCtrl m_list;
	HANDLE m_dwPid;
	vector<HANDLE_INFO> m_vectorSaveAllHandle;           //保存有名字的
	vector<HANDLE_INFO> m_HandleNoNameVector;     //保存没有名字的
	ULONG m_nCnt;  //记录 句柄总数量
	ULONG m_nHaveNameCnt; //只记录 有名字句柄的数量
	CListHandles m_clsHandles;
	CString m_handleInfo;
	BOOL m_bStart;
public:
	virtual BOOL OnInitDialog();
	void EnumProcessHandles();
	VOID AddHandleItem(HANDLE_INFO HandleInfo);
	CString AnalysisProcessContext(ULONG_PTR a);
	CString AnalysisThreadContext(ULONG_PTR b);
	CString AnalysisSectionContext(ULONG_PTR b);
	CString AnalysisTokenContext(ULONG_PTR b);
	CString AnalysisFileContext(ULONG_PTR b);
	CString AnalysisKeyContext(ULONG_PTR b);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnRclickHanleList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHandledlgRefresh();
	afx_msg void OnHandledlgShow();
	afx_msg LRESULT OnAddItem(WPARAM wParam,LPARAM lParam);
	
};
