
// mfcServer1Dlg.h : ͷ�ļ�
//

#pragma once
#include "mfcServer1.h"

#include "ServerSocket.h"
#include <fstream>
#include <map>

class CServerSocket;

#define WM_NOTIFYICONMSG WM_USER+3 //������Ϣ

#define MBox(s) MessageBox(s,"��ܰ��ʾ")
#define MBox2(s1,s2) MessageBox(s1,s2)
#define elif else if
#define FOR(ii,start,end) for(ii=start;ii<end;++ii)

inline CString rightN(CString str, int n)
{
    return str.Right(str.GetLength() - n);
}

extern CString STR[5];
struct MyMsg {
    CString userId;
    CString pw;
    CString data;
    CString type;
    CString fromUser;
    CString toUser;
    explicit MyMsg(CString str = "") {
        load(str);
    }
    CString load(CString str) {
        CString tempStr[6] = { "" };
        int index = 0, i;
        FOR(index, 0, 5) {
            i = str.Find(STR[index]);
            tempStr[index] = str.Left(i);
            str = rightN(str, i + 3);
            if (str == "")
                break;
        }
        tempStr[5] = str;
        i = str.Find(STR[0]);
        if (i != -1)
            str = rightN(str, i + 3);
        index = 0;
        userId = tempStr[index++];
        pw = tempStr[index++];
        fromUser = tempStr[index++];
        toUser = tempStr[index++];
        type = tempStr[index++];
        data = tempStr[index++];
        return str;
    }
    const CString join(CString _data = "", CString _type = "", CString _user = "", CString _from = "", CString _to = "", CString _pw = "") const {
        if (_user == "")
            _user = userId;
        //�û���+����+����+ȥ��+����+����
        return _user + STR[0] + _pw + STR[1] + _from + STR[2] + _to + STR[3] + _type + STR[4] + _data;
    }
};
// CmfcServer1Dlg �Ի���
class CmfcServer1Dlg : public CDialogEx
{
#define DATASRC CString("UserData.dat")
#define OLMSG CString("offlineMsg")
public:
    CmfcServer1Dlg(CWnd* pParent = NULL);   // ��׼���캯��
    ~CmfcServer1Dlg();

    // �Ի�������
    enum { IDD = IDD_MFCSERVER1_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

protected:
    HICON m_hIcon;
    NOTIFYICONDATA nd;  //֪ͨ��ͼ��

    // ���ɵ���Ϣӳ�亯��
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
public:
    const static int UserNumMax = 1000; //���������1000���û�
    struct _UserInfo {
        char User[17]; //�û���
        char Pw[17];   //����
        bool Online;   //����״̬
        bool refuse;   //�Ƿ񱻾ܾ���¼
    } userInfo[UserNumMax];
    typedef std::map<std::string, CServerSocket*> UserSocket;
    UserSocket user_socket; //����<�û���,socket>ӳ���ϵ
    MyMsg mymsg;
    int userNum;            //������ע���û���
    CString fileUser;       //���ļ�������ص��û�
    CString userList;       //�û��б�,���û���¼ʱ�������û�
    CServerSocket* listenSocket;//���ڷ�����������������
    bool m_connect;         //���ڱ�Ƿ�����״̬
    CString fileSendName;   //��Ҫ���͵��ļ����ļ���
    bool readFileEnd;       //����ļ���ȡ�Ƿ����
    RecvFile rf;            //�����ļ�����
    struct SendFileTo {
        CString fileToUser;     //�������ݴ���ļ���Ŀ���û�
        CString fileFromUser;   //�������ݴ���ļ�����Դ�û�
        bool fileSendOver;      //��������Ҫת�����ļ��Ƿ������
        CString fileInfo;       //��Ҫ���͵��ļ�����Ϣ
        void set(CString to, CString from, bool over, CString info) {
            fileToUser = to;
            fileFromUser = from;
            fileSendOver = over;
            fileInfo = info;
        }
    } sft;  //������ת���ļ��ṹ

    void AddClient();                       // �����û�����Ӧ�û�����
    void RemoveClient(const CString _user); // �Ƴ����ߵ��û�
    void ReceData(CServerSocket* pSocket);  // ��ȡ����
    void UpdateEvent(CString str, CString from = "������\t");// �����¼���־
    void SendMSG(CString str);              // ������Ϣ�������ͻ���
    void ControlPC(CString AndroidControl); // �ֻ�����PC����Ӧ����
    void SendCloseMsg();  // ���͹رշ���������Ϣ
    int UserInfoValid(bool CheckOnline = 0, bool onlyUser = 0, CString checkUser = "");  // ��֤�û����������Ƿ���Ч
    int GetOnlineNum();   //�õ������û���Ŀ
    bool isOnline(CString _user);   //�õ��û��Ƿ����ߣ��������
    void fileSend(MyMsg& msg, bool NoAsk = 0);       //�յ��û�from�����ļ�������
    int fileTransfer(MyMsg& msg, const char* pData); //�ļ�����
    void modifyStatus(CString sta, bool _sleep = 1); //�޸�״̬��
    void sendFileToOthers(bool first = 0);  //���������ļ��������û�

    virtual void OnOK();

    DECLARE_MESSAGE_MAP()

    UINT m_port;
    CEdit m_event;
    UINT m_userOnlineCount;
    DWORD m_ip;
    afx_msg LRESULT OnNotifyIconMsg(WPARAM wParam, LPARAM lParam);//����֪ͨ����Ϣ
    afx_msg void OnOpenCloseServer();
    afx_msg void OnHide();
    afx_msg void OnExit();
    afx_msg void OnShow();
    afx_msg void OnClose();
    afx_msg void OnClearLog();
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
