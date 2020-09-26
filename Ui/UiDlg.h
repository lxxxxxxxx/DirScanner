
// UiDlg.h: 头文件
//

#pragma once

#include "DirScanner.h"


// CUiDlg 对话框
class CUiDlg : public CDialogEx,public SubscriberBase
{
// 构造
public:
	CUiDlg(CWnd* pParent = nullptr);	// 标准构造函数
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	void OnScanOneStart(ScanInfo info);
	void OnScanOneFinish(ScanInfo info);
	void OnScanAllFinish(ScanInfo info);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CDirScanner m_dirScanner;
public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnEnChangeMfceditbrowse2();
	CRichEditCtrl m_richedit;
	afx_msg void OnBnClickedButton4();
};
