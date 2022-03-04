#pragma once
#include "SortListCtrl.h"
#include "CListVadInfo.h"
// CListVadDlg 对话框

typedef enum _VAD_DLG_
{
	eVAD = 0,
	eStart,
	eEnd,
	eType,
	eProtect,
	eSection,

}VAD_INFOS;


class CListVadDlg : public CDialogEx
{
	
	DECLARE_DYNAMIC(CListVadDlg)

public:
	CListVadDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CListVadDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VAD_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	CommonFunctions m_Functions;
	ULONG m_nCnt;
	ULONG m_dwPid;    //要打开的PID
	CSortListCtrl m_listVad;
	CListVadInfo m_clsVad;
	vector<VAD_INFO> m_vectorVad;

public:
	void EnumVadInfo();
	void AddVadInfoItem(VAD_INFO item);
	CString MemoryProtection(ULONG_PTR flags);
	CString MemoryProtectionEx(ULONG_PTR flags);
	CString MemoryType(ULONG_PTR flags,ULONG_PTR a);
	afx_msg void OnRclickVadList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnProcessdlgRefresh();
	afx_msg void OnChangeVadProtection();
	PVAD_INFO GetVadInfoByItem(int nItem);
};
