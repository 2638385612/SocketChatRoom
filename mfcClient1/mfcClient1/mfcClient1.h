
// mfcClient1.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CmfcClient1App:
// �йش����ʵ�֣������ mfcClient1.cpp
//

class CmfcClient1App : public CWinApp
{
public:
    CmfcClient1App();

// ��д
public:
    virtual BOOL InitInstance();

// ʵ��

    DECLARE_MESSAGE_MAP()
};

extern CmfcClient1App theApp;
