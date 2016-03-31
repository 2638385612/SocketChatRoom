#pragma once
#ifndef _CHATLOGDLG_H_
#define _CHATLOGDLG_H_

#include <afxdialogex.h>

//��Ϣ��¼
class CChatLogDlg : public CDialogEx
{
public:
    CChatLogDlg(CString id) : CDialogEx(CChatLogDlg::IDD) { userID = id; }
    // �Ի�������
    enum { IDD = IDD_ChatLog };
protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
    virtual BOOL OnInitDialog();
public:
    CEdit* p_editCL;
    CString userID, chatLog;
    DECLARE_MESSAGE_MAP()
    afx_msg void OnDelAllLog();
    afx_msg void OnUpdateLog();
};

#endif // !_CHATLOGDLG_H_
