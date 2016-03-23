
// mfcClient1Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "mfcClient1.h"
#include "mfcClient1Dlg.h"
#include "afxdialogex.h"
#include "md5.hpp"

#define IP_LAN		 "172.27.35.3"
#define IP_LOCALHOST "127.0.0.1"
#define IP_SERVER	 "120.25.207.230"

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
{
}

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
    ON_BN_CLICKED(IDC_Connect, &CmfcClient1Dlg::OnBnClickedConnect)
    ON_BN_CLICKED(IDC_Send, &CmfcClient1Dlg::OnBnClickedSend)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_LogOff, &CmfcClient1Dlg::OnBnClickedLogoff)
    ON_BN_CLICKED(IDC_Chatlog, &CmfcClient1Dlg::OnBnClickedChatlog)
    ON_CBN_SELCHANGE(IDC_msgTo, &CmfcClient1Dlg::OnCbnSelChangeMsgTo)
    ON_WM_DROPFILES()
END_MESSAGE_MAP()
#pragma endregion

BOOL CmfcClient1Dlg::PreTranslateMessage(MSG* pMsg)
{
    if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN) {
        CWnd *pw = CWnd::GetFocus();
        if (pw == GetDlgItem (IDC_DataSend)) {
            OnBnClickedSend();
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

    LONG style = ::GetWindowLong(this->GetSafeHwnd(), GWL_STYLE);
    style &= ~WS_THICKFRAME;//ʹ���ڲ��������ı��С
    ::SetWindowLong(this->GetSafeHwnd(), GWL_STYLE, style);

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
    if(!loginDlg()) {
        PostQuitMessage(0);
    }
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
    if (nIDEvent==0) {		//��¼���
        if(KillTimer(0)) {
            pSock->Close();
            SetForegroundWindow();
            ::MessageBox(GetSafeHwnd(),"��������ʱ�������ԣ�","��ܰ��ʾ",0);
            ShowWindow(SW_HIDE);
            loginDlg();
            ShowWindow(SW_SHOW);
        }
    }
    elif(nIDEvent==1) {		//��������
        if(m_connect)
            pSock->SendMSG(pSock->mymsg.join("",TYPE[I_am_online],"","",""),0);
    }
    elif(nIDEvent==2) {		//�����ļ����
        KillTimer(2);
        if(rf.isRecving()) {
            pSock->SendMSG(pSock->mymsg.join(TYPE[File_Fail],TYPE[AskFileData],"","",fileUser),0);
            rf.recvEnd();
            pSock->updateEvent("�����ļ�"+rf.getFileName()+"��ʱ","ϵͳ֪ͨ");
            ::MessageBox(GetSafeHwnd(),"�����ļ���ʱ�����Ժ����ԣ�","��ܰ��ʾ",0);
            modifyStatus("�ļ����ճ�ʱ��",0);
        }
    }
    elif(nIDEvent==3) {		//�����ļ����Զ������ļ�����
        KillTimer(3);
        readFileEnd = 0;
        CXXFStream fileStr(fileSendName,ios::in | ios::binary);
        long fileSize = (long)fileStr.getSize(),readSize = 0;
        for(unsigned i=0; readSize<fileSize; i++) {
            memset(packageData[i],0,2*PACKAGE_SIZE+1);
            if(fileSize-readSize>PACKAGE_SIZE) {		//���������İ�û�ж�ȡ
                fileStr.readString(packageData[i],PACKAGE_SIZE);
                readSize += PACKAGE_SIZE;
            } else {
                fileStr.readString(packageData[i],fileSize-readSize);
                readSize = fileSize;
            }
        }
        readFileEnd = 1;
        fileStr.close();
        fileSendName = fileSendName.Right(fileSendName.GetLength()-fileSendName.ReverseFind('\\')-1);
    }
    elif(nIDEvent==4) {		//�������Ͽ���ÿ��һ��ʱ��������������
        OnBnClickedConnect();
    }
    CDialog::OnTimer(nIDEvent);
}

void CmfcClient1Dlg::OnBnClickedConnect()
{
    if (m_connect) {    // ����Ѿ����ӣ���Ͽ�������
        m_connect = false;
        pSock->SendMSG( pSock->mymsg.join("",TYPE[Logout],"","","",m_pw) ,0);
        pSock->Close();
        pSock = NULL;
        m_ConPC.SetWindowText(_T("���ӷ�����"));
        UpdateData(false);
        GetDlgItem(IDC_Connect)->EnableWindow(1);
        GetDlgItem(IDC_DataReceive)->EnableWindow(0);	//������Щ�ؼ�
        GetDlgItem(IDC_DataSend)->EnableWindow(0);
        GetDlgItem(IDC_Send)->EnableWindow(0);
        GetDlgItem(IDC_msgTo)->EnableWindow(0);
        sprintf_s(nd.szTip, "�ͻ��ˣ�%s - ����",m_userID);
        Shell_NotifyIcon(NIM_MODIFY, &nd);
        modifyStatus("�ѶϿ�������");
        KillTimer(1);
        return;
    } else {                                            // δ���ӣ������ӷ�����
        pSock = new CClientSocket(m_userID);
        if (!pSock->Create()) {       //�����׽���
            MessageBox(_T("�����׽���ʧ�ܣ�"),"��ܰ��ʾ");
            return;
        }
    }
    //�ڽ������ݵ�ʱ����ִ����ϵͳ��������socket�������Ŀ���������߳��������
    int nSize = DATA_BUF_SIZE;//���û�������С
    setsockopt( *pSock, SOL_SOCKET, SO_RCVBUF, ( char * )&nSize, sizeof( int ) );
    setsockopt( *pSock, SOL_SOCKET, SO_SNDBUF, ( char * )&nSize, sizeof( int ) );
    ////��ֹNagle�㷨����ͨ����δȷ�ϵ����ݴ��뻺����ֱ������һ����һ���͵ķ������������������͵�����С���ݰ�����Ŀ��
    /*bool b_noDely = 1;
    setsockopt( *pSock, SOL_SOCKET, TCP_NODELAY, ( char * )&b_noDely, sizeof( b_noDely ) );*/
    if (!pSock->Connect(_T(m_ip), m_port)) {  //���ӷ�����ʧ��
        if (!firstCon) {
            CString str;
            str.Format("��������룺%d��", pSock->getError());
            MessageBox("���ӷ�����ʧ�ܣ�" + str, "��ܰ��ʾ");
        }
        return;
    } else {
        pSock->SendMSG( pSock->mymsg.join("",TYPE[Login],"","","",m_pw),0 );	//���ӷ�����ʱ������������֤���
        SetTimer(0,3000,NULL);		//������ʱ
        m_ConPC.SetWindowText(_T("�������ӷ�����..."));
        GetDlgItem(IDC_Connect)->EnableWindow(0);	//���������ͳ�����δ����Ӧǰ�������Ӱ�ť
    }
    firstCon = 0;
    GetDlgItem(IDC_DataSend)->SetFocus();
}

void CmfcClient1Dlg::OnBnClickedSend()
{
    if (!m_connect) {
        MessageBox("�������ӷ�����", "��ܰ��ʾ");
        return;                               //δ���ӷ�������ִ��
    }
    UpdateData(true);                         //��ȡ�ؼ�����
    if (m_DataSend != "") {
        if(m_msgTo==m_userID) {
            MessageBox("�벻Ҫ���Լ�������Ϣ", "��ܰ��ʾ");
        } else {
            pSock->SendMSG( pSock->mymsg.join(m_DataSend,TYPE[ChatMsg],"","",m_msgTo));
        }
    } else {
        MessageBox("������������", "��ܰ��ʾ");
    }
    GetDlgItem(IDC_DataSend)->SetFocus();
}

void CmfcClient1Dlg::OnOK()
{
    OnBnClickedSend();
}

void CmfcClient1Dlg::OnBnClickedLogoff()
{
    KillTimer(4);
    if (m_connect) {    // ����Ѿ����ӣ���Ͽ�������
        m_connect = false;
        pSock->SendMSG( pSock->mymsg.join("",TYPE[Logout],"","","",m_pw) ,0);
        pSock->Close();
        pSock = NULL;
        GetDlgItem(IDC_DataSend)->SetWindowText("");
    }
    sprintf_s(nd.szTip, "�ͻ��� - ��½");
    Shell_NotifyIcon(NIM_MODIFY, &nd);
    GetDlgItem(IDC_DataReceive)->SetWindowText("");
    ShowWindow(SW_HIDE);
    if(!loginDlg())
        PostQuitMessage(0);
    ShowWindow(SW_SHOW);
}

void CmfcClient1Dlg::OnBnClickedChatlog()
{
    delete pChatlog;
    pChatlog = new CChatLogDlg(m_userID);
    pChatlog->Create(IDD_ChatLog);//����һ����ģ̬�Ի���
    pChatlog->ShowWindow(SW_SHOWNORMAL); //��ʾ��ģ̬�Ի���
    int lastLine =  pChatlog->p_editCL->LineIndex( pChatlog->p_editCL->GetLineCount() - 1);
    pChatlog->p_editCL->SetSel(lastLine+1,lastLine+2, 0);	//ѡ��༭�����һ��
    pChatlog->p_editCL->ReplaceSel(pChatlog->chatLog);   //�滻��ѡ��һ�е�����
}

void CmfcClient1Dlg::OnCbnSelChangeMsgTo()
{
    m_cbMsgTo.GetLBText(m_cbMsgTo.GetCurSel(),m_msgTo);	//��ȡ��Ϣ���͵�Ŀ���û�
    to_isOnline = 1;
    if(m_msgTo=="������" || m_msgTo=="����������")
        modifyStatus("�������["+m_msgTo+"]���ɷ�����Ϣ");
    else
        pSock->SendMSG( pSock->mymsg.join(m_msgTo,TYPE[OnlineState]) ,0);
}

void CmfcClient1Dlg::OnDropFiles(HDROP hDropInfo)
{
    SetForegroundWindow();		//���ô����ö���ʾ
    if(!m_connect) {
        MBox("�����ѶϿ����������ӷ�������");
        return;
    }
    if(rf.isRecving()) {
        MBox("��ǰ���ڽ����ļ���Ϊ�˲�Ӱ�����ݽ��գ���ȴ���ǰ������ɺ��ٷ����ļ���");
        return;
    }
    // ��ȡ�ļ��������ļ�����������ת����ĳ���û�
    int  nFileCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 256);   //��ȡ������ļ�����
    char filepath[1024]="";
    DragQueryFile(hDropInfo,0,filepath,1024);	// ��ȡ�Ϸŵ�1���ļ��������ļ���
    do {
        if(GetFileAttributes(filepath)!=FILE_ATTRIBUTE_DIRECTORY) {
            CFile fileSend(filepath,CFile::modeRead);
            long size = (long)fileSend.GetLength();
            fileSend.Close();
            if(size<=0) {
                MBox("�ļ�Ϊ�գ����飡");
                break;
            }
            elif(size>MAX_PACKAGE_NUM*PACKAGE_SIZE) {
                CString s;
                s.Format("��ֻ֧�ִ��� %dMB ���ڵ��ļ���",MAX_PACKAGE_NUM*PACKAGE_SIZE/1024/1024);
                MBox(s);
                break;
            }
            if(to_isOnline!=1) {
                MBox("�Է������ߣ���ʱ�޷����������ļ�������Է���������Ϣ������ϵ");
                break;
            }
            save_SendFileInfo(filepath,size);
        } else
            MBox("����Ĳ���һ����Ч�ļ�");
    } while(0);
    DragFinish(hDropInfo);
    CDialogEx::OnDropFiles(hDropInfo);
}

BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDSET, &CLoginDlg::OnBnClickedSet)
    ON_BN_CLICKED(IDC_regist, &CLoginDlg::OnBnClickedregist)
    ON_BN_CLICKED(IDC_CHECK_remPw, &CLoginDlg::OnBnClickedCheckrempw)
    ON_BN_CLICKED(IDC_Swap, &CLoginDlg::OnBnClickedSwap)
    ON_BN_CLICKED(IDC_ShowPw, &CLoginDlg::OnBnClickedShowpw)
END_MESSAGE_MAP()


void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_userID, userID);
    DDV_MaxChars(pDX, userID, 16);
    DDX_Text(pDX, IDC_pw, pw);
    DDV_MaxChars(pDX, pw, 16);
    DDX_Control(pDX, IDC_IPADDRESS1, m_IPAddr);
    DDX_Text(pDX, IDC_EDIT1, port);
}

bool CLoginDlg::onlyAlNum(const CString s)
{
    for(int i=s.GetLength()-1; i>=0; --i)
        if(s[i]>0 && !isalnum(s[i]))	//������ĸ�������֣�Ҳ���������ַ�
            return 0;
    return 1;
}
//��������Ƿ������Ч��Ϣ
bool CLoginDlg::DataInvalide()
{
    UpdateData(1);
    m_IPAddr.GetWindowText(ip);
    userID.Replace(" ", "");
    pw.Replace(" ", "");
    if (userID == "") {
        MessageBox("�û�������Ϊ��", "��ܰ��ʾ");
        GetDlgItem(IDC_userID)->SetFocus();
        return 1;
    }
    if(!onlyAlNum(userID)) {
        MessageBox("�û���ֻ�ܰ�����ĸ�����ֻ���", "��ܰ��ʾ");
        CEdit* edit = (CEdit*)GetDlgItem(IDC_userID);
        edit->SetFocus();
        edit->SetSel(0,20);
        return 1;
    }
    if (pw == "") {
        MessageBox("���벻��Ϊ��", "��ܰ��ʾ");
        GetDlgItem(IDC_pw)->SetFocus();
        return 1;
    }
    if (port<1024 || port>65535) {
        MessageBox("�˿ں�����������Ч��ΧΪ1024-65535��", "��ܰ��ʾ");
        GetDlgItem(IDC_EDIT1)->SetFocus();
        return 1;
    }
    OnBnClickedCheckrempw();
    return 0;
}

void CLoginDlg::OnBnClickedOk()
{
    if(DataInvalide()==1)
        return;
    CClientSocket sock(userID+"*test");
    if (!sock.Create()) {       //�����׽���
        MessageBox(_T("�����׽���ʧ�ܣ�"), "��ܰ��ʾ");
        return;
    }
    if (!sock.Connect(_T(ip), port)) {  //���ӷ�����
        CString str;
        str.Format("��������룺%d��", sock.getError());
        MessageBox("���ӷ�����ʧ�ܣ�" + str, "��ܰ��ʾ");
        return;
    }
    sock.Close();
    AfxGetApp()->WriteProfileString("Login","IP",ip);
    CDialogEx::OnOK();
}

void CLoginDlg::OnBnClickedSet()
{
    const int dt = 2;
    if (b_strech) {
        for (int i = 0; m_rc.Height() - i >= 200; i += dt) {
            SetWindowPos(NULL, m_rc.left, m_rc.top, m_rc.Width(), m_rc.Height() - i, SWP_NOMOVE | SWP_SHOWWINDOW);
            Sleep(1);
        }
        b_strech = FALSE;
    } else {
        for (int i = 0; i + 200 <= m_rc.Height(); i += dt) {
            SetWindowPos(NULL, m_rc.left, m_rc.top, m_rc.Width(), i + 200, SWP_NOMOVE | SWP_SHOWWINDOW);
            Sleep(1);
        }
        b_strech = TRUE;
    }
}

BOOL CLoginDlg::OnInitDialog()
{
    ((CButton*)GetDlgItem(IDC_CHECK_remPw))->SetCheck(TRUE);
    CDialog::OnInitDialog();
    CString ipAddr = AfxGetApp()->GetProfileString("Login","IP",IP_SERVER);	//��Ҫ���ӵķ�����ip��ַ
    m_IPAddr.SetWindowText(ipAddr);
    b_strech = FALSE;
    CString id_pw = AfxGetApp()->GetProfileString("UserInfo", "userID_pw");
    int i = id_pw.Find(STR[0]);
    if(i!=-1) {
        userID = id_pw.Left(i);
        pw = id_pw.Right(id_pw.GetLength()-i-3);
    } else {
        char _user[17]="", _pw[17]="";
        ifstream user_pw(myDIR+"user_pw.dat",ios::in);
        if(!user_pw.fail()) {
            user_pw>>_user>>_pw;
            user_pw.close();
            if(_user[0]!=0 && _pw[0]!=0) {
                userID = _user;
                pw = _pw;
            }
        } else {
            MessageBox("�ܸ�л�׵�����Ŷ���״�ʹ������ע��һ���Լ����˺Űɣ�","��ܰ��ʾ");
        }
    }
    UpdateData(0);
    GetWindowRect(&m_rc);
    SetWindowPos(NULL, m_rc.left, m_rc.top, m_rc.Width(), 200, SWP_NOMOVE | SWP_SHOWWINDOW);
    GetDlgItem(IDC_userID)->SetFocus();
    return TRUE;
}

void CLoginDlg::OnBnClickedregist()
{
    if(DataInvalide())
        return;
    SetTimer(0,3000,NULL);		//������ʱ
    CClientSocket sock(userID);
    if (!sock.Create()) {       //�����׽���
        MessageBox(_T("ע��ʧ�ܡ��������׽���ʧ�ܣ�"), "��ܰ��ʾ");
        return;
    }
    if (!sock.Connect(_T(ip), port)) {  //���ӷ�����
        CString str;
        str.Format("��������룺%d��", sock.getError());
        MessageBox("ע��ʧ�ܡ������ӷ�����ʧ�ܣ�" + str, "��ܰ��ʾ");
        puts("\a");
        return;
    }
    if(KillTimer(0)) {
        sock.SendMSG( sock.mymsg.join("",TYPE[Register],"","","",pw),0);
        sock.Close();
        MessageBox("ע����Ϣ���ͳɹ������������ͨ���󼴿ɵ�½��", "��ܰ��ʾ");
        OnOK();
    }
}

void CLoginDlg::OnBnClickedCheckrempw()
{
    if(userID!="" && pw!="") {
        if(BST_CHECKED==((CButton*)GetDlgItem(IDC_CHECK_remPw))->GetCheck()) {
            ofstream user_pw(myDIR+"user_pw.dat");
            user_pw<<userID<<"\t"<<pw;
            user_pw.close();
            AfxGetApp()->WriteProfileString("UserInfo", "userID_pw", userID+ STR[0] +pw);
        } else {
            DeleteFile(myDIR+"user_pw.dat");
            AfxGetApp()->WriteProfileString("UserInfo", "userID_pw", "");
        }
    }
}

void CLoginDlg::OnBnClickedSwap()
{
    if(BST_CHECKED==((CButton*)GetDlgItem(IDC_Swap))->GetCheck()) {
        m_IPAddr.SetWindowText(IP_LOCALHOST);
    } else {
        m_IPAddr.SetWindowText(IP_SERVER);
    }
}

void CLoginDlg::OnBnClickedShowpw()
{
    CEdit *edit = (CEdit*)GetDlgItem(IDC_pw);
    if(BST_CHECKED==((CButton*)GetDlgItem(IDC_ShowPw))->GetCheck()) {
        edit->SetPasswordChar(0);
    } else {
        edit->SetPasswordChar(_T('*'));
    }
    edit->RedrawWindow(NULL,NULL);
}

BEGIN_MESSAGE_MAP(CChatLogDlg, CDialogEx)
    ON_BN_CLICKED(IDC_DelAllLog, &CChatLogDlg::OnBnClickedDelalllog)
    ON_BN_CLICKED(IDC_Update, &CChatLogDlg::OnBnClickedUpdate)
END_MESSAGE_MAP()

void CChatLogDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BOOL CChatLogDlg::OnInitDialog()
{
    p_editCL = (CEdit*)GetDlgItem(IDC_Chatlog);
    //���ظ��û��������¼
    chatLog = "";
    string _str;
    char c_str[1024];
    ifstream tmpF(myDIR + userID,ios::in);
    while(!tmpF.eof() && tmpF.is_open()) {
        getline(tmpF,_str);
        if(_str!="") {
            strncpy_s(c_str,&_str[0],1024);
            chatLog += c_str;
            chatLog += "\r\n";
        }
    }
    tmpF.close();
    if(chatLog=="")
        chatLog = "��ʱ���κ������¼";
    return TRUE;
}

void CChatLogDlg::OnBnClickedDelalllog()
{
    if(chatLog != "��ʱ���κ������¼"
       && MessageBox("***ȷ��ɾ�����������¼�𣿴˲������ɳ��أ�***","ɾ��ȷ��",MB_YESNO)==IDYES) {
        DeleteFile(myDIR+userID);
        MessageBox("��ɾ�����������¼��","��ܰ��ʾ");
    } else if(chatLog == "��ʱ���κ������¼") {
        MessageBox("��ʱ���κ������¼","��ܰ��ʾ");
    } else {
        return;
    }
    ::SendMessage(GetSafeHwnd(),WM_CLOSE,0,0);
}

void CChatLogDlg::OnBnClickedUpdate()
{
    chatLog = "";
    string _str;
    char c_str[1024];
    ifstream tmpF(myDIR + userID,ios::in);
    while(!tmpF.eof() && tmpF.is_open()) {
        getline(tmpF,_str);
        if(_str!="") {
            strncpy_s(c_str,&_str[0],1024);
            chatLog += c_str;
            chatLog += "\r\n";
        }
    }
    tmpF.close();
    if(chatLog=="")
        chatLog = "��ʱ���κ������¼";
    SetDlgItemText(IDC_Chatlog,"");
    int lastLine =  p_editCL->LineIndex( p_editCL->GetLineCount() - 1);
    p_editCL->SetSel(lastLine+1,lastLine+2, 0);	//ѡ��༭�����һ��
    p_editCL->ReplaceSel(chatLog);   //�滻��ѡ��һ�е�����
}


int  CClientDlg::loginDlg()
{
    logining = 1;
    bool loginOK = 0;
    // �����û����������Ƿ���Ч
    if (login.DoModal() != IDOK)
        return 0;
    m_userID = login.userID;
    m_pw = login.pw;
    m_ip = login.ip;
    m_port = login.port;
    pDlg->SetWindowText("�ͻ��� - " + m_userID);
    pDlg->OnBnClickedConnect();
    logining = 0;
    return 1;
}

void CClientDlg::modifyStatus(CString sta,bool _sleep)	//�޸�״̬��
{
    HWND h = ::CreateStatusWindow(WS_CHILD | WS_VISIBLE, sta, pDlg->m_hWnd, 0);
    if(_sleep)
        Sleep(10 * _sleep);
    ::SendMessage(h, SB_SETBKCOLOR, 1, RGB(0, 125, 205));
}

void CClientDlg::save_SendFileInfo(const char* filepath,long size)
{
    if(rf.isRecving()) {
        pDlg->MBox("��ǰ���ڽ����ļ���Ϊ�˲�Ӱ�����ݽ��գ���ȴ���ǰ������ɺ��ٷ����ļ���");
        return;
    }
    fileSendName = filepath;
    CString name = fileSendName ,s;
    pDlg->SetTimer(3,1,0);	//���ú�̨�̶߳�ȡ�ļ�����
    name = name.Right(name.GetLength()-name.ReverseFind('\\')-1);
    static MD5 md5;
    char szMD5[33] = "";
    md5.fileMd5(szMD5,filepath);
    s.Format("%s|%d|%s",name,size,szMD5);
    pSock->SendMSG( pSock->mymsg.join(s,TYPE[FileSend],"","",m_msgTo),0);
    rf.setPackNum((size+PACKAGE_SIZE-1)/PACKAGE_SIZE);
}
