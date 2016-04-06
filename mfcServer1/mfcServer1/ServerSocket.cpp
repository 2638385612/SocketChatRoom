#include "stdafx.h"
#include "ServerSocket.h"
#include "mfcServer1Dlg.h"

CString TYPE[30] = { TYPE_ChatMsg , TYPE_Server_is_closed , TYPE_UserList , TYPE_OnlineState , TYPE_FileSend , TYPE_FileData , TYPE_AskFileData , TYPE_File_NO , TYPE_File_Over , TYPE_File_Fail , TYPE_LoginFail , TYPE_UserIsOnline , TYPE_OfflineMsg , TYPE_AllUser , TYPE_AddUserList , TYPE_I_am_online , TYPE_Logout , TYPE_Login , TYPE_Register , TYPE_Status };
//�û���+����+����+ȥ��+����+����
CString STR[5] = { "@@@","<<<",">>>","&&&","###" };

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
