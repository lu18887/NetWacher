// NetWacherDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include <vector>
#include "PClist.h"
using namespace std;


// CNetWacherDlg dialog
class CNetWacherDlg : public CDialog
{
// Construction
public:
	CNetWacherDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_NETWACHER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	PClist list;
	bool ToMost;
	UINT_PTR m_timer;

public:
	CEdit m_edit_result;
	HANDLE hThread; 
	DWORD ThreadID;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnFileTopmost();
	void StartTimer(UINT ms=5000);
};
