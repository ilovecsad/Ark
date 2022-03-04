#pragma once
#include "Common.h"
#include "ListMemory.h"
// CChangeProtectionDlg 对话框

class CChangeProtectionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChangeProtectionDlg)

public:
	CChangeProtectionDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CChangeProtectionDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHANGE_MEMORY_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListMemory m_clsMemory;
	PMEMORY_INFO m_pMemoryInfo;
	ULONG m_dwPid;
	virtual BOOL OnInitDialog();
	CComboBox m_cbProtection;
	vector<ULONG_PTR> m_seletProtection;
	afx_msg void OnSelchangeProtectionCombo();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

};
