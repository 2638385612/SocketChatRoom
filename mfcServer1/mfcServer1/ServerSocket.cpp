#include "stdafx.h"
#include "resource.h"
#include "ServerSocket.h"


CServerSocket::CServerSocket(void)
{
}


CServerSocket::~CServerSocket(void)
{
}


void CServerSocket::OnAccept(int nErrorCode)
{
    m_pDlg->AddClient();    //��������û�
	CSocket::OnAccept(nErrorCode);
}


void CServerSocket::OnClose(int nErrorCode)
{
	CSocket::OnClose(nErrorCode);
}


void CServerSocket::OnReceive(int nErrorCode)
{
	m_pDlg->ReceData(this);     // ��������
	CSocket::OnReceive(nErrorCode);
}
