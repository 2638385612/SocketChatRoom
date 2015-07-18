#pragma once
#include "afxsock.h"

#define MBox(s) MessageBox(s,"��ܰ��ʾ")
#define MBox2(s1,s2) MessageBox(s1,s2)
#define elif else if
#define FOR(ii,start,end) for(ii=start;ii<end;++ii)

extern CString TYPE[30];	//��Ϣ���͵Ķ���
extern CString STR[5];		//������Ϣ�ĸ�������

inline CString rightN(CString str,int n)
{
	return str.Right(str.GetLength()-n);
}

	//���ݻ�������С
#define DATA_BUF_SIZE	(8*1024)
	//�ļ������ÿ��������ֽ���
#define PACKAGE_SIZE	(512)
	//������ݰ���Ŀ
#define MAX_PACKAGE_NUM	(16*1024)
class CmfcClient1Dlg;
class CClientSocket : public CSocket
{
public:
	struct MyMsg
	{
		CString userId;		//�û���
		CString pw;			//����
		CString type;		//��Ϣ����
		CString fromUser;	//��Ϣ����
		CString toUser;		//��Ϣȥ��
		CString data;		//��Ϣ����
		CString load(CString str,bool OLMsg=0)
		{
			CString tempStr[6] = {""};
			int index=0, i;
			FOR(index,0,5)
			{
				i = str.Find(STR[index]);
				tempStr[index] = str.Left(i);
				str = rightN(str,i+3);
				if(str == "")
					break;
			}
			tempStr[5] = str;
			i = str.Find(STR[0]);
			if(i!=-1 && OLMsg)
			{
				tempStr[5] = str.Left(i-1);
				str = rightN(str,i-1);
			}
			index=0;
			userId = tempStr[index++];
			pw = tempStr[index++];
			fromUser = tempStr[index++];
			toUser = tempStr[index++];
			type = tempStr[index++];
			data = tempStr[index++];
			return str;
		}
		const CString join(CString _data="",CString _type="",CString _user="",CString _from="",CString _to="",CString _pw="") const
		{
			if(_user=="")
				_user = userID;//userId;
			//�û���+����+����+ȥ��+����+����
			return _user+STR[0] + _pw+STR[1] + _from+STR[2] + _to+STR[3] +_type +STR[4] + _data;
		}
	};
public:	
	MyMsg mymsg;
	//CString user, msg;	//��ʱ�û�������Ϣ����
	static CString userID;	//���û����û���
	//CString fromUser;	//�Ӹ��û���������Ϣ
	LONG	IDLen;		//ID�ĳ���
	int errorCode;		//������

	CmfcClient1Dlg* pDlg;
	HWND hWnd;
	CClientSocket(CString _user);
	~CClientSocket();
	virtual void OnReceive(int nErrorCode);// ��д���պ�����ͨ����������
	void SendMSG(CString send,bool upEvent=1);// ���ͺ��������ڷ������ݸ�������
	int getError();	//�õ����һ���������
	void updateEvent(CString showMsg, CString from="������:",bool reset = 0,int timeFMT = 2);
	void fileSend(MyMsg& msg);	//���յ������ļ�������ʱ
};

#include "CXXFStream.hpp"

#include "RecvFile.hpp"
