
// mfcClient1Dlg.h : ͷ�ļ�
//

#pragma once
#include "ClientSocket.h"
#include "LoginDlg.h"
#include "ChatLogDlg.h"


extern CString myDIR;       //ȫ���ⲿ�����������ļ��У�λ����ʱĿ¼
class CmfcClient1Dlg;
class ClientInfo
{
public:
    CString m_userID, m_pw; //�û���,����
    int userNum;            //��ע���û�����
    CString m_DataSend;     //��Ҫ���͵���Ϣ
    CString m_msgTo;        //��Ϣ��Ŀ���û�
    bool to_isOnline;       //Ŀ���û��Ƿ����ߣ�

    int m_port;
    CString m_ip;
    bool logining;          //��ǰ���ڵ�½��
    bool m_connect;         //��ǰ�����ӵ���������
    bool firstCon;          //��һ�����ӣ�

    RecvFile rf;            //�����ļ�����
    CString fileUser;       //���ļ�����������û�
    CString fileSendName;   //Ҫ���͵��ļ��������ļ���
    int fileTimeOut;        //�ļ���ʱʱ��
    bool readFileEnd;       //��ȡ�ļ�������

    CString myDIR;          //�����ļ��У�λ����ʱĿ¼
    CClientSocket* pSock;   //�ͻ����׽��ֶ���ָ��
    CmfcClient1Dlg* pDlg;   //������ָ��
public:
    ClientInfo() {
        pDlg = 0;
        firstCon = 1;
        logining = 0;
        readFileEnd = 0;
        m_connect = 0;
        m_DataSend = m_userID = "";

        char path[2048] = "";
        GetTempPath(2048, path);
        strcat(path, "mfcClient1");
        CreateDirectory(path, 0);
        AfxGetApp()->WriteProfileString("ClientSetting", "tempDir", path);
        myDIR = path + CString("\\");
        ::myDIR = myDIR;
        DeleteFile(myDIR + "send.txt");
        DeleteFile(myDIR + "TransLog.txt");
    }
    virtual ~ClientInfo() {
        if (m_connect) {
            pSock->SendMSG(pSock->mymsg.join("", TYPE[Logout], "", "", "", m_pw), 0);
            pSock->Close();
            delete pSock;
        }
    }
    //�����ļ����ݲ������ļ�����
    void save_SendFileInfo(const char* filepath, long size);
    //��ʾ��¼�Ի���Ҫ���¼
    bool login();
    //�޸�״̬��
    void modifyStatus(CString sta, bool _sleep = 1);
};


// CmfcClient1Dlg �Ի���
class CmfcClient1Dlg : public CDialogEx, public ClientInfo
{
public:
    CmfcClient1Dlg(CWnd* pParent = NULL) : ClientInfo(), CDialogEx(CmfcClient1Dlg::IDD, pParent) {
        pChatlog = 0;
    }
    ~CmfcClient1Dlg();
    // �Ի�������
    enum { IDD = IDD_MFCCLIENT1_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    virtual void OnOK();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    // ʵ��
public:
    HICON m_hIcon;
    CChatLogDlg* pChatlog;
    CButton m_ConPC;        //���ӷ�������ť
    NOTIFYICONDATA nd;      //֪ͨ��ͼ��
    CComboBox m_cbMsgTo;    //���͸�������

    // ���ɵ���Ϣӳ�亯��
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg HCURSOR OnQueryDragIcon();

    afx_msg void OnConnect();
    afx_msg void OnSend();
    afx_msg void OnLogoff();
    afx_msg void OnChatlog();
    afx_msg void OnCbnSelChangeMsgTo();
    afx_msg void OnDropFiles(HDROP hDropInfo);
    DECLARE_MESSAGE_MAP()
};
