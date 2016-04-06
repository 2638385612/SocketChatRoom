#pragma once
#ifndef _SERVERSOCKET_H_
#define _SERVERSOCKET_H_

#include <afxsock.h>

#define DATA_BUF_SIZE   (32*1024) //���ݻ�������С
#define PACKAGE_SIZE    (512)     //�ļ������ÿ��������ֽ���
#define MAX_PACKAGE_NUM (16*1024) //������ݰ���Ŀ

#define MBox(s) MessageBox(s,"��ܰ��ʾ")
#define MBox2(s1,s2) MessageBox(s1,s2)
#define elif else if
#define FOR(ii,start,end) for(ii=start;ii<end;++ii)

extern CString TYPE[30];
extern CString STR[5];      //������Ϣ�ĸ�������

//������Ϣ�����ͷ�װ���࣬����Ϊstruct���������ݳ�Ա������
struct MyMsg
{
    CString userId;     //�û���
    CString pw;         //����
    CString type;       //��Ϣ����
    CString fromUser;   //��Ϣ����
    CString toUser;     //��Ϣȥ��
    CString data;       //��Ϣ����

    explicit MyMsg(const CString str = "") {
        if (str != "")
            load(str);
    }
    //������Ϣ����������Ϣ��OLMsg����Ƿ���������Ϣ
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
    //������Ϣ�������֣�����װ��Ϣ
    const CString join(CString _data = "", CString _type = "", CString _user = "", CString _from = "", CString _to = "", CString _pw = "") const {
        if (_user == "")
            _user = userId;
        //�û���+����+����+ȥ��+����+����
        return _user + STR[0] + _pw + STR[1] + _from + STR[2] + _to + STR[3] + _type + STR[4] + _data;
    }
    //����ȥ��strǰn���ַ�֮����ұ�ʣ����Ӵ�
    static CString rightN(CString str, int n) {
        return str.Right(str.GetLength() - n);
    }
};

class CmfcServer1Dlg; //������,��Ϊ���涨����һ��������ָ��
class CServerSocket : public CSocket
{
public:
    CServerSocket(CmfcServer1Dlg* m_pDlg);

    virtual void OnAccept(int nErrorCode);
    virtual void OnReceive(int nErrorCode);
public:
    CmfcServer1Dlg* m_pDlg;
};

#endif //_SERVERSOCKET_H_
