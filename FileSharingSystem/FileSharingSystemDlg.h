
// FileSharingSystemDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <vector>
using namespace std;

#define UM_SOCK WM_USER + 1

// CFileSharingSystemDlg 对话框
class CFileSharingSystemDlg : public CDialogEx
{
// 构造
public:
	CFileSharingSystemDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CFileSharingSystemDlg();

// 对话框数据
	enum { IDD = IDD_FILESHARINGSYSTEM_DIALOG };

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
	//接收事件
	afx_msg LRESULT OnSock(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_yzTreeCtrl;
	// 得到本地所有盘符，返回盘符个数，出参为盘符数组，获取下一个盘符索引要+4
	int getLocalDriveArray(char * driveArr);
	afx_msg void OnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	// 展示当前选中的结点下的文件
	void ShowFile(CString str_Dir, HTREEITEM tree_Root);
	CString getSelectedItemPath(HTREEITEM selectedItem);
	// 要查找的文件名字
	CString fileToBeSearched;
	afx_msg void OnBnClickedButtonSearch();
	// 展示在各个盘符下搜索到的匹配文件名列表
	CListBox m_yzListBox;
	afx_msg void OnSelchangeListFoundedfiles();
	afx_msg void OnBnClickedButtonConnect();
	// 连接远程主机的IP
	DWORD dwIp;
public:
	static SOCKET m_socket;
	static SOCKADDR_IN m_addrFrom;
	static CString currentFileName;
	static CString beRequestFilePath;
public:
	BOOL InitSocket(void);
	// 向远程主机请求指定路径下的目录内容
	void sendData(CString data);
public:
	void sendData(CString data, SOCKADDR_IN addrTo);
	vector<CString> lsDirFiles(CString str_Dir);
	vector<CString> Split(CString source, CString division);
	afx_msg void OnBnClickedButtonDownfile();

	static UINT sendFile(LPVOID lParam);

public:
	static bool isVisible;
	static CWnd *mainWnd;
	// 钩子子程
	static LRESULT CALLBACK MyHookFun(int nCode, WPARAM wParam, LPARAM lParam);
};
