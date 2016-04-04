
// mfcClient1Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "mfcClient1.h"
#include "mfcClient1Dlg.h"

CString myDIR;			//�����ļ��У�λ����ʱĿ¼
extern CString TYPE[30];

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

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
{}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CmfcClient1Dlg �Ի���


CmfcClient1Dlg::~CmfcClient1Dlg()
{
    delete pChatlog;
    Shell_NotifyIcon(NIM_DELETE, &nd);
}

void CmfcClient1Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_Connect, m_ConPC);
    DDX_Control(pDX, IDC_msgTo, m_cbMsgTo);
    DDX_Text(pDX, IDC_DataSend, m_DataSend);
}

#pragma region MessageMap
BEGIN_MESSAGE_MAP(CmfcClient1Dlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_Connect, &CmfcClient1Dlg::OnConnect)
    ON_BN_CLICKED(IDC_Send, &CmfcClient1Dlg::OnSend)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_LogOff, &CmfcClient1Dlg::OnLogoff)
    ON_BN_CLICKED(IDC_Chatlog, &CmfcClient1Dlg::OnChatlog)
    ON_CBN_SELCHANGE(IDC_msgTo, &CmfcClient1Dlg::OnCbnSelChangeMsgTo)
    ON_WM_DROPFILES()
END_MESSAGE_MAP()
#pragma endregion

BOOL CmfcClient1Dlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
        CWnd *pw = CWnd::GetFocus();
        if (pw == GetDlgItem(IDC_DataSend)) {
            OnSend();
        }
        return true;
    }
    return CDialog::PreTranslateMessage(pMsg);
}

// CmfcClient1Dlg ��Ϣ�������

BOOL CmfcClient1Dlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // ��������...���˵�����ӵ�ϵͳ�˵��С�
    // IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL) {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    // ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�ִ�д˲���
    SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
    SetIcon(m_hIcon, FALSE);		// ����Сͼ��
    //TODO:�ڴ˴�����Զ������
    LONG style = ::GetWindowLong(GetSafeHwnd(), GWL_STYLE);
    style &= ~WS_THICKFRAME;//ʹ���ڲ��������ı��С
    ::SetWindowLong(GetSafeHwnd(), GWL_STYLE, style);

    HWND h = ::CreateStatusWindow(WS_CHILD | WS_VISIBLE, "����!", this->m_hWnd, 0);
    ::SendMessage(h, SB_SETBKCOLOR, 1, RGB(0, 120, 200));

    GetDlgItem(IDC_DataReceive)->EnableWindow(0);	//������Щ�ؼ�
    GetDlgItem(IDC_DataSend)->EnableWindow(0);
    GetDlgItem(IDC_Connect)->EnableWindow(0);
    GetDlgItem(IDC_Send)->EnableWindow(0);

    nd.cbSize = sizeof(NOTIFYICONDATA);
    nd.hWnd = m_hWnd;
    nd.uID = IDR_MAINFRAME;
    nd.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nd.uCallbackMessage = 0;
    nd.hIcon = m_hIcon;
    sprintf_s(nd.szTip, "�ͻ��� - ��½");
    Shell_NotifyIcon(NIM_ADD, &nd);
    pDlg = (CmfcClient1Dlg*)theApp.GetMainWnd();
    if (!login()) //�������ȡ�����߹ر���ֱ���˳����Ի���
        OnCancel();
    return 0;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CmfcClient1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    } else {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

void CmfcClient1Dlg::OnPaint()
{
    if (IsIconic()) {
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
    } else {
        CDialogEx::OnPaint();
    }
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CmfcClient1Dlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CmfcClient1Dlg::OnTimer(UINT nIDEvent)
{
    if (nIDEvent == 0) {	//��¼���
        if (KillTimer(0)) {
            pSock->Close();
            SetForegroundWindow();
            MessageBox("��������ʱ�������ԣ�", "��ܰ��ʾ", MB_ICONERROR);
            ShowWindow(SW_HIDE);
            login();
            ShowWindow(SW_SHOW);
        }
    }
    elif(nIDEvent == 1) {	//��������
        if (m_connected)
            pSock->SendMSG(pSock->mymsg.join("", TYPE[I_am_online], pSock->userID, "", ""), 0);
    }
    elif(nIDEvent == 2) {	//�����ļ����
        KillTimer(2);
        if (rf.isRecving()) {
            pSock->SendMSG(pSock->mymsg.join(TYPE[File_Fail], TYPE[AskFileData], pSock->userID, "", fileUser), 0);
            rf.recvEnd();
            pSock->updateEvent("�����ļ�" + rf.getFileName() + "��ʱ", "ϵͳ֪ͨ");
            ::MessageBox(GetSafeHwnd(), "�����ļ���ʱ�����Ժ����ԣ�", "��ܰ��ʾ", 0);
            modifyStatus("�ļ����ճ�ʱ��", 0);
        }
    }
    elif(nIDEvent == 3) {	//�����ļ����Զ������ļ�����
        KillTimer(3);
        readFileEnd = 0;
        CXXFStream fileStr(fileSendName, std::ios::in | std::ios::binary);
        long fileSize = (long)fileStr.getSize(), readSize = 0;
        for (unsigned i = 0; readSize < fileSize; i++) {
            memset(packageData[i], 0, 2 * PACKAGE_SIZE + 1);
            if (fileSize - readSize > PACKAGE_SIZE) {		//���������İ�û�ж�ȡ
                fileStr.readString(packageData[i], PACKAGE_SIZE);
                readSize += PACKAGE_SIZE;
            } else {
                fileStr.readString(packageData[i], fileSize - readSize);
                readSize = fileSize;
            }
        }
        readFileEnd = 1;
        fileStr.close();
        fileSendName = fileSendName.Right(fileSendName.GetLength() - fileSendName.ReverseFind('\\') - 1);
    }
    elif(nIDEvent == 4) {	//�������Ͽ���ÿ��һ��ʱ��������������
        OnConnect();
    }
    CDialog::OnTimer(nIDEvent);
}

void CmfcClient1Dlg::OnConnect()
{
    if (m_connected) {    // ����Ѿ����ӣ���Ͽ�������
        m_connected = false;
        pSock->SendMSG(pSock->mymsg.join("", TYPE[Logout], pSock->userID, "", "", m_pw), 0);
        pSock->Close();
        pSock = NULL;
        m_ConPC.SetWindowText(_T("���ӷ�����"));
        UpdateData(false);
        GetDlgItem(IDC_Connect)->EnableWindow(1);
        GetDlgItem(IDC_DataReceive)->EnableWindow(0);	//������Щ�ؼ�
        GetDlgItem(IDC_DataSend)->EnableWindow(0);
        GetDlgItem(IDC_Send)->EnableWindow(0);
        GetDlgItem(IDC_msgTo)->EnableWindow(0);
        sprintf_s(nd.szTip, "�ͻ��ˣ�%s - ����", (LPCTSTR)m_userID);
        Shell_NotifyIcon(NIM_MODIFY, &nd);
        modifyStatus("�ѶϿ�������");
        KillTimer(1);
        return;
    } else { // δ���ӣ������ӷ�����
        pSock = new CClientSocket(m_userID);
        if (!pSock->Create()) {       //�����׽���
            MessageBox(_T("�����׽���ʧ�ܣ�"), "��ܰ��ʾ");
            return;
        }
    }
    //�ڽ������ݵ�ʱ����ִ����ϵͳ��������socket�������Ŀ���������߳��������
    int nSize = DATA_BUF_SIZE;//���û�������С
    setsockopt(*pSock, SOL_SOCKET, SO_RCVBUF, (char *)&nSize, sizeof(int));
    setsockopt(*pSock, SOL_SOCKET, SO_SNDBUF, (char *)&nSize, sizeof(int));
    /*//��ֹNagle�㷨����ͨ����δȷ�ϵ����ݴ��뻺����ֱ������һ����һ���͵ķ������������������͵�����С���ݰ�����Ŀ��
    bool b_noDely = 1;
    setsockopt( *pSock, SOL_SOCKET, TCP_NODELAY, ( char * )&b_noDely, sizeof( b_noDely ) );*/
    if (!pSock->Connect(m_ip, m_port)) {  //���ӷ�����ʧ��
        if (!firstCon)
            MessageBox("���ӷ�����ʧ�ܣ�" + getLastErrorStr(), "��ܰ��ʾ");
        return;
    } else {
        pSock->SendMSG(pSock->mymsg.join("", TYPE[Login], pSock->userID, "", "", m_pw), 0); //���ӷ�����ʱ������������֤���
        SetTimer(0, 3000, NULL); //���õ�¼��ʱ
        m_ConPC.SetWindowText(_T("�������ӷ�����..."));
        GetDlgItem(IDC_Connect)->EnableWindow(0); //���������ͳ�����δ����Ӧǰ�������Ӱ�ť
    }
    firstCon = 0;
    GetDlgItem(IDC_DataSend)->SetFocus();
}

void CmfcClient1Dlg::OnSend()
{
    if (!m_connected) {
        MessageBox("�������ӷ�����", "��ܰ��ʾ");
        return;                               //δ���ӷ�������ִ��
    }
    UpdateData(true);                         //��ȡ�ؼ�����
    if (m_DataSend != "") {
        if (m_msgTo == m_userID) {
            MessageBox("�벻Ҫ���Լ�������Ϣ", "��ܰ��ʾ");
        } else {
            pSock->SendMSG(pSock->mymsg.join(m_DataSend, TYPE[ChatMsg], pSock->userID, "", m_msgTo));
        }
    } else {
        MessageBox("������������", "��ܰ��ʾ");
    }
    GetDlgItem(IDC_DataSend)->SetFocus();
}

void CmfcClient1Dlg::OnOK()
{
    OnSend();
}

void CmfcClient1Dlg::OnLogoff()
{
    KillTimer(4);
    if (m_connected) {    // ����Ѿ����ӣ���Ͽ�������
        m_connected = false;
        pSock->SendMSG(pSock->mymsg.join("", TYPE[Logout], pSock->userID, "", "", m_pw), 0);
        pSock->Close();
        pSock = NULL;
        GetDlgItem(IDC_DataSend)->SetWindowText("");
    }
    sprintf_s(nd.szTip, "�ͻ��� - ��½");
    Shell_NotifyIcon(NIM_MODIFY, &nd);
    GetDlgItem(IDC_DataReceive)->SetWindowText("");
    ShowWindow(SW_HIDE);
    if (!login())
        OnCancel();
    ShowWindow(SW_SHOW);
}

void CmfcClient1Dlg::OnChatlog()
{
    delete pChatlog;
    pChatlog = new CChatLogDlg(m_userID);
    pChatlog->Create(IDD_ChatLog);//����һ����ģ̬�Ի���
    pChatlog->ShowWindow(SW_SHOWNORMAL); //��ʾ��ģ̬�Ի���
    int lastLine = pChatlog->p_editCL->LineIndex(pChatlog->p_editCL->GetLineCount() - 1);
    pChatlog->p_editCL->SetSel(lastLine + 1, lastLine + 2, 0);	//ѡ��༭�����һ��
    pChatlog->p_editCL->ReplaceSel(pChatlog->chatLog);   //�滻��ѡ��һ�е�����
}

void CmfcClient1Dlg::OnCbnSelChangeMsgTo()
{
    m_cbMsgTo.GetLBText(m_cbMsgTo.GetCurSel(), m_msgTo);	//��ȡ��Ϣ���͵�Ŀ���û�
    to_isOnline = 1;
    if (m_msgTo == "������" || m_msgTo == "����������")
        modifyStatus("�������[" + m_msgTo + "]���ɷ�����Ϣ");
    else
        pSock->SendMSG(pSock->mymsg.join(m_msgTo, TYPE[OnlineState], pSock->userID), 0);
}

void CmfcClient1Dlg::OnDropFiles(HDROP hDropInfo)
{
    SetForegroundWindow();		//���ô����ö���ʾ
    if (!m_connected) {
        MBox("�����ѶϿ����������ӷ�������");
        return;
    }
    if (rf.isRecving()) {
        MBox("��ǰ���ڽ����ļ���Ϊ�˲�Ӱ�����ݽ��գ���ȴ���ǰ������ɺ��ٷ����ļ���");
        return;
    }
    // ��ȡ�ļ��������ļ�����������ת����ĳ���û�
    int  nFileCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 256);   //��ȡ������ļ�����
    char filepath[1024] = "";
    DragQueryFile(hDropInfo, 0, filepath, 1024);	// ��ȡ�Ϸŵ�1���ļ��������ļ���
    do {
        if (GetFileAttributes(filepath) != FILE_ATTRIBUTE_DIRECTORY) {
            CFile fileSend(filepath, CFile::modeRead);
            long size = (long)fileSend.GetLength();
            fileSend.Close();
            if (size <= 0) {
                MBox("�ļ�Ϊ�գ����飡");
                break;
            }
            elif(size > MAX_PACKAGE_NUM*PACKAGE_SIZE) {
                CString s;
                s.Format("��ֻ֧�ִ��� %dMB ���ڵ��ļ���", MAX_PACKAGE_NUM*PACKAGE_SIZE / 1024 / 1024);
                MBox(s);
                break;
            }
            if (to_isOnline != 1) {
                MBox("�Է������ߣ���ʱ�޷����������ļ�������Է���������Ϣ������ϵ");
                break;
            }
            save_SendFileInfo(filepath, size);
        } else
            MBox("����Ĳ���һ����Ч�ļ�");
    } while (0);
    DragFinish(hDropInfo);
    CDialogEx::OnDropFiles(hDropInfo);
}



#include "md5.h"
bool ClientInfo::login()
{
    logining = 1;
    CLoginDlg loginDlg;		//���ӵ����Ի���
    // �����û����������Ƿ���Ч
    if (loginDlg.DoModal() != IDOK)
        return 0;
    m_userID = loginDlg.userID;
    m_pw = loginDlg.pw;
    m_ip = loginDlg.ip;
    m_port = loginDlg.port;
    pDlg->SetWindowText("�ͻ��� - " + m_userID);
    pDlg->OnConnect();
    logining = 0;
    return 1;
}

void ClientInfo::modifyStatus(CString sta, bool _sleep)	//�޸�״̬��
{
    HWND h = ::CreateStatusWindow(WS_CHILD | WS_VISIBLE, sta, pDlg->m_hWnd, 0);
    if (_sleep)
        Sleep(10 * _sleep);
    ::SendMessage(h, SB_SETBKCOLOR, 1, RGB(0, 125, 205));
}

void ClientInfo::save_SendFileInfo(const char* filepath, long size)
{
    if (rf.isRecving()) {
        pDlg->MBox("��ǰ���ڽ����ļ���Ϊ�˲�Ӱ�����ݽ��գ���ȴ���ǰ������ɺ��ٷ����ļ���");
        return;
    }
    fileSendName = filepath;
    CString name = fileSendName, s;
    pDlg->SetTimer(3, 1, 0);	//���ú�̨�̶߳�ȡ�ļ�����
    name = name.Right(name.GetLength() - name.ReverseFind('\\') - 1);
    static MD5 md5;
    char szMD5[33] = "";
    md5.fileMd5(szMD5, filepath);
    s.Format("%s|%d|%s", name, size, szMD5);
    pSock->SendMSG(pSock->mymsg.join(s, TYPE[FileSend], pSock->userID, "", m_msgTo), 0);
    rf.setPackNum((size + PACKAGE_SIZE - 1) / PACKAGE_SIZE);
}
