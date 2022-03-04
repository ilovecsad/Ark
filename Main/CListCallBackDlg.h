#pragma once
#include "SortListCtrl.h"
#include "Common.h"
// CListCallBackDlg 对话框
#include "CallBack.h"
#include "ListDrivers.h"
#include "CommonFunctions.h"
typedef enum _TYPE_CALLBACK_
{
	eCreateProcess = 0,
	eCreateThread,
	eCmCallback,
	eLoadImage,
	eObRegister

}TYPE_CALLBACK;

typedef struct _CALLBACK_DATA_ 
{
	ULONG Type;

	ULONG64 funcAddress;
	OB_CALLBACK info;
	CString szPath;
}CALLBACK_DATA,*PCALLBACK_DATA;




class CListCallBackDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CListCallBackDlg)

public:
	CListCallBackDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CListCallBackDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CALLBACK_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListDrivers m_clsDriver;
	vector<CALLBACK_DATA>m_data;

	CommonFunctions m_Funcionts;
	CCallBack m_clsFunc;
	vector<DRIVER_INFO> m_vectorDrivers;

	CSortListCtrl m_list;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void showAllReusult();
	VOID GetData();
	CString GetPathByAddress(ULONG64 dwAddress);
	vector<ITEM_COLOR> m_vectorColor;
	afx_msg void OnCustomdrawCallbackList(NMHDR *pNMHDR, LRESULT *pResult);
};
