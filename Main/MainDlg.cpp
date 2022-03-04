
// MainDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Main.h"
#include "MainDlg.h"
#include "afxdialogex.h"
#include "CLoadDriver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CString g_drivePath = L"C:\\Users\\1\\Desktop\\anti.sys";
CString g_driveName = L"anti.sys";





CLoadDriver g_connectDriver;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMainDlg 对话框



CMainDlg::CMainDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAIN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAIN_TAB, m_mainTab);
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
ON_WM_DESTROY()

ON_WM_SIZE()
ON_WM_CLOSE()
END_MESSAGE_MAP()


// CMainDlg 消息处理程序

BOOL CMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	GetClientRect(&m_rect);  //获取对话框的大小
	old.x = m_rect.right - m_rect.left;
	old.y = m_rect.bottom - m_rect.top;

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);

			
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//设置 对话框 大小

	//反截屏
	//BOOL bRet = SetWindowDisplayAffinity(AfxGetMainWnd()->m_hWnd, 0x11);
	
	//加载驱动
	
	WCHAR szPath[MAX_PATH] = { 0 };
	CString szTemp;
	GetCurrentDirectory(MAX_PATH, szPath);

	_tcscat_s(szPath, L"\\anti.sys");
	
	szTemp = szPath;

	if (!g_connectDriver.Load(szTemp, g_driveName)) {
		AfxMessageBox(L"初始化失败");
		exit(100);
	}



	//初始化 Table空间
	m_mainTab.AddPage(L"进程", &m_processDlg, IDD_PROCESS_DIALOG);
	m_mainTab.AddPage(L"内核模块", &m_driverDlg, IDD_DRIVER_DIALOG);
	m_mainTab.AddPage(L"系统回调", &m_callBackDlg, IDD_CALLBACK_DIALOG);
	m_mainTab.AddPage(L"应用层钩子", &m_hookDlg, IDD_HOOK_DIALOG);
	m_mainTab.AddPage(L"拓展功能", &m_externDlg, IDD_EXTERN_DIALOG);
	m_mainTab.Show();





	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMainDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CMainDlg::OnDestroy()
{

	g_connectDriver.UnLoad(g_driveName);

	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}



void CMainDlg::ChangeSize(UINT nID, int x, int y)
{
	CWnd *pWnd = GetDlgItem(nID);
	if (pWnd != NULL)
	{
		CRect rec;
		pWnd->GetWindowRect(&rec);//获取控件变化前的大小
		ScreenToClient(rec);   //将控件大小装换位在对话框中的区域坐标
		// x / m_rect.Width(); 伸缩rate
		rec.left = rec.left * x / m_rect.Width();
		rec.top = rec.top * y / m_rect.Height();
		rec.bottom = rec.bottom  * y / m_rect.Height();
		rec.right = rec.right * x / m_rect.Width();
		pWnd->MoveWindow(rec);//伸缩控件
	}
}

void CMainDlg::ReSize(void)
{
	float fsp[2];
	POINT Newp; //获取现在对话框的大小  
	CRect recta;
	GetClientRect(&recta);     //取客户区大小    
	Newp.x = recta.right - recta.left;
	Newp.y = recta.bottom - recta.top;
	fsp[0] = (float)Newp.x / old.x;
	fsp[1] = (float)Newp.y / old.y;
	CRect Rect;
	int woc;
	CPoint OldTLPoint, TLPoint; //左上角  
	CPoint OldBRPoint, BRPoint; //右下角  
	HWND  hwndChild = ::GetWindow(m_hWnd, GW_CHILD);  //列出所有控件    
	while (hwndChild) {
		if (m_mainTab.m_hWnd == hwndChild) {
			int a = 0;
		}

		woc = ::GetDlgCtrlID(hwndChild);//取得ID  
		GetDlgItem(woc)->GetWindowRect(Rect);
		ScreenToClient(Rect);
		OldTLPoint = Rect.TopLeft();
		TLPoint.x = long(OldTLPoint.x*fsp[0]);
		TLPoint.y = long(OldTLPoint.y*fsp[1]);
		OldBRPoint = Rect.BottomRight();
		BRPoint.x = long(OldBRPoint.x *fsp[0]);
		BRPoint.y = long(OldBRPoint.y *fsp[1]);
		Rect.SetRect(TLPoint, BRPoint);
		GetDlgItem(woc)->MoveWindow(Rect, TRUE);
		hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
	}
	old = Newp;
}
void CMainDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (nType != SIZE_MINIMIZED)  //判断窗口是不是最小化了，因为窗口最小化之后 ，窗口的长和宽会变成0，当前一次变化的时就会出现除以0的错误操作
	{
		ReSize();
	}
}


void CMainDlg::OnClose()
{
	if (m_hookDlg.m_hThread)
	{
		
		int a = 0;
		WaitForSingleObject(m_hookDlg.m_hThread, INFINITE);
		CloseHandle(m_hookDlg.m_hThread);
		m_hookDlg.m_hThread = NULL;
	}

	CDialogEx::OnClose();
}
