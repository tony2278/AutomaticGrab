#ifndef CLISTENERIMPL_H
#define CLISTENERIMPL_H

#include "hpsocket/HPSocket.h"
#include "Global/helper.h"
#include <string>



class IPullClientImpl : public IPullClient
{
public:
    /*
    * ���ƣ�ץȡ����
    * �������û�ͨ���÷����� Socket �����ץȡ����
    *
    * ������		pData		-- ץȡ������
    *			iLength		-- ץȡ���ݳ���
    * ����ֵ��	EnFetchResult
    */
    EnFetchResult Fetch	(unsigned char* pData, int iLength);

    std::string Recv() {return recv;}

    /*
    * ���ƣ���̽���ݣ������Ƴ����������ݣ�
    * �������û�ͨ���÷����� Socket ����п�̽����
    *
    * ������		pData		-- ��̽������
    *			iLength		-- ��̽���ݳ���
    * ����ֵ��	EnFetchResult
    */
    EnFetchResult Peek	(BYTE* pData, int iLength) {return FR_OK;}

    virtual ~IPullClientImpl() {}

private:
    std::string recv;
};



/* Listener Class */
class CListenerImpl : public CTcpPullServerListener  //CTcpPullServerListener  //CTcpServerListener
{

public:
    // 5. process network events
    virtual EnHandleResult OnPrepareListen(ITcpServer* pSender, SOCKET soListen);
    virtual EnHandleResult OnAccept(ITcpServer* pSender, CONNID dwConnID, UINT_PTR soClient);
    virtual EnHandleResult OnHandShake(ITcpServer* pSender, CONNID dwConnID);
    virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength);
    virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, const unsigned char* pData, int iLength);
    virtual EnHandleResult OnSend(ITcpServer* pSender, CONNID dwConnID, const unsigned char* pData, int iLength);
    virtual EnHandleResult OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
    virtual EnHandleResult OnShutdown(ITcpServer* pSender);

    TPkgInfo* FindPkgInfo(ITcpServer* pSender, CONNID dwConnID);
    void RemovePkgInfo(ITcpServer* pSender, CONNID dwConnID);

public:

   //IPullClientImpl m_TCPPull;
};



#endif // CLISTENERIMPL_H
