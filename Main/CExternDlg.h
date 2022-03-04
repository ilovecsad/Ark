#pragma once
#include "ExternTool.h"

// CExternDlg 对话框

class CExternDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CExternDlg)

public:
	CExternDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CExternDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXTERN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CButton m_cbEnableInject;
	CEdit m_edProcessName;
	CEdit m_edDllPath;
	CButton m_cbSetProcessName;
	CButton m_cbSetDllPath;
	CExternTool m_clsExtern;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	
	afx_msg void OnClickedButtonProcess();
	afx_msg void OnClickedButtonDll();
	afx_msg void OnBnClickedButtonStart();
	CButton m_cbStartBtn;
	CString m_szProcessName;
	CString m_szDllPath;
	
	CButton m_cbMemory;
};
