#pragma once
#include "TabSheet.h"
#include "CListVadDlg.h"
#include "ListThreadDlg.h"
#include "ListModuleDlg.h"
#include "ListMemoryDlg.h"
#include "ListHanleDlg.h"
#include "CListTokenDlg.h"
#include "ListWindowDlg.h"

// CProcessInfoDlg 对话框

class CProcessInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProcessInfoDlg)

public:
	CProcessInfoDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CProcessInfoDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROCESS_INFO_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	ULONG m_endCnt;
	CListThreadDlg m_threadDlg;
	CListVadDlg m_vadDlg;
	CListModuleDlg m_moduleDlg;
	CListMemoryDlg m_memoryDlg;
	CListHanleDlg m_handleDlg;
	CListTokenDlg m_tokenDlg;
	CListWindowDlg m_wndDlg;
	

	CTabSheet m_tab;
	HANDLE m_nPid;                   //保存进程PID
	PVOID m_pEprocess;               //保存进程 EPROCESS
	CString m_szImageName;            
	CString m_szPath;
	CString m_szParentImage;
	

};
