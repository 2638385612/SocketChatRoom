
// mfcServer1Dlg.h : ͷ�ļ�
//

#pragma once
#include "mfcServer1.h"
#include "ServerSocket.h"
#include "../../mfcClient1/mfcClient1/MyMsg.h"
#include "../../mfcClient1/mfcClient1/RecvFile.hpp"
#include <fstream>
#include <map>

#define elif else if

#define WM_NOTIFYICONMSG WM_USER+3 //������Ϣ

class CServerSocket;
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
    HICON m_hIcon;

    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
private:
    const static int UserNumMax = 100; //�����¼���û��������
    struct _UserInfo {
        char userName[17];      //�û���
        char pwd[17];           //����
        bool isOnline;          //����״̬
        bool isRefused;         //�Ƿ񱻾ܾ���¼
    } userInfo[UserNumMax];
    typedef std::map<std::string, CServerSocket*> UserSocket;
    UserSocket user_socket;     //����<�û���,socket>ӳ���ϵ
    int userNum;                //������ע���û���
    CString userList;           //�û��б�,���û���¼ʱ�������û�
    CServerSocket* listenSocket;//���ڷ�����������������
    bool m_connect;             //���ڱ�Ƿ�����״̬
    CString fileSendName;       //��Ҫ���͵��ļ����ļ���
    bool readFileEnd;           //����ļ���ȡ�Ƿ����
    RecvFile rf;                //�����ļ�����
    struct SendFileTo {
        CString fileToUser;     //�������ݴ���ļ���Ŀ���û�
        CString fileFromUser;   //�������ݴ���ļ�����Դ�û�
        bool fileSendOver;      //��������Ҫת�����ļ��Ƿ������
        CString fileInfo;       //��Ҫ���͵��ļ�����Ϣ
        void set(const CString &to, const CString &from, bool over, const CString &info) {
            fileToUser = to;
            fileFromUser = from;
            fileSendOver = over;
            fileInfo = info;
        }
    } sft;              //������ת���ļ��Ľṹ
    NOTIFYICONDATA nd;  //֪ͨ��ͼ��

public:                 //��������Ա�����ⲿҲ��Ҫ���ʣ�������Ϊ����
    MyMsg mymsg;
    CString fileUser;   //���ļ�������ص��û�

public:
    void addClient();                       // �����û�����Ӧ�û�����
    void removeClient(const CString _user); // �Ƴ����ߵ��û�
    void receData(CServerSocket* pSocket);  // ��ȡ����
    void updateEvent(CString str, CString from = "������\t");// �����¼���־
    void sendMSG(CString str);              // ������Ϣ�������ͻ���
    void controlPC(CString AndroidControl); // �ֻ�����PC����Ӧ����
    void sendCloseMsg();  // ���͹رշ���������Ϣ
    int isUserInfoValid(bool CheckOnline = 0, bool onlyUser = 0, CString checkUser = "");  // ��֤�û����������Ƿ���Ч
    int getOnlineUserNums();   //�õ������û���Ŀ
    bool isUserOnline(CString _user);   //�õ��û��Ƿ����ߣ��������
    void fileSend(MyMsg& msg, bool NoAsk = 0);       //�յ��û�from�����ļ�������
    int fileTransfer(MyMsg& msg, const char* pData); //�ļ�����
    void modifyStatus(CString sta, bool _sleep = 1); //�޸�״̬��
    void sendFileToOthers(bool first = 0);  //���������ļ��������û�

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

    DECLARE_MESSAGE_MAP()
};
