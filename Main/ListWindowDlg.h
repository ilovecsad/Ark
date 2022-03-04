#pragma once

#include "SortListCtrl.h"
typedef enum _WINDOWS_INFO_ENUM
{
	eWndHand,
	eWndTitle,
	eWndClass,
	eWndEnable,
	eWndThread
}WINDOWS_INFO_ENUM;

typedef struct _WINDOWS_INFO_
{
	HWND  dwWndHand;
	BOOL bMain;
	ULONG  dwWndEnable;
	ULONG dwWndThread;
	WCHAR szWndClass[50];
    WCHAR szWndTitle[100];
	
}WINDOWS_INFO,*PWINDOWS_INFO;

typedef struct _WINDOWS_INFO_ALL_
{
	ULONG nCnt;
	ULONG dwProcessPid;
	WINDOWS_INFO pWndInfo[1];
}WINDOWS_INFO_ALL,*PWINDOWS_INFO_ALL;





// CListWindowDlg 对话框

class CListWindowDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CListWindowDlg)

public:
	CListWindowDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CListWindowDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HWND_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	
	ULONG m_dwPid;
	CSortListCtrl m_list;
	virtual BOOL OnInitDialog();
	VOID ListAllWindows(ULONG dwPid);
	vector<WINDOWS_INFO>m_vecWndInfo;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	VOID addItem(WINDOWS_INFO dwItem);
	afx_msg void OnResfresh();
	afx_msg void OnRclickWndList(NMHDR *pNMHDR, LRESULT *pResult);
};
