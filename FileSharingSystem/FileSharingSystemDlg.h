
// FileSharingSystemDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <vector>
using namespace std;

#define UM_SOCK WM_USER + 1

// CFileSharingSystemDlg �Ի���
class CFileSharingSystemDlg : public CDialogEx
{
// ����
public:
	CFileSharingSystemDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CFileSharingSystemDlg();

// �Ի�������
	enum { IDD = IDD_FILESHARINGSYSTEM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//�����¼�
	afx_msg LRESULT OnSock(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_yzTreeCtrl;
	// �õ����������̷��������̷�����������Ϊ�̷����飬��ȡ��һ���̷�����Ҫ+4
	int getLocalDriveArray(char * driveArr);
	afx_msg void OnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	// չʾ��ǰѡ�еĽ���µ��ļ�
	void ShowFile(CString str_Dir, HTREEITEM tree_Root);
	CString getSelectedItemPath(HTREEITEM selectedItem);
	// Ҫ���ҵ��ļ�����
	CString fileToBeSearched;
	afx_msg void OnBnClickedButtonSearch();
	// չʾ�ڸ����̷�����������ƥ���ļ����б�
	CListBox m_yzListBox;
	afx_msg void OnSelchangeListFoundedfiles();
	afx_msg void OnBnClickedButtonConnect();
	// ����Զ��������IP
	DWORD dwIp;
public:
	static SOCKET m_socket;
	static SOCKADDR_IN m_addrFrom;
	static CString currentFileName;
	static CString beRequestFilePath;
public:
	BOOL InitSocket(void);
	// ��Զ����������ָ��·���µ�Ŀ¼����
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
	// �����ӳ�
	static LRESULT CALLBACK MyHookFun(int nCode, WPARAM wParam, LPARAM lParam);
};
