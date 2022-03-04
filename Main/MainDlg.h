
// MainDlg.h: 头文件
//

#pragma once
#include "CExternDlg.h"
#include "CListProcessHook.h"
#include "CListCallBackDlg.h"
// CMainDlg 对话框
class CMainDlg : public CDialogEx
{
// 构造
public:
	CMainDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;


	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	
public:
	CTabSheet m_mainTab;
	CListProcessDlg m_processDlg;
	CListDriverDlg m_driverDlg;
	CExternDlg m_externDlg;
	CListProcessHook m_hookDlg;
	CListCallBackDlg m_callBackDlg;
public:

	int m_test;
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CRect m_rect;
	void ChangeSize(UINT nID, int x, int y);
	void ReSize(void);
	POINT old;
	afx_msg void OnClose();
};
