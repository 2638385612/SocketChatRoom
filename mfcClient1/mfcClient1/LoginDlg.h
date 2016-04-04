#pragma once
#ifndef _LOGINDLG_H_
#define _LOGINDLG_H_

#include <afxdialogex.h>

#define IP_LAN		 "172.27.35.3"
#define IP_LOCALHOST "127.0.0.1"
#define IP_SERVER	 "120.25.207.230"

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

    bool onlyAlNum(const CString &s);
    bool dataInvalide();
    afx_msg void OnOk();
    afx_msg void OnSet();
    afx_msg void OnRegist();
    afx_msg void OnCheckRemPW();
    afx_msg void OnSwap();
    afx_msg void OnShowPW();

    DECLARE_MESSAGE_MAP()
};


#endif // !_LOGINDLG_H_
