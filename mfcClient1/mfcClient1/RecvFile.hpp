#ifndef _RECVFILE_HPP_
#define _RECVFILE_HPP_

#include <fstream>
#include <afxstr.h> //CString

extern char packageData[MAX_PACKAGE_NUM][2 * PACKAGE_SIZE + 1];

class RecvFile
{
    bool recving;
    CString fileNewName;
    long packageNum;    //���ݰ�����Ŀ
    long packageRecv;   //�ѽ��յ����ݰ���Ŀ
    long fileLength;    //�ļ���С
    CString fileMD5;    //�ļ���md5ֵ
    unsigned timeStart; //��ʼ�ļ������ʱ��
    CString timeMsg;    //�ķ�ʱ����ʾ
    CString pack_rec;
    bool success;       //���ճɹ�
public:
    RecvFile() {
        packageRecv = 0;
        recving = 0;
    }
    void init(const CString& name, int size, const CString& md5) {
        timeStart = clock();
        fileNewName = name;
        fileLength = size;
        fileMD5 = md5;
        packageNum = (size + PACKAGE_SIZE - 1) / PACKAGE_SIZE;
        packageRecv = 0;
        clearPackData();
        recving = 1;
        std::ofstream out(fileNewName);
        out.close();
    }
    void clearPackData() {
        memset(packageData, 0, sizeof(packageData));
    }
    void recvEnd(bool clear = 1) {  //clear����Ƿ�������ݰ�����
        if (packageNum > packageRecv) {  //�ļ�����ʧ��
            DeleteFile(fileNewName);
        }
        if(clear)
            clearPackData();
        packageNum = packageRecv = 0;
        recving = 0;
    }
    const bool& isRecving() {
        return recving;
    }
    const CString& getPackRecv() {
        pack_rec.Format("%ld", packageRecv);
        return pack_rec;
    }
    long getPackNum() {
        return packageNum;
    }
    const CString& getFileName() {
        return fileNewName;
    }
    void setPackNum(long num) {
        packageNum = num;
    }
    void addPacketage(const char *data);
private:
    void saveFile(int useTime);
};

#endif //_RECVFILE_HPP_
