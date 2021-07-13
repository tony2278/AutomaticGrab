#ifndef MYSOCKETLISTENER_H
#define MYSOCKETLISTENER_H

#include "hpsocket/HPSocket.h"
#include "CListenerImpl.h"
#include <QMutex>
#include <QMutexLocker>



class MySocketBuffer
{
public:
    static MySocketBuffer* newBuffer(){ return new MySocketBuffer();}
    MySocketBuffer(){}
};



class MyTcpListener:CTcpServerListener
{

public:
    static MyTcpListener* getInstance();
    CTcpPackServerPtr mServer;

    //1.开启服务器
    bool startServer();

    //2.关闭服务器
    bool stopServer();


    void registerView(CListenerImpl* view);
    void unRegisterView();
public:
    //监听成功时触发
    EnHandleResult OnPrepareListen(ITcpServer *pSender, SOCKET soListen);

    //接受到连接时触发
    EnHandleResult OnAccept(ITcpServer *pSender, CONNID dwConnID, UINT_PTR soClient);

    //握手成功时触发
    EnHandleResult OnHandShake(ITcpServer *pSender, CONNID dwConnID);

    //收到数据时触发
    EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength);
    EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, const unsigned char* pData, int iLength);

    //发送数据成功触发
    EnHandleResult OnSend(ITcpServer *pSender, CONNID dwConnID, const unsigned char *pData, int iLength);

    //服务器关闭时触发
    EnHandleResult OnShutdown(ITcpServer *pSender);

    //关闭某个连接时触发
    EnHandleResult OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);

private:
    CListenerImpl *m_pTcpServer;

private:
    //单例实现
    MyTcpListener();
    MyTcpListener(const MyTcpListener&);
    MyTcpListener& operator=(const MyTcpListener&);
    virtual ~MyTcpListener();
    class CGarbo                    //
    {
    public:
        CGarbo(){}
        ~CGarbo()
        {
            if (MyTcpListener::mServerListener)
            {
                delete MyTcpListener::mServerListener;
            }
        }
    };
    static CGarbo mCGarbo;

private:
    static MyTcpListener* mServerListener;
    static QMutex mMutex;
};


#endif // MYSOCKETLISTENER_H
