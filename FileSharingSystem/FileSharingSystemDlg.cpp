
// FileSharingSystemDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FileSharingSystem.h"
#include "FileSharingSystemDlg.h"
#include "afxdialogex.h"
#include "FileHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CMD_LSDIR "CMD_LSDIR"
#define RETURN_DIR "RETURN_DIR"
#define CMD_ROOTDIR "CMD_ROOTDIR"
#define CMD_DOWNFILE "CMD_DOWNFILE"
#define RETURN_FILE "RETURN_FILE"
#define CMD_SEARCH "CMD_SEARCH"
#define RETURN_SEARCH "RETURN_SEARCH"

#define BUFSIZE 1024 * 1024 * 50
#define BLOCKBUFSIZE 1024 * 50

SOCKET CFileSharingSystemDlg::m_socket;
SOCKADDR_IN CFileSharingSystemDlg::m_addrFrom;
CString CFileSharingSystemDlg::currentFileName;
CString CFileSharingSystemDlg::beRequestFilePath;

#define LOCALPORT 64444
#define REMOTEPORT 64444
#define TESTIP "127.0.0.1"

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���


HHOOK g_HookHwnd = NULL ;
// WH_KEYBOARD_LL������Ҫ��ʵ�����ӳ���ȥ
// �����Լ���MSDN��
HINSTANCE hInst;

CWnd * CFileSharingSystemDlg::mainWnd;
bool CFileSharingSystemDlg::isVisible;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFileSharingSystemDlg �Ի���




CFileSharingSystemDlg::CFileSharingSystemDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFileSharingSystemDlg::IDD, pParent)
	, fileToBeSearched(_T(""))
	, dwIp(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_socket = 0;

	isVisible = false;
	mainWnd = this;

	// ��װ����
	g_HookHwnd = SetWindowsHookEx(WH_KEYBOARD_LL, MyHookFun, hInst, 0);
	// ����.ͬ��
	ASSERT(g_HookHwnd);
}

CFileSharingSystemDlg::~CFileSharingSystemDlg()
{
	if(m_socket)
		closesocket(m_socket);

	// ��������SetWindowsHookEx���ص�ID
	bool bLresult = UnhookWindowsHookEx(g_HookHwnd);
	// ASSERT����
	// ��expΪFalse,���������
	// ��Ҫafx.h֧��
	ASSERT(bLresult);
}

void CFileSharingSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_yzTreeCtrl);
	DDX_Text(pDX, IDC_EDIT1, fileToBeSearched);
	DDX_Control(pDX, IDC_LIST_FOUNDEDFILES, m_yzListBox);
	DDX_IPAddress(pDX, IDC_IPADDRESS1, dwIp);
}

BEGIN_MESSAGE_MAP(CFileSharingSystemDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CFileSharingSystemDlg::OnSelchangedTree1)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CFileSharingSystemDlg::OnBnClickedButtonSearch)
	ON_LBN_SELCHANGE(IDC_LIST_FOUNDEDFILES, &CFileSharingSystemDlg::OnSelchangeListFoundedfiles)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CFileSharingSystemDlg::OnBnClickedButtonConnect)
	ON_MESSAGE(UM_SOCK, OnSock)
	ON_BN_CLICKED(IDC_BUTTON_DOWNFILE, &CFileSharingSystemDlg::OnBnClickedButtonDownfile)
END_MESSAGE_MAP()


// CFileSharingSystemDlg ��Ϣ�������

BOOL CFileSharingSystemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog(); 

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_yzListBox.SetHorizontalExtent(1000); //ListBoxˮƽ������Χ
	((CIPAddressCtrl *)GetDlgItem(IDC_IPADDRESS1))->SetAddress(htonl(inet_addr(TESTIP)));
	InitSocket();

	ModifyStyle(WS_THICKFRAME, 0, SWP_NOSIZE);	//�̶��Ի���Ĵ�С
	AfxGetMainWnd()->SetWindowText(_T("�ļ�����ϵͳ"));
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CFileSharingSystemDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFileSharingSystemDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CFileSharingSystemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// �õ����������̷��������̷�����������Ϊ�̷����飬��ȡ��һ���̷�����Ҫ+4
int CFileSharingSystemDlg::getLocalDriveArray(char * driveArr)
{
	int i = 0;
	GetLogicalDriveStringsA(MAX_PATH, driveArr);

	while (driveArr[i-1] != '\0' || driveArr[i] != '\0')
	{
		i += 4;
	}

	return i / 4;
}


void CFileSharingSystemDlg::OnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
	m_yzListBox.SetCurSel(-1);

	// ��ȡ��ǰѡ�нڵ�ľ��
	HTREEITEM hItem = m_yzTreeCtrl.GetSelectedItem();
	// Delete all of the children of hmyItem.
	if (m_yzTreeCtrl.ItemHasChildren(hItem))
	{
		HTREEITEM hNextItem;
		HTREEITEM hChildItem = m_yzTreeCtrl.GetChildItem(hItem);

		while (hChildItem != NULL)
		{
			hNextItem = m_yzTreeCtrl.GetNextItem(hChildItem, TVGN_NEXT);
			m_yzTreeCtrl.DeleteItem(hChildItem);
			hChildItem = hNextItem;
		}
	}

	//���ϻ��ݵ�����·��
	CString selectedItemPath = getSelectedItemPath(hItem);
	CString delimiter = "|$$|";
	sendData(CMD_LSDIR + delimiter + selectedItemPath + delimiter);
}


// չʾ��ǰѡ�еĽ���µ��ļ�
void CFileSharingSystemDlg::ShowFile(CString str_Dir, HTREEITEM tree_Root)
{
	CFileFind FileFind;

    //��ʱ���������Լ�¼���ص����ڵ�
    HTREEITEM tree_Temp;

    //�ж�����Ŀ¼����Ƿ����'\'���������򲹳�
    if (str_Dir.Right(1) != "\\")
        str_Dir += "\\";
    str_Dir += "*.*";
    BOOL res = FileFind.FindFile(str_Dir);
    while (res)
    {
        tree_Temp = tree_Root;
        res = FileFind.FindNextFile();
        if (FileFind.IsDirectory() && !FileFind.IsDots())//Ŀ¼���ļ���
        {
            CString strPath = FileFind.GetFilePath(); //�õ�·������Ϊ�ݹ���õĿ�ʼ
            CString strTitle = FileFind.GetFileName();//�õ�Ŀ¼������Ϊ���صĽ��
			tree_Temp = m_yzTreeCtrl.InsertItem(strTitle, 0, 0, tree_Root);
            //ShowFile(strPath, tree_Temp);
        }
        else if (!FileFind.IsDirectory() && !FileFind.IsDots())//������ļ�
        {
            CString strPath = FileFind.GetFilePath(); //�õ�·������Ϊ�ݹ���õĿ�ʼ
            CString strTitle = FileFind.GetFileName();//�õ��ļ�������Ϊ���صĽ��
            m_yzTreeCtrl.InsertItem(strTitle, 0, 0, tree_Temp);
        }
    }
    FileFind.Close();
}

//��Զ�̼��������Ŀ¼����ʱ����ָ��Ŀ¼�µ�ֱ���ļ�
vector<CString> CFileSharingSystemDlg::lsDirFiles(CString str_Dir)
{
	CFileFind FileFind;
	vector<CString> files;

    //�ж�����Ŀ¼����Ƿ����'\'���������򲹳�
    if (str_Dir.Right(1) != "\\")
        str_Dir += "\\";
    str_Dir += "*.*";
    BOOL res = FileFind.FindFile(str_Dir);
    while (res)
    {
        res = FileFind.FindNextFile();
        if (FileFind.IsDirectory() && !FileFind.IsDots())//Ŀ¼���ļ���
        {
            CString strTitle = FileFind.GetFileName();//�õ�Ŀ¼��
			files.push_back(strTitle);
        }
        else if (!FileFind.IsDirectory() && !FileFind.IsDots())//������ļ�
        {
            CString strTitle = FileFind.GetFileName();//�õ��ļ���
			files.push_back(strTitle);
        }
    }
    FileFind.Close();

	return files;
}

// �õ���ǰѡ�н��ľ���·��
CString CFileSharingSystemDlg::getSelectedItemPath(HTREEITEM selectedItem)
{
	CString currentPath = m_yzTreeCtrl.GetItemText(selectedItem);
	HTREEITEM parent = m_yzTreeCtrl.GetParentItem(selectedItem);
	while(parent){
		CString parentPath = m_yzTreeCtrl.GetItemText(parent);
		if(-1 == parentPath.Find("\\"))
		{
			parentPath += "\\";
		}
		CString newCurrentPath = parentPath + currentPath;
		currentPath = newCurrentPath;
		parent = m_yzTreeCtrl.GetParentItem(parent);
	}

	return currentPath;
}


void CFileSharingSystemDlg::OnBnClickedButtonSearch()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//���ListBox֮ǰ������
	m_yzListBox.ResetContent();
	UpdateData(TRUE);
	CString delimiter = "|$$|";
	sendData(CMD_SEARCH + delimiter + fileToBeSearched + delimiter);
}


void CFileSharingSystemDlg::OnSelchangeListFoundedfiles()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strText;   
    int nCurSel;   
       
	nCurSel = m_yzListBox.GetCurSel();           // ��ȡ��ǰѡ���б���   
    m_yzListBox.GetText(nCurSel, strText);       // ��ȡѡ���б�����ַ���   
}


void CFileSharingSystemDlg::OnBnClickedButtonConnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_yzTreeCtrl.DeleteAllItems();
	CString delimiter = "|$$|";
	sendData(CMD_ROOTDIR + delimiter);
}


BOOL CFileSharingSystemDlg::InitSocket(void)
{
	m_socket = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0, 0);
	if(INVALID_SOCKET == m_socket)
	{
		MessageBox("�����׽���ʧ�ܣ�");
		return FALSE;
	}
	SOCKADDR_IN addrSock;
	addrSock.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSock.sin_family = AF_INET;
	addrSock.sin_port = htons(LOCALPORT);  //����Ĭ�϶˿�
	if(SOCKET_ERROR == bind(m_socket, (SOCKADDR *)&addrSock, sizeof(SOCKADDR)))
	{
		MessageBox("��ʧ�ܣ�");
		return FALSE;
	}
	if(SOCKET_ERROR == WSAAsyncSelect(m_socket, m_hWnd, UM_SOCK, FD_READ))
	{
		MessageBox("ע�������ȡ�¼�ʧ�ܣ�");
		return FALSE;
	}

	return TRUE;
}

LRESULT CFileSharingSystemDlg::OnSock(WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(lParam))
	{
	case FD_READ:
		WSABUF wsabuf;
		wsabuf.buf = (char *)malloc(BLOCKBUFSIZE);
		memset(wsabuf.buf, 0, BLOCKBUFSIZE);
		wsabuf.len = BLOCKBUFSIZE;
		DWORD dwRead;
		DWORD dwFlag = 0;
		SOCKADDR_IN addrFrom;
		int len = sizeof(SOCKADDR);
		CString str;
		if(SOCKET_ERROR == WSARecvFrom(m_socket, &wsabuf, 1, &dwRead, &dwFlag, (SOCKADDR *)&addrFrom, &len, NULL, NULL))
		{
			MessageBox("��������ʧ�ܣ�");
			return -1;
		}
		str = wsabuf.buf;
		vector<CString> splitVector = Split(str, "|$$|");
		
		if(splitVector[0] == CMD_ROOTDIR)
		{
			//���ȵõ����������̷������뵽���ؼ���
			char driveArr[MAX_PATH] = {0};
			int driveCnt = getLocalDriveArray(driveArr);
			CString filesStr;
			for(int i = 0; i < driveCnt * 4; i += 4)
			{
				CString delimiter = "|$$|";
				filesStr += (&driveArr[i] + delimiter);
			}
			CString delimiter = "|$$|";
			sendData(RETURN_DIR + delimiter + filesStr, addrFrom);
		}
		else if(splitVector[0] == CMD_LSDIR)
		{
			vector<CString> files = lsDirFiles(splitVector[1]);
			CString filesStr;
			for(int i = 0; i < files.size(); ++i)
			{
				filesStr += (files[i] + "|$$|");
			}
			CString delimiter = "|$$|";
			sendData(RETURN_DIR + delimiter + filesStr, addrFrom);
		}
		else if(splitVector[0] == RETURN_DIR)
		{
			for(int i = 1; i < splitVector.size() - 1; ++i)
			{
				m_yzTreeCtrl.InsertItem(splitVector[i], 0, 0, m_yzTreeCtrl.GetSelectedItem());
			}
		}
		else if(splitVector[0] == CMD_DOWNFILE)
		{
			m_addrFrom = addrFrom;
			beRequestFilePath = splitVector[1];
			HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sendFile, (LPVOID)(&splitVector[1]), 0, NULL);
			CloseHandle(hThread);
		}
		else if(splitVector[0] == CMD_SEARCH)
		{
			char driveArr[MAX_PATH] = {0};
			int driveCnt = getLocalDriveArray(driveArr);
			FileHelper fileHelper;
			CString foundFiles;
			for(int i = 0; i < driveCnt * 4; i += 4)
			{
				vector<pair<CString, CString>> filePair = fileHelper.FindFilesInDirecotryRecursion(splitVector[1], &driveArr[i], -1);
				for(int i = 0; i < filePair.size(); ++i)
				{
					foundFiles += (filePair[i].second + "|$$|");
				}
			}
			CString delimiter = "|$$|";
			sendData(RETURN_SEARCH + delimiter + foundFiles, addrFrom);
		}
		else if(splitVector[0] == RETURN_SEARCH)
		{
			for(int i = 1; i < splitVector.size() - 1; ++i)
			{
				m_yzListBox.AddString(splitVector[i]);
			}
		}
		else
		{
			FILE *fp = fopen( currentFileName, "a+b");
			fwrite(wsabuf.buf, 1, dwRead, fp);
			fclose(fp);
		}
		free(wsabuf.buf);
		break;
	}
	return 0;
}

// ��Զ����������ָ��·���µ�Ŀ¼����
void CFileSharingSystemDlg::sendData(CString data)
{
	WSABUF wsabuf;
	DWORD dwSend;
	int len;

	UpdateData(TRUE);

	SOCKADDR_IN addrTo;
	addrTo.sin_addr.S_un.S_addr = htonl(dwIp);
	addrTo.sin_family = AF_INET;
	addrTo.sin_port = htons(REMOTEPORT);  //��Ĭ�϶˿ڷ���

	len = data.GetLength();
	wsabuf.buf = data.GetBuffer(len);
	wsabuf.len = len;

	if(SOCKET_ERROR == WSASendTo(m_socket, &wsabuf, 1, &dwSend, 0, (SOCKADDR *)&addrTo, sizeof(SOCKADDR), NULL, NULL))
	{
		MessageBox("��������ʧ�ܣ�");
		return;
	}
}

// �������������������
void CFileSharingSystemDlg::sendData(CString data, SOCKADDR_IN addrTo)
{
	WSABUF wsabuf;
	DWORD dwSend;
	int len;

	len = data.GetLength();
	wsabuf.buf = data.GetBuffer(len);
	wsabuf.len = len;

	if(SOCKET_ERROR == WSASendTo(m_socket, &wsabuf, 1, &dwSend, 0, (SOCKADDR *)&addrTo, sizeof(SOCKADDR), NULL, NULL))
	{
		MessageBox("��������ʧ�ܣ�");
		return;
	}
}

vector<CString> CFileSharingSystemDlg::Split(CString source, CString division)
{
	vector<CString> dest;
	int pos =0;
	int pre_pos =0;
	while( -1!= pos ){
		pos = source.Find(division,(pos+1));
		dest.push_back(source.Mid(pre_pos,(pos-pre_pos)));
		pre_pos = pos + division.GetLength();
	}

	return dest;
}

void CFileSharingSystemDlg::OnBnClickedButtonDownfile()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	HTREEITEM hItem = m_yzTreeCtrl.GetSelectedItem();
	if(hItem != NULL)
	{
		//���ϻ��ݵ�����·��
		CString selectedItemPath = getSelectedItemPath(hItem);

		CString fileName = selectedItemPath.Right(selectedItemPath.GetLength() - selectedItemPath.ReverseFind('\\') - 1);
		currentFileName = fileName;

		CString delimiter = "|$$|";
		sendData(CMD_DOWNFILE + delimiter + selectedItemPath + delimiter);
	}
	else
	{
		int i = m_yzListBox.GetCurSel();
		if(i > -1)
		{
			CString strText; 
			m_yzListBox.GetText(i, strText);
			CString fileName = strText.Right(strText.GetLength() - strText.ReverseFind('\\') - 1);
			currentFileName = fileName;

			CString delimiter = "|$$|";
			sendData(CMD_DOWNFILE + delimiter + strText + delimiter);
		}
	}
}

UINT CFileSharingSystemDlg::sendFile(LPVOID lParam)
{
	const char *fileName = (const char *)lParam;
	FILE *fp = fopen(beRequestFilePath, "rb");
	char *buf = (char *)malloc(BUFSIZE);
	memset(buf, 0, BUFSIZE);
	int len = fread(buf, 1, BUFSIZE, fp);

	int cnt = len / (BLOCKBUFSIZE);
	int lastLen = len % (BLOCKBUFSIZE);
	for(int i = 0; i < cnt; ++i)
	{
		char *blockBuf = buf + i * (BLOCKBUFSIZE);
		WSABUF wsabuf;
		DWORD dwSend;
		wsabuf.buf = blockBuf;
		wsabuf.len = (BLOCKBUFSIZE);

		if(SOCKET_ERROR == WSASendTo(m_socket, &wsabuf, 1, &dwSend, 0, (SOCKADDR *)&m_addrFrom, sizeof(SOCKADDR), NULL, NULL))
		{
			return -1;
		}
		Sleep(500);
	}

	WSABUF wsabuf;
	DWORD dwSend;
	wsabuf.buf = buf + (BLOCKBUFSIZE) * cnt;
	wsabuf.len = lastLen;
	if(SOCKET_ERROR == WSASendTo(m_socket, &wsabuf, 1, &dwSend, 0, (SOCKADDR *)&m_addrFrom, sizeof(SOCKADDR), NULL, NULL))
	{
		return -1;
	}

	free(buf);
	fclose(fp);
}

LRESULT CALLBACK CFileSharingSystemDlg::MyHookFun(int nCode, WPARAM wParam, LPARAM lParam)
{
	// ���Structure�����˼��̵���Ϣ
	/*typedef struct {
	DWORD vkCode;
	DWORD scanCode;
	DWORD flags;
	DWORD time;
	ULONG_PTR dwExtraInfo;
	} KBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT;*/
	// ����ֻ��Ҫ�Ǹ�vkCode
	PKBDLLHOOKSTRUCT pVirKey = (PKBDLLHOOKSTRUCT)lParam;

	// MSDN˵��,nCode < 0��ʱ�����
	if (nCode >= 0)
	{
		// ������Ϣ
		switch(wParam) 
		{
		case WM_KEYDOWN:
			break;
		case WM_SYSKEYDOWN:
			if(pVirKey->vkCode == VK_F12)
			{
				isVisible = !isVisible;
				if(isVisible)
				{
					mainWnd->ShowWindow(SW_SHOW);
				}
				else
				{
					mainWnd->ShowWindow(SW_HIDE);
				}
			}
			break;
		case WM_KEYUP:
			//case WM_SYSKEYUP:
			//	switch(pVirKey->vkCode) 
			//	{
			//	case VK_LWIN:
			//	case VK_RWIN:
			//		return 1;  // �Ե���Ϣ
			//		break;
			//	}
			break;
		}
	}

	return CallNextHookEx(g_HookHwnd, nCode, wParam, lParam);
}
