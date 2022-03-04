#pragma once

#include "ListThreads.h"
// CCallDlg 对话框

class CCallDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCallDlg)

public:
	CCallDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCallDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CALL_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	HANDLE m_dwPid;
	CListThreads m_clsThread;
	CEdit m_edCall;
	// 可空
	CEdit m_edArg;
	afx_msg void OnBnClickedOk();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
