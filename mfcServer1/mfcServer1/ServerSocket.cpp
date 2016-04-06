#include "stdafx.h"
#include "ServerSocket.h"
#include "mfcServer1Dlg.h"

CServerSocket::CServerSocket(CmfcServer1Dlg* pDlg) : m_pDlg(pDlg)
{
}

//��Ӧһ���µ���������
void CServerSocket::OnAccept(int nErrorCode)
{
    m_pDlg->addClient();//ת����������غ�������
    CSocket::OnAccept(nErrorCode);
}

//��Ӧ���յ��µ���Ϣ������
void CServerSocket::OnReceive(int nErrorCode)
{
    m_pDlg->receData(this);//ת����������غ�������
    CSocket::OnReceive(nErrorCode);
}
