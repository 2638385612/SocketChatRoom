
// mfcClient1Dlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "ClientSocket.h"
#include "afxcmn.h"


// CLoginDlg (��½)�Ի���
class CLoginDlg : public CDialogEx
{
public:
    CLoginDlg() : CDialogEx(CLoginDlg::IDD), userID(_T("")), pw(_T("")), port(22783) {}
    // �Ի�������
    enum { IDD = IDD_LOGIN };
protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
    virtual BOOL OnInitDialog();
public:
    bool b_strech;	//�Ƿ���չ����չ��
    CRect m_rc;
    CString userID, pw;
    CIPAddressCtrl m_IPAddr;
    CString ip;
    int port;

    bool onlyAlNum(const CString s);
    DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedSet();
    afx_msg void OnBnClickedregist();
    bool DataInvalide();
    afx_msg void OnBnClickedCheckrempw();
    afx_msg void OnBnClickedSwap();
    afx_msg void OnBnClickedShowpw();
};

extern CString myDIR;		//ȫ���ⲿ�����������ļ��У�λ����ʱĿ¼
class CmfcClient1Dlg;
class CClientDlg
{
public:
    CString m_userID,m_pw;	//�û���,����
    int userNum;			//��ע���û�����
    CString m_DataSend;		//��Ҫ���͵���Ϣ
    CString m_msgTo;		//��Ϣ��Ŀ���û�
    bool to_isOnline;		//Ŀ���û��Ƿ����ߣ�

    int m_port;
    CString m_ip;
    CLoginDlg login;		//���ӵ����Ի���
    bool logining;			//��ǰ���ڵ�½��
    bool m_connect;			//��ǰ�����ӵ���������
    bool firstCon;			//��һ�����ӣ�

    RecvFile rf;			//�����ļ�����
    CString fileUser;		//���ļ�����������û�
    CString fileSendName;	//Ҫ���͵��ļ��������ļ���
    int fileTimeOut;		//�ļ���ʱʱ��
    bool readFileEnd;		//��ȡ�ļ�������

    CString myDIR;			//�����ļ��У�λ����ʱĿ¼
    CClientSocket* pSock;   //�ͻ����׽���ָ�����
    CmfcClient1Dlg* pDlg;	//������ָ��
public:
    CClientDlg() {
        pDlg = 0;
        firstCon = 1;
        logining = 0;
        readFileEnd = 0;
        m_connect = 0;
        m_DataSend = m_userID = "";

        char path[2048] = "";
        GetTempPath(2048,path);
        strcat(path,"mfcClient1");
        CreateDirectory(path,0);
        AfxGetApp()->WriteProfileString("ClientSetting", "tempDir", path);
        myDIR = path + CString("\\");
        ::myDIR = myDIR;
        DeleteFile(myDIR+"send.txt");
        DeleteFile(myDIR+"TransLog.txt");
    }
    virtual ~CClientDlg() {
        if (m_connect) {
            pSock->SendMSG( pSock->mymsg.join("",TYPE[Logout],"","","",m_pw) ,0);
            pSock->Close();
            delete pSock;
        }
    }
    //�����ļ����ݲ������ļ�����
    void save_SendFileInfo(const char* filepath,long size);
    //��¼�Ի���
    int  loginDlg();
    //�޸�״̬��
    void modifyStatus(CString sta,bool _sleep=1);
};

//��Ϣ��¼
class CChatLogDlg : public CDialogEx
{
public:
    CChatLogDlg(CString id) : CDialogEx(CChatLogDlg::IDD) {
        userID = id;
    }
    // �Ի�������
    enum { IDD = IDD_ChatLog };
protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
    virtual BOOL OnInitDialog();
public:
    CEdit* p_editCL;
    CString userID,chatLog;
    DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedDelalllog();
    afx_msg void OnBnClickedUpdate();
};

// CmfcClient1Dlg �Ի���
class CmfcClient1Dlg : public CDialogEx,public CClientDlg
{
public:
    CmfcClient1Dlg(CWnd* pParent = NULL): CClientDlg(),CDialogEx(CmfcClient1Dlg::IDD, pParent) {
        pChatlog = 0;
    }
    ~CmfcClient1Dlg();
// �Ի�������
    enum { IDD = IDD_MFCCLIENT1_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
    virtual void OnOK();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

// ʵ��
public:
    HICON m_hIcon;
    CChatLogDlg* pChatlog;
    CButton m_ConPC;		//���ӷ�������ť
    NOTIFYICONDATA nd;		//֪ͨ��ͼ��
    CComboBox m_cbMsgTo;	//���͸�������

    // ���ɵ���Ϣӳ�亯��
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnBnClickedConnect();
    afx_msg void OnBnClickedSend();
    afx_msg void OnBnClickedLogoff();
    afx_msg void OnBnClickedChatlog();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnCbnSelChangeMsgTo();
    afx_msg void OnDropFiles(HDROP hDropInfo);
};
