#include "MyTcpListener.h"
#include <iostream>
#include <string>


MyTcpListener* MyTcpListener::mServerListener=nullptr;
QMutex MyTcpListener::mMutex;


MyTcpListener::MyTcpListener()
    :mServer(this)//Ҫ�������Ч ������봫������߶���
    ,m_pTcpServer(nullptr)
{

}


MyTcpListener::~MyTcpListener()
{

}

MyTcpListener *MyTcpListener::getInstance()
{
    MyTcpListener* tmp = mServerListener;
    if (tmp == nullptr)
    {
        QMutexLocker locker(&mMutex);
        tmp = mServerListener;
        if (tmp == nullptr)
        {
            tmp = new MyTcpListener();
            mServerListener = tmp;
        }
    }
    return mServerListener;
}

bool MyTcpListener::startServer()
{
    //mServer ->SetSocketBufferSize(2048);
    std::cout <<"Start server";

    return mServer->Start((LPCTSTR)"0,0,0,0", 8088);
}

bool MyTcpListener::stopServer()
{
    std::cout <<"Stop server";
    return mServer->Stop();
}

void MyTcpListener::registerView(CListenerImpl *view)
{
    m_pTcpServer = view;
}

void MyTcpListener::unRegisterView()
{
    m_pTcpServer=nullptr;
}

EnHandleResult MyTcpListener::OnPrepareListen(ITcpServer *pSender, SOCKET soListen)
{
    //�ص����� ע�ⲻ��ֱ�Ӹ�UI Qt��������źŲ�
    if(m_pTcpServer!=nullptr)
    {
        m_pTcpServer->OnPrepareListen(pSender, soListen);
    }

    //��ȡ������ip port��Ϣ
    TCHAR lpszAddress[30];
    int iAddressLen;
    USHORT unPort;
    pSender->GetListenAddress(lpszAddress, iAddressLen, unPort);

    return HR_OK;
}

EnHandleResult MyTcpListener::OnAccept(ITcpServer *pSender, CONNID dwConnID, UINT_PTR soClient)
{
    //�ص����� ע�ⲻ��ֱ�Ӹ�UI Qt��������źŲ�
    if(m_pTcpServer!=nullptr)
    {
        m_pTcpServer->OnAccept(pSender, dwConnID, soClient);
    }

    std::cout <<"Accept one.";

    //Ϊÿһ���½�������� ����һ������ ��Ϊ�����Ǹ���ID���������ӵ�
    pSender->SetConnectionExtra(dwConnID,MySocketBuffer::newBuffer());


    return HR_OK;
}

EnHandleResult MyTcpListener::OnHandShake(ITcpServer *pSender, CONNID dwConnID)
{
    //�ص����� ע�ⲻ��ֱ�Ӹ�UI Qt��������źŲ�
    if(m_pTcpServer!=nullptr)
    {
        m_pTcpServer->OnHandShake(pSender, dwConnID);
    }

    return HR_OK;
}

EnHandleResult MyTcpListener::OnReceive(ITcpServer *pSender, CONNID dwConnID, int iLength)
{
    //�ص����� ע�ⲻ��ֱ�Ӹ�UI Qt��������źŲ�
    if(m_pTcpServer!=nullptr)
    {
        m_pTcpServer->OnReceive(pSender, dwConnID, iLength);
    }

    //����Ϳ��Բ���Acceptʱ���ӵĶ������������綨�巽�� ���ݻ����
    MySocketBuffer *pSocketBuffer=nullptr;
    pSender->GetConnectionExtra(dwConnID, (PVOID*)pSocketBuffer);

    if(pSocketBuffer != nullptr)
    {
        //���ڴ˶�pSocketBuffer������
        std::string recv = (char *)pSocketBuffer;
        std::cout<<"OnReceive123: ";
        std::cout<<recv;
    }

    return HR_OK;
}

EnHandleResult MyTcpListener::OnReceive(ITcpServer *pSender, CONNID dwConnID, const unsigned char *pData, int iLength)
{
    //�ر�ע������ص������ķ�ʽ   ��ͬһ��ID����ʱ�ǰ���˳�򴥷�
    //����ǲ�ͬID���������̴߳��� ���������ﲻ������������
    //�ر��ǲ��ܼ�����ͬ������
    //�������ٴ������� ������̳߳ط�ʽ

    //�ص����� ע�ⲻ��ֱ�Ӹ�UI Qt��������źŲ�
    if(m_pTcpServer!=nullptr)
    {
        m_pTcpServer->OnReceive(pSender, dwConnID, pData, iLength);
    }

    //����Ϳ��Բ���Acceptʱ���ӵĶ������������綨�巽�� ���ݻ����
    MySocketBuffer *pSocketBuffer=nullptr;
    pSender->GetConnectionExtra(dwConnID, (PVOID*)pSocketBuffer);

    if(pSocketBuffer != nullptr)
    {
        //���ڴ˶�pSocketBuffer������
        std::string recv = (char *)pSocketBuffer;
        std::cout<<"OnReceive001: ";
        std::cout<<recv;
    }

    return HR_OK;
}



EnHandleResult MyTcpListener::OnSend(ITcpServer *pSender, CONNID dwConnID, const unsigned char *pData, int iLength)
{
    //�ص����� ע�ⲻ��ֱ�Ӹ�UI Qt��������źŲ�
    if(m_pTcpServer!=nullptr)
    {
        m_pTcpServer->OnSend(pSender, dwConnID, pData, iLength);
        std::cout<<"Tcp send: "<<pData;
    }


    return HR_OK;
}

EnHandleResult MyTcpListener::OnShutdown(ITcpServer *pSender)
{
    //�ص����� ע�ⲻ��ֱ�Ӹ�UI Qt��������źŲ�
    if(m_pTcpServer!=nullptr)
    {
        m_pTcpServer->OnShutdown(pSender);
        std::cout<<"Tcp Shutdown!";
    }


    return HR_OK;
}

EnHandleResult MyTcpListener::OnClose(ITcpServer *pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
    //�ص����� ע�ⲻ��ֱ�Ӹ�UI Qt��������źŲ�
    if(m_pTcpServer!=nullptr)
    {
        m_pTcpServer->OnClose(pSender, dwConnID, enOperation, iErrorCode);
    }

    //��һ�����ӹر�ʱ ������Ҫ�Լ���onAccept ʱ new�����Ķ���delete��
    MySocketBuffer *pSocketBuffer=nullptr;
    pSender->GetConnectionExtra(dwConnID, (PVOID*)pSocketBuffer);
    if(pSocketBuffer!=nullptr)
    {
        delete pSocketBuffer;
    }

    return HR_OK;
}





