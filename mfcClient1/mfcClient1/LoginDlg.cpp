#include "stdafx.h"
#include "mfcClient1.h"
#include "mfcClient1Dlg.h"
#include "LoginDlg.h"

#include <fstream>
using namespace std;

BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &CLoginDlg::OnOk)
    ON_BN_CLICKED(IDSET, &CLoginDlg::OnSet)
    ON_BN_CLICKED(IDC_regist, &CLoginDlg::OnRegist)
    ON_BN_CLICKED(IDC_CHECK_remPw, &CLoginDlg::OnCheckRemPW)
    ON_BN_CLICKED(IDC_Swap, &CLoginDlg::OnSwap)
    ON_BN_CLICKED(IDC_ShowPw, &CLoginDlg::OnShowPW)
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

BOOL CLoginDlg::OnInitDialog()
{
    ((CButton*)GetDlgItem(IDC_CHECK_remPw))->SetCheck(TRUE);
    CDialog::OnInitDialog();
    CString ipAddr = AfxGetApp()->GetProfileString("Login", "IP", IP_SERVER);	//��Ҫ���ӵķ�����ip��ַ
    m_IPAddr.SetWindowText(ipAddr);
    b_strech = FALSE;
    CString id_pw = AfxGetApp()->GetProfileString("UserInfo", "userID_pw");
    int i = id_pw.Find(STR[0]);
    if (i != -1) {
        userID = id_pw.Left(i);
        pw = id_pw.Right(id_pw.GetLength() - i - 3);
    } else {
        char _user[17] = "", _pw[17] = "";
        ifstream user_pw(myDIR + "user_pw.dat", ios::in);
        if (!user_pw.fail()) {
            user_pw >> _user >> _pw;
            user_pw.close();
            if (_user[0] != 0 && _pw[0] != 0) {
                userID = _user;
                pw = _pw;
            }
        } else {
            MessageBox("�ܸ�л�׵�����Ŷ���״�ʹ������ע��һ���Լ����˺Űɣ�", "��ܰ��ʾ");
        }
    }
    UpdateData(0);
    GetWindowRect(&m_rc);
    SetWindowPos(NULL, m_rc.left, m_rc.top, m_rc.Width(), 200, SWP_NOMOVE | SWP_SHOWWINDOW);
    GetDlgItem(IDC_userID)->SetFocus();
    return TRUE;
}

bool CLoginDlg::onlyAlNum(const CString &s)
{
    for (int i = s.GetLength() - 1; i >= 0; --i)
        if (s[i] > 0 && !isalnum(s[i]))	//������ĸ�������֣�Ҳ���������ַ�
            return 0;
    return 1;
}
//��������Ƿ������Ч��Ϣ
bool CLoginDlg::dataInvalide()
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
    if (!onlyAlNum(userID)) {
        MessageBox("�û���ֻ�ܰ�����ĸ�����ֻ���", "��ܰ��ʾ");
        CEdit* edit = (CEdit*)GetDlgItem(IDC_userID);
        edit->SetFocus();
        edit->SetSel(0, 20);
        return 1;
    }
    if (pw == "") {
        MessageBox("���벻��Ϊ��", "��ܰ��ʾ");
        GetDlgItem(IDC_pw)->SetFocus();
        return 1;
    }
    if (port < 1024 || port>65535) {
        MessageBox("�˿ں�����������Ч��ΧΪ1024-65535��", "��ܰ��ʾ");
        GetDlgItem(IDC_EDIT1)->SetFocus();
        return 1;
    }
    OnCheckRemPW();
    return 0;
}

void CLoginDlg::OnOk()
{
    if (dataInvalide() == 1)
        return;
    CClientSocket sock(userID + "*test");
    if (!sock.Create()) {       //�����׽���
        MessageBox("�����׽���ʧ�ܣ�", "��ܰ��ʾ", MB_ICONERROR);
        return;
    }
    SetTimer(0, 3000, NULL);		//������ʱ
    if (!sock.Connect(ip, port)) {  //���ӷ�����
        MessageBox("���ӷ�����ʧ�ܣ�" + getLastErrorStr(), "��ܰ��ʾ", MB_ICONERROR);
        return;
    }
    sock.Close();
    AfxGetApp()->WriteProfileString("Login", "IP", ip);
    CDialogEx::OnOK();
}

void CLoginDlg::OnSet()
{
    const int dt = 20;
    if (b_strech) {
        for (int i = 0; m_rc.Height() - i >= 200; i += dt) {
            SetWindowPos(NULL, m_rc.left, m_rc.top, m_rc.Width(), m_rc.Height() - i, SWP_NOMOVE | SWP_SHOWWINDOW);
            Sleep(10);
        }
        b_strech = FALSE;
    } else {
        for (int i = 0; i + 200 <= m_rc.Height(); i += dt) {
            SetWindowPos(NULL, m_rc.left, m_rc.top, m_rc.Width(), i + 200, SWP_NOMOVE | SWP_SHOWWINDOW);
            Sleep(10);
        }
        b_strech = TRUE;
    }
}

void CLoginDlg::OnRegist()
{
    if (dataInvalide())
        return;
    CClientSocket sock(userID);
    if (!sock.Create()) {       //�����׽���
        MessageBox("ע��ʧ�ܡ��������׽���ʧ�ܣ�", "��ܰ��ʾ",MB_ICONERROR);
        return;
    }
    SetTimer(0, 3000, NULL);		//������ʱ
    if (!sock.Connect(ip, port)) {  //���ӷ�����
        MessageBox("ע��ʧ�ܡ������ӷ�����ʧ�ܣ�" + getLastErrorStr(), "��ܰ��ʾ", MB_ICONERROR);
        puts("\a");
        return;
    }
    if (KillTimer(0)) {
        sock.SendMSG(sock.mymsg.join("", TYPE[Register], userID, "", "", pw), 0);
        sock.Close();
        MessageBox("ע����Ϣ���ͳɹ������ͨ���󼴿ɵ�½��", "��ܰ��ʾ");
        OnOK();
    }
}

void CLoginDlg::OnCheckRemPW()
{
    if (userID != "" && pw != "") {
        if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHECK_remPw))->GetCheck()) {
            ofstream user_pw(myDIR + "user_pw.dat");
            user_pw << userID << "\t" << pw;
            user_pw.close();
            AfxGetApp()->WriteProfileString("UserInfo", "userID_pw", userID + STR[0] + pw);
        } else {
            DeleteFile(myDIR + "user_pw.dat");
            AfxGetApp()->WriteProfileString("UserInfo", "userID_pw", "");
        }
    }
}

void CLoginDlg::OnSwap()
{
    if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_Swap))->GetCheck()) {
        m_IPAddr.SetWindowText(IP_LOCALHOST);
    } else {
        m_IPAddr.SetWindowText(IP_SERVER);
    }
}

void CLoginDlg::OnShowPW()
{
    CEdit *edit = (CEdit*)GetDlgItem(IDC_pw);
    if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_ShowPw))->GetCheck()) {
        edit->SetPasswordChar(0);
    } else {
        edit->SetPasswordChar(_T('*'));
    }
    edit->RedrawWindow(NULL, NULL);
}
