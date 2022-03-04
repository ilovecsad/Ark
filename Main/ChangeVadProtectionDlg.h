#pragma once
#include "CListVadInfo.h"

// CChangeVadProtectionDlg 对话框

class CChangeVadProtectionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChangeVadProtectionDlg)

public:
	CChangeVadProtectionDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CChangeVadProtectionDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VAD_PROTECTION_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListVadInfo m_clsVad;
	ULONG_PTR m_dwBaseAddress;
	ULONG m_dwPid;
	ULONG_PTR m_oldVadFlags;
	
	CComboBox m_cbView;
	CButton m_clearNoChange;
	CButton m_clearEnclave;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendcancelCbVad();
	afx_msg void OnBnClickedQuxiao();
};
