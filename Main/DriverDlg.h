#pragma once
#include <list>
#include "SortListCtrl.h"
#include "ListDrivers.h"
#include "Registry.h"
#include "CommonFunctions.h"

using namespace std;

typedef LONG NTSTATUS;

#define STATUS_SUCCESS                  ((NTSTATUS)0x00000000L)   
#define STATUS_UNSUCCESSFUL             ((NTSTATUS)0xC0000001L)   
#define STATUS_NOT_IMPLEMENTED          ((NTSTATUS)0xC0000002L)   
#define STATUS_INVALID_INFO_CLASS       ((NTSTATUS)0xC0000003L)   
#define STATUS_INFO_LENGTH_MISMATCH     ((NTSTATUS)0xC0000004L)   
typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemModuleInformation = 11
} SYSTEM_INFORMATION_CLASS;

typedef struct tagSYSTEM_MODULE_INFORMATION {
	PVOID Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	CHAR ImageName[256];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

typedef struct {
	ULONG  dwNumberOfModules;
	PSYSTEM_MODULE_INFORMATION  smi;
} MODULE_INFO, *PMODULE_INFO;

typedef
NTSTATUS(WINAPI *pfnZwQuerySystemInformation)
(IN SYSTEM_INFORMATION_CLASS,
	IN OUT PVOID,
	IN ULONG,
	OUT PULONG OPTIONAL);



typedef enum _DRIVER_HEADER_INDEX
{
	eDriverName,
	eDriverBase,
	eDriverSize,
	eDriverObject,
	eDriverPath,
	eDriverServiceName,
	eDriverStartupType,
	eDriverLoadOrder,
	eDriverFileCorporation,
	eDriverFileDescription,
}DRIVER_HEADER_INDEX;
// CDriverDlg 对话框
typedef struct _SERVICE_KEY_
{
	CString szKey;
	CString szPath;
}SERVICE_KEY, *PSERVICE_KEY;


class CListDriverDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CListDriverDlg)

public:
	CListDriverDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CListDriverDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DRIVER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLvnItemchangedDriverList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCustomdrawDriverList(NMHDR *pNMHDR, LRESULT *pResult);
public:
	ULONG nHideDriverCnt;
	CommonFunctions m_Functions;
	PVOID m_driverBuffer;
	CommonFunctions m_Funcionts;
	CSortListCtrl m_driverList;
	CString m_driverInfo;
	vector<DRIVER_INFO> m_vectorDrivers;


	vector<ITEM_COLOR> m_vectorColor;
	vector<ULONG_PTR> m_vectorRing3Drivers;
	list<SERVICE_KEY> m_vectorServiceKeys;
	CListDrivers m_clsDriver;
	CRegistry m_Registry;
	

public:
	void ListDrivers();
	CString GetServiceKeyImagePathValue(CString szKey);
	void InsertDriverIntem();
	void Ring3GetDriverModules();
	void GetServiceKeys();
	CString GetServiceName(CString szPath);
	CString GetDriverStartupType(CString szService);
	BOOL GetStartType(CString szKey, DWORD *dwType);
	BOOL IsHideDriver(ULONG_PTR Base);
	
	VOID AddItem(DRIVER_INFO itor);
	afx_msg void OnRclickDriverList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnProcessdlgRefresh();
	afx_msg void OnUnloadDriver();
};
