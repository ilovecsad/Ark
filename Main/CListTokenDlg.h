#pragma once
#include "SortListCtrl.h"

// CListTokenDlg 对话框


typedef struct _TOKEN_EX
{
	ULONG_PTR  dwIndex;
	ITEM_COLOR clrItem;
}TOKEN_EXEX;

class CListTokenDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CListTokenDlg)

public:
	CListTokenDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CListTokenDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOKEN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	ULONG_PTR m_dwToken;
	vector<TOKEN_EXEX> m_vectorToken;
	ULONG m_dwPid;
	CSortListCtrl m_list;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void InitFunc();

	afx_msg void OnCustomdrawTokenList(NMHDR *pNMHDR, LRESULT *pResult);
	void ShowProcessToken(ULONG_PTR dwToken);
};
