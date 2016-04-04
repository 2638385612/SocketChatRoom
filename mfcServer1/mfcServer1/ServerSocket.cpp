#include "stdafx.h"
#include "resource.h"
#include "ServerSocket.h"

CServerSocket::CServerSocket(const CmfcServer1Dlg* pDlg) : m_pDlg(pDlg)
{
}

//��Ӧһ���µ���������
void CServerSocket::OnAccept(int nErrorCode)
{
    m_pDlg->AddClient();//ת����������غ�������
    CSocket::OnAccept(nErrorCode);
}

//��Ӧһ���ر�����
void CServerSocket::OnClose(int nErrorCode)
{
    CSocket::OnClose(nErrorCode);
}

//��Ӧ���յ��µ���Ϣ������
void CServerSocket::OnReceive(int nErrorCode)
{
    m_pDlg->ReceData(this);//ת����������غ�������
    CSocket::OnReceive(nErrorCode);
}
