
// mfcServer1.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CmfcServer1App:
// �йش����ʵ�֣������ mfcServer1.cpp
//

class CmfcServer1App : public CWinApp
{
public:
    CmfcServer1App();

// ��д
public:
    virtual BOOL InitInstance();

// ʵ��

    DECLARE_MESSAGE_MAP()
};

extern CmfcServer1App theApp;
