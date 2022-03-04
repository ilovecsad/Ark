#pragma once
#include "SortListCtrl.h"
#include "ListModules.h"
// CListProcessHook 对话框

#include "Includes/BeFunc.h"
#include "capstone/capstone.h"
#pragma (lib,"capstone/capstone.lib")

typedef struct _DUMP_INFO_
{
	CString szPath;
	ULONG_PTR nBase;
	ULONG_PTR nSize;
	PVOID pSectionBuffer;  //PE来的
	PVOID pReloadBuffer;    //内存 copy来的 
	BOOL bFixed;
	BOOL bIs64;
}DUMP_INFO, *PDUMP_INFO;

typedef struct _BOUNT_IMPORT_INFO
{
	CString szModule;
	CString szRefModule;
	ULONG_PTR dwRefTimeDateStamp;
	ULONG_PTR dwRefBase;
	ULONG_PTR dwRefSize;
}BOUND_IMPORT_INFO, *PBOUNT_IMPORT_INFO;


typedef struct _IAT_HOOK_INFO
{
	CString szHookedModule; // 被hook的模块名
	CString szExpModule;	// 导出函数的模块
	CString szFunction;		// 函数名
	ULONG_PTR dwHookAddress;	// hook后的函数地址
	ULONG_PTR dwOriginAddress;	// 原始函数地址
	ULONG_PTR dwIatAddress;
}IAT_HOOK_INFO, *PIAT_HOOK_INFO;

typedef struct _INLINE_HOOK_INFO
{
	ULONG_PTR dwBase;		// 模块基地址
	ULONG_PTR dwHookOffset;	// hook后的函数偏移
	ULONG_PTR dwLen;		// hook长度
	ULONG_PTR dwReload;
}INLINE_HOOK_INFO, *PINLINE_HOOK_INFO;


typedef struct _COMPARE_DATA_
{
	ULONG_PTR startAddress;		// 保存的Buffer的地址
	ULONG_PTR EndAddress;	// 结束地址
	ULONG_PTR dwFixBase;    //
	ULONG   Size;	// 大小
	ULONG type;
}COMPARE_DATA, *PCOMPARE_DATA;




class CListProcessHook : public CDialogEx
{
	DECLARE_DYNAMIC(CListProcessHook)

public:
	CListProcessHook(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CListProcessHook();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HOOK_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	list<cs_insn>m_disam;
	list<BOUND_IMPORT_INFO> m_BoundList;
	list<INLINE_HOOK_INFO> m_InlineHookInfo;
	list<IAT_HOOK_INFO> m_IatHookList;
	CString m_szStatus;
	list<DUMP_INFO> m_DumpList;
	CommonFunctions m_Functions;
	CSortListCtrl m_listProcess;
	CSortListCtrl m_listHookInfo;
	vector<MODULE_INFO_EX>m_vectorModules;
	virtual BOOL OnInitDialog();
	VOID ShowAllProcess();
	CListProcess m_clsProcess;
	CListModules m_clsListModules;
	vector<PROCESS_INFO> m_vectorProcess;
	ULONG m_InlineHookCnt;
	void AddProcessItem(PROCESS_INFO item);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnRclickEnumprocessList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnResfresh();
	afx_msg void OnCheckHook();
	HANDLE m_hThread;
	void EnumProcessHooks(PPROCESS_INFO pInfo);
	BOOL m_bIsRun;
	PPROCESS_INFO GetInfoByItem(int nItem);
	PVOID DumpDllModuleSection(ULONG dwPid,ULONG_PTR dwBase, DWORD dwSize);
	PVOID MapFileWithoutFix(CString szPath);
	BOOL ImageFile(PVOID FileBuffer, PVOID* ImageModuleBase);
	UINT AlignSize(UINT nSize, UINT nAlign);
	void FixImprtAndReacDir();
	BOOL FixImportTable(BYTE *ImageBase, ULONG_PTR ExistImageBase);
	BOOL GetPeImportTable(BYTE *ImageBase, PIMAGE_IMPORT_DESCRIPTOR *ImageImportDescriptor);
	void GetDllModuleBase(CString szModuleName, ULONG_PTR *dwRet);
	PVOID GetReloadBuffer(ULONG_PTR dwBase);
	PVOID MiFindExportedRoutine(IN PVOID DllBase,BOOL ByName,IN char *RoutineName,DWORD Ordinal);
	BOOL FixBaseRelocTable(PVOID NewImageBase, ULONG_PTR ExistImageBase);
	void EnumIatHook(PVOID pSection, CString szPath, ULONG_PTR nBase);
	VOID GetImageBoundImport(PIMAGE_BOUND_IMPORT_DESCRIPTOR ImageBoundImportDescriptor, list<BOUND_IMPORT_INFO> &m_BoundList);
	void GetDllModuleBaseAndSize(CString szModuleName, ULONG_PTR *dwBase, ULONG_PTR *dwSize);
	ULONG_PTR GetImportFuncOffset(PVOID ImageBase, CHAR *szFunc);
	ULONG_PTR FindExportedRoutineInReloadModule(IN ULONG_PTR DllBase, IN char *RoutineName);
	BOOL IsBoundImport(CString szModule, ULONG_PTR dwFunction, list<BOUND_IMPORT_INFO> BoundList);
	void EnumInlineHook(PVOID pSection, PVOID pReload, ULONG_PTR dwBase);
	ULONG_PTR PassIATAddress(PVOID pSection, ULONG_PTR pVA, ULONG nSize);
	LRESULT OnAddItemBySendMessage(WPARAM wParam, LPARAM lParam);
	void ShowScanResult();
	PVOID GetOriginBuffer(ULONG_PTR dwBase);
	CString GetModuleImageName(ULONG_PTR dwBase);
	CString GetFunctionName(PVOID pSection, DWORD dwOffset);
	ULONG_PTR GetInlineHookAddress(PVOID pBuffer, ULONG_PTR dwBase, ULONG dwHookOffset, ULONG nCodeLen);
	CString GetDllModulePath(ULONG_PTR dwRet);
	BOOL m_bIs64;
	void EnumInlineHookEx(PVOID pSection, PVOID pReload, ULONG_PTR dwBase);
	VOID DisasmCode(PCOMPARE_DATA pData);
	afx_msg void OnExportExcel();
	afx_msg void OnRclickProcesshook2List(NMHDR *pNMHDR, LRESULT *pResult);
	void EnumInlineHook32(PVOID pSectionFromMemory, PVOID pReloadFromPe, ULONG_PTR dwBase);
	BOOLEAN IsHookCode(CString szTemp);
	VOID clearWork();
	BOOL ListProcessModules(DWORD dwPID);
};


#define  PANSI_STRING CHAR*
typedef PVOID (WINAPI* pfnImageDirectoryEntryToData)(
	_In_ PVOID Base,
	_In_ BOOLEAN MappedAsImage,
	_In_ USHORT DirectoryEntry,
	_Out_ PULONG Size
);

typedef PVOID (WINAPI* pfnRtlFindExportedRoutineByName)(
	IN PVOID DllBase,
	IN PANSI_STRING AnsiImageRoutineName
);
typedef  PIMAGE_BASE_RELOCATION(WINAPI* pfnLdrProcessRelocationBlock)(
	IN ULONG_PTR VA,
	IN ULONG SizeOfBlock,
	IN PUSHORT NextOffset,
	IN LONGLONG Diff
	);

typedef PIMAGE_NT_HEADERS (WINAPI* pfnRtlImageNtHeader)(PVOID Base);