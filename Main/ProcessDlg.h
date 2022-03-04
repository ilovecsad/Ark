#pragma once
#include "ListProcess.h"
#include <list>
#include <vector>
#include "CommonFunctions.h"
#include "SortListCtrl.h"

using namespace std;
// CProcessDlg 对话框

typedef enum _PROCESS_HEADER_INDEX_
{
	eProcessImageName,
	eProcessPid,
	eProcessParentPid,
	eProcessPath,
	eProcessEprocess,
	eProcessR3Access,
	eProcessFileCompany,
	eProcessFileDescription,
	eProcessCreateTime,
	eProcessCommandLine
}PROCESS_HEADER_INDEX;

class CListProcessDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CListProcessDlg)

public:
	CListProcessDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CListProcessDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROCESS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCustomdrawProcessList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRclickProcessList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHelpRefresh();
	afx_msg void OnHelpShowAllInfo();
	afx_msg void OnSusprocess();
	afx_msg void OnResumeProcess();

private:
	
public:
	CString m_szProcessCnt;						// 进程下方状态,显示进程数量等信息
	list<HANDLE> m_Ring3ProcessList;				// 进程Ring3枚举得到的list,只保存pid,用来比较哪些是隐藏的进程
	vector<PROCESS_INFO> m_Ring0ProcessList;	// R0枚举上来的进程信息列表
	vector<ITEM_COLOR_INFO> m_vectorColor;		// 颜色列表
	vector<PROCESS_INFO> m_vectorProcesses;		// 在列表框中显示的进程列表



	CSortListCtrl m_processlist; //CListCtrl m_processlist;
	CommonFunctions m_Functions;				// 常用的API类
	CImageList m_ProImageList;
	HICON m_hProcessIcon;
	CListProcess m_clsProcess;					// 进程类
	ULONG m_nProcessCnt;						// 总共的进程数量						
	ULONG m_nHideCnt;							// 隐藏进程的数量
	ULONG m_nDenyAccessCnt;						// R3无法打开的进程数量
	COLORREF m_clrHideProc;						// 隐藏进程的颜色
	COLORREF m_clrMicrosoftProc;				// 微软的进程颜色
	COLORREF m_clrNormalProc;					// 正常的进程，不是微软进程的颜色
	COLORREF m_clrMicroHaveOtherModuleProc;		// 微软带有非微软模块进程颜色
	COLORREF m_clrNoSigniture;					// 没有数字签名的进程颜色

public:
	void ListProcess();
	void ShowProcessAsList();
	void AddProcessItem(PROCESS_INFO item);
	BOOL CanOpenProcess(HANDLE dwPid);
	CString FileTime2SystemTime(ULONGLONG ulCreateTile);
	BOOL GetRing3ProcessList();
	BOOL IsProcessHide(HANDLE pid);
	BOOL IsHaveNotMicrosoftModule(DWORD dwPID);
	void ShowProcessListDetailInfo(int nItem);
	PPROCESS_INFO GetProcessInfoByItem(int nItem);
	CString GetParentProcessInfo(HANDLE dwParentPid);

	afx_msg void OnLocationExplorer();
	afx_msg void OnCopyProcessName();
	afx_msg void OnCopyProcessPath();
	afx_msg void OnKillProcess();
	afx_msg void OnExportText();
	afx_msg void OnExportExcel();
};
