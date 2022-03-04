#pragma once
#include <afxcmn.h>

#define WM_CUSTOMER_MY					WM_USER + 1000

// 进程相关的消息
#define WM_RESIZE_ALL_PROC_WND			WM_CUSTOMER_MY + 1		// 重新分布进程列表下面的各个窗口
#define WM_CLICK_LIST_HEADER_START		WM_CUSTOMER_MY + 2		// 开始点击列表头
#define WM_CLICK_LIST_HEADER_END		WM_CUSTOMER_MY + 3		// 结束点击列表头


#ifndef _SORTLISTCTRL_INCLUDE__
#define _SORTLISTCTRL_INCLUDE__

/////////////////////////////////////////////////////////////////////////////////////////
//一个点击列头时可以排序的列表类
class CSortListCtrl : public CListCtrl
{
	// Construction
public:
	CSortListCtrl();
	void EnableSort(BOOL bEnable = TRUE) { m_bEnableSort = bEnable; };
	// Attributes
public:
	struct Info
	{
		CSortListCtrl* pListCtrl;
		int nSubItem;
		BOOL bAsc;		//是否是升序
		BOOL bIsNumber;	//该列是否都是数字
	};

	// Summary: 改变列头图标和排序状态，并返回新的排序状态。bClear为真时清除列头图标。
	BOOL ChangeHeardCtrlState(CHeaderCtrl* pHeardCtrl, int nItem, BOOL bClear);

	static int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	// Operations
public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSortListCtrl)
		//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CSortListCtrl();

	// Generated message map functions
protected:
	CBitmap* MakeColorBoxImage(BOOL bUp);
	CImageList m_ImageList;
	BOOL m_bInit;
	BOOL m_bEnableSort;
	CBitmap *m_pBmp[2];

	//{{AFX_MSG(CSortListCtrl)
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif