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

    //1.����������
    bool startServer();

    //2.�رշ�����
    bool stopServer();


    void registerView(CListenerImpl* view);
    void unRegisterView();
public:
    //�����ɹ�ʱ����
    EnHandleResult OnPrepareListen(ITcpServer *pSender, SOCKET soListen);

    //���ܵ�����ʱ����
    EnHandleResult OnAccept(ITcpServer *pSender, CONNID dwConnID, UINT_PTR soClient);

    //���ֳɹ�ʱ����
    EnHandleResult OnHandShake(ITcpServer *pSender, CONNID dwConnID);

    //�յ�����ʱ����
    EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength);
    EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, const unsigned char* pData, int iLength);

    //�������ݳɹ�����
    EnHandleResult OnSend(ITcpServer *pSender, CONNID dwConnID, const unsigned char *pData, int iLength);

    //�������ر�ʱ����
    EnHandleResult OnShutdown(ITcpServer *pSender);

    //�ر�ĳ������ʱ����
    EnHandleResult OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);

private:
    CListenerImpl *m_pTcpServer;

private:
    //����ʵ��
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
