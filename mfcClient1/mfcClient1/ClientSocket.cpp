#include "stdafx.h"
#include "ClientSocket.h"
#include "mfcClient1.h"
#include "mfcClient1Dlg.h"

CClientSocket::CClientSocket(const CString &_user)
{
    pDlg = (CmfcClient1Dlg*)theApp.GetMainWnd();
}

//	˵�������ͻ��˽��յ��������˷�������ʱ����Ӧ���պ���OnReceive
void CClientSocket::OnReceive(int nErrorCode)
{
    pDlg->receData();
    CSocket::OnReceive(nErrorCode);
}

CString CClientSocket::getLastErrorStr()
{
    LPVOID lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
        | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
        , 0, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
        , (LPTSTR)&lpMsgBuf, 0, 0);
    CString errStr = (LPCTSTR)lpMsgBuf;
    LocalFree(lpMsgBuf);
    return errStr;
}
