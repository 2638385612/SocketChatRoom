#include "stdafx.h"
#include "mfcClient1.h"
#include "mfcClient1Dlg.h"
#include "ChatLogDlg.h"

#include <string>
using namespace std;

BEGIN_MESSAGE_MAP(CChatLogDlg, CDialogEx)
    ON_BN_CLICKED(IDC_DelAllLog, &CChatLogDlg::OnDelAllLog)
    ON_BN_CLICKED(IDC_Update, &CChatLogDlg::OnUpdateLog)
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
    ifstream tmpF(ClientInfo::myDIR + userID, ios::in);
    while (tmpF.is_open() && !tmpF.eof()) {
        getline(tmpF, _str);
        if (_str != "") {
            chatLog += _str.c_str();
            chatLog += "\r\n";
        }
    }
    tmpF.close();
    if (chatLog == "")
        chatLog = "��ʱ���κ������¼";
    return TRUE;
}

void CChatLogDlg::OnDelAllLog()
{
    if (chatLog != "��ʱ���κ������¼"
        && MessageBox("***ȷ��ɾ�����������¼�𣿴˲������ɳ��أ�***", "ɾ��ȷ��", MB_YESNO) == IDYES) {
        DeleteFile(ClientInfo::myDIR + userID);
        MessageBox("��ɾ�����������¼��", "��ܰ��ʾ");
    } else if (chatLog == "��ʱ���κ������¼") {
        MessageBox("��ʱ���κ������¼", "��ܰ��ʾ");
    } else {
        return;
    }
    ::SendMessage(GetSafeHwnd(), WM_CLOSE, 0, 0);
}

void CChatLogDlg::OnUpdateLog()
{
    chatLog = "";
    string _str;
    ifstream tmpF(ClientInfo::myDIR + userID, ios::in);
    while (!tmpF.eof() && tmpF.is_open()) {
        getline(tmpF, _str);
        if (_str != "")
            chatLog += (_str+"\r\n").c_str();
    }
    tmpF.close();
    if (chatLog == "")
        chatLog = "�����κ������¼";
    SetDlgItemText(IDC_Chatlog, "");
    int lastLine = p_editCL->LineIndex(p_editCL->GetLineCount() - 1);
    p_editCL->SetSel(lastLine + 1, lastLine + 2, 0);	//ѡ��༭�����һ��
    p_editCL->ReplaceSel(chatLog);   //�滻��ѡ��һ�е�����
}
