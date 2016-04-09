#ifndef _MYMSG_H_
#define _MYMSG_H_

#include "afxstr.h"

extern const CString TYPE[30];  //��Ϣ����
extern const char seperator[3]; //��Ϣ�����ֶεķָ���

//������Ϣ�����ͷ�װ���࣬����Ϊstruct���������ݳ�Ա������
struct MyMsg
{
    CString userId;     //�û���
    CString pw;         //����
    CString type;       //��Ϣ����
    CString fromUser;   //��Ϣ����
    CString toUser;     //��Ϣȥ��
    CString data;       //��Ϣ����

    explicit MyMsg(CString str = "");
    //������Ϣ����������Ϣ��OLMsg����Ƿ���������Ϣ
    CString load(CString str, bool OLMsg = 0);
    //������Ϣ�������֣�����װ��Ϣ
    CString join(CString _data = "", CString _type = "", CString _user = "", CString _from = "", CString _to = "", CString _pw = "") const;
    //����ȥ��strǰn���ַ�֮����ұ�ʣ����Ӵ�
    static CString rightN(const CString &str, int n) {
        return str.GetLength() > n ? str.Right(str.GetLength() - n) : "";
    }
};

#endif // !_MYMSG_H_
