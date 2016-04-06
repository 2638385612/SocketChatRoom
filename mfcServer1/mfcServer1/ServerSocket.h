#pragma once
#ifndef _SERVERSOCKET_H_
#define _SERVERSOCKET_H_

#include <afxsock.h>

#define DATA_BUF_SIZE   (32*1024) //���ݻ�������С
#define PACKAGE_SIZE    (512)     //�ļ������ÿ��������ֽ���
#define MAX_PACKAGE_NUM (16*1024) //������ݰ���Ŀ

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
