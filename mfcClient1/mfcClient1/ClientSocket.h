#pragma once
#ifndef _CLIENTSOCKET_H_
#define _CLIENTSOCKET_H_

#define DATA_BUF_SIZE   (32*1024) //���ݻ�������С
#define PACKAGE_SIZE    (512)     //�ļ������ÿ��������ֽ���
#define MAX_PACKAGE_NUM (16*1024) //������ݰ���Ŀ

#include <afxsock.h>
#include "CXXFStream.hpp"
#include "RecvFile.hpp"

#define MBox(s) MessageBox(s,"��ܰ��ʾ")
#define MBox2(s1,s2) MessageBox(s1,s2)
#define elif else if
#define FOR(ii,start,end) for(ii=start;ii<end;++ii)

extern CString TYPE[30];    //��Ϣ���͵Ķ���
extern CString STR[5];      //������Ϣ�ĸ�������

struct MyMsg { //����Ϊstruct���������ݳ�Ա������
    CString userId;     //�û���
    CString pw;         //����
    CString type;       //��Ϣ����
    CString fromUser;   //��Ϣ����
    CString toUser;     //��Ϣȥ��
    CString data;       //��Ϣ����

    static CString rightN(CString str, int n) {
        return str.Right(str.GetLength() - n);
    }
    CString load(CString str, bool OLMsg = 0) {
        CString tempStr[6] = { "" };
        int index = 0, i;
        FOR(index, 0, 5) {
            i = str.Find(STR[index]);
            tempStr[index] = str.Left(i);
            str = rightN(str, i + 3);
            if (str == "") {
                break;
            }
        }
        tempStr[5] = str;
        i = str.Find(STR[0]);
        if (i != -1 && OLMsg) {
            tempStr[5] = str.Left(i - 1);
            str = rightN(str, i - 1);
        }
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
        if (_user == "") {
            _user = userId;
        }
        //�û���+����+����+ȥ��+����+����
        return _user + STR[0] + _pw + STR[1] + _from + STR[2] + _to + STR[3] + _type + STR[4] + _data;
    }
};

class CmfcClient1Dlg;
class CClientSocket : public CSocket
{
public:
    CClientSocket(const CString &_user);
    virtual void OnReceive(int nErrorCode);// ��д���պ�����ͨ����������

    void SendMSG(CString send, bool upEvent = 1);// ���ͺ��������ڷ������ݸ�������
    void updateEvent(CString showMsg, CString from = "������:", bool reset = 0, int timeFMT = 2);//������Ϣ���
    void fileSend(MyMsg& msg);  //���յ������ļ�������ʱ
public:
    MyMsg mymsg;
    CString userID; //���û����û���

    HWND hWnd;
    CmfcClient1Dlg* pDlg;
};

CString getLastErrorStr();

#endif //_CLIENTSOCKET_H_
