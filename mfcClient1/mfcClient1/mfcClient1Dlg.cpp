
// mfcClient1Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "mfcClient1.h"
#include "mfcClient1Dlg.h"

#include "CXXFStream.hpp"
#include "md5.h"

using namespace std;

char packageData[MAX_PACKAGE_NUM][2 * PACKAGE_SIZE + 1]; //�ļ�������

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//------------------------- CAboutDlg���ڶԻ����� ---------------------------//
class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg() : CDialogEx(CAboutDlg::IDD)
    {}

    enum {
        IDD = IDD_ABOUTBOX
    };
};


//---RecvFile���������Ա���� :����������ʹ����theApp,��˲��ܷ�����������---//
void RecvFile::addPacketage(const char *data)
{
    static CmfcClient1Dlg* pDlg = (CmfcClient1Dlg*)theApp.GetMainWnd();
    if (packageRecv < packageNum) {
        packageRecv++;
        if (packageRecv < packageNum)		//�������ݰ�û�н���
            pDlg->sendMSG(pDlg->mymsg.join(getPackRecv(), TYPE[AskFileData], "", "", pDlg->m_fileUser), 0);//������һ�����ݰ�
        strcpy_s(packageData[packageRecv - 1], data);
        static int showTransLog;//��ʾ������־
        GET_WRITE(showTransLog, "showTransLog", "0");
        if (showTransLog) {
            CString str;
            str.Format("���յ�%d�����ݰ�,packageNum=%d", packageRecv, packageNum);
            pDlg->updateEvent(str, "ϵͳ֪ͨ");
        }
    }
    static long timeNow = timeStart;
    if (pDlg != 0) {
        if (clock() - timeNow > 400) {
            timeNow = clock();
            CString str;
            str.Format("�ļ��ѽ��� %.1f%%��    ��ʱ %.1fs   ƽ���ٶ� %.1fk/s", 100.0 * packageRecv / packageNum
                       , (clock() - timeStart) / 1000.0, 1.0*packageRecv / packageNum*fileLength / ((clock() - timeStart)));
            pDlg->modifyStatus(str, 0);
        }
    }
    if (packageNum == packageRecv)
        saveFile(clock() - timeStart);
}

void RecvFile::saveFile(int useTime)
{
    static CmfcClient1Dlg* pDlg = (CmfcClient1Dlg*)theApp.GetMainWnd();
    static HWND hWnd = pDlg->GetSafeHwnd();
    if (packageNum == packageRecv) {
        pDlg->modifyStatus("����У���ļ����ݣ�", 0);
        CXXFStream out(fileNewName, ios::out | ios::binary);
        for (int i = 0; i < packageRecv - 1; i++) {
            out.writeString(packageData[i], PACKAGE_SIZE);
        }
        if (fileLength % PACKAGE_SIZE == 0)	//���һ����������
            out.writeString(packageData[packageNum - 1], PACKAGE_SIZE);
        else								//���һ��û�������
            out.writeString(packageData[packageNum - 1], fileLength % PACKAGE_SIZE);
        out.close();
        static MD5 md5;
        char newFileMD5[33] = "";
        md5.fileMd5(newFileMD5, (LPCTSTR)fileNewName);
        if (fileMD5 == CString(newFileMD5)) {
            pDlg->sendMSG(pDlg->mymsg.join(TYPE[File_Over], TYPE[AskFileData], "", "", pDlg->m_fileUser), 0);
            timeMsg.Format("    ��ʱ %.1fs   ƽ���ٶ� %.1fk/s", useTime / 1000.0, 1.0*fileLength / useTime);
            pDlg->modifyStatus("�ļ��ѽ�����ϣ�" + timeMsg, 0);
            pDlg->updateEvent("�ļ�\"" + fileNewName + "\"�ѽ�����ϣ�" + timeMsg, "ϵͳ֪ͨ");
            MessageBox(hWnd, "���ղ������ļ��ɹ�", "��ܰ��ʾ", 0);
        }
        else {
            pDlg->sendMSG(pDlg->mymsg.join(TYPE[File_Fail], TYPE[AskFileData], "", "", pDlg->m_fileUser), 0);
            pDlg->updateEvent("���ջ򱣴��ļ�\"" + fileNewName + "\"ʧ�ܣ����Ժ����ԣ���������룺0x00041)", "ϵͳ֪ͨ");
            MessageBox(hWnd, "���ջ򱣴��ļ�ʧ�ܣ����Ժ����ԣ���������룺0x00041)", "��ܰ��ʾ", 0);
            pDlg->modifyStatus("�ļ�����ʧ��", 0);
            DeleteFile(fileNewName);
        }
    }
    recvEnd();
}


//------------------------ ClientInfo��Ĺ��캯�� ---------------------------//
int ClientInfo::userNum = 0;
CString ClientInfo::myDIR;

ClientInfo::ClientInfo() {
    pDlg = 0;
    m_readFileEnd = 0;
    m_connected = 0;
    m_DataSend = m_userID = "";

    char path[2048] = "";
    GetTempPath(2048, path);
    strcat(path, "mfcClient1");
    CreateDirectory(path, 0);
    AfxGetApp()->WriteProfileString("ClientSetting", "tempDir", path);
    myDIR = path + CString("\\");
    DeleteFile(myDIR + "send.txt");
    DeleteFile(myDIR + "TransLog.txt");
}


//------------------- CmfcClient1Dlg ��ĳ�Ա����ʵ�� -----------------------//

CmfcClient1Dlg::CmfcClient1Dlg(CWnd* pParent)
    : CDialogEx(CmfcClient1Dlg::IDD, pParent)
    , pChatlog(NULL)
    , autoConnect(0)
{
}

CmfcClient1Dlg::~CmfcClient1Dlg()
{
    if (m_connected) {
        sendMSG(mymsg.join("", TYPE[Logout], m_userID, "", "", m_pw), 0);
        pSock->Close();
        delete pSock;
    }
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

BEGIN_MESSAGE_MAP(CmfcClient1Dlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_QUERYDRAGICON()
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_WM_DROPFILES()
    ON_BN_CLICKED(IDC_Connect, &CmfcClient1Dlg::OnConnect)
    ON_BN_CLICKED(IDC_Send, &CmfcClient1Dlg::OnSend)
    ON_BN_CLICKED(IDC_LogOff, &CmfcClient1Dlg::OnLogoff)
    ON_BN_CLICKED(IDC_Chatlog, &CmfcClient1Dlg::OnChatlog)
    ON_CBN_SELCHANGE(IDC_msgTo, &CmfcClient1Dlg::OnCbnSelChangeMsgTo)
END_MESSAGE_MAP()

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

    HWND h = CreateStatusWindow(WS_CHILD | WS_VISIBLE, "����!", m_hWnd, 0);
    ::SendMessage(h, SB_SETBKCOLOR, 0, RGB(0, 120, 200));

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
    }
    else {
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
    }
    else {
        CDialogEx::OnPaint();
    }
}

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
            sendMSG(mymsg.join("", TYPE[I_am_online], m_userID, "", ""), 0);
    }
    elif(nIDEvent == 2) {	//�����ļ����
        KillTimer(2);
        if (m_rf.isRecving()) {
            sendMSG(mymsg.join(TYPE[File_Fail], TYPE[AskFileData], m_userID, "", m_fileUser), 0);
            m_rf.recvEnd();
            updateEvent("�����ļ�" + m_rf.getFileName() + "��ʱ", "ϵͳ֪ͨ");
            ::MessageBox(GetSafeHwnd(), "�����ļ���ʱ�����Ժ����ԣ�", "��ܰ��ʾ", 0);
            modifyStatus("�ļ����ճ�ʱ��", 0);
        }
    }
    elif(nIDEvent == 3) {	//�����ļ����Զ������ļ�����
        KillTimer(3);
        m_readFileEnd = 0;
        CXXFStream fileStr(m_fileSendName, ios::in | ios::binary);
        long fileSize = (long)fileStr.getSize(), readSize = 0;
        for (unsigned i = 0; readSize < fileSize; i++) {
            memset(packageData[i], 0, 2 * PACKAGE_SIZE + 1);
            if (fileSize - readSize > PACKAGE_SIZE) {		//���������İ�û�ж�ȡ
                fileStr.readString(packageData[i], PACKAGE_SIZE);
                readSize += PACKAGE_SIZE;
            }
            else {
                fileStr.readString(packageData[i], fileSize - readSize);
                readSize = fileSize;
            }
        }
        m_readFileEnd = 1;
        fileStr.close();
        m_fileSendName = m_fileSendName.Right(m_fileSendName.GetLength() - m_fileSendName.ReverseFind('\\') - 1);
    }
    elif(nIDEvent == 4) {	//�������Ͽ���ÿ��һ��ʱ��������������
        autoConnect = 1;
        OnConnect();
        autoConnect = 0;
    }
    CDialog::OnTimer(nIDEvent);
}

void CmfcClient1Dlg::OnConnect()
{
    if (m_connected) {    // ����Ѿ����ӣ���Ͽ�������
        m_connected = false;
        sendMSG(mymsg.join("", TYPE[Logout], m_userID, "", "", m_pw), 0);
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
    }
    //δ���ӣ���׼�����ӷ�����
    pSock = new CClientSocket(m_userID);
    if (!pSock->Create()) { //�����׽���ʧ��
        MessageBox(_T("�����׽���ʧ�ܣ�"), "��ܰ��ʾ");
        return;
    }
    //�ڽ������ݵ�ʱ����ִ����ϵͳ��������socket�������Ŀ���������߳��������
    int nSize = DATA_BUF_SIZE;//���û�������С
    setsockopt(*pSock, SOL_SOCKET, SO_RCVBUF, (char *)&nSize, sizeof(int));
    setsockopt(*pSock, SOL_SOCKET, SO_SNDBUF, (char *)&nSize, sizeof(int));
    //��ֹNagle�㷨����ͨ����δȷ�ϵ����ݴ��뻺����ֱ������һ����һ���͵ķ������������������͵�����С���ݰ�����Ŀ��
    bool b_noDely = 1;
    setsockopt(*pSock, SOL_SOCKET, TCP_NODELAY, (char *)&b_noDely, sizeof(b_noDely));

    static bool firstCon = 1; //�Ƿ�Ϊ��һ������
    if (pSock->Connect(m_ip, m_port)) {  //���ӷ������ɹ�
        sendMSG(mymsg.join("", TYPE[Login], m_userID, "", "", m_pw), 0); //���ӷ�����ʱ������������֤���
        SetTimer(0, 3000, NULL); //���õ�¼��ʱ
        m_ConPC.SetWindowText(_T("�������ӷ�����..."));
        GetDlgItem(IDC_Connect)->EnableWindow(0); //���������ͳ�����δ����Ӧǰ�������Ӱ�ť
    }
    else {
        if (!firstCon) {
            if (!autoConnect)
                MessageBox("���ӷ�����ʧ�ܣ�" + pSock->getLastErrorStr(), "��ܰ��ʾ", MB_ICONERROR);
        }
        else
            firstCon = 0;
        return;
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
        }
        else {
            sendMSG(mymsg.join(m_DataSend, TYPE[ChatMsg], m_userID, "", m_msgTo));
        }
    }
    else {
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
        sendMSG(mymsg.join("", TYPE[Logout], m_userID, "", "", m_pw), 0);
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
    m_toUserIsOnline = 1;
    if (m_msgTo == "������" || m_msgTo == "����������")
        modifyStatus("�������[" + m_msgTo + "]���ɷ�����Ϣ");
    else
        sendMSG(mymsg.join(m_msgTo, TYPE[OnlineState], m_userID), 0);
}

void CmfcClient1Dlg::OnDropFiles(HDROP hDropInfo)
{
    SetForegroundWindow();		//���ô����ö���ʾ
    if (!m_connected) {
        MBox("�����ѶϿ����������ӷ�������");
        return;
    }
    if (m_rf.isRecving()) {
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
            if (m_toUserIsOnline != 1) {
                MBox("�Է������ߣ���ʱ�޷����������ļ�������Է���������Ϣ������ϵ");
                break;
            }
            save_SendFileInfo(filepath, size);
        }
        else
            MBox("����Ĳ���һ����Ч�ļ�");
    } while (0);
    DragFinish(hDropInfo);
    CDialogEx::OnDropFiles(hDropInfo);
}

void CmfcClient1Dlg::receData()
{
    KillTimer(1);
    KillTimer(4);
    GetDlgItem(IDC_DataSend)->SetFocus();
    char pData[DATA_BUF_SIZE] = ""; //������յ�������
    pSock->Receive(pData, DATA_BUF_SIZE);
    mymsg.load(pData);
    MyMsg msg = mymsg;
    static const CString myLogFile = myDIR + "ClientLog-" + m_userID + ".txt";
    static int del_one_time = DeleteFile(myLogFile);
    static ofstream logFile(myLogFile, ios::out | ios::app);
    logFile << pData << endl;
    static int showEveryMsg; //���洫����־
    GET_WRITE(showEveryMsg, "showEveryMsg", "0");
    if (showEveryMsg == 1)
        ::MessageBox(0, pData, "�յ���Ϣ", 0);
    if (msg.type == TYPE[Server_is_closed]) { //�������ѹرյ���Ϣ�ض��ᴦ�������ȴ���
        sprintf_s(nd.szTip, "�ͻ��ˣ�%s - ����", (LPCTSTR)m_userID);
        Shell_NotifyIcon(NIM_MODIFY, &nd);
        if (m_connected) {    //����Ѿ����ӣ���Ͽ�������
            KillTimer(2);
            if (m_rf.isRecving()) {
                sendMSG(msg.join(TYPE[File_Fail], TYPE[AskFileData], m_fileUser), 0);
                m_rf.recvEnd();
            }
            m_connected = false;
            pSock->Close();
            pSock = NULL;
            m_ConPC.SetWindowText(_T("���ӷ�����"));
            UpdateData(false);
            GetDlgItem(IDC_DataReceive)->EnableWindow(0);	//������Щ�ؼ�
            GetDlgItem(IDC_DataSend)->EnableWindow(0);
            GetDlgItem(IDC_Send)->EnableWindow(0);
            GetDlgItem(IDC_msgTo)->EnableWindow(0);
        }
        modifyStatus("Server is closed!");
        SetTimer(4, 2000, 0);
    }
    elif(msg.userId == m_userID) //���Լ�����Ϣ
    {
        //MessageBox(0,msg.data,"",0);
        if (msg.type == TYPE[ChatMsg]) {
            updateEvent(msg.data, "[" + msg.fromUser + "]:");
            modifyStatus("�յ�[" + msg.fromUser + "]������Ϣ��");
        }
        elif(msg.type == TYPE[UserList]) //�յ����û��б����ʾ��¼�ɹ�
        {
            KillTimer(0);
            userNum = 0;
            m_cbMsgTo.ResetContent();
            int i = msg.data.Find(";");
            CString newUser;
            do {
                newUser = msg.data.Left(i);
                if (newUser != m_userID) { //�ѳ��Լ�������û���ӵ��û��б�
                    m_cbMsgTo.AddString(newUser);
                    userNum++;
                }
                msg.data = MyMsg::rightN(msg.data, i + 1);//msg.data.Right(msg.GetLength()-i-1);
                i = msg.data.Find(";");
            } while (i != -1);
            m_cbMsgTo.AddString("������");
            m_cbMsgTo.AddString("����������");
            m_cbMsgTo.SetCurSel(m_cbMsgTo.GetCount() - 1);
            m_cbMsgTo.GetLBText(m_cbMsgTo.GetCurSel(), m_msgTo);	//��ȡ��Ϣ���͵�Ŀ���û�
            if (m_msgTo == "������" || m_msgTo == "����������") {
                modifyStatus("�����ӵ�������. �������[" + m_msgTo + "]���ɷ�����Ϣ");
                m_toUserIsOnline = 1;
            }
            else
                sendMSG(msg.join(m_msgTo, TYPE[OnlineState], "", "", "������"), 0);
            updateEvent(msg.data, "", 1);
            GetDlgItem(IDC_DataReceive)->EnableWindow(1);
            GetDlgItem(IDC_DataSend)->EnableWindow(1);
            GetDlgItem(IDC_Connect)->EnableWindow(1);
            GetDlgItem(IDC_Send)->EnableWindow(1);
            GetDlgItem(IDC_msgTo)->EnableWindow(1);
            sprintf_s(nd.szTip, "�ͻ��ˣ�%s - ����", (LPCTSTR)m_userID);	//״̬�ڵ�½����ʱ�õ�
            Shell_NotifyIcon(NIM_MODIFY, &nd);
            m_connected = true;
            m_ConPC.SetWindowText(_T("�Ͽ�������"));
            UpdateData(false);
        }
        elif(msg.type == TYPE[OnlineState])
        {
            CString sel;
            GetDlgItemText(IDC_msgTo, sel);
            if (msg.data == "1") {
                modifyStatus("[" + sel + "]��ǰ����");
                m_toUserIsOnline = 1;
            }
            else if (msg.data == "0") {
                modifyStatus("[" + sel + "]��ǰ������");
                m_toUserIsOnline = 0;
            }
        }
        elif(msg.type == TYPE[FileSend])  //������Խ��ܷ����������Լ����ļ�����
        {
            SetForegroundWindow();
            fileSend(msg);
        }
        elif(msg.type == TYPE[FileData])
        {
            if (m_rf.isRecving()) {
                KillTimer(2);
                m_rf.addPacketage(msg.data);
                SetTimer(2, m_fileTimeOut, 0);
            }
        }
        elif(msg.type == TYPE[AskFileData])
        {
            static fstream dataTrans;
            static int saveTrans = atoi(AfxGetApp()->GetProfileString("ClientSetting", "saveTransLog", "-1"));//���洫����־
            if (saveTrans == -1) {
                AfxGetApp()->WriteProfileString("ClientSetting", "saveTransLog", "0");
                saveTrans = 0;
            }
            if (msg.data == TYPE[File_NO])	//�ܾ����ո��ļ�
                updateEvent("[" + msg.fromUser + "]�ܾ������ļ�����" + m_fileSendName + "��", "[" + msg.fromUser + "]");
            else if (msg.data == TYPE[File_Over]) {
                updateEvent("[" + msg.fromUser + "]�ѽ����ļ�����" + m_fileSendName + "��", "[" + msg.fromUser + "]");
                if (saveTrans) dataTrans.close();
            }
            else if (msg.data == TYPE[File_Fail]) {
                updateEvent("[" + msg.fromUser + "]δ�ܳɹ������ļ�����" + m_fileSendName + "��", "[" + msg.fromUser + "]");
                if (saveTrans) dataTrans.close();
            }
            else {			//���������ݰ�
                int dataIndex = atoi(msg.data);
                while (0 == dataIndex && m_readFileEnd == 0) {
                    Sleep(10);
                    static MSG msg1;
                    if (PeekMessage(&msg1, (HWND)NULL, 0, 0, PM_REMOVE)) {
                        ::SendMessage(msg1.hwnd, msg1.message, msg1.wParam, msg1.lParam);
                    }
                }	//�ȴ���ȡ�ļ����ݽ���
                if (dataIndex >= 0 && dataIndex < MAX_PACKAGE_NUM) {
                    if (0 == dataIndex)
                        m_fileUser = msg.fromUser;
                    if (saveTrans) {
                        if (0 == dataIndex)
                            dataTrans.open(myDIR + "TransLog.txt", ios::out);
                        dataTrans << "�������ݰ� " << dataIndex << "\t  " << strlen(packageData[dataIndex]) << endl;
                    }
                    sendMSG(msg.join(packageData[dataIndex], TYPE[FileData], "", "", m_fileUser), 0);
                    static int showTransLog;//��ʾ������־
                    GET_WRITE(showTransLog, "showTransLog", "0");
                    if (showTransLog) {
                        CString str;
                        str.Format("�ѷ������ݰ�%d(%d),packageNum=%d", dataIndex, strlen(packageData[dataIndex]), m_rf.getPackNum());
                        updateEvent(str, "ϵͳ֪ͨ");
                    }
                    //MessageBox(packageData[dataIndex],"tmpMsg");
                }
            }
        }
        elif(msg.type == TYPE[LoginFail]) //�û���֤ʧ��
        {
            KillTimer(0);
            MessageBox("�û���������������飡\r\n���û���˺���ע�����˺ţ�", "��½ʧ��");
            ShowWindow(SW_HIDE);
            login();
            ShowWindow(SW_SHOW);
        }
        elif(msg.type == TYPE[UserIsOnline]) //�û��Ѿ�����
        {
            KillTimer(0);
            MessageBox("���û��Ѿ�����,�����ظ���½��", "��½ʧ��");
            ShowWindow(SW_HIDE);
            login();
            ShowWindow(SW_SHOW);
        }
        elif(msg.type == TYPE[OfflineMsg]) //������Ϣ
        {
            static MyMsg olmsg;
            CString tmpMsg(msg.data);
            do {
                tmpMsg = olmsg.load(tmpMsg, 1);
                updateEvent(olmsg.type + "  " + olmsg.data, "[" + olmsg.fromUser + "]:", 0, 3);
            } while (tmpMsg.Find(seperator) != -1);
            updateEvent("��������������������������������������Ϣ��������������������������", "", 0, 3);
            modifyStatus("�յ�������Ϣ��");
        }
        elif(msg.type == TYPE[Status]) //�޸�״̬
        {
            modifyStatus(msg.data);
        }
    }
    elif(msg.userId == TYPE[AllUser]) //���������û���
    {
        if (msg.type == TYPE[ChatMsg]) {
            if (msg.fromUser != ("������-" + m_userID)) { //�����Լ��յ������Լ�����Ϣ
                updateEvent(msg.data, "[" + msg.fromUser + "]:");
                modifyStatus("�յ�[" + msg.fromUser + "]������Ϣ��");
            }
        }
        elif(msg.type == TYPE[AddUserList])
        {
            m_cbMsgTo.InsertString(m_cbMsgTo.GetCount() - 2, msg.data);
            userNum++;
        }
        elif(msg.type == TYPE[FileSend]) //������Խ��ܷ�����Ⱥ�����ļ�����
        {
            SetForegroundWindow();
            fileSend(msg);
        }
    }
    SetTimer(1, 2000, 0);	//����
}

//���ڷ�����Ϣ��������
void CmfcClient1Dlg::sendMSG(const CString &send, bool upEvent)
{
    static MyMsg tmp_msg;
    tmp_msg.load(send);
    //MessageBox(0,send,"�ͻ��˷���",0);
    if (pSock->Send(send, send.GetLength() + 1) == SOCKET_ERROR) {	//������Ϣʧ��
        CString err = pSock->getLastErrorStr();
        if (tmp_msg.type != TYPE[I_am_online]) {
            MessageBox("����ʧ�ܣ�" + err, "��ܰ��ʾ", 0);
        }
        else {
            KillTimer(1);
            sprintf_s(nd.szTip, "�ͻ��ˣ�%s - ����", (LPCTSTR)m_userID);
            Shell_NotifyIcon(NIM_MODIFY, &nd);
            if (m_connected) {    // ����Ѿ����ӣ���Ͽ�������
                m_connected = false;
                pSock->Close();
                pSock = NULL;
                m_ConPC.SetWindowText(_T("���ӷ�����"));
                UpdateData(false);
                GetDlgItem(IDC_DataReceive)->EnableWindow(0);	//������Щ�ؼ�
                GetDlgItem(IDC_DataSend)->EnableWindow(0);
                GetDlgItem(IDC_Send)->EnableWindow(0);
                GetDlgItem(IDC_msgTo)->EnableWindow(0);
            }
            SetTimer(4, 2000, 0);
            MessageBox("������������쳣���볢������: " + err, "��ܰ��ʾ", 0);
        }
        return;
    }
    if (tmp_msg.type != TYPE[I_am_online]) {
        static ofstream logFile(myDIR + "send.txt", ios::out | ios::app);
        logFile << "send:" << send << endl;
    }
    if (upEvent && tmp_msg.pw == "") {	//û�����벿��
        if (tmp_msg.type == TYPE[ChatMsg]) {
            static CString lastUser = "", nowUser = "";
            nowUser = tmp_msg.toUser;
            CString data = tmp_msg.data;
            if (nowUser != lastUser) {
                lastUser = nowUser;
                tmp_msg.data = "����������������������[" + nowUser + "]���졪������������������";
                updateEvent(tmp_msg.data, "", 0, 3);
            }
            tmp_msg.data = data;
            updateEvent(tmp_msg.data, "��:");
        }
        CmfcClient1Dlg* pDlg = (CmfcClient1Dlg*)theApp.GetMainWnd();
        m_DataSend = "";
        SetDlgItemText(IDC_DataSend, "");	// ��շ��ʹ��ڵ�����
    }
}

void CmfcClient1Dlg::updateEvent(const CString &showMsg, const CString &from,
                                 bool reset, int timeFMT)
{
    CString str, sysMsg;
    CmfcClient1Dlg* pDlg = (CmfcClient1Dlg*)theApp.GetMainWnd();
    GetDlgItemText(IDC_DataReceive, str);
    CTime time = CTime::GetCurrentTime();	// ��ȡϵͳ��ǰʱ��
    if (str == "" && timeFMT == 2) //��ϢΪ������ʾ���ں�ʱ��
        timeFMT = 1;
    switch (timeFMT) {
        case 1:
            sysMsg = from + "  " + time.Format(_T("%Y/%m/%d %H:%M:%S  "));
            break;
        case 2:
            sysMsg = from + "  " + time.Format(_T("%H:%M:%S  "));
            break;
        case 3:
            sysMsg = from == "" ? from : from + "  ";
        default:
            break;
    }
    if (str != "" && reset) {
        str = "����������������������������������ʷ��Ϣ��������������������������\r\n";
    }
    else {
        if (showMsg != "")
            str = sysMsg + showMsg + "\r\n";
    }
    CEdit* m_event = (CEdit*)GetDlgItem(IDC_DataReceive);
    int lastLine = m_event->LineIndex(m_event->GetLineCount() - 1);//��ȡ�༭�����һ������
    m_event->SetSel(lastLine + 1, lastLine + 2, 0);	//ѡ��༭�����һ��
    m_event->ReplaceSel(str);                     //�滻��ѡ��һ�е�����
    ofstream myDIR_userID(myDIR + m_userID, ios::out | ios::app);
    myDIR_userID << str;
    myDIR_userID.close();
    GetDlgItemText(IDC_DataReceive, str);
    if (str.GetLength() > 15000) {
        str = str.Right(1000);
        SetDlgItemText(IDC_DataReceive, str);
        m_event->SetSel(lastLine + 1, lastLine + 2, 0);	//ѡ��༭�����һ��
        m_event->ReplaceSel("");                     //�滻��ѡ��һ�е�����
    }
}

void CmfcClient1Dlg::fileSend(MyMsg& msg)
{
    int i = msg.data.Find('|');
    CString name = msg.data.Left(i);
    CString size = MyMsg::rightN(msg.data, i + 1);
    i = size.Find('|');
    CString fileMD5 = MyMsg::rightN(size, i + 1);
    size = size.Left(i);
    int fileSize = atoi(size);
    if (fileSize > 1024 * 1024)
        size.Format("%.2f MB", fileSize / 1024.0 / 1024.0);
    else if (fileSize > 1024)
        size.Format("%.2f KB", fileSize / 1024.0);
    else
        size.Format("%d �ֽ�", fileSize);
    if (m_rf.isRecving()) {
        CString tips = '[' + msg.fromUser + "] ���㷢���ļ���\n�ļ�����" + name + "\n�ļ���С��"
            + size + "\n��ǰ���ڽ����ļ�����ȴ���ǰ�ļ��������ٽ��������ļ�����ֹ��ǰ�ļ�����";
        MessageBox(tips, "��ܰ��ʾ");
        return;
    }
    GET_WRITE(m_fileTimeOut, "fileTimeOut", "2000");
    CString tips = '[' + msg.fromUser + "] ���㷢���ļ���\n�ļ�����" + name + "\n�ļ���С��" + size
        + "\n�Ƿ�ͬ����գ�";
    if (MessageBox(tips, "��ܰ��ʾ", MB_YESNO | MB_ICONQUESTION) == IDYES) {
        CString fmt = "*" + name.Right(name.GetLength() - name.ReverseFind('.'));
        CFileDialog dlg(false, 0, name, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, fmt + "|" + fmt + "|All Files(*.*)|*.*||");
        if (dlg.DoModal() == IDOK) {
            m_fileUser = msg.fromUser;
            sendMSG(msg.join("0", TYPE[AskFileData], "", "", m_fileUser), 0);
            m_rf.init(dlg.GetPathName(), fileSize, fileMD5);
            SetTimer(2, m_fileTimeOut, 0);
            modifyStatus("׼�������ļ���");
        }
        else
            sendMSG(msg.join(TYPE[File_NO], TYPE[AskFileData], "", "", msg.fromUser), 0);
    }
    else
        sendMSG(msg.join(TYPE[File_NO], TYPE[AskFileData], "", "", msg.fromUser), 0);
}

void CmfcClient1Dlg::modifyStatus(const CString &sta, bool _sleep)	//�޸�״̬��
{
    HWND h = CreateStatusWindow(WS_CHILD | WS_VISIBLE, sta, m_hWnd, 0);
    if (_sleep)
        Sleep(20);
    ::SendMessage(h, SB_SETBKCOLOR, 0, RGB(0, 125, 205));
}

bool CmfcClient1Dlg::login()
{
    CLoginDlg loginDlg;
    if (loginDlg.DoModal() != IDOK)  //������¼�Ի���
        return false;
    m_userID = loginDlg.userID;
    m_pw = loginDlg.pw;
    m_ip = loginDlg.ip;
    m_port = loginDlg.port;
    SetWindowText("�ͻ��� - " + m_userID);
    OnConnect();
    return true;
}

void CmfcClient1Dlg::save_SendFileInfo(const CString &filepath, long size)
{
    if (m_rf.isRecving()) {
        MBox("��ǰ���ڽ����ļ���Ϊ�˲�Ӱ�����ݽ��գ���ȴ���ǰ������ɺ��ٷ����ļ���");
        return;
    }
    m_fileSendName = filepath;
    CString name = m_fileSendName, s;
    SetTimer(3, 1, 0);	//���ú�̨�̶߳�ȡ�ļ�����
    name = name.Right(name.GetLength() - name.ReverseFind('\\') - 1);
    static MD5 md5;
    char szMD5[33] = "";
    md5.fileMd5(szMD5, filepath);
    s.Format("%s|%d|%s", name, size, szMD5);
    sendMSG(mymsg.join(s, TYPE[FileSend], m_userID, "", m_msgTo), 0);
    m_rf.setPackNum((size + PACKAGE_SIZE - 1) / PACKAGE_SIZE);
}
