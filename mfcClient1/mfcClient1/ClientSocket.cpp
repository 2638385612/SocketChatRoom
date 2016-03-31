#include "stdafx.h"
#include "ClientSocket.h"
#include "mfcClient1.h"
#include "mfcClient1Dlg.h"
#include "md5.h"

using namespace std;

CString CClientSocket::userID = "";

char packageData[MAX_PACKAGE_NUM][2 * PACKAGE_SIZE + 1];

CString TYPE[30] = { TYPE_ChatMsg , TYPE_Server_is_closed , TYPE_UserList , TYPE_OnlineState , TYPE_FileSend , TYPE_FileData , TYPE_AskFileData , TYPE_File_NO , TYPE_File_Over , TYPE_File_Fail , TYPE_LoginFail , TYPE_UserIsOnline , TYPE_OfflineMsg , TYPE_AllUser , TYPE_AddUserList , TYPE_I_am_online , TYPE_Logout , TYPE_Login , TYPE_Register , TYPE_Status };
CString STR[5] = { "@@@","<<<",">>>","&&&","###" };//�û���+����+����+ȥ��+����+����

#define GETS(y,z) AfxGetApp()->GetProfileString("ClientSetting",y,z)
#define WRITE(x,y,z) { char *s = x;if(x==0) s="ClientSetting"; AfxGetApp()->WriteProfileString(s,y,z);}
//x��������ͱ�����y���룬z��ֵ��Ĭ��ֵ
#define GET_WRITE(x,y,z) {\
	if((x=atoi(GETS(y,"-1")))==-1){\
		x = atoi(z);\
		WRITE(0,y,z);\
	}\
}
//����������ʹ����theApp,��˲��ܷ�����������
void RecvFile::addPacketage(const char *data)
{
    static CmfcClient1Dlg* pDlg = (CmfcClient1Dlg*)theApp.GetMainWnd();
    if (packageRecv < packageNum) {
        packageRecv++;
        if (packageRecv < packageNum)		//�������ݰ�û�н���
            pDlg->pSock->SendMSG(pDlg->pSock->mymsg.join(getPackRecv(), TYPE[AskFileData], "", "", pDlg->fileUser), 0);//������һ�����ݰ�
        strcpy_s(packageData[packageRecv - 1], data);
        static int showTransLog;//��ʾ������־
        GET_WRITE(showTransLog, "showTransLog", "0");
        if (showTransLog) {
            CString str;
            str.Format("���յ�%d�����ݰ�,packageNum=%d", packageRecv, packageNum);
            pDlg->pSock->updateEvent(str, "ϵͳ֪ͨ");
        }
    }
    static long timeNow = timeStart;
    if (pDlg != 0) {
        if (clock() - timeNow>400) {
            timeNow = clock();
            CString str;
            str.Format("�ļ��ѽ��� %.1f%%��    ��ʱ %.1fs   ƽ���ٶ� %.1fk/s", 100.0 * packageRecv / packageNum
                       , (clock() - timeStart) / 1000.0, 1.0*packageRecv / packageNum*fileLength / ((clock() - timeStart)));
            pDlg->modifyStatus(str, 0);
        }
    }
    if (packageNum == packageRecv)
        saveFile(clock() - timeStart);
}
void RecvFile::saveFile(int useTime)
{
    static CmfcClient1Dlg* pDlg = (CmfcClient1Dlg*)theApp.GetMainWnd();
    static HWND hWnd = pDlg->GetSafeHwnd();
    if (packageNum == packageRecv) {
        pDlg->modifyStatus("����У���ļ����ݣ�", 0);
        CXXFStream out(fileNewName, ios::out | ios::binary);
        for (int i = 0; i < packageRecv - 1; i++) {
            out.writeString(packageData[i], PACKAGE_SIZE);
        }
        if (fileLength % PACKAGE_SIZE == 0)	//���һ����������
            out.writeString(packageData[packageNum - 1], PACKAGE_SIZE);
        else								//���һ��û�������
            out.writeString(packageData[packageNum - 1], fileLength % PACKAGE_SIZE);
        out.close();
        static MD5 md5;
        char newFileMD5[33] = "";
        md5.fileMd5(newFileMD5, (LPCTSTR)fileNewName);
        if (fileMD5 == CString(newFileMD5)) {
            pDlg->pSock->SendMSG(pDlg->pSock->mymsg.join(TYPE[File_Over], TYPE[AskFileData], "", "", pDlg->fileUser), 0);
            timeMsg.Format("    ��ʱ %.1fs   ƽ���ٶ� %.1fk/s", useTime / 1000.0, 1.0*fileLength / useTime);
            pDlg->modifyStatus("�ļ��ѽ�����ϣ�" + timeMsg, 0);
            pDlg->pSock->updateEvent("�ļ�\"" + fileNewName + "\"�ѽ�����ϣ�" + timeMsg, "ϵͳ֪ͨ");
            MessageBox(hWnd, "���ղ������ļ��ɹ�", "��ܰ��ʾ", 0);
        } else {
            pDlg->pSock->SendMSG(pDlg->pSock->mymsg.join(TYPE[File_Fail], TYPE[AskFileData], "", "", pDlg->fileUser), 0);
            pDlg->pSock->updateEvent("���ջ򱣴��ļ�\"" + fileNewName + "\"ʧ�ܣ����Ժ����ԣ���������룺0x00041)", "ϵͳ֪ͨ");
            MessageBox(hWnd, "���ջ򱣴��ļ�ʧ�ܣ����Ժ����ԣ���������룺0x00041)", "��ܰ��ʾ", 0);
            pDlg->modifyStatus("�ļ�����ʧ��", 0);
            DeleteFile(fileNewName);
        }
    }
    recvEnd();
}

CClientSocket::CClientSocket(CString _user)
{
    pDlg = (CmfcClient1Dlg*)theApp.GetMainWnd();
    hWnd = pDlg->GetSafeHwnd();
    userID = _user;
}

CClientSocket::~CClientSocket()
{}

void CClientSocket::updateEvent(CString showMsg, CString from, bool reset, int timeFMT)
{
    CString str, sysMsg;
    CmfcClient1Dlg* pDlg = (CmfcClient1Dlg*)theApp.GetMainWnd();
    pDlg->GetDlgItemText(IDC_DataReceive, str);
    CTime time = CTime::GetCurrentTime();	// ��ȡϵͳ��ǰʱ��
    if (str == "" && timeFMT == 2) //��ϢΪ������ʾ���ں�ʱ��
        timeFMT = 1;
    switch (timeFMT) {
    case 1:
        sysMsg = from + "  " + time.Format(_T("%Y/%m/%d %H:%M:%S  "));
        break;
    case 2:
        sysMsg = from + "  " + time.Format(_T("%H:%M:%S  "));
        break;
    case 3:
        sysMsg = from == "" ? from : from + "  ";
    default:
        break;
    }
    if (str != "" && reset) {
        str = "����������������������������������ʷ��Ϣ��������������������������\r\n";
    } else {
        if (showMsg != "")
            str = sysMsg + showMsg + "\r\n";
    }
    CEdit* m_event = (CEdit*)pDlg->GetDlgItem(IDC_DataReceive);
    int lastLine = m_event->LineIndex(m_event->GetLineCount() - 1);//��ȡ�༭�����һ������
    m_event->SetSel(lastLine + 1, lastLine + 2, 0);	//ѡ��༭�����һ��
    m_event->ReplaceSel(str);                     //�滻��ѡ��һ�е�����
    ofstream myDIR_userID(myDIR + userID, ios::out | ios::app);
    myDIR_userID << str;
    myDIR_userID.close();
    pDlg->GetDlgItemText(IDC_DataReceive, str);
    if (str.GetLength() > 15000) {
        str = str.Right(1000);
        pDlg->SetDlgItemText(IDC_DataReceive, str);
        m_event->SetSel(lastLine + 1, lastLine + 2, 0);	//ѡ��༭�����һ��
        m_event->ReplaceSel("");                     //�滻��ѡ��һ�е�����
    }
}

//	˵�������ͻ��˽��յ��������˷�������ʱ����Ӧ���պ���OnReceive
void CClientSocket::OnReceive(int nErrorCode)
{
    pDlg->KillTimer(1);
    pDlg->KillTimer(4);
    pDlg->GetDlgItem(IDC_DataSend)->SetFocus();
    char pData[DATA_BUF_SIZE] = ""; //������յ�������
    Receive(pData, DATA_BUF_SIZE);
    mymsg.load(pData);
    MyMsg msg = mymsg;
    static const CString myLogFile = myDIR + "ClientLog-" + userID + ".txt";
    static int del_one_time = DeleteFile(myLogFile);
    static ofstream logFile(myLogFile, ios::out | ios::app);
    logFile << pData << endl;
    static int showEveryMsg;//���洫����־
    GET_WRITE(showEveryMsg, "showEveryMsg", "0");
    if (showEveryMsg == 1)
        MessageBox(0, pData, "�յ���Ϣ", 0);
    if (msg.type == TYPE[Server_is_closed]) {
        sprintf_s(pDlg->nd.szTip, "�ͻ��ˣ�%s - ����", (LPCTSTR)userID);
        Shell_NotifyIcon(NIM_MODIFY, &pDlg->nd);
        if (pDlg->m_connect) {    // ����Ѿ����ӣ���Ͽ�������
            pDlg->KillTimer(2);
            if (pDlg->rf.isRecving()) {
                pDlg->pSock->SendMSG(msg.join(TYPE[File_Fail], TYPE[AskFileData], pDlg->fileUser), 0);
                pDlg->rf.recvEnd();
            }
            pDlg->m_connect = false;
            pDlg->pSock->Close();
            pDlg->pSock = NULL;
            pDlg->m_ConPC.SetWindowText(_T("���ӷ�����"));
            pDlg->UpdateData(false);
            pDlg->GetDlgItem(IDC_DataReceive)->EnableWindow(0);	//������Щ�ؼ�
            pDlg->GetDlgItem(IDC_DataSend)->EnableWindow(0);
            pDlg->GetDlgItem(IDC_Send)->EnableWindow(0);
            pDlg->GetDlgItem(IDC_msgTo)->EnableWindow(0);
        }
        pDlg->modifyStatus("Server is closed!");
        pDlg->firstCon = 1;
        pDlg->SetTimer(4, 2000, 0);
    }
    elif(msg.userId == userID) { //���Լ�����Ϣ
        if (pDlg->logining) {	//���ڵ�½���ǰһ�ε����ӶϿ�
            pDlg->pSock->SendMSG(msg.join("", TYPE[Logout], "", "", "", pDlg->m_pw), 0);
        } else {
            //MessageBox(0,msg.data,"",0);
            if (msg.type == TYPE[ChatMsg]) {
                updateEvent(msg.data, "[" + msg.fromUser + "]:");
                pDlg->modifyStatus("�յ�[" + msg.fromUser + "]������Ϣ��");
            }
            elif(msg.type == TYPE[UserList]) {	//�յ����û��б����ʾ��¼�ɹ�
                pDlg->userNum = 0;
                pDlg->m_cbMsgTo.ResetContent();
                int i = msg.data.Find(";");
                CString newUser;
                do {
                    newUser = msg.data.Left(i);
                    if (newUser != userID) {	//�ѳ��Լ�������û���ӵ��û��б�
                        pDlg->m_cbMsgTo.AddString(newUser);
                        pDlg->userNum++;
                    }
                    msg.data = rightN(msg.data, i + 1);//msg.data.Right(msg.GetLength()-i-1);
                    i = msg.data.Find(";");
                } while (i != -1);
                pDlg->m_cbMsgTo.AddString("������");
                pDlg->m_cbMsgTo.AddString("����������");
                pDlg->m_cbMsgTo.SetCurSel(pDlg->m_cbMsgTo.GetCount() - 1);
                pDlg->m_cbMsgTo.GetLBText(pDlg->m_cbMsgTo.GetCurSel(), pDlg->m_msgTo);	//��ȡ��Ϣ���͵�Ŀ���û�
                if (pDlg->m_msgTo == "������" || pDlg->m_msgTo == "����������") {
                    pDlg->modifyStatus("�����ӵ�������. �������[" + pDlg->m_msgTo + "]���ɷ�����Ϣ");
                    pDlg->to_isOnline = 1;
                } else
                    SendMSG(msg.join(pDlg->m_msgTo, TYPE[OnlineState], "", "", "������"), 0);
                updateEvent(msg.data, "", 1);
                pDlg->KillTimer(0);
                pDlg->GetDlgItem(IDC_DataReceive)->EnableWindow(1);
                pDlg->GetDlgItem(IDC_DataSend)->EnableWindow(1);
                pDlg->GetDlgItem(IDC_Connect)->EnableWindow(1);
                pDlg->GetDlgItem(IDC_Send)->EnableWindow(1);
                pDlg->GetDlgItem(IDC_msgTo)->EnableWindow(1);
                sprintf_s(pDlg->nd.szTip, "�ͻ��ˣ�%s - ����", (LPCTSTR)userID);	//״̬�ڵ�½����ʱ�õ�
                Shell_NotifyIcon(NIM_MODIFY, &pDlg->nd);
                pDlg->m_connect = true;
                pDlg->m_ConPC.SetWindowText(_T("�Ͽ�������"));
                pDlg->UpdateData(false);
            }
            elif(msg.type == TYPE[OnlineState]) {
                CString sel;
                pDlg->GetDlgItemText(IDC_msgTo, sel);
                if (msg.data == "1") {
                    pDlg->modifyStatus("[" + sel + "]��ǰ����");
                    pDlg->to_isOnline = 1;
                } else if (msg.data == "0") {
                    pDlg->modifyStatus("[" + sel + "]��ǰ������");
                    pDlg->to_isOnline = 0;
                }
            }
            elif(msg.type == TYPE[FileSend]) {
                //������Խ��ܷ����������Լ����ļ�����
                pDlg->SetForegroundWindow();
                fileSend(msg);
            }
            elif(msg.type == TYPE[FileData]) {
                if (pDlg->rf.isRecving()) {
                    pDlg->KillTimer(2);
                    pDlg->rf.addPacketage(msg.data);
                    pDlg->SetTimer(2, pDlg->fileTimeOut, 0);
                }
            }
            elif(msg.type == TYPE[AskFileData]) {
                static fstream dataTrans;
                static int saveTrans = atoi(AfxGetApp()->GetProfileString("ClientSetting", "saveTransLog", "-1"));//���洫����־
                if (saveTrans == -1) {
                    AfxGetApp()->WriteProfileString("ClientSetting", "saveTransLog", "0");
                    saveTrans = 0;
                }
                if (msg.data == TYPE[File_NO])	//�ܾ����ո��ļ�
                    updateEvent("[" + msg.fromUser + "]�ܾ������ļ�����" + pDlg->fileSendName + "��", "[" + msg.fromUser + "]");
                else if (msg.data == TYPE[File_Over]) {
                    updateEvent("[" + msg.fromUser + "]�ѽ����ļ�����" + pDlg->fileSendName + "��", "[" + msg.fromUser + "]");
                    if (saveTrans) dataTrans.close();
                } else if (msg.data == TYPE[File_Fail]) {
                    updateEvent("[" + msg.fromUser + "]δ�ܳɹ������ļ�����" + pDlg->fileSendName + "��", "[" + msg.fromUser + "]");
                    if (saveTrans) dataTrans.close();
                } else {			//���������ݰ�
                    int dataIndex = atoi(msg.data);
                    while (0 == dataIndex && pDlg->readFileEnd == 0) {
                        Sleep(10);
                        static MSG msg1;
                        if (PeekMessage(&msg1, (HWND)NULL, 0, 0, PM_REMOVE)) {
                            ::SendMessage(msg1.hwnd, msg1.message, msg1.wParam, msg1.lParam);
                        }
                    }	//�ȴ���ȡ�ļ����ݽ���
                    if (dataIndex >= 0 && dataIndex < MAX_PACKAGE_NUM) {
                        if (0 == dataIndex)
                            pDlg->fileUser = msg.fromUser;
                        if (saveTrans) {
                            if (0 == dataIndex)
                                dataTrans.open(myDIR + "TransLog.txt", ios::out);
                            dataTrans << "�������ݰ� " << dataIndex << "\t  " << strlen(packageData[dataIndex]) << endl;
                        }
                        SendMSG(msg.join(packageData[dataIndex], TYPE[FileData], "", "", pDlg->fileUser), 0);
                        static int showTransLog;//��ʾ������־
                        GET_WRITE(showTransLog, "showTransLog", "0");
                        if (showTransLog) {
                            CString str;
                            str.Format("�ѷ������ݰ�%d(%d),packageNum=%d", dataIndex, strlen(packageData[dataIndex]), pDlg->rf.getPackNum());
                            pDlg->pSock->updateEvent(str, "ϵͳ֪ͨ");
                        }
                        //MessageBox(packageData[dataIndex],"tmpMsg");
                    }
                }
            }
            elif(msg.type == TYPE[LoginFail]) {
                //�û���֤ʧ��
                if (!pDlg->logining) {	//û���ڵ�¼�Ž�����Щ���������ڵ�¼�򲻽��ո���Ϣ
                    pDlg->KillTimer(0);
                    MessageBox(hWnd, "�û���������������飡\r\n���û���˺���ע�����˺ţ�", "��½ʧ��", 0);
                    pDlg->ShowWindow(SW_HIDE);
                    pDlg->login();
                    pDlg->ShowWindow(SW_SHOW);
                }
            }
            elif(msg.type == TYPE[UserIsOnline]) {
                //�û��Ѿ�����
                if (0==strstr(pDlg->nd.szTip, "����")) {//�ܾ����ڵ�½���û�
                    pDlg->KillTimer(0);
                    MessageBox(hWnd, "���û��Ѿ�����,�����ظ���½��", "��½ʧ��", 0);
                    pDlg->ShowWindow(SW_HIDE);
                    pDlg->login();
                    pDlg->ShowWindow(SW_SHOW);
                } else
                    MessageBox(hWnd, "����˺�����һ����½����������㱾�˲������뼰ʱ�޸����룡", "��ȫ��ʾ", 0);
            }
            elif(msg.type == TYPE[OfflineMsg]) {	//������Ϣ
                static MyMsg olmsg;
                CString tmpMsg(msg.data);
                do {
                    tmpMsg = olmsg.load(tmpMsg, 1);
                    updateEvent(olmsg.type + "  " + olmsg.data, "[" + olmsg.fromUser + "]:", 0, 3);
                } while (tmpMsg.Find(STR[0]) != -1);
                updateEvent("��������������������������������������Ϣ��������������������������", "", 0, 3);
                pDlg->modifyStatus("�յ�������Ϣ��");
            }
            elif(msg.type == TYPE[Status]) { //�޸�״̬
                pDlg->modifyStatus(msg.data);
            }
        }
    }
    elif(msg.userId == TYPE[AllUser]) {	//���������û���
        if (msg.type == TYPE[ChatMsg]) {
            if (msg.fromUser != ("������-" + userID)) { //�����Լ��յ������Լ�����Ϣ
                updateEvent(msg.data, "[" + msg.fromUser + "]:");
                pDlg->modifyStatus("�յ�[" + msg.fromUser + "]������Ϣ��");
            }
        }
        elif(msg.type == TYPE[AddUserList]) {
            pDlg->m_cbMsgTo.InsertString(pDlg->m_cbMsgTo.GetCount() - 2, msg.data);
            pDlg->userNum++;
        }
        elif(msg.type == TYPE[FileSend]) { //������Խ��ܷ�����Ⱥ�����ļ�����
            pDlg->SetForegroundWindow();
            fileSend(msg);
        }
    }
    pDlg->SetTimer(1, 2000, 0);	//����
    CSocket::OnReceive(nErrorCode);
}

/*SendMSG���������������������Ϣ�������������Ի������е��á�*/
void CClientSocket::SendMSG(CString send, bool upEvent)
{
    static MyMsg tmp_msg;
    tmp_msg.load(send);
    //MessageBox(0,send,"�ͻ��˷���",0);
    if (Send(send, send.GetLength() + 1) == SOCKET_ERROR) {	//������Ϣʧ��
        CString err = getLastErrorStr();
        if (tmp_msg.type != TYPE[I_am_online]) {
            MessageBox(hWnd, "����ʧ�ܣ�" + err, "��ܰ��ʾ", 0);
        } else {
            pDlg->KillTimer(1);
            sprintf_s(pDlg->nd.szTip, "�ͻ��ˣ�%s - ����", (LPCTSTR)userID);
            Shell_NotifyIcon(NIM_MODIFY, &pDlg->nd);
            if (pDlg->m_connect) {    // ����Ѿ����ӣ���Ͽ�������
                pDlg->m_connect = false;
                pDlg->pSock->Close();
                pDlg->pSock = NULL;
                pDlg->m_ConPC.SetWindowText(_T("���ӷ�����"));
                pDlg->UpdateData(false);
                pDlg->GetDlgItem(IDC_DataReceive)->EnableWindow(0);	//������Щ�ؼ�
                pDlg->GetDlgItem(IDC_DataSend)->EnableWindow(0);
                pDlg->GetDlgItem(IDC_Send)->EnableWindow(0);
                pDlg->GetDlgItem(IDC_msgTo)->EnableWindow(0);
            }
            pDlg->firstCon = 1;
            pDlg->SetTimer(4, 2000, 0);
            MessageBox(hWnd, "������������쳣���볢������: " + err, "��ܰ��ʾ", 0);
        }
        return;
    }
    if (tmp_msg.type != TYPE[I_am_online]) {
        static ofstream logFile(myDIR + "send.txt", ios::out | ios::app);
        logFile << "send:" << send << endl;
    }
    if (upEvent && tmp_msg.pw == "") {	//û�����벿��
        if (tmp_msg.type == TYPE[ChatMsg]) {
            static CString lastUser = "", nowUser = "";
            nowUser = tmp_msg.toUser;
            CString data = tmp_msg.data;
            if (nowUser != lastUser) {
                lastUser = nowUser;
                tmp_msg.data = "����������������������[" + nowUser + "]���졪������������������";
                updateEvent(tmp_msg.data, "", 0, 3);
            }
            tmp_msg.data = data;
            updateEvent(tmp_msg.data, "��:");
        }
        CmfcClient1Dlg* pDlg = (CmfcClient1Dlg*)theApp.GetMainWnd();
        pDlg->m_DataSend = "";
        pDlg->SetDlgItemText(IDC_DataSend, "");	// ��շ��ʹ��ڵ�����
    }
}

void CClientSocket::fileSend(MyMsg& msg)
{
    int i = msg.data.Find('|');
    CString name = msg.data.Left(i);
    CString size = rightN(msg.data, i + 1);
    i = size.Find('|');
    CString fileMD5 = rightN(size, i + 1);
    size = size.Left(i);
    int fileSize = atoi(size);
    if (fileSize > 1024 * 1024)
        size.Format("%.2f MB", fileSize / 1024.0 / 1024.0);
    else if (fileSize > 1024)
        size.Format("%.2f KB", fileSize / 1024.0);
    else
        size.Format("%d �ֽ�", fileSize);
    if (pDlg->rf.isRecving()) {
        MessageBox(hWnd, '[' + msg.fromUser + "] ���㷢���ļ���\n�ļ�����" + name + "\n�ļ���С��" + size +
                   "\n��ǰ���ڽ����ļ�����ȴ���ǰ�ļ��������ٽ��������ļ�����ֹ��ǰ�ļ�����", "��ܰ��ʾ", 0);
        return;
    }
    GET_WRITE(pDlg->fileTimeOut, "fileTimeOut", "2000");
    if (MessageBox(hWnd, '[' + msg.fromUser + "] ���㷢���ļ���\n�ļ�����" + name + "\n�ļ���С��" + size + "\n�Ƿ�ͬ����գ�",
                   "��ܰ��ʾ", MB_YESNO | MB_ICONQUESTION) == IDYES) {
        CString fmt = "*" + name.Right(name.GetLength() - name.ReverseFind('.'));
        CFileDialog dlg(false, 0, name, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, fmt + "|" + fmt + "|All Files(*.*)|*.*||");
        if (dlg.DoModal() == IDOK) {
            pDlg->fileUser = msg.fromUser;
            pDlg->pSock->SendMSG(msg.join("0", TYPE[AskFileData], "", "", pDlg->fileUser), 0);
            pDlg->rf.init(dlg.GetPathName(), fileSize, fileMD5);
            pDlg->SetTimer(2, pDlg->fileTimeOut, 0);
            pDlg->modifyStatus("׼�������ļ���");
        } else
            pDlg->pSock->SendMSG(msg.join(TYPE[File_NO], TYPE[AskFileData], "", "", msg.fromUser), 0);
    } else
        pDlg->pSock->SendMSG(msg.join(TYPE[File_NO], TYPE[AskFileData], "", "", msg.fromUser), 0);
}

CString getLastErrorStr()
{
    LPVOID lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
                  | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
                  , 0, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
                  , (LPTSTR)&lpMsgBuf, 0, 0);
    CString errStr = (LPCTSTR)lpMsgBuf;
    LocalFree(lpMsgBuf);
    return errStr;
}
