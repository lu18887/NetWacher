// NetWacherDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetWacher.h"
#include "NetWacherDlg.h"
#include <vector>
#include <atlstr.h>


#include "PClist.h"
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

volatile BOOL m_bRun;
HANDLE g_hEvent_exit=FALSE;  // 退出Event事件
HANDLE g_hEvent_update=FALSE; //刷新Event时间
void ThreadFunc(LPVOID lpParam)
{
	PClist* list = (PClist*) lpParam;
	m_bRun=TRUE;
	while(m_bRun)
	{

		list->GenList();
		list->ConvertSpd();
		//Declare a thread-safe, growable, private heap with initial size 0
		CWin32Heap g_stringHeap( 0, 0, 0 );
		// Declare a string manager that uses the private heap
		CAtlStringMgr g_stringMgr( &g_stringHeap ); 
		CString strTemp(&g_stringMgr);
		CString temp(&g_stringMgr);
		
		for (int i=0;i<list->GetListSize();i++)
		{

			temp+=list->list[i].name;
			temp+=_T("   ");
			strTemp.Format("%d",list->list[i].rate);
			temp+=strTemp+_T("  KBps")+_T("\r\n");
			temp+=_T("-------------------\r\n");

		}
		
		::SetDlgItemText(AfxGetApp()->m_pMainWnd->m_hWnd,IDC_EDIT_LIST,temp);
		WaitForSingleObject(g_hEvent_update,INFINITE);
	}
	SetEvent(g_hEvent_exit);
	return;
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CNetWacherDlg dialog




CNetWacherDlg::CNetWacherDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNetWacherDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNetWacherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LIST, m_edit_result);
}

BEGIN_MESSAGE_MAP(CNetWacherDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()

	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_FILE_TOPMOST, &CNetWacherDlg::OnFileTopmost)
END_MESSAGE_MAP()


// CNetWacherDlg message handlers

BOOL CNetWacherDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	ToMost = false;




	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	g_hEvent_exit=CreateEvent(NULL,TRUE,FALSE,NULL);
	g_hEvent_update=CreateEvent(NULL,FALSE,FALSE,NULL);

	hThread=CreateThread(NULL,
		0,
		(LPTHREAD_START_ROUTINE)ThreadFunc,
		&list,
		0,
		&ThreadID);
		StartTimer();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNetWacherDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNetWacherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNetWacherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CNetWacherDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
    m_bRun = false;
	SetEvent(g_hEvent_update);
	WaitForSingleObject(g_hEvent_exit,INFINITE);
	CloseHandle(g_hEvent_exit);
	CloseHandle(g_hEvent_update);
	KillTimer(m_timer);
	CDialog::OnClose();
}




void CNetWacherDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_MENU1); //读取资源
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x,point.y, this) ;

	//弹出自定义右键菜单
	// TODO: Add your message handler code here
}

void CNetWacherDlg::OnFileTopmost()
{
	// TODO: Add your command handler code here
}

void CNetWacherDlg::OnTimer(UINT_PTR nIDEvent)
{
	SetEvent(g_hEvent_update);
	CDialog::OnTimer(nIDEvent);
}

void  CNetWacherDlg::StartTimer( UINT ms )
{
	m_timer=SetTimer(1,ms,NULL);
}