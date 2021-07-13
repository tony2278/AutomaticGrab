#include "MyTcpListener.h"
#include <iostream>
#include <string>


MyTcpListener* MyTcpListener::mServerListener=nullptr;
QMutex MyTcpListener::mMutex;


MyTcpListener::MyTcpListener()
    :mServer(this)//要想监听生效 这个必须传入监听者对象
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
    //回调触发 注意不能直接改UI Qt建议采用信号槽
    if(m_pTcpServer!=nullptr)
    {
        m_pTcpServer->OnPrepareListen(pSender, soListen);
    }

    //获取监听的ip port信息
    TCHAR lpszAddress[30];
    int iAddressLen;
    USHORT unPort;
    pSender->GetListenAddress(lpszAddress, iAddressLen, unPort);

    return HR_OK;
}

EnHandleResult MyTcpListener::OnAccept(ITcpServer *pSender, CONNID dwConnID, UINT_PTR soClient)
{
    //回调触发 注意不能直接改UI Qt建议采用信号槽
    if(m_pTcpServer!=nullptr)
    {
        m_pTcpServer->OnAccept(pSender, dwConnID, soClient);
    }

    std::cout <<"Accept one.";

    //为每一个新接入的连接 附加一个对象 因为我们是根据ID来区分连接的
    pSender->SetConnectionExtra(dwConnID,MySocketBuffer::newBuffer());


    return HR_OK;
}

EnHandleResult MyTcpListener::OnHandShake(ITcpServer *pSender, CONNID dwConnID)
{
    //回调触发 注意不能直接改UI Qt建议采用信号槽
    if(m_pTcpServer!=nullptr)
    {
        m_pTcpServer->OnHandShake(pSender, dwConnID);
    }

    return HR_OK;
}

EnHandleResult MyTcpListener::OnReceive(ITcpServer *pSender, CONNID dwConnID, int iLength)
{
    //回调触发 注意不能直接改UI Qt建议采用信号槽
    if(m_pTcpServer!=nullptr)
    {
        m_pTcpServer->OnReceive(pSender, dwConnID, iLength);
    }

    //这里就可以采用Accept时附加的对象做处理，比如定义方法 数据缓存等
    MySocketBuffer *pSocketBuffer=nullptr;
    pSender->GetConnectionExtra(dwConnID, (PVOID*)pSocketBuffer);

    if(pSocketBuffer != nullptr)
    {
        //可在此对pSocketBuffer做操作
        std::string recv = (char *)pSocketBuffer;
        std::cout<<"OnReceive123: ";
        std::cout<<recv;
    }

    return HR_OK;
}

EnHandleResult MyTcpListener::OnReceive(ITcpServer *pSender, CONNID dwConnID, const unsigned char *pData, int iLength)
{
    //特别注意这个回调触发的方式   当同一个ID触发时是按照顺序触发
    //如果是不同ID触发则是线程触发 所以在这里不能做阻塞处理
    //特别是不能加锁做同步处理
    //如果想高速处理数据 请采用线程池方式

    //回调触发 注意不能直接改UI Qt建议采用信号槽
    if(m_pTcpServer!=nullptr)
    {
        m_pTcpServer->OnReceive(pSender, dwConnID, pData, iLength);
    }

    //这里就可以采用Accept时附加的对象做处理，比如定义方法 数据缓存等
    MySocketBuffer *pSocketBuffer=nullptr;
    pSender->GetConnectionExtra(dwConnID, (PVOID*)pSocketBuffer);

    if(pSocketBuffer != nullptr)
    {
        //可在此对pSocketBuffer做操作
        std::string recv = (char *)pSocketBuffer;
        std::cout<<"OnReceive001: ";
        std::cout<<recv;
    }

    return HR_OK;
}



EnHandleResult MyTcpListener::OnSend(ITcpServer *pSender, CONNID dwConnID, const unsigned char *pData, int iLength)
{
    //回调触发 注意不能直接改UI Qt建议采用信号槽
    if(m_pTcpServer!=nullptr)
    {
        m_pTcpServer->OnSend(pSender, dwConnID, pData, iLength);
        std::cout<<"Tcp send: "<<pData;
    }


    return HR_OK;
}

EnHandleResult MyTcpListener::OnShutdown(ITcpServer *pSender)
{
    //回调触发 注意不能直接改UI Qt建议采用信号槽
    if(m_pTcpServer!=nullptr)
    {
        m_pTcpServer->OnShutdown(pSender);
        std::cout<<"Tcp Shutdown!";
    }


    return HR_OK;
}

EnHandleResult MyTcpListener::OnClose(ITcpServer *pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
    //回调触发 注意不能直接改UI Qt建议采用信号槽
    if(m_pTcpServer!=nullptr)
    {
        m_pTcpServer->OnClose(pSender, dwConnID, enOperation, iErrorCode);
    }

    //当一个连接关闭时 我们需要自己将onAccept 时 new出来的对象delete掉
    MySocketBuffer *pSocketBuffer=nullptr;
    pSender->GetConnectionExtra(dwConnID, (PVOID*)pSocketBuffer);
    if(pSocketBuffer!=nullptr)
    {
        delete pSocketBuffer;
    }

    return HR_OK;
}





