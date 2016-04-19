
// mfcClient1Dlg.h : ͷ�ļ�
//

#pragma once
#include "ClientSocket.h"
#include "LoginDlg.h"
#include "ChatLogDlg.h"

#include "MyMsg.h"
#include "RecvFile.hpp"

#define elif else if
#define MBox(s) MessageBox(s,"��ܰ��ʾ")

class CmfcClient1Dlg;
class ClientInfo
{
public:
    ClientInfo();
public:
    CString m_userID, m_pw; //�û���,����
    bool m_connected;       //��ǰ�����ӵ���������
    static int userNum;     //��ע���û�����

    CString m_DataSend;     //��Ҫ���͵���Ϣ
    CString m_msgTo;        //��Ϣ��Ŀ���û�
    bool m_toUserIsOnline;  //Ŀ���û��Ƿ����ߣ�

    CString m_ip;           //���ӵ���ip��ַ
    int m_port;             //���ӵ��Ķ˿ں�

    RecvFile m_rf;          //���ڽ����ļ��Ķ���
    CString m_fileUser;     //���ļ�����������û�
    CString m_fileSendName; //Ҫ���͵��ļ��������ļ���
    int m_fileTimeOut;      //�ļ����䳬ʱʱ��������
    bool m_readFileEnd;     //��ȡ�ļ�������

    CClientSocket* pSock;   //�ͻ����׽��ֶ���ָ��
    CmfcClient1Dlg* pDlg;   //������ָ��

    static CString myDIR;   //�����ļ��У�λ����ʱĿ¼
};


// CmfcClient1Dlg �Ի���
class CmfcClient1Dlg : public CDialogEx, public ClientInfo
{
public:
    CmfcClient1Dlg(CWnd* pParent = NULL);
    ~CmfcClient1Dlg();

    enum {
        IDD = IDD_MFCCLIENT1_DIALOG
    };

protected:
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV ֧��
    virtual void OnOK();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnInitDialog();

public:
    HICON m_hIcon;
    CChatLogDlg* pChatlog;
    CButton m_ConPC;        //���ӷ�������ť
    NOTIFYICONDATA nd;      //֪ͨ��ͼ��
    CComboBox m_cbMsgTo;    //���͸�������
    MyMsg mymsg;
    bool autoConnect;       //�Ƿ���ִ���Զ���ʱ���ӷ������Ĺ���
public:
    //���յ���Ϣ����Ҫ����
    void receData();
    //���ͺ��������ڷ������ݸ�������
    void sendMSG(const CString &send, bool upEvent = 1);
    //������Ϣ���
    void updateEvent(const CString &showMsg, const CString &from = "������:",
                     bool reset = 0, int timeFMT = 2);
    //���յ������ļ�������ʱ
    void fileSend(MyMsg& msg);
    //�޸�״̬��
    void modifyStatus(const CString &sta, bool _sleep = 1);
    //�����ļ����ݲ������ļ�����
    void save_SendFileInfo(const CString &filepath, long size);
    //������¼�Ի���Ҫ���¼
    bool login();

    // ��Ϣӳ�亯��
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    //��ʱ����Ӧ����
    afx_msg void OnTimer(UINT nIDEvent);
    //���ӷ�������ť��Ӧ����
    afx_msg void OnConnect();
    //���Ͱ�ť��Ӧ����
    afx_msg void OnSend();
    //ע����ť��Ӧ����
    afx_msg void OnLogoff();
    //�����¼��ť��Ӧ����
    afx_msg void OnChatlog();
    //�ı�Ŀ���û�����ѡ������
    afx_msg void OnCbnSelChangeMsgTo();
    //�������ļ�
    afx_msg void OnDropFiles(HDROP hDropInfo);
    DECLARE_MESSAGE_MAP()
};
