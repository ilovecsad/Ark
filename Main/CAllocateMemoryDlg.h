#pragma once

#include "ListMemory.h"
// CAllocateMemoryDlg 对话框

class CAllocateMemoryDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAllocateMemoryDlg)

public:
	CAllocateMemoryDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAllocateMemoryDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	ULONG m_dwPid;
	CEdit m_edAddress;
	CEdit m_edSize;
	CComboBox m_cbProtection;
	CListMemory m_clsMemory;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
