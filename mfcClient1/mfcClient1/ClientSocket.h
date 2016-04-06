#pragma once
#ifndef _CLIENTSOCKET_H_
#define _CLIENTSOCKET_H_

#include <afxsock.h>

#define DATA_BUF_SIZE   (32*1024) //���ݻ�������С
#define PACKAGE_SIZE    (512)     //�ļ������ÿ��������ֽ���
#define MAX_PACKAGE_NUM (16*1024) //������ݰ���Ŀ

//������һЩ����ini�����ļ��ĺ����
#define GETS(y,z) AfxGetApp()->GetProfileString("ClientSetting",y,z)
#define WRITE(x,y,z) { char *s = x;if(x==0) s="ClientSetting"; AfxGetApp()->WriteProfileString(s,y,z);}
//x��������ͱ�����y���룬z��ֵ��Ĭ��ֵ
#define GET_WRITE(x,y,z) {\
	if((x=atoi(GETS(y,"-1")))==-1){\
		x = atoi(z);\
		WRITE(0,y,z);\
	}\
}

class CmfcClient1Dlg;
class CClientSocket : public CSocket
{
public:
    CClientSocket(const CString &_user);
    //��д���պ�����ͨ����������
    virtual void OnReceive(int nErrorCode);
    //��ȡ��һ��socket������ʾ���ַ���ֵ
    static CString getLastErrorStr();
public:
    CmfcClient1Dlg* pDlg;
};

#endif //_CLIENTSOCKET_H_
