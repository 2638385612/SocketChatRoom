#pragma once
#include "afxsock.h"
	//���ݻ�������С
#define DATA_BUF_SIZE		(8*1024)
	//�ļ������ÿ��������ֽ���
#define PACKAGE_SIZE		(512)
	//������ݰ���Ŀ
#define MAX_PACKAGE_NUM		(16*1024)

#include "../../mfcClient1/mfcClient1/RecvFile.hpp"
#include "../../mfcClient1/mfcClient1/CXXFStream.hpp"


#include "mfcServer1Dlg.h"		//���Ի���ͷ�ļ�
class CmfcServer1Dlg; 
class CServerSocket : public CSocket
{
public:
	CmfcServer1Dlg* m_pDlg;
	CServerSocket(void);
	~CServerSocket(void);
	virtual void OnAccept(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
};

